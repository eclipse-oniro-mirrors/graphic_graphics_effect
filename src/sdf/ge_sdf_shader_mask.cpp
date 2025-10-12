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

#include "sdf/ge_sdf_shader_mask.h"
#include "sdf/ge_sdf_rrect_shader_mask.h"
#include "sdf/ge_sdf_union_op_shader_mask.h"

#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffect> GESDFShaderMask::GenerateDrawingShader(float width, float height) const
{
    // currently we only support sdf filter
    return nullptr;
}

std::shared_ptr<ShaderEffect> GESDFShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    // currently we only support sdf filter
    return nullptr;
}

void GESDFShaderMask::CopyState(const GESDFShaderMask& mask)
{
    if (GetSDFMaskType() == GESDFMaskType::RRECT) {
        const auto* rectMask = static_cast<const GESDFRRectShaderMask*>(&mask);
        auto* thisRect = static_cast<GESDFRRectShaderMask*>(this);
        thisRect->CopyState(*rectMask);
    } else if (GetSDFMaskType() == GESDFMaskType::UNION_OP) {
        const auto* unionMask = static_cast<const GESDFUnionOpShaderMask*>(&mask);
        auto* thisUnion = static_cast<GESDFUnionOpShaderMask*>(this);
        thisUnion->CopyState(*unionMask);
    }
}

} // Drawing
} // namespace Rosen
} // namespace OHOS
