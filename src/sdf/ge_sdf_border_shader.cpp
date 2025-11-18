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
#include "sdf/ge_sdf_border_shader.h"


namespace OHOS {
namespace Rosen {
static constexpr float SDF_BORDER_MIN_THRESHOLD = 0.0001f;

GESDFBorderShader::GESDFBorderShader(const Drawing::GESDFBorderShaderParams& params)
    :params_(params), sdfTreeProcessor_(std::make_optional<Drawing::GESDFTreeProcessor>(params.shape))
{}

void GESDFBorderShader::MakeDrawingShader(const Drawing::Rect &rect, float progress)
{
    drShader_ = MakeSDFBorderShader(rect);
}

std::shared_ptr<Drawing::ShaderEffect> GESDFBorderShader::MakeSDFBorderShader(const Drawing::Rect &rect)
{
    if (!rect.IsValid()) {
        GE_LOGE("GESDFBorderShader::MakeSDFBorderShader rect is invalid.");
        return nullptr;
    }
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }

    if (!params_.shape || !sdfTreeProcessor_) {
        GE_LOGE("GESDFBorderShader::MakeSDFBorderShader shape is invalid.");
        return nullptr;
    }

    if (!shaderEffectBuilder_) {
        auto sdfBorderEffect = GetSDFBorderEffect();
        shaderEffectBuilder_ = std::make_optional<Drawing::RuntimeShaderBuilder>(sdfBorderEffect);
    }

    sdfTreeProcessor_->UpdateUniformDatas(*shaderEffectBuilder_);

    shaderEffectBuilder_->SetUniform("u_borderColor", params_.border.color.GetRedF(), params_.border.color.GetGreenF(),
        params_.border.color.GetBlueF());
    shaderEffectBuilder_->SetUniform("u_borderWidth", std::max(params_.border.width, SDF_BORDER_MIN_THRESHOLD));
    auto outShader = shaderEffectBuilder_->MakeShader(nullptr, false);
    if (outShader == nullptr) {
        GE_LOGE("GESDFBorderShader::MakeSDFBorderShader sdfBorderShader is nullptr");
        return nullptr;
    }
    return outShader;
}

std::shared_ptr<Drawing::RuntimeEffect> GESDFBorderShader::GetSDFBorderEffect()
{
    if (shaderCode_.empty()) {
        shaderCode_ += borderHeaders_;
        std::string headers;
        std::string sdfMaskFunctions;
        sdfTreeProcessor_->ProcessSDFShape(headers, sdfMaskFunctions);
        shaderCode_ += headers;
        shaderCode_ += sdfMaskFunctions;
        shaderCode_ += borderEffectsFunctions_;
        shaderCode_ += mainFunctionCode_;
    }
    return Drawing::RuntimeEffect::CreateForShader(shaderCode_);
}
}
}