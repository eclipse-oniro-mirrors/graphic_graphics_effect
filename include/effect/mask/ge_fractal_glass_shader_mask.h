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

#ifndef GRAPHICS_EFFECT_GE_FRACTAL_GLASS_SHADER_MASK_H
#define GRAPHICS_EFFECT_GE_FRACTAL_GLASS_SHADER_MASK_H

#include "common/rs_vector4.h"
#include "ge_filter_type_info.h"
#include "ge_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class GE_EXPORT GEFractalGlassShaderMask : public GEShaderMask {
public:
    GEFractalGlassShaderMask(const GEFractalGlassMaskParams& param);
    GEFractalGlassShaderMask(const GEFractalGlassShaderMask&) = delete;
    virtual ~GEFractalGlassShaderMask() = default;

    DECLARE_GEFILTER_TYPEFUNC(GEFractalGlassShaderMask, GEFractalGlassMaskParams);

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

private:
    bool IsValid() const;
    void SetCommonUniforms(std::shared_ptr<Drawing::RuntimeShaderBuilder> builder, float width, float height) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFractalGlassShaderMaskBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFractalGlassShaderNormalMaskBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFractalGlassShaderMaskBuilderNoTexture() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFractalGlassShaderNormalMaskBuilderNoTexture() const;
    GEFractalGlassMaskParams param_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FRACTAL_GLASS_SHADER_MASK_H