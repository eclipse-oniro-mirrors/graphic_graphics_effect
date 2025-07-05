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

#ifndef GRAPHICS_EFFECT_WAVY_RIPPLE_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_WAVY_RIPPLE_LIGHT_SHADER_H

#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "common/rs_vector2.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEWavyRippleLightShader : public GEShader {

public:
    GEWavyRippleLightShader();
    GEWavyRippleLightShader(Drawing::GEWavyRippleLightShaderParams& wavyRippleLightParams);
    ~GEWavyRippleLightShader() override = default;
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    const std::string GetDescription() const { return "GEWavyRippleLightShader"; }
    void SetWavyRippleLightParams(const Drawing::GEWavyRippleLightShaderParams& params)
    {
        wavyRippleLightParams_ = params;
    }
    std::shared_ptr<Drawing::ShaderEffect> MakeWavyRippleLightShader(const Drawing::Rect& rect);
    static std::shared_ptr<GEWavyRippleLightShader> CreateWavyRippleLightShader(
        Drawing::GEWavyRippleLightShaderParams& param);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetWavyRippleLightBuilder();

private:
    GEWavyRippleLightShader(const GEWavyRippleLightShader&) = delete;
    GEWavyRippleLightShader(const GEWavyRippleLightShader&&) = delete;
    GEWavyRippleLightShader& operator=(const GEWavyRippleLightShader&) = delete;
    GEWavyRippleLightShader& operator=(const GEWavyRippleLightShader&&) = delete;

    Drawing::GEWavyRippleLightShaderParams wavyRippleLightParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H