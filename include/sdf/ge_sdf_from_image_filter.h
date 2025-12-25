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

#ifndef GRAPHICS_EFFECT_GE_SDF_FROM_IMAGE_FILTER_H
#define GRAPHICS_EFFECT_GE_SDF_FROM_IMAGE_FILTER_H

#include <memory>

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class GESDFFromImageFilter : public GEShaderFilter {
public:
    GE_EXPORT GESDFFromImageFilter(const Drawing::GESDFFromImageFilterParams& params);
    ~GESDFFromImageFilter() override = default;

    GE_EXPORT int GetSpreadFactor() const;

    DECLARE_GEFILTER_TYPEFUNC(GESDFFromImageFilter, Drawing::GESDFFromImageFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    int spreadFactor_ = 64;
    bool generateDerivs_ = true;

    bool InitJFAPrepareEffect();
    bool InitJfaIterationEffect();
    bool InitJfaProcessResultEffect();
    bool InitFillDerivEffect();

    std::shared_ptr<Drawing::Image> RunJFAPrepareEffect(Drawing::Canvas& canvas, std::shared_ptr<Drawing::Image> image,
        const Drawing::SamplingOptions& samplingOptions, const Drawing::Rect& src, const Drawing::Rect& dst);

    std::shared_ptr<Drawing::Image> RunJfaIterationsEffect(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::Image> image, const Drawing::SamplingOptions& samplingOptions);

    std::shared_ptr<Drawing::Image> RunJfaProcessResultEffect(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::Image> image, const Drawing::SamplingOptions& samplingOptions);

    std::shared_ptr<Drawing::Image> RunFillDerivEffect(Drawing::Canvas& canvas, std::shared_ptr<Drawing::Image> image,
        const Drawing::SamplingOptions& samplingOptions);

    bool IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst);

    Drawing::Matrix BuildStretchMatrix(
        const Drawing::Rect& src, const Drawing::Rect& dst, int inputWidth, int inputHeight) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_FROM_IMAGE_FILTER_H
