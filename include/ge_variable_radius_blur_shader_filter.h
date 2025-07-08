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
#ifndef GRAPHICS_EFFECT_GE_VARIABLE_RADIUS_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_VARIABLE_RADIUS_BLUR_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEVariableRadiusBlurShaderFilter : public GEShaderFilter {
public:
    GEVariableRadiusBlurShaderFilter(const Drawing::GEVariableRadiusBlurShaderFilterParams& params);
    GEVariableRadiusBlurShaderFilter(const GEVariableRadiusBlurShaderFilter&) = delete;
    GEVariableRadiusBlurShaderFilter operator=(const GEVariableRadiusBlurShaderFilter&) = delete;
    ~GEVariableRadiusBlurShaderFilter() override = default;

    virtual std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

    std::string GetDescription();
    std::string GetDetailedDescription();


protected:
    Drawing::GEVariableRadiusBlurShaderFilterParams params_;

private:
    static void MakeHorizontalMeanBlurEffect();
    static void MakeVerticalMeanBlurEffect();
    static void MakeTextureShaderEffect();

    static std::shared_ptr<Drawing::Image> DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
        Drawing::Canvas& canvas, float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
        const Drawing::Rect& dst);
    static std::shared_ptr<Drawing::Image> BuildMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
        Drawing::Canvas& canvas, float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
        Drawing::Matrix blurMatrix);
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VARIABLE_RADIUS_BLUR_SHADER_FILTER_H
