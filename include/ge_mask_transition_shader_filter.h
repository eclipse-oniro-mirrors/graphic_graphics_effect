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

#ifndef GRAPHICS_EFFECT_GE_MASK_TRANSITION_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_MASK_TRANSITION_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
REGISTER_GEFILTER_TYPEINFO(MASK_TRANSITION, GEMaskTransitionShaderFilter,
                           Drawing::GEMaskTransitionShaderFilterParams, Drawing::GE_FILTER_MASK_TRANSITION);
class GEMaskTransitionShaderFilter : public GEShaderFilter {
public:
    GE_EXPORT GEMaskTransitionShaderFilter(const Drawing::GEMaskTransitionShaderFilterParams& params);
    ~GEMaskTransitionShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEMaskTransitionShaderFilter);
    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetMaskTransitionEffect();
    Drawing::GEMaskTransitionShaderFilterParams params_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_MASK_TRANSITION_SHADER_FILTER_H