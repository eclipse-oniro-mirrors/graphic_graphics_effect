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
GEParticleCircularHaloShader::GEParticleCircularHaloShader(Drawing::GEParticleCircularHaloShaderParams& param)
{
    ParticleCircularHaloParams_ = param;
}

void GEParticleCircularHaloShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeParticleCircularHaloShader(rect);
}

void GEParticleCircularHaloShader::SetGlobalRadius(const float globalRadius)
{
    ParticleCircularHaloParams_.radius_ = globalRadius;
}

void GEParticleCircularHaloShader::SetRotationCenter(const std::pair<float, float>& rotationCenter)
{
    ParticleCircularHaloParams_.center_ = rotationCenter;
}

void GEParticleCircularHaloShader::SetRandomNoise(const float randomNoise)
{
    ParticleCircularHaloParams_.noise_ = randomNoise;
}

std::shared_ptr<GEParticleCircularHaloShader> GEParticleCircularHaloShader::CreateParticleCircularHaloShader(
    Drawing::GEParticleCircularHaloShaderParams& param)
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
            uniform half globalRadius; // globalRadius of all circles in animations, range: 0.0 - 1.3
            uniform half2 rotationCenter; // Center of the halos or rings, recommended value: (0., 0.)
            uniform half randomNoise;

            half4 fragColor;
            const int NUMBER_OF_SAMPLES = 20;

            half colorScale = 1.2;

            half width1 = 0.006; // width of the first halo, low accuracy
            half width2 = 0.035; // width of the second halo, high accuracy

            const half PI = 3.14159;
            const half PI2 = 6.28318;
            const half SQRTPI2 = 2.50662;

            // ****************************** Sub-functions ******************************
            half2 random2(half2 st)
            {
                st = half2(dot(st, half2(127.1, 1.7)), dot(st, half2(269.5, 183.3)));
                return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
            }

            half noise(half2 st)
            {
                half2 i = floor(st);
                half2 f = fract(st);
                half2 u = f * f * (3.0 - 2.0 * f);

                return mix(mix(dot(random2(i + half2(0.0, 0.0)), f - half2(0.0, 0.0)),
                               dot(random2(i + half2(1.0, 0.0)), f - half2(1.0, 0.0)), u.x),
                           mix(dot(random2(i + half2(0.0, 1.0)), f - half2(0.0, 1.0)),
                               dot(random2(i + half2(1.0, 1.0)), f - half2(1., 1.0)), u.x), u.y);
            }

            half random11(half x)
            {
                return fract(sin(x) * 43758.5453123);
            }

            half smoothRandomNoise11(half t)
            {
                half bias = fract(t);
                // Cubic Hermite Curve. Same as SmoothStep()
                bias = bias * bias * (3.0 - 2.0 * bias);
                return mix(random11(floor(t)), random11(floor(t) + 1.0), bias);
            }

            half2 shapePerturbation(half2 uv,  half noiseVariation, half noiseScale, half noiseStrength)
            {
                half screenNoise = noise(half2(uv.x - noiseVariation, uv.y + noiseVariation) * noiseScale);
                return uv + screenNoise * noiseStrength;
            }

            half2 simpleHaloRingShape(half2 uv, half2 position, half radius, half noiseVariation, half haloThickness)
            {
                uv = shapePerturbation(uv, noiseVariation, 3.0, 0.10);
                half2 polarCoords = half2((atan(uv.y, uv.x) + PI) / (2.0 * PI), length(uv-position));
                polarCoords.y -= radius;

                half angularRandomVal = smoothRandomNoise11((polarCoords.x) * 10.0);
                // ensure that the radial noise is seamless
                angularRandomVal = mix(angularRandomVal, 0.5, smoothstep(0.9, 1.0, abs(polarCoords.x * 2.0-1.0)));

                half thickness = mix(haloThickness, haloThickness * 4.0, angularRandomVal);
                half circleBorder = smoothstep(thickness, thickness*0.2, abs(polarCoords.y));
 
                return clamp(half2(circleBorder), 0.0, 1.0);
            }

            // Colorbar stops and colors
            const half colorStopPos0 = 0.00;
            const half colorStopPos1 = 0.22;
            const half colorStopPos2 = 0.75;
            const half colorStopPos3 = 0.90;
            const half colorStopPos4 = 1.00;

            const half3 colorStopColor0 = half3(255.0, 133.0, 127.0) / 255.0;   // FF717F
            const half3 colorStopColor1 = half3(86.0,  146.0, 255.0) / 255.0;   // 5692FF
            const half3 colorStopColor2 = half3(137.0, 240.0, 255.0) / 255.0;   // 89F0FF
            const half3 colorStopColor3 = half3(255.0, 200.0, 161.0) / 255.0;   // FFC8A1
            const half3 colorStopColor4 = half3(253.0, 216.0, 98.0) / 255.0;   // FDD862

            half3 getColorFromColorbar(half2 pt, half radius)
            {
                half3 colorValue;
                // Map pt.x to the range [0.0, 1.0] for color interpolation
                half t = (pt.x + radius) / (2.0 * radius);
                colorValue =
                    step(pt.x, -radius) * colorStopColor0 +
                    // return the first color when pt.x is to the left of the halo
                    step(colorStopPos0, t) * step(t, colorStopPos1) * mix(colorStopColor0, colorStopColor1,
                        (t - colorStopPos0) / (colorStopPos1 - colorStopPos0)) + // 0.00 < t <= 0.22
                    step(colorStopPos1, t) * step(t, colorStopPos2) * mix(colorStopColor1, colorStopColor2,
                        (t - colorStopPos1) / (colorStopPos2 - colorStopPos1)) + // 0.22 < t <= 0.75
                    step(colorStopPos2, t) * step(t, colorStopPos3) * mix(colorStopColor2, colorStopColor3,
                        (t - colorStopPos2) / (colorStopPos3 - colorStopPos2)) + // 0.75 < t <= 0.90
                    step(colorStopPos3, t) * step(t, colorStopPos4) * mix(colorStopColor3, colorStopColor4,
                        (t - colorStopPos3) / (colorStopPos4 - colorStopPos3)) + // 0.90 < t <= 1.00
                    step(colorStopPos4, t) * colorStopColor4; // t > 1.00
 
                return colorValue;
            }
            // **************************** Main Functions ****************************
            // The main fuction uses two moving belts to simulate the movements of the particles.

            half4 main(vec2 fragCoord)
            {
                half innerRadiusEdge = globalRadius * 0.6;
                half outerRadiusEdge = globalRadius * 1.5;

                half2 centeredUVs = (fragCoord.xy * 2. - iResolution.xy) *
                    (1.0 / min(iResolution.x, iResolution.y)); // Normalize the coordinates to [-1, 1]

                centeredUVs = centeredUVs;
                half2 directionVector = centeredUVs - rotationCenter;
                half angle = PI2 * 2.;

                half blurFactor = 0.10;
                half sigma = 10.;  // controls the blur spread
                half normalizationFactor = 1.0 / (sigma * SQRTPI2);

                // Final blurred color accumulator
                half4 finalColor = half4(0.0);

                half colorRotScale = globalRadius * PI * 0.5; // make the ring colors rotated.
                mat2 rotColorRing =
                    mat2(cos(colorRotScale), - sin(colorRotScale), sin(colorRotScale), cos(colorRotScale));
                // Get the color of each pixel from the colorbar determined by the UX
                half3 colorRing = getColorFromColorbar(centeredUVs * rotColorRing, globalRadius);

                if (length(centeredUVs) >= globalRadius * 0.6 && length(centeredUVs) <= globalRadius * 1.3) {
                    float weightSum1 = 0.0;
                    float weight1;
                    vec4 color1 = vec4(0.);

                  	float weightSum2 = 0.0;
                    float weight2;
                    vec4 color2 = vec4(0.);

                    for (int i = -NUMBER_OF_SAMPLES / 2; i <= NUMBER_OF_SAMPLES / 2; i++) {
                        float weight1 = normalizationFactor * exp(-0.1 * pow(float(i) / sigma, 2.0));
                        float rotationFactor1 = float(i) * blurFactor / float(NUMBER_OF_SAMPLES);
                        vec2 rotatedDir1 = mat2(cos(angle * rotationFactor1), -sin(angle * rotationFactor1),
                            sin(angle * rotationFactor1), cos(angle * rotationFactor1)) * directionVector;
                        vec2 haloRing1 = simpleHaloRingShape(rotationCenter + rotatedDir1, rotationCenter,
                            globalRadius, randomNoise, width1 + 0.01 * globalRadius);
                        color1 += vec4(haloRing1.x) * weight1;
                        weightSum1 += weight1;  // Accumulate the weight for normalization

                      	float weight2 = normalizationFactor * exp(-0.1 * pow(float(i) / sigma, 2.0));
                        float rotationFactor2 = float(i) * blurFactor / float(NUMBER_OF_SAMPLES);
                        vec2 rotatedDir2 = mat2(cos(angle * rotationFactor2), -sin(angle * rotationFactor2),
                            sin(angle * rotationFactor2), cos(angle * rotationFactor2)) * directionVector;
                        vec2 haloRing2 = simpleHaloRingShape(rotationCenter + rotatedDir2, rotationCenter,
                            globalRadius * (0.05 * globalRadius + 0.92), randomNoise, width2 + 0.01 * globalRadius);
                        color2 += vec4(haloRing2.x) * weight2;
                        weightSum2 += weight2;  // Accumulate the weight for normalization
                    }
                    color1 /= weightSum1 * 0.8;
                    finalColor = finalColor + color1 - finalColor * color1;
                    
                    color2 /= weightSum2 * 2.;
                    finalColor = finalColor + color2 - finalColor * color2;
                }

                half4 glow = clamp(pow(0.01 / abs(length(centeredUVs - rotationCenter) - globalRadius *
                    (0.05 * globalRadius + 0.90)), 3.), 0., 0.3) * half4(colorRing, 0.4 * globalRadius);
                finalColor = finalColor + glow - finalColor * glow; // Add a glow circle
                fragColor = finalColor * vec4(colorRing, 0.8);

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
} // namespace Rosen
} // namespace OHOS