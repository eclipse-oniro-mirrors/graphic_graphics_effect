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
#ifndef GRAPHICS_EFFECT_GE_BORDER_SDF_LG_COLOR_SHADER_H
#define GRAPHICS_EFFECT_GE_BORDER_SDF_LG_COLOR_SHADER_H

#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEBorderSDFLGColorShader : public GEShader {
public:
    GEBorderSDFLGColorShader();
    GEBorderSDFLGColorShader(const Drawing::GEBorderSDFLGColorShaderParams& params);
    GEBorderSDFLGColorShader(const GEBorderSDFLGColorShader&) = delete;
    GEBorderSDFLGColorShader& operator=(const GEBorderSDFLGColorShader&) = delete;
    GEBorderSDFLGColorShader(GEBorderSDFLGColorShader&&) = delete;
    GEBorderSDFLGColorShader& operator=(GEBorderSDFLGColorShader&&) = delete;
    ~GEBorderSDFLGColorShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEBorderSDFLGColorShader, Drawing::GEBorderSDFLGColorShaderParams);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override {}

    void MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress) override;
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;
    void OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    Drawing::GEBorderSDFLGColorShaderParams params_;
    inline static const std::string lgColorShaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec2 iResolution;
        uniform float u_width;
        uniform float u_isOutline;
        uniform float u_angle;
        uniform vec4 u_color0;
        uniform vec4 u_color1;
        uniform vec4 u_color2;
        uniform vec4 u_color3;
        uniform vec4 u_color4;
        uniform float u_pos0;
        uniform float u_pos1;
        uniform float u_pos2;
        uniform float u_pos3;
        uniform float u_pos4;
        uniform float u_pos_max;
        const float aa = 0.5;
        const float eps = 1e-6;

        float getGradT(vec2 p, vec2 res, float deg)
        {
            float ang = radians(deg);
            // Unit vector for gradient direction
            vec2 dir = vec2(cos(ang), sin(ang));

            // Four corners
            vec2 halfRes = res * 0.5;
            vec2 c1 = vec2(-halfRes.x, -halfRes.y);
            vec2 c2 = vec2( halfRes.x, -halfRes.y);
            vec2 c3 = vec2(-halfRes.x,  halfRes.y);
            vec2 c4 = vec2( halfRes.x,  halfRes.y);

            // Projection of four corners towards the gradient direction
            float p1 = dot(c1, dir);
            float p2 = dot(c2, dir);
            float p3 = dot(c3, dir);
            float p4 = dot(c4, dir);

            float minP = min( min(p1,p2), min(p3,p4) );
            float maxP = max( max(p1,p2), max(p3,p4) );
            float curr = dot(p, dir);
            float range = maxP - minP;
            return (range > eps) ? (curr - minP) / range : 0.0;
        }

        vec4 gradientColor(float t, vec4 col0, vec4 col1, vec4 col2, vec4 col3, vec4 col4,
            float pos0, float pos1, float pos2, float pos3, float pos4)
        {
            vec4 res;
            res = step(pos0, t) * (1.0 - step(pos1, t)) *
                    mix(col0, col1, clamp((t - pos0) / max(pos1 - pos0, eps), 0.0, 1.0)) +
                step(pos1, t) * (1.0 - step(pos2, t)) *
                    mix(col1, col2, clamp((t - pos1) / max(pos2 - pos1, eps), 0.0, 1.0)) +
                step(pos2, t) * (1.0 - step(pos3, t)) *
                    mix(col2, col3, clamp((t - pos2) / max(pos3 - pos2, eps), 0.0, 1.0)) +
                step(pos3, t) * (1.0 - step(pos4, t)) *
                    mix(col3, col4, clamp((t - pos3) / max(pos4 - pos3, eps), 0.0, 1.0));
            return res;
        }

        half4 main(vec2 fragCoord) {
            float d = sdfShape.eval(fragCoord).a * u_isOutline;

            // calc distance between fragcoord and center of the component
            vec2 p = fragCoord - iResolution.xy * 0.5;

            float t = getGradT(p, iResolution.xy, u_angle);

            // ensure posMin(u_pos0) <= t < u_posmax
            t = clamp(t, u_pos0, u_pos_max - eps);
            vec4 gradCol = gradientColor(t, u_color0, u_color1, u_color2, u_color3, u_color4,
                u_pos0, u_pos1, u_pos2, u_pos3, u_pos4);
            return gradCol * smoothstep(-aa, 0.0, d) * (1.0 - smoothstep(u_width, u_width+aa, d));
        }
    )";

    std::shared_ptr<Drawing::RuntimeEffect> GetEffect();
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_BORDER_SDF_LG_COLOR_SHADER_H
