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

#include <chrono>

#include "ge_log.h"
#include "ge_radial_gradient_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr static uint8_t SIZE_ARRAY = 12;  // 12 max length
// max radius should equals height of image, 2.0 is a scale from diameter to radius.
constexpr static float RADIUS_SCALE = 2.0f;
} // namespace

GERadialGradientShaderMask::GERadialGradientShaderMask(GERadialGradientShaderMaskParams param) : param_(param) {}

std::shared_ptr<ShaderEffect> GERadialGradientShaderMask::GenerateDrawingShader(float width, float height) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetRadialGradientShaderMaskBuilder();
    if (!builder) {
        LOGE("GERadialGradientShaderMask::GenerateDrawingShaderHas builder error");
        return nullptr;
    }
    auto radialGradientMaskEffectShader = GenerateShaderEffect(width, height, builder);

    return radialGradientMaskEffectShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GERadialGradientShaderMask::GetRadialGradientShaderMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> radialGradientShaderMaskBuilder = nullptr;
    if (radialGradientShaderMaskBuilder) {
        return radialGradientShaderMaskBuilder;
    }

    static constexpr char prog[] = R"(
        const int sizeArray = 12;
        uniform half2 iResolution;
        uniform half2 centerPos;
        uniform half radiusX;
        uniform half radiusY;
        uniform half colors[sizeArray];
        uniform half positions[sizeArray];

        float colorGradient(float colorA, float colorB, float startPos, float endPos, float threshold)
        {
            if (startPos <= threshold && threshold < endPos) {
                return mix(colorA, colorB, smoothstep(startPos, endPos, threshold));
            }
            return 0.0;
        }

        float radialGradientMask(vec2 uv, vec2 centerPosition)
        {
            float sdfValue = length(uv - centerPosition) / radiusY;
            sdfValue = clamp(sdfValue, 0.0, 1.0);

            float color = 0.0;
            for (int i = 0; i < int(sizeArray - 1); i++) {
                color += colorGradient(colors[i], colors[i + 1], positions[i], positions[i + 1], sdfValue);
            }
            return color;
        }

        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord.xy / iResolution.xy;
            float screenRatio = iResolution.x / iResolution.y;
            vec2 centeredUVs = uv * 2.0 - 1.0;
            centeredUVs.x *= screenRatio * (radiusY / radiusX);
            vec2 centerPosition = centerPos * 2 - 1.0 ;
            centerPosition.x *= screenRatio;

            float finalColor = radialGradientMask(centeredUVs, centerPosition);
            return half4(finalColor);
        }
    )";

    auto radialGradientShaderMaskEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!radialGradientShaderMaskEffect) {
        LOGE("GERadialGradientShaderMask::GetRadialGradientShaderMaskBuilder effect error");
        return nullptr;
    }

    radialGradientShaderMaskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(radialGradientShaderMaskEffect);
    return radialGradientShaderMaskBuilder;
}

std::shared_ptr<ShaderEffect> GERadialGradientShaderMask::GenerateDrawingShaderHasNormal(float width,
    float height) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetRadialGradientNormalMaskBuilder();
    if (!builder) {
        LOGE("GERadialGradientShaderMask::GenerateDrawingShaderHasNormal builder error");
        return nullptr;
    }
    auto radialGradientMaskEffectShader = GenerateShaderEffect(width, height, builder);

    return radialGradientMaskEffectShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GERadialGradientShaderMask::GetRadialGradientNormalMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> radialGradientMaskNormalBuilder = nullptr;
    if (radialGradientMaskNormalBuilder) {
        return radialGradientMaskNormalBuilder;
    }

    static constexpr char prog[] = R"(
        const int sizeArray = 12;
        uniform half2 iResolution;
        uniform half2 centerPos;
        uniform half radiusX;
        uniform half radiusY;
        uniform half colors[sizeArray];
        uniform half positions[sizeArray];

        float colorGradient(float colorA, float colorB, float startPos, float endPos, float threshold)
        {
            if (startPos <= threshold && threshold < endPos) {
                return mix(colorA, colorB, smoothstep(startPos, endPos, threshold));
            }
            return 0.0;
        }

        vec4 radialGradientMask(vec2 uv, vec2 centerPosition)
        {
            float sdfValue = length(uv - centerPosition) / radiusY;
            sdfValue = clamp(sdfValue, 0.0, 1.0);

            float color = 0.0;
            for (int i = 0; i < int(sizeArray - 1); i++) {
                color += colorGradient(colors[i], colors[i + 1], positions[i], positions[i + 1], sdfValue);
            }

            vec2 directioinVector = normalize(uv - centerPosition) * color * 0.5 + 0.5;
            return vec4(directioinVector, 1.0, color);
        }

        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord.xy / iResolution.xy;
            float screenRatio = iResolution.x / iResolution.y;
            vec2 centeredUVs = uv * 2.0 - 1.0;
            centeredUVs.x *= screenRatio * (radiusY / radiusX);
            vec2 centerPosition = centerPos * 2 - 1.0 ;
            centerPosition.x *= screenRatio;

            vec4 finalColor = radialGradientMask(centeredUVs, centerPosition);
            return half4(finalColor);
        }
    )";

    auto radialGradientShaderMaskNormalEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!radialGradientShaderMaskNormalEffect) {
        LOGE("GERadialGradientShaderMask::GetRadialGradientNormalMaskBuilder effect error");
        return nullptr;
    }

    radialGradientMaskNormalBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(
        radialGradientShaderMaskNormalEffect);
    return radialGradientMaskNormalBuilder;
}

std::shared_ptr<ShaderEffect> GERadialGradientShaderMask::GenerateShaderEffect(float width, float height,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GERadialGradientShaderMask::GenerateShaderEffect builder error");
        return nullptr;
    }

    size_t colorSize = param_.colors_.size();
    size_t positionSize = param_.positions_.size();
    // 0.01f is the min value
    if (colorSize <= 0 || colorSize > SIZE_ARRAY || colorSize != positionSize || width < 0.01f || height < 0.01f) {
        return nullptr;
    }
    // if radius <= 0, no need to draw. 0.001f is the min value
    if (param_.radiusX_ < 0.001f || param_.radiusY_ < 0.001f) {
        return nullptr;
    }

    float color[SIZE_ARRAY] = {0.0f}; // 0.0 default
    float position[SIZE_ARRAY] = {1.0f}; // 1.0 default
    for (size_t i = 0; i < SIZE_ARRAY; i++) {
        if (i < colorSize) {
            color[i] = std::clamp(param_.colors_[i], 0.0f, 1.0f); // 0.0 1.0 min and max value
            position[i] = std::clamp(param_.positions_[i], 0.0f, 1.0f); // 0.0 1.0 min and max value
        }
    }

    bool success = true;
    for (size_t i = 1; i < colorSize; i++) {
        success = success && (position[i] >= position[i - 1]);
    }
    if (!success) {
        return nullptr;
    }

    if (position[0] < 0.001f) { // 0.001 represents the fraction bias
        position[0] -= 0.001f;
    }

    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("centerPos", param_.center_.first, param_.center_.second);
    builder->SetUniform("radiusX", param_.radiusX_ * RADIUS_SCALE);
    builder->SetUniform("radiusY", param_.radiusY_ * RADIUS_SCALE);
    builder->SetUniform("colors", color, SIZE_ARRAY);
    builder->SetUniform("positions", position, SIZE_ARRAY);
    auto radialGradientMaskEffectShader = builder->MakeShader(nullptr, false);
    if (!radialGradientMaskEffectShader) {
        LOGE("GERadialGradientShaderMask::GenerateDrawingShaderHas effect error");
    }
    return radialGradientMaskEffectShader;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS