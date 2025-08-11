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
#ifndef GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H

#include "ge_gradient_blur_para.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "ge_linear_gradient_shader_mask.h"
#include "ge_variable_radius_blur_shader_filter.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
class GELinearGradientBlurShaderFilter : public GEShaderFilter {
public:
    GE_EXPORT GELinearGradientBlurShaderFilter(const Drawing::GELinearGradientBlurShaderFilterParams& params);
    GELinearGradientBlurShaderFilter(const GELinearGradientBlurShaderFilter&) = delete;
    GELinearGradientBlurShaderFilter operator=(const GELinearGradientBlurShaderFilter&) = delete;
    ~GELinearGradientBlurShaderFilter() override = default;

    virtual GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

    GE_EXPORT std::string GetDescription();
    GE_EXPORT std::string GetDetailedDescription();
    void SetBoundsGeometry(float geoWidth, float geoHeight)
    {
        geoWidth_ = geoWidth;
        geoHeight_ = geoHeight;
    }

    const std::string& Type() const override;

protected:
    std::shared_ptr<GELinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    float imageScale_ = 1.f;
    float geoWidth_ = 0.f;
    float geoHeight_ = 0.f;
    float tranX_ = 0.f;
    float tranY_ = 0.f;
    Drawing::Matrix mat_;
    bool isOffscreenCanvas_ = true;

    void TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias);
    uint8_t CalcDirectionBias(const Drawing::Matrix& mat);
    bool ProcessGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction);  // 2 size of points
    bool GetGEGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction);  // 2 size of points
private:
    void ComputeScale(float width, float height, bool useMaskAlgorithm);
    static std::shared_ptr<Drawing::Image> DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
        Drawing::Canvas& canvas, std::shared_ptr<GEShaderFilter>& blurFilter,
        std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeMaskLinearGradientBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
        std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    GE_EXPORT std::shared_ptr<Drawing::Image> ProcessImageDDGR(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image, uint8_t directionBias);

    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
