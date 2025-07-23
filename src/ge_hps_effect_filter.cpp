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

#include "ge_hps_effect_filter.h"

#include "draw/surface.h"
#include "ge_system_properties.h"
#include "ge_aibar_shader_filter.h"
#include "ge_grey_shader_filter.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_linear_gradient_blur_shader_filter.h"

namespace OHOS {
namespace Rosen {

namespace {
static constexpr uint32_t MAX_SURFACE_SIZE = 10000;
static constexpr size_t EXTENSION_SIZE_LIMIT = 1000;
static thread_local std::vector<const char *> g_extensionProperties;
const std::map<Drawing::GEVisualEffectImpl::FilterType, const char *> g_hpsSupportEffectExtensions {
    {Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR, "hps_gaussian_blur_effect"},
    {Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR, "hps_mesa_blur_effect"},
    {Drawing::GEVisualEffectImpl::FilterType::GREY, "hps_gray_effect"},
    {Drawing::GEVisualEffectImpl::FilterType::AIBAR, "hps_aibar_effect"},
    {Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR, "hps_gradient_blur_effect"}
};

static bool GetHpsEffectEnabled()
{
#ifdef GE_OHOS
    // Determine whether the hps effect render should be enabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.hpsEffectEnabled", "0")).c_str()) != 0;
    return enabled;
#else
    return false;
#endif
}

static float GetHpsEffectBlurNoiseFactor()
{
#ifdef GE_OHOS
    static float noiseFactor =
        std::atof((system::GetParameter("persist.sys.graphic.HpsEffectBlurNoiseFactor", "1.75")).c_str());
    return noiseFactor;
#else
    return 0.0f;
#endif
}

Drawing::Matrix GetShaderTransform(const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

void ApplyMaskColorFilter(Drawing::Canvas& offscreenCanvas, uint32_t maskColor)
{
    if (maskColor == 0) {
        return;
    }
    Drawing::Brush maskBrush;
    maskBrush.SetColor(maskColor);
    LOGD("HpsEffectFilter newMaskColor %{public}#x,", maskColor);
    offscreenCanvas.DrawBackground(maskBrush);
}
} // namespace

static std::shared_ptr<Drawing::RuntimeEffect> g_upscaleEffect;

HpsEffectFilter::HpsEffectFilter(Drawing::Canvas& canvas)
{
    if (g_extensionProperties.empty()) {
        if (canvas.GetGPUContext() == nullptr) {
            LOGE("HpsEffectFilter canvas.GetGPUContext is nullptr");
            return;
        }
        canvas.GetGPUContext()->GetHpsEffectSupport(g_extensionProperties);
        if (g_extensionProperties.empty()) {
            // add InitFinish for query once only
            g_extensionProperties.push_back("InitFinish");
        }
        if (g_extensionProperties.size() > EXTENSION_SIZE_LIMIT) {
            g_extensionProperties.clear();
            g_extensionProperties.push_back("InitFailed");
            LOGE("HpsEffectFilter g_extensionProperties is too large");
        }
    }
}

bool IsGradientSupport(
    const std::shared_ptr<Drawing::GELinearGradientBlurShaderFilterParams>& linearGradientBlurParams)
{
    if (!linearGradientBlurParams) {
        LOGE("HpsSupportEffectGE linearGradientBlurParams is null");
        return false;
    }
    return linearGradientBlurParams->isRadiusGradient;
}

bool HpsEffectFilter::IsEffectSupported(const std::shared_ptr<Drawing::GEVisualEffect> vef)
{
    auto ve = vef->GetImpl();
    auto veType = ve->GetFilterType();
    auto typeIt = g_hpsSupportEffectExtensions.find(veType);
    if (typeIt == g_hpsSupportEffectExtensions.end()) {
        return false;
    }
    for (const auto GEExtension : g_extensionProperties) {
        if (strcmp(GEExtension, typeIt->second) != 0) {
            continue;
        }
        if (veType == Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR) {
            const auto& linearGradientBlurParams = ve->GetLinearGradientBlurParams();
            return IsGradientSupport(linearGradientBlurParams);
        }
        return true;
    }
    return false;
}

bool HpsEffectFilter::HpsSupportEffectGE(Drawing::GEVisualEffectContainer& veContainer)
{
    if (!GetHpsEffectEnabled()) {
        return false;
    }
    for (auto vef : veContainer.GetFilters()) {
        if (!IsEffectSupported(vef)) {
            return false;
        }
    }
    return true;
}

std::vector<HpsEffectFilter::IndexRange> HpsEffectFilter::HpsSupportedEffectsIndexRanges(
    const std::vector<std::shared_ptr<Drawing::GEVisualEffect>>& filters)
{
    std::vector<IndexRange> intervals;
    std::vector<size_t> supportedIndices;

    if (!GetHpsEffectEnabled()) {
        return intervals;
    }

    for (size_t i = 0; i < filters.size(); ++i) {
        const auto& vef = filters[i];
        if (IsEffectSupported(vef)) {
            supportedIndices.push_back(i);
        }
    }

    if (supportedIndices.empty()) {
        return intervals;
    }

    size_t start = supportedIndices[0];
    size_t end = start;

    for (size_t i = 1; i < supportedIndices.size(); ++i) {
        if (supportedIndices[i] == end + 1) {
            end = supportedIndices[i];
        } else {
            intervals.push_back({start, end});
            start = supportedIndices[i];
            end = start;
        }
    }

    intervals.push_back({start, end});
    return intervals;
}

void HpsEffectFilter::GenerateVisualEffectFromGE(const std::shared_ptr<Drawing::GEVisualEffectImpl>& visualEffectImpl,
    const Drawing::Rect& src, const Drawing::Rect& dst, float saturationForHPS, float brightnessForHPS)
{
    switch (visualEffectImpl->GetFilterType()) {
        case Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR: {
            const auto& mesaParams = visualEffectImpl->GetMESAParams();
            isBlur_ = true;
            GenerateMesaBlurEffect(*mesaParams, src, dst);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
            const auto& kawaseParams = visualEffectImpl->GetKawaseParams();
            isBlur_ = true;
            GenerateKawaseBlurEffect(*kawaseParams, src, dst, saturationForHPS, brightnessForHPS);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GREY: {
            const auto& greyParams = visualEffectImpl->GetGreyParams();
            GenerateGreyEffect(*greyParams, src, dst);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
            const auto& aiBarParams = visualEffectImpl->GetAIBarParams();
            GenerateAIBarEffect(*aiBarParams, src, dst);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
            const auto& linearGradientBlurParams = visualEffectImpl->GetLinearGradientBlurParams();
            GenerateGradientBlurEffect(*linearGradientBlurParams, src, dst);
            break;
        }
        default:
            break;
    }
}

void HpsEffectFilter::GenerateMesaBlurEffect(const Drawing::GEMESABlurShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float radius = params.radius;
    float greyCoefLow = params.greyCoef1;
    float greyCoefHigh = params.greyCoef2;
    float offsetX = params.offsetX;
    float offsetY = params.offsetY;
    float offsetZ = params.offsetZ;
    float offsetW = params.offsetW;
    int tileMode = params.tileMode;
    float width = params.width;
    float height = params.height;

    if (radius < 1e-6) {
        LOGE("HpsEffectFilter::GenerateVisualEffectFromGE MESA sigma is zero");
        return;
    }

    auto mesaParamPtr = std::make_shared<Drawing::HpsMesaParameter>(
        src, dst, radius, greyCoefLow, greyCoefHigh, offsetX, offsetY,
        offsetZ, offsetW, tileMode, width, height);
    hpsEffect_.push_back(mesaParamPtr);
}

void HpsEffectFilter::GenerateKawaseBlurEffect(const Drawing::GEKawaseBlurShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst, float saturationForHPS, float brightnessForHPS)
{
    int radius = params.radius;
    if (radius < 1e-6) {
        LOGE("HpsEffectFilter::GenerateVisualEffectFromGE KAWASE_BLUR sigma is zero");
        return;
    }
    auto blurParamPtr = std::make_shared<Drawing::HpsBlurEffectParameter>(src, dst, Drawing::scalar(radius),
        saturationForHPS, brightnessForHPS);
    hpsEffect_.push_back(blurParamPtr);
}

void HpsEffectFilter::GenerateGreyEffect(const Drawing::GEGreyShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float greyCoefLow = params.greyCoef1;
    float greyCoefHigh = params.greyCoef2;
    auto greyParamPtr = std::make_shared<Drawing::HpsGreyParameter>(src, dst, greyCoefLow, greyCoefHigh);
    hpsEffect_.push_back(greyParamPtr);
}

void HpsEffectFilter::GenerateAIBarEffect(const Drawing::GEAIBarShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float low = params.aiBarLow;
    float high = params.aiBarHigh;
    float threshold = params.aiBarThreshold;
    float opacity = params.aiBarOpacity;
    float saturation = params.aiBarSaturation;
    auto aiBarParamPtr = std::make_shared<Drawing::HpsAiBarParameter>(
        src, dst, low, high, threshold, opacity, saturation);
    hpsEffect_.push_back(aiBarParamPtr);
}

void HpsEffectFilter::GenerateGradientBlurEffect(const Drawing::GELinearGradientBlurShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float blurRadius = params.blurRadius;
    std::vector<std::pair<float, float>> fractionStops = params.fractionStops;
    int direction = params.direction;
    float geoWidth = params.geoWidth;
    float geoHeight = params.geoHeight;
    Drawing::Matrix mat = params.mat;
    float tranX = params.tranX;
    float tranY = params.tranY;
    bool isOffscreenCanvas = params.isOffscreenCanvas;

    if (isOffscreenCanvas) {
        mat.PostTranslate(-tranX, -tranY);
    }

    if (blurRadius < 1e-6) {
        LOGE("HpsEffectFilter::GenerateVisualEffectFromGE LINEAR_GRADIENT_BLUR sigma is zero");
        return;
    }

    uint32_t fractionStopsCount = 0;
    std::shared_ptr<std::vector<float>> gra_fractions = std::make_shared<std::vector<float>>();
    const size_t GRA_MAT_SIZE = 9;
    std::array<float, GRA_MAT_SIZE> gra_mat;
    for (size_t i = 0; i < fractionStops.size(); i++) {
        gra_fractions->push_back(fractionStops[i].first);
        gra_fractions->push_back(fractionStops[i].second);
        fractionStopsCount++;
    }
    for (size_t i = 0; i < GRA_MAT_SIZE; i++) {
        gra_mat[i] = mat.Get(i);
    }
    auto graBlurParamPtr = std::make_shared<Drawing::HpsGradientBlurParameter>(
        src, dst, blurRadius, gra_fractions, fractionStopsCount, direction, geoWidth, geoHeight, gra_mat);
    hpsEffect_.push_back(graBlurParamPtr);
}

bool HpsEffectFilter::InitUpEffect() const
{
    if (g_upscaleEffect != nullptr) {
        return true;
    }

    static const std::string mixString(R"(
        uniform shader blurredInput;
        uniform float inColorFactor;

        highp float random(float2 xy) {
            float t = dot(xy, float2(78.233, 12.9898));
            return fract(sin(t) * 43758.5453);
        }
        half4 main(float2 xy) {
            highp float noiseGranularity = inColorFactor / 255.0;
            half4 finalColor = blurredInput.eval(xy);
            float noise = mix(-noiseGranularity, noiseGranularity, random(xy));
            finalColor.rgb += noise;
            return finalColor;
        }
    )");
    g_upscaleEffect = Drawing::RuntimeEffect::CreateForShader(mixString);
    if (g_upscaleEffect == nullptr) {
        return false;
    }

    return true;
}

bool HpsEffectFilter::DrawImageWithHps(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& imageCache,
    std::shared_ptr<Drawing::Image>& outImage, const Drawing::Rect& dst, const HpsEffectContext& hpsContext)
{
    if (imageCache == nullptr) {
        LOGE("HpsEffectFilter::DrawImageWithHps imageCache is nullptr");
        return false;
    }
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(*imageCache, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, upscale_matrix_);
    InitUpEffect();
    if (g_upscaleEffect == nullptr) {
        return false;
    }
    Drawing::Brush brush;
    float factor = GetHpsEffectBlurNoiseFactor();
    LOGD("HpsEffectFilter::DrawImageWithHps HpsNoise %{public}f", factor);
    static constexpr float epsilon = 0.1f;
    if (!ROSEN_LE(factor, epsilon)) {
        Drawing::RuntimeShaderBuilder mixBuilder(g_upscaleEffect);
        mixBuilder.SetChild("blurredInput", blurShader);
        mixBuilder.SetUniform("inColorFactor", factor);
        brush.SetShaderEffect(mixBuilder.MakeShader(nullptr, imageCache->IsOpaque()));
    } else {
        brush.SetShaderEffect(blurShader);
    }
    if (hpsContext.colorFilter != nullptr) {
        Drawing::Filter filter;
        filter.SetColorFilter(hpsContext.colorFilter);
        brush.SetFilter(filter);
    }
    brush.SetAlphaF(hpsContext.alpha);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    outImage = imageCache;
    return true;
}

bool HpsEffectFilter::ApplyHpsSmallCanvas(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Image>& outImage, const HpsEffectContext& hpsContext)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || image == nullptr || hpsEffect_.empty()) {
        return false;
    }
    std::shared_ptr<const Drawing::HpsBlurParameter> blurParams = nullptr;
    for (const auto& effectInfo : hpsEffect_) {
        if (effectInfo->GetEffectType() == Drawing::HpsEffect::BLUR) {
            auto kawaseParam = std::static_pointer_cast<Drawing::HpsBlurEffectParameter>(effectInfo);
            blurParams = std::make_shared<const Drawing::HpsBlurParameter>(kawaseParam->src,
                kawaseParam->dst, kawaseParam->sigma, kawaseParam->saturation, kawaseParam->brightness);
            break;
        }
        if (effectInfo->GetEffectType() == Drawing::HpsEffect::MESA) {
            auto mesaParam = std::static_pointer_cast<Drawing::HpsMesaParameter>(effectInfo);
            blurParams = std::make_shared<const Drawing::HpsBlurParameter>(
                mesaParam->src, mesaParam->dst, mesaParam->sigma, 1.0, 1.0);
            break;
        }
    }
    std::array<int, 2> dimension = canvas.CalcHpsBluredImageDimension(*blurParams); // There are 2 variables
    auto dst = blurParams->dst;
    if (dimension[0] <= 0 || dimension[1] <= 0 || dimension[0] >= static_cast<int>(MAX_SURFACE_SIZE)
        || dimension[1] >= static_cast<int>(MAX_SURFACE_SIZE)) {
        LOGD("HpsEffectFilter::ApplyHpsEffect CalcHpsBluredImageDimension error");
        return false;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(dimension[0], dimension[1]);
    if (offscreenSurface == nullptr) {
        return false;
    }

    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();

    Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
    for (auto& effectInfo : hpsEffect_) {
        effectInfo->dst = dimensionRect;
    }
    if (!offscreenCanvas->DrawImageEffectHPS(*image, hpsEffect_)) return false;
    ApplyMaskColorFilter(*offscreenCanvas, hpsContext.maskColor);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        return false;
    }

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto upscale_matrix = GetShaderTransform(dst, dst.GetWidth() / imageCache->GetWidth(),
        dst.GetHeight() / imageCache->GetHeight());
    upscale_matrix_ = upscale_matrix;
    return DrawImageWithHps(canvas, imageCache, outImage, dst, hpsContext);
}

bool HpsEffectFilter::ApplyHpsEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Image>& outImage, const HpsEffectContext& hpsContext)
{
    if (isBlur_) {
        isBlur_ = false;
        return ApplyHpsSmallCanvas(canvas, image, outImage, hpsContext);
    }
    auto surface = canvas.GetSurface();
    if (surface == nullptr || image == nullptr || hpsEffect_.empty()) {
        return false;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(static_cast<int>(image->GetWidth()),
                                                                              static_cast<int>(image->GetHeight()));
    if (offscreenSurface == nullptr) {
        return false;
    }
    for (auto& effectInfo : hpsEffect_) {
        effectInfo->dst = effectInfo->src;
    }
    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return false;
    }
    if (!offscreenCanvas->DrawImageEffectHPS(*image, hpsEffect_)) {
        return false;
    }

    auto imageCache = offscreenSurface->GetImageSnapshot();
    outImage = imageCache;
    return false;
}

} // namespace Rosen
} // namespace OHOS
