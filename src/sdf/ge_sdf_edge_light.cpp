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

#include "sdf/ge_sdf_edge_light.h"

#include <unordered_map>

#include "ge_log.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_ripple_shader_mask.h"

namespace OHOS::Rosen {
namespace {
constexpr float DEFAULT_RADIUS = 6.0f;
}
GESDFEdgeLight::GESDFEdgeLight(const Drawing::GESDFEdgeLightFilterParams& params)
    : sdfSpreadFactor_(params.sdfSpreadFactor), bloomIntensityCutoff_(params.bloomIntensityCutoff),
      maxIntensity_(params.maxIntensity), maxBloomIntensity_(params.maxBloomIntensity),
      bloomFalloffPow_(params.bloomFalloffPow), minBorderWidth_(params.minBorderWidth),
      maxBorderWidth_(params.maxBorderWidth), innerBorderBloomWidth_(params.innerBorderBloomWidth),
      outerBorderBloomWidth_(params.outerBorderBloomWidth), sdfImage_(params.sdfImage), lightMask_(params.lightMask)
{}

std::shared_ptr<Drawing::Image> GESDFEdgeLight::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GESDFEdgeLight::OnProcessImage image is null");
        return image;
    }

    if (!sdfImage_) {
        LOGE("GESDFEdgeLight::OnProcessImage sdfImage_ is null");
        return image;
    }

    if (!lightMask_) {
        LOGE("GESDFEdgeLight::OnProcessImage lightMask_ is null");
        return image;
    }

    if (!blurredSdfImage_) {
        blurredSdfImage_ = BlurSdfMap(canvas, image, DEFAULT_RADIUS);
    }

    float imageWidth = image->GetWidth();
    float imageHeight = image->GetHeight();
    auto builder = MakeEffectShader(imageWidth, imageHeight);
    if (!builder) {
        LOGE("GESDFEdgeLight::OnProcessImage builder is null");
        return image;
    }

#ifdef RS_ENABLE_GPU
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
#else
    auto resultImage = builder->MakeImage(nullptr, nullptr, image->GetImageInfo(), false);
#endif
    if (!resultImage) {
        LOGE("GESDFEdgeLight::OnProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

std::shared_ptr<Drawing::Image> GESDFEdgeLight::BlurSdfMap(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> sdfImage, float radius)
{
    if (sdfImage == nullptr) {
        LOGE("GESDFEdgeLight::BlurSdfMap input is invalid");
        return nullptr;
    }

    Drawing::Rect imageRect = { 0, 0, sdfImage->GetWidth(), sdfImage->GetHeight() };

    Drawing::GEMESABlurShaderFilterParams params;
    params.radius = radius;
    GEMESABlurShaderFilter blurFilter(params);

    auto blurImage = blurFilter.OnProcessImage(canvas, sdfImage, imageRect, imageRect);
    if (blurImage == nullptr) {
        LOGE("GESDFEdgeLight::BlurSdfMap blur error");
        return sdfImage;
    }
    return blurImage;
}

void GESDFEdgeLight::SetSDFImage(std::shared_ptr<Drawing::Image> sdfImage)
{
    sdfImage_ = sdfImage;
    blurredSdfImage_ = nullptr;
}

void GESDFEdgeLight::SetLightMask(std::shared_ptr<Drawing::GEShaderMask> mask)
{
    lightMask_ = mask;
}

namespace {
constexpr char SHADER[] = R"(
    uniform vec2 iResolution;

    uniform shader sdfImageShader;
    uniform shader blurredSdfImageShader;
    uniform shader lightMaskShader;

    uniform float spreadFactor;

    uniform vec3 lightColor;
    uniform float bloomIntensityCutoff; // at what intensity is bloom supposed to disappear
    uniform float maxIntensity;
    uniform float maxBloomIntensity;
    uniform float bloomFalloffPow;

    uniform float minBorderWidth;
    uniform float maxBorderWidth;

    uniform float innerBorderBloomWidth;
    uniform float outerBorderBloomWidth;

    float bloomMultiplierFromDist(float d) {
        float dNorm = abs(d) / (d > 0 ? outerBorderBloomWidth : innerBorderBloomWidth);
        float falloff = max((1 - dNorm) / pow(1 + dNorm, bloomFalloffPow), 0);
        return maxBloomIntensity * falloff;
    }

    float edgeLightFakeBloom(float intensity, float d, float smoothD) {
        float bloomBorder = 1 - step(outerBorderBloomWidth, d);
        bloomBorder *= step(-innerBorderBloomWidth, d);

        float edgeThickness = smoothstep(0, 1, intensity) *
                              (maxBorderWidth - minBorderWidth) + minBorderWidth;
        float thinBorder = (1 - smoothstep(0, edgeThickness, d)) * smoothstep(-edgeThickness, 0, d);

        float b = intensity * maxIntensity * (thinBorder + smoothstep(bloomIntensityCutoff, 1, intensity) *
                  bloomBorder * bloomMultiplierFromDist(smoothD));
        return b;
    }

    vec4 decodeSdfMap(vec4 inputSdfMap) {
        return vec4(inputSdfMap * 2.0 - vec4(1.0)) * spreadFactor;
    }

    half4 main(vec2 fragCoord)
    {
        float lightMaskValue = lightMaskShader.eval(fragCoord).r;

        half4 result = half4(0, 0, 0, 1);

        vec4 sdfMapSample = decodeSdfMap(sdfImageShader.eval(fragCoord));
        vec4 blurredSdfMapSample = decodeSdfMap(blurredSdfImageShader.eval(fragCoord));

        result += lightColor.rgb1 * edgeLightFakeBloom(lightMaskValue, sdfMapSample.a, blurredSdfMapSample.a);

        return result;
    }
)";
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFEdgeLight::MakeEffectShader(float imageWidth, float imageHeight)
{
    static std::shared_ptr<Drawing::RuntimeEffect> effectShader_;
    if (!sdfImage_ || !blurredSdfImage_ || !lightMask_) {
        LOGE("GESDFEdgeLight::MakeEffectShader input is invalid");
        return nullptr;
    }
    if (!effectShader_) {
        effectShader_ = Drawing::RuntimeEffect::CreateForShader(SHADER);
        if (!effectShader_) {
            LOGE("MakeEffectShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }

    Drawing::Matrix matrix;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(effectShader_);

    builder->SetUniform("iResolution", imageWidth, imageHeight);

    auto sdfImageShader = Drawing::ShaderEffect::CreateImageShader(*sdfImage_, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("sdfImageShader", sdfImageShader);
    auto blurredSdfImageShader = Drawing::ShaderEffect::CreateImageShader(*blurredSdfImage_, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("blurredSdfImageShader", blurredSdfImageShader);
    auto lightMaskShader = lightMask_->GenerateDrawingShader(imageWidth, imageHeight);
    builder->SetChild("lightMaskShader", lightMaskShader);

    builder->SetUniform("spreadFactor", sdfSpreadFactor_);

    builder->SetUniform("lightColor", 1.0f, 1.0f, 1.0f);
    builder->SetUniform("bloomIntensityCutoff", bloomIntensityCutoff_);
    builder->SetUniform("maxIntensity", maxIntensity_);
    builder->SetUniform("maxBloomIntensity", maxBloomIntensity_);
    builder->SetUniform("bloomFalloffPow", bloomFalloffPow_);

    builder->SetUniform("minBorderWidth", minBorderWidth_);
    builder->SetUniform("maxBorderWidth", maxBorderWidth_);

    builder->SetUniform("innerBorderBloomWidth", innerBorderBloomWidth_);
    builder->SetUniform("outerBorderBloomWidth", outerBorderBloomWidth_);

    return builder;
}
} // namespace OHOS::Rosen
