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
#ifndef GRAPHICS_EFFECT_GE_CONTENT_LIGHT_FILTER_H
#define GRAPHICS_EFFECT_GE_CONTENT_LIGHT_FILTER_H

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
class GEContentLightFilter : public GEShaderFilter {
public:
    GE_EXPORT GEContentLightFilter(const Drawing::GEContentLightFilterParams& params);
    ~GEContentLightFilter() override = default;

    GE_EXPORT std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

private:
    void GetContentLightEffect();
    static std::shared_ptr<Drawing::RuntimeEffect> contentLightShaderEffect_;
    
    Vector3f lightPosition_;
    Vector4f lightColor_;
    float lightIntensity_;
    Vector3f rotationAngle_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_CONTENT_LIGHT_FILTER_H