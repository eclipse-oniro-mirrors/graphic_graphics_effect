/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_content_light_shader_filter.h"

namespace OHOS {
namespace Rosen {

constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
constexpr size_t NUM_2 = 2;
constexpr size_t NUM_3 = 3;
constexpr size_t NUM_4 = 4;

std::shared_ptr<Drawing::RuntimeEffect> GEContentLightFilter::contentLightShaderEffect_ = nullptr;

GEContentLightFilter::GEContentLightFilter(const Drawing::GEContentLightFilterParams& params)
    : lightPosition_(params.lightPosition), lightColor_(params.lightColor), lightIntensity_(params.lightIntensity),
    rotationAngle_(params.rotationAngle)
{}

std::shared_ptr<Drawing::Image> GEContentLightFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEContentLightFilter::ProcessImage input is invalid");
        return nullptr;
    }

    Drawing::Matrix matrix;
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }
    GetContentLightEffect();

    float lightColor[NUM_4] = {lightColor_[NUM_0], lightColor_[NUM_1], lightColor_[NUM_2], lightColor_[NUM_3]};
    Drawing::RuntimeShaderBuilder builder(contentLightShaderEffect_);
    builder.SetChild("image", shader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("lightIntensity", lightIntensity_);
    builder.SetUniform("lightPosition", lightPosition_[NUM_0], lightPosition_[NUM_1], lightPosition_[NUM_2]);
    builder.SetUniform("lightColor", lightColor, 4);
    builder.SetUniform("contentRotationAngle", rotationAngle_[0], rotationAngle_[1], rotationAngle_[2]);
#ifdef RS_ENABLE_GPU
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
#else
    auto invertedImage = builder.MakeImage(nullptr, nullptr, imageInfo, false);
#endif
    if (invertedImage == nullptr) {
        LOGE("GEContentLightFilter::ProcessImage make image failed");
        return nullptr;
    }
    return invertedImage;
}


void GEContentLightFilter::GetContentLightEffect()
{
    const std::string shaderStringContentLight = R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half lightIntensity;
        uniform half3 lightPosition;
        uniform half4 lightColor;
        uniform half3 contentRotationAngle;

        const float cornerRadius = 30.0;
        const float boundaryThickness = 5.0;

        float sdRoundedBox(vec2 p, vec2 b, float r)
        {
            vec2 q = abs(p) - b + r;
            return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
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

        vec4 createContentNormal(vec2 pos, float maskAlpha) // pos in [-w, w] x [-1, 1]
        {
            if (maskAlpha < 0.01) {
                return vec4(0.0, 0.0, -1.0, -1.0);
            }

            float R = max(iResolution.x, iResolution.y) * 4. / iResolution.y;
            float z = sqrt(R * R - pos.x * pos.x - pos.y * pos.y);
            vec3 normal = normalize(vec3(pos.x / z, pos.y / z, 0.5));
            
            return vec4(normal, 1.);
        }
        
        mat3 GetRotationMatrix(vec3 rotAngle)
        {
            rotAngle *= 3.1415926 / 180.0;
            mat3 Rx = mat3 {1.0, 0.0, 0.0,
                            0.0, cos(rotAngle.x), -sin(rotAngle.x),
                            0.0, sin(rotAngle.x), cos(rotAngle.x)};
            mat3 Ry = mat3 {cos(rotAngle.y), 0.0, sin(rotAngle.y),
                            0.0, 1.0, 0.0,
                            -sin(rotAngle.y), 0.0, cos(rotAngle.y)};
            mat3 Rz = mat3 {cos(rotAngle.z), -sin(rotAngle.z), 0.0,
                            sin(rotAngle.z), cos(rotAngle.z), 0.0,
                            0.0, 0.0, 1.0};
            return Rz * Ry * Rx;
        }

        vec4 ContentShinning(
            vec2 uv, vec4 specularColor, float shinning, vec3 lightPos, vec3 viewPos, mat3 rotM, float maskAlpha)
        {
            vec3 fragPos = vec3(uv, 0.0);
            vec4 normal = createContentNormal(uv, maskAlpha);
            if (normal.w < 0.0) {
                return vec4(0.0);
            }
            vec3 fragNormal = rotM * normal.xyz;

            vec4 shinningColor = shinningEffect(fragPos, fragNormal, lightPos, viewPos, specularColor, shinning);
            // shinningColor *= smoothstep(0.0, 0.1, normal.w) * smoothstep(1.0, 0.9, normal.w);
            return shinningColor;
        }

        vec4 main(in vec2 fragCoord)
        {
            vec2 uv = fragCoord / iResolution.xy;
            uv = uv + uv - 1.0;
            float screenRatio = iResolution.x / iResolution.y;
            uv.x *= screenRatio;

            float alpha = 0.7;
            mat3 rotM = GetRotationMatrix(contentRotationAngle);
            vec4 specularColor = lightColor;
            float shinning = 8.0;
            vec3 lightPos = lightPosition;

            vec3 viewPos = lightPos;

            vec4 inputImage = image.eval(fragCoord);
            vec4 shinningColor =
                ContentShinning(uv, specularColor, shinning, lightPos, viewPos, rotM, inputImage.w);

            return vec4(inputImage.rgb + shinningColor.rgb * lightIntensity, 1.0);
        }

    )";
    if (contentLightShaderEffect_ == nullptr) {
        contentLightShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(shaderStringContentLight);
    }
}
} // namespace Rosen
} // namespace OHOS