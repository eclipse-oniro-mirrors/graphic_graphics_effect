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

#include "sdf/ge_sdf_union_op_shader_shape.h"
#include "utils/ge_trace.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void GESDFUnionOpShaderShape::CopyState(const GESDFUnionOpShaderShape& shape)
{
    params_.spacing = shape.params_.spacing;
    params_.op = shape.params_.op;
    if (params_.left) {
        params_.left->CopyState(*static_cast<const GESDFShaderShape*>(shape.params_.left.get()));
    }

    if (params_.right) {
        params_.right->CopyState(*static_cast<const GESDFShaderShape*>(shape.params_.right.get()));
    }
}

} // Drawing
} // namespace Rosen
} // namespace OHOS
