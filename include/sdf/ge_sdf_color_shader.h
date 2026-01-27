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

#ifndef GRAPHICS_EFFECT_GE_SDF_COLOR_H
#define GRAPHICS_EFFECT_GE_SDF_COLOR_H

#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
 
class GESDFColorShader : public GEShader {
public:
    GESDFColorShader(const Drawing::GESDFColorShaderParams& params);
    GESDFColorShader(const GESDFColorShader&) = delete;
    GESDFColorShader operator=(const GESDFColorShader&) = delete;
    ~GESDFColorShader() override = default;

    const std::string GetDescription() const { return "GESDFColorShader"; }

    void OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    void SetSDFColorParams(const Drawing::GESDFColorShaderParams& params)
    {
        params_ = params;
    }

    std::shared_ptr<Drawing::ShaderEffect> MakeSDFColorShader(const Drawing::Rect& rect);

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetSDFColorEffect();
    Drawing::GESDFColorShaderParams params_;

    inline static const std::string shaderCode_ = R"(
        uniform shader sdfShape;
        uniform vec4 color;

        half4 main(float2 fragCoord)
        {
            float d = sdfShape.eval(fragCoord).a;
            float alpha = color.a * (1.0 - smoothstep(-1.0, 0.0, d));
            return half4(color.rgb * alpha, alpha);
        }
    )";
};
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SDF_COLOR_SHADER_H