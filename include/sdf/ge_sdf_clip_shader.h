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

#ifndef GRAPHICS_EFFECT_GE_SDF_CLIP_H
#define GRAPHICS_EFFECT_GE_SDF_CLIP_H

#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_sdf_tree_processor.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"


namespace OHOS {
namespace Rosen {
 
class GESDFClipShader : public GEShader {
public:
    GESDFClipShader(const Drawing::GESDFClipShaderParams& params);
    GESDFClipShader(const GESDFClipShader&) = delete;
    GESDFClipShader operator=(const GESDFClipShader&) = delete;
    ~GESDFClipShader() override = default;

    const std::string GetDescription() const { return "GESDFClipShader"; }

    void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    void SetSDFClipParams(const Drawing::GESDFClipShaderParams& params)
    {
        params_ = params;
    }

    std::shared_ptr<Drawing::ShaderEffect> MakeSDFClipShader(const Drawing::Rect& rect);

private:
    std::shared_ptr<Drawing::RuntimeEffect> GetSDFClipEffect();
    Drawing::GESDFClipShaderParams params_;
    std::string shaderCode_;
    std::optional<Drawing::GESDFTreeProcessor> sdfTreeProcessor_;
    std::optional<Drawing::RuntimeShaderBuilder> shaderEffectBuilder_;

    inline static const std::string mainFunctionCode_ = R"(
        half4 main(float2 fragCoord)
        {
            vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
            vec2 coord = fragCoord;
            float d = SDFMap(coord);
            color.a = 1.0 - smoothstep(-1.0, 0.0, d);
            return half4(color);
        }
    )";
};
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SDF_CLIP_SHADER_H