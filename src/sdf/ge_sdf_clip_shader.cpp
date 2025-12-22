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

#include <chrono>
 
#include "ge_log.h"
#include "sdf/ge_sdf_clip_shader.h"
 
namespace OHOS {
namespace Rosen {

GESDFClipShader::GESDFClipShader(const Drawing::GESDFClipShaderParams& params) : params_(params)
{}

void GESDFClipShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Preprocess(canvas, rect); // to calculate your cache data
    MakeDrawingShader(rect, -1.f);
    auto shader = GetDrawingShader();
    Drawing::Brush brush;
    if (shader == nullptr) {
        brush.SetColor(Drawing::Color::COLOR_TRANSPARENT);
    }
    brush.SetBlendMode(Drawing::BlendMode::DST_IN);
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}

void GESDFClipShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    if (!rect.IsValid()) {
        GE_LOGE("GESDFClipShader::MakeSDFClipShader rect is invalid.");
        return;
    }
    drShader_ = MakeSDFClipShader(rect);
}

std::shared_ptr<Drawing::ShaderEffect> GESDFClipShader::MakeSDFClipShader(const Drawing::Rect& rect)
{
    if (!params_.shape) {
        GE_LOGE("GESDFClipShader::MakeSDFClipShader mask is invalid.");
        return nullptr;
    }

    auto sdfShader = params_.shape->GenerateDrawingShader(rect.GetWidth(), rect.GetHeight());
    if (sdfShader == nullptr) {
        GE_LOGE("GESDFClipShader: failed generate GESDFClipShader.");
        return nullptr;
    }

    auto sdfEffect = GetSDFClipEffect();
    if (sdfEffect == nullptr) {
        GE_LOGE("GESDFClipShader: failed GetSDFClipEffect.");
        return nullptr;
    }
    auto shaderEffectBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfEffect);
    shaderEffectBuilder->SetChild("sdfShape", sdfShader);
    auto outShader = shaderEffectBuilder->MakeShader(nullptr, false);
    if (outShader == nullptr) {
        GE_LOGE("GESDFClipShader::MakeSDFClipShader sdfShadowShader is nullptr.");
        return nullptr;
    }
    return outShader;
}

std::shared_ptr<Drawing::RuntimeEffect> GESDFClipShader::GetSDFClipEffect()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> sdfClipShader = nullptr;
    if (sdfClipShader == nullptr) {
        sdfClipShader = Drawing::RuntimeEffect::CreateForShader(shaderCode_);
    }
    return sdfClipShader;
}

} // namespace Rosen
} // namespace OHOS