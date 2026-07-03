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
#ifndef GRAPHICS_EFFECT_DEPTH_OCCLUSION_SHADER_FILTER_H
#define GRAPHICS_EFFECT_DEPTH_OCCLUSION_SHADER_FILTER_H

#include <memory>

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEDepthOcclusionShaderFilter : public GEShaderFilter {
public:
    GEDepthOcclusionShaderFilter(const Drawing::GEDepthOcclusionShaderFilterParams& params) : params_(params) {}
    ~GEDepthOcclusionShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEDepthOcclusionShaderFilter, Drawing::GEDepthOcclusionShaderFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    Drawing::GEDepthOcclusionShaderFilterParams params_;

    std::shared_ptr<Drawing::RuntimeEffect> MakeDepthOcclusionMaskShaderEffect();
    std::shared_ptr<Drawing::Image> GetOcclusionMask(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src);
    std::shared_ptr<Drawing::RuntimeEffect> MakeDepthOcclusionShaderEffect();
    bool IsInputImageValid(const std::shared_ptr<Drawing::Image>& image);
    bool IsInputParamsValid(const Drawing::GEDepthOcclusionShaderFilterParams& params);
    std::shared_ptr<Drawing::Image> GetOutputOnFailure(const std::shared_ptr<Drawing::Image>& input);
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_DEPTH_OCCLUSION_SHADER_FILTER_H