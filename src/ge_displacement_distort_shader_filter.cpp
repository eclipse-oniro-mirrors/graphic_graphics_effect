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
 
#include <chrono>
 
#include "ge_log.h"
#include "ge_displacement_distort_shader_filter.h"
 
namespace OHOS {
namespace Rosen {

GEDisplacementDistortFilter::GEDisplacementDistortFilter(const Drawing::GEDisplacementDistortFilterParams& params)
    :params_(params)
{}

 
std::shared_ptr<Drawing::Image> GEDisplacementDistortFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEDisplacementDistortFilter::ProcessImage input is invalid");
        return nullptr;
    }

    Drawing::Matrix matrix;
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6 || params_.mask_ == nullptr) {
        return nullptr;
    }

    auto maskEffectShader = params_.mask_->GenerateDrawingShaderHasNormal(width, height);
    if (!maskEffectShader) {
        LOGE("GEDisplacementDistortFilter::ProcessImage maskEffectShader generate failed");
        return nullptr;
    }

    auto displacementDistortShader = GetDisplacementDistortEffect();
    if (!displacementDistortShader) {
        LOGE("GEDisplacementDistortFilter::ProcessImage g_displacementdistortShader init failed");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder builder(displacementDistortShader);
    builder.SetChild("image", shader);
    builder.SetChild("maskEffect", maskEffectShader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("factor", params_.factor_.first, params_.factor_.second);

    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (!invertedImage) {
        LOGE("GEDisplacementDistortFilter::ProcessImage make image failed");
        return nullptr;
    }
    return invertedImage;
}

std::shared_ptr<Drawing::RuntimeEffect> GEDisplacementDistortFilter::GetDisplacementDistortEffect()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_displacementdistortShader = nullptr;
    if (g_displacementdistortShader == nullptr) {
        g_displacementdistortShader =
            Drawing::RuntimeEffect::CreateForShader(shaderStringDisplacementDistort);
    }
    return g_displacementdistortShader;
}

} // namespace Rosen
} // namespace OHOS