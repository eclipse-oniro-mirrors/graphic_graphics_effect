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

#ifndef GRAPHICS_EFFECT_GE_SDF_PIXELMAP_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_PIXELMAP_SHAPE_H

#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GESDFPixelmapShaderShape : public GESDFShaderShape {
public:
    GESDFPixelmapShaderShape(const GESDFPixelmapShapeParams& param) : params_(param) {}
    GESDFPixelmapShaderShape(const GESDFPixelmapShaderShape&) = delete;
    virtual ~GESDFPixelmapShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::PIXELMAP;
    }

    const std::shared_ptr<Drawing::Image> GetImage() const
    {
        return params_.image;
    }

    void CopyState(const GESDFPixelmapShaderShape& shape)
    {
        params_ = shape.params_;
    }

private:
    std::shared_ptr<ShaderEffect> GeneratePixelmapShader() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFPixelmapShaderShapeBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFPixelmapNormalShaderShapeBuilder() const;
    std::shared_ptr<ShaderEffect> GenerateSDFShaderEffect(std::shared_ptr<ShaderEffect> pixelmapShader,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    std::shared_ptr<ShaderEffect> GenerateSDFNormalShaderEffect(std::shared_ptr<ShaderEffect> pixelmapShader,
    float width, float height, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    GESDFPixelmapShapeParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_PIXELMAP_SHAPE_H