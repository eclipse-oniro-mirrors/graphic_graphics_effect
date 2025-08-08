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

REGISTER_GEFILTER_TYPEINFO(DISPLACEMENT_DISTORT_FILTER, GEDisplacementDistortFilter,
                           Drawing::GEDisplacementDistortFilterParams);
class GEDisplacementDistortFilter : public GEShaderFilter {
public:
    GEDisplacementDistortFilter(const Drawing::GEDisplacementDistortFilterParams& params);
    GEDisplacementDistortFilter(const GEDisplacementDistortFilter&) = delete;
    GEDisplacementDistortFilter operator=(const GEDisplacementDistortFilter&) = delete;
    ~GEDisplacementDistortFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEDisplacementDistortFilter);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;
 
    const std::string GetDescription() const;

    const std::string& Type() const override;

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetDisplacementDistortEffect();
    Drawing::GEDisplacementDistortFilterParams params_;
    static const std::string type_;

    inline static const std::string shaderStringDisplacementDistort = R"(
        uniform shader image;
        uniform shader maskEffect;
        uniform half2 iResolution;
        uniform half2 factor;

        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord.xy/iResolution.xy;

            vec4 maskColor = maskEffect.eval(fragCoord);
            vec4 finalColor = vec4(0.0);
            if (maskColor.a > 0.0) {
                //Distortion Effects
                vec2 directionVector = 2.0 * (maskColor.rg - 0.5);
                vec2 normal = directionVector * factor;
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