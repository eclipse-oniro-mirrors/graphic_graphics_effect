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

#ifndef GRAPHICS_EFFECT_GE_SPATIAL_GLASS_SHADER_EFFECT_H
#define GRAPHICS_EFFECT_GE_SPATIAL_GLASS_SHADER_EFFECT_H

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

#include "ge_common.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
namespace SpatialGlassEffect {
GE_EXPORT void InterpolateAdaptiveParams(Drawing::GESpatialGlassEffectParams& params);
}

class GESpatialGlassEffect : public GEShader {
public:
    GESpatialGlassEffect(const Drawing::GESpatialGlassEffectParams& params);
    ~GESpatialGlassEffect() override = default;
    GESpatialGlassEffect(const GESpatialGlassEffect&) = delete;
    GESpatialGlassEffect(const GESpatialGlassEffect&&) = delete;
    GESpatialGlassEffect& operator=(const GESpatialGlassEffect&) = delete;
    GESpatialGlassEffect& operator=(const GESpatialGlassEffect&&) = delete;

    DECLARE_GEFILTER_TYPEFUNC(GESpatialGlassEffect, Drawing::GESpatialGlassEffectParams);

    const std::string& GetDescription() const
    {
        static const std::string desc = "GESpatialGlassEffect";
        return desc;
    }

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeSpatialGlassShader(
        std::shared_ptr<Drawing::ShaderEffect> imageShader, const Drawing::Rect& rect);
    bool InitSpatialGlassEffect();
    Drawing::GESpatialGlassEffectParams spatialGlassEffectParams_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SPATIAL_GLASS_SHADER_EFFECT_H
