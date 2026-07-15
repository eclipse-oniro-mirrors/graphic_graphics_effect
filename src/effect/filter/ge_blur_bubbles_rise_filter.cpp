/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ge_blur_bubbles_rise_filter.h"

#include <algorithm>

#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr float BLUR_INTENSITY_SCALE_FACTOR = 10.0f;
struct BlurBubblesRiseProcessContext {
    Drawing::Matrix matrix;
    Drawing::Matrix invertMatrix;
    Drawing::ImageInfo imageInfo;
    float width = 0.0f;
    float height = 0.0f;
};

inline static const std::string g_shaderStringResample = R"(
    uniform shader image;
    uniform half2 srcResolution;
    uniform half2 dstResolution;

    half4 main(float2 fragCoord)
    {
        half2 uv = fragCoord / max(dstResolution, half2(1.0));
        return image.eval(uv * srcResolution);
    }
)";

std::shared_ptr<Drawing::Image> MakeRuntimeImage(Drawing::RuntimeShaderBuilder& builder, Drawing::Canvas& canvas,
    const Drawing::Matrix& matrix, const Drawing::ImageInfo& imageInfo)
{
#ifdef RS_ENABLE_GPU
    return builder.MakeImage(canvas.GetGPUContext().get(), &matrix, imageInfo, false);
#else
    return builder.MakeImage(nullptr, &matrix, imageInfo, false);
#endif
}

std::shared_ptr<Drawing::RuntimeEffect> GetResampleShaderEffect()
{
    thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_resampleShader = nullptr;
    if (g_resampleShader == nullptr) {
        LOGD("GetResampleShaderEffect creating resample shader effect");
        g_resampleShader = GECreateRuntimeEffectForShader(g_shaderStringResample);
        if (g_resampleShader == nullptr) {
            LOGE("GetResampleShaderEffect create resample shader effect failed");
        }
    }
    return g_resampleShader;
}

bool PrepareProcessContext(const std::shared_ptr<Drawing::Image>& image, const Drawing::Matrix& canvasMatrix,
    float tranX, float tranY, BlurBubblesRiseProcessContext& context)
{
    context.matrix = canvasMatrix;
    context.matrix.PostTranslate(-tranX, -tranY);
    if (!context.matrix.Invert(context.invertMatrix)) {
        LOGE("GEBlurBubblesRiseFilter::PrepareProcessContext invert matrix failed");
        return false;
    }

    context.imageInfo = image->GetImageInfo();
    context.width = context.imageInfo.GetWidth();
    context.height = context.imageInfo.GetHeight();
    if (context.width < 1e-6 || context.height < 1e-6) {
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::ShaderEffect> BuildImageShader(const std::shared_ptr<Drawing::Image>& image,
    Drawing::TileMode tileModeX, Drawing::TileMode tileModeY, const BlurBubblesRiseProcessContext& context)
{
    if (image == nullptr) {
        return nullptr;
    }
    return Drawing::ShaderEffect::CreateImageShader(*image, tileModeX, tileModeY,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), context.invertMatrix);
}

struct DownsampleParams {
    int32_t width = 1;
    int32_t height = 1;
    float widthF = 1.0f;
    float heightF = 1.0f;
    float blurIntensity = 0.0f;
    Drawing::ImageInfo imageInfo;
};

DownsampleParams BuildDownsampleParams(const BlurBubblesRiseProcessContext& context, float blurIntensity)
{
    DownsampleParams params;
    params.width = std::max(1, static_cast<int32_t>(context.width * 0.5f));
    params.height = std::max(1, static_cast<int32_t>(context.height * 0.5f));
    params.widthF = static_cast<float>(params.width);
    params.heightF = static_cast<float>(params.height);
    params.blurIntensity = blurIntensity;
    params.imageInfo = Drawing::ImageInfo(params.width, params.height, context.imageInfo.GetColorType(),
        context.imageInfo.GetAlphaType(), context.imageInfo.GetColorSpace());
    return params;
}

std::shared_ptr<Drawing::ShaderEffect> BuildDownsampledShader(Drawing::Canvas& canvas,
    const BlurBubblesRiseProcessContext& context,
    const std::shared_ptr<Drawing::Image>& sourceImage,
    const std::shared_ptr<Drawing::RuntimeEffect>& resampleEffect,
    const DownsampleParams& params)
{
    if (sourceImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildDownsampledShader source image is null");
        return nullptr;
    }

    // Create correct sourceShader for downsampling using unit matrix instead of context.invertMatrix
    Drawing::Matrix unitMatrix;
    auto sourceShader = Drawing::ShaderEffect::CreateImageShader(*sourceImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), unitMatrix);
    if (sourceShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildDownsampledShader source shader create failed");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder downsampleBuilder(resampleEffect);
    downsampleBuilder.SetChild("image", sourceShader);
    downsampleBuilder.SetUniform("srcResolution", context.width, context.height);
    downsampleBuilder.SetUniform("dstResolution", params.widthF, params.heightF);

    // Validate params.imageInfo before using it
    if (params.imageInfo.GetWidth() < 1e-6 || params.imageInfo.GetHeight() < 1e-6) {
        LOGE("GEBlurBubblesRiseFilter::BuildDownsampledShader params.imageInfo is invalid");
        return nullptr;
    }

    // Use unit matrix instead of context.matrix to ensure fragCoord is pixel coordinate
    auto downsampledImage = MakeRuntimeImage(downsampleBuilder, canvas, unitMatrix, params.imageInfo);
    if (downsampledImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildDownsampledShader downsample image build failed");
        return nullptr;
    }

    // Create correct shader for downsampled image using unit matrix
    auto downsampledShader = Drawing::ShaderEffect::CreateImageShader(*downsampledImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), unitMatrix);
    if (downsampledShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildDownsampledShader downsample shader create failed");
        return nullptr;
    }
    return downsampledShader;
}

std::shared_ptr<Drawing::Image> BuildHalfResBlurredImage(Drawing::Canvas& canvas,
    const BlurBubblesRiseProcessContext& context,
    const std::shared_ptr<Drawing::ShaderEffect>& downsampledShader,
    const std::shared_ptr<Drawing::RuntimeEffect>& blurEffect,
    const DownsampleParams& params)
{
    // Validate params.imageInfo before using it
    if (params.imageInfo.GetWidth() < 1e-6 || params.imageInfo.GetHeight() < 1e-6) {
        LOGE("GEBlurBubblesRiseFilter::BuildHalfResBlurredImage params.imageInfo is invalid");
        return nullptr;
    }

    Drawing::Matrix unitMatrix;
    Drawing::RuntimeShaderBuilder blurBuilderX(blurEffect);
    blurBuilderX.SetChild("image", downsampledShader);
    blurBuilderX.SetUniform("iResolution", params.widthF, params.heightF);
    blurBuilderX.SetUniform("blurIntensity", params.blurIntensity * BLUR_INTENSITY_SCALE_FACTOR);
    blurBuilderX.SetUniform("horizontal", 1.0f);
    auto blurredImageX = MakeRuntimeImage(blurBuilderX, canvas, unitMatrix, params.imageInfo);
    if (blurredImageX == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildHalfResBlurredImage blur X image build failed");
        return nullptr;
    }

    // Create shader for blurred image X using unit matrix
    auto blurXShader = Drawing::ShaderEffect::CreateImageShader(*blurredImageX,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), unitMatrix);
    if (blurXShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildHalfResBlurredImage blur X shader create failed");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder blurBuilderY(blurEffect);
    blurBuilderY.SetChild("image", blurXShader);
    blurBuilderY.SetUniform("iResolution", params.widthF, params.heightF);
    blurBuilderY.SetUniform("blurIntensity", params.blurIntensity * BLUR_INTENSITY_SCALE_FACTOR);
    blurBuilderY.SetUniform("horizontal", 0.0f);
    auto blurredImage = MakeRuntimeImage(blurBuilderY, canvas, unitMatrix, params.imageInfo);
    if (blurredImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildHalfResBlurredImage blur Y image build failed");
        return nullptr;
    }
    return blurredImage;
}

std::shared_ptr<Drawing::ShaderEffect> BuildUpsampledBlurredShader(Drawing::Canvas& canvas,
    const BlurBubblesRiseProcessContext& context,
    const std::shared_ptr<Drawing::Image>& downsampledBlurredImage,
    const std::shared_ptr<Drawing::RuntimeEffect>& resampleEffect,
    const DownsampleParams& params)
{
    if (downsampledBlurredImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildUpsampledBlurredShader downsampledBlurredImage is null");
        return nullptr;
    }

    Drawing::Matrix unitMatrix;
    // Create shader for downsampled blurred image using unit matrix
    auto downsampledBlurredShader = Drawing::ShaderEffect::CreateImageShader(*downsampledBlurredImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), unitMatrix);
    if (downsampledBlurredShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildUpsampledBlurredShader downsample blurred shader create failed");
        return nullptr;
    }

    // Validate context.imageInfo before using it
    if (context.imageInfo.GetWidth() < 1e-6 || context.imageInfo.GetHeight() < 1e-6) {
        LOGE("GEBlurBubblesRiseFilter::BuildUpsampledBlurredShader context.imageInfo is invalid");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder upsampleBuilder(resampleEffect);
    upsampleBuilder.SetChild("image", downsampledBlurredShader);
    upsampleBuilder.SetUniform("srcResolution", params.widthF, params.heightF);
    upsampleBuilder.SetUniform("dstResolution", context.width, context.height);
    // Use unit matrix to ensure coordinates are correct during upsampling
    auto upsampledImage = MakeRuntimeImage(upsampleBuilder, canvas, unitMatrix, context.imageInfo);
    if (upsampledImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildUpsampledBlurredShader upsample image build failed");
        return nullptr;
    }

    // Create shader for final upsampled image using context.invertMatrix to match original image space
    auto blurredShader = Drawing::ShaderEffect::CreateImageShader(*upsampledImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), context.invertMatrix);
    if (blurredShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildUpsampledBlurredShader blurred shader create failed");
        return nullptr;
    }
    return blurredShader;
}

std::shared_ptr<Drawing::ShaderEffect> BuildBlurredShader(Drawing::Canvas& canvas,
    const BlurBubblesRiseProcessContext& context,
    const std::shared_ptr<Drawing::Image>& sourceImage,
    const std::shared_ptr<Drawing::RuntimeEffect>& blurEffect, float blurIntensity)
{
    auto resampleEffect = GetResampleShaderEffect();
    if (resampleEffect == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::BuildBlurredShader resample effect create failed");
        return nullptr;
    }
    auto params = BuildDownsampleParams(context, blurIntensity);
    auto downsampledShader = BuildDownsampledShader(canvas, context, sourceImage, resampleEffect, params);
    if (downsampledShader == nullptr) {
        return nullptr;
    }

    auto downsampledBlurredImage = BuildHalfResBlurredImage(canvas, context, downsampledShader, blurEffect, params);
    if (downsampledBlurredImage == nullptr) {
        return nullptr;
    }

    return BuildUpsampledBlurredShader(canvas, context, downsampledBlurredImage, resampleEffect, params);
}

} // namespace

GEBlurBubblesRiseFilter::GEBlurBubblesRiseFilter(const Drawing::GEBlurBubblesRiseFilterParams& params)
    : blurIntensity_(params.blurIntensity), mixStrength_(params.mixStrength), progress_(params.progress),
    maskImage_(params.maskImage)
{}

std::shared_ptr<Drawing::Image> GEBlurBubblesRiseFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& /*src*/, const Drawing::Rect& /*dst*/)
{
    if (image == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage input is invalid");
        return nullptr;
    }

    BlurBubblesRiseProcessContext context;
    if (!PrepareProcessContext(image, canvasInfo_.mat, canvasInfo_.tranX, canvasInfo_.tranY, context)) {
        return image;
    }

    auto sourceShader = BuildImageShader(image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, context);
    if (sourceShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage source shader create failed");
        return image;
    }

    auto blurEffect = GetBlurShaderEffect();
    auto mixEffect = GetMaskMixShaderEffect();
    if (blurEffect == nullptr || mixEffect == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage runtime effect create failed");
        return image;
    }

    CheckBlurBubblesRiseParams();

    auto blurredShader = BuildBlurredShader(canvas, context, image, blurEffect, blurIntensity_);
    if (blurredShader == nullptr) {
        return image;
    }

    auto maskImage = (maskImage_ != nullptr) ? maskImage_ : image;
    auto maskShader = BuildImageShader(maskImage, Drawing::TileMode::MIRROR, Drawing::TileMode::MIRROR, context);
    if (maskShader == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage mask shader create failed");
        return image;
    }

    if (maskImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage mask image is null");
        return image;
    }
    auto maskImageInfo = maskImage->GetImageInfo();
    if (maskImageInfo.GetWidth() < 1e-6 || maskImageInfo.GetHeight() < 1e-6) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage mask image info invalid");
        return image;
    }

    auto maskResolutionX = static_cast<float>(maskImageInfo.GetWidth());
    auto maskResolutionY = static_cast<float>(maskImageInfo.GetHeight());

    Drawing::RuntimeShaderBuilder mixBuilder(mixEffect);
    mixBuilder.SetChild("blur_tex", blurredShader);
    mixBuilder.SetChild("original_tex", sourceShader);
    mixBuilder.SetChild("blur_mask", maskShader);
    mixBuilder.SetUniform("iResolution", context.width, context.height);
    mixBuilder.SetUniform("maskResolution", maskResolutionX, maskResolutionY);
    mixBuilder.SetUniform("mixStrength", mixStrength_);
    mixBuilder.SetUniform("progress", progress_ * timeScale_);

    // Validate context.imageInfo before using it in final output
    if (context.imageInfo.GetWidth() < 1e-6 || context.imageInfo.GetHeight() < 1e-6) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage context.imageInfo is invalid");
        return image;
    }

    auto outputImage = MakeRuntimeImage(mixBuilder, canvas, context.matrix, context.imageInfo);
    if (outputImage == nullptr) {
        LOGE("GEBlurBubblesRiseFilter::OnProcessImage output image build failed");
        return image;
    }

    return outputImage;
}

void GEBlurBubblesRiseFilter::CheckBlurBubblesRiseParams()
{
    blurIntensity_ = std::clamp(blurIntensity_, 0.0f, 1.0f);
    mixStrength_ = std::clamp(mixStrength_, 0.0f, 1.0f);
    progress_ = std::clamp(progress_, 0.0f, 1.0f);
}

std::shared_ptr<Drawing::RuntimeEffect> GEBlurBubblesRiseFilter::GetBlurShaderEffect()
{
    thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_blurShader = nullptr;
    if (g_blurShader == nullptr) {
        LOGD("GEBlurBubblesRiseFilter::GetBlurShaderEffect creating blur shader effect");
        g_blurShader = GECreateRuntimeEffectForShader(shaderStringGaussianBlur);
        if (g_blurShader == nullptr) {
            LOGE("GEBlurBubblesRiseFilter::GetBlurShaderEffect create blur shader effect failed");
        }
    }
    return g_blurShader;
}

std::shared_ptr<Drawing::RuntimeEffect> GEBlurBubblesRiseFilter::GetMaskMixShaderEffect()
{
    thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_maskMixShader = nullptr;
    if (g_maskMixShader == nullptr) {
        LOGD("GEBlurBubblesRiseFilter::GetMaskMixShaderEffect creating mask mix shader effect");
        g_maskMixShader = GECreateRuntimeEffectForShader(shaderStringMaskMix);
        if (g_maskMixShader == nullptr) {
            LOGE("GEBlurBubblesRiseFilter::GetMaskMixShaderEffect create mask mix shader effect failed");
        }
    }
    return g_maskMixShader;
}

} // namespace Rosen
} // namespace OHOS
