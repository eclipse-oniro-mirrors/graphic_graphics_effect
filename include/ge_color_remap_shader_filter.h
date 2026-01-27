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

#ifndef GRAPHICS_EFFECT_GE_COLOR_RAMAP_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_COLOR_RAMAP_SHADER_FILTER_H

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEColorRemapShaderFilter  : public GEShaderFilter {
public:
    GEColorRemapShaderFilter(const Drawing::GEColorRemapFilterParams& params);
    GEColorRemapShaderFilter(const GEColorRemapShaderFilter&) = delete;
    GEColorRemapShaderFilter operator=(const GEColorRemapShaderFilter&) = delete;
    ~GEColorRemapShaderFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEColorRemapShaderFilter, Drawing::GEColorRemapFilterParams);
    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string GetDescription() const {
        std::string desc = "GEColorRemapShaderFilter";
        desc.append(" colors_:").append(std::to_string(params_.colors.size()));
        desc.append(" positions_:").append(std::to_string(params_.positions.size()));
        return desc;
    }

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeColorRemapBuilder();
    bool CheckInColorRemapParams(float* color, float* position);

    Drawing::GEColorRemapFilterParams params_;
    Vector4f tailColor_ = Vector4f(0.0, 0.0, 0.0, 0.0);
    float tailPosition_ = 0.0;
    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_COLOR_RAMAP_SHADER_FILTER_H