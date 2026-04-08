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

// Additional includes for complex shapes:
// #include <chrono>
// #include "ge_trace.h"
// #include "ge_shader.h"
// #include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// SDF shader code with gradient support (for GenerateDrawingShaderHasNormal)
inline static const std::string g_sdfGradProg$CLASS_NAME = R"(
    uniform vec2 centerPos;
    uniform vec2 halfSize;
    uniform float radius;

    const float N_EPS = 1e-6;
    const float N_SCALE = 2048.0;

    vec2 safeNorm(vec2 v)
    {
        return v / max(length(v), N_EPS);
    }

    vec3 sdgShape(vec2 p, vec2 b, float r)
    {
        // TODO: Implement your SDF shape function with gradient
        // Return vec3(distance, gradientX, gradientY)
        return vec3(0.0);
    }

    float EncodeDir(vec2 dir)
    {
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    vec4 main(float2 fragCoord)
    {
        vec2 posFromCenter = fragCoord - centerPos;
        vec3 sdg = sdgShape(posFromCenter, halfSize, radius);
        float packedDir = EncodeDir(posFromCenter);
        return vec4(sdg.yz, packedDir, sdg.x);
    }
)";

// Standard SDF shader code (for GenerateDrawingShader)
inline static const std::string g_shaderString$CLASS_NAME = R"(
    uniform vec2 centerPos;
    uniform vec2 halfSize;
    uniform float radius;

    float sdfShape(vec2 p, vec2 b, float r)
    {
        // TODO: Implement your SDF shape function
        // Return signed distance
        return 0.0;
    }

    half4 main(vec2 fragCoord)
    {
        float sdf = sdfShape(fragCoord, centerPos, halfSize, radius);
        return half4(0, 0, 0, sdf);  // Alpha channel stores SDF
    }
)";

$CLASS_NAME::$CLASS_NAME(const $PARAMS_CLASS& params)
$INITIALIZATION_LIST
{
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShader(float width, float height) const
{
    // 1. Get shader builder
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetShaderBuilder();
    if (!builder) {
        LOGE("$CLASS_NAME::GenerateDrawingShader has builder error");
        return nullptr;
    }

    // 2. Generate shader effect
    return GenerateShaderEffect(builder);
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateDrawingShaderHasNormal(float width, float height) const
{
    // 1. Get shader builder for gradient/normal support
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetNormalShaderBuilder();
    if (!builder) {
        LOGE("$CLASS_NAME::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }

    // 2. Generate shader effect
    return GenerateShaderEffect(builder);
}

std::shared_ptr<RuntimeShaderBuilder> $CLASS_NAME::GetShaderBuilder() const
{
    static const std::shared_ptr<Drawing::RuntimeShader> s_effect =
        Drawing::RuntimeEffect::CreateForShader(g_shaderString$CLASS_NAME);

    if (!s_effect) {
        LOGE("$CLASS_NAME::GetShaderBuilder effect error");
        return nullptr;
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(s_effect);
}

std::shared_ptr<RuntimeShaderBuilder> $CLASS_NAME::GetNormalShaderBuilder() const
{
    static const std::shared_ptr<Drawing::RuntimeShader> s_effect =
        Drawing::RuntimeEffect::CreateForShader(g_sdfGradProg$CLASS_NAME);

    if (!s_effect) {
        LOGE("$CLASS_NAME::GetNormalShaderBuilder effect error");
        return nullptr;
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(s_effect);
}

std::shared_ptr<ShaderEffect> $CLASS_NAME::GenerateShaderEffect(
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("$CLASS_NAME::GenerateShaderEffect builder error");
        return nullptr;
    }

    // TODO: Validate shape parameters
    // if (params_.size < 0.0001f) {
    //     return nullptr;
    // }

    // TODO: Set uniforms based on your shape parameters
    // builder->SetUniform("centerPos", centerX, centerY);
    // builder->SetUniform("halfSize", halfWidth, halfHeight);
    // builder->SetUniform("radius", radius);

    auto shader = builder->MakeShader(nullptr, false);
    if (!shader) {
        LOGE("$CLASS_NAME::GenerateShaderEffect shaderEffect error");
    }

    return shader;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
