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
#ifndef GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_H
#define GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_H
 
#include <parcel.h>
 
#include "gex_complex_shader_params.h"
#include "ge_shader.h"
#include "ge_common.h"
 
namespace OHOS {
namespace Rosen {
 
class GEXComplexShader : public GEShader {
public:
    GEXComplexShader() = default;
    ~GEXComplexShader() override = default;
 
    static GE_EXPORT std::shared_ptr<GEXComplexShader> CreateDynamicImpl(const GEXComplexShaderParams& param);
 
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override { }
};
 
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_H