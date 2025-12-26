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
#include "ge_radial_gradient_shader_mask.h"
#include "ge_grey_shader_filter.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_linear_gradient_blur_shader_filter.h"
#include "ge_pixel_map_shader_mask.h"

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
    {Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR, "hps_gradient_blur_effect"},
    {Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT, "hps_edgelight_effect"}
};

const std::unordered_set<Drawing::HpsEffect> g_needDownScaleEffect {
    Drawing::HpsEffect::BLUR,
    Drawing::HpsEffect::MESA
};

const std::unordered_map<Drawing::GEVisualEffectImpl::FilterType, std::unordered_set<Drawing::GEFilterType>>
    g_hpsEffectMaskSupportExtensions {
    {Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT, {Drawing::GEFilterType::PIXEL_MAP_MASK,
        Drawing::GEFilterType::RADIAL_GRADIENT_MASK}},
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

std::array<float, Drawing::MATRIX_3X3_SIZE> GetTransformMatrix(
    const Drawing::CanvasInfo& canvasInfo, const std::shared_ptr<Drawing::Image>& image)
{
    Drawing::Matrix matrix = canvasInfo.mat;
    matrix.PostTranslate(-canvasInfo.tranX, -canvasInfo.tranY);
    if (image != nullptr) {
        auto imageInfo = image->GetImageInfo();
        if (abs(canvasInfo.geoWidth - imageInfo.GetWidth()) > 1e-6 ||
            abs(canvasInfo.geoHeight - imageInfo.GetHeight()) > 1e-6) {
            // width or height not equal, needs mat to scale width and height
            float sx = 1.0f;
            float sy = 1.0f;
            if (imageInfo.GetWidth() > 0) {
                sx = static_cast<float>(canvasInfo.geoWidth) / static_cast<float>(imageInfo.GetWidth());
            }
            if (imageInfo.GetHeight() > 0) {
                sy = static_cast<float>(canvasInfo.geoHeight) / static_cast<float>(imageInfo.GetHeight());
            }
            matrix.PreScale(sx, sy);
        }
    }
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    return value;
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

// Global parameters storage for cache usage
static thread_local uint32_t g_lastEdgeLightImageID = 0;
static thread_local std::shared_ptr<Drawing::HpsEdgeLightParameter> g_lastEdgeLightParameter = nullptr;

// Edge light update type enum
enum class EdgeLightUpdateType : uint32_t {
    IMAGE = 0x1,
    ALPHA = 0x2,
    EDGE_SOBEL = 0x4,
    GAUSS_PYRAMID = 0x8,
    MASK = 0x10,
};

// Get EdgeLight updatedType num for hps cache usage
uint32_t GetUpdatedTypeForEdgeLight(const std::shared_ptr<Drawing::HpsEdgeLightParameter>& paraLeft,
    const std::shared_ptr<Drawing::HpsEdgeLightParameter>& paraRight, uint32_t imgLeft, uint32_t imgRight)
{
    uint32_t type = 0;
    type |= imgLeft == imgRight ? static_cast<uint32_t>(EdgeLightUpdateType::IMAGE) : 0;
    type |= GE_EQ(paraLeft->alpha, paraRight->alpha) ? static_cast<uint32_t>(EdgeLightUpdateType::ALPHA) : 0;
    type |= (GE_EQ(paraLeft->edgeSobelParams.edgeThreshold, paraRight->edgeSobelParams.edgeThreshold) &&
             GE_EQ(paraLeft->edgeSobelParams.edgeIntensity, paraRight->edgeSobelParams.edgeIntensity) &&
             GE_EQ(paraLeft->edgeSobelParams.edgeSoftThreshold, paraRight->edgeSobelParams.edgeSoftThreshold) &&
             paraLeft->edgeSobelParams.edgeDetectColor == paraRight->edgeSobelParams.edgeDetectColor)
        ? static_cast<uint32_t>(EdgeLightUpdateType::EDGE_SOBEL) : 0;
    type |= paraLeft->bloom == paraRight->bloom ? static_cast<uint32_t>(EdgeLightUpdateType::GAUSS_PYRAMID) : 0;
    type |= HpsEffectFilter::IsMaskParameterChanged(paraLeft->mask, paraRight->mask)
        ? static_cast<uint32_t>(EdgeLightUpdateType::MASK) : 0;
    return type;
}
} // namespace

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

bool IsEffectMaskSupport(
    Drawing::GEVisualEffectImpl::FilterType type, const std::shared_ptr<Drawing::GEShaderMask>& mask)
{
    if (mask == nullptr) {
        return true;
    }
    auto typeIt = g_hpsEffectMaskSupportExtensions.find(type);
    if (typeIt == g_hpsEffectMaskSupportExtensions.end()) {
        return false;
    }

    const auto& supportedMaskTypes = typeIt->second;
    return supportedMaskTypes.find(mask->Type()) != supportedMaskTypes.end();
}

bool HpsEffectFilter::IsEffectSupported(const std::shared_ptr<Drawing::GEVisualEffect>& vef)
{
    auto ve = vef->GetImpl();
    auto veType = ve->GetFilterType();
    auto typeIt = g_hpsSupportEffectExtensions.find(veType);
    if (typeIt == g_hpsSupportEffectExtensions.end()) {
        return false;
    }
    LOGD("HpsEffectFilter::IsEffectSupported type found: veType=%{public}d", static_cast<int>(veType));
    for (const auto GEExtension : g_extensionProperties) {
        if (strcmp(GEExtension, typeIt->second) != 0) {
            continue;
        }
        switch (veType) {
            case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
                const auto& linearGradientBlurParams = ve->GetLinearGradientBlurParams();
                return linearGradientBlurParams && IsGradientSupport(linearGradientBlurParams);
            }
            case Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT: {
                const auto& edgeLightParams = ve->GetEdgeLightParams();
                return edgeLightParams && edgeLightParams->mask && IsEffectMaskSupport(veType, edgeLightParams->mask);
            }
            default:
                return true;
        }
    }
    LOGD("HpsEffectFilter::IsEffectSupported GEExtension not found");
    return false;
}

bool HpsEffectFilter::IsFilterSupport() const
{
    Drawing::GEVisualEffectImpl::FilterType filterType = Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR;
    auto typeIt = g_hpsSupportEffectExtensions.find(filterType);
    if (typeIt == g_hpsSupportEffectExtensions.end()) {
        return false;
    }
    for (const auto GEExtension : g_extensionProperties) {
        if (strcmp(GEExtension, typeIt->second) != 0) {
            continue;
        }
    }
    return false;
}

void HpsEffectFilter::GenerateBlur(const Drawing::GEKawaseBlurShaderFilterParams& params,
    const Drawing::Rect& src, const Drawing::Rect& dst, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::CanvasInfo& canvasInfo)
{
    auto KawaseParams = GenerateKawaseBlurEffect(*params, src, dst, 1.0f, 1.0f);
    if (KawaseParams) {
        KawaseParams->transformMatrix = GetTransformMatrix(canvasInfo, image);
        hpsEffect_.push_back(KawaseParams);  // Add hpsEffectParameter to container
    }
}

std::shared_ptr<Drawing::Image> HpsEffectFilter::GetBlurForFrostedGlassBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image)
{
    if (image == nullptr || hpsEffect_.empty()) {
        LOGD("HpsEffectFilter::GetBlurForFrostedGlassBlur image is null or hpsEffect_ is empty");
        return nullptr;
    }

    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetWidth() <= 0 || imageInfo.GetHeight() <= 0) {
        LOGE("HpsEffectFilter::GetBlurForFrostedGlassBlur image size is zero");
        return nullptr;
    }
    needClampFilter_ = ((imageInfo.GetColorType() == Drawing::ColorType::COLORTYPE_RGBA_F16) ? false : true);
    bool isDownScaled = IsNeedDownscale();
    auto dimension = GetSurfaceSize(canvas, image, isDownScaled);
    if (dimension[0] == 0 || dimension[1] == 0) {
        LOGE("HpsEffectFilter::GetBlurForFrostedGlassBlur dimension equals zero");
        return nullptr;
    }
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(dimension[0], dimension[1]);
    if (offscreenSurface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return nullptr;
    }
    if (isDownScaled) {
        Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
        for (auto& effectInfo : hpsEffect_) {
            effectInfo->dst = dimensionRect;
        }
    } else {
        for (auto& effectInfo : hpsEffect_) {
            effectInfo->dst = effectInfo->src;
        }
    }
    if (!offscreenCanvas->DrawImageEffectHPS(*image, hpsEffect_)) {
        LOGD("HpsEffectFilter::GetBlurForFrostedGlassBlur DrawImageEffectHPS fail");
        return nullptr;
    }

    auto imageCache = offscreenSurface->GetImageSnapshot();
    return imageCache;
}

bool HpsEffectFilter::HpsSupportEffectGE(const Drawing::GEVisualEffectContainer& veContainer)
{
    if (!GetHpsEffectEnabled()) {
        return false;
    }
    for (const auto& vef : veContainer.GetFilters()) {
        if (!IsEffectSupported(vef)) {
            return false;
        }
    }
    return true;
}

bool HpsEffectFilter::IsHpsEffectEnabled() const
{
    return GetHpsEffectEnabled();
}

void HpsEffectFilter::GenerateVisualEffectFromGE(const std::shared_ptr<Drawing::GEVisualEffectImpl>& visualEffectImpl,
    const Drawing::Rect& src, const Drawing::Rect& dst, float saturationForHPS, float brightnessForHPS,
    const std::shared_ptr<Drawing::Image>& image)
{
    if (!visualEffectImpl) {
        return;
    }
    std::shared_ptr<Drawing::HpsEffectParameter> params = nullptr;
    originDst_ = dst;
    switch (visualEffectImpl->GetFilterType()) {
        case Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR: {
            const auto& mesaParams = visualEffectImpl->GetMESAParams();
            params = GenerateMesaBlurEffect(*mesaParams, src, dst, image);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
            const auto& kawaseParams = visualEffectImpl->GetKawaseParams();
            params = GenerateKawaseBlurEffect(*kawaseParams, src, dst, saturationForHPS, brightnessForHPS);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GREY: {
            const auto& greyParams = visualEffectImpl->GetGreyParams();
            params = GenerateGreyEffect(*greyParams, src, dst);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
            const auto& aiBarParams = visualEffectImpl->GetAIBarParams();
            params = GenerateAIBarEffect(*aiBarParams, src, dst);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
            const auto& linearGradientBlurParams = visualEffectImpl->GetLinearGradientBlurParams();
            auto canvasInfo = visualEffectImpl->GetCanvasInfo();
            params = GenerateGradientBlurEffect(*linearGradientBlurParams, src, dst, image, canvasInfo);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT: {
            const auto& edgeLightParams = visualEffectImpl->GetEdgeLightParams();
            auto canvasInfo = visualEffectImpl->GetCanvasInfo();
            params = GenerateEdgeLightEffect(*edgeLightParams, src, dst, image, canvasInfo);
            break;
        }
        default:
            LOGW("HpsEffectFilter::GenerateVisualEffectFromGE not supported, type=%{public}d",
                 static_cast<int>(visualEffectImpl->GetFilterType()));
            break;
    }
    if (params) {
        params->transformMatrix = GetTransformMatrix(visualEffectImpl->GetCanvasInfo(), image);
        hpsEffect_.push_back(params);  // Add hpsEffectParameter to container
    }
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateMesaBlurEffect(
    const Drawing::GEMESABlurShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst,
    const std::shared_ptr<Drawing::Image>& image)
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
        return nullptr;
    }

    int imagewidth = image->GetWidth();
    int imageHeight = image->GetHeight();
    if (width > 0) {
        offsetX = imagewidth * (offsetX / width);
        offsetZ = imagewidth * (offsetZ / width);
    }
    if (height > 0) {
        offsetY = imageHeight * (offsetY / height);
        offsetW = imageHeight * (offsetW / height);
    }

    auto mesaParamPtr = std::make_shared<Drawing::HpsMesaParameter>(
        src, dst, radius, greyCoefLow, greyCoefHigh, offsetX, offsetY,
        offsetZ, offsetW, tileMode, width, height);
    return mesaParamPtr;
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateKawaseBlurEffect(
    const Drawing::GEKawaseBlurShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst,
    float saturationForHPS, float brightnessForHPS)
{
    int radius = params.radius;
    if (radius < 1e-6) {
        LOGD("HpsEffectFilter::GenerateVisualEffectFromGE KAWASE_BLUR sigma is zero");
        return nullptr;
    }
    auto blurParamPtr = std::make_shared<Drawing::HpsBlurEffectParameter>(src, dst, Drawing::scalar(radius),
        saturationForHPS, brightnessForHPS);
    return blurParamPtr;
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateGreyEffect(
    const Drawing::GEGreyShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float greyCoefLow = params.greyCoef1;
    float greyCoefHigh = params.greyCoef2;
    auto greyParamPtr = std::make_shared<Drawing::HpsGreyParameter>(src, dst, greyCoefLow, greyCoefHigh);
    return greyParamPtr;
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateAIBarEffect(
    const Drawing::GEAIBarShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    float low = params.aiBarLow;
    float high = params.aiBarHigh;
    float threshold = params.aiBarThreshold;
    float opacity = params.aiBarOpacity;
    float saturation = params.aiBarSaturation;
    auto aiBarParamPtr = std::make_shared<Drawing::HpsAiBarParameter>(
        src, dst, low, high, threshold, opacity, saturation);
    return aiBarParamPtr;
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateGradientBlurEffect(
    const Drawing::GELinearGradientBlurShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst,
    const std::shared_ptr<Drawing::Image>& image, Drawing::CanvasInfo info)
{
    float blurRadius = params.blurRadius;
    std::vector<std::pair<float, float>> fractionStops = params.fractionStops;
    int direction = params.direction;
    float geoWidth = info.geoWidth;
    float geoHeight = info.geoHeight;
    Drawing::Matrix mat = info.mat;
    float tranX = info.tranX;
    float tranY = info.tranY;
    bool isOffscreenCanvas = params.isOffscreenCanvas;

    if (isOffscreenCanvas) {
        mat.PostTranslate(-tranX, -tranY);
    }

    if (blurRadius < 1e-6) {
        LOGE("HpsEffectFilter::GenerateVisualEffectFromGE LINEAR_GRADIENT_BLUR sigma is zero");
        return nullptr;
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

    auto imageInfo = image->GetImageInfo();
    if (abs(geoWidth - imageInfo.GetWidth()) > 1e-3 || abs(geoHeight - imageInfo.GetHeight()) > 1e-3) {
        // width or height not equal, needs mat to scale width and height
        float sx = 1.0;
        float sy = 1.0;
        if (imageInfo.GetWidth() > 0) {
            sx = static_cast<float>(geoWidth) / static_cast<float>(imageInfo.GetWidth());
        }
        if (imageInfo.GetHeight() > 0) {
            sy = static_cast<float>(geoHeight) / static_cast<float>(imageInfo.GetHeight());
        }
        mat.PreScale(sx, sy);
    }
    for (size_t i = 0; i < GRA_MAT_SIZE; i++) {
        gra_mat[i] = mat.Get(i);
    }
    auto graBlurParamPtr = std::make_shared<Drawing::HpsGradientBlurParameter>(
        src, dst, blurRadius, gra_fractions, fractionStopsCount, direction, geoWidth, geoHeight, gra_mat);
    return graBlurParamPtr;
}

std::shared_ptr<Drawing::HpsEffectParameter> HpsEffectFilter::GenerateEdgeLightEffect(
    const Drawing::GEEdgeLightShaderFilterParams& params, const Drawing::Rect& src, const Drawing::Rect& dst,
    const std::shared_ptr<Drawing::Image>& image, Drawing::CanvasInfo info)
{
    float alpha = params.alpha;
    bool bloom = params.bloom;
    bool useRawColor = params.useRawColor;
    std::vector<float> color = {params.color[0], params.color[1], params.color[2], params.color[3]};
    std::shared_ptr<Drawing::HpsMaskParameter> mask;
    if (params.mask) {
        mask = GenerateMaskParameter(params.mask);
    }
    // Fix parameters for edge light render
    constexpr float edgeThreshold = 0.3f;
    constexpr float edgeIntensity = 0.8f;
    constexpr float edgeSoftThreshold = 0.3f;
    const std::vector<float> edgeDetectColor = {0.22f, 0.707f, 0.875f, 0.0f};
    Drawing::HpsEdgeLightParameter::EdgeSobelParameter edgeSobelParam {
        edgeThreshold, edgeIntensity, edgeSoftThreshold, edgeDetectColor};
    auto edgeLightParamPtr = std::make_shared<Drawing::HpsEdgeLightParameter>(
        src, dst, alpha, bloom, useRawColor, color, mask, edgeSobelParam, 0);
    if (g_lastEdgeLightParameter) {
        edgeLightParamPtr->updatedType = GetUpdatedTypeForEdgeLight(
            g_lastEdgeLightParameter, edgeLightParamPtr, g_lastEdgeLightImageID, image ? image->GetUniqueID() : 0);
    }
    g_lastEdgeLightParameter = edgeLightParamPtr;
    g_lastEdgeLightImageID = image ? image->GetUniqueID() : 0;
    return edgeLightParamPtr;
}

std::shared_ptr<Drawing::HpsMaskParameter> HpsEffectFilter::GenerateMaskParameter(
    const std::shared_ptr<Drawing::GEShaderMask>& mask)
{
    switch (mask->Type()) {
        case Drawing::GEFilterType::PIXEL_MAP_MASK: {
            auto pixelMapMask = std::static_pointer_cast<Drawing::GEPixelMapShaderMask>(mask);
            return GeneratePixelMapMaskParameter(pixelMapMask->GetGEPixelMapMaskParams());
        }
        case Drawing::GEFilterType::RADIAL_GRADIENT_MASK: {
            auto radialGradientShaderMask = std::static_pointer_cast<Drawing::GERadialGradientShaderMask>(mask);
            return GenerateRadialGradientShaderMaskParameter(
                radialGradientShaderMask->GetGERadialGradientShaderMaskParams());
        }
        default:
            LOGW("HpsEffectFilter::GenerateMaskParameter mask type not supported, type=%{public}d", mask->Type());
            return nullptr;
    }
}

std::shared_ptr<Drawing::HpsMaskParameter> HpsEffectFilter::GeneratePixelMapMaskParameter(
    const Drawing::GEPixelMapMaskParams& params)
{
    /* Compare to standard pixel map mask, HSP use difference transform matrix */
    Drawing::Matrix matrix;
    auto sx = params.src.GetWidth() / params.dst.GetWidth();
    auto sy = params.src.GetHeight() / params.dst.GetHeight();
    auto tx = params.src.left_ - params.dst.left_ * sx;
    auto ty = params.src.bottom_ - params.dst.bottom_ * sy;
    matrix.SetScaleTranslate(sx, sy, tx, ty);
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    std::vector<float> fillColor = {params.fillColor[0], params.fillColor[1], params.fillColor[2], params.fillColor[3]};
    return std::make_shared<Drawing::HpsPixelMapMaskParameter>(params.image, params.dst, value, fillColor);
}

std::shared_ptr<Drawing::HpsMaskParameter> HpsEffectFilter::GenerateRadialGradientShaderMaskParameter(
    const Drawing::GERadialGradientShaderMaskParams& params)
{
    return std::make_shared<Drawing::HpsRadialGradientMaskParameter>(params.center_.first, params.center_.second,
        params.radiusX_, params.radiusY_, params.colors_, params.positions_);
}

std::shared_ptr<Drawing::RuntimeEffect> HpsEffectFilter::GetUpscaleEffect() const
{
    if (needClampFilter_) {
        static std::shared_ptr<Drawing::RuntimeEffect> s_clampUpEffect = [] {
            static const std::string mixClampString(R"(
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
                finalColor.rgb = clamp(finalColor.rgb, vec3(0.0), vec3(1.0));
                return finalColor;
            }
            )");
            return Drawing::RuntimeEffect::CreateForShader(mixClampString);
        }();
        return s_clampUpEffect;
    }
    static std::shared_ptr<Drawing::RuntimeEffect> s_upscaleEffect = [] {
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
        return Drawing::RuntimeEffect::CreateForShader(mixString);
    }();
    return s_upscaleEffect;
}

bool HpsEffectFilter::DrawImageWithHpsUpscale(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& imageCache, std::shared_ptr<Drawing::Image>& outImage,
    const HpsEffectContext& hpsContext)
{
    if (imageCache == nullptr) {
        LOGE("HpsEffectFilter::DrawImageWithHpsUpscale imageCache is nullptr");
        return false;
    }
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix matrix = GetShaderTransform(
        originDst_, originDst_.GetWidth() / imageCache->GetWidth(), originDst_.GetHeight() / imageCache->GetHeight());
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, matrix);
    auto upscaleEffect = GetUpscaleEffect();
    if (upscaleEffect == nullptr) {
        return false;
    }
    float factor = GetHpsEffectBlurNoiseFactor();
    if (IsNeedUpscale()) {
        Drawing::Matrix invertMatrix;
        invertMatrix.Invert(matrix);
        invertMatrix.Translate(-originDst_.GetLeft(), -originDst_.GetTop());
        auto imageInfo = imageCache->GetImageInfo();
        imageInfo.SetWidth(originDst_.GetWidth());
        imageInfo.SetHeight(originDst_.GetHeight());
        Drawing::RuntimeShaderBuilder mixBuilder(upscaleEffect);
        mixBuilder.SetChild("blurredInput", blurShader);
        mixBuilder.SetUniform("inColorFactor", factor);
        outImage = mixBuilder.MakeImage(canvas.GetGPUContext().get(), &(invertMatrix), imageInfo, false);
        return false;
    }
    Drawing::Brush brush;
    static constexpr float epsilon = 0.1f;
    if (!GE_LE(factor, epsilon)) {
        Drawing::RuntimeShaderBuilder mixBuilder(upscaleEffect);
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
    canvas.DrawRect(originDst_);
    canvas.DetachBrush();
    return true;
}

std::array<int, ARRAY_SIZE_DIMENSION> HpsEffectFilter::GetSurfaceSize(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, bool isDownScaled)
{
    if (!image) {
        LOGE("HpsEffectFilter::GetSurfaceSize image is null");
        return {0, 0};
    }
    if (!isDownScaled) {
        return {static_cast<int>(image->GetWidth()), static_cast<int>(image->GetHeight())};
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
    if (blurParams == nullptr) {
        LOGE("HpsEffectFilter::GetSurfaceSize no BLUR but downscale");
        return {0, 0};
    }
    std::array<int, ARRAY_SIZE_DIMENSION> dimension = canvas.CalcHpsBluredImageDimension(*blurParams);
    if (dimension[0] <= 0 || dimension[1] <= 0 || dimension[0] >= static_cast<int>(MAX_SURFACE_SIZE) ||
        dimension[1] >= static_cast<int>(MAX_SURFACE_SIZE)) {
        LOGD("HpsEffectFilter::GetSurfaceSize CalcHpsBluredImageDimension error");
        return {0, 0};
    }
    Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
    for (auto& effectInfo : hpsEffect_) {
        effectInfo->dst = dimensionRect;
    }
    return dimension;
}

// Check mask parameters changed or not when hps cache enabled
bool HpsEffectFilter::IsMaskParameterChanged(
    const std::shared_ptr<Drawing::HpsMaskParameter>& pL, const std::shared_ptr<Drawing::HpsMaskParameter>& pR)
{
    if (!pL || !pR || pL->GetMaskType() != pR->GetMaskType()) {
        return true;
    }
    auto type = pL->GetMaskType();
    switch (type) {
        case Drawing::HpsMask::PIXEL_MAP_MASK: {
            const auto convertedL = std::static_pointer_cast<Drawing::HpsPixelMapMaskParameter>(pL);
            const auto convertedR = std::static_pointer_cast<Drawing::HpsPixelMapMaskParameter>(pR);
            auto imageIdL = convertedL->image ? convertedL->image->GetUniqueID() : 0;
            auto imageIdR = convertedR->image ? convertedR->image->GetUniqueID() : 0;
            return !(convertedL->visibleRegion == convertedR->visibleRegion &&
                     convertedL->transformMatrix == convertedR->transformMatrix &&
                     convertedL->fillColor == convertedR->fillColor &&
                     imageIdL == imageIdR);
        }
        case Drawing::HpsMask::RADIAL_GRADIENT_MASK: {
            const auto convertedL = std::static_pointer_cast<Drawing::HpsRadialGradientMaskParameter>(pL);
            const auto convertedR = std::static_pointer_cast<Drawing::HpsRadialGradientMaskParameter>(pR);
            return !(convertedL->centerX == convertedR->centerX && convertedL->centerY == convertedR->centerY &&
                     convertedL->radiusX == convertedR->radiusX && convertedL->radiusY == convertedR->radiusY &&
                     convertedL->colors == convertedR->colors && convertedL->positions == convertedR->positions);
        }
        default:
            return true;
    }
}

bool HpsEffectFilter::IsNeedDownscale()
{
    bool isDownScaled = false;
    for (const auto& effectInfo : hpsEffect_) {
        if (g_needDownScaleEffect.find(effectInfo->GetEffectType()) != g_needDownScaleEffect.end()) {
            isDownScaled = true;
            break;
        }
    }
    return isDownScaled;
}

bool HpsEffectFilter::ApplyHpsEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<Drawing::Image>& outImage, const HpsEffectContext& hpsContext)
{
    if (image == nullptr || hpsEffect_.empty()) {
        LOGD("HpsEffectFilter::ApplyHpsEffect image is null or hpsEffect_ is empty");
        return false;
    }

    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetWidth() <= 0 || imageInfo.GetHeight() <= 0) {
        LOGE("HpsEffectFilter::ApplyHpsEffect image size is zero");
        return false;
    }
    needClampFilter_ = ((imageInfo.GetColorType() == Drawing::ColorType::COLORTYPE_RGBA_F16) ? false : true);
    bool isDownScaled = IsNeedDownscale();
    auto dimension = GetSurfaceSize(canvas, image, isDownScaled);
    if (dimension[0] == 0 || dimension[1] == 0) {
        LOGE("HpsEffectFilter::ApplyHpsEffect dimension equals zero");
        return false;
    }
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        return false;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(dimension[0], dimension[1]);
    if (offscreenSurface == nullptr) { return false; }
    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) { return false; }
    if (isDownScaled) {
        Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
        for (auto& effectInfo : hpsEffect_) {
            effectInfo->dst = dimensionRect;
        }
    } else {
        for (auto& effectInfo : hpsEffect_) {
            effectInfo->dst = effectInfo->src;
        }
    }
    if (!offscreenCanvas->DrawImageEffectHPS(*image, hpsEffect_)) {
        LOGD("HpsEffectFilter::ApplyHpsEffect DrawImageEffectHPS fail");
        return false;
    }
    if (isDownScaled) {
        ApplyMaskColorFilter(*offscreenCanvas, hpsContext.maskColor);
    }

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (isDownScaled) {
        return DrawImageWithHpsUpscale(canvas, imageCache, outImage, hpsContext); // downscale need upscale or drawback
    }
    outImage = imageCache;
    return false;
}

bool HpsEffectFilter::IsNeedUpscale()
{
    return needUpscale_;
}

void HpsEffectFilter::SetNeedUpscale(bool needUpscale)
{
    needUpscale_ = needUpscale;
}

void HpsEffectFilter::UnitTestSetExtensionProperties(const std::vector<const char *>& extensionProperties)
{
    // Used in unit tests due to non-Mockable Drawing::GPUContext, don't use in general cases
    g_extensionProperties = extensionProperties;
}

} // namespace Rosen
} // namespace OHOS
