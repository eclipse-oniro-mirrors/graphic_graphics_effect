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
#ifndef GRAPHICS_EFFECT_TONE_MAPPING_HELPER_H
#define GRAPHICS_EFFECT_TONE_MAPPING_HELPER_H

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "draw/color.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

constexpr float EFFECT_MAX_LUMINANCE = 2.0f;

class GEToneMappingHelper {
public:
    static Vector4f GetBrightnessMapping(float headroom, Vector4f& input);

    static float GetBrightnessMapping(float headroom, float input);

    static Drawing::Color4f GetBrightnessMapping(float brightness, Drawing::Color4f& input);

    static inline bool NeedToneMapping(float supportHeadroom)
    {
        return ROSEN_GNE(supportHeadroom, 0.0f) && ROSEN_LNE(supportHeadroom, EFFECT_MAX_LUMINANCE);
    }

private:
    static bool CalcBezierResultY(
        const Vector2f& start, const Vector2f& end, const Vector2f& control, float x, float& y);
};
} // Rosen
} // OHOS
#endif // GRAPHICS_EFFECT_TONE_MAPPING_HELPER_H