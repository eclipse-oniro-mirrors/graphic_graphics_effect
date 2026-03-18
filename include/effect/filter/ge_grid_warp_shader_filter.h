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
#ifndef GRAPHICS_EFFECT_GE_GRID_WARP_FILTER_H
#define GRAPHICS_EFFECT_GE_GRID_WARP_FILTER_H

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

enum class LocationType {
    NONE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
};

class GEGridWarpShaderFilter : public GEShaderFilter {
public:
    GEGridWarpShaderFilter(const Drawing::GEGridWarpShaderFilterParams& params);
    ~GEGridWarpShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEGridWarpShaderFilter, Drawing::GEGridWarpShaderFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;
    const std::string GetDescription() const { return "GEGridWarpShaderFilter"; }

private:
    static constexpr size_t GRID_ROW_OR_COL_NUM = 2;
    static constexpr size_t GRID_WARP_MAIN_POINT_NUM = 9;
    static constexpr size_t BEZIER_WARP_POINT_NUM = 12;
    static constexpr size_t GRID_TEXTURE_COORDS_NUM = 4;
    static constexpr size_t GRID_NUM = 4;

    Drawing::GEGridWarpShaderFilterParams params_;
    std::array<Drawing::Point, GRID_WARP_MAIN_POINT_NUM> gridPointsCoords_;
    std::array<Drawing::Point, GRID_WARP_MAIN_POINT_NUM> gridPointsAngles_;
    std::array<std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>, GRID_TEXTURE_COORDS_NUM> bezierPatch_;

    Drawing::Brush GetBrush(const std::shared_ptr<Drawing::Image>& image) const;
    Drawing::Point CalcPointCoord(const int index, const float pointsDistance, LocationType location);
    using BezierPatchArray = std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>;
    BezierPatchArray CalculateBezierPatch(int row, int col, int n, float pointsDistance);
    using GridTextureCoords = std::array<std::array<Drawing::Point, GRID_TEXTURE_COORDS_NUM>, GRID_NUM>;
    GridTextureCoords CalcTexCoords(int imageWidth, int imageHeight);
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_GRID_WARP_FILTER_H