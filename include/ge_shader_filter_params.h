/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
#define GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H

#include <memory>
#include <vector>
#include <utility>

#include "utils/matrix.h"
#include "ge_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr char GE_FILTER_AI_BAR[] = "AIBAR";
constexpr char GE_FILTER_AI_BAR_LOW[] = "AIBAR_LOW";
constexpr char GE_FILTER_AI_BAR_HIGH[] = "AIBAR_HIGH";
constexpr char GE_FILTER_AI_BAR_THRESHOLD[] = "AIBAR_THRESHOLD";
constexpr char GE_FILTER_AI_BAR_OPACITY[] = "AIBAR_OPACITY";
constexpr char GE_FILTER_AI_BAR_SATURATION[] = "AIBAR_SATURATION";
struct GEAIBarShaderFilterParams {
    float aiBarLow;
    float aiBarHigh;
    float aiBarThreshold;
    float aiBarOpacity;
    float aiBarSaturation;
};

constexpr char GE_FILTER_WATER_RIPPLE[] = "WATER_RIPPLE";
constexpr char GE_FILTER_WATER_RIPPLE_PROGRESS[] = "PROGRESS";
constexpr char GE_FILTER_WATER_RIPPLE_WAVE_NUM[] = "WAVE_NUM";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X[] = "RIPPLE_CENTER_X";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y[] = "RIPPLE_CENTER_Y";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_MODE[] = "RIPPLE_MODE";
struct GEWaterRippleFilterParams {
    float progress = 0.0f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.5f;
    float rippleCenterY = 0.7f;
    uint32_t rippleMode = 1;
};

constexpr char GE_FILTER_SOUND_WAVE[] = "SOUND_WAVE";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_A[] = "COLORA";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_B[] = "COLORB";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_C[] = "COLORC";
constexpr char GE_FILTER_SOUND_WAVE_COLORPROGRESS[] = "COLORPROGRESS";
constexpr char GE_FILTER_SOUND_WAVE_CENTERBRIGHTNESS[] = "CENTERBRIGHTNESS";
constexpr char GE_FILTER_SOUND_WAVE_SOUNDINTENSITY[] = "SOUNDINTENSITY";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A[] = "SHOCKWAVEALPHAA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B[] = "SHOCKWAVEALPHAB";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A[] = "SHOCKWAVEPROGRESSA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B[] = "SHOCKWAVEPROGRESSB";
struct GESoundWaveFilterParams {
    // sound wave
    uint32_t colorA = 0xFFFFFFFF;
    uint32_t colorB = 0xFFFFFFFF;
    uint32_t colorC = 0xFFFFFFFF;
    float colorProgress = 0.0f;
    float centerBrightness = 1.0f;
    float soundIntensity = 0.0f;

    // shock wave
    float shockWaveAlphaA = 1.0f;
    float shockWaveAlphaB = 1.0f;
    float shockWaveProgressA = 0.0f;
    float shockWaveProgressB = 0.0f;
};

constexpr char GE_FILTER_GREY[] = "GREY";
constexpr char GE_FILTER_GREY_COEF_1[] = "GREY_COEF_1";
constexpr char GE_FILTER_GREY_COEF_2[] = "GREY_COEF_2";
struct GEGreyShaderFilterParams {
    float greyCoef1;
    float greyCoef2;
};

constexpr char GE_FILTER_KAWASE_BLUR[] = "KAWASE_BLUR";
constexpr char GE_FILTER_KAWASE_BLUR_RADIUS[] = "KAWASE_BLUR_RADIUS";
struct GEKawaseBlurShaderFilterParams {
    int radius;
};

constexpr char GE_FILTER_MESA_BLUR[] = "MESA_BLUR";
constexpr char GE_FILTER_MESA_BLUR_RADIUS[] = "MESA_BLUR_RADIUS";
constexpr char GE_FILTER_MESA_BLUR_GREY_COEF_1[] = "MESA_BLUR_GREY_COEF_1";
constexpr char GE_FILTER_MESA_BLUR_GREY_COEF_2[] = "MESA_BLUR_GREY_COEF_2";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X[] = "OFFSET_X";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y[] = "OFFSET_Y";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z[] = "OFFSET_Z";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W[] = "OFFSET_W";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE[] = "TILE_MODE";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_WIDTH[] = "WIDTH";
constexpr char GE_FILTER_MESA_BLUR_STRETCH_HEIGHT[] = "HEIGHT";
struct GEMESABlurShaderFilterParams {
    int radius;
    float greyCoef1;
    float greyCoef2;
    float offsetX;
    float offsetY;
    float offsetZ;
    float offsetW;
    int tileMode;
    float width;
    float height;
};

constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR[] = "LINEAR_GRADIENT_BLUR";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION[] = "DIRECTION";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN[] = "ISOFFSCREEN";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT[] = "CANVASMAT";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS[] = "FRACTIONSTOPS";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS[] = "BLURRADIUS";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH[] = "GEOWIDTH";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT[] = "GEOHEIGHT";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X[] = "TRANX";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y[] = "TRANY";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_IS_RADIUS_GRADIENT[] = "ISRADIUSGRADIENT";
struct GELinearGradientBlurShaderFilterParams {
    float blurRadius;
    std::vector<std::pair<float, float>> fractionStops;
    int direction;
    float geoWidth;
    float geoHeight;
    Drawing::Matrix mat;
    float tranX;
    float tranY;
    bool isOffscreenCanvas;
    bool isRadiusGradient;
};

constexpr char GE_FILTER_MAGNIFIER[] = "MAGNIFIER";
constexpr char GE_FILTER_MAGNIFIER_FACTOR[] = "FACTOR";
constexpr char GE_FILTER_MAGNIFIER_WIDTH[] = "WIDTH";
constexpr char GE_FILTER_MAGNIFIER_HEIGHT[] = "HEIGHT";
constexpr char GE_FILTER_MAGNIFIER_CORNER_RADIUS[] = "CORNERRADIUS";
constexpr char GE_FILTER_MAGNIFIER_BORDER_WIDTH[] = "BORDERWIDTH";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X[] = "SHADOWOFFSETX";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y[] = "SHADOWOFFSETY";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_SIZE[] = "SHADOWSIZE";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_STRENGTH[] = "SHADOWSTRENGTH";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_1[] = "GRADIENTMASKCOLOR1";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_2[] = "GRADIENTMASKCOLOR2";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_1[] = "OUTERCONTOURCOLOR1";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_2[] = "OUTERCONTOURCOLOR2";
constexpr char GE_FILTER_MAGNIFIER_ROTATE_DEGREE[] = "ROTATEDEGREE";
struct GEMagnifierShaderFilterParams {
    float factor = 0.f;
    float width = 0.f;
    float height = 0.f;
    float cornerRadius = 0.f;
    float borderWidth = 0.f;

    float shadowOffsetX = 0.f;
    float shadowOffsetY = 0.f;
    float shadowSize = 0.f;
    float shadowStrength = 0.f;

    // rgba
    uint32_t gradientMaskColor1 = 0x00000000;
    uint32_t gradientMaskColor2 = 0x00000000;
    uint32_t outerContourColor1 = 0x00000000;
    uint32_t outerContourColor2 = 0x00000000;

    int32_t rotateDegree = 0;
};

constexpr char GE_MASK_RIPPLE[] = "MASK_RIPPLE";
constexpr char GE_MASK_RIPPLE_CENTER[] = "MASK_RIPPLE_CENTER";
constexpr char GE_MASK_RIPPLE_RADIUS[] = "MASK_RIPPLE_RADIUS";
constexpr char GE_MASK_RIPPLE_WIDTH[] = "MASK_RIPPLE_WIDTH";
constexpr char GE_MASK_RIPPLE_WIDTH_CENTER_OFFSET[] = "MASK_RIPPLE_WIDTH_CENTER_OFFSET";
struct GERippleShaderMaskParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radius_ = 0.f;
    float width_ = 0.f;
    float widthCenterOffset_ = 0.0f;
};

constexpr char GE_FILTER_DISPLACEMENT_DISTORT[] = "DISPLACEMENT_DISTORT";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_FACTOR[] = "DISTORT_FACTOR";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_MASK[] = "DISTORT_MASK";
struct GEDisplacementDistortFilterParams {
    std::pair<float, float> factor_ = {1.0f, 1.0f};
    std::shared_ptr<GEShaderMask> mask_;
};

constexpr char GE_FILTER_COLOR_GRADIENT[] = "COLOR_GRADIENT";
constexpr char GE_FILTER_COLOR_GRADIENT_COLOR[] = "COLOR";
constexpr char GE_FILTER_COLOR_GRADIENT_POSITION[] = "POSITION";
constexpr char GE_FILTER_COLOR_GRADIENT_STRENGTH[] = "STRENGTH";
constexpr char GE_FILTER_COLOR_GRADIENT_MASK[] = "MASK";
struct GEColorGradientShaderFilterParams {
    std::vector<float> colors;
    std::vector<float> positions;
    std::vector<float> strengths;
    std::shared_ptr<GEShaderMask> mask = nullptr;
};

constexpr char GE_FILTER_EDGE_LIGHT[] = "EDGE_LIGHT";
constexpr char GE_FILTER_EDGE_LIGHT_ALPHA[] = "EDGE_LIGHT_ALPHA";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R[] = "EDGE_LIGHT_EDGE_COLOR_R";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G[] = "EDGE_LIGHT_EDGE_COLOR_G";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B[] = "EDGE_LIGHT_EDGE_COLOR_B";
constexpr char GE_FILTER_EDGE_LIGHT_MASK[] = "EDGE_LIGHT_MASK";
constexpr char GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR[] = "EDGE_LIGHT_USE_RAW_COLOR";
struct GEEdgeLightShaderFilterParams {
    float alpha = 1.0f;
    float edgeColorR = 0.2f;
    float edgeColorG = 0.7f;
    float edgeColorB = 0.1f;
    std::shared_ptr<GEShaderMask> mask = nullptr;
    bool useRawColor = false;
};

constexpr char GE_FILTER_BEZIER_WARP[] = "BEZIER_WARP";
constexpr char GE_FILTER_BEZIER_WARP_DESTINATION_PATCH[] = "BEZIER_WARP_DESTINATION_PATCH";
constexpr size_t GE_FILTER_BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch
struct GEBezierWarpShaderFilterParams {
    std::array<Drawing::Point, GE_FILTER_BEZIER_WARP_POINT_NUM> destinationPatch;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
