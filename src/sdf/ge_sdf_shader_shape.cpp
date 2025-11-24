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

#include "sdf/ge_sdf_shader_shape.h"
#include "sdf/ge_sdf_pixelmap_shader_shape.h"
#include "sdf/ge_sdf_rrect_shader_shape.h"
#include "sdf/ge_sdf_union_op_shader_shape.h"

#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShader(float width, float height) const
{
    // currently we only support sdf filter
    return nullptr;
}

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    // currently we only support sdf filter
    return nullptr;
}

void GESDFShaderShape::CopyState(const GESDFShaderShape& shape)
{
    if (GetSDFShapeType() == GESDFShapeType::RRECT) {
        const auto* rectShape = static_cast<const GESDFRRectShaderShape*>(&shape);
        auto* thisRect = static_cast<GESDFRRectShaderShape*>(this);
        thisRect->CopyState(*rectShape);
    } else if (GetSDFShapeType() == GESDFShapeType::UNION_OP) {
        const auto* unionShape = static_cast<const GESDFUnionOpShaderShape*>(&shape);
        auto* thisUnion = static_cast<GESDFUnionOpShaderShape*>(this);
        thisUnion->CopyState(*unionShape);
    } else if (GetSDFShapeType() == GESDFShapeType::PIXELMAP) {
        const auto* pixelmapShape = static_cast<const GESDFPixelmapShaderShape*>(&shape);
        auto* thisPixelmap = static_cast<GESDFPixelmapShaderShape*>(this);
        thisPixelmap->CopyState(*pixelmapShape);
    }
}

} // Drawing
} // namespace Rosen
} // namespace OHOS
