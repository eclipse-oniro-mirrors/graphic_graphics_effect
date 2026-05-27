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
#include "ge_border_sdf_lg_color_shader.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
GEBorderSDFLGColorShader::GEBorderSDFLGColorShader() {}

GEBorderSDFLGColorShader::GEBorderSDFLGColorShader(const Drawing::GEBorderSDFLGColorShaderParams& params)
    : params_(params)
{}

std::shared_ptr<Drawing::RuntimeEffect> GEBorderSDFLGColorShader::GetEffect()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> lgColorEffect = nullptr;
    if (!lgColorEffect) {
        lgColorEffect = Drawing::RuntimeEffect::CreateForShader(lgColorShaderCode_);
    }
    return lgColorEffect;
}
void GEBorderSDFLGColorShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    const Vector4f zeroColor(0.0f, 0.0f, 0.0f, 0.0f);
    if (params_.colorNumber <= 2) {
        params_.color2 = zeroColor;
        params_.position2 = 0.0f;
    }
    if (params_.colorNumber <= 3) {
        params_.color3 = zeroColor;
        params_.position3 = 0.0f;
    }
    if (params_.colorNumber <= 4) {
        params_.color4 = zeroColor;
        params_.position4 = 0.0f;
    }
}

void GEBorderSDFLGColorShader::MakeDrawingShader(Drawing::Canvas& canvas, Drawing::Rect& rect, float progress)
{
    // 1. Validate parameters
    float positions[5] = {
        params_.position0, params_.position1, params_.position2, params_.position3, params_.position4
    };
    for (int i = 1; i < params_.colorNumber; i++) {
        if (ROSEN_LE(positions[i], positions[i - 1])) {
            GE_LOGE("GEBorderSDFLGColorShader::MakeDrawingShader positions not increasing at index=%{public}d.", i);
            return;
        }
    }
    float posMax = positions[params_.colorNumber - 1];
    auto sdfShader = params_.shape->GenerateDrawingShader(rect.GetWidth(), rect.GetHeight());
    if (sdfShader == nullptr) {
        GE_LOGE("GEBorderSDFLGColorShader::MakeSDFBorderShader no valid sdfShader.");
        return;
    }

    // 2. Get shader effect
    auto effect = GetEffect();
    if (!effect) {
        GE_LOGE("GEBorderSDFLGColorShader::MakeDrawingShader effect is nullptr.");
        return;
    }

    // 3. Create RuntimeShaderBuilder
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);

    // 4. Set uniforms
    builder->SetChild("sdfShape", sdfShader);
    builder->SetUniform("iResolution", rect.GetWidth(), rect.GetHeight());
    builder->SetUniform("u_width", params_.width);
    builder->SetUniform("u_isOutline", params_.isOutline ? 1.f : -1.f);
    builder->SetUniform("u_angle", params_.angle);
    builder->SetUniformVec4("u_color0", params_.color0.x_, params_.color0.y_, params_.color0.z_, params_.color0.w_);
    builder->SetUniformVec4("u_color1", params_.color1.x_, params_.color1.y_, params_.color1.z_, params_.color1.w_);
    builder->SetUniformVec4("u_color2", params_.color2.x_, params_.color2.y_, params_.color2.z_, params_.color2.w_);
    builder->SetUniformVec4("u_color3", params_.color3.x_, params_.color3.y_, params_.color3.z_, params_.color3.w_);
    builder->SetUniformVec4("u_color4", params_.color4.x_, params_.color4.y_, params_.color4.z_, params_.color4.w_);
    builder->SetUniform("u_pos0", params_.position0);
    builder->SetUniform("u_pos1", params_.position1);
    builder->SetUniform("u_pos2", params_.position2);
    builder->SetUniform("u_pos3", params_.position3);
    builder->SetUniform("u_pos4", params_.position4);
    builder->SetUniform("u_pos_max", posMax);

    // 5. Create shader and store in base class member
    drShader_ = builder->MakeShader(nullptr, false);
    if (!drShader_) {
        GE_LOGE("GEBorderSDFLGColorShader::MakeDrawingShader shader is nullptr.");
    }
}

void GEBorderSDFLGColorShader::OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Drawing::Rect newRect(rect);
    if (ROSEN_LE(params_.width, 0.0f)) {
        return;
    }

    if (params_.isOutline) {
        float outset = params_.width + 0.5f;
        newRect.MakeOutset(outset, outset);
    }

    // Using component rect to calc SDF and border color
    MakeDrawingShader(canvas, rect, -1.f);
    auto shader = GetDrawingShader();
    if (!shader) {
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(newRect);
    canvas.DetachBrush();
}

} // namespace Rosen
} // namespace OHOS
