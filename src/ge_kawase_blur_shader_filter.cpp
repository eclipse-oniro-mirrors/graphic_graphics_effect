/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ge_kawase_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"
#include "src/core/SkOpts.h"

#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"
#define PROPERTY_BLUR_EXTRA_FILTER "persist.sys.graphic.blurExtraFilter"
#define PROPERTY_KAWASE_ORIGINAL_IMAGE "persist.sys.graphic.kawaseOriginalEnable"

namespace {
constexpr uint32_t BLUR_SAMPLE_COUNT = 5;
constexpr float BASE_BLUR_SCALE = 0.5f;        // base downSample radio
constexpr int32_t MAX_PASSES_LARGE_RADIUS = 7; // Maximum number of render passes
constexpr float DILATED_CONVOLUTION_LARGE_RADIUS = 4.6f;
// To avoid downscaling artifacts, interpolate the blurred fbo with the full composited image, up to this radius
constexpr float MAX_CROSS_FADE_RADIUS = 10.0f;
static std::shared_ptr<Drawing::RuntimeEffect> g_blurEffect;
static std::shared_ptr<Drawing::RuntimeEffect> g_mixEffect;
static std::shared_ptr<Drawing::RuntimeEffect> g_blurEffectAf;
static std::shared_ptr<Drawing::RuntimeEffect> g_simpleFilter;

} // namespace

// Advanced Filter: we can get normalized uv offset from width and height
struct OffsetInfo {
    float offsetX;
    float offsetY;
    int width;
    int height;
};

// Advanced Filter
static bool IsAdvancedFilterUsable()
{
    std::string gpuVersion = GESystemProperties::GetEventProperty(PROPERTY_HIGPU_VERSION);
    // The AF Feature is only enabled on higpu v200 platform
    if (gpuVersion.compare("higpu.v200") != 0) {
        return false;
    }
    // If persist.sys.graphic.supports_af=0
    // we will not use it
    return GESystemProperties::GetBoolSystemProperty(PROPERTY_DEBUG_SUPPORT_AF, false);
}

static bool GetBlurExtraFilterEnabled()
{
    static bool blurExtraFilterEnabled =
        (std::atoi(GESystemProperties::GetEventProperty(PROPERTY_BLUR_EXTRA_FILTER).c_str()) != 0);
    return blurExtraFilterEnabled;
}

static bool GetKawaseOriginalEnabled()
{
#ifdef GE_OHOS
    static bool kawaseOriginalEnabled =
        (std::atoi(GESystemProperties::GetEventProperty(PROPERTY_KAWASE_ORIGINAL_IMAGE).c_str()) != 0);
    return kawaseOriginalEnabled;
#else
    return false;
#endif
}

void GEKawaseBlurShaderFilter::GetNormalizedOffset(
    SkV2* offsets, const uint32_t offsetCount, const OffsetInfo &offsetInfo)
{
    if (offsets == nullptr || offsetCount != BLUR_SAMPLE_COUNT) {
        LOGE("%s: Invalid offsets.", __func__);
        return;
    }
    if (std::fabs(offsetInfo.width) < 1e-6 || std::fabs(offsetInfo.height) < 1e-6) {
        LOGE("%s: Invalid width or height.", __func__);
        return;
    }
    const SkV2 normalizedOffsets[BLUR_SAMPLE_COUNT] = { SkV2 { 0.0f, 0.0f },
        SkV2 { offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height },
        SkV2 { -offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height },
        SkV2 { offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height },
        SkV2 { -offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height } };
    for (uint32_t i = 0; i < BLUR_SAMPLE_COUNT; ++i) {
        offsets[i] = normalizedOffsets[i];
    }
}

static const bool IS_ADVANCED_FILTER_USABLE_CHECK_ONCE = IsAdvancedFilterUsable();

GEKawaseBlurShaderFilter::GEKawaseBlurShaderFilter(const Drawing::GEKawaseBlurShaderFilterParams& params)
    : radius_(params.radius)
{
    params_ = std::make_optional<Drawing::GEKawaseBlurShaderFilterParams>(params);
    type_ = "Blur";
    if (!InitBlurEffect()) {
        LOGE("GEKawaseBlurShaderFilter::GEKawaseBlurShaderFilter failed when initializing BlurEffect.");
        return;
    }
    // Advanced Filter
    if (IS_ADVANCED_FILTER_USABLE_CHECK_ONCE && !InitBlurEffectForAdvancedFilter()) {
        LOGE("GEKawaseBlurShaderFilter::GEKawaseBlurShaderFilter failed when initializing BlurEffectAF.");
        return;
    }

    if (!InitMixEffect()) {
        LOGE("GEKawaseBlurShaderFilter::GEKawaseBlurShaderFilter failed when initializing MixEffect.");
        return;
    }

    if (radius_ < 1) {
        LOGI("GEKawaseBlurShaderFilter radius(%{public}d) should be [1, 8k], ignore blur.", radius_);
        radius_ = 0;
    }

    if (radius_ > 8000) { // 8000 experienced value
        LOGI("GEKawaseBlurShaderFilter radius(%{public}d) should be [1, 8k], change to 8k.", radius_);
        radius_ = 8000; // 8000 experienced value
    }

    if (GetBlurExtraFilterEnabled()) {
        if (!InitSimpleFilter()) {
            LOGE("GEKawaseBlurShaderFilter::GEKawaseBlurShaderFilter failed to construct SimpleFilter");
            return;
        }
    }
}

int GEKawaseBlurShaderFilter::GetRadius() const
{
    return radius_;
}

std::shared_ptr<Drawing::ShaderEffect> GEKawaseBlurShaderFilter::ApplySimpleFilter(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& input, const std::shared_ptr<Drawing::ShaderEffect>& prevShader,
    const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear) const
{
    Drawing::RuntimeShaderBuilder simpleBlurBuilder(g_simpleFilter);
    simpleBlurBuilder.SetChild("imageInput", prevShader);
#ifdef RS_ENABLE_GPU
    std::shared_ptr<Drawing::Image> tmpSimpleBlur(simpleBlurBuilder.MakeImage(
        canvas.GetGPUContext().get(), nullptr, scaledInfo, false));
#else
    std::shared_ptr<Drawing::Image> tmpSimpleBlur(simpleBlurBuilder.MakeImage(nullptr, nullptr, scaledInfo, false));
#endif
    if (tmpSimpleBlur == nullptr) {
        LOGE("GEKawaseBlurShaderFilter::ApplySimpleFilter tmpSimpleBlur is nullptr");
        return nullptr;
    }
    return Drawing::ShaderEffect::CreateImageShader(*tmpSimpleBlur, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        linear, Drawing::Matrix());
}

std::shared_ptr<Drawing::Image> GEKawaseBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!IsInputValid(canvas, image, src, dst)) {
        return image;
    }

    auto input = image;
    CheckInputImage(canvas, image, input, src);
    ComputeRadiusAndScale(radius_);

    float tmpRadius = static_cast<float>(blurRadius_ / DILATED_CONVOLUTION_LARGE_RADIUS);
    int numberOfPasses =
        std::min(MAX_PASSES_LARGE_RADIUS, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num

    float radiusByPasses = tmpRadius / numberOfPasses;

    auto width = std::max(static_cast<int>(std::ceil(dst.GetWidth())), input->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst.GetHeight())), input->GetHeight());
    auto originImageInfo = input->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(width * blurScale_), std::ceil(height * blurScale_),
        originImageInfo.GetColorType(), originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::Matrix blurMatrix = BuildMatrix(src, scaledInfo, input);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    // Advanced Filter: check is AF usable only the first time
    bool isUsingAF = IS_ADVANCED_FILTER_USABLE_CHECK_ONCE && g_blurEffectAf != nullptr;
    auto tmpShader = Drawing::ShaderEffect::CreateImageShader(
        *input, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, blurMatrix);
    Drawing::RuntimeShaderBuilder blurBuilder(isUsingAF ? g_blurEffectAf : g_blurEffect);
    if (GetBlurExtraFilterEnabled() && g_simpleFilter) {
        tmpShader = ApplySimpleFilter(canvas, input, tmpShader, scaledInfo, linear);
    }
    blurBuilder.SetChild("imageInput", tmpShader);

    auto offsetXY = radiusByPasses * blurScale_;
    SetBlurBuilderParam(blurBuilder, offsetXY, scaledInfo, width, height);
#ifdef RS_ENABLE_GPU
    auto tmpBlur(blurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false));
#else
    auto tmpBlur(blurBuilder.MakeImage(nullptr, nullptr, scaledInfo, false));
#endif

    if (!tmpBlur) {
        return image;
    }

    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        auto blurShader = Drawing::ShaderEffect::CreateImageShader(
            *tmpBlur, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, Drawing::Matrix());
        const float stepScale = static_cast<float>(i) * blurScale_;
        blurBuilder.SetChild("imageInput", blurShader);

        // Advanced Filter
        auto offsetXYFilter = radiusByPasses * stepScale;
        SetBlurBuilderParam(blurBuilder, offsetXYFilter, scaledInfo, width, height);
#ifdef RS_ENABLE_GPU
        tmpBlur = blurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
#else
        tmpBlur = blurBuilder.MakeImage(nullptr, nullptr, scaledInfo, false);
#endif
    }

    auto output = ScaleAndAddRandomColor(canvas, input, tmpBlur, src, dst, width, height);
    return output;
}

bool GEKawaseBlurShaderFilter::IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!g_blurEffect || !g_mixEffect || !image) {
        LOGE("GEKawaseBlurShaderFilter::shader error");
        return false;
    }
    if (radius_ <= 0) {
        LOGD("GEKawaseBlurShaderFilter::input invalid radius : %{public}d", radius_);
        OutputOriginalImage(canvas, image, src, dst);
        return false;
    }
    if (GetKawaseOriginalEnabled()) {
        OutputOriginalImage(canvas, image, src, dst);
        return false;
    }
    return true;
}

void GEKawaseBlurShaderFilter::SetBlurBuilderParam(Drawing::RuntimeShaderBuilder& blurBuilder, const float offsetXY,
    const Drawing::ImageInfo& scaledInfo, const int width, const int height)
{
    // Advanced Filter: check is AF usable only the first time
    bool isUsingAF = IS_ADVANCED_FILTER_USABLE_CHECK_ONCE && g_blurEffectAf != nullptr;
    if (isUsingAF) {
        SkV2 offsets[BLUR_SAMPLE_COUNT];
        OffsetInfo offsetInfo = { offsetXY, offsetXY, scaledInfo.GetWidth(), scaledInfo.GetHeight() };
        GetNormalizedOffset(offsets, BLUR_SAMPLE_COUNT, offsetInfo);
        blurBuilder.SetUniform(
            "in_blurOffset", offsetInfo.offsetX, offsetInfo.offsetY, offsetInfo.width, offsetInfo.height);
    } else {
        blurBuilder.SetUniform("in_blurOffset", offsetXY, offsetXY);
        blurBuilder.SetUniform("in_maxSizeXY", width * blurScale_, height * blurScale_);
    }
}

const OHOS::Rosen::Drawing::Matrix GEKawaseBlurShaderFilter::BuildMatrix(
    const Drawing::Rect& src, const Drawing::ImageInfo& scaledInfo, const std::shared_ptr<Drawing::Image>& input)
{
    Drawing::Matrix blurMatrix;
    blurMatrix.Translate(-src.GetLeft(), -src.GetTop());
    int scaleWidth = scaledInfo.GetWidth();
    int width = input->GetWidth();
    float scaleW = static_cast<float>(scaleWidth) / (width > 0 ? width : 1);

    int scaleHeight = scaledInfo.GetHeight();
    int height = input->GetHeight();
    float scaleH = static_cast<float>(scaleHeight) / (height > 0 ? height : 1);
    blurMatrix.PostScale(scaleW, scaleH);
    return blurMatrix;
}

bool GEKawaseBlurShaderFilter::InitBlurEffect()
{
    if (g_blurEffect != nullptr) {
        return true;
    }

    static std::string blurString(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            return half4(c.rgba * 0.2);
        }
    )");
    g_blurEffect = Drawing::RuntimeEffect::CreateForShader(blurString);
    if (g_blurEffect == nullptr) {
        LOGE("GEKawaseBlurShaderFilter::RuntimeShader blurEffect create failed");
        return false;
    }

    return true;
}

bool GEKawaseBlurShaderFilter::InitMixEffect()
{
    if (g_mixEffect != nullptr) {
        return true;
    }

    static std::string mixString(R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;
        uniform float inColorFactor;

        highp float random(float2 xy) {
            float t = dot(xy, float2(78.233, 12.9898));
            return fract(sin(t) * 43758.5453);
        }
        half4 main(float2 xy) {
            highp float noiseGranularity = inColorFactor / 255.0;
            half4 finalColor = mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor);
            float noise  = mix(-noiseGranularity, noiseGranularity, random(xy));
            finalColor.rgb += noise;
            return finalColor;
        }
    )");
    g_mixEffect = Drawing::RuntimeEffect::CreateForShader(mixString);
    if (g_mixEffect == nullptr) {
        LOGE("GEKawaseBlurShaderFilter::RuntimeShader mixEffect create failed");
        return false;
    }

    return true;
}

bool GEKawaseBlurShaderFilter::InitSimpleFilter()
{
    if (g_simpleFilter != nullptr) {
        return true;
    }

    static std::string simpleShader(R"(
        uniform shader imageInput;
        half4 main(float2 xy) {
            return imageInput.eval(xy);
        }
    )");
    g_simpleFilter = Drawing::RuntimeEffect::CreateForShader(simpleShader);
    if (g_simpleFilter == nullptr) {
        LOGE("GEKawaseBlurShaderFilter::RuntimeShader failed to create simple filter");
        return false;
    }

    return true;
}

// Advanced Filter
bool GEKawaseBlurShaderFilter::InitBlurEffectForAdvancedFilter()
{
    if (g_blurEffectAf != nullptr) {
        return true;
    }

    Drawing::RuntimeEffectOptions ops;
    ops.useAF = true;
    static std::string blurStringAF(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset[5];

        half4 main(float2 xy) {
            half4 c = half4(0, 0, 0, 0);
            for (int i = 0; i < 5; ++i) {
                c += imageInput.eval(float2(xy.x + in_blurOffset[i].x, xy.y + in_blurOffset[i].y));
            }
            return half4(c.rgba * 0.2);
        }
    )");
    g_blurEffectAf = Drawing::RuntimeEffect::CreateForShader(blurStringAF, ops);
    if (g_blurEffectAf == nullptr) {
        LOGE("%s: RuntimeShader blurEffectAF create failed", __func__);
        return false;
    }
    
    return true;
}

Drawing::Matrix GEKawaseBlurShaderFilter::GetShaderTransform(
    const Drawing::Canvas* canvas, const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

void GEKawaseBlurShaderFilter::CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Image>& checkedImage, const Drawing::Rect& src) const
{
#ifdef RS_ENABLE_GPU
    auto srcRect = Drawing::RectI(src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom());
    if (image->GetImageInfo().GetBound() != srcRect) {
        auto resizedImage = std::make_shared<Drawing::Image>();
        auto gpuCtx = canvas.GetGPUContext();
        if (gpuCtx == nullptr || !(image->IsValid(gpuCtx.get()))) {
            LOGE("GEKawaseBlurShaderFilter::CheckInputImage invalid image");
            return;
        }
        if (resizedImage->BuildSubset(image, srcRect, *gpuCtx)) {
            checkedImage = resizedImage;
            LOGD("GEKawaseBlurShaderFilter::resize image success");
        } else {
            LOGD("GEKawaseBlurShaderFilter::resize image failed, use original image");
        }
    }
#endif
}

void GEKawaseBlurShaderFilter::OutputOriginalImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto width = image->GetWidth();
    auto height = image->GetHeight();
    if (width == 0 || height == 0) {
        return;
    }

    Drawing::Brush brush;
    Drawing::Matrix inputMatrix;
    float scaleW = dst.GetWidth() / width;
    float scaleH = dst.GetHeight() / height;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(scaleW, scaleH);
    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto inputShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, inputMatrix);
    brush.SetShaderEffect(inputShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GEKawaseBlurShaderFilter::ScaleAndAddRandomColor(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& blurImage,
    const Drawing::Rect& src, const Drawing::Rect& dst, int& width, int& height) const
{
    if (abs(blurScale_) < 1e-6 || blurImage->GetWidth() < 1e-6 || blurImage->GetHeight() < 1e-6 ||
        image->GetWidth() < 1e-6 || image->GetHeight() < 1e-6) {
        LOGE("GEKawaseBlurShaderFilter::blurScale is zero.");
        return blurImage;
    }

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);

    Drawing::RuntimeShaderBuilder mixBuilder(g_mixEffect);
    const auto scaleMatrix = GetShaderTransform(
        &canvas, dst, dst.GetWidth() / blurImage->GetWidth(), dst.GetHeight() / blurImage->GetHeight());
    auto tmpShader = Drawing::ShaderEffect::CreateImageShader(
        *blurImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, scaleMatrix);
    mixBuilder.SetChild("blurredInput", tmpShader);
    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / image->GetWidth(), dst.GetHeight() / image->GetHeight());
    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);
    auto mixShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, inputMatrix);
    mixBuilder.SetChild("originalInput", mixShader);
    float mixFactor = (abs(MAX_CROSS_FADE_RADIUS) <= 1e-6) ? 1.f : (blurRadius_ / MAX_CROSS_FADE_RADIUS);
    mixBuilder.SetUniform("mixFactor", std::min(1.0f, mixFactor));

    static auto factor = 1.75; // 1.75 from experience
    mixBuilder.SetUniform("inColorFactor", factor);
    LOGD("GEKawaseBlurShaderFilter::kawase random color factor : %{public}f", factor);
    auto scaledInfo = Drawing::ImageInfo(width, height, blurImage->GetImageInfo().GetColorType(),
        blurImage->GetImageInfo().GetAlphaType(), blurImage->GetImageInfo().GetColorSpace());

#ifdef RS_ENABLE_GPU
    auto output = mixBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
#else
    auto output = mixBuilder.MakeImage(nullptr, nullptr, scaledInfo, false);
#endif
    return output;
}

void GEKawaseBlurShaderFilter::ComputeRadiusAndScale(int radius)
{
    static constexpr int noiseFactor = 3;                 // 3 : smooth the radius change
    blurRadius_ = radius * 4 / noiseFactor * noiseFactor; // 4 : scale between gauss radius and kawase
    AdjustRadiusAndScale();
}

void GEKawaseBlurShaderFilter::AdjustRadiusAndScale()
{
    static constexpr int radiusStep1 = 50;         // 50 : radius step1
    static constexpr int radiusStep2 = 150;        // 150 : radius step2
    static constexpr int radiusStep3 = 400;        // 400 : radius step3
    static constexpr float scaleFactor1 = 0.25f;   // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f;  // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.0625f; // 0.0625 : downSample scale for step3
    auto radius = static_cast<int>(blurRadius_);
    if (radius > radiusStep3) {
        blurScale_ = scaleFactor3;
    } else if (radius > radiusStep2) {
        blurScale_ = scaleFactor2;
    } else if (radius > radiusStep1) {
        blurScale_ = scaleFactor1;
    } else {
        blurScale_ = BASE_BLUR_SCALE;
    }
}

std::string GEKawaseBlurShaderFilter::GetDescription() const
{
    return "blur radius is " + std::to_string(blurRadius_);
}

} // namespace Rosen
} // namespace OHOS
