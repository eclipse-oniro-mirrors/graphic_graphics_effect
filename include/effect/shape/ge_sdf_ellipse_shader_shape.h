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

#ifndef GRAPHICS_EFFECT_GE_SDF_ELLIPSE_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_ELLIPSE_SHAPE_H

#include "core/ge_filter_type_info.h"
#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GESDFEllipseShaderShape : public GESDFShaderShape {
public:
    DECLARE_GEFILTER_TYPEFUNC(GESDFEllipseShaderShape, Drawing::GESDFEllipseShapeParams);
    GESDFEllipseShaderShape(const GESDFEllipseShapeParams& param) : params_(param) {}
    GESDFEllipseShaderShape(const GESDFEllipseShaderShape&) = delete;
    virtual ~GESDFEllipseShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::ELLIPSE;
    }

    bool TryGetCenter(float& outX, float& outY) const override;

    const Vector2f& GetCenter() const
    {
        return params_.center;
    }

    const Vector2f& GetRadius() const
    {
        return params_.radius;
    }

    void CopyState(const GESDFEllipseShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override
    {
        return type == GESDFShapeType::ELLIPSE;
    }

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFEllipseShaderShapeBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFEllipseNormalShapeBuilder() const;
    std::shared_ptr<ShaderEffect> GenerateShaderEffect(std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    GESDFEllipseShapeParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_ELLIPSE_SHAPE_H
