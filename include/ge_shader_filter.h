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
#include "image/image.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
class GEShaderFilter {
public:
    using FilterParams = std::optional<std::variant<Drawing::GEAIBarShaderFilterParams,
        Drawing::GEWaterRippleFilterParams, Drawing::GEGreyShaderFilterParams, Drawing::GEKawaseBlurShaderFilterParams,
        Drawing::GEMESABlurShaderFilterParams, Drawing::GELinearGradientBlurShaderFilterParams,
        Drawing::GEMagnifierShaderFilterParams>>;

    GEShaderFilter() = default;
    GEShaderFilter(const GEShaderFilter&) = delete;
    virtual ~GEShaderFilter() = default;

    virtual std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) = 0;

    uint32_t Hash() const
    {
        return hash_;
    }

    std::string Type() const
    {
        return type_;
    }
 
    FilterParams& Params()
    {
        return params_;
    }

    void SetShaderFilterCanvasinfo(const Drawing::CanvasInfo& canvasInfo)
    {
        canvasInfo_ = canvasInfo;
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
    Drawing::CanvasInfo canvasInfo_;
    uint32_t hash_ = 0;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;
    std::string type_ = "";
    FilterParams params_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_H
