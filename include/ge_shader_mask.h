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

#ifndef GRAPHICS_EFFECT_GE_SHADER_MASK_H
#define GRAPHICS_EFFECT_GE_SHADER_MASK_H
 
#include "draw/canvas.h"
#include "ge_common.h"
#include "ge_filter_type.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
 
namespace OHOS {
namespace Rosen {
namespace Drawing {
class GE_EXPORT GEShaderMask : public Drawing::IGEFilterType {
public:
    GEShaderMask() = default;
    GEShaderMask(const GEShaderMask&) = delete;
    virtual ~GEShaderMask() = default;
    virtual std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const = 0;
    virtual std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const = 0;
    virtual bool IsSDFShaderMask() const { return false; }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SHADER_MASK_H