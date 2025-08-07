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
#ifndef GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H
#define GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H

#include <memory>
#include <optional>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "draw/color.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEEdgeLightShaderFilter : public GEShaderFilter {
public:
    GEEdgeLightShaderFilter(const Drawing::GEEdgeLightShaderFilterParams& params);
    ~GEEdgeLightShaderFilter() override = default;

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string& Type() const override;

protected:
    float alpha_ = 1.0f;
    bool bloom_ = true;
    Vector4f color_ = {0.2f, 0.7f, 0.1f, 0.0f};
    std::shared_ptr<Drawing::GEShaderMask> mask_ = nullptr;
    bool useRawColor_ = false;

    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_EDGE_LIGHT_SHADER_FILTER_H
