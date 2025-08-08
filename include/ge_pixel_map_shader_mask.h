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

#ifndef GRAPHICS_EFFECT_GE_PIXEL_MAP_MASK_H
#define GRAPHICS_EFFECT_GE_PIXEL_MAP_MASK_H

#include "common/rs_vector4.h"
#include "ge_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
REGISTER_GEFILTER_TYPEINFO(PIXEL_MAP_MASK, GEPixelMapShaderMask, GEPixelMapMaskParams);
class GE_EXPORT GEPixelMapShaderMask : public GEShaderMask {
public:
    GEPixelMapShaderMask(const GEPixelMapMaskParams& param);
    GEPixelMapShaderMask(const GEPixelMapShaderMask&) = delete;
    virtual ~GEPixelMapShaderMask() = default;

    DECLARE_GEFILTER_TYPEFUNC(GEPixelMapShaderMask);

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

private:
    bool IsValid() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPixelMapShaderMaskBuilder() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPixelMapShaderNormalMaskBuilder() const;
    GEPixelMapMaskParams param_;
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_PIXEL_MAP_MASK_H