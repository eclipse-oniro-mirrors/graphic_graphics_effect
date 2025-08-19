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
#ifndef GRAPHICS_EFFECT_GE_BEZIER_WARP_FILTER_H
#define GRAPHICS_EFFECT_GE_BEZIER_WARP_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch

REGISTER_GEFILTER_TYPEINFO(BEZIER_WARP, GEBezierWarpShaderFilter, 
                           Drawing::GEBezierWarpShaderFilterParams, Drawing::GE_FILTER_BEZIER_WARP);
class GEBezierWarpShaderFilter : public GEShaderFilter {
public:
    GEBezierWarpShaderFilter(const Drawing::GEBezierWarpShaderFilterParams& params);
    ~GEBezierWarpShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEBezierWarpShaderFilter);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    Drawing::Brush GetBrush(const std::shared_ptr<Drawing::Image>& image) const;

private:
    Drawing::GEBezierWarpShaderFilterParams params_;
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> destinationPatch_;
    static const std::string type_;

    void InitCtrlPoints(std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& bezierPatch) const;
    void SetPathTo(Drawing::Path &path, std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& bezierPatch) const;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_BEZIER_WARP_FILTER_H