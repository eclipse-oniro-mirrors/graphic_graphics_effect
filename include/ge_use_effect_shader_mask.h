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
 
#ifndef GRAPHICS_EFFECT_GE_USE_EFFECT_SHADER_MASK_H
#define GRAPHICS_EFFECT_GE_USE_EFFECT_SHADER_MASK_H
 
#include "common/rs_vector4.h"
#include "ge_log.h"
#include "ge_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class GE_EXPORT GEUseEffectShaderMask : public GEShaderMask {
public:
    GEUseEffectShaderMask(const GEUseEffectMaskParams& param) : param_(param) {}
    GEUseEffectShaderMask(const GEUseEffectShaderMask&) = delete;
    virtual ~GEUseEffectShaderMask() = default;
 
    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    std::weak_ptr<Drawing::Image> GetImage() const override
    {
        if (!param_.useEffect) {
            LOGE("GEUseEffectShaderMask::GetImage failed.");
            return {};
        }
        return param_.image;
    }

    bool GetUseEffect() const override
    {
        return param_.useEffect;
    }

private:
    bool IsValid() const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetUseEffectShaderMaskBuilder() const;
    GEUseEffectMaskParams param_;
};
} // Drawing
} // namespace Rosen
} // namespace OHOS
 
#endif
