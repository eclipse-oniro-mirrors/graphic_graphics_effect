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
#include "ge_linear_gradient_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
constexpr static uint8_t DIRECTION_NUM = 4;

static bool GetMaskLinearBlurEnabled()
{
#ifdef GE_OHOS
    // Determine whether the mask LinearBlur render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.maskLinearBlurEnabled", "1")).c_str()) != 0;
    return enabled;
#else
    return false;
#endif
}
} // namespace

thread_local static std::shared_ptr<Drawing::RuntimeEffect> maskBlurShaderEffect_ = nullptr;

GELinearGradientBlurShaderFilter::GELinearGradientBlurShaderFilter(
    const Drawing::GELinearGradientBlurShaderFilterParams& params)
{
    geoWidth_ = params.geoWidth;
    geoHeight_ = params.geoHeight;
    auto maskLinearBlur = GetMaskLinearBlurEnabled();
    linearGradientBlurPara_ = std::make_shared<GELinearGradientBlurPara>(
        params.blurRadius, params.fractionStops, static_cast<GEGradientDirection>(params.direction), maskLinearBlur);
    linearGradientBlurPara_->isRadiusGradient_ = params.isRadiusGradient;
    mat_ = params.mat;
    tranX_ = params.tranX;
    tranY_ = params.tranY;
    isOffscreenCanvas_ = params.isOffscreenCanvas;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::ProcessImageDDGR(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image, uint8_t directionBias)
{
    auto& para = linearGradientBlurPara_;
    auto clipIPadding = Drawing::Rect(0, 0, geoWidth_ * imageScale_, geoHeight_ * imageScale_);
    uint8_t direction = static_cast<uint8_t>(para->direction_);
    TransformGradientBlurDirection(direction, directionBias);
    float radius = para->blurRadius_;

    Drawing::Brush brush;
    Drawing::Filter imageFilter;
    Drawing::GradientBlurType blurType;
    if (GetMaskLinearBlurEnabled() && !para->isRadiusGradient_) {
        blurType = Drawing::GradientBlurType::ALPHA_BLEND;
        radius /= 2; // 2: half radius.
    } else {
        radius = std::clamp(radius, 0.0f, 60.0f); // 60.0 represents largest blur radius
        blurType = Drawing::GradientBlurType::RADIUS_GRADIENT;
    }
    imageFilter.SetImageFilter(Drawing::ImageFilter::CreateGradientBlurImageFilter(
        radius, para->fractionStops_, static_cast<Drawing::GradientDir>(direction), blurType, nullptr));
    brush.SetFilter(imageFilter);

    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawImageRect(
        *image, rect, clipIPadding, Drawing::SamplingOptions(), Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
    return image;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto& para = linearGradientBlurPara_;
    if (!image || para == nullptr || para->blurRadius_ <= 0) {
        return image;
    }
    LOGD("GELinearGradientBlurShaderFilter::DrawImageRect%{public}f,  %{public}f, %{public}f, %{public}f, %{public}f "
         "%{public}d", para->blurRadius_, geoWidth_, geoHeight_, tranX_, tranY_, (int)isOffscreenCanvas_);

    ComputeScale(dst.GetWidth(), dst.GetHeight(), !para->isRadiusGradient_);
    Drawing::Point pts[2];
    uint8_t direction = static_cast<uint8_t>(para->direction_);
    auto clipIPadding = Drawing::Rect(0, 0, geoWidth_ * imageScale_, geoHeight_ * imageScale_);
    constexpr uint8_t directionBias = 0;
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGEGradientDirectionPoints(pts, clipIPadding, static_cast<GEGradientDirection>(direction));
    if (!result) {
        return image;
    }
    Drawing::GELinearGradientShaderMaskParams maskParams {para->fractionStops_, pts[0], pts[1]};
    auto mask = std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams);

    if (GetMaskLinearBlurEnabled() && !para->isRadiusGradient_) {
        // use faster LinearGradientBlur if valid
        if (para->linearGradientBlurFilter_ == nullptr) {
            LOGE("RSPropertiesPainter::DrawLinearGradientBlur blurFilter null");
            return image;
        }

        const auto& RSFilter = para->linearGradientBlurFilter_;
        auto filter = RSFilter;
        auto alphaGradientShader = mask->GenerateDrawingShader(geoWidth_ * imageScale_, geoHeight_ * imageScale_);
        if (alphaGradientShader == nullptr) {
            LOGE("GELinearGradientBlurShaderFilter::alphaGradientShader null");
            return image;
        }
        return DrawMaskLinearGradientBlur(image, canvas, filter, alphaGradientShader, dst);
    } else {
        // use original LinearGradientBlur
        float radius = std::clamp(para->blurRadius_, 0.0f, 60.0f); // 60.0 represents largest blur radius
        Drawing::GEVariableRadiusBlurShaderFilterParams radiusGradientBlurParam {mask, radius};
        GEVariableRadiusBlurShaderFilter radiusGradientBlur(radiusGradientBlurParam)；
        return radiusGradientBlur.ProcessImage(canvas, image, src, dst);
    }
}

void GELinearGradientBlurShaderFilter::ComputeScale(float width, float height, bool useMaskAlgorithm)
{
    if (GetMaskLinearBlurEnabled() && useMaskAlgorithm) {
        imageScale_ = 1.0f;
    } else {
        if (width * height < 10000) { // 10000 for 100 * 100 resolution
            imageScale_ = 0.7f;       // 0.7 for scale
        } else {
            imageScale_ = 0.5f; // 0.5 for scale
        }
    }
}

uint8_t GELinearGradientBlurShaderFilter::CalcDirectionBias(const Drawing::Matrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
        // 0 and 4 represents rotate matrix's index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
        // 1 and 3 represents rotate matrix's index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}

void GELinearGradientBlurShaderFilter::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(GEGradientDirection::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_TOP) ||
               direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(GEGradientDirection::BOTTOM)) {
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
    } else {
        direction -= DIRECTION_NUM;
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
        direction += DIRECTION_NUM;
    }
    if (direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(GEGradientDirection::LEFT_BOTTOM) ||
               direction == static_cast<uint8_t>(GEGradientDirection::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

bool GELinearGradientBlurShaderFilter::GetGEGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction) // 2 size of points
{
    switch (direction) {
        case GEGradientDirection::BOTTOM: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop()); // 2 middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            break;
        }
        case GEGradientDirection::TOP: {
            pts[0].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetBottom()); // 2  middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2 + clipBounds.GetLeft(), clipBounds.GetTop());    // 2  middle of width;
            break;
        }
        case GEGradientDirection::RIGHT: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            pts[1].Set(clipBounds.GetRight(),
                clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        case GEGradientDirection::LEFT: {
            pts[0].Set(clipBounds.GetRight(),
                clipBounds.GetHeight() / 2 + clipBounds.GetTop());                              // 2  middle of height;
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetHeight() / 2 + clipBounds.GetTop()); // 2  middle of height;
            break;
        }
        default: {
        }
    }
    return ProcessGradientDirectionPoints(pts, clipBounds, direction);
}

bool GELinearGradientBlurShaderFilter::ProcessGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction)  // 2 size of points
{
    switch (direction) {
        case GEGradientDirection::RIGHT_BOTTOM: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            break;
        }
        case GEGradientDirection::LEFT_TOP: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetTop());
            break;
        }
        case GEGradientDirection::LEFT_BOTTOM: {
            pts[0].Set(clipBounds.GetRight(), clipBounds.GetTop());
            pts[1].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            break;
        }
        case GEGradientDirection::RIGHT_TOP: {
            pts[0].Set(clipBounds.GetLeft(), clipBounds.GetBottom());
            pts[1].Set(clipBounds.GetRight(), clipBounds.GetTop());
            break;
        }
        default: {
        }
    }
    Drawing::Matrix pointsMat = mat_;
    if (isOffscreenCanvas_) {
        pointsMat.PostTranslate(-tranX_, -tranY_);
    }
    std::vector<Drawing::Point> points(pts, pts + 2); // 2 size of pts
    pointsMat.MapPoints(points, points, points.size());
    pts[0].Set(points[0].GetX(), points[0].GetY());
    pts[1].Set(points[1].GetX(), points[1].GetY());
    return true;
}

std::shared_ptr<Drawing::Image> GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur(
    const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas, std::shared_ptr<GEShaderFilter>& blurFilter,
    std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur image is null");
        return image;
    }

    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetWidth() < 1e-6 || imageInfo.GetHeight() < 1e-6) {
        return image;
    }
    auto srcRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    auto blurImage = blurFilter->ProcessImage(canvas, image, srcRect, dst);

    Drawing::Matrix matrix;
    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / imageInfo.GetWidth(), dst.GetHeight() / imageInfo.GetHeight());

    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), inputMatrix);
    auto blurImageShader = Drawing::ShaderEffect::CreateImageShader(*blurImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto builder = MakeMaskLinearGradientBlurShader(srcImageShader, blurImageShader, alphaGradientShader);
    auto outImageInfo = Drawing::ImageInfo(dst.GetWidth(), dst.GetHeight(), blurImage->GetImageInfo().GetColorType(),
        blurImage->GetImageInfo().GetAlphaType(), blurImage->GetImageInfo().GetColorSpace());
#ifdef RS_ENABLE_GPU
    auto outImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, outImageInfo, false);
#else
    auto outImage = builder->MakeImage(nullptr, nullptr, outImageInfo, false);
#endif
    if (!outImage) {
        LOGE("GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur fail to make gradient blur image");
        return image;
    }
    return outImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GELinearGradientBlurShaderFilter::MakeMaskLinearGradientBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
    std::shared_ptr<Drawing::ShaderEffect> gradientShader)
{
    if (maskBlurShaderEffect_ == nullptr) {
        static const char* prog = R"(
            uniform shader srcImageShader;
            uniform shader blurImageShader;
            uniform shader gradientShader;

            half4 main(float2 coord)
            {
                vec3 srcColor = srcImageShader.eval(coord).rgb;
                vec3 blurColor = blurImageShader.eval(coord).rgb;
                float gradient = gradientShader.eval(coord).a;

                vec3 color = blurColor * gradient + srcColor * (1.0 - gradient);
                return vec4(color, 1.0);
            }
        )";
        maskBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (maskBlurShaderEffect_ == nullptr) {
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(maskBlurShaderEffect_);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetChild("blurImageShader", blurImageShader);
    builder->SetChild("gradientShader", gradientShader);
    return builder;
}

std::string GELinearGradientBlurShaderFilter::GetDescription()
{
    return "GELinearGradientBlurShaderFilter";
}

std::string GELinearGradientBlurShaderFilter::GetDetailedDescription()
{
    if (!linearGradientBlurPara_) {
        return "GELinearGradientBlurShaderFilterBlur, radius: unavailable";
    }
    return "GELinearGradientBlurShaderFilterBlur, radius: " + std::to_string(linearGradientBlurPara_->blurRadius_);
}
} // namespace Rosen
} // namespace OHOS
