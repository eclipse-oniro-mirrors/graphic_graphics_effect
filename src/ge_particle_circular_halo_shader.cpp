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

#include "ge_log.h"
#include "ge_particle_circular_halo_shader.h"
#include "ge_visual_effect_impl.h"

namespace {
    constexpr float MIN_CENTER_POSITION = 0.001f;
    constexpr float MAX_CENTER_POSITION = 1.0f;
    constexpr float MIN_RADIUS = 0.001f;
    constexpr float MAX_RADIUS = 1.0f;
    constexpr float MIN_NOISE = 0.001f;
}

namespace OHOS {
namespace Rosen {
GEParticleCircularHaloShader::GEParticleCircularHaloShader(Drawing::GEParticleCircularHaloShaderParams& params)
{
    particleCircularHaloParams_ = params;
}

void GEParticleCircularHaloShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeParticleCircularHaloShader(rect);
}

void GEParticleCircularHaloShader::SetGlobalRadius(const float globalRadius)
{
    particleCircularHaloParams_.radius_ = std::clamp(globalRadius, MIN_RADIUS, MAX_RADIUS);
}

void GEParticleCircularHaloShader::SetRotationCenter(const std::pair<float, float>& rotationCenter)
{
    particleCircularHaloParams_.center_.first =
        std::clamp(rotationCenter.first, MIN_CENTER_POSITION, MAX_CENTER_POSITION);
    particleCircularHaloParams_.center_.second =
        std::clamp(rotationCenter.second, MIN_CENTER_POSITION, MAX_CENTER_POSITION);
}

void GEParticleCircularHaloShader::SetRandomNoise(const float randomNoise)
{
    particleCircularHaloParams_.noise_ = std::max(randomNoise, MIN_NOISE);
}

std::shared_ptr<GEParticleCircularHaloShader> GEParticleCircularHaloShader::CreateParticleCircularHaloShader(
    Drawing::GEParticleCircularHaloShaderParams& params)
{
    std::shared_ptr<GEParticleCircularHaloShader> particleCircularHaloShader =
        std::make_shared<GEParticleCircularHaloShader>(params);
    return particleCircularHaloShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEParticleCircularHaloShader::GetParticleCircularHaloBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> particleCircularHaloShaderEffect_ = nullptr;

    if (particleCircularHaloShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform vec2 iResolution;
            uniform float globalRadius;   // globalRadius of all circles in animations, range: 0.0 - 0.9
            uniform vec2 rotationCenter;  // Center of the halos or rings, recommended value: (0.5, 0.5)
            uniform float randomNoise;    // randomNoise seed, recommended value: 4.0

            vec4 fragColor;
            const float NUMBER_OF_SAMPLES = 11.;
            const float width = 0.03;
            const float PI = 3.14159;
            const float PI2 = 6.28318;
            const float SQRTPI2 = 2.50662;
            // ****************************** Sub-functions ******************************
            vec2 random2(vec2 st)
            {
                st = vec2(dot(st, vec2(127.1, 1.7)), dot(st, vec2(269.5, 183.3)));
                return -1.0 + 2.0 * fract(sin(st) * 4258.5453123);
            }
            float noise(vec2 st)
            {
                vec2 i = floor(st);
                vec2 f = fract(st);
                vec2 u = f * f * (3.0 - 2.0 * f);
                return mix(mix(dot(random2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
                               dot(random2(i + vec2(1.0, .0)), f - vec2(1.0, 0.0)), u.x),
                           mix(dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
                               dot(random2(i + vec2(1.0, 1.0)), f - vec2(1., 1.0)), u.x),
                           u.y);
            }

            float random11(float x)
            {
                return fract(sin(x) * 2358.5453123);
            }

            float smoothRandomNoise11(float t)
            {
                float bias = fract(t);
                // Cubic Hermite Curve. Same as SmoothStep()
                bias = bias * bias * (3.0 - 2.0 * bias);
                return mix(random11(floor(t)), random11(floor(t) + 1.0), bias);
            }

            vec2 shapePerturbation(vec2 uv, float noiseVariation, float noiseScale, float noiseStrength)
            {
                float screenNoise = noise(vec2(uv.x - noiseVariation, uv.y + noiseVariation) * noiseScale);
                return uv + screenNoise * noiseStrength;
            }

            vec2 simpleHaloRingShape(vec2 uv, vec2 position, float radius, float noiseVariation, float haloThickness)
            {
                uv = shapePerturbation(uv, noiseVariation, 3.0, radius * 0.2);
                vec2 polarCoords = vec2((atan(uv.y, uv.x) + PI) / (2.0 * PI), length(uv - position));
                polarCoords.y -= radius;
                float angularRandomVal = smoothRandomNoise11((polarCoords.x) * 10.0);
                // ensure that the radial noise is seamless
                angularRandomVal = mix(angularRandomVal, 0.5, smoothstep(0.9, 1.0, abs(polarCoords.x * 2.0 - 1.0)));
                float thickness = mix(haloThickness, haloThickness * 4.0, angularRandomVal);
                float circleBorder = smoothstep(thickness, thickness * 0.2, abs(polarCoords.y));
                return clamp(vec2(circleBorder), 0.0, 1.0);
            }

            // Colorbar stops and colors
            const float colorStopPos0 = 0.00;
            const float colorStopPos1 = 0.22;
            const float colorStopPos2 = 0.75;
            const float colorStopPos3 = 0.90;
            const float colorStopPos4 = 1.00;
            const vec3 colorStopColor0 = vec3(255.0, 133.0, 127.0) / 255.0;  // FF717F
            const vec3 colorStopColor1 = vec3(86.0, 146.0, 255.0) / 255.0;   // 5692FF
            const vec3 colorStopColor2 = vec3(137.0, 240.0, 255.0) / 255.0;  // 89F0FF
            const vec3 colorStopColor3 = vec3(255.0, 200.0, 161.0) / 255.0;  // FFC8A1
            const vec3 colorStopColor4 = vec3(253.0, 216.0, 98.0) / 255.0;   // FDD862

            vec3 getColorFromColorbar(vec2 pt, float radius)
            {
                vec3 colorValue;
                // Map pt.x to the range [0.0, 1.0] for color interpolation
                float t = (pt.x + radius) / (2.0 * radius);
                colorValue = step(pt.x, -radius) * colorStopColor0 +
                             // return the first color when pt.x is to the left of the halo
                             step(colorStopPos0, t) * step(t, colorStopPos1) *
                                 mix(colorStopColor0, colorStopColor1,
                                     (t - colorStopPos0) / (colorStopPos1 - colorStopPos0)) +  // 0.00 < t <= 0.22
                             step(colorStopPos1, t) * step(t, colorStopPos2) *
                                 mix(colorStopColor1, colorStopColor2,
                                     (t - colorStopPos1) / (colorStopPos2 - colorStopPos1)) +  // 0.22 < t <= 0.75
                             step(colorStopPos2, t) * step(t, colorStopPos3) *
                                 mix(colorStopColor2, colorStopColor3,
                                     (t - colorStopPos2) / (colorStopPos3 - colorStopPos2)) +  // 0.75 < t <= 0.90
                             step(colorStopPos3, t) * step(t, colorStopPos4) *
                                 mix(colorStopColor3, colorStopColor4,
                                     (t - colorStopPos3) / (colorStopPos4 - colorStopPos3)) +  // 0.90 < t <= 1.00
                             step(colorStopPos4, t) * colorStopColor4;                         // t > 1.00
                return colorValue;
            }

            float centralAmbienceHaloBorder(vec2 uv, vec2 polarCoords, float radius, float animationTime,
                float rotationTimeScale, float noiseScale, float noiseDisplacement)
            {
                polarCoords.x = fract((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8));
                float screenNoise = noise(vec2(uv + (animationTime * rotationTimeScale)) * noiseScale);
                float angularRandomVal = smoothRandomNoise11((polarCoords.x) * 10.0);
                angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x *2.0 - 1.0)));
                radius += (screenNoise * noiseDisplacement);
                polarCoords.y -= radius;
                float thickness = mix(0.01, 0.035, angularRandomVal);
                float circleBorder = smoothstep(thickness * radius, 0.0, abs(polarCoords.y));
                circleBorder *= smoothstep(0.1, 1.0, angularRandomVal);
                circleBorder *= screenNoise * 0.5 + 0.5;
                return circleBorder;
            }

            float centralAmbienceHaloGlow(vec2 polarCoords, float animationTime, float radius, float glowRadius,
                float glowExposure)
            {
                polarCoords.x = fract(((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8)));
                float angularRandomVal = smoothRandomNoise11(polarCoords.x * 10.);
                angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x *2.0 - 1.0)));
                polarCoords.y -= radius;
                float circleBorderGlow = smoothstep(glowRadius, 0.0, abs(polarCoords.y)) * glowExposure;
                return circleBorderGlow;
            }
            // **************************** Main Functions ****************************
            vec4 main(vec2 fragCoord)
            {
                float radius = globalRadius * 2.;
                float innerRadiusEdge = step(radius, 0.5) * 0.001 + step(0.5, radius) * 0.7 * radius;
                float outerRadiusEdge = step(radius, 0.5) * 0.7 + step(0.5, radius) * 1.35 * radius;
                float haloColor = 0.;
              	float glowColor = 0.;

                vec2 uv = fragCoord.xy / iResolution.xy;
                float screenRatio = iResolution.x / iResolution.y;
                vec2 centeredUVs = uv * 2.0 - 1.0;
                centeredUVs.x *= screenRatio;
                vec2 centerPosition = rotationCenter * 2. - 1.0;
                centerPosition.x *= screenRatio;
                vec2 directionVector = centeredUVs - centerPosition;
                vec2 polarCoords = vec2((atan(directionVector.y, directionVector.x) + PI) / (2.0 * PI),
                    length(directionVector));

                float angle = PI2 * 2.;
                float blurFactor = 0.10;
                float colorRotScale = radius * PI * 0.5;  // make the ring colors rotated.
                mat2 rotColorRing =
                    mat2(cos(colorRotScale), -sin(colorRotScale), sin(colorRotScale), cos(colorRotScale));
                vec3 colorRing = getColorFromColorbar(centeredUVs * rotColorRing, radius);

                if (length(directionVector) >= innerRadiusEdge && length(directionVector) <= outerRadiusEdge) {
                    float bgHaloColor1 = 0.;
                    float rate = blurFactor / NUMBER_OF_SAMPLES;
                    for (float i = -NUMBER_OF_SAMPLES / 2.; i <= NUMBER_OF_SAMPLES / 2.; i++) {
                        float rotationFactor1 = i * rate;
                        float rotAngle = angle * rotationFactor1;
                        vec2 rotatedDir = mat2(cos(rotAngle), -sin(rotAngle),
                                                sin(rotAngle), cos(rotAngle)) * directionVector;
                        vec2 haloRing = simpleHaloRingShape(centerPosition + rotatedDir, centerPosition, radius,
                                            randomNoise * radius, width + 0.015 * globalRadius);
                        bgHaloColor1 += haloRing.x;
                    }
                    bgHaloColor1 = bgHaloColor1 / (NUMBER_OF_SAMPLES * 1.5);
                    haloColor = haloColor + bgHaloColor1 - haloColor * bgHaloColor1;

                    float ambienceHaloBorder = centralAmbienceHaloBorder(directionVector, polarCoords, radius * 0.82,
                    globalRadius * 0.1, 1.0, 1.25, 0.05) * 2.5;
                    ambienceHaloBorder = clamp(ambienceHaloBorder, 0., 1.);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;

                    ambienceHaloBorder = centralAmbienceHaloBorder(directionVector, polarCoords, radius*0.95,
                        globalRadius * 0.25, 1.0, 0.25, 0.05);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;

                    ambienceHaloBorder = centralAmbienceHaloBorder(directionVector, polarCoords, radius,
                        globalRadius * 0.35, 1.0, 0.25, 0.05);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;
                    float ambienceHaloGlow = centralAmbienceHaloGlow(polarCoords, globalRadius * 0.1, radius * 0.83,
                        0.2 * radius, 0.4 * radius) * 1.3;
                    float glow = clamp(ambienceHaloGlow, 0., 1.);
                    glowColor = glowColor + glow - glowColor * glow; // Add a glow circle
                }

              	fragColor = vec4(haloColor + glowColor - haloColor * glowColor);
                fragColor.xyz *= colorRing;

                return fragColor;
            }
        )";
        particleCircularHaloShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (particleCircularHaloShaderEffect_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader:: GetParticleCircularHaloBuilder ShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(particleCircularHaloShaderEffect_);
}

std::shared_ptr<Drawing::ShaderEffect> GEParticleCircularHaloShader::MakeParticleCircularHaloShader(
    const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    builder_ = GetParticleCircularHaloBuilder();
    if (builder_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::MakeParticleCircularHaloShader builder_ is nullptr.");
        return nullptr;
    }

    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("globalRadius", particleCircularHaloParams_.radius_);
    builder_->SetUniform("rotationCenter", particleCircularHaloParams_.center_.first,
                         particleCircularHaloParams_.center_.second);
    builder_->SetUniform("randomNoise", particleCircularHaloParams_.noise_);
    auto particleCircularHaloShader = builder_->MakeShader(nullptr, false);
    if (particleCircularHaloShader == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::MakeParticleCircularHaloShader particleCircularHaloShader is nullptr.");
        return nullptr;
    }
    return particleCircularHaloShader;
}
}  // namespace Rosen
}  // namespace OHOS