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

// Additional includes for complex masks:
// #include <chrono>
// #include "ge_trace.h"
// #include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Shader code (inline static for simple effects, or in GetShaderBuilder() for complex effects)
inline static const std::string g_shaderString$CLASS_NAME = R"(
    uniform vec2 iResolution;

    half4 main(vec2 fragCoord)
    {
        vec2 uv = fragCoord.xy / iResolution.xy;

        // TODO: Implement your mask logic here
        return half4(1.0);  // Default: fully opaque mask
    }
)";

$CLASS_NAME::$CLASS_NAME(const $PARAMS_CLASS& params)
$INITIALIZATION_LIST
{
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShader(float width, float height) const
{
    // Simple mask: Use common implementation or direct shader creation
    // return MakeCommonMask(width, height);

    // Complex mask: Use RuntimeShaderBuilder
    auto builder = GetShaderBuilder();
    if (!builder) {
        LOGE("$CLASS_NAME::GenerateDrawingShader builder error");
        return nullptr;
    }

    builder->SetUniform("iResolution", width, height);

    // TODO: Set additional uniforms based on your parameters
    // builder->SetUniform("parameterName", value);

    auto maskEffectShader = builder->MakeShader(nullptr, false);
    if (!maskEffectShader) {
        LOGE("$CLASS_NAME::GenerateDrawingShader effect error");
    }

    return maskEffectShader;
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShaderHasNormal(float width, float height) const
{
    // For simple masks without normal support, return same as GenerateDrawingShader
    // return GenerateDrawingShader(width, height);

    // For masks with normal support (e.g., for SDF effects)
    auto builder = GetNormalShaderBuilder();
    if (!builder) {
        LOGE("$CLASS_NAME::GenerateDrawingShaderHasNormal builder error");
        return nullptr;
    }

    builder->SetUniform("iResolution", width, height);

    // TODO: Set additional uniforms based on your parameters
    // builder->SetUniform("parameterName", value);

    auto maskEffectShader = builder->MakeShader(nullptr, false);
    if (!maskEffectShader) {
        LOGE("$CLASS_NAME::GenerateDrawingShaderHasNormal effect error");
    }

    return maskEffectShader;
}

std::shared_ptr<RuntimeShaderBuilder> $CLASS_NAME::GetShaderBuilder() const
{
    static const std::shared_ptr<Drawing::RuntimeEffect> s_effect =
        Drawing::RuntimeEffect::CreateForShader(g_shaderString$CLASS_NAME);

    if (s_effect == nullptr) {
        LOGE("$CLASS_NAME::GetShaderBuilder effect error");
        return nullptr;
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(s_effect);
}

// Optional: Separate builder for normal shader (if different from standard shader)
// std::shared_ptr<RuntimeShaderBuilder> $CLASS_NAME::GetNormalShaderBuilder() const
// {
//     static const std::string g_normalShaderString = R"(
//         // Normal-specific shader code
//     )";
//
//     static const std::shared_ptr<Drawing::RuntimeEffect> g_effect =
//         Drawing::RuntimeEffect::CreateForShader(g_normalShaderString);
//
//     if (g_effect == nullptr) {
//         LOGE("$CLASS_NAME::GetNormalShaderBuilder effect error");
//         return nullptr;
//     }
//
//     return std::make_shared<Drawing::RuntimeShaderBuilder>(g_effect);
// }

} // namespace Drawing
} // namespace Rosen
} // // namespace OHOS
