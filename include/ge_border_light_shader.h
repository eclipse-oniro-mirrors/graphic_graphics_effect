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
#ifndef GRAPHICS_EFFECT_BORDER_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_BORDER_LIGHT_SHADER_H

#include "ge_shader.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEBorderLightShader : public GEShader {
public:
    GEBorderLightShader();
    GEBorderLightShader(Drawing::GEBorderLightShaderParams& borderLightParams);

    ~GEBorderLightShader() override = default;

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    const std::string GetDescription() const { return "GEBorderLightShader"; }
    
    void SetBorderLightParams(const Drawing::GEBorderLightShaderParams& params)
    {
        borderLightParams_ = params;
    }

    std::shared_ptr<Drawing::ShaderEffect> MakeBorderLightShader(const Drawing::Rect& rect);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetBorderLightBuilder();
private:
    GEBorderLightShader(const GEBorderLightShader&) = delete;
    GEBorderLightShader(const GEBorderLightShader&&) = delete;
    GEBorderLightShader& operator=(const GEBorderLightShader&) = delete;
    GEBorderLightShader& operator=(const GEBorderLightShader&&) = delete;

    Drawing::GEBorderLightShaderParams borderLightParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H