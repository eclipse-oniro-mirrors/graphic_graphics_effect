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

#ifndef GRAPHICS_EFFECT_GE_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_SHADER_FILTER_H

#include <any>
#include <optional>
#include <variant>

#include "draw/canvas.h"
#include "ge_filter_type.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
class GEShaderFilter : public Drawing::IGEFilterType {
public:
    GEShaderFilter() = default;
    GEShaderFilter(const GEShaderFilter&) = delete;
    virtual ~GEShaderFilter() = default;

    virtual std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) = 0;

    /**
     * @brief Directly draw on canvas
     * @return Whether the image is successfully drawn onto the canvas.
     *         The default implementation returns false, indicating that this function is unimplemented.
     * @note Any GEShaderFilter supporting directly drawing on canvas should override this function and provides a
     *       correct implementation. It will be executed when any GEShaderFilter is generated from the last
     *       GEVisualEffect in GEVisualEffectContainer. When returning false, the canvas should be unchanged
     *       and GERender will automatically fall-back to OnProcessImage.
     */
    virtual bool OnDrawImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::Brush& brush)
    {
        return false; // Placeholder default implementation (Does not support directly draw on canvas)
    }

    GE_EXPORT std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst);

    GE_EXPORT bool DrawImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::Brush& brush);

    uint32_t Hash() const
    {
        return hash_;
    }

    void SetShaderFilterCanvasinfo(const Drawing::CanvasInfo& canvasInfo)
    {
        canvasInfo_ = canvasInfo;
    }

    void SetSupportHeadroom(float supportHeadroom)
    {
        supportHeadroom_ = supportHeadroom;
    }

    virtual void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& src, const Drawing::Rect& dst) {}

    void SetCache(std::shared_ptr<std::any> cacheData)
    {
        cacheAnyPtr_ = cacheData;
    }

    std::shared_ptr<std::any> GetCache() const
    {
        return cacheAnyPtr_;
    }

protected:
    /**
     * @brief Create a matrix that translates to the top-left corner of destination area.
     *        Used in OnDrawImage.
     * @param dst destination area
     * @return Translation matrix. Call the PostConcat method on your matrix with it before calling MakeShader
     */
    static Drawing::Matrix CreateDestinationTranslateMatrix(const Drawing::Rect& dst)
    {
        Drawing::Matrix translateMatrix;
        translateMatrix.Translate(dst.GetLeft(), dst.GetTop());
        return translateMatrix;
    }

protected:
    Drawing::CanvasInfo canvasInfo_;
    float supportHeadroom_ = 0.0f;
    uint32_t hash_ = 0;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_H
