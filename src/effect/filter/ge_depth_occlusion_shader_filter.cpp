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

#include "ge_depth_occlusion_shader_filter.h"
#include "ge_mesa_blur_shader_filter.h"

#include "ge_log.h"

#undef LOG_TAG
#define LOG_TAG "GEDepthOcclusionShaderFilter"

namespace OHOS {
namespace Rosen {

namespace {
constexpr float MIN_IMAGE_SIZE = 1.0;
constexpr float NEAR_LIMIT = 0.001f;
constexpr float FAR_LIMIT = 1000.f;
constexpr int MASK_BLUR_RADIUS = 3;

inline static const std::string g_shaderStringDepthOcclusion = R"(
    uniform shader image;
    uniform shader mask;

    half4 main(float2 fragCoord)
    {
        half smoothAlpha = mask.eval(fragCoord).a;
        half4 imageColor = image.eval(fragCoord).rgba;
        return imageColor * smoothAlpha;
    }
)";

inline static const std::string g_shaderStringDepthOcclusionMask = R"(
    uniform shader depthMap;
    uniform half4 depthPlane;
    uniform half weight;
    uniform half near;
    uniform half far;

    half4 main(float2 fragCoord)
    {
        half sceneDisp = depthMap.eval(fragCoord).r;
        half sceneDepth = 1.0 / sceneDisp;
        half twoD = dot(depthPlane.xyw, half3(fragCoord, 1.0));
        half objectDepth = -depthPlane.z / twoD;
        const half threshold = 0.0005;
        half occlusion = smoothstep(objectDepth - threshold, objectDepth + threshold, sceneDepth);
        occlusion = mix(0.0, 1.0 - occlusion, weight);
        occlusion = max(occlusion, 1.0 - step(near, objectDepth) * step(objectDepth, far));
        return half4(occlusion);
    }
)";

} // namespace

std::shared_ptr<Drawing::RuntimeEffect> GEDepthOcclusionShaderFilter::MakeDepthOcclusionShaderEffect()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> depthOcclusionShaderEffect = nullptr;
    if (depthOcclusionShaderEffect != nullptr) {
        return depthOcclusionShaderEffect;
    }

    depthOcclusionShaderEffect = Drawing::RuntimeEffect::CreateForShader(g_shaderStringDepthOcclusion);
    if (depthOcclusionShaderEffect == nullptr) {
        LOGE("MakeDepthOcclusionShaderEffect: depthOcclusionShaderEffect create failed.");
        return nullptr;
    }

    return depthOcclusionShaderEffect;
}

std::shared_ptr<Drawing::RuntimeEffect> GEDepthOcclusionShaderFilter::MakeDepthOcclusionMaskShaderEffect()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> depthOcclusionMaskShaderEffect = nullptr;
    if (depthOcclusionMaskShaderEffect != nullptr) {
        return depthOcclusionMaskShaderEffect;
    }

    depthOcclusionMaskShaderEffect = Drawing::RuntimeEffect::CreateForShader(g_shaderStringDepthOcclusionMask);
    if (depthOcclusionMaskShaderEffect == nullptr) {
        LOGE("MakeDepthOcclusionMaskShaderEffect: depthOcclusionMaskShaderEffect create failed.");
        return nullptr;
    }

    return depthOcclusionMaskShaderEffect;
}

bool GEDepthOcclusionShaderFilter::IsInputImageValid(const std::shared_ptr<Drawing::Image>& image)
{
    if (image == nullptr) {
        LOGE("IsInputImageValid: image is nullptr.");
        return false;
    }

    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < MIN_IMAGE_SIZE || width < MIN_IMAGE_SIZE) {
        LOGE("IsInputImageValid:Input image size is invalid. H:[%{public}f] W:[%{public}f]", height, width);
        return false;
    }

    return true;
}

bool GEDepthOcclusionShaderFilter::IsInputParamsValid(const Drawing::GEDepthOcclusionShaderFilterParams& params)
{
    if (!params.depthMap) {
        LOGE("IsInputParamsValid:Input depthMap is nullptr.");
        return false;
    }

    return true;
}

std::shared_ptr<Drawing::Image> GEDepthOcclusionShaderFilter::GetOutputOnFailure(
    const std::shared_ptr<Drawing::Image>& input)
{
    // Fail-safe handling: return input normally, or nullptr in reverse mode to skip occlusion
    return params_.isReverse ? nullptr : input;
}

std::shared_ptr<Drawing::Image> GEDepthOcclusionShaderFilter::GetOcclusionMask(Drawing::Canvas &canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src)
{
    auto occlusionMaskShader = MakeDepthOcclusionMaskShaderEffect();
    if (!occlusionMaskShader) {
        return nullptr;
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(occlusionMaskShader);
    // child: depth map
    auto depthMapShader = Drawing::ShaderEffect::CreateImageShader(*params_.depthMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), params_.matrix);
    builder->SetChild("depthMap", depthMapShader);
    // uniforms
    builder->SetUniformVec4("depthPlane",
        params_.depthPlane.x_, params_.depthPlane.y_, params_.depthPlane.z_, params_.depthPlane.w_);
    builder->SetUniform("weight", std::clamp(params_.weight, 0.f, 1.f));
    builder->SetUniform("near", std::max(NEAR_LIMIT, params_.near));
    builder->SetUniform("far", std::min(FAR_LIMIT, params_.far));

    auto imageInfo = image->GetImageInfo();
    Drawing::ImageInfo outputImageInfo(imageInfo.GetWidth(), imageInfo.GetHeight(),
        imageInfo.GetColorType(), imageInfo.GetAlphaType(), params_.depthMap->GetImageInfo().GetColorSpace());
#ifdef RS_ENABLE_GPU
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
    auto resultImage = builder->MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
    if (resultImage == nullptr) {
        LOGE("GEDepthOcclusionShaderFilter::GetOcclusionMask: MakeImage failed.");
        return nullptr;
    }

    return resultImage;
}

std::shared_ptr<Drawing::Image> GEDepthOcclusionShaderFilter::OnProcessImage(Drawing::Canvas &canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst)
{
    if (!IsInputImageValid(image)) {
        return nullptr;
    }
    if (!IsInputParamsValid(params_)) {
        return GetOutputOnFailure(image);
    }

    auto imageInfo = image->GetImageInfo();
    Drawing::Rect imageRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    auto maskImage = GetOcclusionMask(canvas, image, imageRect);
    if (!maskImage) {
        return GetOutputOnFailure(image);
    }

    Drawing::GEMESABlurShaderFilterParams blurImgParams{};
    blurImgParams.radius = MASK_BLUR_RADIUS;
    auto mesaBlurFilter = std::make_shared<GEMESABlurShaderFilter>(blurImgParams);
    maskImage = mesaBlurFilter->OnProcessImage(canvas, maskImage, imageRect, imageRect);
    if (!maskImage) {
        return GetOutputOnFailure(image);
    }

    auto occlusionShader = MakeDepthOcclusionShaderEffect();
    if (!occlusionShader) {
        return GetOutputOnFailure(image);
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(occlusionShader);
    Drawing::Matrix imageMatrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), imageMatrix);
    builder->SetChild("image", imageShader);
    auto maskShader = Drawing::ShaderEffect::CreateImageShader(*maskImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), imageMatrix);
    builder->SetChild("mask", maskShader);

    Drawing::ImageInfo outputImageInfo(imageInfo.GetWidth(), imageInfo.GetHeight(),
        imageInfo.GetColorType(), imageInfo.GetAlphaType(), imageInfo.GetColorSpace());
#ifdef RS_ENABLE_GPU
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, outputImageInfo, false);
#else
    auto resultImage = builder->MakeImage(nullptr, nullptr, outputImageInfo, false);
#endif
    if (resultImage == nullptr) {
        LOGE("GEDepthOcclusionShaderFilter::OnProcessImage: MakeImage failed.");
        return GetOutputOnFailure(image);
    }

    return resultImage;
}

} // namespace Rosen
} // namespace OHOS