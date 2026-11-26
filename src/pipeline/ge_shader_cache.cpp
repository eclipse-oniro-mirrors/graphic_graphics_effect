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

#include "ge_shader_cache.h"

#include "ge_render.h"
#include "core/ge_effect_factory.h"
#include "ge_log.h"
#include "ge_param_hash_helper.h"
#include "ge_system_properties.h"
#include "ge_trace.h"
#include "ge_visual_effect_impl.h"

#include "draw/canvas.h"
#include "draw/brush.h"
#include "draw/surface.h"
#include "effect/color_space.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

GEShaderCache::OffscreenCanvasCreator GEShaderCache::offscreenCanvasCreator_;

void GEShaderCache::SetOffscreenCanvasCreator(OffscreenCanvasCreator creator)
{
    offscreenCanvasCreator_ = std::move(creator);
}

void GEShaderCache::DrawWithCache(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, const Drawing::Rect& bounds)
{
    auto cacheStore = veContainer.GetCacheStore();
    bool cacheUnavailable =
        !cacheStore || cacheStore->IsCacheIncompatible() || !GESystemProperties::GetShaderFrameCacheEnabled();
    if (cacheUnavailable) {
        for (auto vef : veContainer.GetFilters()) {
            geRender.DrawSingleShaderEffect(canvas, vef, bounds);
        }
        return;
    }
    auto level = cacheStore->GetCacheLevel();
    if (level == GEFrameCacheLevel::PER_EFFECT) {
        DrawPerEffect(geRender, canvas, veContainer, *cacheStore, bounds);
    } else if (level == GEFrameCacheLevel::CONTAINER) {
        DrawContainer(geRender, canvas, veContainer, *cacheStore, bounds);
    } else {
        LOGE("GEShaderCache::DrawWithCache unexpected level %{public}d, fallback to direct draw",
            static_cast<int32_t>(level));
        for (auto vef : veContainer.GetFilters()) {
            geRender.DrawSingleShaderEffect(canvas, vef, bounds);
        }
    }
}

void GEShaderCache::DrawPerEffect(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, GEFrameCacheStore& cacheStore, const Drawing::Rect& bounds)
{
    uint32_t canvasHash = CalcCanvasHash(canvas, bounds);
    const auto& filters = veContainer.GetFilters();
    bool allHit = true;
    for (size_t i = 0; i < filters.size(); i++) {
        const auto& vef = filters[i];
        if (vef == nullptr) {
            continue;
        }
        auto ve = vef->GetImpl();
        if (!ve) {
            continue;
        }
        uint32_t paramHash = ve->Hash();
        auto filterType = ve->GetFilterType();
        std::string cacheKey = GEFrameCacheStore::MakeEffectKey(static_cast<int32_t>(filterType), i);
        auto cache = cacheStore.GetEffectCache(cacheKey);
        const auto& skipPolicy = cacheStore.GetEffectSkipPolicy(cacheKey);
        bool cacheHit = cache && paramHash != 0 &&
            GEIsCacheHit(*cache, paramHash, canvasHash, skipPolicy);
        if (cacheHit) {
            GE_TRACE_ALWAYS_FMT("ShaderCache: perEffect hit, idx=%zu", i);
            DrawCachedImage(canvas, bounds, cache->cachedImage);
            continue;
        }
        allHit = false;
        GE_TRACE_ALWAYS_FMT("ShaderCache: perEffect miss, idx=%zu", i);
        auto offscreenImage = RenderToOffscreen(geRender, canvas, vef, bounds);
        cacheStore.UpdateEffectCache(cacheKey, paramHash, canvasHash,
            offscreenImage, skipPolicy.skipInterval);
        DrawCachedImage(canvas, bounds, offscreenImage);
    }
    DrawCacheDebugOverlay(canvas, bounds, allHit);
}

void GEShaderCache::DrawContainer(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
    Drawing::GEVisualEffectContainer& veContainer, GEFrameCacheStore& cacheStore, const Drawing::Rect& bounds)
{
    uint32_t canvasHash = CalcCanvasHash(canvas, bounds);
    uint32_t paramHash = ComputeContainerHash(veContainer);
    const auto& skipPolicy = cacheStore.GetSkipPolicy();
    bool cacheHit = paramHash != 0 && cacheStore.IsContainerCacheValid() &&
        GEIsCacheHit(cacheStore.GetContainerCacheData(), paramHash, canvasHash, skipPolicy);
    if (cacheHit) {
        GE_TRACE_ALWAYS_FMT("ShaderCache: container hit");
        DrawCachedImage(canvas, bounds, cacheStore.GetContainerCache().cachedImage);
        DrawCacheDebugOverlay(canvas, bounds, true);
        return;
    }
    GE_TRACE_ALWAYS_FMT("ShaderCache: container miss, update");
    int32_t width = static_cast<int32_t>(bounds.GetWidth());
    int32_t height = static_cast<int32_t>(bounds.GetHeight());
    OffscreenCanvas off;
    if (offscreenCanvasCreator_) {
        off = offscreenCanvasCreator_(canvas, width, height);
    }
    bool offscreenReady = off.canvas && off.surface;
    if (!offscreenReady) {
        LOGE("GEShaderCache::DrawContainer offscreen canvas creation failed");
        return;
    }
    off.canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    off.canvas->Translate(-bounds.GetLeft(), -bounds.GetTop());
    for (auto vef : veContainer.GetFilters()) {
        geRender.DrawSingleShaderEffect(*off.canvas, vef, bounds);
    }
    auto offscreenImage = off.surface->GetImageSnapshot();
    cacheStore.UpdateContainerCache(paramHash, canvasHash,
        offscreenImage, skipPolicy.skipInterval);
    DrawCachedImage(canvas, bounds, offscreenImage);
    DrawCacheDebugOverlay(canvas, bounds, false);
}

uint32_t GEShaderCache::CalcCanvasHash(Drawing::Canvas& canvas, const Drawing::Rect& bounds)
{
    int width = static_cast<int>(bounds.GetWidth());
    int height = static_cast<int>(bounds.GetHeight());
    float left = bounds.GetLeft();
    float top = bounds.GetTop();
    uint32_t hash = GEParamHashHelper::HashFields(0, width, height, left, top);
    auto surface = canvas.GetSurface();
    auto imageInfo = surface ? surface->GetImageInfo() : Drawing::ImageInfo();
    hash = GEParamHashHelper::HashValue(static_cast<int32_t>(imageInfo.GetColorType()), hash);
    auto colorSpace = imageInfo.GetColorSpace();
    auto colorData = colorSpace ? colorSpace->Serialize() : nullptr;
    int32_t colorNull = -1;
    hash = colorData ? GEParamHashHelper::Hash(colorData->GetData(), colorData->GetSize(), hash) :
        GEParamHashHelper::HashValue(colorNull, hash);
    return hash;
}

uint32_t GEShaderCache::ComputeContainerHash(const Drawing::GEVisualEffectContainer& veContainer)
{
    uint32_t hash = 0;
    for (const auto& vef : veContainer.GetFilters()) {
        bool valid = vef && vef->GetImpl();
        if (!valid) {
            continue;
        }
        hash = GEParamHashHelper::CombineHash(hash, vef->GetImpl()->Hash());
    }
    return hash;
}

void GEShaderCache::DrawCachedImage(Drawing::Canvas& canvas, const Drawing::Rect& bounds,
    const std::shared_ptr<Drawing::Image>& cachedImage)
{
    if (!cachedImage) {
        LOGE("GEShaderCache::DrawCachedImage cachedImage is nullptr");
        return;
    }
    Drawing::Rect src(0, 0, static_cast<float>(cachedImage->GetWidth()),
        static_cast<float>(cachedImage->GetHeight()));
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*cachedImage, src, bounds, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

void GEShaderCache::DrawCacheDebugOverlay(Drawing::Canvas& canvas, const Drawing::Rect& bounds, bool hit)
{
    if (!GESystemProperties::GetShaderCacheDebugOverlay()) {
        return;
    }
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    constexpr uint8_t OVERLAY_ALPHA = 80;
    // Full RGB channel value for the debug overlay colors (hit=green, miss=red).
    constexpr uint32_t COLOR_CHANNEL_MAX = 255;
    if (hit) {
        brush.SetColor(Drawing::Color(0, COLOR_CHANNEL_MAX, 0, OVERLAY_ALPHA));
    } else {
        brush.SetColor(Drawing::Color(COLOR_CHANNEL_MAX, 0, 0, OVERLAY_ALPHA));
    }
    canvas.AttachBrush(brush);
    canvas.DrawRect(bounds);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::Image> GEShaderCache::RenderToOffscreen(GraphicsEffectEngine::GERender& geRender,
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::GEVisualEffect>& vef, const Drawing::Rect& bounds)
{
    int32_t width = static_cast<int32_t>(bounds.GetWidth());
    int32_t height = static_cast<int32_t>(bounds.GetHeight());
    bool sizeInvalid = width <= 0 || height <= 0;
    if (sizeInvalid) {
        return nullptr;
    }
    OffscreenCanvas off;
    if (offscreenCanvasCreator_) {
        off = offscreenCanvasCreator_(canvas, width, height);
    }
    bool offscreenReady = off.canvas && off.surface;
    if (!offscreenReady) {
        return nullptr;
    }
    off.canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    off.canvas->Translate(-bounds.GetLeft(), -bounds.GetTop());
    geRender.DrawSingleShaderEffect(*off.canvas, vef, bounds);
    return off.surface->GetImageSnapshot();
}

} // namespace Rosen
} // namespace OHOS
