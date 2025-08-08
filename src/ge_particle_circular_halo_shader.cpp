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
            uniform half2 iResolution;
            uniform float globalRadius;   // globalRadius of all circles in animations, range: 0.0 - 1.0
            uniform half2 rotationCenter;  // Center of the halos or rings, recommended value: (0.5, 0.5)
            uniform float randomNoise;    // randomNoise seed

            half4 fragColor;
            const int NUMBER_OF_SAMPLES = 11;
            const float NUMBER_OF_SAMPLES_F = 11.;
            const float WIDTH = 0.07;
            const float PI = 3.14159;
            const float PI2 = 6.28318;

            // Colorbar stops and colors
            const float STOP_POS0 = 0.00;
            const float STOP_POS1 = 0.22;
            const float STOP_POS2 = 0.75;
            const float STOP_POS3 = 0.90;
            const float STOP_POS4 = 1.00;
            const half3 STOP_COLOR0 = half3(255.0, 133.0, 127.0) / 255.0;  // FF717F
            const half3 STOP_COLOR1 = half3(86.0, 146.0, 255.0) / 255.0;   // 5692FF
            const half3 STOP_COLOR2 = half3(137.0, 240.0, 255.0) / 255.0;  // 89F0FF
            const half3 STOP_COLOR3 = half3(255.0, 200.0, 161.0) / 255.0;  // FFC8A1
            const half3 STOP_COLOR4 = half3(253.0, 216.0, 98.0) / 255.0;   // FDD862

            // ****************************** Sub-functions ******************************
            half2 Random2(half2 st)
            {
                st = half2(dot(st, half2(127.1, 1.7)), dot(st, half2(269.5, 183.3)));
                return -1.0 + 2.0 * fract(sin(st) * 4258.5453123);
            }

            float Noise2(half2 st)
            {
                half2 i = floor(st);
                half2 f = fract(st);
                half2 u = smoothstep(0.0, 1.0, f);
                return mix(mix(dot(Random2(i + half2(0.0, 0.0)), f - half2(0.0, 0.0)),
                               dot(Random2(i + half2(1.0, .0)), f - half2(1.0, 0.0)), u.x),
                           mix(dot(Random2(i + half2(0.0, 1.0)), f - half2(0.0, 1.0)),
                               dot(Random2(i + half2(1.0, 1.0)), f - half2(1., 1.0)), u.x),
                           u.y);
            }

            float Random1(float x)
            {
                return fract(sin(x) * 2358.5453123);
            }

            float Noise1(float t)
            {
                float i = floor(t);
                float f = fract(t);
                float u = smoothstep(0.0, 1.0, f);
                return mix(Random1(i), Random1(i + 1.0), u);
            }

            half2 ShapePerturbation(half2 uv, float noiseVariation, float noiseScale, float noiseStrength)
            {
                float screenNoise = Noise2(half2(uv.x - noiseVariation, uv.y + noiseVariation) * noiseScale);
                return uv + screenNoise * noiseStrength;
            }

            float SimpleHaloRingShape(half2 uv, float radius, float noiseVariation, float haloThickness)
            {
                uv = ShapePerturbation(uv, noiseVariation, 2.0, radius * 0.25);
                half2 polarCoords = half2((atan(uv.y, uv.x) + PI) / (2.0 * PI), length(uv));
                polarCoords.y -= radius;
                float angularRandomVal = Noise1((polarCoords.x) * 10.0);
                // ensure that the radial noise is seamless
                angularRandomVal = mix(angularRandomVal, 0.5, smoothstep(0.9, 1.0, abs(polarCoords.x * 2.0 - 1.0)));
                float thickness = mix(haloThickness, haloThickness * 4.0, angularRandomVal);
                float circleBorder = smoothstep(thickness, thickness * 0.2, abs(polarCoords.y));
                return clamp(circleBorder, 0.0, 1.0);
            }

            half3 GetColorFromColorbar(half2 pt, float radius)
            {
                half3 colorValue;
                // Map pt.x to the range [0.0, 1.0] for color interpolation
                float t = (pt.x + radius) / (2.0 * radius);
                colorValue = step(pt.x, -radius) * STOP_COLOR0 +
                             // return the first color when pt.x is to the left of the halo
                             step(STOP_POS0, t) * step(t, STOP_POS1) *
                                 mix(STOP_COLOR0, STOP_COLOR1,
                                     (t - STOP_POS0) / (STOP_POS1 - STOP_POS0)) +  // 0.00 < t <= 0.22
                             step(STOP_POS1, t) * step(t, STOP_POS2) *
                                 mix(STOP_COLOR1, STOP_COLOR2,
                                     (t - STOP_POS1) / (STOP_POS2 - STOP_POS1)) +  // 0.22 < t <= 0.75
                             step(STOP_POS2, t) * step(t, STOP_POS3) *
                                 mix(STOP_COLOR2, STOP_COLOR3,
                                     (t - STOP_POS2) / (STOP_POS3 - STOP_POS2)) +  // 0.75 < t <= 0.90
                             step(STOP_POS3, t) * step(t, STOP_POS4) *
                                 mix(STOP_COLOR3, STOP_COLOR4,
                                     (t - STOP_POS3) / (STOP_POS4 - STOP_POS3)) +  // 0.90 < t <= 1.00
                             step(STOP_POS4, t) * STOP_COLOR4;                         // t > 1.00
                return colorValue;
            }

            float CentralAmbienceHaloBorder(half2 uv, half2 polarCoords, float radius, float animationTime,
                float rotationTimeScale, float noiseScale, float noiseDisplacement)
            {
                polarCoords.x = fract((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8));
                float screenNoise = Noise2(half2(uv + (animationTime * rotationTimeScale)) * noiseScale);
                float angularRandomVal = Noise1((polarCoords.x) * 10.0);
                angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x *2.0 - 1.0)));
                radius += (screenNoise * noiseDisplacement);
                polarCoords.y -= radius;
                float thickness = mix(0.01, 0.035, angularRandomVal);
                float circleBorder = smoothstep(thickness * radius, -0.01, abs(polarCoords.y));
                circleBorder *= smoothstep(0.1, 1.0, angularRandomVal);
                circleBorder *= screenNoise * 0.5 + 0.5;
                return circleBorder;
            }

            float CentralAmbienceHaloGlow(half2 polarCoords, float animationTime, float radius, float glowRadius,
                float glowExposure)
            {
                polarCoords.x = fract(((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8)));
                float angularRandomVal = Noise1(polarCoords.x * 10.);
                angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x *2.0 - 1.0)));
                polarCoords.y -= radius;
                float circleBorderGlow = smoothstep(glowRadius, -0.01, abs(polarCoords.y)) * glowExposure;
                return circleBorderGlow;
            }
            // **************************** Main Functions ****************************
            half4 main(vec2 fragCoord)
            {
                float radius = globalRadius * 2.;
                float innerRadiusEdge = step(radius, 0.5) * 0.001 + step(0.5, radius) * 0.6 * radius;
                float outerRadiusEdge = step(radius, 0.5) * 0.7 + step(0.5, radius) * 1.4 * radius;
                float haloColor = 0.;
              	float glowColor = 0.;

                half2 uv = fragCoord.xy / iResolution.xy;
                float screenRatio = iResolution.x / iResolution.y;
                half2 centeredUVs = uv * 2.0 - 1.0;
                centeredUVs.x *= screenRatio;
                half2 centerPosition = rotationCenter * 2. - 1.0;
                centerPosition.x *= screenRatio;
                half2 directionVector = centeredUVs - centerPosition;
                half2 polarCoords = half2((atan(directionVector.y, directionVector.x) + PI) / (2.0 * PI),
                    length(directionVector));

                float angle = PI2;
                float blurFactor = 0.20;
                float colorRotScale = radius * PI * 0.5;  // make the ring colors rotated.
                mat2 rotColorRing =
                    mat2(cos(colorRotScale), -sin(colorRotScale), sin(colorRotScale), cos(colorRotScale));
                half3 colorRing = GetColorFromColorbar(centeredUVs * rotColorRing, radius);

                if (length(directionVector) >= innerRadiusEdge && length(directionVector) <= outerRadiusEdge) {
                    float particleHalo = 0.;
                    float rate = blurFactor / NUMBER_OF_SAMPLES_F;
                    for (int i = -NUMBER_OF_SAMPLES / 2; i <= NUMBER_OF_SAMPLES / 2; i++) {
                        float rotationFactor = float(i) * rate;
                        float rotAngle = angle * rotationFactor;
                        half2 rotatedDir = mat2(cos(rotAngle), -sin(rotAngle),
                                                sin(rotAngle), cos(rotAngle)) * directionVector;
                        float haloRing = SimpleHaloRingShape(rotatedDir, radius, randomNoise * radius,
                                            WIDTH * globalRadius);
                        particleHalo += haloRing;
                    }
                    particleHalo = particleHalo / NUMBER_OF_SAMPLES_F;
                    haloColor = haloColor + particleHalo - haloColor * particleHalo;

                    float ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius * 0.85,
                    globalRadius * 0.1, 1.0, 1.25, 0.05) * 2.5;
                    ambienceHaloBorder = clamp(ambienceHaloBorder, 0., 1.);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;

                    ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius * 0.95,
                        globalRadius * 0.25, 1.0, 0.25, 0.05);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;

                    ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius,
                        globalRadius * 0.35, 1.0, 0.25, 0.05);
                    haloColor = haloColor + ambienceHaloBorder - haloColor * ambienceHaloBorder;
                    float ambienceHaloGlow = CentralAmbienceHaloGlow(polarCoords, globalRadius * 0.1, radius * 0.86,
                        0.3 * radius, 0.6) * 1.3;
                    float glow = ambienceHaloGlow;
                    glowColor = glowColor + glow - glowColor * glow; // Add a glow circle
                }

              	fragColor = half4(haloColor + glowColor - haloColor * glowColor);
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