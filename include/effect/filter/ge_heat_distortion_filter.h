/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_HEAT_DISTORTION_FILTER_H
#define GRAPHICS_EFFECT_GE_HEAT_DISTORTION_FILTER_H

#include <memory>

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

class GEHeatDistortionFilter : public GEShaderFilter {
public:
    explicit GEHeatDistortionFilter(const Drawing::GEHeatDistortionFilterParams& params);
    ~GEHeatDistortionFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEHeatDistortionFilter, Drawing::GEHeatDistortionFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    void CheckHeatDistortionParams();
    std::shared_ptr<Drawing::RuntimeEffect> GetHeatDistortionEffect();

    float intensity_ = 1.0f;
    float noiseScale_ = 1.0f;
    float riseWeight_ = 0.2f;
    float progress_ = 0.0f;

    const float timeScale_ = 25.0f;
    const float intensityScale_ = 3.0f;

    inline static const std::string shaderStringHeatDistortion = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half progress;

        uniform half intensity;

        uniform half noiseScale;
        uniform half riseWeight;

        half random(half2 st)
        {
            st = half2(0.3183099, 0.3678794) * st + half2(0.25, 0.5);
            return fract(16.0 * fract(st.x * st.y * (st.x + st.y)));
        }

        half2 randomGrad(half2 p)
        {
            half angle = random(p) * 6.28318530718;
            return half2(cos(angle), sin(angle));
        }

        half perlinNoise(half2 p)
        {
            half2 i = floor(p);
            half2 f = fract(p);
            half2 u = f * f * (3.0 - 2.0 * f);

            half dot00 = dot(randomGrad(i), f);
            half dot10 = dot(randomGrad(i + half2(1.0, 0.0)), f - half2(1.0, 0.0));
            half dot01 = dot(randomGrad(i + half2(0.0, 1.0)), f - half2(0.0, 1.0));
            half dot11 = dot(randomGrad(i + half2(1.0, 1.0)), f - half2(1.0, 1.0));

            return mix(mix(dot00, dot10, u.x), mix(dot01, dot11, u.x), u.y) * 0.5 + 0.5;
        }

        half turbulence(half2 p, half freq)
        {
            half value = 0.0;
            half amp = 1.0;
            const int turbulenceOctaves = 2;
            const half turbulenceGain = 0.5;
            const half turbulenceLacunarity = 2.0;

            for (int i = 0; i < turbulenceOctaves; ++i) {
                value += abs(perlinNoise(p * freq) * 2.0 - 1.0) * amp;
                freq *= turbulenceLacunarity;
                amp *= turbulenceGain;
            }
            return value;
        }

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord.xy / iResolution.xy;
            half time = progress;

            const half2 baseDistortion = half2(0.02, 0.03);
            const half2 detailDistortion = half2(0.005, 0.005);
            const half riseFrequency = 15.0;
            const half riseAmplitude = 0.5;
            const half riseOffset = 0.5;
            const half fixedRiseSpeed = 1.0;
            const half fixedNoiseSpeed = 0.4;

            half2 scaledUV = uv * noiseScale;
            half timeX = time * fixedNoiseSpeed;

            half turb = turbulence(scaledUV * 2.0 + half2(timeX, 0.0), 1.0);

            half risePhase = uv.y * riseFrequency + time * fixedRiseSpeed;
            half rise = sin(risePhase) * riseAmplitude + riseOffset;

            half baseNoise = perlinNoise(scaledUV * 3.0 + half2(timeX * 0.6, 0.0));
            half verticalFactor = turb * (1.0 - riseWeight) + rise * riseWeight;
            half2 distortion = half2(
                (baseNoise - 0.5) * baseDistortion.x,
                verticalFactor * baseDistortion.y
            );

            half detailNoise = perlinNoise(scaledUV * 9.0 + half2(timeX * 1.8, timeX * 1.2));
            half2 detailDistort = half2(
                detailNoise * detailDistortion.x,
                detailNoise * detailDistortion.y
            );
            distortion += detailDistort;

            half2 distortedUV = clamp(uv + distortion * intensity, 0.001, 0.999);
            half4 finalColor = image.eval(distortedUV * iResolution.xy);
            return half4(finalColor);
        }
    )";
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_HEAT_DISTORTION_FILTER_H
