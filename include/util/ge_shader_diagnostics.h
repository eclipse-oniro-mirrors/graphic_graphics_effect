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

#ifndef GRAPHICS_EFFECT_SHADER_DIAGNOSTICS_H
#define GRAPHICS_EFFECT_SHADER_DIAGNOSTICS_H

#include <memory>
#include <string>

#include "effect/runtime_effect.h"
#include "ge_common.h"
#include "ge_source_location.h"

namespace OHOS {
namespace Rosen {

/**
 * @brief Creates a RuntimeEffect for shader with optional diagnostics tracking.
 *
 * This function wraps Drawing::RuntimeEffect::CreateForShader to provide:
 * - Zero-overhead forwarding when GE_DIAGNOSTICS_DUMP_SHADER_CREATOR is not defined
 * - Shader source hash and source location tracking when diagnostics are enabled
 *
 * @param shaderSrc The shader source code string
 * @param srcLoc Source location captured at call site (default: GESourceLocation::Current())
 * @return std::shared_ptr<Drawing::RuntimeEffect> The created runtime effect, or nullptr on failure
 *
 * @note When GE_DIAGNOSTICS_DUMP_SHADER_CREATOR is defined, this function will:
 *       1. Compute SHA256 digest of the shader source
 *       2. Append digest + source location to CSV (lazy-init on first call)
 *       3. Dump shader source to /data/local/tmp/ge_shader_diagnostics.{hash}.sksl
 */
GE_EXPORT std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc = GESourceLocation::Current());

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_SHADER_DIAGNOSTICS_H