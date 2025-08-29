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
#ifndef GRAPHICS_EFFECT_GE_FILTER_PARAMS_REGISTERS_EXT_H
#define GRAPHICS_EFFECT_GE_FILTER_PARAMS_REGISTERS_EXT_H

#include "ge_shader_filter_params.h"

// This header registers known GEFilterParams without filter implementation inside this repo
// Known unregistered types due to non-existent params in ge_shader_filter_params.h:
// - COMPLEX_SHADER
// - DOT_MATRIX
// - FLOW_LIGHT_SWEEP

namespace OHOS {
namespace Rosen {
namespace Drawing {
REGISTER_GEFILTERPARAM_TYPEINFO(DISPERSION, GEDispersionShaderFilterParams);
REGISTER_GEFILTERPARAM_TYPEINFO(COLOR_GRADIENT_EFFECT, GEXColorGradientEffectParams);
REGISTER_GEFILTERPARAM_TYPEINFO(LIGHT_CAVE, GEXLightCaveShaderParams);
REGISTER_GEFILTERPARAM_TYPEINFO(AIBAR_GLOW, GEXAIBarGlowEffectParams);
REGISTER_GEFILTERPARAM_TYPEINFO(ROUNDED_RECT_FLOWLIGHT, GEXRoundedRectFlowlightEffectParams);
REGISTER_GEFILTERPARAM_TYPEINFO(GRADIENT_FLOW_COLORS, GEXGradientFlowColorsEffectParams);
}
}
}

#endif // GRAPHICS_EFFECT_GE_FILTER_PARAMS_REGISTERS_EXT_H