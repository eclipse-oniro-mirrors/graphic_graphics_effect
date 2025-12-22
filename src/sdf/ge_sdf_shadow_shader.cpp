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
#include "sdf/ge_sdf_shadow_shader.h"
#include "common/rs_common_def.h"
 
namespace OHOS {
namespace Rosen {
static constexpr float SDF_SHADOW_MIN_THRESHOLD = 0.0001f;
static constexpr float RADIUS_FACTOR = 1.5f;

GESDFShadowShader::GESDFShadowShader(const Drawing::GESDFShadowShaderParams& params) : params_(params)
{}

void GESDFShadowShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeSDFShadowShader(rect);
}

void GESDFShadowShader::UpdateRectForShadow(Drawing::Rect& rect)
{
    float radius = std::max(params_.shadow.radius * RADIUS_FACTOR, SDF_SHADOW_MIN_THRESHOLD);
    float left = params_.shadow.offsetX - radius;
    float right = params_.shadow.offsetX + radius;
    float top = params_.shadow.offsetY - radius;
    float bottom =  params_.shadow.offsetY + radius;
    if (ROSEN_LNE(left, 0.0f)) {
        rect.SetLeft(rect.GetLeft() + left);
    }
    if (ROSEN_GNE(right, 0.0f)) {
        rect.SetRight(rect.GetRight() + right);
    }
    if (ROSEN_LNE(top, 0.0f)) {
        rect.SetTop(rect.GetTop() + top);
    }
    if (ROSEN_GNE(bottom, 0.0f)) {
        rect.SetBottom(rect.GetBottom() + bottom);
    }
}

void GESDFShadowShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Drawing::Rect newRect = rect;
    UpdateRectForShadow(newRect);
    MakeDrawingShader(newRect, -1.f); // not use progress
    auto shader = GetDrawingShader();
    if (shader == nullptr) {
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(newRect);
    canvas.DetachBrush();
}

std::shared_ptr<Drawing::ShaderEffect> GESDFShadowShader::MakeSDFShadowShader(const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }

    if (!params_.shape) {
        GE_LOGE("GESDFShadowShader::MakeSDFShadowShader shape is invalid.");
        return nullptr;
    }

    auto sdfShader = params_.shape->GenerateDrawingShader(width, height);
    if (sdfShader == nullptr) {
        GE_LOGE("GESDFShadowShader: failed generate GESDFShadowShader.");
        return nullptr;
    }

    auto sdfEffect = GetSDFShadowEffect();
    if (sdfEffect == nullptr) {
        GE_LOGE("GESDFShadowShader: failed GetSDFShadowEffect.");
        return nullptr;
    }
    auto shaderEffectBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfEffect);
    shaderEffectBuilder->SetChild("sdfShape", sdfShader);
    shaderEffectBuilder->SetUniform("iResolution", static_cast<float>(width), static_cast<float>(height));
    shaderEffectBuilder->SetUniform("shadowColor", params_.shadow.color.GetRedF(),
        params_.shadow.color.GetGreenF(), params_.shadow.color.GetBlueF());
    shaderEffectBuilder->SetUniform("shadowOffset", params_.shadow.offsetX, params_.shadow.offsetY);
    shaderEffectBuilder->SetUniform("shadowRadius",
        std::max(params_.shadow.radius * RADIUS_FACTOR, SDF_SHADOW_MIN_THRESHOLD));
    shaderEffectBuilder->SetUniform("isFilled", static_cast<float>(params_.shadow.isFilled));

    auto outShader = shaderEffectBuilder->MakeShader(nullptr, false);
    if (outShader == nullptr) {
        GE_LOGE("GESDFShadowShader::MakeSDFShadowShader sdfShadowShader is nullptr.");
        return nullptr;
    }
    return outShader;
}

std::shared_ptr<Drawing::RuntimeEffect> GESDFShadowShader::GetSDFShadowEffect()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> sdfShadowShader = nullptr;
    if (sdfShadowShader == nullptr) {
        sdfShadowShader = Drawing::RuntimeEffect::CreateForShader(shaderCode_);
    }
    return sdfShadowShader;
}

} // namespace Rosen
} // namespace OHOS