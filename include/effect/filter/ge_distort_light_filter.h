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

#ifndef GRAPHICS_EFFECT_GE_DISTORT_LIGHT_FILTER_H
#define GRAPHICS_EFFECT_GE_DISTORT_LIGHT_FILTER_H

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEDistortLightFilter : public GEShaderFilter {
public:
    GEDistortLightFilter(const Drawing::GEDistortLightFilterParams& params);
    GEDistortLightFilter(const GEDistortLightFilter&) = delete;
    GEDistortLightFilter& operator=(const GEDistortLightFilter&) = delete;
    GEDistortLightFilter(GEDistortLightFilter&&) = delete;
    GEDistortLightFilter& operator=(GEDistortLightFilter&&) = delete;
    ~GEDistortLightFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEDistortLightFilter, Drawing::GEDistortLightFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    // Build the runtime shader, bind children/uniforms and produce the output image.
    // Returns nullptr on any failure; callers fall back to the input image.
    std::shared_ptr<Drawing::Image> MakeDistortLightImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::ImageInfo& imageInfo,
        const std::shared_ptr<Drawing::RuntimeEffect> shader);

    std::shared_ptr<Drawing::GEShaderMask> distortMask_ = nullptr;
    float lightFactor_ = 0.5f;
    float highLightFactor_ = 1.0f;
    float highLightWidth_ = 0.2f;
    float highLightSoft_ = 0.1f;
    Drawing::Color4f highLightColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_DISTORT_LIGHT_FILTER_H
