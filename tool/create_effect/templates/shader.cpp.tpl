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

#include "$HEADER_FILE"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"

// Additional includes for complex shaders:
// #include <chrono>
// #include "ge_visual_effect_impl.h"
// #include "draw/surface.h"
// #include "image/image.h"

namespace OHOS {
namespace Rosen {

// Shader code (inline static for simple effects, or in GetEffect() for complex effects)
inline static const std::string g_shaderString$CLASS_NAME = R"(
    uniform vec2 iResolution;

    half4 main(vec2 fragCoord)
    {
        vec2 uv = fragCoord.xy / iResolution.xy;

        // TODO: Implement your shader logic here
        return half4(1.0);  // Default: white color
    }
)";

$CLASS_NAME::$CLASS_NAME(const Drawing::$PARAMS_CLASS& params)
{
}

std::shared_ptr<Drawing::RuntimeEffect> $CLASS_NAME::GetEffect()
{
    static const std::shared_ptr<Drawing::RuntimeEffect> s_effect =
        Drawing::RuntimeEffect::CreateForShader(g_shaderString$CLASS_NAME);

    if (s_effect == nullptr) {
        GE_LOGE("$CLASS_NAME::GetEffect effect is nullptr.");
    }

    return s_effect;
}

void $CLASS_NAME::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    // 1. Validate rect
    if (!rect.IsValid()) {
        GE_LOGE("$CLASS_NAME::MakeDrawingShader rect is invalid.");
        return;
    }

    // 2. Get shader effect
    auto effect = GetEffect();
    if (!effect) {
        GE_LOGE("$CLASS_NAME::MakeDrawingShader effect is nullptr.");
        return;
    }

    // 3. Create RuntimeShaderBuilder
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);

    // 4. Set uniforms
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    builder->SetUniform("iResolution", width, height);

    // TODO: Set additional uniforms based on your parameters
    // builder->SetUniform("parameterName", value);
    // builder->SetUniformVec4("colorName", r, g, b, a);

    // 5. Create shader and store in base class member
    drShader_ = builder->MakeShader(nullptr, false);

    if (!drShader_) {
        GE_LOGE("$CLASS_NAME::MakeDrawingShader shader is nullptr.");
    }
}

} // namespace Rosen
} // namespace OHOS
