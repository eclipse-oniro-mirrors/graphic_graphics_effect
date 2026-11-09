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

#ifndef GRAPHICS_EFFECT_GE_SDF_UBER_SHADER_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_UBER_SHADER_SHAPE_H

#include <cmath>
#include <vector>

#include "ge_filter_type_info.h"
#include "ge_log.h"
#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class GESDFUberType : int {
    NONE = 0,
    BASIC_SHAPE_UBER = 1,
    PIXELMAP_ATLAS_UBER = 2,
};

constexpr int BASIC_SHAPE_LEAF_MIN = 7;
constexpr int PIXELMAP_UBER_LEAF_MIN = 2;
constexpr int BASIC_SHAPE_LEAF_MAX = 30;
constexpr int PIXELMAP_UBER_LEAF_MAX = 6;

constexpr int UBER_SHADER_VARIANT_COUNT = 4;
constexpr int UBER_VARIANT_NORMAL_OFFSET = 2;
constexpr int UBER_VARIANT_SMOOTH_OFFSET = 1;

enum GESDFUberOffset : int {
    UBER_OFFSET_1 = 1,
    UBER_OFFSET_2 = 2,
    UBER_OFFSET_3 = 3,
    UBER_OFFSET_4 = 4,
    UBER_OFFSET_5 = 5,
    UBER_OFFSET_6 = 6,
    UBER_OFFSET_7 = 7,
};

enum class GEUberBasicShapeType : int {
    G1_UNUNIFORM_RRECT = 0, // non-uniform or elliptical per-corner radii: general per-corner path
    G1_UNIFORM_RRECT = 1,   // uniform circular radii in the G1 mid region (between the G2 bound and the capsule clamp)
    G2_RRECT = 2,           // minWH >= 2 * UBER_MAX_CURVE_X * radius: continuous-eligible rounded rect
    V_CAPSULE = 3,          // vertical stadium (radius = width/2, height >= width)
    H_CAPSULE = 4,          // horizontal stadium (radius = height/2, width > height)
    TRIANGLE = 5,
    ELLIPSE = 6,
};

constexpr float UBER_MAX_CURVE_X = 1.7f;

struct GESDFUberLeafInfo {
    Drawing::Matrix parentToLocal = Drawing::Matrix();
};

struct GESDFUberTreeDesc {
    bool opResolved = false;
    bool isSmoothUnion = false;
    float spacing = 0.f;
    int leafCount = 0;
};

struct GESDFUberLeafPos {
    std::shared_ptr<GESDFShaderShape> leaf;
    Drawing::Matrix parentToLocal = Drawing::Matrix();
};

class GE_EXPORT GESdfUberShaderShape : public GESDFShaderShape {
public:
    using GESDFShaderShape::GenerateDrawingShader;
    using GESDFShaderShape::GenerateDrawingShaderHasNormal;
    GESdfUberShaderShape(const GESdfUberShaderShapeParams& params) : params_(params) {}
    GESdfUberShaderShape(const GESdfUberShaderShape&) = delete;
    GESdfUberShaderShape& operator=(const GESdfUberShaderShape&) = delete;
    ~GESdfUberShaderShape() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GESdfUberShaderShape, Drawing::GESdfUberShaderShapeParams);

    static bool WalkUberLeaves(const std::shared_ptr<GESDFShaderShape>& shape, const Drawing::Matrix& acc,
        GESDFUberTreeDesc& desc, std::vector<GESDFUberLeafPos>& leaves);

    static GEUberBasicShapeType ClassifyUberRRect(const GERRect& rrect);

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::UBER;
    }

    bool HasType(const GESDFShapeType type) const override;

    const std::shared_ptr<GESDFShaderShape>& GetRootShape() const
    {
        return params_.rootShape;
    }

    GESDFUberType GetUberType() const
    {
        return static_cast<GESDFUberType>(params_.uberType);
    }

    void CopyState(const GESdfUberShaderShape& shape)
    {
        params_ = shape.params_;
    }

    static bool IsAffineTransform(const Drawing::Matrix& matrix)
    {
        constexpr float AFFINE_EPSILON = 1e-6f;
        const float persp0 = matrix.Get(Drawing::Matrix::Index::PERSP_0);
        const float persp1 = matrix.Get(Drawing::Matrix::Index::PERSP_1);
        const float persp2 = matrix.Get(Drawing::Matrix::Index::PERSP_2);
        if (std::abs(persp0) > AFFINE_EPSILON || std::abs(persp1) > AFFINE_EPSILON ||
            std::abs(persp2 - 1.0f) > AFFINE_EPSILON) {
            GE_LOGD("IsAffineTransform: non-affine transform, persp=(%{public}f, %{public}f, %{public}f)",
                persp0, persp1, persp2);
            return false;
        }
        return true;
    }

protected:
    virtual std::shared_ptr<ShaderEffect> GenerateUberShader(float width, float height, bool hasNormal) const;

    static bool IsLeafCountInRange(int count, int leafMax, int leafMin = BASIC_SHAPE_LEAF_MIN)
    {
        if (count < leafMin || count > leafMax) {
            // 2D pre-filters with the same constants, so this is a defensive fallback gate.
            GE_LOGD("IsLeafCountInRange: leaf count %{public}d out of range [%{public}d, %{public}d]",
                count, leafMin, leafMax);
            return false;
        }
        return true;
    }

private:
    GESdfUberShaderShapeParams params_ {};
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_UBER_SHADER_SHAPE_H
