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

#ifndef GRAPHICS_EFFECT_GE_HPS_EFFECT_FILTER_H
#define GRAPHICS_EFFECT_GE_HPS_EFFECT_FILTER_H

#include "draw/canvas.h"
#include "draw/brush.h"
#include "draw/pen.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

#include "ge_shader_filter.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "ge_visual_effect_impl.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
class HpsEffectFilter {
public:
    HpsEffectFilter() = default;
    HpsEffectFilter(Drawing::Canvas& canvas);
    ~HpsEffectFilter() = default;

    using IndexRange = std::pair<int32_t, int32_t>;
    struct HpsEffectContext {
        float alpha;
        std::shared_ptr<Drawing::ColorFilter> colorFilter;
        uint32_t maskColor;
    };

    bool HpsSupportEffectGE(const Drawing::GEVisualEffectContainer& veContainer);
    bool IsHpsEffectEnabled() const;
    bool IsEffectSupported(const std::shared_ptr<Drawing::GEVisualEffect>& vef);
    void GenerateVisualEffectFromGE(const std::shared_ptr<Drawing::GEVisualEffectImpl>& visualEffectImpl,
        const Drawing::Rect& src, const Drawing::Rect& dst, float saturationForHPS, float brightnessForHPS,
        const std::shared_ptr<Drawing::Image>& image);
    bool ApplyHpsEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& outImage, const HpsEffectContext& hpsContext);

private:
    std::vector<std::shared_ptr<Drawing::HpsEffectParameter>> hpsEffect_;
    bool isBlur_ {false};
    Drawing::Matrix upscale_matrix_;
    void GenerateMesaBlurEffect(const Drawing::GEMESABlurShaderFilterParams& params,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    void GenerateKawaseBlurEffect(const Drawing::GEKawaseBlurShaderFilterParams& params,
        const Drawing::Rect& src, const Drawing::Rect& dst, float saturationForHPS, float brightnessForHPS);
    void GenerateGreyEffect(const Drawing::GEGreyShaderFilterParams& params,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    void GenerateAIBarEffect(const Drawing::GEAIBarShaderFilterParams& params,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    void GenerateGradientBlurEffect(const Drawing::GELinearGradientBlurShaderFilterParams& params,
        const Drawing::Rect& src, const Drawing::Rect& dst, const std::shared_ptr<Drawing::Image>& image,
        Drawing::CanvasInfo info);
    bool ApplyHpsSmallCanvas(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& outImage, const HpsEffectContext& hpsContext);
    std::shared_ptr<Drawing::RuntimeEffect> GetUpscaleEffect() const;
    bool DrawImageWithHps(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& imageCache,
        std::shared_ptr<Drawing::Image>& outImage, const Drawing::Rect& dst, const HpsEffectContext& hpsContext);
};
}
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_HPS_EFFECT_FILTER_H