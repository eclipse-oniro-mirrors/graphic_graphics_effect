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

#ifndef GRAPHICS_EFFECT_GE_SDF_BORDER_SHADER_H
#define GRAPHICS_EFFECT_GE_SDF_BORDER_SHADER_H

#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {

class GESDFBorderShader : public GEShader {
public:
    GESDFBorderShader(const Drawing::GESDFBorderShaderParams &params);
    GESDFBorderShader(const GESDFBorderShader&) = delete;
    GESDFBorderShader operator =(const GESDFBorderShader&) = delete;
    ~GESDFBorderShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GESDFBorderShader, Drawing::GESDFBorderShaderParams);

    const std::string GetDescription() const { return "GESDFBorderShader"; }
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    void SetSDFBorderParams(const Drawing::GESDFBorderShaderParams& params)
    {
        params_ = params;
    }

    std::shared_ptr<Drawing::ShaderEffect> MakeSDFBorderShader(const Drawing::Rect& rect);
    void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetSDFBorderEffect();
    Drawing::GESDFBorderShaderParams params_;

    inline static const std::string shaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec3 u_borderColor;
        uniform float u_borderWidth;

        // Input data:
        // float d - current SDF shape distance
        // vec4 color - color of the background
        // vec3 borderColor - color of the border
        // float borderWidth - width of the border
        vec4 borderEffect(float d, vec4 color, vec3 borderColor, float borderWidth)
        {
            if (d < 0.0 && d >= -borderWidth) {
                color = vec4(borderColor, 1.0) * (1.0 - smoothstep(-1.0, 0.0, d));
            }

            if (d < -borderWidth && d >= (-borderWidth - 1.0)) {
                color = vec4(borderColor, 1.0) * smoothstep(-borderWidth - 1.0, -borderWidth, d);
            }
            
            return color;
        }

        half4 main(float2 fragCoord)
        {
            vec4 color = vec4(0.0);
            float d = sdfShape.eval(fragCoord).a;
            color = borderEffect(d, color, u_borderColor, u_borderWidth);
            return half4(color);
        }
    )";
};
}  // namespace Rosen
}  // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_SDF_BORDER_SHADER_H