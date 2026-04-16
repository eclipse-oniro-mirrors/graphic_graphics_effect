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
namespace {
    const int COLOR_CHANNEL = 4;
    const int POSITION_DIMENSION = 3;
}

GESpatialPointLightShader::GESpatialPointLightShader() {}

GESpatialPointLightShader::GESpatialPointLightShader(Drawing::GESpatialPointLightShaderParams& param)
{
    GE_LOGD("GESpatialPointLightShader ctor: intensity=%f, attenuation=%f",
        param.lightIntensity, param.attenuation);
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
            uniform shader mask;

            half4 main(vec2 fragCoord)
            {
                vec2 textureUVs = fragCoord.xy / iResolution.xy;
                vec3 lightDirection = lightPosition - vec3(fragCoord.x, fragCoord.y, 0.0);
                vec3 lightDir = normalize(vec3(lightDirection.xy, lightDirection.z));
                vec4 NormalMap = mask.eval(fragCoord);

                vec3 viewDir = lightDir;
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(vec3(0.0, 0.0, 1.0), halfwayDir), 0.0), attenuation);

                vec4 fragColor = spec * lightIntensity * lightColor * NormalMap.r;
                return fragColor;
            }
        )";
        spatialPointLightShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        GE_LOGD("GESpatialPointLightShader RuntimeEffect created");
    }

    if (spatialPointLightShaderEffect_ == nullptr) {
        GE_LOGE("GESpatialPointLightShader::GetSpatialPointLightBuilder effect is nullptr.");
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
        GE_LOGE("GESpatialPointLightShader::MakeSpatialPointLightShader builder is nullptr.");
        return nullptr;
    }
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("lightPosition", pointLightParams_.lightPosition.GetData(), POSITION_DIMENSION);
    builder_->SetUniform("lightColor", pointLightParams_.lightColor.GetData(), COLOR_CHANNEL);
    builder_->SetUniform("lightIntensity", pointLightParams_.lightIntensity);
    builder_->SetUniform("attenuation", pointLightParams_.attenuation);
    if (pointLightParams_.mask != nullptr) {
        builder_->SetChild("mask", pointLightParams_.mask->GenerateDrawingShader(width, height));
    }
    auto spatialPointLightShader = builder_->MakeShader(nullptr, false);
    if (spatialPointLightShader == nullptr) {
        GE_LOGE("GESpatialPointLightShader::MakeSpatialPointLightShader shader is nullptr.");
        return nullptr;
    }
    return spatialPointLightShader;
}

} // namespace Rosen
} // namespace OHOS