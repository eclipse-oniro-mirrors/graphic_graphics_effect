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
#include "ge_log.h"
#include "ge_border_light_shader.h"
#include "ge_visual_effect_impl.h"

namespace OHOS{
namespace Rosen {

constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
constexpr size_t NUM_2 = 2;
constexpr size_t NUM_3 = 3;
constexpr size_t NUM_4 = 4;

GEBorderLightShader::GEBorderLightShader() {}

GEBorderLightShader::GEBorderLightShader(BorderLightParams& param)
{
    borderLightParams_ = param;
}

std::shared_ptr<GEBorderLightShader> GEBorderLightShader::CreateBorderLightShader(BorderLightParams& param)
{
    std::shared_ptr<GEBorderLightShader> borderLightShader = std::make_shared<GEBorderLightShader>(param);
    return borderLightShader;
}

void GEBorderLightShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeBorderLightShader(rect);
}

void GEBorderLightShader::SetRotationAngle(const Vector3f& rotationAngle)
{
    borderLightParams_.rotationAngle_ = rotationAngle;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEBorderLightShader::GetBorderLightBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> borderLightShaderEffect_ = nullptr;

    if (borderLightShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform half2 iResolution;
            uniform half3 lightPosition;
            uniform half4 lightColor;
            uniform half lightIntensity;
            uniform half lightWidth;
            uniform half3 borderLightRotationAngle;

            const float cornerRadius = 30.0;
            const float boundaryThickness = 5.0;

            // 计算圆角矩形SDF
            float sdRoundedBox(vec2 p, vec2 b, float r)
            {
                vec2 q = abs(p)-b+r;
                return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
            }

            vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
            {
                vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
                vec2 q = abs(p) - b + r;
                vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
                nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
                return signs * nor;
            }

            vec4 shinningEffect(in vec3 fragPos, in vec3 normal, in vec3 lightPos, in vec3 viewPos,
                in vec4 specularColor, in float shinning)
            {
                vec3 lightDir = normalize(lightPos - fragPos);
                vec3 viewDir = normalize(viewPos - fragPos);
                vec3 halfwayDir = normalize(lightDir + viewDir);
                
                // Blinn-Phong
                vec4 specularC = specularColor * pow(max(dot(normal, halfwayDir), 0.), shinning);

                return specularC;
            }

            vec4 createBoundNormal(vec2 pos) // pos in [-w, w] x [-1, 1]
            {
                vec2 halfWH = vec2(iResolution.x, iResolution.y) / iResolution.y;
                float r = min(cornerRadius / iResolution.y, min(halfWH.x, halfWH.y));
                float dist = sdRoundedBox(pos, halfWH, r);
                float thickness = min(boundaryThickness, cornerRadius) / iResolution.y;
                if (dist >= 0.0 || dist <= -thickness) {
                    return vec4(0.0, 0.0, 1.0, -1.0);
                }

                dist += thickness;
                float R = thickness * 2.0; // 1.0 is adjustable parameters
                float sinTheta = dist / R;
                float normalZ = sqrt(1.0 - sinTheta * sinTheta);
                vec2 normalXY = sdRoundedBoxGradient(pos, halfWH, r) * sinTheta;

                return vec4(normalXY, normalZ, dist / thickness);
            }

            vec4 RoundedBoxShinning(vec2 uv, vec4 specularColor, float shinning, vec3 lightPos, vec3 viewPos, mat3 rotM)
            {
                vec3 fragPos = vec3(uv, 0.0);
                vec4 normal = createBoundNormal(uv);
                if (normal.w < 0.0) {
                    return vec4(0.0);
                }
                vec3 fragNormal = rotM * normal.xyz;

                vec4 shinningColor = shinningEffect(fragPos, fragNormal, lightPos, viewPos, specularColor, shinning);
                shinningColor *= smoothstep(0.0, 0.1, normal.w) * smoothstep(1.0, 0.9, normal.w);
                return shinningColor;
            }

            mat3 GetRotationMatrix(vec3 rotAngle)
            {
                rotAngle *= 3.1415926 / 180.0;
                mat3 Rx = mat3 (1.0, 0.0, 0.0,
                                0.0, cos(rotAngle.x), -sin(rotAngle.x),
                                0.0, sin(rotAngle.x), cos(rotAngle.x));
                mat3 Ry = mat3 (cos(rotAngle.y), 0.0, sin(rotAngle.y),
                                0.0, 1.0, 0.0,
                                -sin(rotAngle.y), 0.0, cos(rotAngle.y));
                mat3 Rz = mat3 (cos(rotAngle.z), -sin(rotAngle.z), 0.0,
                                sin(rotAngle.z), cos(rotAngle.z), 0.0,
                                0.0, 0.0, 1.0);
                return Rz * Ry * Rx;
            }

            vec4 main(vec2 fragCoord )
            {
                vec2 uv = fragCoord / iResolution.xy;
                uv = uv + uv - 1.0;
                float screenRatio = iResolution.x / iResolution.y;
                uv.x *= screenRatio;

                mat3 rotM = GetRotationMatrix(borderLightRotationAngle);
                vec4 specularColor = lightColor;
                float shinning = 16.0f;
                vec3 lightPos = lightPosition;
                vec3 viewPos = lightPos;
                vec4 shinningColor = RoundedBoxShinning(uv, specularColor, shinning, lightPos, viewPos, rotM);

                return shinningColor * lightIntensity;
            }
        )";
        borderLightShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);

    }

    if (borderLightShaderEffect_ == nullptr) {
        GE_LOGE("GEBorderLightShader::GetBorderLightBuilder borderLightShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(borderLightShaderEffect_);
}

std::shared_ptr<Drawing::ShaderEffect> GEBorderLightShader::MakeBorderLightShader(const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    float lightColor[NUM_4] = {borderLightParams_.lightColor_[NUM_0], borderLightParams_.lightColor_[NUM_1],
        borderLightParams_.lightColor_[NUM_2], borderLightParams_.lightColor_[NUM_3]};
    builder_ = GetBorderLightBuilder();
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("lightPosition", borderLightParams_.lightPosition_[NUM_0],
        borderLightParams_.lightPosition_[NUM_1], borderLightParams_.lightPosition_[NUM_2]);
    builder_->SetUniform("lightColor", lightColor, NUM_4);
    builder_->SetUniform("lightIntensity", borderLightParams_.lightIntensity_);
    builder_->SetUniform("lightWidth", borderLightParams_.lightWidth_);
    builder_->SetUniform("borderLightRotationAngle", borderLightParams_.rotationAngle_[NUM_0],
        borderLightParams_.rotationAngle_[NUM_1], borderLightParams_.rotationAngle_[NUM_2]);

    auto borderLightShader = builder_->MakeShader(nullptr, false);

    if (borderLightShader == nullptr) {
        GE_LOGE("GEBorderLightShader::MakeBorderLightShader borderLightShader is nullptr.");
        return nullptr;
    }
    return borderLightShader;
}

} // namespace Rosen
} // namespace OHOS