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

#ifndef GRAPHICS_EFFECT_GE_SDF_SHADOW_SHADER_H
#define GRAPHICS_EFFECT_GE_SDF_SHADOW_SHADER_H

#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"

 
namespace OHOS {
namespace Rosen {
 
class GESDFShadowShader : public GEShader {
public:
    GESDFShadowShader(const Drawing::GESDFShadowShaderParams& params);
    GESDFShadowShader(const GESDFShadowShader&) = delete;
    GESDFShadowShader operator=(const GESDFShadowShader&) = delete;
    ~GESDFShadowShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GESDFShadowShader, Drawing::GESDFShadowShaderParams);

    const std::string GetDescription() const { return "GESDFShadowShader"; }

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    void SetSDFShadowParams(const Drawing::GESDFShadowShaderParams& params)
    {
        params_ = params;
    }

    std::shared_ptr<Drawing::ShaderEffect> MakeSDFShadowShader(const Drawing::Rect& rect);
    void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetSDFShadowEffect();
    Drawing::GESDFShadowShaderParams params_;

    inline static const std::string shaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec2 iResolution;
        uniform vec3 shadowColor;
        uniform vec2 shadowOffset;
        uniform float shadowRadius;
        uniform float isFilled;

        // Input data:
        // vec2 coord - coordinates used to calculate SDFMap
        // vec2 shadowOffset - offset of the shadow
        // float d - current SDF shape distance
        // vec3 shadowColor - color of the shadow
        // float shadowRadius - radius of the shadow
        // bool isFilled - should SDFBody be filled with shadow
        vec4 shadowEffect(vec2 coord, vec2 shadowOffset,
            float d, vec3 shadowColor, float shadowRadius, bool isFilled)
        {
            vec4 color = vec4(0.0);
            float alphaFilled = 1.0;
            if (!isFilled && d < 0.0)
            {
                alphaFilled = smoothstep(-1.0, 0.0, d);
            }

            // Recalculate the distance if there is offset
            if (any(notEqual(shadowOffset, vec2(0.0))))
            {
                d = sdfShape.eval(coord - shadowOffset).a;
            }

            if (d <= shadowRadius)
            {
                d += shadowRadius * 0.5;
                float alpha = clamp(d / shadowRadius, 0.0, 1.0);
                alpha = 1.0 - alpha;
                color = vec4(shadowColor, 1.0) * alpha * alphaFilled;
            }

            return color;
        }

        half4 main(float2 fragCoord)
        {
            vec2 coord = fragCoord;
            float d = sdfShape.eval(coord).a;
            vec4 color = shadowEffect(coord, shadowOffset, d, shadowColor, shadowRadius, isFilled > 0.5);
            return half4(color);
        }
    )";
};
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SDF_SHADOW_SHADER_H