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

#ifndef GRAPHICS_EFFECT_GE_DIRECTION_LIGHT_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_DIRECTION_LIGHT_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

REGISTER_GEFILTER_TYPEINFO(DIRECTION_LIGHT, GEDirectionLightShaderFilter,
                           Drawing::GEDirectionLightShaderFilterParams);
class GE_EXPORT GEDirectionLightShaderFilter : public GEShaderFilter {
public:
    GEDirectionLightShaderFilter(const Drawing::GEDirectionLightShaderFilterParams& params);
    ~GEDirectionLightShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEDirectionLightShaderFilter);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

    const std::string& Type() const override;

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetNormalMaskEffect();
    std::shared_ptr<Drawing::RuntimeEffect> GetDirectionLightEffect();
    std::shared_ptr<Drawing::RuntimeEffect> GetDirectionLightNoNormalEffect();
    Drawing::GEDirectionLightShaderFilterParams params_;
    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_DIRECTION_LIGHT_SHADER_FILTER_H