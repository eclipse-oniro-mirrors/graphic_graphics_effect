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

#ifndef GRAPHICS_EFFECT_GE_WAVE_DISTURBANCE_H
#define GRAPHICS_EFFECT_GE_WAVE_DISTURBANCE_H

#include "draw/canvas.h"
#include "ge_filter_type_info.h"
#include "ge_shader_mask.h"
#include "image/image.h"
#include "ge_shader_filter_params.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GEWaveDisturbanceShaderMask : public GEShaderMask {
public:
    GEWaveDisturbanceShaderMask(GEWaveDisturbanceShaderMaskParams& param) : params_(param) {};
    GEWaveDisturbanceShaderMask(const GEWaveDisturbanceShaderMask&) = delete;
    GEWaveDisturbanceShaderMask& operator=(const GEWaveDisturbanceShaderMask&) = delete;
    virtual ~GEWaveDisturbanceShaderMask() = default;
    
    DECLARE_GEFILTER_TYPEFUNC(GEWaveDisturbanceShaderMask, GEWaveDisturbanceShaderMaskParams);

    // for this class no difference here
    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

private:
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetWaveDisturbanceBuilder() const;
    GEWaveDisturbanceShaderMaskParams params_;
};
} // Drawing
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_WAVE_Disturbance_MASK_H