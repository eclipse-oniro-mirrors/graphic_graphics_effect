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
#ifndef GRAPHICS_EFFECT_GE_SDF_SUB_OP_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_SUB_OP_SHAPE_H

#include "ge_filter_type_info.h"
#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GESDFSubOpShaderShape : public GESDFShaderShape {
public:
    using GESDFShaderShape::GenerateDrawingShader;
    using GESDFShaderShape::GenerateDrawingShaderHasNormal;
    DECLARE_GEFILTER_TYPEFUNC(GESDFSubOpShaderShape, Drawing::GESDFSubOpShapeParams);

    GESDFSubOpShaderShape(const GESDFSubOpShapeParams& param) : params_(param) {}
    GESDFSubOpShaderShape(const GESDFSubOpShaderShape&) = delete;
    virtual ~GESDFSubOpShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    void Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal) override;

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::SUB_OP;
    }

    void CopyState(const GESDFSubOpShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override;

protected:
    GESDFSubOpShapeParams params_ {};
    virtual std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFSubOpBuilder(bool hasNormal) const;
    std::shared_ptr<ShaderEffect> GenerateSubOpDrawingShader(
        std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const;
};

class GE_EXPORT GESDFSmoothSubOpShaderShape : public GESDFShaderShape {
public:
    using GESDFShaderShape::GenerateDrawingShader;
    using GESDFShaderShape::GenerateDrawingShaderHasNormal;
    DECLARE_GEFILTER_TYPEFUNC(GESDFSmoothSubOpShaderShape, Drawing::GESDFSmoothSubOpShapeParams);

    GESDFSmoothSubOpShaderShape(const GESDFSmoothSubOpShapeParams& param) : params_(param) {}
    GESDFSmoothSubOpShaderShape(const GESDFSmoothSubOpShaderShape&) = delete;
    virtual ~GESDFSmoothSubOpShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    void Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal) override;

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::SMOOTH_SUB_OP;
    }

    void CopyState(const GESDFSmoothSubOpShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override;

private:
    GESDFSmoothSubOpShapeParams params_ {};
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFSmoothSubOpBuilder(bool hasNormal) const;
    std::shared_ptr<ShaderEffect> GenerateSmoothSubOpDrawingShader(
        std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_SUB_OP_SHAPE_H
