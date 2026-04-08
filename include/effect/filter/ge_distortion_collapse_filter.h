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
#ifndef GRAPHICS_EFFECT_GE_DISTORTION_COLLAPSE_FILTER_H
#define GRAPHICS_EFFECT_GE_DISTORTION_COLLAPSE_FILTER_H

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "effect/shader_effect.h"
#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS::Rosen {
class GE_EXPORT GEDistortionCollapseFilter final : public GEShaderFilter {
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeEffectShader(const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst);

public:
    explicit GEDistortionCollapseFilter(const Drawing::GEDistortionCollapseFilterParams& params);

    GEDistortionCollapseFilter(GEDistortionCollapseFilter&& distortionCollapseFilter) = delete;
    GEDistortionCollapseFilter(const GEDistortionCollapseFilter& distortionCollapseFilter) = delete;

    GEDistortionCollapseFilter& operator=(GEDistortionCollapseFilter&& distortionCollapseFilter) = delete;
    GEDistortionCollapseFilter& operator=(const GEDistortionCollapseFilter& distortionCollapseFilter) = delete;

    DECLARE_GEFILTER_TYPEFUNC(GEDistortionCollapseFilter, Drawing::GEDistortionCollapseFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    Drawing::Point LUCorner_;
    Drawing::Point RUCorner_;
    Drawing::Point RBCorner_;
    Drawing::Point LBCorner_;

    Vector4f barrelDistortion_;
};
} // namespace OHOS::Rosen

#endif // GRAPHICS_EFFECT_GE_DISTORTION_COLLAPSE_FILTER_H