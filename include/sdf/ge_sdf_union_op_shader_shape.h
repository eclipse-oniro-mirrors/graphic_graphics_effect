/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_SDF_UNION_OP_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_UNION_OP_SHAPE_H

#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GESDFUnionOpShaderShape : public GESDFShaderShape {
public:
    GESDFUnionOpShaderShape(const GESDFUnionOpShapeParams& param) : params_(param) {}
    GESDFUnionOpShaderShape(const GESDFUnionOpShaderShape&) = delete;
    virtual ~GESDFUnionOpShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::UNION_OP;
    }

    inline float GetSpacing() const
    {
        return params_.spacing;
    }

    inline const std::shared_ptr<GESDFShaderShape>& GetLeftSDFShape() const
    {
        return params_.left;
    }

    inline const std::shared_ptr<GESDFShaderShape>& GetRightSDFShape() const
    {
        return params_.right;
    }

    inline const GESDFUnionOp& GetSDFUnionOp() const
    {
        return params_.op;
    }

    void CopyState(const GESDFUnionOpShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override;
private:
    std::shared_ptr<ShaderEffect> GenerateUnionOpDrawingShader(std::shared_ptr<ShaderEffect> leftShader,
        std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFUnionBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFSmoothUnionBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFNormalSmoothUnionBuilder() const;
    std::shared_ptr<ShaderEffect> GenerateUnionShaderEffect(
        std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    std::shared_ptr<ShaderEffect> GenerateSmoothUnionShaderEffect(
        std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    GESDFUnionOpShapeParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_UNION_OP_SHAPE_H
