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

#include "ge_direction_light_shader_filter.h"
#include <memory>
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace {
inline static const std::string g_shaderStringNormalMask = R"(
    uniform shader mask;
    uniform vec2 iResolution;

    float bump(vec2 uv)
    {
        return mask.eval(uv).a;
    }

    vec4 main(vec2 fragCoord)
    {
        vec2 offset = vec2(1.0, iResolution.y / iResolution.x);

        float h1_u = bump(fragCoord - vec2(offset.x, 0.0));
        float h2_u = bump(fragCoord + vec2(offset.x, 0.0));
        vec3 tangent_u = vec3(2.0, 0.0, (h2_u - h1_u));

        float h1_v = bump(fragCoord - vec2(0.0, offset.y));
        float h2_v = bump(fragCoord + vec2(0.0, offset.y));
        vec3 tangent_v = vec3(0.0, 2.0, (h2_v - h1_v));

        return vec4(normalize(cross(tangent_u, tangent_v)), 1.0);
    }
)";

inline static const std::string g_shaderStringDirectionLight = R"(
    uniform shader image;
    uniform shader mask;
    uniform vec2 iResolution;
    uniform vec3 light_direction;
    uniform vec4 light_color;
    uniform float light_intensity;

    const float scale = 1.0;
    const float height = 20.0;
    const float displacement = 20.0;
    const float eta = 0.2;
    const float specular = 0.4;
    const float roughness = 0.6;
    const float metal = 0.5;
    const float inv_pi = 0.31830988618379067154;

    float pow2(float x)
    {
        return x * x;
    }

    float roughness_to_alpha(float roughness)
    {
        const float min_roughness = 0.089;
        const float min_roughness_square = 0.007921;
        return clamp(pow2(roughness), min_roughness_square, 1.0);
    }

    float distribution_GGX(float roughness, float cos_N_H)
    {
        float alpha = roughness_to_alpha(roughness);
        float alpha2 = pow2(alpha);
        float f = (cos_N_H * alpha2 - cos_N_H) * cos_N_H + 1.0;
        return alpha2 / pow2(f) * inv_pi;
    }

    float visibility_Smith_GGX_correlated_approx(float roughness, float cos_N_V, float cos_N_L)
    {
        float alpha = roughness_to_alpha(roughness);
        float term_V = cos_N_L * (cos_N_V * (1.0 - alpha) + alpha);
        float term_L = cos_N_V * (cos_N_L * (1.0 - alpha) + alpha);
        return 0.5 / (term_V + term_L);
    }

    vec3 Fresnel_Schlick(vec3 F0, float cos_V_H)
    {
        float f = pow(1.0 - cos_V_H, 5.0);
        return saturate(50.0 * F0.g) * f + (1.0 - f) * F0;
    }

    vec4 scatter(vec3 pos, vec3 displacement_normal, vec3 shading_normal, float eta)
    {
        vec3 view_dir = vec3(0.0, 0.0, 1.0);
        vec3 light_dir = normalize(light_direction);

        float cos_N_L = dot(shading_normal, light_dir);
        if (cos_N_L <= 0.0) return vec4(0.0);

        vec3 half_dir = normalize(view_dir + light_dir);
        float cos_N_V = saturate(dot(shading_normal, view_dir));
        float cos_N_H = saturate(dot(shading_normal, half_dir));
        float cos_V_H = saturate(dot(view_dir, half_dir));
        float cos_L_H = saturate(dot(light_dir, half_dir));

        vec3 in_dir = normalize(refract(-view_dir, displacement_normal, eta));
        float in_t = -pos.z / in_dir.z;
        vec3 hit_pos = pos + in_dir * in_t;

        float distribution = distribution_GGX(roughness, cos_N_H);
        float visibility = visibility_Smith_GGX_correlated_approx(roughness, cos_N_V, cos_N_L);
        vec3 albedo = image.eval(hit_pos.xy).rgb;
        vec3 diffuse_color = mix(albedo, vec3(0.0), metal);
        vec3 specular_color = mix(vec3(0.08 * specular), albedo, metal);
        vec3 fresnel = Fresnel_Schlick(specular_color, cos_V_H);

        vec3 reflected_color = distribution * visibility * fresnel * cos_N_L * light_color.rgb;
        vec3 refracted_color = diffuse_color * cos_N_L * light_color.rgb;
        return vec4(reflected_color + refracted_color, 1.0);
    }

    vec4 main(vec2 fragCoord)
    {
        vec3 pos = vec3(
            fragCoord.xy,
            displacement
        );

        vec3 normal = normalize(mask.eval(fragCoord).xyz);
        vec3 displacement_normal = normalize(vec3(1.0 / scale, 1.0 / scale,
            1.0 / (height * light_intensity * displacement)) * normal);
        vec3 shading_normal = normalize(vec3(1.0 / scale, 1.0 / scale, 1.0 / (height * light_intensity)) * normal);
        return scatter(pos, displacement_normal, shading_normal, eta);
    }
)";

inline static const std::string g_shaderStringDirectionLightNoNormal = R"(
    uniform shader image;
    uniform vec2 iResolution;
    uniform vec3 light_direction;
    uniform vec4 light_color;
    uniform float light_intensity;

    const float scale = 1.0;
    const float height = 20.0;
    const float displacement = 20.0;
    const float eta = 0.2;
    const float specular = 0.4;
    const float roughness = 0.6;
    const float metal = 0.5;
    const float inv_pi = 0.31830988618379067154;

    float pow2(float x)
    {
        return x * x;
    }

    float roughness_to_alpha(float roughness)
    {
        const float min_roughness = 0.089;
        const float min_roughness_square = 0.007921;
        return clamp(pow2(roughness), min_roughness_square, 1.0);
    }

    float distribution_GGX(float roughness, float cos_N_H)
    {
        float alpha = roughness_to_alpha(roughness);
        float alpha2 = pow2(alpha);
        float f = (cos_N_H * alpha2 - cos_N_H) * cos_N_H + 1.0;
        return alpha2 / pow2(f) * inv_pi;
    }

    float visibility_Smith_GGX_correlated_approx(float roughness, float cos_N_V, float cos_N_L)
    {
        float alpha = roughness_to_alpha(roughness);
        float term_V = cos_N_L * (cos_N_V * (1.0 - alpha) + alpha);
        float term_L = cos_N_V * (cos_N_L * (1.0 - alpha) + alpha);
        return 0.5 / (term_V + term_L);
    }

    vec3 Fresnel_Schlick(vec3 F0, float cos_V_H)
    {
        float f = pow(1.0 - cos_V_H, 5.0);
        return saturate(50.0 * F0.g) * f + (1.0 - f) * F0;
    }

    vec4 scatter(vec3 pos, vec3 displacement_normal, vec3 shading_normal, float eta)
    {
        vec3 view_dir = vec3(0.0, 0.0, 1.0);
        vec3 light_dir = normalize(light_direction);

        float cos_N_L = dot(shading_normal, light_dir);
        if (cos_N_L <= 0.0) return vec4(0.0);

        vec3 half_dir = normalize(view_dir + light_dir);
        float cos_N_V = saturate(dot(shading_normal, view_dir));
        float cos_N_H = saturate(dot(shading_normal, half_dir));
        float cos_V_H = saturate(dot(view_dir, half_dir));
        float cos_L_H = saturate(dot(light_dir, half_dir));

        vec3 in_dir = normalize(refract(-view_dir, displacement_normal, eta));
        float in_t = -pos.z / in_dir.z;
        vec3 hit_pos = pos + in_dir * in_t;

        float distribution = distribution_GGX(roughness, cos_N_H);
        float visibility = visibility_Smith_GGX_correlated_approx(roughness, cos_N_V, cos_N_L);
        vec3 albedo = image.eval(hit_pos.xy).rgb;
        vec3 diffuse_color = mix(albedo, vec3(0.0), metal);
        vec3 specular_color = mix(vec3(0.08 * specular), albedo, metal);
        vec3 fresnel = Fresnel_Schlick(specular_color, cos_V_H);

        vec3 reflected_color = distribution * visibility * fresnel * cos_N_L * light_color.rgb;
        vec3 refracted_color = diffuse_color * cos_N_L * light_color.rgb;
        return vec4(reflected_color + refracted_color, 1.0);
    }

    vec4 main(vec2 fragCoord)
    {
        vec3 pos = vec3(
            fragCoord.xy,
            displacement
        );

        vec3 normal = vec3(0.0, 0.0, 1.0);
        vec3 displacement_normal = normalize(vec3(1.0 / scale, 1.0 / scale,
            1.0 / (height * light_intensity * displacement)) * normal);
        vec3 shading_normal = normalize(vec3(1.0 / scale, 1.0 / scale, 1.0 / (height * light_intensity)) * normal);
        return scatter(pos, displacement_normal, shading_normal, eta);
    }
)";

}

GEDirectionLightShaderFilter::GEDirectionLightShaderFilter(const Drawing::GEDirectionLightShaderFilterParams& params)
    : params_(params)
{
}

std::shared_ptr<Drawing::Image> GEDirectionLightShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        return nullptr;
    }

    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> lightingBuilder = nullptr;

    if (params_.mask != nullptr) {
        // step 1: bump map to normal map
        auto normalMaskShader = GetNormalMaskEffect();
        if (normalMaskShader == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage normalMaskShader init failed");
            return image;
        }
        float imageScale = 8.0;
        float newWidth =  std::ceil(width/imageScale);
        float newHeight = std::ceil(height/imageScale);
        if (newWidth <= 0 || newHeight <= 0) {
            return image;
        }

        Drawing::RuntimeShaderBuilder normalBuilder(normalMaskShader);
        auto maskShader = params_.mask->GenerateDrawingShader(newWidth, newHeight);
        if (maskShader == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage mask generate failed");
            return image;
        }
        normalBuilder.SetChild("mask", maskShader);
        normalBuilder.SetUniform("iResolution", newWidth, newHeight);

        auto scaledInfo = Drawing::ImageInfo(newWidth, newHeight,
            Drawing::ColorType::COLORTYPE_RGBA_F16, imageInfo.GetAlphaType(),
            std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::SRGB_LINEAR));
#ifdef RS_ENABLE_GPU
        auto normalImage = normalBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
#else
        auto normalImage = normalBuilder.MakeImage(nullptr, nullptr, scaledInfo, false);
#endif
        if (normalImage == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage make normalImage failed");
            return image;
        }

        // step2: normal map to light effect map
        auto directionLightShader = GetDirectionLightEffect();
        if (directionLightShader == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage directionLightShader init failed");
            return image;
        }
        lightingBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(directionLightShader);
        if (lightingBuilder == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage lightingBuilder is nullptr");
            return image;
        }
        Drawing::Matrix maskMatrix;
        if (newWidth == 0 || newHeight == 0) {
            return image;
        }
        maskMatrix.SetScale(std::ceil(width/newWidth), std::ceil(height/newHeight));
        auto normalShader = Drawing::ShaderEffect::CreateImageShader(*normalImage, Drawing::TileMode::CLAMP,
            Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), maskMatrix);
        lightingBuilder->SetChild("mask", normalShader);
    } else {
        auto directionLightNoNormalShader = GetDirectionLightNoNormalEffect();
        if (directionLightNoNormalShader == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage directionLightNoNormalShader init failed");
            return image;
        }
        lightingBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(directionLightNoNormalShader);
        if (lightingBuilder == nullptr) {
            LOGE("GEDirectionLightShaderFilter::ProcessImage lightingBuilder is nullptr");
            return image;
        }
    }

    Drawing::Matrix imageMatrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), imageMatrix);
    lightingBuilder->SetChild("image", imageShader);
    lightingBuilder->SetUniform("iResolution", width, height);
    lightingBuilder->SetUniform("light_direction",
        params_.lightDirection.x_, params_.lightDirection.y_, -params_.lightDirection.z_);
    lightingBuilder->SetUniformVec4("light_color",
        params_.lightColor.x_, params_.lightColor.y_, params_.lightColor.z_, params_.lightColor.w_);
    lightingBuilder->SetUniform("light_intensity", params_.lightIntensity);

    auto lightInfo = Drawing::ImageInfo(width, height,
        Drawing::ColorType::COLORTYPE_RGBA_F16, imageInfo.GetAlphaType(),
        std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::SRGB_LINEAR));
#ifdef RS_ENABLE_GPU
    auto lightingImage = lightingBuilder->MakeImage(canvas.GetGPUContext().get(), nullptr, lightInfo, false);
#else
    auto lightingImage = lightingBuilder->MakeImage(nullptr, nullptr, lightInfo, false);
#endif
    if (lightingImage == nullptr) {
        LOGE("GEDirectionLightShaderFilter::ProcessImage make lightingImage failed");
        return image;
    }
    return lightingImage;
}

std::shared_ptr<Drawing::RuntimeEffect> GEDirectionLightShaderFilter::GetNormalMaskEffect()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_normalMaskShaderEffect = nullptr;
    if (g_normalMaskShaderEffect == nullptr) {
        g_normalMaskShaderEffect = Drawing::RuntimeEffect::CreateForShader(g_shaderStringNormalMask);
    }
    return g_normalMaskShaderEffect;
}

std::shared_ptr<Drawing::RuntimeEffect> GEDirectionLightShaderFilter::GetDirectionLightEffect()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_directionLightShaderEffect = nullptr;
    if (g_directionLightShaderEffect == nullptr) {
        g_directionLightShaderEffect = Drawing::RuntimeEffect::CreateForShader(g_shaderStringDirectionLight);
    }
    return g_directionLightShaderEffect;
}

std::shared_ptr<Drawing::RuntimeEffect> GEDirectionLightShaderFilter::GetDirectionLightNoNormalEffect()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_directionLightNoNormalShaderEffect = nullptr;
    if (g_directionLightNoNormalShaderEffect == nullptr) {
        g_directionLightNoNormalShaderEffect =
            Drawing::RuntimeEffect::CreateForShader(g_shaderStringDirectionLightNoNormal);
    }
    return g_directionLightNoNormalShaderEffect;
}

} // namespace Rosen
} // namespace OHOS