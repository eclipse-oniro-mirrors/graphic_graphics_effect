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
#ifndef GRAPHICS_EFFECT_GE_MOTION_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_MOTION_BLUR_SHADER_FILTER_H

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include <memory>
#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/rect.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {

struct MotionBlurCacheData {
    Drawing::Rect lastRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f);
    float radius = 0.0f;
    Vector2f anchor = Vector2f(0.5f, 0.5f);
    int32_t sampleCount = 8;
};

class GE_EXPORT GEMotionBlurShaderFilter : public GEShaderFilter {
public:
    GEMotionBlurShaderFilter(const Drawing::GEMotionBlurShaderFilterParams& params);
    GEMotionBlurShaderFilter(const GEMotionBlurShaderFilter&) = delete;
    GEMotionBlurShaderFilter operator=(const GEMotionBlurShaderFilter&) = delete;
    GEMotionBlurShaderFilter(GEMotionBlurShaderFilter&&) = delete;
    GEMotionBlurShaderFilter& operator=(GEMotionBlurShaderFilter&&) = delete;
    ~GEMotionBlurShaderFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEMotionBlurShaderFilter, Drawing::GEMotionBlurShaderFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> GetMotionBlurEffect();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeMotionBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader,
        const Vector2f& scaleAnchor, const Vector2f& scaleSize, const Vector2f& rectOffset) const;
    void CalculateRect(const Drawing::Rect& lastRect, const Drawing::Rect& curRect,
        Vector2f& rectOffset, Vector2f& scaleSize, Vector2f& scaleAnchorCoord) const;
    bool RectValid(const Drawing::Rect& rect1, const Drawing::Rect& rect2) const;

    bool ValidateInput(const std::shared_ptr<Drawing::Image>& image) const;
    Drawing::Rect CalculateCurrentRect(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image) const;
    std::shared_ptr<Drawing::Image> CreateBlurImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst, const Vector2f& rectOffset,
        const Vector2f& scaleSize, const Vector2f& scaleAnchorCoord) const;
    std::shared_ptr<Drawing::Image> CreateUpscaledImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& blurImage) const;
    void UpdateCache(const Drawing::Rect& rect);
    Drawing::Rect GetLastRectFromCache();

    float radius_ = 0.0f;
    Vector2f anchor_ = Vector2f(0.5f, 0.5f);
    int32_t sampleCount_ = 6;

    bool disableMotionBlur_ = false;

    static constexpr float FLOAT_SCALE_THRESHOLD = 1.1f;
    static constexpr float FLOAT_IMAGE_SCALE = 0.5f;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_MOTION_BLUR_SHADER_FILTER_H