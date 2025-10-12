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

#ifndef GRAPHICS_EFFECT_GE_SDF_FILTER_H
#define GRAPHICS_EFFECT_GE_SDF_FILTER_H

#include <memory>
#include <optional>

#include "ge_shader_filter.h"
#include "ge_shader_filter_params.h"
#include "ge_sdf_tree_processor.h"
#include "ge_visual_effect_impl.h"

#include "common/rs_vector4.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
class GE_EXPORT GESDFShaderFilter : public GEShaderFilter {
public:
    GESDFShaderFilter(const Drawing::GESDFFilterParams& params)
        : sdfTreeProcessor_(
            std::make_optional<Drawing::GESDFTreeProcessor>(params)) {}
    GESDFShaderFilter(const GESDFShaderFilter& params) = delete;
    ~GESDFShaderFilter() override = default;

    std::shared_ptr<Drawing::Image>
    OnProcessImage(Drawing::Canvas& canvas,
                   const std::shared_ptr<Drawing::Image> image,
                   const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string &Type() const; // overload, not override

    Drawing::GESDFTreeProcessor *GetSDFTreeProcessor();
    const Drawing::GESDFTreeProcessor *GetSDFTreeProcessor() const;

    void Update(const Drawing::GESDFFilterParams& params)
    {
        if (sdfTreeProcessor_) {
            sdfTreeProcessor_->UpdateParams(params);
        }
    }
private:
    static const std::string type_;
    std::optional<Drawing::GESDFTreeProcessor> sdfTreeProcessor_;
    std::optional<Drawing::RuntimeShaderBuilder> shaderEffectBuilder_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_FILTER_H
