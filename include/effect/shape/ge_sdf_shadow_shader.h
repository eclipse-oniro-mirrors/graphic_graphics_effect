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
    void OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    bool IsElevationMode() const { return params_.shadow.elevation > 0.0f; }

    // Original SDF shadow methods
    std::shared_ptr<Drawing::RuntimeEffect> GetSDFShadowEffect();
    void UpdateRectForShadow(Drawing::Rect& rect);

    // Elevation shadow methods
    void ComputeElevationParams();
    std::shared_ptr<Drawing::RuntimeEffect> GetElevationShadowEffect();
    void UpdateRectForElevationShadow(Drawing::Rect& rect);
    std::shared_ptr<Drawing::ShaderEffect> MakeElevationShadowShader(const Drawing::Rect& rect);

    Drawing::GESDFShadowShaderParams params_;

    // Elevation computed params
    float ambientBlurRadius_ = 0.0f;
    float spotBlurRadius_ = 0.0f;
    Drawing::Color ambientColor_;
    Drawing::Color spotColor_;

    inline static const std::string shaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec2 iResolution;
        uniform vec4 shadowColor;
        uniform vec2 shadowOffset;
        uniform float shadowRadius;
        uniform float isFilled;

        // Input data:
        // vec2 coord - coordinates used to calculate SDFMap
        // vec2 shadowOffset - offset of the shadow
        // float d - current SDF shape distance
        // vec4 shadowColor - color of the shadow
        // float shadowRadius - radius of the shadow
        // bool isFilled - should SDFBody be filled with shadow
        vec4 shadowEffect(vec2 coord, vec2 shadowOffset,
            float d, vec4 shadowColor, float shadowRadius, bool isFilled)
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
                float alpha = 1.0 - smoothstep(-shadowRadius, shadowRadius, d);
                color = shadowColor * alpha * alphaFilled;
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

    inline static const std::string elevationShaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec2 iResolution;
        uniform vec4 ambientColor;
        uniform float ambientBlurRadius;
        uniform vec4 spotColor;
        uniform float spotBlurRadius;
        uniform float isFilled;
        uniform vec2 shadowOffset;

        // Skia GaussianColorFilter quartic approximation (SkRasterPipeline_opts.h:4957)
        // Input: x = 1.0 at boundary (d=0), 0.0 at blurRadius (d=blurRadius)
        // Approximates: exp(-(1-x)^2 * 4) - 0.018
        float skiaGaussian(float x) {
            const float c4 = -2.26661229133605957031;
            const float c3 =  2.89795351028442382812;
            const float c2 =  0.21345567703247070312;
            const float c1 =  0.15489584207534790039;
            const float c0 =  0.00030726194381713867;
            return c0 + x * (c1 + x * (c2 + x * (c3 + x * c4)));
        }

        // Outer falloff for ambient/spot shadow
        // x = 1.0 at boundary, 0.0 at blurRadius
        float outerGaussian(float d, float blurRadius) {
            if (blurRadius < 0.001 || d >= blurRadius) return 0.0;
            float x = 1.0 - d / blurRadius;
            return max(skiaGaussian(x), 0.0);
        }

        vec4 elevationShadowEffect(float d, float d_original) {
            float alphaFilled = 1.0;
            if (isFilled < 0.5 && d_original < 0.0) {
                alphaFilled = smoothstep(-1.0, 0.0, d_original);
            }

            // Ambient: filled interior + outer Gaussian falloff (matches Skia tessellation model)
            float ambientCoverage = (d <= 0.0) ? 1.0 : outerGaussian(d, ambientBlurRadius);
            vec4 ambient = vec4(ambientColor.rgb, ambientColor.a * ambientCoverage);

            // Spot: same filled interior + outer falloff model
            float spotCoverage = (d <= 0.0) ? 1.0 : outerGaussian(d, spotBlurRadius);
            vec4 spot = vec4(spotColor.rgb, spotColor.a * spotCoverage);

            // SrcOver blend
            vec3 color = ambient.rgb + spot.rgb * (1.0 - ambient.a);
            float alpha = ambient.a + spot.a * (1.0 - ambient.a);

            return vec4(color * alphaFilled, alpha * alphaFilled);
        }

        half4 main(float2 fragCoord) {
            // isFilled clipping: check original shape position
            float d_original = sdfShape.eval(fragCoord).a;

            // Shadow rendering: use offset position (matches shaderCode_ pattern)
            float d = d_original;
            if (any(notEqual(shadowOffset, vec2(0.0)))) {
                d = sdfShape.eval(fragCoord - shadowOffset).a;
            }

            vec4 color = elevationShadowEffect(d, d_original);
            return half4(color);
        }
    )";
};
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SDF_SHADOW_SHADER_H