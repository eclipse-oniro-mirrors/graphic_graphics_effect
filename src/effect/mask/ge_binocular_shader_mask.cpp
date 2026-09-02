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

#include <algorithm>
#include "ge_binocular_shader_mask.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEBinocularShaderMask::GEBinocularShaderMask(const GEBinocularMaskParams& param) : param_(param)
{
    constexpr float minRadius = 0.0f;
    constexpr float maxRadius = 1.0f;
    constexpr float minGap = 0.0f;
    constexpr float maxGap = 1.0f;
    constexpr float minSoftness = 0.0f;
    constexpr float maxSoftness = 1.0f;
    param_.radiusX_ = std::clamp(param_.radiusX_, minRadius, maxRadius);
    param_.radiusY_ = std::clamp(param_.radiusY_, minRadius, maxRadius);
    param_.gap_ = std::clamp(param_.gap_, minGap, maxGap);
    param_.softness_ = std::clamp(param_.softness_, minSoftness, maxSoftness);
}

std::shared_ptr<ShaderEffect> GEBinocularShaderMask::GenerateDrawingShader(float width, float height) const
{
    auto builder = GetBinocularShaderMaskBuilder();
    if (builder == nullptr) {
        LOGE("GEBinocularShaderMask::GenerateDrawingShader get builder error");
        return nullptr;
    }
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("binocularRX", param_.radiusX_);
    builder->SetUniform("binocularRY", param_.radiusY_);
    builder->SetUniform("binocularGAP", param_.gap_);
    builder->SetUniform("binocularSOFT", param_.softness_);

    auto shaderEffect = builder->MakeShader(nullptr, false);
    if (shaderEffect == nullptr) {
        LOGE("GEBinocularShaderMask::GenerateDrawingShader make shader failed");
        return nullptr;
    }
    return shaderEffect;
}

std::shared_ptr<ShaderEffect> GEBinocularShaderMask::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    // BinocularMask is not used with displacementDistort, HasNormal not needed
    return nullptr;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEBinocularShaderMask::GetBinocularShaderMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (builder != nullptr) {
        return builder;
    }

    static constexpr char prog[] = R"(
        uniform half2 iResolution;
        uniform half binocularRX;
        uniform half binocularRY;
        uniform half binocularGAP;
        uniform half binocularSOFT;

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord / iResolution;
            half2 p = uv - 0.5;

            half RX = binocularRX;
            half RY = binocularRY;
            half GAP = binocularGAP;
            half SOFT = binocularSOFT;

            half2 centerL = half2(-GAP, 0.0);
            half2 pl = p - centerL;
            half ellipseL = (pl.x / RX) * (pl.x / RX) + (pl.y / RY) * (pl.y / RY);
            half sideL = 0.0;
            if (pl.x >= 0.0) {
                sideL = 1.0 - smoothstep(1.0 - SOFT, 1.0 + SOFT, ellipseL);
            }

            half2 centerR = half2(+GAP, 0.0);
            half2 pr = p - centerR;
            half ellipseR = (pr.x / RX) * (pr.x / RX) + (pr.y / RY) * (pr.y / RY);
            half sideR = 0.0;
            if (pr.x <= 0.0) {
                sideR = 1.0 - smoothstep(1.0 - SOFT, 1.0 + SOFT, ellipseR);
            }

            half mask = max(sideL, sideR);
            half r = (sideL > 0.0) ? 1.0 : 0.0;
            return half4(r, 0.5, 0.0, mask);
        }
    )";

    auto effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (effect == nullptr) {
        LOGE("GEBinocularShaderMask::GetBinocularShaderMaskBuilder effect error");
        return nullptr;
    }

    builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builder;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS