/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "sdf/ge_sdf_from_image_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"
#include "src/core/SkOpts.h"
#include "ge_mesa_blur_shader_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_SPREAD_FACTOR = 4096;
constexpr int TWO = 2;

const std::string JFA_PREPARE_SHADER = R"(
    uniform shader imageInput;
    uniform float spreadFactor;
    const float SQRT_2 = 1.41421356;

    vec2 getGradient(vec2 fragCoord) {
        const vec2 h = vec2(0.5, 0);
        return vec2(imageInput.eval(fragCoord + h.xy).a - imageInput.eval(fragCoord - h.xy).a,
                imageInput.eval(fragCoord + h.yx).a - imageInput.eval(fragCoord - h.yx).a);
    }
    half4 main(vec2 fragCoord) {
        vec2 centerFragCoord = fragCoord + vec2(0.5);

        float imageSample = imageInput.eval(centerFragCoord).a;

        if (imageSample == 0) {
            return half4(1, 1, 0.5, 0.5);
        }
        if (imageSample == 1) {
            return half4(0.5, 0.5, 1, 1);
        }
        vec2 grad = getGradient(centerFragCoord);
        vec2 normGrad = normalize(grad);
        vec2 edgeCoords = vec2(0.5);

        float borderCoeff = (imageSample - 0.5) * SQRT_2; // determine how far the actual border is from sample value
        edgeCoords -= normGrad * borderCoeff / (2 * spreadFactor);

        if (borderCoeff < 0) {
            return half4(edgeCoords, 0.5, 0.5);
        }
        return half4(0.5, 0.5, edgeCoords);
    }
)";

const std::string JFA_ITERATION_SHADER = R"(
    uniform vec2 iResolution;
    uniform float jfaRadius;
    uniform float spreadFactor;
    uniform shader imageInput;

    vec4 SafeFetch(vec2 fragCoord) {
        if (fragCoord.x < 0 || fragCoord.x > iResolution.x || fragCoord.y < 0 || fragCoord.y > iResolution.y) {
            return vec4(1e6);
        }
        return imageInput.eval(fragCoord);
    }

    vec2 EncodeCoords(vec2 coordsToEncode, vec2 fragCoord) {
        return clamp(((coordsToEncode - fragCoord) / spreadFactor + 1) / 2.0, 0, 1);
    }

    vec2 DecodeCoords(vec2 coordsToDecode, vec2 fragCoord) {
        return (coordsToDecode * 2 - 1) * spreadFactor + fragCoord;
    }

    half4 main(vec2 fragCoord) {
        vec4 O = imageInput.eval(fragCoord);
        O = vec4(DecodeCoords(O.xy, fragCoord), DecodeCoords(O.zw, fragCoord));

        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                vec2 sampleCoord = fragCoord + jfaRadius * vec2(x, y);
                vec4 imgSample = SafeFetch(sampleCoord);
                vec4 a = vec4(DecodeCoords(imgSample.xy, sampleCoord), DecodeCoords(imgSample.zw, sampleCoord));
                if (imgSample.x < 1 && imgSample.y < 1 && imgSample.x > 0 && imgSample.y > 0) {
                    O.xy = length(fragCoord - a.xy) < length(O.xy - fragCoord) ? a.xy : O.xy;
                }
                if (imgSample.z < 1 && imgSample.w < 1 && imgSample.z > 0 && imgSample.w > 0) {
                    O.zw = length(fragCoord - a.zw) < length(O.zw - fragCoord) ? a.zw : O.zw;
                }
            }
        }
        O = vec4(EncodeCoords(O.xy, fragCoord), EncodeCoords(O.zw, fragCoord));
        return O;
    }
)";

const std::string JFA_PROCESS_RESULT_SHADER = R"(
    uniform shader imageInput;

    half4 main(vec2 fragCoord) {
        half4 O = half4(0.0);

        vec4 imageSample = imageInput.eval(fragCoord);
        vec2 xy = imageSample.xy * 2 - 1;
        vec2 zw = imageSample.zw * 2 - 1;

        O.a = length(xy) - length(zw);
        O.a = (O.a + 1.0) / 2.0;
        O.a = clamp(O.a, 0, 1);
        return O;
    }
)";

const std::string SDF_FILL_DERIV_SHADER = R"(
    uniform shader imageInput;
    uniform shader blurredSDFInput;

    const vec2 h = vec2(1, 0);

    float decodeSdf(float sdf) {
        return sdf * 2 - 1; // Real pixel distances
    }

    half4 main(vec2 fragCoord) {
        float sdf0TextureVal = imageInput.eval(fragCoord).a;
        float dx =
            decodeSdf(blurredSDFInput.eval(fragCoord + h.xy).a) - decodeSdf(blurredSDFInput.eval(fragCoord - h.xy).a);
        float dy =
            decodeSdf(blurredSDFInput.eval(fragCoord + h.yx).a) - decodeSdf(blurredSDFInput.eval(fragCoord - h.yx).a);

        half4 O = half4(0);
        O.x = -clamp((dx + 1) / 2.0, 0, 1);
        O.y = -clamp((dy + 1) / 2.0, 0, 1);
        O.w = sdf0TextureVal;
        return O;
    }
)";

std::shared_ptr<Drawing::RuntimeEffect> GetJFAPrepareEffect()
{
    static auto effect = Drawing::RuntimeEffect::CreateForShader(JFA_PREPARE_SHADER);
    return effect;
}

std::shared_ptr<Drawing::RuntimeEffect> GetJfaIterationEffect()
{
    static auto effect = Drawing::RuntimeEffect::CreateForShader(JFA_ITERATION_SHADER);
    return effect;
}

std::shared_ptr<Drawing::RuntimeEffect> GetJfaProcessResultEffect()
{
    static auto effect = Drawing::RuntimeEffect::CreateForShader(JFA_PROCESS_RESULT_SHADER);
    return effect;
}

std::shared_ptr<Drawing::RuntimeEffect> GetSdfFillDerivEffect()
{
    static auto effect = Drawing::RuntimeEffect::CreateForShader(SDF_FILL_DERIV_SHADER);
    return effect;
}
} // namespace

GESDFFromImageFilter::GESDFFromImageFilter(const Drawing::GESDFFromImageFilterParams& params)
    : spreadFactor_(params.spreadFactor), generateDerivs_(params.generateDerivs)
{
    if (spreadFactor_ < 1) {
        GE_LOGD("GESDFFromImageFilter spreadFactor_(%{public}d) should be [1, 4096], changing to 0.", spreadFactor_);
        spreadFactor_ = 1;
    }

    if (spreadFactor_ > MAX_SPREAD_FACTOR) {
        GE_LOGD("GESDFFromImageFilter spreadFactor_(%{public}d) should be [1, 4096], changing to 4096.", spreadFactor_);
        spreadFactor_ = MAX_SPREAD_FACTOR;
    }
}

int GESDFFromImageFilter::GetSpreadFactor() const
{
    return spreadFactor_;
}

static thread_local std::shared_ptr<Drawing::RuntimeEffect> g_sampleShaderEffect = nullptr;

inline static const std::string g_shaderStringSampleFrag = R"(
    uniform shader image;
    uniform float iScale;
    vec4 main(vec2 fragCoord)
    {
        float s = max(iScale, 1.0);
        float2 base = floor(fragCoord / s) * s;
        float2 sampleCoord = base + 0.5 *s;
        return image.eval(sampleCoord);
    }
)";

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::FakeBlur(Drawing::Canvas &canvas,
    const std::shared_ptr<Drawing::Image> edgeImage)
{
    if (!g_sampleShaderEffect) {
        g_sampleShaderEffect = Drawing::RuntimeEffect::CreateForShader(g_shaderStringSampleFrag);
        if (g_sampleShaderEffect == nullptr) {
            GE_LOGE("GEEdgeLightShaderFilter::RuntimeShader g_gaussShaderEffect create failed.");
            return nullptr;
        }
    }
    constexpr float MIN_IMAGE_BLOOM_SIZE = 16.0; // < MIN_IMAGE_BLOOM_SIZE, not bloom, return image.

    // check image width and height in IsInputImageValid;
    auto imageInfo = edgeImage->GetImageInfo();
    float imageHeight = imageInfo.GetHeight();
    float imageWidth = imageInfo.GetWidth();
    if (imageHeight < MIN_IMAGE_BLOOM_SIZE || imageWidth < MIN_IMAGE_BLOOM_SIZE) {
        GE_LOGD("GESDFFromImageFilter::FakeBlur image size is too small to bloom, return edgeImage."
            "H:[%{public}f] W:[%{public}f]", imageHeight, imageWidth);
            return edgeImage;
    }

    auto sampleBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(g_sampleShaderEffect);

    Drawing::Matrix edgeMatrix;
    auto edgeShader = Drawing::ShaderEffect::CreateImageShader(*edgeImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), edgeMatrix);
    if (edgeShader == nullptr) {
        GE_LOGE("GESDFFromImageFilter::FakeBlur create edgeShader failed.");
        return nullptr;
    }
    Drawing::Matrix matrix;
    Drawing::Matrix imageBlurShaderMatrix;

    auto blurImageV = edgeImage;
    auto scaledInfo = imageInfo;
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*blurImageV, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (srcImageShader == nullptr) {
        GE_LOGE("GESDFFromImageFilter::FakeBlur create srcImageShaderV failed.");
        return nullptr;
    }
    sampleBuilder->SetChild("image", srcImageShader);
    sampleBuilder->SetUniform("iScale", 6.0f);

#ifdef RS_ENABLE_GPU
    auto blurImageH = sampleBuilder->MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
#else
    auto blurImageH = sampleBuilder->MakeImage(nullptr, nullptr, scaledInfo, false);
#endif
    if (blurImageH == nullptr) {
        GE_LOGE("GESDFFromImageFilter::FakeBlur blurImageH make image failed.");
        return nullptr;
    }
    return blurImageH;
}

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!IsInputValid(canvas, image, src, dst)) {
        return image;
    }

    const Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);

    std::shared_ptr<Drawing::Image> initOutput = nullptr;
    initOutput = RunJFAPrepareEffect(canvas, image, linear, src, dst, Drawing::ColorType::COLORTYPE_RGBA_F16);
    if (!initOutput) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage Init make image error");
        return image;
    }

    std::shared_ptr<Drawing::Image> iterationsOutput = nullptr;
    iterationsOutput = RunJfaIterationsEffect(canvas, initOutput, nearest, Drawing::ColorType::COLORTYPE_RGBA_F16);
    if (!iterationsOutput) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage Iterations make image error");
        return image;
    }

    std::shared_ptr<Drawing::Image> sdfOutput = nullptr;
    sdfOutput = RunJfaProcessResultEffect(canvas, iterationsOutput, nearest, image->GetColorType());
    if (!sdfOutput) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage ProcessResult make image error");
        return image;
    }

    if (!generateDerivs_) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage generateDerivs_ is nullptr");
        return sdfOutput;
    }

    // Downsample SDF
    auto blurredSdfImage = FakeBlur(canvas, sdfOutput);
    if (!blurredSdfImage) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage FakeBlur make image error");
        return sdfOutput;
    }

    std::shared_ptr<Drawing::Image> derivOutput = nullptr;
    derivOutput = RunFillDerivEffect(canvas, sdfOutput, blurredSdfImage, linear, image->GetColorType());
    if (!derivOutput) {
        GE_LOGE("GESDFFromImageFilter::OnProcessImage fillDeriv make image error");
        return sdfOutput;
    }
    return derivOutput;
}

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::RunJFAPrepareEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::SamplingOptions& samplingOptions,
    const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::ColorType& outputColorType)
{
    auto imageInfo = image->GetImageInfo();
    auto width = image->GetWidth();
    auto height = image->GetHeight();

    auto outputWidth = std::max(static_cast<int>(std::ceil(dst.GetWidth())), width);
    auto outputHeight = std::max(static_cast<int>(std::ceil(dst.GetHeight())), height);
    Drawing::Matrix stretchMatrix = BuildStretchMatrix(src, dst, width, height);
    auto outputImageInfo = Drawing::ImageInfo(
        outputWidth, outputHeight, outputColorType, imageInfo.GetAlphaType(), imageInfo.GetColorSpace());

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, stretchMatrix);
    Drawing::RuntimeShaderBuilder JFAPrepareBuilder(GetJFAPrepareEffect());
    JFAPrepareBuilder.SetChild("imageInput", imageShader);
    JFAPrepareBuilder.SetUniform("spreadFactor", static_cast<float>(spreadFactor_));
#ifdef RS_ENABLE_GPU
    return JFAPrepareBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
    return JFAPrepareBuilder.MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
}

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::RunJfaIterationsEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::SamplingOptions& samplingOptions,
    const Drawing::ColorType& outputColorType)
{
    Drawing::Matrix identityMatrix;

    auto outputImageInfo = image->GetImageInfo();
    outputImageInfo.SetColorType(outputColorType);

    std::shared_ptr<Drawing::Image> input = image;
    std::shared_ptr<Drawing::Image> output = nullptr;

    int jfaIterationCount = static_cast<int>(std::log2(spreadFactor_)) + 1;
    int jfaRadius = spreadFactor_;
    for (int jfaIteration = 0; jfaIteration < jfaIterationCount; ++jfaIteration) {
        Drawing::RuntimeShaderBuilder jfaIterationBuilder(GetJfaIterationEffect());
        auto imageInputShader = Drawing::ShaderEffect::CreateImageShader(
            *input, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, identityMatrix);
        jfaIterationBuilder.SetChild("imageInput", imageInputShader);
        jfaIterationBuilder.SetUniform("iResolution", input->GetWidth(), input->GetHeight());
        jfaIterationBuilder.SetUniform("jfaRadius", static_cast<float>(jfaRadius));
        jfaIterationBuilder.SetUniform("spreadFactor", static_cast<float>(spreadFactor_));
#ifdef RS_ENABLE_GPU
        output = jfaIterationBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
        output = jfaIterationBuilder.MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
        if (!output) {
            GE_LOGE("GESDFFromImageFilter::OnProcessImage Iteration make image error");
            return image;
        }
        jfaRadius = std::max(jfaRadius / TWO, 1);
        std::swap(input, output);
    }
    return input; // last output is swapped here at the end of cycle
}

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::RunJfaProcessResultEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::SamplingOptions& samplingOptions,
    const Drawing::ColorType& outputColorType)
{
    auto outputImageInfo = image->GetImageInfo();
    outputImageInfo.SetColorType(outputColorType);

    Drawing::Matrix identityMatrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, identityMatrix);
    Drawing::RuntimeShaderBuilder jfaProcessResultBuilder(GetJfaProcessResultEffect());
    jfaProcessResultBuilder.SetChild("imageInput", imageShader);
#ifdef RS_ENABLE_GPU
    return jfaProcessResultBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
    return jfaProcessResultBuilder.MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
}

std::shared_ptr<Drawing::Image> GESDFFromImageFilter::RunFillDerivEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const std::shared_ptr<Drawing::Image> blurredSDFImage,
    const Drawing::SamplingOptions& samplingOptions, const Drawing::ColorType& outputColorType)
{
    auto outputImageInfo = image->GetImageInfo();
    outputImageInfo.SetColorType(outputColorType);

    Drawing::Matrix identityMatrix;
    auto imageInputShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, identityMatrix);
    auto blurredSDFInputShader = Drawing::ShaderEffect::CreateImageShader(
        *blurredSDFImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, identityMatrix);
    Drawing::RuntimeShaderBuilder fillDerivBuilder(GetSdfFillDerivEffect());
    fillDerivBuilder.SetChild("imageInput", imageInputShader);
    fillDerivBuilder.SetChild("blurredSDFInput", blurredSDFInputShader);
#ifdef RS_ENABLE_GPU
    return fillDerivBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
    return fillDerivBuilder.MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
}

bool GESDFFromImageFilter::IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image || !GetJFAPrepareEffect() || !GetJfaIterationEffect() || !GetJfaProcessResultEffect() ||
        (!GetSdfFillDerivEffect() && generateDerivs_)) {
        GE_LOGE("GESDFFromImageFilter::IsInputValid invalid shader or image");
        return false;
    }
    return true;
}

Drawing::Matrix GESDFFromImageFilter::BuildStretchMatrix(
    const Drawing::Rect& src, const Drawing::Rect& dst, int imageWidth, int imageHeight) const
{
    Drawing::Matrix matrix;
    float scaleW = static_cast<float>((dst.GetWidth())) / (imageWidth > 0 ? imageWidth : 1);
    float scaleH = static_cast<float>((dst.GetHeight())) / (imageHeight > 0 ? imageHeight : 1);
    matrix.Translate(-src.GetLeft(), -src.GetTop());
    matrix.PostScale(scaleW, scaleH);

    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(dst.GetLeft(), dst.GetTop());
    matrix.PostConcat(translateMatrix);

    return matrix;
}
} // namespace Rosen
} // namespace OHOS
