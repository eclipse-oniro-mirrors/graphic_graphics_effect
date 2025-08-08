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
#ifndef GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H

#include <memory>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

#ifdef USE_M133_SKIA
#include "include/core/SkM44.h"
#endif

namespace OHOS {
namespace Rosen {

struct OffsetInfo;

REGISTER_GEFILTER_TYPEINFO(KAWASE_BLUR, GEKawaseBlurShaderFilter, Drawing::GEKawaseBlurShaderFilterParams);
class GEKawaseBlurShaderFilter : public GEShaderFilter {
public:
    GE_EXPORT GEKawaseBlurShaderFilter(const Drawing::GEKawaseBlurShaderFilterParams& params);
    ~GEKawaseBlurShaderFilter() override = default;
    GE_EXPORT int GetRadius() const;
    DECLARE_GEFILTER_TYPEFUNC(GEKawaseBlurShaderFilter);
    std::shared_ptr<Drawing::GEFilterParams> Params() const override;

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas &canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst) override;

    const std::string& Type() const override;

    // set noise factor
    void SetFactor(float factor);

private:
    static Drawing::Matrix GetShaderTransform(
        const Drawing::Canvas* canvas, const Drawing::Rect& blurRect, float scaleW = 1.0f, float scaleH = 1.0f);
    bool InitBlurEffect();
    bool InitMixEffect();
    GE_EXPORT bool InitSimpleFilter();

    bool InitBlurEffectForAdvancedFilter();
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& checkedImage, const Drawing::Rect& src) const;
    GE_EXPORT void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;

    GE_EXPORT std::shared_ptr<Drawing::Image> ScaleAndAddRandomColor(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& blurImage,
        const Drawing::Rect& src, const Drawing::Rect& dst, int& width, int& height) const;
    std::shared_ptr<Drawing::ShaderEffect> ApplySimpleFilter(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const std::shared_ptr<Drawing::ShaderEffect>& prevShader,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear) const;
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    GE_EXPORT std::string GetDescription() const;
    void SetBlurBuilderParam(Drawing::RuntimeShaderBuilder& blurBuilder, const float offsetXY,
        const Drawing::ImageInfo& scaledInfo, const int width, const int height);
    bool IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst);
    const OHOS::Rosen::Drawing::Matrix BuildMatrix(
        const Drawing::Rect &src, const Drawing::ImageInfo &scaledInfo, const std::shared_ptr<Drawing::Image> &input);

    static void GetNormalizedOffset(SkV2 *offsets, const uint32_t offsetCount, const OffsetInfo &offsetInfo);

    int radius_;
    float blurRadius_ = 0.0f;
    float blurScale_ = 0.25f;
    float factor_ = 1.75f; // 1.75 from experience
    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H
