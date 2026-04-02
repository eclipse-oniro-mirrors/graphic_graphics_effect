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
#include "ge_log.h"
#include "ge_spatial_point_light.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {

GESpatialPointLightShader::GESpatialPointLightShader() {}

GESpatialPointLightShader::GESpatialPointLightShader(Drawing::GESpatialPointLightShaderParams& param)
{
    pointLightParams_ = param;
}

void GESpatialPointLightShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeSpatialPointLightShader(rect);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESpatialPointLightShader::GetSpatialPointLightBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> spatialPointLightShaderEffect_ = nullptr;

    if (spatialPointLightShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform vec2 iResolution;
            uniform vec3 lightPosition;
            uniform half4 lightColor;
            uniform half lightIntensity;
            uniform half attenuation;
            uniform sampler2D maskSampler;

            half4 main(vec2 fragCoord)
            {
                vec2 uv = fragCoord / iResolution.xy;
                
                vec3 fragPos = vec3(uv, 0.0);
                vec3 lightPos = vec3(lightPosition.x, lightPosition.y, lightPosition.z);
                
                float distance = length(fragPos - lightPos);
                float attenuationFactor = 1.0 / (1.0 + attenuation * distance * distance);
                
                vec3 lightDir = normalize(lightPos - fragPos);
                vec3 normal = vec3(0.0, 0.0, 1.0);
                vec3 viewDir = vec3(0.0, 0.0, 1.0);
                vec3 halfwayDir = normalize(lightDir + viewDir);
                
                float ndotl = max(dot(normal, lightDir), 0.0);
                float ndoth = max(dot(normal, halfwayDir), 0.0);
                
                vec3 diffuse = lightColor.rgb * ndotl;
                vec3 specular = lightColor.rgb * pow(ndoth, 32.0);
                
                vec3 result = (diffuse + specular) * lightIntensity * attenuationFactor;
                
                float maskValue = 1.0;
                if (attenuation > 0.0) {
                    maskValue = texture2D(maskSampler, uv).a;
                }
                
                return half4(result * maskValue, lightColor.a * maskValue);
            }
        )";
        spatialPointLightShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (spatialPointLightShaderEffect_ == nullptr) {
        GE_LOGE("GESpatialPointLightShader::GetSpatialPointLightBuilder spatialPointLightShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(spatialPointLightShaderEffect_);
}

std::shared_ptr<Drawing::ShaderEffect> GESpatialPointLightShader::MakeSpatialPointLightShader(const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    
    builder_ = GetSpatialPointLightBuilder();
    if (builder_ == nullptr) {
        GE_LOGE("GESpatialPointLightShader::MakeSpatialPointLightShader builder_ is nullptr.");
        return nullptr;
    }
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("lightPosition", pointLightParams_.lightPosition.x,
        pointLightParams_.lightPosition.y, pointLightParams_.lightPosition.z);
    builder_->SetUniform("lightColor", pointLightParams_.lightColor.x,
        pointLightParams_.lightColor.y, pointLightParams_.lightColor.z, pointLightParams_.lightColor.w);
    builder_->SetUniform("lightIntensity", pointLightParams_.lightIntensity);
    builder_->SetUniform("attenuation", pointLightParams_.attenuation);
    if (pointLightParams_.mask != nullptr) {
        builder_->SetChild("maskSampler", pointLightParams_.mask->GetDrawingShader(rect, 0.0f));
    }
    auto spatialPointLightShader = builder_->MakeShader(nullptr, false);
    if (spatialPointLightShader == nullptr) {
        GE_LOGE("GESpatialPointLightShader::MakeSpatialPointLightShader spatialPointLightShader is nullptr.");
        return nullptr;
    }
    return spatialPointLightShader;
}

} // namespace Rosen
} // namespace OHOS