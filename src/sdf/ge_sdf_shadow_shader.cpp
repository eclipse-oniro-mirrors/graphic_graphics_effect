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
 
namespace OHOS {
namespace Rosen {
static constexpr float SDF_SHADOW_MIN_THRESHOLD = 0.0001f;

GESDFShadowShader::GESDFShadowShader(const Drawing::GESDFShadowShaderParams& params)
    : params_(params), sdfTreeProcessor_(std::make_optional<Drawing::GESDFTreeProcessor>(params.shape))
{}

void GESDFShadowShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeSDFShadowShader(rect);
}

std::shared_ptr<Drawing::ShaderEffect> GESDFShadowShader::MakeSDFShadowShader(const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }

    if (!params_.shape || !sdfTreeProcessor_) {
        GE_LOGE("GESDFShadowShader::MakeSDFShadowShader mask is invalid.");
        return nullptr;
    }

    if (!shaderEffectBuilder_) {
        auto sdfShadowEffect = GetSDFShadowEffect();
        shaderEffectBuilder_ = std::make_optional<Drawing::RuntimeShaderBuilder>(sdfShadowEffect);
    }
    sdfTreeProcessor_->UpdateUniformDatas(*shaderEffectBuilder_);

    shaderEffectBuilder_->SetUniform("iResolution", static_cast<float>(width), static_cast<float>(height));
    shaderEffectBuilder_->SetUniform("shadowColor", params_.shadow.color.GetRedF(),
        params_.shadow.color.GetGreenF(), params_.shadow.color.GetBlueF());
    shaderEffectBuilder_->SetUniform("shadowOffset", params_.shadow.offsetX, params_.shadow.offsetY);
    shaderEffectBuilder_->SetUniform("shadowRadius", std::max(params_.shadow.radius, SDF_SHADOW_MIN_THRESHOLD));
    shaderEffectBuilder_->SetUniform("isFilled", static_cast<float>(params_.shadow.isFilled));

    auto outShader = shaderEffectBuilder_->MakeShader(nullptr, false);
    if (outShader == nullptr) {
        GE_LOGE("GESDFShadowShader::MakeSDFShadowShader sdfShadowShader is nullptr.");
        return nullptr;
    }
    return outShader;
}

std::shared_ptr<Drawing::RuntimeEffect> GESDFShadowShader::GetSDFShadowEffect()
{
    if (shaderCode_.empty()) {
        shaderCode_ += shadowHeaders_;
        std::string headers;
        std::string sdfShapeFunctions;
        sdfTreeProcessor_->ProcessSDFShape(headers, sdfShapeFunctions);
        shaderCode_ += headers;
        shaderCode_ += sdfShapeFunctions;
        shaderCode_ += shadowEffectsFunctions_;
        shaderCode_ += mainFunctionCode_;
    }
    return Drawing::RuntimeEffect::CreateForShader(shaderCode_);
}

} // namespace Rosen
} // namespace OHOS