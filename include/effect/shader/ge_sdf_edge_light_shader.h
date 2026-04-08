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

#ifndef GRAPHICS_EFFECT_SDF_EDGE_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_SDF_EDGE_LIGHT_SHADER_H

#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "effect/runtime_shader_builder.h"
#include "ge_shader_filter_params.h"
#include "ge_sdf_shader_shape.h"
#include "ge_shader_mask.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GESDFEdgeLightShader : public GEShader {
public:
    explicit GESDFEdgeLightShader(const Drawing::GESDFEdgeLightEffectParams& params);

    ~GESDFEdgeLightShader() override = default;

    DECLARE_GEFILTER_TYPEFUNC(GESDFEdgeLightShader, Drawing::GESDFEdgeLightEffectParams);

    void OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    const std::string GetDescription() const { return "GESDFEdgeLightShader"; }

private:
    GESDFEdgeLightShader(const GESDFEdgeLightShader&) = delete;
    GESDFEdgeLightShader(const GESDFEdgeLightShader&&) = delete;
    GESDFEdgeLightShader& operator=(const GESDFEdgeLightShader&) = delete;
    GESDFEdgeLightShader& operator=(const GESDFEdgeLightShader&&) = delete;

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetEffectShaderBuilder(const Drawing::Rect& rect);
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    Drawing::GESDFEdgeLightEffectParams params_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_SDF_EDGE_LIGHT_SHADER_H
