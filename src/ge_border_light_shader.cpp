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

namespace OHOS {
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

void GEBorderLightShader::SetCornerRadius(const float cornerRadius)
{
    borderLightParams_.cornerRadius_ = cornerRadius;
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
            uniform half cornerRadius;

            half sdRoundedBox(half2 p, half2 b, half r)
            {
                half2 q = abs(p) - b + r;
                return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
            }

            half2 sdRoundedBoxGradient(half2 p, half2 b, half r)
            {
                half2 signs = half2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
                half2 q = abs(p) - b + r;
                half2 nor = (q.y > q.x) ? half2(0.0, 1.0) : half2(1.0, 0.0);
                nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
                return signs * nor;
            }

            half4 shinningEffect(in half3 fragPos, in half3 normal, in half3 lightPos, in half3 viewPos,
                in half4 specularColor, in half shinning)
            {
                half3 lightDir = normalize(lightPos - fragPos);
                half3 viewDir = normalize(viewPos - fragPos);
                half3 halfwayDir = normalize(lightDir + viewDir);
                
                // Blinn-Phong
                half4 specularC = specularColor * pow(max(dot(normal, halfwayDir), 0.), shinning);

                return specularC;
            }

            half4 createBoundNormal(half2 pos) // pos in [-w, w] x [-1, 1]
            {
                half2 halfWH = half2(iResolution.x, iResolution.y) / iResolution.y;
                half r = min(cornerRadius / iResolution.y * 2.0, min(halfWH.x, halfWH.y));
                half dist = sdRoundedBox(pos, halfWH, r);
                half thickness = min(lightWidth, cornerRadius) / iResolution.y;
                if (dist >= 0.0 || dist <= -thickness) {
                    return half4(0.0, 0.0, 1.0, -1.0);
                }

                half R = thickness * 9.0;  // 9.0 is adjustable parameters
                half sinTheta = abs(dist) / R;

                half normalZ = sqrt(1.0 - sinTheta * sinTheta);
                half2 normalXY = sdRoundedBoxGradient(pos, halfWH, r) * sinTheta;

                return half4(normalXY, normalZ, dist / thickness + 1.0);
            }

            half4 RoundedBoxShinning(half2 uv, half4 specularColor, half shinning, half3 lightPos, half3 viewPos, mat3 rotM)
            {
                half3 fragPos = half3(uv, 0.0);
                half4 normal = createBoundNormal(uv);
                if (normal.w < 0.0) {
                    return half4(0.0);
                }
                half3 fragNormal = rotM * normal.xyz;

                half4 shinningColor = shinningEffect(fragPos, fragNormal, lightPos, viewPos, specularColor, shinning);
                shinningColor *= smoothstep(0.0, 0.1, normal.w); // edge smooth
                return shinningColor;
            }

            mat3 GetRotationMatrix(half3 rotAngle)
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

            half4 main(vec2 fragCoord)
            {
                half2 uv = fragCoord / iResolution.xy;
                uv = uv + uv - 1.0;
                half screenRatio = iResolution.x / iResolution.y;
                uv.x *= screenRatio;

                half4 shinningColor = half4(0.0);
                half3 lightPos = half3(lightPosition.x * screenRatio, lightPosition.y, lightPosition.z);

                if (dot(borderLightRotationAngle, borderLightRotationAngle) < 0.01) {
                    half r = min(cornerRadius * 2.0, min(iResolution.x, iResolution.y));
                    half2 q = abs(fragCoord + fragCoord - iResolution.xy) - iResolution.xy + r;
                    half dist = min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
                    half normalizedDist = dist / (lightWidth + 0.001) + 1.0;
                    shinningColor = (normalizedDist > 0.0 && normalizedDist < 1.0)
                        ? lightColor * pow(max(normalize(lightPos - half3(uv, 0.0)).z, 0.), 16.0) : half4(0.0);
                    shinningColor *= smoothstep(0.0, 0.1, normalizedDist); // edge smooth
                } else {
                    mat3 rotM = GetRotationMatrix(borderLightRotationAngle);
                    half4 specularColor = lightColor;
                    half shinning = 16.0f;
                    half3 viewPos = lightPos;
                    shinningColor = RoundedBoxShinning(uv, specularColor, shinning, lightPos, viewPos, rotM);
                }

                return half4(shinningColor.xyz * lightColor.w, shinningColor.w) * clamp(lightIntensity, 0.0, 1.0);
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
    builder_->SetUniform("cornerRadius", borderLightParams_.cornerRadius_);
    auto borderLightShader = builder_->MakeShader(nullptr, false);

    if (borderLightShader == nullptr) {
        GE_LOGE("GEBorderLightShader::MakeBorderLightShader borderLightShader is nullptr.");
        return nullptr;
    }
    return borderLightShader;
}

} // namespace Rosen
} // namespace OHOS