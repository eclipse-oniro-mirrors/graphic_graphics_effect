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

constexpr float MIN_CENTER_POSITION = 0.0f;
constexpr float MAX_CENTER_POSITION = 1.0f;
constexpr float MIN_RADIUS = 0.0f;
constexpr float MAX_RADIUS = 1.0f;
constexpr float MIN_NOISE = 0.0f;

namespace OHOS {
namespace Rosen {
GEParticleCircularHaloShader::GEParticleCircularHaloShader(Drawing::GEParticleCircularHaloShaderParams &param)
{
    ParticleCircularHaloParams_ = param;
}

void GEParticleCircularHaloShader::MakeDrawingShader(const Drawing::Rect &rect, float progress)
{
    drShader_ = MakeParticleCircularHaloShader(rect);
}

void GEParticleCircularHaloShader::SetGlobalRadius(const float globalRadius)
{
    ParticleCircularHaloParams_.radius_ = globalRadius;
}

void GEParticleCircularHaloShader::SetRotationCenter(const std::pair<float, float> &rotationCenter)
{
    ParticleCircularHaloParams_.center_ = rotationCenter;
}

void GEParticleCircularHaloShader::SetRandomNoise(const float randomNoise)
{
    ParticleCircularHaloParams_.noise_ = randomNoise;
}

float GEParticleCircularHaloShader::ClampValue(float x, float minValue, float maxValue)
{
    return std::min(std::max(x, minValue), maxValue);
}

std::shared_ptr<GEParticleCircularHaloShader> GEParticleCircularHaloShader::CreateParticleCircularHaloShader(
    Drawing::GEParticleCircularHaloShaderParams &param)
{
    std::shared_ptr<GEParticleCircularHaloShader> ParticleCircularHaloShader =
        std::make_shared<GEParticleCircularHaloShader>(param);
    return ParticleCircularHaloShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEParticleCircularHaloShader::GetParticleCircularHaloBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> ParticleCircularHaloShaderEffect_ = nullptr;

    if (ParticleCircularHaloShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform half2 iResolution;
            uniform float globalRadius;   // globalRadius of all circles in animations, range: 0.0 - 0.9
            uniform vec2 rotationCenter;  // Center of the halos or rings, recommended value: (0.5, 0.5)
            uniform float randomNoise;    // randomNoise seed, recommended value: 4.0

            vec4 fragColor;
            const float NUMBER_OF_SAMPLES = 25.;
            const float width1 = 0.006;  // width of the first halo, low accuracy
            const float width2 = 0.02;   // width of the second halo, high accuracy
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
                uv = shapePerturbation(uv, noiseVariation, 3.0, radius * 0.3);
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
            // **************************** Main Functions ****************************
            vec4 main(vec2 fragCoord)
            {
                float radius = globalRadius * 2.;
                float innerRadiusEdge = radius * 0.7;
                float outerRadiusEdge = radius * 1.3;

                vec2 uv = fragCoord.xy / iResolution.xy;
                float screenRatio = iResolution.x / iResolution.y;
                vec2 centeredUVs = uv * 2.0 - 1.0;
                centeredUVs.x *= screenRatio;
                vec2 centerPosition = rotationCenter * 2. - 1.0;
                centerPosition.x *= screenRatio;
                vec2 directionVector = centeredUVs - centerPosition;
                float angle = PI2 * 2.;
                float blurFactor = 0.10;
                float sigma = 10.;  // controls the blur spread
                float normalizationFactor = 1.0 / (sigma * SQRTPI2);

                // Final blurred color accumulator
                vec4 finalColor = vec4(0.0);
                float colorRotScale = radius * PI * 0.5;  // make the ring colors rotated.
                mat2 rotColorRing =
                    mat2(cos(colorRotScale), -sin(colorRotScale), sin(colorRotScale), cos(colorRotScale));
                // Get the color of each pixel from the colorbar determined by the UX
                vec3 colorRing = getColorFromColorbar(centeredUVs * rotColorRing, radius);

                if (length(directionVector) >= innerRadiusEdge && length(directionVector) <= outerRadiusEdge) {
                    vec4 color1 = vec4(0.);
                    vec4 color2 = vec4(0.);
                    float rate = blurFactor / float(NUMBER_OF_SAMPLES);
                    for (float i = -NUMBER_OF_SAMPLES / 2.; i <= NUMBER_OF_SAMPLES / 2.; i++) {
                        float rotationFactor1 = i * rate;
                        float rotAngle = angle * rotationFactor1;
                        vec2 rotatedDir1 = mat2(cos(rotAngle), -sin(rotAngle),
                                                sin(rotAngle), cos(rotAngle)) *
                                           directionVector;
                        vec2 haloRing1 = simpleHaloRingShape(centerPosition + rotatedDir1, centerPosition, radius,
                                                             randomNoise, width1 + 0.004 * globalRadius);
                        color1 += vec4(haloRing1.x);

                        vec2 haloRing2 = simpleHaloRingShape(centerPosition + rotatedDir1, centerPosition,
                                                             radius * (0.05 * globalRadius + 0.92), randomNoise,
                                                             width2 + 0.02 * globalRadius);
                        color2 += vec4(haloRing2.x);
                    }
                    color1 /= NUMBER_OF_SAMPLES * 0.6;
                    finalColor = finalColor + color1 - finalColor * color1;

                    color2 /= NUMBER_OF_SAMPLES * 2.;
                    finalColor = finalColor + color2 - finalColor * color2;
                }
                vec4 glow = clamp(0.01 / abs(length(directionVector) - radius * (0.05 * radius + 0.90)), 0., 0.3) *
                            vec4(vec3(1.), 0.4 * radius);
                finalColor = finalColor + glow - finalColor * glow;  // Add a glow circle
                fragColor = finalColor * vec4(colorRing, 0.5 * 0.5 * globalRadius);

                return fragColor;
            }
        )";
        ParticleCircularHaloShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (ParticleCircularHaloShaderEffect_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader:: GetParticleCircularHaloBuilder ShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(ParticleCircularHaloShaderEffect_);
}

void GEParticleCircularHaloShader::ClampInputValue()
{
    ParticleCircularHaloParams_.radius_ = ClampValue(ParticleCircularHaloParams_.radius_, MIN_RADIUS, MAX_RADIUS);
    ParticleCircularHaloParams_.center_.first =
        ClampValue(ParticleCircularHaloParams_.center_.first, MIN_CENTER_POSITION, MAX_CENTER_POSITION);
    ParticleCircularHaloParams_.center_.second =
        ClampValue(ParticleCircularHaloParams_.center_.second, MIN_CENTER_POSITION, MAX_CENTER_POSITION);
    ParticleCircularHaloParams_.noise_ = std::max(ParticleCircularHaloParams_.noise_, MIN_NOISE);
}

std::shared_ptr<Drawing::ShaderEffect> GEParticleCircularHaloShader::MakeParticleCircularHaloShader(
    const Drawing::Rect &rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    ClampInputValue();
    builder_ = GetParticleCircularHaloBuilder();
    if (builder_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::MakeParticleCircularHaloShader builder_ is nullptr.");
        return nullptr;
    }

    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("globalRadius", ParticleCircularHaloParams_.radius_);
    builder_->SetUniform("rotationCenter", ParticleCircularHaloParams_.center_.first,
                         ParticleCircularHaloParams_.center_.second);
    builder_->SetUniform("randomNoise", ParticleCircularHaloParams_.noise_);
    auto ParticleCircularHaloShader = builder_->MakeShader(nullptr, false);
    if (ParticleCircularHaloShader == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::MakeParticleCircularHaloShader ParticleCircularHaloShader is nullptr.");
        return nullptr;
    }
    return ParticleCircularHaloShader;
}
}  // namespace Rosen
}  // namespace OHOS