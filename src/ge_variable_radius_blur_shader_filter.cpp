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
#include "ge_variable_radius_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

thread_local static std::shared_ptr<Drawing::RuntimeEffect> horizontalMeanBlurShaderEffect_ = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> verticalMeanBlurShaderEffect_ = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> textureShaderEffect_ = nullptr;

GEVariableRadiusBlurShaderFilter::GEVariableRadiusBlurShaderFilter(
    const Drawing::GEVariableRadiusBlurShaderFilterParams &params)
    : params_(params)
{
}

std::shared_ptr<Drawing::Image> GEVariableRadiusBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image || params_.blurRadius <= 0 || !params_.mask) {
        return image;
    }
    LOGD("GEVariableRadiusBlurShaderFilter::ProcessImage radius: %{public}f", params_.blurRadius);
    auto imageInfo = image->GetImageInfo();
    auto maskShader = params_.mask->GenerateDrawingShader(imageInfo.GetWidth(), imageInfo.GetHeight());
    if (maskShader == nullptr) {
        LOGE("GEVariableRadiusBlurShaderFilter::ProcessImage mask generate failed");
        return image;
    }
    float radius = std::clamp(params_.blurRadius, 0.0f, 60.0f); // 60.0 represents largest blur radius
    radius = radius / 2; // 2 half blur radius
    MakeHorizontalMeanBlurEffect();
    MakeVerticalMeanBlurEffect();
    MakeTextureShaderEffect();
    return DrawMeanLinearGradientBlur(image, canvas, radius, maskShader, dst);
}

void GEVariableRadiusBlurShaderFilter::MakeTextureShaderEffect()
{
    static const std::string generateTextureShader(R"(
        uniform shader imageInput;
        half4 main(float2 xy) {
            return imageInput.eval(xy);
        }
    )");
    if (textureShaderEffect_ == nullptr) {
        textureShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(generateTextureShader);
    }
}

void GEVariableRadiusBlurShaderFilter::MakeHorizontalMeanBlurEffect()
{
    static const std::string HorizontalBlurString(
        R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half x = -30.0; x < 30.0; x += 1.0) {
                if (x > radius) {
                    break;
                }
                if (abs(x) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(x, 0));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )");
    if (horizontalMeanBlurShaderEffect_ == nullptr) {
        horizontalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(HorizontalBlurString);
    }
}

void GEVariableRadiusBlurShaderFilter::MakeVerticalMeanBlurEffect()
{
    static const std::string VerticalBlurString(
        R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half y = -30.0; y < 30.0; y += 1.0) {
                if (y > radius) {
                    break;
                }
                if (abs(y) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(0, y));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )");
    if (verticalMeanBlurShaderEffect_ == nullptr) {
        verticalMeanBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(VerticalBlurString);
    }
}

std::shared_ptr<Drawing::Image> GEVariableRadiusBlurShaderFilter::DrawMeanLinearGradientBlur(
    const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas, float radius,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst)
{
    if (!horizontalMeanBlurShaderEffect_ || !verticalMeanBlurShaderEffect_ || !image) {
        return image;
    }
    Drawing::Matrix m;
    Drawing::Matrix blurMatrix;
    blurMatrix.PostTranslate(dst.GetLeft(), dst.GetTop());
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto tmpBlur4 = BuildMeanLinearGradientBlur(image, canvas, radius, alphaGradientShader, blurMatrix);
    Drawing::Matrix invBlurMatrix;
    auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *tmpBlur4, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, invBlurMatrix);
    Drawing::RuntimeShaderBuilder builder(textureShaderEffect_);
    builder.SetChild("imageInput", blurShader);
    auto imageInfo = Drawing::ImageInfo(dst.GetWidth(), dst.GetHeight(), image->GetImageInfo().GetColorType(),
        image->GetImageInfo().GetAlphaType(), image->GetImageInfo().GetColorSpace());
#ifdef RS_ENABLE_GPU
    auto blurImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
#else
    auto blurImage = builder.MakeImage(nullptr, nullptr, imageInfo, false);
#endif
    if (!blurImage) {
        LOGE("GEVariableRadiusBlurShaderFilter::DrawMeanLinearGradientBlur fail to make final image");
        return image;
    }
    return blurImage;
}
 
std::shared_ptr<Drawing::Image> GEVariableRadiusBlurShaderFilter::BuildMeanLinearGradientBlur(
    const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas, float radius,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, Drawing::Matrix blurMatrix)
{
    auto width = image->GetWidth();
    auto height = image->GetHeight();
    auto originImageInfo = image->GetImageInfo();
    auto scaledInfo = Drawing::ImageInfo(std::ceil(width), std::ceil(height),
        originImageInfo.GetColorType(), originImageInfo.GetAlphaType(), originImageInfo.GetColorSpace());
    Drawing::Matrix m;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::RuntimeShaderBuilder hBlurBuilder(horizontalMeanBlurShaderEffect_);
    hBlurBuilder.SetUniform("r", radius);
    hBlurBuilder.SetChild("gradientShader", alphaGradientShader);
 
    Drawing::RuntimeShaderBuilder vBlurBuilder(verticalMeanBlurShaderEffect_);
    vBlurBuilder.SetUniform("r", radius);
    vBlurBuilder.SetChild("gradientShader", alphaGradientShader);
    int blurTimes = 4;
    auto localImage = image;
    auto blurImageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, blurMatrix);
    bool isHorizontal = true;
    for (int i = 0; i < blurTimes; i++) {
        Drawing::RuntimeShaderBuilder blurBuilder = isHorizontal ? hBlurBuilder : vBlurBuilder;
        isHorizontal = !isHorizontal;
        blurBuilder.SetChild("imageShader", blurImageShader);
        localImage =
#ifdef RS_ENABLE_GPU
            blurBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, scaledInfo, false);
#else
            blurBuilder.MakeImage(nullptr, nullptr, scaledInfo, false);
#endif
        if (!localImage) {
            LOGE("GEVariableRadiusBlurShaderFilter::BuildMeanLinearGradientBlur fail to make blur image");
            return image;
        }
        blurImageShader = Drawing::ShaderEffect::CreateImageShader(
            *localImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, m);
    }
    return localImage;
}

std::string GEVariableRadiusBlurShaderFilter::GetDescription()
{
    return "GEVariableRadiusBlurShaderFilter";
}

std::string GEVariableRadiusBlurShaderFilter::GetDetailedDescription()
{
    return "GEVariableRadiusBlurShaderFilterBlur, radius: " + std::to_string(params_.blurRadius);
}
} // namespace Rosen
} // namespace OHOS