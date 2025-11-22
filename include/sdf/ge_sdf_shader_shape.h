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

#ifndef GRAPHICS_EFFECT_GE_SDF_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_SHAPE_H

#include "ge_shader_shape.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class GESDFShapeType : uint8_t {
    UNION_OP = 0,
    RRECT,
    MAX = RRECT,
};

class GE_EXPORT GESDFShaderShape : public GEShaderShape {
public:
    GESDFShaderShape() = default;
    GESDFShaderShape(const GESDFShaderShape&) = delete;
    virtual ~GESDFShaderShape() = default;

    virtual std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    virtual std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

    virtual GESDFShapeType GetSDFShapeType() const = 0;

    void CopyState(const GESDFShaderShape& shape);
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_SHAPE_H
