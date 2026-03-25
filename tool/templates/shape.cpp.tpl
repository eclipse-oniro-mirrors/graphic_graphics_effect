/*
 * Copyright (c) $YEAR Huawei Device Co., Ltd.
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

#include "$HEADER_FILE"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

$CLASS_NAME::$CLASS_NAME(const $PARAMS_CLASS& params)
$INITIALIZATION_LIST
{
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShader(float width, float height) const
{
    // TODO: Implement your shader generation here
    return nullptr;
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShaderHasNormal(float width, float height) const
{
    // TODO: Implement your shader generation with normal here
    return nullptr;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
