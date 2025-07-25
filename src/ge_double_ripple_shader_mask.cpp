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
#include "ge_double_ripple_shader_mask.h"
 
namespace OHOS {
namespace Rosen {
namespace Drawing {

GEDoubleRippleShaderMask::GEDoubleRippleShaderMask(GEDoubleRippleShaderMaskParams param) : param_(param) {}

std::shared_ptr<ShaderEffect> GEDoubleRippleShaderMask::GenerateDrawingShader(float width, float height) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetDoubleRippleShaderMaskBuilder();
    if (!builder) {
        LOGE("GEDoubleRippleShaderMask::GenerateDrawingShaderHas builder error");
        return nullptr;
    }
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("centerPos1", param_.center1_.first, param_.center1_.second);
    builder->SetUniform("centerPos2", param_.center2_.first, param_.center2_.second);
    builder->SetUniform("rippleRadius", param_.radius_);
    builder->SetUniform("rippleWidth", param_.width_);
    builder->SetUniform("turbulence", param_.turbulence_);
    auto rippleMaskEffectShader = builder->MakeShader(nullptr, false);
    if (!rippleMaskEffectShader) {
        LOGE("GEDoubleRippleShaderMask::GenerateDrawingShaderHas effect error");
    }
    return rippleMaskEffectShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEDoubleRippleShaderMask::GetDoubleRippleShaderMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> rippleShaderMaskBuilder = nullptr;
    if (rippleShaderMaskBuilder) {
        return rippleShaderMaskBuilder;
    }

static constexpr char prog[] = R"(
        uniform half2 iResolution;
        uniform half2 centerPos1;
        uniform half2 centerPos2;
        uniform half rippleRadius;
        uniform half rippleWidth;
        uniform half turbulence;

        const float waveScale  = 0.5;
        const float freqX = 4.0;
        const float freqY = 6.0;
        const float freqDiag = 8.0;

        float ShapeSDF(vec2 p, float radius, float noiseScale)
        {
            float dist = length(p);

            float noise = noiseScale;
            noise += sin(p.x * freqX) * 0.15; // 0.25: reduce the strength of noise on x axis
            noise += sin(p.y * freqY) * 0.15; // 0.35: reduce the strength of noise on y axis
            noise += sin((p.x + p.y) * freqDiag) * 0.075; // 0.1: reduce the strength of noise on diagonal axis

            float distortedDist = dist + noise * noiseScale;
            float attenuation = waveScale / (1.0 + distortedDist * 5.0); // 5.0: control the falloff speed of the wave
            float wave = sin(distortedDist * 30.0) * attenuation; // 30.0: control the frequency of the wave
            return distortedDist - radius + wave * 0.05; // 0.05: control the amplitude of the wave
        }

        float SmoothUnion(float d1, float d2, float k) {
            float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
            return mix(d2, d1, h) - k * h * (1.0 - h);
        }

        half4 main(float2 fragCoord)
        {
            float thickness = rippleWidth;      // 0.1
            float noiseScale = turbulence;      // 0.2

            vec2 center1 = centerPos1;
            vec2 center2 = centerPos2;
            vec2 center = (centerPos1 + centerPos2) / 2.0;

            vec2 uv = fragCoord.xy / iResolution.xy;
            float aspect = iResolution.x / iResolution.y;

            vec2 p = uv * 2.0 - 1.0;
            p.x *= aspect;

            vec2 c1 = center1 * 2.0 - 1.0;
            c1.x *= aspect;
            vec2 delta1 = p - c1;

            vec2 c2 = center2 * 2.0 - 1.0;
            c2.x *= aspect;
            vec2 delta2 = p - c2;

            float currentRadius1 = rippleRadius;
            float currentRadius2 = rippleRadius;
        
            float d1Outer = ShapeSDF(delta1, currentRadius1, noiseScale);
            float d2Outer = ShapeSDF(delta2, currentRadius2, noiseScale);
            float d1Inner = ShapeSDF(delta1, currentRadius1 * (1.0 - thickness), noiseScale);
            float d2Inner = ShapeSDF(delta2, currentRadius2 * (1.0 - thickness), noiseScale);

            float smoothness = 0.35;
            float dOuter = SmoothUnion(d1Outer, d2Outer, smoothness);
            float dInner = SmoothUnion(d1Inner, d2Inner, smoothness);

            float mask = smoothstep(thickness, 0.0, dOuter) * smoothstep(0.0, thickness, dInner);
            return half4(mask);
        }
    )";

    auto rippleShaderMaskEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!rippleShaderMaskEffect) {
        return nullptr;
    }

    rippleShaderMaskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(rippleShaderMaskEffect);
    return rippleShaderMaskBuilder;
}

std::shared_ptr<ShaderEffect> GEDoubleRippleShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetDoubleRippleShaderNormalMaskBuilder();
    if (!builder) {
        LOGE("GEDoubleRippleShaderMask::GenerateDrawingShaderHasNormal builder error");
        return nullptr;
    }
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("centerPos1", param_.center1_.first, param_.center1_.second);
    builder->SetUniform("centerPos2", param_.center2_.first, param_.center2_.second);
    builder->SetUniform("rippleRadius", param_.radius_);
    builder->SetUniform("rippleWidth", param_.width_);
    builder->SetUniform("turbulence", param_.turbulence_);
    auto rippleMaskEffectShader = builder->MakeShader(nullptr, false);
    if (!rippleMaskEffectShader) {
        LOGE("GEDoubleRippleShaderMask::GenerateDrawingShaderHasNormal effect error");
    }
    return rippleMaskEffectShader;
}


std::shared_ptr<Drawing::RuntimeShaderBuilder> GEDoubleRippleShaderMask::GetDoubleRippleShaderNormalMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> rippleShaderMaskNormalBuilder = nullptr;
    if (rippleShaderMaskNormalBuilder) {
        return rippleShaderMaskNormalBuilder;
    }

    static constexpr char prog[] = R"(
        uniform half2 iResolution;
        uniform half2 centerPos1;
        uniform half2 centerPos2;
        uniform half rippleRadius;
        uniform half rippleWidth;
        uniform half turbulence;

        const float waveScale  = 0.5;
        const float freqX = 4.0;
        const float freqY = 6.0;
        const float freqDiag = 8.0;

        float ShapeSDF(vec2 p, float radius, float noiseScale)
        {
            float dist = length(p);

            float noise = noiseScale;
            noise += sin(p.x * freqX) * 0.15; // 0.25: reduce the strength of noise on x axis
            noise += sin(p.y * freqY) * 0.15; // 0.35: reduce the strength of noise on y axis
            noise += sin((p.x + p.y) * freqDiag) * 0.075; // 0.1: reduce the strength of noise on diagonal axis

            float distortedDist = dist + noise * noiseScale;
            float attenuation = waveScale / (1.0 + distortedDist * 5.0); // 5.0: control the falloff speed of the wave
            float wave = sin(distortedDist * 30.0) * attenuation; // 30.0: control the frequency of the wave
            return distortedDist - radius + wave * 0.05; // 0.05: control the amplitude of the wave
        }

        float SmoothUnion(float d1, float d2, float k) {
            float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
            return mix(d2, d1, h) - k * h * (1.0 - h);
        }

        half4 main(float2 fragCoord)
        {
            float thickness = rippleWidth;      // 0.1
            float noiseScale = turbulence;      // 0.2

            vec2 center1 = centerPos1;
            vec2 center2 = centerPos2;
            vec2 center = (centerPos1 + centerPos2) / 2.0;

            vec2 uv = fragCoord.xy / iResolution.xy;
            float aspect = iResolution.x / iResolution.y;

            vec2 p = uv * 2.0 - 1.0;
            p.x *= aspect;

            vec2 c1 = center1 * 2.0 - 1.0;
            c1.x *= aspect;
            vec2 delta1 = p - c1;

            vec2 c2 = center2 * 2.0 - 1.0;
            c2.x *= aspect;
            vec2 delta2 = p - c2;

            float currentRadius1 = rippleRadius;
            float currentRadius2 = rippleRadius;
        
            float d1Outer = ShapeSDF(delta1, currentRadius1, noiseScale);
            float d2Outer = ShapeSDF(delta2, currentRadius2, noiseScale);
            float d1Inner = ShapeSDF(delta1, currentRadius1 * (1.0 - thickness), noiseScale);
            float d2Inner = ShapeSDF(delta2, currentRadius2 * (1.0 - thickness), noiseScale);

            float smoothness = 0.35;
            float dOuter = SmoothUnion(d1Outer, d2Outer, smoothness);
            float dInner = SmoothUnion(d1Inner, d2Inner, smoothness);

            float mask = smoothstep(thickness, 0.0, dOuter) * smoothstep(0.0, thickness, dInner);

            vec2 directionVector = normalize(p + 1.0 - center1 - center2) * mask * 0.5 + 0.5;
            return half4(directionVector, mask, 1.0);
        }
    )";

    auto rippleShaderMaskNormalEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!rippleShaderMaskNormalEffect) {
        LOGE("GEDoubleRippleShaderMask::GetRippleShaderNormalMaskBuilder effect error");
        return nullptr;
    }

    rippleShaderMaskNormalBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(rippleShaderMaskNormalEffect);
    return rippleShaderMaskNormalBuilder;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS