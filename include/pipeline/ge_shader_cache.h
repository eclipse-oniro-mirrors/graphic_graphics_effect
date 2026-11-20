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

#ifndef GRAPHICS_EFFECT_GE_SHADER_CACHE_H
#define GRAPHICS_EFFECT_GE_SHADER_CACHE_H

#include <functional>
#include <memory>

#include "draw/canvas.h"
#include "draw/surface.h"
#include "ge_common.h"
#include "ge_frame_cache.h"
#include "ge_visual_effect_container.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace GraphicsEffectEngine {
class GERender;
}
namespace Rosen {

struct OffscreenCanvas {
    std::shared_ptr<Drawing::Surface> surface;
    std::shared_ptr<Drawing::Canvas> canvas;
};

class GE_EXPORT GEShaderCache {
public:
    using OffscreenCanvasCreator = std::function<OffscreenCanvas(
        Drawing::Canvas&, int32_t, int32_t)>;
    static void SetOffscreenCanvasCreator(OffscreenCanvasCreator creator);

    static void DrawWithCache(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
        Drawing::GEVisualEffectContainer& veContainer, const Drawing::Rect& bounds);

private:
    static void DrawPerEffect(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
        Drawing::GEVisualEffectContainer& veContainer, GEFrameCacheStore& cacheStore, const Drawing::Rect& bounds);
    static void DrawContainer(GraphicsEffectEngine::GERender& geRender, Drawing::Canvas& canvas,
        Drawing::GEVisualEffectContainer& veContainer, GEFrameCacheStore& cacheStore, const Drawing::Rect& bounds);
    static uint32_t CalcCanvasHash(Drawing::Canvas& canvas, const Drawing::Rect& bounds);
    static uint32_t ComputeContainerHash(const Drawing::GEVisualEffectContainer& veContainer);
    static void DrawCachedImage(Drawing::Canvas& canvas, const Drawing::Rect& bounds,
        const std::shared_ptr<Drawing::Image>& cachedImage);
    static void DrawCacheDebugOverlay(Drawing::Canvas& canvas, const Drawing::Rect& bounds, bool hit);
    static std::shared_ptr<Drawing::Image> RenderToOffscreen(GraphicsEffectEngine::GERender& geRender,
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::GEVisualEffect>& vef, const Drawing::Rect& bounds);

    static OffscreenCanvasCreator offscreenCanvasCreator_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_CACHE_H
