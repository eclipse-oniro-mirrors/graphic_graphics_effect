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

#ifndef GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_MASK_H
#define GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_MASK_H

#include "ge_filter_type_info.h"
#include "ge_log.h"
#include "ge_shader_filter_params.h"
#include "ge_shader_mask.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GELinearGradientShaderMask : public GEShaderMask {
public:
    GELinearGradientShaderMask(const GELinearGradientShaderMaskParams& param);
    GELinearGradientShaderMask(const GELinearGradientShaderMask&) = delete;
    ~GELinearGradientShaderMask() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GELinearGradientShaderMask, GELinearGradientShaderMaskParams);

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

private:
    std::vector<std::pair<float, float>> fractionStops_;
    Drawing::Point startPos_;
    Drawing::Point endPos_;
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_MASK_H