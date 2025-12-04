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

#ifndef GRAPHICS_EFFECT_GE_IMAGE_SHADER_MASK_H
#define GRAPHICS_EFFECT_GE_IMAGE_SHADER_MASK_H

#include "common/rs_vector4.h"
#include "ge_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class GE_EXPORT GEImageShaderMask : public GEShaderMask {
public:
    GEImageShaderMask(const GEImageMaskParams& param) : param_(param) {}
    GEImageShaderMask(const GEImageShaderMask&) = delete;
    virtual ~GEImageShaderMask() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    std::weak_ptr<Drawing::Image> GetImage() const override
    {
        return param_.image;
    }

private:
    bool IsValid() const;
    GEImageMaskParams param_;
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif