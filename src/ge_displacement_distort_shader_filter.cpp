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

const std::string GEDisplacementDistortFilter::type_ = Drawing::GE_FILTER_DISPLACEMENT_DISTORT;

GEDisplacementDistortFilter::GEDisplacementDistortFilter(const Drawing::GEDisplacementDistortFilterParams& params)
    :params_(params)
{}

const std::string& GEDisplacementDistortFilter::Type() const
{
    return type_;
}
bool GEDisplacementDistortFilter::SetBuilderParams(Drawing::RuntimeShaderBuilder& builder,
    const Drawing::Image& image, const Drawing::Matrix& localMatrix)
{
    if (params_.mask_ == nullptr) {
        LOGE("GEDisplacementDistortFilter::SetBuilderParams input is invalid");
        return false;
    }
    Drawing::Matrix invertMatrix;

    if (!localMatrix.Invert(invertMatrix)) {
        LOGE("GEDisplacementDistortFilter::SetBuilderParams Invert matrix failed");
        return false;
    }
    auto shader = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertMatrix);

    auto maskEffectShader =
        params_.mask_->GenerateDrawingShaderHasNormal(canvasInfo_.geoWidth, canvasInfo_.geoHeight);
    if (!maskEffectShader) {
        LOGE("GEDisplacementDistortFilter::SetBuilderParams maskEffectShader generate failed");
        return false;
    }

    builder.SetChild("image", shader);
    builder.SetChild("maskEffect", maskEffectShader);
    builder.SetUniform("iResolution", canvasInfo_.geoWidth, canvasInfo_.geoHeight);
    builder.SetUniform("factor", params_.factor_.first, params_.factor_.second);
    return true;
}

bool GEDisplacementDistortFilter::OnDrawImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::Brush& brush)
{
    if (!image) {
        LOGE("GEDisplacementDistortFilter::OnDrawImage input is invalid");
        return false;
    }
    auto displacementDistortShader = GetDisplacementDistortEffect();
    if (!displacementDistortShader) {
        LOGE("GEDisplacementDistortFilter::OnDrawImage g_displacementdistortShader init failed");
        return false;
    }

    Drawing::RuntimeShaderBuilder builder(displacementDistortShader);
    Drawing::Matrix matrix = canvasInfo_.mat;
    matrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    if (!SetBuilderParams(builder, *image, matrix)) {
        LOGE("GEDisplacementDistortFilter::OnDrawImage make shaderBuilder failed");
        return false;
    }

    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(dst.GetLeft(), dst.GetTop());
    matrix.PostConcat(translateMatrix);

    auto tmpShader = builder.MakeShader(&(matrix), false);
    brush.SetShaderEffect(tmpShader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    return true;
}

std::shared_ptr<Drawing::Image> GEDisplacementDistortFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEDisplacementDistortFilter::OnProcessImage input is invalid");
        return nullptr;
    }
    if (isSkipProcessImage_) {
        return image;
    }
    Drawing::Matrix matrix = canvasInfo_.mat;
    matrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6 || params_.mask_ == nullptr) {
        return image;
    }
    auto displacementDistortShader = GetDisplacementDistortEffect();
    if (!displacementDistortShader) {
        LOGE("GEDisplacementDistortFilter::OnProcessImage g_displacementdistortShader init failed");
        return image;
    }

    Drawing::RuntimeShaderBuilder builder(displacementDistortShader);
    if (!SetBuilderParams(builder, *image, matrix)) {
        LOGE("GEDisplacementDistortFilter::OnProcessImage make shaderBuilder failed");
        return image;
    }
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), &(matrix), imageInfo, false);
    if (!invertedImage) {
        LOGE("GEDisplacementDistortFilter::OnProcessImage make image failed");
        return image;
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