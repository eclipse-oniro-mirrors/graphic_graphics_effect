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

namespace {
    constexpr float DOWN_SAMPLE_SCALE = 4.0;
    static constexpr char GLOW_HALO_PROG[] = R"(
        uniform half2 iResolution;
        uniform half2 rotationCenter;  // center of the halo rings, normalized coords (0. - 10.)
        uniform float DOWN_SAMPLE_SCALE;

        vec4 fragColor;
        const float PI = 3.14159;
        const float PI2 = 6.28318;

        const half2 RAND2_A = half2(127.1, 1.7);
        const half2 RAND2_B = half2(269.5, 183.3);
        const float RAND2_MUL = 4258.5453123;
        // ****************************** Sub-functions ******************************
        half2 Random2D(half2 st)
        {
            st = half2(dot(st, RAND2_A), dot(st, RAND2_B));
            return -1.0 + 2.0 * fract(sin(st) * RAND2_MUL);
        }

        float Noise2D(half2 st)
        {
            half2 i = floor(st);
            half2 f = fract(st);
            half2 u = smoothstep(0.0, 1.0, f);
            return mix(mix(dot(Random2D(i + half2(0.0, 0.0)), f - half2(0.0, 0.0)),
                           dot(Random2D(i + half2(1.0, 0.0)), f - half2(1.0, 0.0)), u.x),
                       mix(dot(Random2D(i + half2(0.0, 1.0)), f - half2(0.0, 1.0)),
                           dot(Random2D(i + half2(1.0, 1.0)), f - half2(1.0, 1.0)), u.x),
                       u.y);
        }

        // Simple 1D pseudo-random hash value
        float Random1D(float x)
        {
            return fract(sin(x) * RAND2_MUL);
        }

        float Noise1D(float t)
        {
            float f = fract(t);
            // Cubic Hermite Curve. Same as SmoothStep()
            float u = smoothstep(0.0, 1.0, f);
            return mix(Random1D(floor(t)), Random1D(floor(t) + 1.0), u);
        }

        // Create animated, noisy halo border lines
        float CentralAmbienceHaloBorder(half2 uv, half2 polarCoords, float radius, float animationTime,
            float rotationTimeScale, float noiseScale, float noiseDisplacement)
        {
            polarCoords.x = fract((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8));
            float screenNoise = Noise2D(half2(uv + (animationTime * rotationTimeScale)) * noiseScale);
            float angularRandomVal = Noise1D((polarCoords.x) * 10.0);
            angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x * 2.0 - 1.0)));
            radius += (screenNoise * noiseDisplacement);
            polarCoords.y -= radius;
            float thickness = mix(0.01, 0.035, angularRandomVal);
            float circleBorder = smoothstep(thickness * radius, 0.0, abs(polarCoords.y));
            circleBorder *= smoothstep(0.1, 1.0, angularRandomVal);
            circleBorder *= screenNoise * 0.5 + 0.5;
            return circleBorder;
        }

        // Create soft glow inside halo
        float CentralAmbienceHaloGlow(half2 polarCoords, float animationTime, float radius, float glowRadius,
            float glowExposure)
        {
            polarCoords.x = fract(((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8)));
            float angularRandomVal = Noise1D(polarCoords.x * 10.);
            angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x * 2.0 - 1.0)));
            polarCoords.y -= radius;
            float circleBorderGlow = smoothstep(glowRadius, 0.0, abs(polarCoords.y)) * glowExposure;
            return circleBorderGlow;
        }

        float BlendScreen(float a, float b)
        {
            return a + b - a * b;
        }

        half4 BlendScreen(half4 a, half4 b)
        {
            return a + b - a * b;
        }

        // ****************************** Main Functions ******************************
        vec4 main(vec2 fragCoord)
        {
            float globalRadius = 0.5 / DOWN_SAMPLE_SCALE;
            float radius = globalRadius * 2.0;
            float innerRadiusEdge = 0.5 * radius;
            float outerRadiusEdge = 1.35 * radius;
            float glowColor = 0.;
            float solidColor = 0.0;

            half2 uv = fragCoord.xy / iResolution.xy;
            uv -= rotationCenter;
            float screenRatio = iResolution.x / iResolution.y;
            half2 centeredUVs = uv * 2.0 ;
            centeredUVs.x *= screenRatio;
            half2 directionVector = centeredUVs;
            float lenDirVec = length(directionVector);
            half2 polarCoords =
                half2((atan(directionVector.y, directionVector.x) + PI) / (2.0 * PI), lenDirVec);

            if (lenDirVec > 0.5) {
                fragColor = vec4(0.);
            } else {
                float ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius * 0.82,
                    globalRadius * 0.1, 1.0, 1.25, 0.05) * 2.5;
                ambienceHaloBorder = clamp(ambienceHaloBorder, 0., 1.);
                solidColor = BlendScreen(solidColor, ambienceHaloBorder);
                ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius * 0.95,
                    globalRadius * 0.25, 1.0, 0.25, 0.05);
                solidColor = BlendScreen(solidColor, ambienceHaloBorder);
                ambienceHaloBorder = CentralAmbienceHaloBorder(directionVector, polarCoords, radius,
                    globalRadius * 0.35, 1.0, 0.25, 0.05);
                solidColor = BlendScreen(solidColor, ambienceHaloBorder);
                float ambienceHaloGlow = CentralAmbienceHaloGlow(polarCoords, 1.0, radius * 0.83, 0.2 * radius,
                    0.4);
                float glow = clamp(ambienceHaloGlow, 0., 1.);
                glowColor = BlendScreen(glowColor, glow);  // Add a glow circle
            }

            fragColor = BlendScreen(half4(half3(solidColor), 0.0), half4(glowColor));
            return fragColor;
        }
    )";

    static constexpr char PARTICLE_HALO_PROG[] = R"(
        uniform half2 iResolution;
        uniform half randomNoise;
        uniform half2 rotationCenter; // center of the halo rings, normalized coords (0. - 1.)
        uniform half DOWN_SAMPLE_SCALE;

        half4 fragColor;
        // ****************************** Constants ******************************
        const half WIDTH = 0.055;             // Width of the halo rings
        const half PI = 3.14159;
        const half PI2 = 6.28318;

        const half2 RAND2_A = half2(127.1, 1.7);
        const half2 RAND2_B = half2(269.5, 183.3);
        const half RAND2_MUL = 2358.5453123;

        // ****************************** Sub-functions ******************************
        half2 Random2D(half2 st)
        {
            st = half2(dot(st, RAND2_A), dot(st, RAND2_B));
            return -1.0 + 2.0 * fract(sin(st) * RAND2_MUL);
        }

        half Noise2D(half2 st)
        {
            half2 i = floor(st);
            half2 f = fract(st);
            half2 u = smoothstep(0.0, 1.0, f);
            return mix(mix(dot(Random2D(i + half2(0.0, 0.0)), f - half2(0.0, 0.0)),
                           dot(Random2D(i + half2(1.0, 0.0)), f - half2(1.0, 0.0)), u.x),
                       mix(dot(Random2D(i + half2(0.0, 1.0)), f - half2(0.0, 1.0)),
                           dot(Random2D(i + half2(1.0, 1.0)), f - half2(1.0, 1.0)), u.x),
                       u.y);
        }

        half Random1D(half x)
        {
            return fract(sin(x) * RAND2_MUL);
        }

        half Noise1D(half t)
        {
            half f = fract(t);
            half u = smoothstep(0.0, 1.0, f);
            return mix(Random1D(floor(t)), Random1D(floor(t) + 1.0), u);
        }

        half2 ShapePerturbation(half2 uv, half noiseVariation, half noiseScale, half noiseStrength)
        {
            half screenNoise = Noise2D(half2(uv.x - noiseVariation, uv.y + noiseVariation) * noiseScale);
            return uv + screenNoise * noiseStrength;
        }

        half SimpleHaloRingShape(half2 uv,  half radius, half noiseVariation, half haloThickness)
        {
            uv = ShapePerturbation(uv, noiseVariation, 1.0, radius * 0.6);
            half2 polarCoords = half2((atan(uv.y, uv.x) + PI) / (2.0 * PI), length(uv));
            polarCoords.y -= radius;
            half angularRandomVal = Noise1D((polarCoords.x) * 10.0);
            // ensure that the radial noise is seamless
            angularRandomVal = mix(angularRandomVal, 0.5, smoothstep(0.9, 1.0, abs(polarCoords.x * 2.0 - 1.0)));
            half thickness = mix(haloThickness, haloThickness * 4.0, angularRandomVal);
            half circleBorder = smoothstep(thickness, thickness * 0.2, abs(polarCoords.y));
            return clamp(circleBorder, 0.0, 1.0);
        }
        // ****************************** Main Functions ******************************
        half4 main(vec2 fragCoord)
        {
          	half globalRadius = 0.5 / DOWN_SAMPLE_SCALE;
            half radius = globalRadius * 2.;
            half innerRadiusEdge = 0.5 * radius;
            half outerRadiusEdge = 1.35 * radius;
            half haloColor = 0.;

            half2 uv = fragCoord.xy / iResolution.xy;
            half screenRatio = iResolution.x / iResolution.y;
            uv -= rotationCenter;
            half2 centeredUVs = uv * 2.0;
            centeredUVs.x *= screenRatio;
            half2 directionVector = centeredUVs;
            half lenDir = length(directionVector);
            if (lenDir > 0.5) {
                fragColor = vec4(0.);
            } else {
                half2 polarCoords =
                half2((atan(directionVector.y, directionVector.x) + PI) / (2.0 * PI), lenDir);

                if (lenDir >= innerRadiusEdge && lenDir <= outerRadiusEdge) {
                    half particleHaloColor = 0.;
                    half haloRing = SimpleHaloRingShape(directionVector, radius, randomNoise, WIDTH * radius);
                    particleHaloColor += haloRing;
                    haloColor = haloColor + particleHaloColor - haloColor * particleHaloColor;
                }
                fragColor = half4(haloColor);
            }
            return fragColor;
        }
    )";

    static constexpr char MAIN_SHADER_PROG[] = R"(
        uniform half2 iResolution;
        uniform float globalRadius;   // global radius controlling the overall size of halos
        uniform half2 rotationCenter;  // center of the halo rings, normalized coords (0. - 1.)
        uniform shader particleHalo;
        uniform shader glowHalo;
        uniform float DOWN_SAMPLE_SCALE;

        vec4 fragColor;
        const int NUMBER_OF_SAMPLES = 11;  // Number of samples used for blur effects
        const float NUMBER_OF_SAMPLES_F = 11.;
        const float PI = 3.14159;
        const float PI2 = 6.28318;

        // Color stops for the color bar
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

        const half2 RAND2_A = half2(127.1, 1.7);
        const half2 RAND2_B = half2(269.5, 183.3);
        const float RAND2_MUL = 4258.5453123;

        const float COS_ROT1 = 0.993482;
        const float COS_ROT2 = 0.974012;
        const float COS_ROT3 = 0.941844;
        const float COS_ROT4 = 0.897398;
        const float COS_ROT5 = 0.841254;
        const float COS_ROT6 = 0.774142;
        const float COS_ROT7 = 0.696938;
        const float COS_ROT8 = 0.610648;
        const float COS_ROT9 = 0.516397;
        const float COS_ROT10 = 0.415415;

        const float SIN_ROT1 = 0.113991;
        const float SIN_ROT2 = 0.226497;
        const float SIN_ROT3 = 0.336049;
        const float SIN_ROT4 = 0.441221;
        const float SIN_ROT5 = 0.540641;
        const float SIN_ROT6 = 0.633012;
        const float SIN_ROT7 = 0.717132;
        const float SIN_ROT8 = 0.791902;
        const float SIN_ROT9 = 0.856349;
        const float SIN_ROT10 = 0.909632;

        // ****************************** Sub-functions ******************************
        half2 Random2D(half2 st)
        {
            st = half2(dot(st, RAND2_A), dot(st, RAND2_B));
            return -1.0 + 2.0 * fract(sin(st) * RAND2_MUL);
        }

        float Noise2D(half2 st)
        {
            half2 i = floor(st);
            half2 f = fract(st);
            half2 u = smoothstep(0.0, 1.0, f);
            return mix(mix(dot(Random2D(i + half2(0.0, 0.0)), f - half2(0.0, 0.0)),
                           dot(Random2D(i + half2(1.0, 0.0)), f - half2(1.0, 0.0)), u.x),
                       mix(dot(Random2D(i + half2(0.0, 1.0)), f - half2(0.0, 1.0)),
                           dot(Random2D(i + half2(1.0, 1.0)), f - half2(1.0, 1.0)), u.x),
                       u.y);
        }

        // Simple 1D pseudo-random hash value
        float Random1(float x)
        {
            return fract(sin(x) * RAND2_MUL);
        }

        float Noise1D(float t)
        {
            float f = fract(t);
            float u = smoothstep(0.0, 1.0, f);
            return mix(Random1(floor(t)), Random1(floor(t) + 1.0), u);
        }

        // Create animated, noisy halo border lines
        float CentralAmbienceHaloBorder(half2 uv, half2 polarCoords, float radius, float animationTime,
            float rotationTimeScale, float noiseScale, float noiseDisplacement)
        {
            polarCoords.x = fract((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8));
            float screenNoise = Noise2D(half2(uv + (animationTime * rotationTimeScale)) * noiseScale);
            float angularRandomVal = Noise1D((polarCoords.x) * 10.0);
            angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x * 2.0 - 1.0)));
            radius += (screenNoise * noiseDisplacement);
            polarCoords.y -= radius;
            float thickness = mix(0.01, 0.035, angularRandomVal);
            float circleBorder = smoothstep(thickness * radius, 0.0, abs(polarCoords.y));
            circleBorder *= smoothstep(0.1, 1.0, angularRandomVal);
            circleBorder *= screenNoise * 0.5 + 0.5;
            return circleBorder;
        }

        // Create soft glow inside halo
        float CentralAmbienceHaloGlow(half2 polarCoords, float animationTime, float radius, float glowRadius,
            float glowExposure)
        {
            polarCoords.x = fract(((polarCoords.x - animationTime * 4.0) - (polarCoords.y * 0.8)));
            float angularRandomVal = Noise1D(polarCoords.x * 10.);
            angularRandomVal = mix(angularRandomVal, 0.2, smoothstep(0.5, 0.8, abs(polarCoords.x * 2.0 - 1.0)));
            polarCoords.y -= radius;
            float circleBorderGlow = smoothstep(glowRadius, 0.0, abs(polarCoords.y)) * glowExposure;
            return circleBorderGlow;
        }

        half3 GetColorFromColorbar(half2 pt, float radius)
        {
            // Map pt.x to the range [0.0, 1.0] for color interpolation
            float t = (pt.x + radius) / (2.0 * radius);
            half3 colorValue =
                step(pt.x, -radius) * STOP_COLOR0 +
                // Return the first color when pt.x is to the left of the halo
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
                step(STOP_POS4, t) * STOP_COLOR4;                           // t > 1.00
            return colorValue;
        }

        float BlendScreen(float a, float b)
        {
            return a + b - a * b;
        }

        half2 UV2PixelParticle(half2 rotatedDir, float DOWN_SAMPLE_SCALE, float screenRatio, half radius,
            half2 rotationCenter, half2 iRes)
        {
            half lenRotDir = length(rotatedDir);
            half cosBeta = rotatedDir.x / lenRotDir;
            half sinBeta = rotatedDir.y / lenRotDir;
            rotatedDir = half2(cosBeta, sinBeta) * lenRotDir / radius / DOWN_SAMPLE_SCALE;

            rotatedDir /= 2.0;
            rotatedDir.x /= screenRatio;
            rotatedDir += rotationCenter;
            return  rotatedDir * iRes;
        }
		
        // ****************************** Main Functions ******************************
        vec4 main(vec2 fragCoord)
        {
            float radius = globalRadius * 2.0;
            float innerRadiusEdge = 0.5 * radius;
            float outerRadiusEdge = 1.35 * radius;
            float haloColor = 0.;
            half4 glowColor = half4(0.0);

            half2 uv = fragCoord.xy / iResolution.xy;
            uv -= rotationCenter;
            float screenRatio = iResolution.x / iResolution.y;
            half2 centeredUVs = uv * 2.0 ;
            centeredUVs.x *= screenRatio;
            half2 directionVector = centeredUVs;
            float lenDirVec = length(directionVector);
            half2 polarCoords =
                half2((atan(directionVector.y, directionVector.x) + PI) / (2.0 * PI), lenDirVec);

            float colorRotScale = radius * PI * 0.5;  // Make the ring colors rotated.
            mat2 rotColorRing =
                mat2(cos(colorRotScale), -sin(colorRotScale), sin(colorRotScale), cos(colorRotScale));
            half3 colorRing = GetColorFromColorbar(directionVector * rotColorRing, radius);

            if (lenDirVec >= innerRadiusEdge && lenDirVec <= outerRadiusEdge) {
                float particleHaloColor = 0.;

                half2 rotatedDir = directionVector;
              	half2 samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                half4 samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT1, SIN_ROT1, -SIN_ROT1, COS_ROT1) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT2, SIN_ROT2, -SIN_ROT2, COS_ROT2) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT3, SIN_ROT3, -SIN_ROT3, COS_ROT3) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              	
              	rotatedDir = mat2(COS_ROT4, SIN_ROT4, -SIN_ROT4, COS_ROT4) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT5, SIN_ROT5, -SIN_ROT5, COS_ROT5) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT6, SIN_ROT6, -SIN_ROT6, COS_ROT6) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT7, SIN_ROT7, -SIN_ROT7, COS_ROT7) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT8, SIN_ROT8, -SIN_ROT8, COS_ROT8) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT9, SIN_ROT9, -SIN_ROT9, COS_ROT9) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
              	rotatedDir = mat2(COS_ROT10, SIN_ROT10, -SIN_ROT10, COS_ROT10) * directionVector;
                samplePixel = UV2PixelParticle(rotatedDir, DOWN_SAMPLE_SCALE, screenRatio, radius,
                    rotationCenter, iResolution.xy);
                samplerColor = particleHalo.eval(samplePixel);
                particleHaloColor += samplerColor.x;
              
                particleHaloColor /= NUMBER_OF_SAMPLES_F;

                haloColor = BlendScreen(haloColor, particleHaloColor);
				
              	half glowRadiusScale = 1.05;
                float angle = radius * PI;
                half2 rotatedUV = mat2(cos(angle), -sin(angle), sin(angle), cos(angle)) * centeredUVs;
                float lenRotUV = length(rotatedUV);
                float cosAlpha = rotatedUV.x / lenRotUV;
                float sinAlpha = rotatedUV.y / lenRotUV;
                rotatedUV = half2(cosAlpha, sinAlpha) * lenRotUV / radius / DOWN_SAMPLE_SCALE / glowRadiusScale;
                rotatedUV /= 2.0;
                rotatedUV.x /= screenRatio;
                rotatedUV += rotationCenter;
                
                glowColor = glowHalo.eval(rotatedUV * iResolution.xy);
                glowColor.rgb *= colorRing;
            }
            fragColor = half4(clamp(haloColor, 0.0, 1.0));
            fragColor.xyz *= colorRing;
            fragColor = fragColor + glowColor - fragColor * glowColor;
            return fragColor;
        }
    )";
}

using CacheDataType = struct CacheData {
    std::shared_ptr<Drawing::Image> cacheImg = nullptr;
};

GEParticleCircularHaloShader::GEParticleCircularHaloShader() {}

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

void GEParticleCircularHaloShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Drawing::ImageInfo particleHaloImg(rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    particleHaloImg_ = MakeParticleHaloShader(canvas, particleHaloImg);
    
    float currentCenterX = particleCircularHaloParams_.center_.first;
    float currentCenterY = particleCircularHaloParams_.center_.second;
    if (cacheAnyPtr_ == nullptr || lastCenterX_ != currentCenterX || lastCenterY_ != currentCenterY) {
        CacheDataType cacheData;
        Drawing::ImageInfo cacheImgInf(rect.GetWidth(), rect.GetHeight(),
            Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
        auto cacheImg = MakeGlowHaloShader(canvas, cacheImgInf);
        if (cacheImg) {
            cacheData.cacheImg = cacheImg;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
        lastCenterX_ = currentCenterX;
        lastCenterY_ = currentCenterY;
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEParticleCircularHaloShader::GetGlowHaloBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> preCalculatedGlowHaloEffect_ = nullptr;

    if (preCalculatedGlowHaloEffect_ == nullptr) {
        preCalculatedGlowHaloEffect_ = Drawing::RuntimeEffect::CreateForShader(GLOW_HALO_PROG);
    }
    if (preCalculatedGlowHaloEffect_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader preCalculatedGlowHaloEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(preCalculatedGlowHaloEffect_);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEParticleCircularHaloShader::GetParticleHaloBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> particleHaloEffect_ = nullptr;

    if (particleHaloEffect_ == nullptr) {
        particleHaloEffect_ = Drawing::RuntimeEffect::CreateForShader(PARTICLE_HALO_PROG);
    }
    if (particleHaloEffect_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::GetParticleHaloBuilder particleHaloEffect_ is "
                "nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(particleHaloEffect_);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEParticleCircularHaloShader::GetParticleCircularHaloBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> glowHaloEffect_ = nullptr;

    if (glowHaloEffect_ == nullptr) {
        glowHaloEffect_ = Drawing::RuntimeEffect::CreateForShader(MAIN_SHADER_PROG);
    }

    if (glowHaloEffect_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader:: GetParticleCircularHaloBuilder ShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(glowHaloEffect_);
}

std::shared_ptr<Drawing::Image> GEParticleCircularHaloShader::MakeGlowHaloShader(
    Drawing::Canvas& canvas, const Drawing::ImageInfo& imageInfo)
{
    float width = imageInfo.GetWidth();
    float height = imageInfo.GetHeight();
    auto glowHaloBuilder_ = GetGlowHaloBuilder();
    if (glowHaloBuilder_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader MakeGlowHaloShader preCalculatedBuilder_ is nullptr.");
        return nullptr;
    }
    glowHaloBuilder_->SetUniform("iResolution", width, height);
    glowHaloBuilder_->SetUniform("rotationCenter", particleCircularHaloParams_.center_.first,
        particleCircularHaloParams_.center_.second);
    glowHaloBuilder_->SetUniform("DOWN_SAMPLE_SCALE", DOWN_SAMPLE_SCALE);
    auto preCalculatedGlowHaloShader = glowHaloBuilder_->MakeImage(canvas.GetGPUContext().get(),
        nullptr, imageInfo, false);
    if (preCalculatedGlowHaloShader == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader preCalculatedGlowHaloShader is nullptr.");
        return nullptr;
    }
    return preCalculatedGlowHaloShader;
}

std::shared_ptr<Drawing::Image> GEParticleCircularHaloShader::MakeParticleHaloShader(
    Drawing::Canvas& canvas, const Drawing::ImageInfo& imageInfo)
{
    float width = imageInfo.GetWidth();
    float height = imageInfo.GetHeight();

    particleHaloBuilder_ = GetParticleHaloBuilder();
    if (particleHaloBuilder_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader MakeParticleHaloShader particleTextureBuilder_ is nullptr.");
        return nullptr;
    }
    particleHaloBuilder_->SetUniform("iResolution", width, height);
    particleHaloBuilder_->SetUniform("rotationCenter", particleCircularHaloParams_.center_.first,
        particleCircularHaloParams_.center_.second);
    particleHaloBuilder_->SetUniform("randomNoise", particleCircularHaloParams_.noise_);
    particleHaloBuilder_->SetUniform("DOWN_SAMPLE_SCALE", DOWN_SAMPLE_SCALE);
    auto particleHaloShader =
        particleHaloBuilder_->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (particleHaloShader == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader MakeParticleHaloShader is nullptr.");
        return nullptr;
    }
    return particleHaloShader;
}

std::shared_ptr<Drawing::ShaderEffect> GEParticleCircularHaloShader::MakeParticleCircularHaloShader(
    const Drawing::Rect& rect)
{
    if (particleHaloImg_ == nullptr) {
        GE_LOGD("GEParticleCircularHaloShader MakeParticleCircularHaloShader particleHaloImg_ is nullptr.");
        return nullptr;
    }

    if (cacheAnyPtr_ == nullptr) {
        GE_LOGD("GEParticleCircularHaloShader MakeParticleCircularHaloShader cacheAnyPtr_ is nullptr.");
        return nullptr;
    }

    auto width = rect.GetWidth();
    auto height = rect.GetHeight();

    Drawing::Matrix matrix;
    auto preCalcImg_ = std::any_cast<CacheDataType>(*cacheAnyPtr_).cacheImg;
    if (preCalcImg_ == nullptr) {
        GE_LOGD("GEParticleCircularHaloShader MakeParticleCircularHaloShader preCalcImg_ is nullptr.");
        return nullptr;
    }
    auto preCalcImgShader = Drawing::ShaderEffect::CreateImageShader(*preCalcImg_, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);

    auto particleHaloShader = Drawing::ShaderEffect::CreateImageShader(*particleHaloImg_,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);

    builder_ = GetParticleCircularHaloBuilder();
    if (builder_ == nullptr) {
        GE_LOGE("GEParticleCircularHaloShader::MakeParticleCircularHaloShader builder_ is nullptr.");
        return nullptr;
    }
    builder_->SetChild("glowHalo", preCalcImgShader);
    builder_->SetChild("particleHalo", particleHaloShader);
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("globalRadius", particleCircularHaloParams_.radius_);
    builder_->SetUniform("rotationCenter", particleCircularHaloParams_.center_.first,
        particleCircularHaloParams_.center_.second);
    builder_->SetUniform("DOWN_SAMPLE_SCALE", DOWN_SAMPLE_SCALE);
    auto particleCircularHaloShader = builder_->MakeShader(nullptr, false);
    if (particleCircularHaloShader == nullptr) {
        GE_LOGE(
            "GEParticleCircularHaloShader::MakeParticleCircularHaloShader particleCircularHaloShader is nullptr.");
        return nullptr;
    }
    return particleCircularHaloShader;
}

}  // namespace Rosen
}  // namespace OHOS