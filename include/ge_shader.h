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

#ifndef GRAPHICS_EFFECT_GE_SHADER_H
#define GRAPHICS_EFFECT_GE_SHADER_H

#include <any>
#include "draw/canvas.h"
#include "utils/rect.h"
#include "effect/shader_effect.h"
#include "ge_common.h"
#include "ge_filter_type.h"

namespace OHOS {
namespace Rosen {
class GE_EXPORT GEShader : public Drawing::IGEFilterType {
public:
    GEShader() = default;
    GEShader(const GEShader&) = delete;
    virtual ~GEShader() = default;

    virtual void MakeDrawingShader(const Drawing::Rect& rect, float progress) = 0;

    virtual const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() { return drShader_; }

    virtual void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect);

    uint32_t Hash() const { return hash_; }

    void SetCache(std::shared_ptr<std::any> cacheData)
    {
        cacheAnyPtr_ = cacheData;
    }

    std::shared_ptr<std::any> GetCache()
    {
        return cacheAnyPtr_;
    }

    void SetSupportHeadroom(float supportHeadroom)
    {
        supportHeadroom_ = supportHeadroom;
    }

protected:
    virtual void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) {}

protected:
    /**
     * @brief Get the rectangle area can be subtracted.
     * @details Specifies the inner rectangle (in normalized units) to be excluded from the rendering area.
     *          This is typically used to construct a nine-patch layout where the center region is subtracted to
     *          reduce the rendered area.
     * Nine-patch regions: 1 2 3
     *                     4 5 6
     *                     7 8 9
     * Where region 5 (the center) is subtracted, others preserved.
     * @note If the returned value is empty or invalid, no subtraction should be applied.
     */
    virtual Drawing::Rect GetSubtractedRect() const { return Drawing::Rect(); }
    uint32_t hash_ = 0;
    float supportHeadroom_ = 0.0f;
    std::shared_ptr<Drawing::ShaderEffect> drShader_ = nullptr;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;

private:
    bool TryDrawShaderWithPen(Drawing::Canvas& canvas, const Drawing::Rect& rect);
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_H
