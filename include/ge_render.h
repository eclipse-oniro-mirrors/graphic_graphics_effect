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

#ifndef GRAPHICS_EFFECT_GE_RENDER_H
#define GRAPHICS_EFFECT_GE_RENDER_H

#include <memory>

#include "ge_shader.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GE_EXPORT GERender {
public:
    GERender();
    ~GERender();
    void DrawShaderEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const Drawing::Rect& bounds);

    void DrawImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling);

    std::shared_ptr<Drawing::Image> ApplyImageEffect(Drawing::Canvas& canvas,
        Drawing::GEVisualEffectContainer& veContainer, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling);

    bool ApplyHpsImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const std::shared_ptr<Drawing::Image>& image, std::shared_ptr<Drawing::Image>& outImage,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::Brush& brush);

private:
    std::vector<std::shared_ptr<GEShader>> GenerateShaderEffect(Drawing::GEVisualEffectContainer& veContainer);

    std::shared_ptr<GEShaderFilter> GenerateShaderFilter(const std::shared_ptr<Drawing::GEVisualEffect>& ve);
    std::vector<std::shared_ptr<GEShaderFilter>> GenerateShaderFilters(Drawing::GEVisualEffectContainer& veContainer);

    std::shared_ptr<GEShaderFilter> GenerateExtShaderFilter(const std::shared_ptr<Drawing::GEVisualEffectImpl>&);
};

} // namespace GraphicsEffectEngine
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_RENDER_H
