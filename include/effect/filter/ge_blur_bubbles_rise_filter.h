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
#ifndef GRAPHICS_EFFECT_GE_BLUR_BUBBLES_RISE_FILTER_H
#define GRAPHICS_EFFECT_GE_BLUR_BUBBLES_RISE_FILTER_H

#include <cstdint>
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

class GEBlurBubblesRiseFilter : public GEShaderFilter {
    friend class GEBlurBubblesRiseFilterTest;

public:
    explicit GEBlurBubblesRiseFilter(const Drawing::GEBlurBubblesRiseFilterParams& params);
    ~GEBlurBubblesRiseFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEBlurBubblesRiseFilter, Drawing::GEBlurBubblesRiseFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    void CheckBlurBubblesRiseParams();

    std::shared_ptr<Drawing::RuntimeEffect> GetBlurShaderEffect();
    std::shared_ptr<Drawing::RuntimeEffect> GetMaskMixShaderEffect();

    float blurIntensity_ = 0.3f;
    float mixStrength_ = 1.0f;
    float progress_ = 0.0f;
    std::shared_ptr<Drawing::Image> maskImage_ = nullptr;

    const float timeScale_ = 25.0f;

    inline static const std::string shaderStringGaussianBlur = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half blurIntensity;
        uniform half horizontal;

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord.xy / iResolution;
            half2 texelSize = half2(1.0 / iResolution.x, 1.0 / iResolution.y);
            half2 direction = (horizontal > 0.5) ? half2(1.0, 0.0) : half2(0.0, 1.0);

            half sigma = max(0.35, blurIntensity * 0.5);
            half negInvSigmaSq2 = -1.0 / (sigma * sigma * 2.0);

            half4 centerColor = image.eval(uv * iResolution);
            half3 color = centerColor.rgb;
            half totalWeight = 1.0;

            const int MAX_SAMPLES = 10;
            for (int i = 1; i <= MAX_SAMPLES; ++i) {
                half fi = half(i);
                half sampleMask = step(fi, blurIntensity + 0.5);

                half weight = exp(fi * fi * negInvSigmaSq2) * sampleMask;

                if (weight < 0.001) continue;

                half2 offset = direction * texelSize * fi;
                color += image.eval((uv + offset) * iResolution).rgb * weight;
                color += image.eval((uv - offset) * iResolution).rgb * weight;
                totalWeight += 2.0 * weight;
            }

            return half4(color / max(totalWeight, 0.0001), centerColor.a);
        }
    )";

    inline static const std::string shaderStringMaskMix = R"(
        uniform shader blur_tex;
        uniform shader original_tex;
        uniform shader blur_mask;
        uniform half2 iResolution;
        uniform half2 maskResolution;
        uniform half mixStrength;
        uniform half progress;

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord.xy / iResolution;
            half2 maskUV = uv + half2(0.0, progress * 0.07);

            half4 maskColor = blur_mask.eval(maskUV * maskResolution);
            half maskValue = maskColor.r;

            half blend = clamp(maskValue * mixStrength, 0.0, 1.0);
            half4 originColor = original_tex.eval(fragCoord);
            half4 blurredColor = blur_tex.eval(fragCoord);
            half3 mixedRgb = mix(originColor.rgb, blurredColor.rgb, blend);
            return half4(mixedRgb, originColor.a);
        }
    )";
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_BLUR_BUBBLES_RISE_FILTER_H
