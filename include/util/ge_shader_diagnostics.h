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

#ifdef GE_DIAGNOSTICS_DUMP_SHADER_CREATOR
constexpr const char* GE_SHADER_DIAGNOSTICS_OUT_DIR = "/data/service/el0/render_service/";
#endif

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
 *       2. Atomically write per-hash files using O_CREAT|O_EXCL (no cross-process contention):
 *          - /data/service/el0/render_service/ge_shader_diagnostics.{hash}.csv  (file,function,line,srcLen)
 *          - /data/service/el0/render_service/ge_shader_diagnostics.{hash}.sksl (shader source)
 *       3. If files already exist (same hash from another process), skip writing.
 *          Only the first process to encounter a given shader hash records its source
 *          location; subsequent encounters across processes are silently skipped.
 */
GE_EXPORT std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc = GESourceLocation::Current());

/**
 * @brief Overload that accepts RuntimeEffectOptions.
 *
 * Same diagnostics behavior as the two-parameter version above.
 *
 * @param shaderSrc The shader source code string
 * @param options RuntimeEffectOptions (forceNoInline, useAF, useHighpLocalCoords)
 * @param srcLoc Source location captured at call site (default: GESourceLocation::Current())
 * @return std::shared_ptr<Drawing::RuntimeEffect> The created runtime effect, or nullptr on failure
 */
GE_EXPORT std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(const std::string& shaderSrc,
    const Drawing::RuntimeEffectOptions& options, const GESourceLocation& srcLoc = GESourceLocation::Current());

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_SHADER_DIAGNOSTICS_H