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

#ifndef GRAPHICS_EFFECT_DISPERSION_SHADER_FILTER_H
#define GRAPHICS_EFFECT_DISPERSION_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEDispersionShaderFilter : public GEShaderFilter {
public:
    GEDispersionShaderFilter(const Drawing::GEDispersionShaderFilterParams& params);
    ~GEDispersionShaderFilter() override = default;

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

protected:
    virtual std::shared_ptr<Drawing::RuntimeEffect> GetDispersionEffect();
    Drawing::GEDispersionShaderFilterParams params_;

private:
    inline static const std::string g_shaderStringDispersion = R"(
        uniform shader image;
        uniform shader mask;
        uniform vec2 iResolution;

        uniform float opacity;
        uniform vec2 redOffset;
        uniform vec2 greenOffset;
        uniform vec2 blueOffset;

        vec4 main(vec2 fragCoord)
        {
            vec2 fragUV = fragCoord.xy / iResolution.xy;
            vec4 maskColor = mask.eval(fragCoord);

            vec2 sdf = (maskColor.rg - 0.5) * 2.0;
            float alpha = maskColor.a * opacity;
            vec3 dispersedColor = vec3(0.0);

            vec2 offset = redOffset * sdf;
            dispersedColor[0] = image.eval((fragUV + offset) * iResolution).r;
            offset = greenOffset * sdf;
            dispersedColor[1] = image.eval((fragUV + offset) * iResolution).g;
            offset = blueOffset * sdf;
            dispersedColor[2] = image.eval((fragUV + offset) * iResolution).b;

            vec4 imageColor = image.eval(fragCoord.xy);
            vec3 finalColor = mix(imageColor.rgb, dispersedColor, alpha);

            // Final Image
            return vec4(finalColor, imageColor.a);
        }
    )";
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_DISPERSION_SHADER_FILTER_H