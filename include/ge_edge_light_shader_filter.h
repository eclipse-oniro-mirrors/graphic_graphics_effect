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
#ifndef GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H
#define GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H

#include <memory>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

class GEEdgeLightShaderFilter : public GEShaderFilter {
public:
    GE_EXPORT GEEdgeLightShaderFilter(const Drawing::GEEdgeLightShaderFilterParams& params);
    ~GEEdgeLightShaderFilter() override = default;

    GE_EXPORT std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

private:
    float edgeLightEdgeThreshold_ = 0.3f;
    float edgeLightEdgeIntensity_ = 1.0f;
    float edgeLightEdgeSoftThreshold_ = 0.3f;
    float edgeDetectColorR_ = 0.2f;
    float edgeDetectColorG_ = 0.7f;
    float edgeDetectColorB_ = 0.1f;
    float edgeColorR_ = 0.2f;
    float edgeColorG_ = 0.7f;
    float edgeColorB_ = 0.1f;
    bool edgeRawColor_ = true;
    uint32_t edgeBloomLevel_ = 6;
    bool edgeLightIfGradient_ = true;
    float alphaProgress_ = 1.0;
    bool addImage_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H
