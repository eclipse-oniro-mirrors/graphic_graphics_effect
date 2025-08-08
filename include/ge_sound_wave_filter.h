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
REGISTER_GEFILTER_TYPEINFO(SOUND_WAVE, GESoundWaveFilter,
                           Drawing::GESoundWaveFilterParams);
class GESoundWaveFilter : public GEShaderFilter {
public:
    GESoundWaveFilter(const Drawing::GESoundWaveFilterParams& params);
    ~GESoundWaveFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GESoundWaveFilter);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string& Type() const override;
 
private:
    void CheckSoundWaveParams();
    void CheckSoundWaveColor4f(Drawing::Color4f& color);
    std::shared_ptr<Drawing::RuntimeEffect> GetSoundWaveEffect();
    // sound wave
    Drawing::Color4f colorA_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Drawing::Color4f colorB_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Drawing::Color4f colorC_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float colorProgress_ = 0.0f;
    float soundIntensity_ = 0.0f;

    // shock wave
    float shockWaveAlphaA_ = 1.0f;
    float shockWaveAlphaB_ = 1.0f;
    float shockWaveProgressA_ = 0.0f;
    float shockWaveProgressB_ = 0.0f;
    float shockWaveTotalAlpha_ = 1.0f;
    static const std::string type_;

    inline static const std::string shaderStringSoundWave = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half3 colorA;
        uniform half3 colorB;
        uniform half3 colorC;
        uniform half colorProgress;
        uniform half soundIntensity;
        uniform half shockWaveAlphaA;
        uniform half shockWaveAlphaB;
        uniform half shockWaveProgressA;
        uniform half shockWaveProgressB;
        uniform half shockWaveTotalAlpha;

        const half circleRadius = 0.125;

        half smin(half a, half b, half k)
        {
            half k6 = k * 6.0;
            half h = max(k6 - abs(a - b), 0.0) / k6;
            return min(a, b) - h * h * h * k;
        }

        half3 colorWheel(half2 uv, half animationTime)
        {
            half mask = length(uv) / (circleRadius + 2.0);
            if (mask >= 1.0) {
                return half3(0.0);
            }
            half distanceFromCenter = fract(mask - animationTime);

            half3 color = distanceFromCenter < 0.2 ? mix(colorA, colorB, smoothstep(0.0, 0.2, distanceFromCenter))
                : distanceFromCenter < 0.6 ? mix(colorB, colorC, smoothstep(0.2, 0.6, distanceFromCenter))
                : mix(colorC, colorA, smoothstep(0.6, 1.0, distanceFromCenter));

            return color;
        }
 
        half4 soundWaveDistortionEffects(half2 screenUVs, half2 centeredUVs, half animationTime)
        {
            half2 lightPulseUVs = centeredUVs + half2(0.0, 0.1); // uv minus pulse center position
            half frequency = fract(animationTime); // frequency of distortion waves
            half radius = mix(0.14, 0.52, frequency);
            half lightPulseDistance = length(lightPulseUVs) - radius;

            half lightPulseThickness = 0.12;
            half lightPulse = smoothstep(lightPulseThickness, -0.025, abs(lightPulseDistance));
            if (lightPulse > 0.0) {
                half animationMask = smoothstep(1.0, 0.4, frequency);
 
                half2 directionVector = normalize(lightPulseUVs);
                half2 normal = directionVector * lightPulseDistance * lightPulse * animationMask;
                half2 refractedUVs = clamp(mix(screenUVs, screenUVs - normal * 0.25, 0.3), 0.001, 0.999);
                return half4(refractedUVs, max(normal.y, 0.0), pow(lightPulse, 6.0) * 0.3);
            }
            return half4(screenUVs, 0.0, 0.0);
        }
 
        half soundWaveLightEffects(half2 centeredUVs)
        {
            // Control the height of the circle
            half circleHeight = mix(-0.4, 0.03, soundIntensity);

            half smoothUnionThreshold = mix(0.0657, 0.09, soundIntensity);
            half2 circlePosition = half2(0.0, circleHeight);

            half barPosition = mix(0.09, 0.0, soundIntensity);
            half circleSDF = length(centeredUVs - circlePosition) - circleRadius;
            centeredUVs.y += barPosition;

            circleSDF += smoothUnionThreshold;
            half smoothUnionDistance = smin(circleSDF, centeredUVs.y, smoothUnionThreshold);

            half horizontalGradient = smoothstep(0.75, 0.0, abs(centeredUVs.x));
            half smoothGap = mix(0.08, 0.1085, horizontalGradient);
            half smoothUnion = smoothstep(smoothGap, -0.035, mix(0.0, 0.66, smoothUnionDistance));

            half verticalGradient = centeredUVs.y - barPosition;
			half verticalGap = max(smoothUnionThreshold - barPosition, 1e-4); // minEpsilon in half is 2^-14
			verticalGradient = 1.0 - min(verticalGap, verticalGradient) / verticalGap;
            half gradient = mix(1.0, horizontalGradient, 1.0 - verticalGradient) * horizontalGradient;
            smoothUnion *= gradient;

            return smoothUnion;
        }
 
        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord.xy / iResolution.xy;
            uv.y = 1.0 - uv.y;

            const half screenHeight = 0.25; // height of sound card in application
            half screenRatio = screenHeight * iResolution.x / iResolution.y;
            half2 centeredUVs = uv + uv - half2(1.0, 0.0);
            centeredUVs.y *= screenHeight;
            centeredUVs.x *= screenRatio;

            // Shock wave distort
            half additionalColorStrength = 0.0;
            if (shockWaveTotalAlpha > 0.0) {
                half4 soundWaveDistortionA =
                    soundWaveDistortionEffects(uv, centeredUVs, shockWaveProgressA);
                half4 soundWaveDistortionB =
                    soundWaveDistortionEffects(soundWaveDistortionA.xy, centeredUVs, shockWaveProgressB);
                uv = soundWaveDistortionB.xy;
                half AlphaA = shockWaveAlphaA * shockWaveTotalAlpha;
                half AlphaB = shockWaveAlphaB * shockWaveTotalAlpha;
                additionalColorStrength += (soundWaveDistortionA.z + soundWaveDistortionA.w) * AlphaA;
                additionalColorStrength += (soundWaveDistortionB.z + soundWaveDistortionB.w) * AlphaB;
            }

            // Sound wave Effect
            if (centeredUVs.y < 0.214 * soundIntensity + 0.0776) {
                additionalColorStrength += soundWaveLightEffects(centeredUVs);
            }

            half3 finalColor = image.eval(half2(uv.x, 1.0 - uv.y) * iResolution.xy).rgb;
            half3 centerColor = additionalColorStrength > 0.0 ?
                                colorWheel(centeredUVs, colorProgress) : half3(0.0);
            finalColor += centerColor * additionalColorStrength;

            return half4(finalColor, 1.0);
        }
    )";
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SOUND_WAVE_FILTER_H