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
#ifndef GRAPHICS_EFFECT_GE_SOUND_WAVE_FILTER_H
#define GRAPHICS_EFFECT_GE_SOUND_WAVE_FILTER_H
 
#include <memory>
 
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
 
#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"
 
namespace OHOS {
namespace Rosen {
class GESoundWaveFilter : public GEShaderFilter {
public:
    GESoundWaveFilter(const Drawing::GESoundWaveFilterParams& params);
    ~GESoundWaveFilter() override = default;
 
    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;
 
private:
    void CheckSoundWaveParams();
    void CheckSoundWaveColor4f(Drawing::Color4f& color);
    std::shared_ptr<Drawing::RuntimeEffect> GetSoundWaveEffect();
    // sound wave
    Drawing::Color4f colorA_ = {1.0, 1.0, 1.0, 1.0};
    Drawing::Color4f colorB_ = {1.0, 1.0, 1.0, 1.0};
    Drawing::Color4f colorC_ = {1.0, 1.0, 1.0, 1.0};
    float colorProgress_ = 0.0f;
    float soundIntensity_ = 0.0f;

    // shock wave
    float shockWaveAlphaA_ = 1.0f;
    float shockWaveAlphaB_ = 1.0f;
    float shockWaveProgressA_ = 0.0f;
    float shockWaveProgressB_ = 0.0f;
    float shockWaveTotalAlpha_ = 1.0f;

    inline static const std::string shaderStringSoundWave = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform vec3 colorA;
        uniform vec3 colorB;
        uniform vec3 colorC;
        uniform half colorProgress;
        uniform half soundIntensity;
        uniform half shockWaveAlphaA;
        uniform half shockWaveAlphaB;
        uniform half shockWaveProgressA;
        uniform half shockWaveProgressB;
        uniform half shockWaveTotalAlpha;

        const float circleRadius = 0.125;

        float smin(float a, float b, float k)
        {
            k *= 6.0;
            float h = max(k - abs(a - b), 0.0) / k;
            return min(a, b) - h * h * h * k * (1.0 / 6.0);
        }

        // Create a smooth color gradient effect based on the threshold over X or Y or user defined
        // Ideal for movement, but not for rotation
        vec3 colorGradient(vec3 colorA, vec3 colorB, float startPos, float endPos, float threshold)
        {
            float stepValue = (threshold >= startPos && threshold <= endPos) ? 1.0 : 0.0;
            vec3 returnValue = mix(colorA, colorB, smoothstep(startPos, endPos, threshold)) * stepValue;
            return returnValue;
        }
 
        vec3 colorWheel(vec2 uv, float animationTime)
        {
            float mask = length(uv) / (circleRadius + 2.0);
            float distanceFromCenter = fract(mask - animationTime);

            vec3 color = colorGradient(colorA, colorB, 0.0, 0.2, distanceFromCenter) +
                        colorGradient(colorB, colorC, 0.2, 0.6, distanceFromCenter) +
                        colorGradient(colorC, colorA, 0.6, 1.0, distanceFromCenter);
            color *= (1.0 - step(1.0, mask));
            return color;
        }
 
        vec4 soundWaveDistortionEffects(vec2 screenUVs, vec2 centeredUVs, float animationTime)
        {
            vec2 lightPulseUVs = centeredUVs + vec2(0.0, 0.1);        // uv minus pulse center position
            float frequency = fract(animationTime);  // frequency of distortion waves
            float radius = mix(0.14, 0.52, frequency);
            float lightPulseDistance = length(lightPulseUVs) - radius;

            float lightPulseThickness = 0.12;
            float lightPulse = smoothstep(lightPulseThickness, -0.025, abs(lightPulseDistance));
            if (lightPulse > 0.0) {
                float animationMask = smoothstep(1.0, 0.4, frequency);
 
                vec2 directionVector = normalize(lightPulseUVs);
                vec2 normal = directionVector * lightPulseDistance * lightPulse*animationMask;
                vec2 refractedUVs = clamp(mix(screenUVs, screenUVs - normal * 0.25, 0.3), 0.001, 0.999);
                return vec4(refractedUVs, max(normal.y,0.0), lightPulse);
            }
            return vec4(screenUVs, 0.0, 0.0);
        }
 
        vec3 soundWaveLightEffects(vec2 centeredUVs, vec3 currentColor, vec3 centerColor)
        {
            // Control the height of the circle
            float circleHeight = mix(-0.4, 0.03, soundIntensity);

            float smoothUnionThreshold = mix(0.0657, 0.09, soundIntensity);
            vec2 circlePosition = vec2(0.0, circleHeight);

            float barPosition = mix(0.09, 0.0, soundIntensity);
            float circleSDF = length(centeredUVs - circlePosition) - circleRadius;
            centeredUVs.y += barPosition;

            circleSDF += smoothUnionThreshold;
            float smoothUnionDistance = smin(circleSDF, centeredUVs.y, smoothUnionThreshold);

            float horizontalGradient = smoothstep(0.75, 0.0, abs(centeredUVs.x));
            float smoothGap = mix(0.08, 0.1085, horizontalGradient);
            float smoothUnion = smoothstep(smoothGap, -0.035, mix(0.0, 0.66, smoothUnionDistance));

            float verticalGradient = centeredUVs.y - barPosition;
            verticalGradient =
                1.0 - min(smoothUnionThreshold - barPosition, verticalGradient) / (smoothUnionThreshold - barPosition);
            float gradient = mix(1.0, horizontalGradient, 1.0 - verticalGradient) * horizontalGradient;
            smoothUnion *= gradient;

            return currentColor + centerColor * smoothUnion;
        }
 
        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord.xy / iResolution.xy;
            uv.y = 1.0 - uv.y;

            vec2 adjustedResolution = vec2(iResolution.x, iResolution.y * 4.);
            float screenRatio = adjustedResolution.x / adjustedResolution.y;
            vec2 centeredUVs = uv * 2.0 - 1.0 + vec2(0.0, 1.0);
            centeredUVs *= iResolution.xy / adjustedResolution;
            centeredUVs.x *= screenRatio;

            vec3 finalColor = vec3(0.0);
            vec3 centerColor = colorWheel(centeredUVs, colorProgress);

            // Shock wave distort
            vec4 soundWaveDistortionA = vec4(0.0);
            vec4 soundWaveDistortionB = vec4(0.0);
            if (shockWaveTotalAlpha > 0.0) {
                soundWaveDistortionA = soundWaveDistortionEffects(uv, centeredUVs, shockWaveProgressA);
                soundWaveDistortionB =
                    soundWaveDistortionEffects(soundWaveDistortionA.xy, centeredUVs, shockWaveProgressB);
                uv = soundWaveDistortionB.xy;
            }

            finalColor = image.eval(vec2(uv.x, 1.0 - uv.y) * iResolution.xy).rgb;
            // Sound wave Effect
            finalColor = soundWaveLightEffects(centeredUVs, finalColor, centerColor);

            if (shockWaveTotalAlpha > 0.0) {
                // Shock wave Effect: Add sutil light from the refraction distortion
                float AlphaA = shockWaveAlphaA * shockWaveTotalAlpha;
                float AlphaB = shockWaveAlphaB * shockWaveTotalAlpha;
                finalColor += centerColor * soundWaveDistortionA.z * AlphaA;
                finalColor += centerColor * pow(soundWaveDistortionA.w, 6.0) * 0.3 * AlphaA;
                finalColor += centerColor * soundWaveDistortionB.z * AlphaB;
                finalColor += centerColor * pow(soundWaveDistortionB.w, 6.0) * 0.3 * AlphaB;
            }

            return vec4(finalColor, 1.0);
        }
    )";
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SOUND_WAVE_FILTER_H