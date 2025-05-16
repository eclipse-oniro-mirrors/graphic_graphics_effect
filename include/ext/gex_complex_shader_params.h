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
#ifndef GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_PARAMS_H
#define GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_PARAMS_H
 
#include <vector>

#include "ge_common.h"
#include "parcel.h"

namespace OHOS {
namespace Rosen {

enum class GexComplexShaderType: uint32_t {
    NONE = 0,
    COLOR_GRADIENT,
    RADIAL_GRADIENT,
    SCREEN_FLOWLIGHT,
    AI_BAR_HALO,
    UV_VOICE,
    UV_SCREEN,
    SOUND_WAVE,
    MAX
};

struct GE_EXPORT GEXComplexShaderParams {
    GexComplexShaderType type_;
    std::vector<float> params_;
 
    bool Marshalling(Parcel& parcel);
    
    bool Unmarshalling(Parcel& parcel);
};

} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_EXT_COMPLEX_SHADER_PARAMS_H