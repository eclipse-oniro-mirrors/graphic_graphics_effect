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

#ifndef GRAPHICS_EFFECT_GE_RENDER_H
#define GRAPHICS_EFFECT_GE_RENDER_H

#include <memory>

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_filter_composer_pass.h"
#include "ge_hps_effect_filter.h"
#include "ge_shader.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "image/image.h"

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GE_EXPORT GERender {
public:
    GERender();
    ~GERender();
    void DrawShaderEffect(
        Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer, const Drawing::Rect& bounds);

    void DrawImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling);
    /**
     * @brief Applies a sequence of visual effects to an image using the GE (Graphics Effect) pipeline.
     *
     * This function processes a chain of visual effects defined in the GEVisualEffectContainer
     * and applies them sequentially to an input image. Each effect is applied via GEShaderFilter,
     * with the output of one effect serving as the input to the next. It leverages
     * the GEShaderFilter mechanism to handle rendering, with proper cache management and
     * compatibility handling for legacy use cases.
     *
     * @note The function assumes the input image is valid and non-null. If any effect fails
     *       to generate a valid shader or filter, the process continues but may result in
     *       unexpected output.
     * @see ProcessShaderFilter for the underlying effect application mechanism.
     * @see GEVisualEffectContainer for the container of visual effects.
     * @see GEShaderFilter for the filtering pipeline.
     *
     * @param canvas Reference to the Drawing::Canvas used for rendering context.
     * @param veContainer Reference to the GEVisualEffectContainer containing the sequence
     *                    of visual effects to apply.
     * @param image Shared pointer to the input image to process.
     * @param src Source rectangle defining the region of the input image to process.
     * @param dst Destination rectangle defining where to draw the result.
     * @param sampling Sampling options for image scaling and filtering.
     *
     * @return Shared pointer to the resulting image after all effects have been applied.
     *         Returns nullptr if the input image is null or processing fails.
     */
    std::shared_ptr<Drawing::Image> ApplyImageEffect(Drawing::Canvas& canvas,
        Drawing::GEVisualEffectContainer& veContainer, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling);

    struct HpsGEImageEffectContext {
        std::shared_ptr<Drawing::Image> image {};
        Drawing::Rect src {};
        Drawing::Rect dst {};
        Drawing::SamplingOptions sampling {};
        float alpha {};
        std::shared_ptr<Drawing::ColorFilter> colorFilter {};
        uint32_t maskColor {};
        float saturationForHPS {};
        float brightnessForHPS {};
    };

    /**
     * @brief Applies a sequence of visual effects to an image using HPS pipeline only, with special
     * handling for HPS blur effects.
     *
     * This function is a legacy internal helper used primarily for unit testing (e.g., ApplyHpsImageEffect001).
     * In the future, the unit tests may be refactored and this function may be removed.
     *
     * @note This function is NOT intended for external use. All external clients should use
     *       ApplyHpsGEImageEffect instead.
     * @see ApplyHpsGEImageEffect for the recommended external API.
     * @see HpsEffectFilter for HPS-specific effect processing.
     *
     * @return true if HPS blur was successfully applied, false otherwise.
     *         Please notice that returning false *DOES NOT* imply image application failure (outImage == nullptr)
     */
    bool ApplyHpsImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const HpsGEImageEffectContext& context, std::shared_ptr<Drawing::Image>& outImage);

    // Return type of ApplyHpsGEImageEffect indicates whether image has drawn on canvas and hps blur is applied.
    struct ApplyHpsGEResult {
        bool hasDrawnOnCanvas;
        bool isHpsBlurApplied;
        static ApplyHpsGEResult CanvasNotDrawnAndHpsNotApplied()
        {
            return { false, false };
        }
    };

    /**
     * @brief Applies a sequence of visual effects to an image using HPS and GE pipeline, with special
     * handling for HPS blur effects.
     *
     * This function orchestrates the application of a series of visual effects (via GEVisualEffectContainer) to an
     * input image, leveraging a specialized rendering pipeline that includes HPS-specific passes for optimized
     * performance. It supports:
     * - HPS effects via GEHpsBuildPass.
     * - Direct drawing on canvas when allowed.
     * - Compatibility fallbacks for legacy behavior.
     *
     * The function processes each visual effect in sequence, using a filter composer to manage the rendering pipeline.
     * It returns whether the final output was drawn directly on the canvas and whether an HPS blur was successfully
     * applied.
     *
     * @see GEFilterComposer for the underlying pipeline composition mechanism.
     * @see GEHpsBuildPass for the HPS composition mechanism
     * @see ProcessShaderFilter for the GE effect application mechanism.
     *
     * @param canvas Reference to the Drawing::Canvas where rendering occurs.
     * @param veContainer Reference to the GEVisualEffectContainer containing the sequence of visual effects to apply.
     * @param context The HpsGEImageEffectContext containing image, source/destination rectangles, sampling options,
     *                alpha, color filter, mask color, and HPS-specific parameters (saturation/brightness).
     * @param outImage Reference to a shared pointer where the resulting image will be stored.
     * @param brush Reference to a Drawing::Brush used for direct drawing on the canvas when applicable.
     *
     * @return ApplyHpsGEResult indicating:
     *         - `hasDrawnOnCanvas`: True if the final result was drawn directly on the canvas.
     *         - `isHpsBlurApplied`: True if an HPS blur effect was successfully applied.
     */
    ApplyHpsGEResult ApplyHpsGEImageEffect(Drawing::Canvas& canvas, Drawing::GEVisualEffectContainer& veContainer,
        const HpsGEImageEffectContext& context, std::shared_ptr<Drawing::Image>& outImage, Drawing::Brush& brush);

    bool IsGasifyFilter();

    static void SetMesablurAllEnabledByCCM(bool flag);

private:
    struct ShaderFilterEffectContext {
        std::shared_ptr<Drawing::Image> image {};
        Drawing::Rect src {};
        Drawing::Rect dst {};
    };

    // Return type of ProcessShaderFilter() and DrawShaderFilter() indicates the applied target for visualEffect.
    enum class ApplyShaderFilterTarget { Error, DrawOnImage, DrawOnCanvas };

    /**
     * @brief Shader filter creation, set flags and prepare works before execute the shader filter.
     * This function generates a GEShaderFilter from visualEffect and handle the cache/process/draw pipeline.
     * Used as an internal helper of GEShaderFilter pipeline.
     * @see ProcessShaderFilter for drawing on the image.
     * @see DrawShaderFilter for drawing on the canvas.
     * @return True if the process is successful
     */
    bool BeforeExecuteShaderFilter(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::GEVisualEffect>& visualEffect, const ShaderFilterEffectContext& context,
        std::shared_ptr<GEShaderFilter>& outShaderFilter);

    /**
     * @brief Update information after executing the shader filter, including writing cache back.
     * Used as an internal helper of GEShaderFilter pipeline.
     * @see ProcessShaderFilter for drawing on the image.
     * @see DrawShaderFilter for drawing on the canvas.
     * @return True if the process is successful
     */
    bool AfterExecuteShaderFilter(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::GEVisualEffect>& visualEffect,
        const ShaderFilterEffectContext& context, const std::shared_ptr<GEShaderFilter>& geShaderFilter);

    // Internal helper for dispatching between ProcessShaderFilter and DrawShaderFilter.
    // Used in ApplyHpsGEImageEffect only.
    ApplyShaderFilterTarget DispatchGEShaderFilter(Drawing::Canvas& canvas, Drawing::Brush& brush,
        GEFilterComposable& composable, std::shared_ptr<Drawing::GEVisualEffect>& visualEffect,
        ShaderFilterEffectContext& geContext);

    /**
     * @brief Apply a GEVisualEffect on outImage through GEShaderFilter::ProcessImage
     * @return The applied target for visualEffect.
     * @retval `ApplyShaderFilterTarget::Error`: Failed to apply visualEffect due to invalid visualEffect.
     * @retval `ApplyShaderFilterTarget::DrawOnImage`: The output is drawn on `outImage`.
     */
    ApplyShaderFilterTarget ProcessShaderFilter(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::GEVisualEffect> visualEffect, std::shared_ptr<Drawing::Image>& outImage,
        const ShaderFilterEffectContext& context);

    /**
     * @brief Apply a GEVisualEffect on canvas through GEShaderFilter::DrawImage
     * @return The applied target for visualEffect.
     * @retval `ApplyShaderFilterTarget::Error`: Failed to apply visualEffect due to invalid visualEffect.
     * @retval `ApplyShaderFilterTarget::DrawOnCanvas`: The output is drawn on `canvas`. Used when direct drawing
     *          is applied for the last effect in a GEVisualEffect sequence (generally GEVisualEffectContainer).
     */
    ApplyShaderFilterTarget DrawShaderFilter(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::GEVisualEffect> visualEffect, Drawing::Brush& brush,
        const ShaderFilterEffectContext& context);

    std::shared_ptr<GEShader> GenerateShaderEffect(const std::shared_ptr<Drawing::GEVisualEffectImpl>& ve);

    std::shared_ptr<GEShaderFilter> GenerateShaderFilter(const std::shared_ptr<Drawing::GEVisualEffect>& ve);
    std::vector<std::shared_ptr<GEShaderFilter>> GenerateShaderFilters(
        const Drawing::GEVisualEffectContainer& veContainer);

    std::shared_ptr<GEShaderFilter> GenerateExtShaderFilter(const std::shared_ptr<Drawing::GEVisualEffectImpl>&);

    std::shared_ptr<GEShaderFilter> GenerateShaderKawaseBlur(const std::shared_ptr<Drawing::GEVisualEffectImpl>&);

    std::shared_ptr<GEShaderFilter> GenerateExtShaderFrostedGlass(const std::shared_ptr<Drawing::GEVisualEffectImpl>&);

    std::shared_ptr<GEShaderFilter> GenerateExtShaderFrostedGlassBlur(
        const std::shared_ptr<Drawing::GEVisualEffectImpl> &ve);
    static bool isMesablurAllEnable_;
    bool isGasifyFilter_ = false;
};

} // namespace GraphicsEffectEngine
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_RENDER_H
