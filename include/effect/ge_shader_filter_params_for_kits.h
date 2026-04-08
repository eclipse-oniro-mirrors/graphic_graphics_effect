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
#ifndef GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_KITS_H
#define GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_KITS_H

#include "utils/point.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct GEWaterGlassDataParams final {
    float speed = 0.38f;
    float distortSpeed = 0.2f;
    Drawing::Point refractionSpeed = {1.18f, 0.47f};
    float progress = 0.0f;
    Drawing::Point shakingDirection1 = {0.0f, 0.0f};
    Drawing::Point shakingDirection2 = {0.0f, 0.0f};
    Drawing::Point waveDensityXY = {4.8f, 14.6f};
    float waveStrength = 5.8f;
    float waveRefraction = 0.2f;
    float waveSpecular = 0.32f;
    float waveFrequency = 6.15f;
    float waveShapeDistortion = 0.54f;
    float waveDistortionAngle = 0.93f;
    float rippleXWave = 0.01f;
    float rippleYWave = 0.04f;
    float borderRadius = 0.1f;
    float borderThickness = 0.1f;
    Drawing::Point waveInnerMaskXY = {0.85f, 0.9f};
    float waveInnerMaskRadius = 0.59f;
    float waveInnerMaskSmoothness = 1.0f;
    float waveOuterMaskPadding = 0.04f;
    float waveSpecularPower = 50.0f;
    float refractionDetailDark = 0.85f;
    float refractionDetailWhite = 0.48f;
    float detailStrength = 0.61f;
};

struct GEReededGlassDataParams final {
    float refractionFactor = 0.3f;
    uint8_t horizontalPatternNumber = 20;
    float gridLightStrength = 0.1f;
    float gridLightPositionStart = 0.97f;
    float gridLightPositionEnd = 0.92f;
    float gridShadowStrength = 0.02f;
    float gridShadowPositionStart = 0.0f;
    float gridShadowPositionEnd = 0.7f;
    Drawing::Point portalLightSize = {0.2f, 0.01f};
    Drawing::Point portalLightTilt = {0.526f, 0.46f};
    Drawing::Point portalLightPosition = {0.55f, 0.7f};
    float portalLightDisperseAttenuation = 2.4f;
    float portalLightDisperse = 0.01f;
    float portalLightSmoothBorder = 0.03f;
    float portalLightShadowBorder = 0.06f;
    float portalLightShadowPositionShift = 0.0f;
    float portalLightStrength = 1.21f;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_KITS_H