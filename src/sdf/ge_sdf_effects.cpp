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

#include "sdf/ge_sdf_effects.h"
#include "effect/runtime_shader_builder.h"

#include "ge_log.h"
static constexpr float SDF_EFFECT_MIN_THRESHOLD = 0.0001f;
namespace OHOS::Rosen::Drawing {
    bool GESDFEffect::InOrderComparator(const GESDFEffect& effect1,
                                        const GESDFEffect& effect2)
    {
        return effect1.GetOrder() < effect2.GetOrder();
    }

    GESDFBorder::GESDFBorder(const Color& color, float width)
        : color_(color), width_(width)
    { }

    void GESDFBorder::Process(std::string& headers, std::string& calls, std::string& functions) const
    {
        headers += "uniform vec3 u_borderColor;\n";
        headers += "uniform float u_borderWidth;\n";

        functions += R"(
            // Input data:
            // float d - current SDF shape distance
            // vec4 color - color of the background
            // vec3 borderColor - color of the border
            // float borderWidth - width of the border
            vec4 borderEffect(float d, vec4 color, vec3 borderColor, float borderWidth)
            {
                if (d < 0.0 && d >= -borderWidth)
                {
                    color = vec4(borderColor, 1.0);
                }
                
                return color;
            }
        )";

        calls += R"(
            color = borderEffect(d, color, u_borderColor, u_borderWidth);
        )";
    }

    void GESDFBorder::UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder) const
    {
        builder.SetUniform("u_borderColor", static_cast<float>(color_.GetRed()) / 255.0f,
                                static_cast<float>(color_.GetGreen()) / 255.0f,
                                static_cast<float>(color_.GetBlue()) / 255.0f);
        builder.SetUniform("u_borderWidth", std::max(width_, SDF_EFFECT_MIN_THRESHOLD));
    }

    SDFEffectOrder GESDFBorder::GetOrder() const
    {
        return SDFEffectOrder::BORDER;
    }

    GESDFShadow::GESDFShadow(const Color& color, float offsetX, float offsetY,
                                float radius, const Path& path, bool isFilled)
        : color_(color), offsetX_(offsetX), offsetY_(offsetY), radius_(radius), isFilled_(isFilled)
    { }

    void GESDFShadow::Process(std::string& headers, std::string& calls, std::string& functions) const
    {
        headers += "uniform vec3 u_shadowColor;\n";
        headers += "uniform vec2 u_shadowOffset;\n";
        headers += "uniform float u_shadowRadius;\n";
        headers += "uniform float u_shadowIsFilled;\n";

        functions += R"(
            // Input data:
            // vec2 coord - coordinates used to calculate SDFMap
            // float d - current SDF shape distance
            // vec4 color - color of the background
            // vec3 shadowColor - color of the shadow
            // vec2 shadowOffset - offset of the shadow
            // float shadowRadius - radius of the shadow
            // bool isFilled - should SDFBody be filled with shadow
            vec4 shadowEffect(vec2 coord, float d, vec4 color, vec3 shadowColor,
                                vec2 shadowOffset, float shadowRadius, bool isFilled)
            {
                if (!isFilled && d < 0.0)
                {
                    return color;
                }
                // Recalculate the distance if there is offset
                if (any(notEqual(shadowOffset, vec2(0.0))))
                {
                    d = SDFMap(coord - shadowOffset);
                }
                
                if (d <= shadowRadius)
                {
                    d += shadowRadius * 0.5;
                    float alpha = clamp(d / shadowRadius, 0.0, 1.0);
                    alpha = 1.0 - alpha;
                    color = color * (1.0 - alpha) + vec4(shadowColor, 1.0) * alpha;
                }
                
                return color;
            }
        )";

        calls += R"(
            color = shadowEffect(coord, d, u_color, u_shadowColor, u_shadowOffset,
                    u_shadowRadius, !(u_shadowIsFilled < 1.0));
        )";
    }

    void GESDFShadow::UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder) const
    {
        builder.SetUniform("u_shadowColor", static_cast<float>(color_.GetRed()) / 255.0f,
                                static_cast<float>(color_.GetGreen()) / 255.0f,
                                static_cast<float>(color_.GetBlue()) / 255.0f);
        builder.SetUniform("u_shadowOffset", offsetX_, offsetY_);
        builder.SetUniform("u_shadowRadius", std::max(radius_, SDF_EFFECT_MIN_THRESHOLD));
        builder.SetUniform("u_shadowIsFilled", static_cast<int>(isFilled_));
    }

    SDFEffectOrder GESDFShadow::GetOrder() const
    {
        return SDFEffectOrder::SHADOW;
    }
} // OHOS::Rosen::Drawing