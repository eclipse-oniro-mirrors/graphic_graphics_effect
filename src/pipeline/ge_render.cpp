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

#include "core/ge_effect_factory.h"
#include "ge_direct_draw_on_canvas_pass.h"
#include "ge_filter_composer.h"
#include "ge_hps_build_pass.h"
#include "ge_hps_effect_filter.h"
#include "ge_hps_upscale_pass.h"
#include "ge_log.h"
#include "ge_mesa_fusion_pass.h"
#include "ge_system_properties.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace GraphicsEffectEngine {
namespace {
constexpr int MAX_IMAGE_DIMENSION = 10000;
} // namespace
#define PROPERTY_MESA_BLUR_ALL_ENABLED "persist.sys.graphic.kawaseDisable"
#ifdef GE_OHOS
bool GERender::isMesablurAllEnable_ = (std::atoi(
    GESystemProperties::GetEventProperty(PROPERTY_MESA_BLUR_ALL_ENABLED).c_str()));
#else
bool GERender::isMesablurAllEnable_ = false;
#endif
using namespace Rosen::Drawing;

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
    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetWidth() > MAX_IMAGE_DIMENSION || imageInfo.GetHeight() > MAX_IMAGE_DIMENSION) {
        LOGE("GERender::DrawImageRect image dimension exceeds limit, max is %{public}d", MAX_IMAGE_DIMENSION);
        return;
    }

    auto resImage = ApplyImageEffect(canvas, veContainer, {image, src, dst}, sampling);
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
    Drawing::GEVisualEffectContainer& veContainer, const ShaderFilterEffectContext& context,
    const Drawing::SamplingOptions& sampling)
{
    if (!context.image) {
        LOGE("GERender::ApplyImageEffect image is null");
        return nullptr;
    }
    auto imageInfo = context.image->GetImageInfo();
    if (imageInfo.GetWidth() > MAX_IMAGE_DIMENSION || imageInfo.GetHeight() > MAX_IMAGE_DIMENSION) {
        LOGE("GERender::ApplyImageEffect image dimension exceeds limit, max is %{public}d", MAX_IMAGE_DIMENSION);
        return nullptr;
    }
    auto resImage = context.image;
    for (auto& vef: veContainer.GetFilters()) {
        ShaderFilterEffectContext innerContext {resImage, context.src, context.dst, context.geCacheProvider};
        ProcessShaderFilter(canvas, vef, resImage, innerContext);
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
    geShaderFilter->SetCacheProvider(context.geCacheProvider);
    geShaderFilter->Preprocess(canvas, context.src, context.dst);
    return true;
}

bool GERender::AfterApplyShaderFilter(Drawing::Canvas& canvas, const Drawing::GEVisualEffect& visualEffect,
    const ShaderFilterEffectContext& context, const GEShaderFilter& geShaderFilter)
{
    // Update information after executing the shader filter
    auto ve = visualEffect.GetImpl();
    ve->SetCache(geShaderFilter.GetCache());
    if (ve->GetFilterType() == Drawing::GEVisualEffectImpl::FilterType::GASIFY ||
        ve->GetFilterType() == Drawing::GEVisualEffectImpl::FilterType::PARTICLE_ABLATION) {
        isNeedExpansionFilter_ = true;
        expansionRect_.SetLeft(context.src.GetLeft());
        expansionRect_.SetTop(context.src.GetTop());
        expansionRect_.SetRight(context.src.GetRight());
        expansionRect_.SetBottom(context.src.GetBottom());
    }
    return true;
}

GERender::ApplyShaderFilterTarget GERender::DrawShaderFilter(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::GEVisualEffect> visualEffect, Drawing::Brush& brush,
    const ShaderFilterEffectContext& context)
{
    std::shared_ptr<GEShaderFilter> geShaderFilter;
    if (!BeforeApplyShaderFilter(canvas, visualEffect, context, geShaderFilter)) {
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
    composer.Add<GEHpsUpscalePass>();
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
    auto currentImage = context.image;
    std::shared_ptr<Drawing::Image> resImage = nullptr;
    bool appliedHpsBlur = false;
    bool lastAppliedHpsBlur = false;
    ApplyShaderFilterTarget applyTarget = ApplyShaderFilterTarget::Error; // Last applied target
    for (auto& composable: composables) {
        resImage = nullptr; // cleared the out variable to determine errors in hps call
        if (auto visualEffect = composable.GetEffect(); visualEffect != nullptr) {
            ShaderFilterEffectContext geContext { currentImage, context.src, context.dst, context.geCacheProvider };
            applyTarget = DispatchGEShaderFilter(canvas, brush, composable, visualEffect, geContext);
            resImage = geContext.image;
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
        if (resImage == nullptr) { // On error early returns
            return ApplyHpsGEResult::CanvasNotDrawnAndHpsNotApplied();
        }
        currentImage = resImage;
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

std::shared_ptr<GEShaderFilter> GERender::GenerateShaderFilter(
    const std::shared_ptr<Drawing::GEVisualEffect>& vef)
{
    auto ve = vef->GetImpl();
    LOGD("GERender::GenerateShaderFilter %{public}d", (int)ve->GetFilterType());

    auto shaderFilter = GEEffectFactory::CreateFilter(ve);
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
    LOGD("GERender::shaderEffects %{public}zu", veContainer.GetFilters().size());
    std::vector<std::shared_ptr<GEShader>> shaderEffects;
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
        geShaderEffect->SetShaderCanvasInfo(vef->GetCanvasInfo());
        geShaderEffect->SetCache(ve->GetCache());
        geShaderEffect->DrawShader(canvas, bounds);
        ve->SetCache(geShaderEffect->GetCache());
    }
}

std::shared_ptr<GEShader> GERender::GenerateShaderEffect(const std::shared_ptr<Drawing::GEVisualEffectImpl>& ve)
{
    return GEEffectFactory::CreateShader(ve);
}

void GERender::SetMesablurAllEnabledByCCM(bool flag)
{
    isMesablurAllEnable_ = isMesablurAllEnable_ || flag;
}

bool GERender::IsNeedExpansionFilter()
{
    if (isNeedExpansionFilter_) {
        isNeedExpansionFilter_ = false;
        return true;
    }
    return false;
}

bool GERender::IsFrostedGlassFilter(Drawing::GEVisualEffectContainer& veContainer)
{
    for (auto& vef: veContainer.GetFilters()) {
        auto ve = vef->GetImpl();
        if (ve->GetFilterType() == Drawing::GEVisualEffectImpl::FilterType::FROSTED_GLASS) {
            return true;
        }
    }
    return false;
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
