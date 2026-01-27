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

#ifndef GRAPHICS_EFFECT_GE_GAMMA_CORRECTION_FILTER_H
#define GRAPHICS_EFFECT_GE_GAMMA_CORRECTION_FILTER_H

#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEGammaCorrectionFilter  : public GEShaderFilter {
public:
    GEGammaCorrectionFilter(const float gamma);
    GEGammaCorrectionFilter(const GEGammaCorrectionFilter&) = delete;
    GEGammaCorrectionFilter operator=(const GEGammaCorrectionFilter&) = delete;
    ~GEGammaCorrectionFilter() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GEGammaCorrectionFilter, float);
    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    const std::string GetDescription() const {
        std::string desc = "GEGammaCorrectionFilter";
        desc.append(" gamma_:").append(std::to_string(gamma_));
        return desc;
    }

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeGammaCorrectionBuilder();

    float gamma_ = 1.0;
    static const std::string type_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GEX_GAMMA_CORRECTION_SHADER_FILTER_H