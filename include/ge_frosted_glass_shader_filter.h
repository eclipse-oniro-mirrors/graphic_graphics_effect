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
#ifndef GRAPHICS_EFFECT_GE_FROSTED_GLASS_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_FROSTED_GLASS_SHADER_FILTER_H

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class GEMESABlurShaderFilter;
class GEFrostedGlassShaderFilter : public GEShaderFilter {
public:
    GEFrostedGlassShaderFilter(const Drawing::GEFrostedGlassShaderFilterParams& params);
    ~GEFrostedGlassShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEFrostedGlassShaderFilter, Drawing::GEFrostedGlassShaderFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    bool InitFrostedGlassEffect();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeFrostedGlassShader(
        std::shared_ptr<Drawing::ShaderEffect> imageShader, std::shared_ptr<Drawing::ShaderEffect> largeRBlurShader,
        std::shared_ptr<Drawing::ShaderEffect> smallRBlurShader, float imageWidth, float imageHeight);

private:
    std::shared_ptr<Drawing::ShaderEffect> CreateLargeRadiusBlurShader(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::Matrix& invertMatrix);
    std::shared_ptr<Drawing::ShaderEffect> CreateSmallRadiusBlurShader(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::Matrix& invertMatrix);
    std::shared_ptr<Drawing::Image> MakeSmallRadiusBlurImg(Drawing::Canvas& canvas, const Drawing::Rect& src,
        const Drawing::Rect& dst, std::shared_ptr<Drawing::Image> image);
    std::shared_ptr<Drawing::Image> MakeLargeRadiusBlurImg(Drawing::Canvas& canvas, const Drawing::Rect& src,
        const Drawing::Rect& dst, std::shared_ptr<Drawing::Image> image);
    Drawing::GEFrostedGlassShaderFilterParams frostedGlassParams_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FROSTED_GLASS_SHADER_FILTER_H