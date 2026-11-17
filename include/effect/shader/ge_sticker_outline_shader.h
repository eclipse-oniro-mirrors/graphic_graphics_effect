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
#ifndef GRAPHICS_EFFECT_GE_STICKER_OUTLINE_SHADER_H
#define GRAPHICS_EFFECT_GE_STICKER_OUTLINE_SHADER_H

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEStickerOutlineShader : public GEShader {
public:
    explicit GEStickerOutlineShader(const Drawing::GEStickerOutlineShaderParams& params);
    GEStickerOutlineShader(const GEStickerOutlineShader&) = delete;
    GEStickerOutlineShader& operator=(const GEStickerOutlineShader&) = delete;
    GEStickerOutlineShader(GEStickerOutlineShader&&) = delete;
    GEStickerOutlineShader& operator=(GEStickerOutlineShader&&) = delete;
    ~GEStickerOutlineShader() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEStickerOutlineShader, Drawing::GEStickerOutlineShaderParams);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    // Static helpers for sticker SDF generation, reusable by other sticker effects
    GE_EXPORT static int CalSdfSpreadFactor(float requiredRangePx);
    GE_EXPORT static uint32_t CalStickerHash(const std::shared_ptr<Drawing::Image>& sticker, int spreadFactor);

    GE_EXPORT static std::shared_ptr<Drawing::Image> MakeStickerSDFImage(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& sticker, float requiredRangePx = 64.0f);

    GE_EXPORT static std::shared_ptr<Drawing::Image> MakeDecodedStickerSDFImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& sdfImage, const Drawing::ImageInfo& stickerInfo,
        float requiredRangePx = 64.0f);

    // Compose outline card shader for use by other sticker effects (e.g., halftone)
    GE_EXPORT static std::shared_ptr<Drawing::ShaderEffect> MakeOutlineCardShader(
        const std::shared_ptr<Drawing::Image>& srcImage, const std::shared_ptr<Drawing::Image>& decodedSdfImage,
        const Drawing::Rect& rect, float outlineWidth, const Vector2f& lightDir, const Vector4f& shadowColor,
        float shadowRadius, float shadowDistance);

    GE_EXPORT static std::shared_ptr<Drawing::ShaderEffect> MakeOutlineCardShaderImpl(
        const std::shared_ptr<Drawing::Image>& srcImage, const std::shared_ptr<Drawing::Image>& decodedSdfImage,
        const Drawing::Rect& rect, float outlineWidth, const Vector2f& lightDir, const Vector4f& shadowColor,
        float shadowRadius, float shadowDistance,
        const std::shared_ptr<Drawing::ShaderEffect>& bandColorShader, float bandInfluence);

    GE_EXPORT static std::shared_ptr<Drawing::Image> MakeCompositeImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::ShaderEffect>& cardShader,
        const std::shared_ptr<Drawing::Image>& contentImage, const Drawing::Rect& rect);

protected:
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    Drawing::GEStickerOutlineShaderParams params_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_STICKER_OUTLINE_SHADER_H
