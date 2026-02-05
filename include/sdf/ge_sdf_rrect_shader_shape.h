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

#ifndef GRAPHICS_EFFECT_GE_SDF_RRECT_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_RRECT_SHAPE_H

#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GESDFRRectShaderShape : public GESDFShaderShape {
public:
    GESDFRRectShaderShape(const GESDFRRectShapeParams& param) : params_(param) {}
    GESDFRRectShaderShape(const GESDFRRectShaderShape&) = delete;
    virtual ~GESDFRRectShaderShape() = default;

    bool GenerateCascadeShaderHasNormal(GESDFCascadeManager& manager, float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::RRECT;
    }

    const GERRect& GetRRect() const
    {
        return params_.rrect;
    }

    void CopyState(const GESDFRRectShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override
    {
        return type == GESDFShapeType::RRECT;
    }

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFRRectShaderShapeBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFRRectNormalShapeBuilder() const;
    std::shared_ptr<ShaderEffect> GenerateShaderEffect(std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    GESDFRRectShapeParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_RRECT_SHAPE_H
