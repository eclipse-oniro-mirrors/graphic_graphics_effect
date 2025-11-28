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
#include "ge_circle_flowlight_effect.h"
#include "ge_color_gradient_shader_filter.h"
#include "ge_content_light_shader_filter.h"
#include "ge_contour_diagonal_flow_light_shader.h"
#include "ge_direct_draw_on_canvas_pass.h"
#include "ge_direction_light_shader_filter.h"
#include "ge_dispersion_shader_filter.h"
#include "ge_displacement_distort_shader_filter.h"
#include "ge_edge_light_shader_filter.h"
#include "ge_external_dynamic_loader.h"
#include "ge_filter_composer.h"
#include "ge_frosted_glass_shader_filter.h"
#include "ge_grey_shader_filter.h"
#include "ge_grid_warp_shader_filter.h"
#include "ge_hps_build_pass.h"
#include "ge_hps_effect_filter.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_linear_gradient_blur_shader_filter.h"
#include "ge_log.h"
#include "ge_magnifier_shader_filter.h"
#include "ge_mask_transition_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_mesa_fusion_pass.h"
#include "ge_particle_circular_halo_shader.h"
#include "ge_sound_wave_filter.h"
#include "ge_system_properties.h"
#include "ge_variable_radius_blur_shader_filter.h"
#include "ge_visual_effect_impl.h"
#include "ge_water_ripple_filter.h"
#include "ge_wavy_ripple_light_shader.h"
#include "sdf/ge_sdf_border_shader.h"
#include "sdf/ge_sdf_clip_shader.h"
#include "sdf/ge_sdf_shader_filter.h"
#include "sdf/ge_sdf_shadow_shader.h"

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
            out = std::make_shared<GEAuroraNoiseShader>(*params);
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
    {GEVisualEffectImpl::FilterType::SDF_BORDER, [] (std::shared_ptr<GEVisualEffectImpl> ve) {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                return out;
            }
            const auto& params = ve->GetSDFBorderShaderParams();
            out = std::make_shared<GESDFBorderShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::SDF_SHADOW, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                return out;
            }
            const auto& params = ve->GetSDFShadowShaderParams();
            out = std::make_shared<GESDFShadowShader>(*params);
            return out;
        }
    },
    {GEVisualEffectImpl::FilterType::SDF_CLIP, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr) {
                LOGE("GERender::GenerateShaderEffect GEVisualEffectImpl is null");
                return out;
            }
            const auto& params = ve->GetSDFClipShaderParams();
            out = std::make_shared<GESDFClipShader>(*params);
        return out;
        }
    },
    {GEVisualEffectImpl::FilterType::CIRCLE_FLOWLIGHT, [] (std::shared_ptr<GEVisualEffectImpl> ve)
        {
            std::shared_ptr<GEShader> out = nullptr;
            if (ve == nullptr || ve->GetCircleFlowlightEffectParams() == nullptr) {
                return out;
            }
            const auto& params = ve->GetCircleFlowlightEffectParams();
            out = std::make_shared<GECircleFlowlightEffect>(*params);
            return out;
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
    for (auto& vef: veContainer.GetFilters()) {
        ShaderFilterEffectContext context {resImage, src, dst};
        ProcessShaderFilter(canvas, vef, resImage, context);
    }

    return resImage;
}

bool GERender::BeforeApplyShaderFilter(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::GEVisualEffect>& visualEffect, const ShaderFilterEffectContext& context,
    std::shared_ptr<GEShaderFilter>& geShaderFilter)
{
    if (visualEffect == nullptr) {
        LOGD("GERender::BeforeApplyShaderFilter visualEffect is null");
        return false;
    }
    auto ve = visualEffect->GetImpl();
    geShaderFilter = GenerateShaderFilter(visualEffect);
    if (geShaderFilter == nullptr) {
        LOGD("GERender::BeforeApplyShaderFilter geShaderFilter is null");
        return false;
    }
    geShaderFilter->SetSupportHeadroom(visualEffect->GetSupportHeadroom());
    geShaderFilter->SetCache(ve->GetCache());
    geShaderFilter->Preprocess(canvas, context.src, context.dst);
    return true;
}

bool GERender::AfterApplyShaderFilter(Drawing::Canvas& canvas, const Drawing::GEVisualEffect& visualEffect,
    const ShaderFilterEffectContext& context, const GEShaderFilter& geShaderFilter)
{
    // Update information after executing the shader filter
    auto ve = visualEffect.GetImpl();
    ve->SetCache(geShaderFilter.GetCache());
    if (ve->GetFilterType() == Drawing::GEVisualEffectImpl::FilterType::GASIFY_SCALE_TWIST) {
        isGasifyFilter_ = true;
    }
    return true;
}

GERender::ApplyShaderFilterTarget GERender::DrawShaderFilter(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::GEVisualEffect> visualEffect, Drawing::Brush& brush,
    const ShaderFilterEffectContext& context)
{
    std::shared_ptr<GEShaderFilter> geShaderFilter;
    if (!BeforeApplyShaderFilter(canvas, visualEffect, context, geShaderFilter)) {
        LOGD("GERender::DrawShaderFilter failed before executing shader filter");
        return ApplyShaderFilterTarget::Error;
    }
    // When BeforeApplyShaderFilter returning true, geShaderFilter and visualEffect is guranteed not nullptr
    bool status = geShaderFilter->DrawImage(canvas, context.image, context.src, context.dst, brush);
    if (!status) {
        return ApplyShaderFilterTarget::Error;
    }
    if (!AfterApplyShaderFilter(canvas, *visualEffect, context, *geShaderFilter)) {
        return ApplyShaderFilterTarget::Error;
    }
    return ApplyShaderFilterTarget::DrawOnCanvas;
}

GERender::ApplyShaderFilterTarget GERender::ProcessShaderFilter(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::GEVisualEffect> visualEffect, std::shared_ptr<Drawing::Image>& resImage,
    const ShaderFilterEffectContext& context)
{
    std::shared_ptr<GEShaderFilter> geShaderFilter;
    if (!BeforeApplyShaderFilter(canvas, visualEffect, context, geShaderFilter)) {
        return ApplyShaderFilterTarget::Error;
    }
    // When BeforeApplyShaderFilter returning true, geShaderFilter and visualEffect is guranteed not nullptr
    resImage = geShaderFilter->ProcessImage(canvas, resImage, context.src, context.dst);
    if (!AfterApplyShaderFilter(canvas, *visualEffect, context, *geShaderFilter)) {
        return ApplyShaderFilterTarget::Error;
    }
    return ApplyShaderFilterTarget::DrawOnImage;
}

// Internal helper for composing / transforming effects with GEFilterComposer.
// Add passes in this function if needed.
// Used in ApplyHpsGEImageEffect only.
static bool ComposeEffects(Drawing::Canvas& canvas,
    const std::vector<std::shared_ptr<GEVisualEffect>>& visualEffects,
    const GERender::HpsGEImageEffectContext& context, std::vector<GEFilterComposable>& composables)
{
    if (visualEffects.empty()) {
        return false;
    }
    GEFilterComposer composer;
    composer.Add<GEHpsBuildPass>(canvas, context);
    composer.Add<GEMesaFusionPass>();
    composer.Add<GEDirectDrawOnCanvasPass>();
    composables = GEFilterComposer::BuildComposables(visualEffects);
    auto composerResult = composer.Run(composables);
    if (!composerResult.anyPassChanged) { // Compatiblity fallback when no change applied to composables
        return false;
    }
    return true;
}

GERender::ApplyHpsGEResult GERender::ApplyHpsGEImageEffect(Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, const HpsGEImageEffectContext& context,
    std::shared_ptr<Drawing::Image>& outImage, Drawing::Brush& brush)
{
    if (!context.image) {
        LOGE("GERender::ApplyHpsGEImageEffect image is null");
        return ApplyHpsGEResult::CanvasNotDrawnAndHpsNotApplied();
    }
    const auto& visualEffects = veContainer.GetFilters();
    std::vector<GEFilterComposable> composables;
    if (!ComposeEffects(canvas, visualEffects, context, composables)) {
        return ApplyHpsGEResult::CanvasNotDrawnAndHpsNotApplied();
    }
    auto resImage = context.image;
    bool appliedHpsBlur = false;
    bool lastAppliedHpsBlur = false;
    ApplyShaderFilterTarget applyTarget = ApplyShaderFilterTarget::Error; // Last applied target
    for (auto& composable: composables) {
        auto currentImage = resImage;
        if (auto visualEffect = composable.GetEffect(); visualEffect != nullptr) {
            ShaderFilterEffectContext geContext { resImage, context.src, context.dst };
            applyTarget = DispatchGEShaderFilter(canvas, brush, composable, visualEffect, geContext);
        } else if (auto hpsEffect = composable.GetHpsEffect(); hpsEffect != nullptr) {
            HpsEffectFilter::HpsEffectContext hpsEffectContext = {
                context.alpha, context.colorFilter, context.maskColor};
            lastAppliedHpsBlur = hpsEffect->ApplyHpsEffect(canvas, currentImage, resImage, hpsEffectContext);
            appliedHpsBlur |= lastAppliedHpsBlur;
            applyTarget =
                lastAppliedHpsBlur ? ApplyShaderFilterTarget::DrawOnCanvas : ApplyShaderFilterTarget::DrawOnImage;
        } else {
            LOGE("GERender::ApplyHpsGEImageEffect unhandled composable type");
        }
    }

    outImage = resImage;
    return { applyTarget == ApplyShaderFilterTarget::DrawOnCanvas, appliedHpsBlur }; // canvas drawn & applied hps blur
}

GERender::ApplyShaderFilterTarget GERender::DispatchGEShaderFilter(Drawing::Canvas& canvas, Drawing::Brush& brush,
    GEFilterComposable& composable, std::shared_ptr<Drawing::GEVisualEffect>& visualEffect,
    ShaderFilterEffectContext& geContext)
{
    ApplyShaderFilterTarget applyTarget;
    // Enabled direct drawing on canvas
    if (DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(composable)) {
        applyTarget = DrawShaderFilter(canvas, visualEffect, brush, geContext);
        if (applyTarget == ApplyShaderFilterTarget::DrawOnCanvas) {
            return applyTarget;
        }
    }
    // Compatibility issue: dst assigned with src is a legacy issue when RSDrawingFilter calls
    // geRender->ApplyImageEffect(). When the issue is resolved, please remove this line.
    geContext.dst = geContext.src;
    // Direct drawing on canvas is disabled / not supported / failed, fallback to ProcessShaderFilter
    applyTarget = ProcessShaderFilter(canvas, visualEffect, geContext.image, geContext);
    return applyTarget;
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
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY_SCALE_TWIST: {
            const auto& gasifyScaleTwistParams = ve->GetGasifyScaleTwistFilterParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type),
                sizeof(Drawing::GEGasifyScaleTwistFilterParams),
                static_cast<void*>(gasifyScaleTwistParams.get()));
            if (!object) {
                return nullptr;
            }
            std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter *>(object));
            return dmShader;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY_BLUR: {
            const auto& gasifyBlurParams = ve->GetGasifyBlurFilterParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type),
                sizeof(Drawing::GEGasifyBlurFilterParams),
                static_cast<void*>(gasifyBlurParams.get()));
            if (!object) {
                return nullptr;
            }
            std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter *>(object));
            return dmShader;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY: {
            const auto& gasifyParams = ve->GetGasifyFilterParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(type),
                sizeof(Drawing::GEGasifyFilterParams),
                static_cast<void*>(gasifyParams.get()));
            if (!object) {
                return nullptr;
            }
            std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter *>(object));
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
        case Drawing::GEVisualEffectImpl::FilterType::FROSTED_GLASS: {
            const auto &params = ve->GetFrostedGlassParams();
            auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::FROSTED_GLASS),
                sizeof(Drawing::GEFrostedGlassShaderFilterParams),
                static_cast<void *>(params.get()));
            if (!object) {
                return std::make_shared<GEFrostedGlassShaderFilter>(*params);
            }
            std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter *>(object));
            return dmShader;
            break;
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
        }
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY_SCALE_TWIST: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY_BLUR: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GASIFY: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::FROSTED_GLASS: {
            shaderFilter = GenerateExtShaderFilter(ve);
            break;
        }
        case Drawing::GEVisualEffectImpl::FilterType::GRID_WARP: {
            const auto &params = ve->GetGridWarpFilterParams();
            shaderFilter = std::make_shared<GEGridWarpShaderFilter>(*params);
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

bool GERender::IsGasifyFilter()
{
    if (isGasifyFilter_) {
        isGasifyFilter_ = false;
        return true;
    }
    return false;
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
