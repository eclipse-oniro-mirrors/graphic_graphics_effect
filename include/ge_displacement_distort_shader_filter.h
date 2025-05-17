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
#ifndef GRAPHICS_EFFECT_GE_DISPLACEMENT_DISTORT_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_DISPLACEMENT_DISTORT_SHADER_FILTER_H
 
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
 
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
 
namespace OHOS {
namespace Rosen {
 
class GEDisplacementDistortFilter : public GEShaderFilter {
public:
    GEDisplacementDistortFilter(const Drawing::GEDisplacementDistortFilterParams& params);
    GEDisplacementDistortFilter(const GEDisplacementDistortFilter&) = delete;
    GEDisplacementDistortFilter operator=(const GEDisplacementDistortFilter&) = delete;
    ~GEDisplacementDistortFilter() override = default;
 
    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;
 
    const std::string GetDescription() const;

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetDisplacementDistortEffect();
    Drawing::GEDisplacementDistortFilterParams params_;

    inline static const std::string shaderStringDisplacementDistort = R"(
        uniform shader image;
        uniform shader maskEffect;
        uniform half2 iResolution;
        uniform half2 factor;

        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord.xy/iResolution.xy;

            float maskAlpha = maskEffect.eval(fragCoord).w;
            vec4 finalColor = vec4(0.0);
            if (maskAlpha > 0.0) {
                //Distortion Effects
                vec2 directionVector = 2.0 * (maskEffect.eval(fragCoord).xy - 0.5);
                vec2 normal = directionVector * maskAlpha * factor;
                vec2 refractedUVs = clamp(mix(uv, uv - normal, 0.05), 0.001, 0.999);
                finalColor = image.eval(iResolution.xy * refractedUVs);
            } else {
                finalColor = image.eval(fragCoord);
            }
            return finalColor;
        }
    )";
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_DISPLACEMENT_DISTORT_SHADER_FILTER_H