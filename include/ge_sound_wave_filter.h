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
    std::shared_ptr<Drawing::RuntimeEffect> GetSoundWaveEffect();
    // sound wave
    Drawing::Color colorA_ = Drawing::Color::COLOR_WHITE;
    Drawing::Color colorB_ = Drawing::Color::COLOR_WHITE;
    Drawing::Color colorC_ = Drawing::Color::COLOR_WHITE;
    float colorProgress_ = 0.0f;
    float centerBrightness_ = 1.0f;
    float soundIntensity_ = 0.0f;

    // shock wave
    float shockWaveAlphaA_ = 1.0f;
    float shockWaveAlphaB_ = 1.0f;
    float shockWaveProgressA_ = 0.0f;
    float shockWaveProgressB_ = 0.0f;

    inline static const std::string shaderStringSoundWave = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform vec3 colorA;
        uniform vec3 colorB;
        uniform vec3 colorC;
        uniform half colorProgress;
        uniform half centerBrightness;
        uniform half soundIntensity;
        uniform half shockWaveAlphaA;
        uniform half shockWaveAlphaB;
        uniform half shockWaveProgressA;
        uniform half shockWaveProgressB;
 
        float smin(float a, float b, float k)
        {
            k *= 6.0;
            float h = max(k-abs(a-b), 0.0)/k;
            return min(a, b) - h*h*h*k*(1.0/6.0);
        }

        // Create a smooth color gradient effect based on the threshold over X or Y or user defined
        // Ideal for movement, but not for rotation
        vec3 colorGradient(vec3 colorA, vec3 colorB, float startPos, float endPos, float threshold)
        {
            float stepValue = (threshold >= startPos && threshold <= endPos) ? 1.0 : 0.0;
            vec3 returnValue = mix(colorA, colorB, smoothstep(startPos, endPos, threshold)) * stepValue;
            return returnValue;
        }
 
        vec3 colorWheel(vec2 uv, vec2 circleCenter, float circleRadius, float animationTime)
        {
            float mask = length(uv+circleCenter)/circleRadius;
            float distanceFromCenter = fract(mask-animationTime);
 
            vec3 color = colorGradient(colorA, colorB, 0.0, 0.2, distanceFromCenter)
                         + colorGradient(colorB, colorC, 0.2, 0.6, distanceFromCenter)
                         + colorGradient(colorC, colorA, 0.6, 1.0, distanceFromCenter);
            color *= (1.0 - step(1.0, mask));
            return color;
        }
 
        vec4 soundWaveDistortionEffects(vec2 screenUVs, vec2 centeredUVs, float animationTime)
        {
            vec2 lightPulseUVs = centeredUVs + vec2(0.0, 1.12); // uv minus pulse center position
            float frequency = fract(animationTime); // frequency of distortion waves
            float radius = mix(0.17, 0.68, frequency);
            float lightPulseDistance = length(lightPulseUVs) - radius;
 
            float lightPulseThickness = 0.12;
            float lightPulse = smoothstep(lightPulseThickness, -0.025, abs(lightPulseDistance));
            if (lightPulse > 0.0)
            {
                float animationMask = smoothstep(1.0, 0.4, frequency);
 
                vec2 directionVector = normalize(lightPulseUVs);
                vec2 normal = directionVector * lightPulseDistance * lightPulse*animationMask;
                vec2 refractedUVs = clamp(mix(screenUVs, screenUVs - normal * 0.25, 0.3), 0.001, 0.999);
                return vec4(refractedUVs, max(normal.y,0.0), lightPulse);
            }
            return vec4(screenUVs, 0.0, 0.0);
        }
 
        vec3 soundWaveLightEffects(vec2 uv, vec2 centeredUVs, vec3 currentColor, vec3 centerColor)
        {
            float circleRadius = 0.125;
 
            // Control the height of the circle
            float circleHeight = mix(-0.2, 0.03, soundIntensity);
            float spreadX = pow(100.0, -soundIntensity) + 1.0;// Control the spread of the mask across X
 
            float smoothUnionThreshold = mix(0.0657, 0.09, soundIntensity);
            float horizonOffset = -0.02;
            vec2 circlePosition = vec2(0.0, circleHeight);
            centeredUVs.y += 1.0;
            centeredUVs.y += mix(0.09, 0.0, soundIntensity);
            float circleSDF = length(centeredUVs-circlePosition)-circleRadius ;
            circleSDF += smoothUnionThreshold;
            float smoothUnionDistance = smin(circleSDF, centeredUVs.y-horizonOffset, smoothUnionThreshold);
            float horizontalGradient = smoothstep(0.9 * spreadX, 0.0, abs(uv.x*2.0-1.0));
            float smoothGap = mix(0.08, 0.1085, horizontalGradient);
            float smoothUnion = smoothstep(smoothGap, -0.035, mix(0.0, 0.66, smoothUnionDistance));
 
            // Control the spread of the mask across X
            smoothUnion *= mix(0.65, 1.0, horizontalGradient);
 
            float brightnessValue = centerBrightness *smoothstep(5.0, 0.0, circleSDF);
 
            return currentColor+centerColor*smoothUnion* brightnessValue;
        }
 
        half4 main(float2 fragCoord) {
            vec2 uv = fragCoord.xy/iResolution.xy;
            uv.y = 1.0 - uv.y;
            if (uv.y>0.3) {
                return vec4(0.0);
            }
            float screenRatio = iResolution.x/iResolution.y;
            vec2 centeredUVs = uv*2.0 - 1.0;
            centeredUVs.x *= screenRatio;
            vec2 screenUVs = uv;
 
            vec3 finalColor = vec3(0.);
            vec3 centerColor = colorWheel(centeredUVs, vec2(0.0, 1.0), 2.125, colorProgress);
 
            // Shock wave distort
            vec4 soundWaveDistortionA = vec4(0.0);
            vec4 soundWaveDistortionB = vec4(0.0);
            soundWaveDistortionA = soundWaveDistortionEffects(uv, centeredUVs, shockWaveProgressA);
            soundWaveDistortionB = soundWaveDistortionEffects(soundWaveDistortionA.xy, centeredUVs, shockWaveProgressB);
            uv = soundWaveDistortionB.xy;
 
            finalColor.rgb = image.eval(vec2(uv.x, 1.0 - uv.y) * iResolution.xy).rgb;
            // Sound wave Effect
            finalColor.rgb = soundWaveLightEffects(screenUVs, centeredUVs, finalColor.rgb, centerColor);
 
            // Shock wave Effect: Add sutil light from the refraction distortion
            finalColor.rgb = finalColor.rgb + centerColor * vec3(soundWaveDistortionA.z) * shockWaveAlphaA;
            finalColor.rgb = finalColor.rgb + centerColor * vec3(pow(soundWaveDistortionA.w, 6.0))*0.3 * shockWaveAlphaA;
 
            finalColor.rgb = finalColor.rgb + centerColor * vec3(soundWaveDistortionB.z) * shockWaveAlphaB;
            finalColor.rgb = finalColor.rgb + centerColor * vec3(pow(soundWaveDistortionB.w, 6.0))*0.3 * shockWaveAlphaB;
 
            return vec4(finalColor, 1.0);
        }
    )";
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SOUND_WAVE_FILTER_H