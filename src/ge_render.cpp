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
#include "ge_bezier_warp_shader_filter.h"
#include "ge_color_gradient_shader_filter.h"
#include "ge_grey_shader_filter.h"
#include "ge_kawase_blur_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_linear_gradient_blur_shader_filter.h"
#include "ge_hps_effect_filter.h"
#include "ge_log.h"
#include "ge_magnifier_shader_filter.h"
#include "ge_displacement_distort_shader_filter.h"
#include "ge_particle_circular_halo_shader.h"
#include "ge_visual_effect_impl.h"
#include "ge_water_ripple_filter.h"
#include "ge_sound_wave_filter.h"
#include "ge_external_dynamic_loader.h"
#include "ge_edge_light_shader_filter.h"
#include "ge_content_light_shader_filter.h"
#include "ge_contour_diagonal_flow_light_shader.h"


namespace OHOS {
namespace GraphicsEffectEngine {

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
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters; // = GenerateShaderFilters(veContainer);
    auto resImage = image;
    for (auto vef : veContainer.GetFilters()) {
        if (vef == nullptr) {
            LOGD("GERender::ApplyImageEffect vef is null");
            continue;
        }
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShaderFilter> geShaderFilter = GenerateShaderFilter(ve);
        if (geShaderFilter == nullptr) {
            LOGD("GERender::ApplyImageEffect filter is null");
            continue;
        }
        geShaderFilter->SetCache(ve->GetCache());
        geShaderFilter->Preprocess(canvas, src, dst);
        resImage = geShaderFilter->ProcessImage(canvas, resImage, src, dst);
        ve->SetCache(geShaderFilter->GetCache());
    }

    return resImage;
}

// true represent Draw Kawase or Mesa succ, false represent Draw Kawase or Mesa false or no Kawase and Mesa
bool GERender::ApplyHpsImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const std::shared_ptr<Drawing::Image>& image, std::shared_ptr<Drawing::Image>& outImage, const Drawing::Rect& src,
    const Drawing::Rect& dst, Drawing::Brush& brush)
{
    auto hpsEffectFilter = std::make_shared<HpsEffectFilter>(canvas);
    if (!image) {
        LOGE("GERender::ApplyImageEffect image is null");
        return false;
    }
    std::vector<std::shared_ptr<GEShaderFilter>> geShaderFilters; // = GenerateShaderFilters(veContainer);

    if (veContainer.GetFilters().empty()) {
        return false;
    }

    if (hpsEffectFilter->HpsSupportEffectGE(veContainer)) {
        for (auto vef : veContainer.GetFilters()) {
            auto ve = vef->GetImpl();
            hpsEffectFilter->GenerateVisualEffectFromGE(ve, src, dst);
        }

        return hpsEffectFilter->ApplyHpsEffect(canvas, image, outImage, brush);
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
                return nullptr;
            }
            std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter*>(object));
            return dmShader;
        }
        default:
            break;
    }
    return nullptr;
}

std::shared_ptr<GEShaderFilter> GERender::GenerateShaderFilter(
    const std::shared_ptr<Drawing::GEVisualEffectImpl>& ve)
{
    std::shared_ptr<GEShaderFilter> shaderFilter;
    LOGD("GERender::GenerateShaderFilter %{public}d", (int)ve->GetFilterType());
    switch (ve->GetFilterType()) {
        case Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR: {
            const auto& kawaseParams = ve->GetKawaseParams();
            shaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(*kawaseParams);
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
        default:
            break;
    }
    return shaderFilter;
}

std::vector<std::shared_ptr<GEShaderFilter>> GERender::GenerateShaderFilters(
    Drawing::GEVisualEffectContainer& veContainer)
{
    LOGD("GERender::shaderFilters %{public}d", (int)veContainer.GetFilters().size());
    std::vector<std::shared_ptr<GEShaderFilter>> shaderFilters;
    for (auto vef : veContainer.GetFilters()) {
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShaderFilter> shaderFilter = GenerateShaderFilter(ve);
        if (shaderFilter) {
            shaderFilter->SetShaderFilterCanvasinfo(vef->GetCanvasInfo());
        }
        shaderFilters.push_back(shaderFilter);
    }
    return shaderFilters;
}

void GERender::DrawShaderEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
    const Drawing::Rect& bounds)
{
    std::vector<std::shared_ptr<GEShader>> geShaderEffects = GenerateShaderEffect(veContainer);
    for (auto geShaderEffect : geShaderEffects) {
        if (geShaderEffect == nullptr) {
            LOGD("GERender::DrawShaderEffect shader is null");
            continue;
        }
        geShaderEffect->MakeDrawingShader(bounds, -1.f); // new flow not use progress
        auto shader = geShaderEffect->GetDrawingShader();
        Drawing::Brush brush;
        brush.SetShaderEffect(shader);
        canvas.AttachBrush(brush);
        canvas.DrawRect(bounds);
        canvas.DetachBrush();
    }
}

std::vector<std::shared_ptr<GEShader>> GERender::GenerateShaderEffect(Drawing::GEVisualEffectContainer& veContainer)
{
    LOGD("GERender::shaderEffects %{public}zu", veContainer.GetFilters().size());
    std::vector<std::shared_ptr<GEShader>> shaderEffects;
    for (auto vef : veContainer.GetFilters()) {
        auto ve = vef->GetImpl();
        std::shared_ptr<GEShader> shaderEffect;
        LOGD("GERender::shaderEffects %{public}d", static_cast<int>(ve->GetFilterType()));
        switch (ve->GetFilterType()) {
            case Drawing::GEVisualEffectImpl::FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT: {
                const auto& params = ve->GetContenDiagonalParams();
                shaderEffect = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(*params);
                shaderEffects.push_back(shaderEffect);
                return shaderEffects;
            }

            case Drawing::GEVisualEffectImpl::FilterType::WAVY_RIPPLE_LIGHT: {
                const auto& params = ve->GetWavyRippleLightParams();
                (void)params;
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::AURORA_NOISE: {
                const auto& params = ve->GetAuroraNoiseParams();
                (void)params;
                break;
            }
            case Drawing::GEVisualEffectImpl::FilterType::PARTICLE_CIRCULAR_HALO: {
                const auto& params = ve->GetParticleCircularHaloParams();
                (void)params;
                break;
            }
            default:
                break;
        }
        shaderEffects.push_back(shaderEffect);
    }
    return shaderEffects;
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
