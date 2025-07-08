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
#include "ge_linear_gradient_shader_mask.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GELinearGradientShaderMask::GELinearGradientShaderMask(const GELinearGradientShaderMaskParams& params)
    : fractionStops_(params.fractionStops), startPos_(params.startPosition), endPos_(params.endPosition)
{
}

std::shared_ptr<ShaderEffect> GELinearGradientShaderMask::GenerateDrawingShader(float width, float height) const
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;

    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    LOGD("GELinearGradientShaderMask::GenerateDrawingShader %{public}f, %{public}f, %{public}f, %{public}f, "
         "%{public}f, %{public}f, %{public}d",
         width,
         height,
         static_cast<float>(startPos_.GetX()),
         static_cast<float>(startPos_.GetY()),
         static_cast<float>(endPos_.GetX()),
         static_cast<float>(endPos_.GetY()),
         static_cast<int>(fractionStops_.size()));

    if (fractionStops_[0].second > 0.01) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMin, ColorMin, ColorMin));
        p.emplace_back(fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }

    for (size_t i = 0; i < fractionStops_.size(); i++) {
        uint8_t alpha = static_cast<uint8_t>(fractionStops_[i].first * ColorMax)
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(alpha, alpha, alpha, alpha));
        p.emplace_back(fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (fractionStops_[fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(fractionStops_[fractionStops_.size() - 1].second + 0.01);
    }
    return Drawing::ShaderEffect::CreateLinearGradient(startPos_, endPos_, c, p, Drawing::TileMode::CLAMP);
}

std::shared_ptr<ShaderEffect> GELinearGradientShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;

    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    LOGD("GELinearGradientShaderMask::GenerateDrawingShader %{public}f, %{public}f, %{public}f, %{public}f, "
         "%{public}f, %{public}f, %{public}d",
         width,
         height,
         static_cast<float>(startPos_.GetX()),
         static_cast<float>(startPos_.GetY()),
         static_cast<float>(endPos_.GetX()),
         static_cast<float>(endPos_.GetY()),
         static_cast<int>(fractionStops_.size()));

    if (fractionStops_[0].second > 0.01) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMin, ColorMin, ColorMin));
        p.emplace_back(fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }

    for (size_t i = 0; i < fractionStops_.size(); i++) {
        uint8_t alpha = static_cast<uint8_t>(fractionStops_[i].first * ColorMax)
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(alpha, alpha, alpha, alpha));
        p.emplace_back(fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (fractionStops_[fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(fractionStops_[fractionStops_.size() - 1].second + 0.01);
    }
    return Drawing::ShaderEffect::CreateLinearGradient(startPos_, endPos_, c, p, Drawing::TileMode::CLAMP);
}
}
}
}