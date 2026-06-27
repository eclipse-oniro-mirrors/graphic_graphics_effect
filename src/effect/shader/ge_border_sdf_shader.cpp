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
#include "ge_border_sdf_shader.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {
GEBorderSDFShader::GEBorderSDFShader() {}

GEBorderSDFShader::GEBorderSDFShader(const Drawing::GEBorderSDFShaderParams& params) : params_(params) {}

std::shared_ptr<Drawing::RuntimeEffect> GEBorderSDFShader::GetEffect()
{
    if (params_.style == 0) {
        thread_local std::shared_ptr<Drawing::RuntimeEffect> borderSdfSolidEffect = nullptr;
        if (!borderSdfSolidEffect) {
            borderSdfSolidEffect = GECreateRuntimeEffectForShader(borderShaderCode_);
        }
        return borderSdfSolidEffect;
    }
    return nullptr;
}

void GEBorderSDFShader::MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress)
{
    // 1. Validate param
    if (!params_.shape) {
        GE_LOGE("GEBorderSDFShader::MakeSDFBorderShader shape is invalid.");
        return;
    }

    auto sdfShader = params_.shape->GenerateDrawingShader(canvas,
        canvasInfo_.geoWidth != 0 ? canvasInfo_.geoWidth : rect.GetWidth(),
        canvasInfo_.geoHeight != 0 ? canvasInfo_.geoHeight : rect.GetHeight());
    if (sdfShader == nullptr) {
        GE_LOGD("GEBorderSDFShader::MakeSDFBorderShader no valid params.");
        return;
    }

    // 2. Get shader effect
    auto effect = GetEffect();
    if (!effect) {
        GE_LOGE("GEBorderSDFShader::MakeDrawingShader effect is nullptr.");
        return;
    }

    // 3. Create RuntimeShaderBuilder
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);

    // 4. Set uniforms
    builder->SetChild("sdfShape", sdfShader);
    builder->SetUniformVec4("u_color", params_.color.x_, params_.color.y_, params_.color.z_, params_.color.w_);
    builder->SetUniform("u_width", params_.width);
    builder->SetUniform("u_isOutline", params_.isOutline ? 1.f : -1.f);
    builder->SetUniform("u_dashWidth", params_.dashWidth);
    builder->SetUniform("u_dashGap", params_.dashGap);

    // 5. Create shader and store in base class member
    drShader_ = builder->MakeShader(nullptr, false);
    if (!drShader_) {
        GE_LOGE("GEBorderSDFShader::MakeDrawingShader shader is nullptr.");
    }
}

void GEBorderSDFShader::OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Drawing::Rect newRect(rect);
    if (ROSEN_LE(params_.width, 0.0f)) {
        return;
    }

    if (params_.isOutline) {
        float outset = params_.width + 0.5f;
        newRect.MakeOutset(outset, outset);
    }
    
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
