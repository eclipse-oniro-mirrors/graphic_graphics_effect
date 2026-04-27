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
#ifndef GRAPHICS_EFFECT_GE_BORDER_SDF_SHADER_H
#define GRAPHICS_EFFECT_GE_BORDER_SDF_SHADER_H

#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEBorderSDFShader : public GEShader {
public:
    GEBorderSDFShader();
    GEBorderSDFShader(const Drawing::GEBorderSDFShaderParams& params);
    GEBorderSDFShader(const GEBorderSDFShader&) = delete;
    GEBorderSDFShader operator=(const GEBorderSDFShader&) = delete;
    GEBorderSDFShader(GEBorderSDFShader&&) = delete;
    GEBorderSDFShader& operator=(GEBorderSDFShader&&) = delete;
    ~GEBorderSDFShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEBorderSDFShader, Drawing::GEBorderSDFShaderParams);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

private:
    Drawing::GEBorderSDFShaderParams params_;
    inline static const std::string borderShaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec4 u_color;
        uniform float u_width;
        uniform float u_isOutline;
        uniform float u_dashWidth;
        uniform float u_dashGap;
        const float aa = 0.5;

        half4 main(float2 fragCoord)
        {
            float d = sdfShape.eval(fragCoord).a;
            float p = d * u_isOutline;
            return u_color * smoothstep(-aa, 0.0, p) * (1.0 - smoothstep(u_width, u_width+aa, p));
        }
    )";

    std::shared_ptr<Drawing::RuntimeEffect> GetEffect();
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_BORDER_SDF_SHADER_H
