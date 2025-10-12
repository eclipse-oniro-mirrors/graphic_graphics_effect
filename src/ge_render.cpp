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
#include "ge_render.h"

#include "ge_aibar_shader_filter.h"
#include "ge_aurora_noise_shader.h"
#include "ge_bezier_warp_shader_filter.h"
#include "ge_border_light_shader.h"
#include "ge_color_gradient_shader_filter.h"
#include "ge_content_light_shader_filter.h"
#include "ge_contour_diagonal_flow_light_shader.h"
#include "ge_direction_light_shader_filter.h"
#include "ge_dispersion_shader_filter.h"
#include "ge_displacement_distort_shader_filter.h"
#include "ge_edge_light_shader_filter.h"
#include "ge_external_dynamic_loader.h"
#include "ge_filter_composer.h"
#include "ge_grey_shader_filter.h"
#include "ge_hps_compatible_pass.h"
#include "ge_hps_effect_filter.h"
#include "ge_hps_build_pass.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_linear_gradient_blur_shader_filter.h"
#include "ge_log.h"
#include "ge_magnifier_shader_filter.h"
#include "ge_mask_transition_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_mesa_fusion_pass.h"
#include "ge_particle_circular_halo_shader.h"
#include "sdf/ge_sdf_shader_filter.h"
#include "ge_sound_wave_filter.h"
#include "ge_system_properties.h"
#include "ge_visual_effect_impl.h"
#include "ge_variable_radius_blur_shader_filter.h"
#include "ge_water_ripple_filter.h"
#include "ge_wavy_ripple_light_shader.h"

namespace OHOS {
namespace GraphicsEffectEngine {
#define PROPERTY_MESA_BLUR_ALL_ENABLED "persist.sys.graphic.kawaseDisable"
#ifdef GE_OHOS
bool GERender::isMesablurAllEnable_ = (std::atoi(
    GESystemProperties::GetEventProperty(PROPERTY_MESA_BLUR_ALL_ENABLED).c_str()));
#else
bool GERender::isMesablurAllEnable_ = false;
#endif
using namespace Rosen::Drawing;
using ShaderCreator = std::function<std::shared_ptr<GEShader>(std::shared_ptr<GEVisualEffectImpl>)>;

static std::unordered_map<GEVisualEffectImpl::FilterType, ShaderCreator> g_shaderCreatorLUT = {
    {GEVisualEffectImpl::FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetContenDiagonalParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetContenDiagonalParams();
            out = std::make_shared<GEContourDiagonalFlowLightShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::WAVY_RIPPLE_LIGHT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetWavyRippleLightParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetWavyRippleLightParams();
            out = std::make_shared<GEWavyRippleLightShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::AURORA_NOISE, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetAuroraNoiseParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetAuroraNoiseParams();
            out = GEAuroraNoiseShader::CreateAuroraNoiseShader(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::PARTICLE_CIRCULAR_HALO, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetParticleCircularHaloParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetParticleCircularHaloParams();
            out = std::make_shared<GEParticleCircularHaloShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::COLOR_GRADIENT_EFFECT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                return out;
            }
            const auto& params = ve->GetColorGradientEffectParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::COLOR_GRADIENT_EFFECT);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXColorGradientEffectParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXColorGradientEffect::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
    {GEVisualEffectImpl::FilterType::HARMONIUM_EFFECT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                return out;
            }
            const auto& params = ve->GetHarmoniumEffectParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::HARMONIUM_EFFECT);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEHarmoniumEffectShaderParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXHarmoniumEffect::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
    {GEVisualEffectImpl::FilterType::LIGHT_CAVE, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                return out;
            }
            const auto& params = ve->GetLightCaveParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::LIGHT_CAVE);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXLightCaveShaderParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXLightCaveShader::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
    {GEVisualEffectImpl::FilterType::BORDER_LIGHT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetBorderLightParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetBorderLightParams();
            out = std::make_shared<GEBorderLightShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::AIBAR_GLOW, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (!ve) {
                return out;
            }
            const auto& params = ve->GetAIBarGlowEffectParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::AIBAR_GLOW);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXAIBarGlowEffectParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXAIBarGlowEffect::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
    {GEVisualEffectImpl::FilterType::ROUNDED_RECT_FLOWLIGHT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (!ve) {
                return out;
            }
            const auto& params = ve->GetRoundedRectFlowlightEffectParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::ROUNDED_RECT_FLOWLIGHT);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXRoundedRectFlowlightEffectParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXRoundedRectFlowlightEffect::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
    {GEVisualEffectImpl::FilterType::GRADIENT_FLOW_COLORS, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (!ve) {
                return out;
            }
            const auto& params = ve->GetGradientFlowColorsEffectParams();
            if (params == nullptr) {
                return out;
            }
            auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::GRADIENT_FLOW_COLORS);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXGradientFlowColorsEffectParams), static_cast<void*>(params.get()));
            if (!impl) {
                GE_LOGE("GEXGradientFlowColorsEffect::CreateDynamicImpl create object failed.");
                return out;
            }
            std::shared_ptr<GEShader> dmShader(static_cast<GEShader*>(impl));
            return dmShader;
        }
    },
};

GERender::GERender() {}

GERender::~GERender() {}

void GERender::DrawImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
    const Drawing::SamplingOptions& sampling)
{
    if (!image) {
        LOGE("GERender::DrawImageRect image is null");
        return;
    }

    auto resImage = ApplyImageEffect(canvas, veContainer, image, src, dst, sampling);
    if (!resImage) {
        LOGE("GERender::DrawImageRect resImage is null");
        return;
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*resImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GERender::ApplyImageEffect(Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    if (!image) {
        LOGE("GERender::ApplyImageEffect image is null");
        return nullptr;
    }
    auto resImage = image;
    for (auto vef: veContainer.GetFilters()) {
        ShaderFilterEffectContext context {resImage, src, dst};
        ApplyShaderFilter(canvas, vef, resImage, context);
    }

    return resImage;
}

void GERender::ApplyShaderFilter(Drawing::Canvas& canvas, std::shared_ptr<Drawing::GEVisualEffect> visualEffect,
                                 std::shared_ptr<Drawing::Image>& resImage, const ShaderFilterEffectContext& context)
{
    if (visualEffect == nullptr) {
        LOGD("GERender::ApplyShaderFilter visualEffect is null");
        return;
    }
    auto ve = visualEffect->GetImpl();
    std::shared_ptr<GEShaderFilter> geShaderFilter = GenerateShaderFilter(visualEffect);
    if (geShaderFilter == nullptr) {
        LOGD("GERender::ApplyShaderFilter geShaderFilter is null");
        return;
    }
    geShaderFilter->SetSupportHeadroom(visualEffect->GetSupportHeadroom());
    geShaderFilter->SetCache(ve->GetCache());
    geShaderFilter->Preprocess(canvas, context.src, context.dst);
    resImage = geShaderFilter->ProcessImage(canvas, resImage, context.src, context.dst);
    ve->SetCache(geShaderFilter->GetCache());
}

bool GERender::ApplyHpsGEImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const HpsGEImageEffectContext& context, std::shared_ptr<Drawing::Image>& outImage, Drawing::Brush& brush)
{
    if (!context.image) {
        LOGE("GERender::ApplyHpsGEImageEffect image is null");
        return false;
    }
    const auto& visualEffects = veContainer.GetFilters();
    if (visualEffects.empty()) {
        return false;
    }
    GEFilterComposer composer;
    auto hpsCompatiblePass = std::make_shared<GEHpsCompatiblePass>();
    composer.Add(hpsCompatiblePass);
    composer.Add<GEHpsBuildPass>(canvas, context);
    composer.Add<GEMesaFusionPass>();
    auto composables = GEFilterComposer::BuildComposables(visualEffects);
    auto composerResult = composer.Run(composables);
    if (!composerResult.anyPassChanged) {
        return false; // No pass has changed the effect sequence, fallback to original route to ensure compatibility
    }
    auto resImage = context.image;
    bool appliedHpsBlur = false;
    bool lastAppliedGE = true;
    for (auto& composable: composables) {
        auto currentImage = resImage;
        if (auto visualEffect = composable.GetEffect(); visualEffect != nullptr) {
            // dst assigned with src because GE doesn't apply downsample like HPS
            ShaderFilterEffectContext geContext {resImage, context.src, context.src};
            ApplyShaderFilter(canvas, visualEffect, resImage, geContext);
            lastAppliedGE = true;
        } else if (auto hpsEffect = composable.GetHpsEffect(); hpsEffect != nullptr) {
            HpsEffectFilter::HpsEffectContext hpsEffectContext = {
                context.alpha, context.colorFilter, context.maskColor};
            appliedHpsBlur |= hpsEffect->ApplyHpsEffect(canvas, currentImage, resImage, hpsEffectContext);
            lastAppliedGE = false;
        } else {
            LOGE("GERender::ApplyHpsGEImageEffect unhandled composable type");
        }
    }

    outImage = resImage;
    if (lastAppliedGE) { // ApplyShaderFilter does not draw to canvas by default
        DrawToCanvas(canvas, context, outImage, brush);
        return hpsCompatiblePass->IsBlurFilterExists();
    } else { // ApplyHpsEffect have done canvas sync
        return appliedHpsBlur; // If false, fallback to HPS 1.0 late-applied kawase blur
    }
}

void GERender::DrawToCanvas(Drawing::Canvas& canvas, const HpsGEImageEffectContext& context,
                            std::shared_ptr<Drawing::Image>& outImage, Drawing::Brush& brush)
{
    if (outImage != nullptr) {
        canvas.AttachBrush(brush);
        canvas.DrawImageRect(*outImage, context.src, context.dst, context.sampling);
        canvas.DetachBrush();
    }
}

// true represent Draw Kawase or Mesa succ, false represent Draw Kawase or Mesa false or no Kawase and Mesa
bool GERender::ApplyHpsImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const HpsGEImageEffectContext& context, std::shared_ptr<Drawing::Image>& outImage)
{
    auto hpsEffectFilter = std::make_shared<HpsEffectFilter>(canvas);
    if (!context.image) {
        LOGE("GERender::ApplyImageEffect image is null");
        return false;
    }

    if (veContainer.GetFilters().empty()) {
        return false;
    }

    if (hpsEffectFilter->HpsSupportEffectGE(veContainer)) {
        for (auto vef : veContainer.GetFilters()) {
            auto ve = vef->GetImpl();
            hpsEffectFilter->GenerateVisualEffectFromGE(ve, context.src, context.dst, context.saturationForHPS,
                context.brightnessForHPS, context.image);
        }

        HpsEffectFilter::HpsEffectContext hpsEffectContext = {context.alpha, context.colorFilter, context.maskColor};
        return hpsEffectFilter->ApplyHpsEffect(canvas, context.image, outImage, hpsEffectContext);
    }

    return false;
}

std::shared_ptr<GEShaderFilter> GERender::GenerateExtShaderFilter(
    const std::shared_ptr<Drawing::GEVisualEffectImpl>& ve)
{
    auto type = ve->GetFilterType();
    switch (type) {
        case Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR: {
            const auto& mesaParams = ve->GetMESAParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type), sizeof(Drawing::GEMESABlurShaderFilterParams),
                static_cast<void*>(mesaParams.get()));
            if (!object) {
                return std::make_shared<GEMESABlurShaderFilter>(*mesaParams);
            }
            std::shared_ptr<GEMESABlurShaderFilter> dmShader(static_cast<GEMESABlurShaderFilter*>(object));
            return dmShader;
        }
        case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
            const auto& linearGradientBlurParams = ve->GetLinearGradientBlurParams();
            if (linearGradientBlurParams->isRadiusGradient) {
                auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                    static_cast<uint32_t>(type), sizeof(Drawing::GELinearGradientBlurShaderFilterParams),
                    static_cast<void*>(linearGradientBlurParams.get()));
                if (object) {
                    std::shared_ptr<GELinearGradientBlurShaderFilter>
                        dmShader(static_cast<GELinearGradientBlurShaderFilter*>(object));
                    return dmShader;
                }
            }
            return std::make_shared<GELinearGradientBlurShaderFilter>(*linearGradientBlurParams);
        }
        case Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT: {
            const auto& edgeLightParams = ve->GetEdgeLightParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type), sizeof(Drawing::GEEdgeLightShaderFilterParams),
                static_cast<void*>(edgeLightParams.get()));
            if (!object) {
                return std::make_shared<GEEdgeLightShaderFilter>(*edgeLightParams);
            }
            std::shared_ptr<GEEdgeLightShaderFilter> dmShader(static_cast<GEEdgeLightShaderFilter*>(object));
            return dmShader;
        }
        case Drawing::GEVisualEffectImpl::FilterType::DISPERSION: {
            const auto& dispersionParams = ve->GetDispersionParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type), sizeof(Drawing::GEDispersionShaderFilterParams),
                static_cast<void*>(dispersionParams.get()));
            if (!object) {
                return std::make_shared<GEDispersionShaderFilter>(*dispersionParams);
            }
            std::shared_ptr<GEDispersionShaderFilter> dmShader(static_cast<GEDispersionShaderFilter*>(object));
            return dmShader;
        }
        case Drawing::GEVisualEffectImpl::FilterType::VARIABLE_RADIUS_BLUR: {
            const auto& variableRadiusBlurParams = ve->GetVariableRadiusBlurParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type),
                sizeof(Drawing::GEVariableRadiusBlurShaderFilterParams),
                static_cast<void*>(variableRadiusBlurParams.get()));
            if (!object) {
                return std::make_shared<GEVariableRadiusBlurShaderFilter>(*variableRadiusBlurParams);
            }
            std::shared_ptr<GEVariableRadiusBlurShaderFilter> dmShader(
                static_cast<GEVariableRadiusBlurShaderFilter*>(object));
            return dmShader;
        }
        default:
            break;
    }
    return nullptr;
}

std::shared_ptr<GEShaderFilter> GERender::GenerateShaderKawaseBlur(
    const std::shared_ptr<Drawing::GEVisualEffectImpl> &ve)
{
    const auto& kawaseParams = ve->GetKawaseParams();
    // Choose to use mesa blur algorithm or kawase blur algorithm
    if (!isMesablurAllEnable_) {
        return std::make_shared<GEKawaseBlurShaderFilter>(*kawaseParams);
    }
    // Transfer the kawaseParams to mesaParams
    auto mesaParams = std::make_shared<GEMESABlurShaderFilterParams>();
    mesaParams->radius = kawaseParams->radius;
    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR),
        sizeof(Drawing::GEMESABlurShaderFilterParams),
        static_cast<void*>(mesaParams.get()));
    if (!object) {
        return std::make_shared<GEMESABlurShaderFilter>(*mesaParams);
    }
    std::shared_ptr<GEMESABlurShaderFilter> dmShader(static_cast<GEMESABlurShaderFilter*>(object));
    return dmShader;
}

std::shared_ptr<GEShaderFilter> GERender::GenerateShaderFilter(
    const std::shared_ptr<Drawing::GEVisualEffect>& vef)
{
    auto ve = vef->GetImpl();
    std::shared_ptr<GEShaderFilter> shaderFilter;
    LOGD("GERender::GenerateShaderFilter %{public}d", (int)ve->GetFilterType());
    switch (ve->GetFilterType()) {
        case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
            shaderFilter = GenerateShaderKawaseBlur(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::AIBAR: {
            const auto& aiBarParams = ve->GetAIBarParams();
            shaderFilter = std::make_shared<GEAIBarShaderFilter>(*aiBarParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::COLOR_GRADIENT: {
            const auto& colorGradientParams = ve->GetColorGradientParams();
            shaderFilter = std::make_shared<GEColorGradientShaderFilter>(*colorGradientParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GREY: {
            const auto& greyParams = ve->GetGreyParams();
            shaderFilter = std::make_shared<GEGreyShaderFilter>(*greyParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER: {
            const auto& magnifierParams = ve->GetMagnifierParams();
            shaderFilter = std::make_shared<GEMagnifierShaderFilter>(*magnifierParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE: {
            const auto& waterRippleParams = ve->GetWaterRippleParams();
            shaderFilter = std::make_shared<GEWaterRippleFilter>(*waterRippleParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER: {
            const auto& displacementDistortParams = ve->GetDisplacementDistortParams();
            shaderFilter = std::make_shared<GEDisplacementDistortFilter>(*displacementDistortParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::SOUND_WAVE: {
            const auto& soundWaveParams = ve->GetSoundWaveParams();
            shaderFilter = std::make_shared<GESoundWaveFilter>(*soundWaveParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::BEZIER_WARP: {
            const auto& bezierWarpParams = ve->GetBezierWarpParams();
            shaderFilter = std::make_shared<GEBezierWarpShaderFilter>(*bezierWarpParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::DISPERSION: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::CONTENT_LIGHT: {
            const auto& contentLightParams = ve->GetContentLightParams();
            shaderFilter = std::make_shared<GEContentLightFilter>(*contentLightParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::DIRECTION_LIGHT: {
            const auto& directionLightParams = ve->GetDirectionLightParams();
            shaderFilter = std::make_shared<GEDirectionLightShaderFilter>(*directionLightParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::MASK_TRANSITION: {
            const auto& maskTransitionParams = ve->GetMaskTransitionParams();
            shaderFilter = std::make_shared<GEMaskTransitionShaderFilter>(*maskTransitionParams);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::VARIABLE_RADIUS_BLUR: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::SDF: {
            const auto& params = ve->GetSDFFilterParams();
            if (!sdfShaderFilter_) {
                sdfShaderFilter_ = std::make_shared<GESDFShaderFilter>(*params);
            } else {
                sdfShaderFilter_->Update(*params);
            }
            shaderFilter = sdfShaderFilter_;
            break;
        }
        default:
            break;
    }
    if (shaderFilter) {
        shaderFilter->SetShaderFilterCanvasinfo(vef->GetCanvasInfo());
        shaderFilter->SetSupportHeadroom(vef->GetSupportHeadroom());
    }
    return shaderFilter;
}

std::vector<std::shared_ptr<GEShaderFilter>> GERender::GenerateShaderFilters(
    const Drawing::GEVisualEffectContainer& veContainer)
{
    LOGD("GERender::shaderFilters %{public}d", (int)veContainer.GetFilters().size());
    std::vector<std::shared_ptr<GEShaderFilter>> shaderFilters;
    for (auto vef : veContainer.GetFilters()) {
        std::shared_ptr<GEShaderFilter> shaderFilter = GenerateShaderFilter(vef);
        shaderFilters.push_back(shaderFilter);
    }
    return shaderFilters;
}

void GERender::DrawShaderEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const Drawing::Rect& bounds)
{
    LOGD("GERender::DrawShaderEffect %{public}zu", veContainer.GetFilters().size());
    for (auto vef : veContainer.GetFilters()) {
        if (vef == nullptr) {
            LOGD("GERender::DrawShaderEffect vef is null");
            continue;
        }
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShader> geShaderEffect = GenerateShaderEffect(ve);
        if (geShaderEffect == nullptr) {
            LOGD("GERender::DrawShaderEffect shader is null");
            continue;
        }
        geShaderEffect->SetCache(ve->GetCache());
        geShaderEffect->DrawShader(canvas, bounds);
        ve->SetCache(geShaderEffect->GetCache());
    }
}

std::shared_ptr<GEShader> GERender::GenerateShaderEffect(const std::shared_ptr<Drawing::GEVisualEffectImpl>& ve)
{
    auto it = g_shaderCreatorLUT.find(ve->GetFilterType());
    return it != g_shaderCreatorLUT.end() ? it->second(ve) : nullptr;
}

void GERender::SetMesablurAllEnabledByCCM(bool flag)
{
    isMesablurAllEnable_ = isMesablurAllEnable_ || flag;
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
