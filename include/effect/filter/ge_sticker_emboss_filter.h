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
#ifndef GRAPHICS_EFFECT_GE_STICKER_EMBOSS_FILTER_H
#define GRAPHICS_EFFECT_GE_STICKER_EMBOSS_FILTER_H

#include <memory>

#include "draw/canvas.h"
#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "image/image.h"

#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEStickerEmbossFilter : public GEShaderFilter {
    friend class GEStickerEmbossFilterTest;

public:
    GEStickerEmbossFilter(const Drawing::GEStickerEmbossFilterParams& params);
    GEStickerEmbossFilter(const GEStickerEmbossFilter&) = delete;
    GEStickerEmbossFilter& operator=(const GEStickerEmbossFilter&) = delete;
    GEStickerEmbossFilter(GEStickerEmbossFilter&&) = delete;
    GEStickerEmbossFilter& operator=(GEStickerEmbossFilter&&) = delete;
    ~GEStickerEmbossFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEStickerEmbossFilter, Drawing::GEStickerEmbossFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    // Decode the sticker alpha mask into a native-resolution SDF image (cached)
    std::shared_ptr<Drawing::Image> MakeDecodedStickerSDFImage(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> sticker, float requiredRangePx);

    // Compose the emboss effect from the input image and the decoded sticker SDF image
    std::shared_ptr<Drawing::Image> MakeStickerEmbossImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const std::shared_ptr<Drawing::Image> decodedSdfImage);

    // Bind emboss shader uniforms from params_
    void SetEmbossShaderUniforms(Drawing::RuntimeShaderBuilder& builder, const std::shared_ptr<Drawing::Image> image);

    Drawing::GEStickerEmbossFilterParams params_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_STICKER_EMBOSS_FILTER_H
