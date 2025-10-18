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
#include <array>
#include <utility>
#include <optional>
#include <variant>
#include <type_traits>

// rs
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
// drawing
#include "utils/matrix.h"
// ge
#include "ge_filter_type.h"
#include "ge_shader_mask.h"
#include "sdf/ge_sdf_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

template<typename T>
struct GEFilterParamsTypeInfo {
    static constexpr GEFilterType ID = GEFilterType::NONE;
    static constexpr std::string_view FilterName = "";
};

// Register type info for GE Filter Param type without Filter type (Non-instructive)
#define REGISTER_GEFILTERPARAM_TYPEINFO(ENUM_VALUE, PARAM_TYPE, FILTER_TYPE_NAME)            \
    template<>                                                                               \
    struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<PARAM_TYPE> {                      \
        using Self = PARAM_TYPE;                                                             \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        static constexpr std::string_view FilterName = FILTER_TYPE_NAME;                     \
    }

struct CanvasInfo {
    float geoWidth = 0.0f;
    float geoHeight = 0.0f;
    float tranX = 0.0f;
    float tranY = 0.0f;
    Drawing::Matrix mat;
};

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
REGISTER_GEFILTERPARAM_TYPEINFO(AIBAR, GEAIBarShaderFilterParams, GE_FILTER_AI_BAR);

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
REGISTER_GEFILTERPARAM_TYPEINFO(WATER_RIPPLE, GEWaterRippleFilterParams, GE_FILTER_WATER_RIPPLE);

constexpr char GE_FILTER_SOUND_WAVE[] = "SoundWave";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_A[] = "SoundWave_ColorA";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_B[] = "SoundWave_ColorB";
constexpr char GE_FILTER_SOUND_WAVE_COLOR_C[] = "SoundWave_ColorC";
constexpr char GE_FILTER_SOUND_WAVE_COLORPROGRESS[] = "SoundWave_ColorProgress";
constexpr char GE_FILTER_SOUND_WAVE_SOUNDINTENSITY[] = "SoundWave_Intersity";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A[] = "SoundWave_AlphaA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B[] = "SoundWave_AlphaB";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A[] = "SoundWave_ProgressA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B[] = "SoundWave_ProgressB";
constexpr char GE_FILTER_SOUND_WAVE_TOTAL_ALPHA[] = "SoundWave_TotalAlpha";
struct GESoundWaveFilterParams {
    // sound wave
    Drawing::Color4f colorA = {1.0, 1.0, 1.0, 1.0};
    Drawing::Color4f colorB = {1.0, 1.0, 1.0, 1.0};
    Drawing::Color4f colorC = {1.0, 1.0, 1.0, 1.0};
    float colorProgress = 0.0f;
    float soundIntensity = 0.0f;

    // shock wave
    float shockWaveAlphaA = 1.0f;
    float shockWaveAlphaB = 1.0f;
    float shockWaveProgressA = 0.0f;
    float shockWaveProgressB = 0.0f;
    float shockWaveTotalAlpha = 1.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(SOUND_WAVE, GESoundWaveFilterParams, GE_FILTER_SOUND_WAVE);

constexpr char GE_FILTER_GREY[] = "GREY";
constexpr char GE_FILTER_GREY_COEF_1[] = "GREY_COEF_1";
constexpr char GE_FILTER_GREY_COEF_2[] = "GREY_COEF_2";
struct GEGreyShaderFilterParams {
    float greyCoef1;
    float greyCoef2;
};
REGISTER_GEFILTERPARAM_TYPEINFO(GREY, GEGreyShaderFilterParams, GE_FILTER_GREY);

constexpr char GE_FILTER_KAWASE_BLUR[] = "KAWASE_BLUR";
constexpr char GE_FILTER_KAWASE_BLUR_RADIUS[] = "KAWASE_BLUR_RADIUS";
struct GEKawaseBlurShaderFilterParams {
    int radius;
};
REGISTER_GEFILTERPARAM_TYPEINFO(KAWASE_BLUR, GEKawaseBlurShaderFilterParams, GE_FILTER_KAWASE_BLUR);

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
REGISTER_GEFILTERPARAM_TYPEINFO(MESA_BLUR, GEMESABlurShaderFilterParams, GE_FILTER_MESA_BLUR);

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
REGISTER_GEFILTERPARAM_TYPEINFO(LINEAR_GRADIENT_BLUR, GELinearGradientBlurShaderFilterParams,
                                GE_FILTER_LINEAR_GRADIENT_BLUR);

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
REGISTER_GEFILTERPARAM_TYPEINFO(MAGNIFIER, GEMagnifierShaderFilterParams, GE_FILTER_MAGNIFIER);

constexpr char GE_MASK_RIPPLE[] = "RippleMask";
constexpr char GE_MASK_RIPPLE_CENTER[] = "RippleMask_Center";
constexpr char GE_MASK_RIPPLE_RADIUS[] = "RippleMask_Radius";
constexpr char GE_MASK_RIPPLE_WIDTH[] = "RippleMask_Width";
constexpr char GE_MASK_RIPPLE_WIDTH_CENTER_OFFSET[] = "RippleMask_Offset";
struct GERippleShaderMaskParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radius_ = 0.f;
    float width_ = 0.f;
    float widthCenterOffset_ = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(RIPPLE_MASK, GERippleShaderMaskParams, GE_MASK_RIPPLE);

constexpr char GE_MASK_DOUBLE_RIPPLE[] = "DoubleRippleMask";
constexpr char GE_MASK_DOUBLE_RIPPLE_CENTER1[] = "DoubleRippleMask_Center1";
constexpr char GE_MASK_DOUBLE_RIPPLE_CENTER2[] = "DoubleRippleMask_Center2";
constexpr char GE_MASK_DOUBLE_RIPPLE_RADIUS[] = "DoubleRippleMask_Radius";
constexpr char GE_MASK_DOUBLE_RIPPLE_WIDTH[] = "DoubleRippleMask_Width";
constexpr char GE_MASK_DOUBLE_RIPPLE_TURBULENCE[] = "DoubleRippleMask_Turbulence";
constexpr char GE_MASK_DOUBLE_RIPPLE_HALOTHICKNESS[] = "DoubleRippleMask_HaloThickness";
struct GEDoubleRippleShaderMaskParams {
    std::pair<float, float> center1_ = {0.f, 0.f};
    std::pair<float, float> center2_ = {0.f, 0.f};
    float radius_ = 0.f;
    float width_ = 0.f;
    float turbulence_ = 0.0f;
    float haloThickness_ = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DOUBLE_RIPPLE_MASK, GEDoubleRippleShaderMaskParams, GE_MASK_DOUBLE_RIPPLE);

constexpr char GE_FILTER_DISPLACEMENT_DISTORT[] = "DispDistort";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_FACTOR[] = "DispDistort_Factor";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_MASK[] = "DispDistort_Mask";
struct GEDisplacementDistortFilterParams {
    std::pair<float, float> factor_ = {1.0f, 1.0f};
    std::shared_ptr<GEShaderMask> mask_;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DISPLACEMENT_DISTORT_FILTER, GEDisplacementDistortFilterParams,
                                GE_FILTER_DISPLACEMENT_DISTORT);

constexpr char GE_FILTER_COLOR_GRADIENT[] = "ColorGradient";
constexpr char GE_FILTER_COLOR_GRADIENT_COLOR[] = "ColorGradient_Colors";
constexpr char GE_FILTER_COLOR_GRADIENT_POSITION[] = "ColorGradient_Positions";
constexpr char GE_FILTER_COLOR_GRADIENT_STRENGTH[] = "ColorGradient_Strengths";
constexpr char GE_FILTER_COLOR_GRADIENT_MASK[] = "ColorGradient_Mask";
struct GEColorGradientShaderFilterParams {
    std::vector<float> colors;
    std::vector<float> positions;
    std::vector<float> strengths;
    std::shared_ptr<GEShaderMask> mask = nullptr;
};
REGISTER_GEFILTERPARAM_TYPEINFO(COLOR_GRADIENT, GEColorGradientShaderFilterParams, GE_FILTER_COLOR_GRADIENT);

constexpr char GE_SHADER_HARMONIUM_EFFECT[] = "HarmoniumEffect";
constexpr char GE_SHADER_HARMONIUM_EFFECT_MASK[] = "HarmoniumEffect_Mask";
constexpr char GE_SHADER_HARMONIUM_EFFECT_MASKCLOCK[] = "HarmoniumEffect_MaskClock";
constexpr char GE_SHADER_HARMONIUM_EFFECT_TINTCOLOR[] = "HarmoniumEffect_TintColor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_RIPPLEPOSITION[] = "HarmoniumEffect_RipplePosition";
constexpr char GE_SHADER_HARMONIUM_EFFECT_RIPPLEPROGRESS[] = "HarmoniumEffect_RippleProgress";
constexpr char GE_SHADER_HARMONIUM_EFFECT_DISTORTPROGRESS[] = "HarmoniumEffect_DistortProgress";
constexpr char GE_SHADER_HARMONIUM_EFFECT_DISTORTFACTOR[] = "HarmoniumEffect_DistortFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_REFLECTIONFACTOR[] = "HarmoniumEffect_ReflectionFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_REFRACTIONFACTOR[] = "HarmoniumEffect_RefractionFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_BLURIMAGE[] = "HarmoniumEffect_BlurImage";
constexpr char GE_SHADER_HARMONIUM_EFFECT_BLURLEFT[] = "HarmoniumEffect_BlurLeft";
constexpr char GE_SHADER_HARMONIUM_EFFECT_BLURTOP[] = "HarmoniumEffect_BlurTop";
constexpr char GE_SHADER_HARMONIUM_EFFECT_MATERIALFACTOR[] = "HarmoniumEffect_MaterialFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_CORNERRADIUS[] = "HarmoniumEffect_CornerRadius";

struct GEHarmoniumEffectShaderParams {
    std::shared_ptr<GEShaderMask> mask = nullptr;
    std::shared_ptr<GEShaderMask> maskClock = nullptr;
    Vector4f tintColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    Vector3f ripplePosition = Vector3f(0.0f, 0.0f, 0.0f);
    float rippleProgress = 0.f;
    float distortProgress = 0.f;
    float distortFactor = 0.f;
    float reflectionFactor = 0.f;
    float refractionFactor = 0.f;
    std::shared_ptr<Drawing::Image> blurImage = nullptr;
    float blurLeft = 0.f;
    float blurTop = 0.f;
    float materialFactor = 0.f;
    float cornerRadius = 0.f;
};

constexpr char GE_MASK_HARMONIUM_EFFECT[] = "HarmoniumEffectMask";
constexpr char GE_MASK_HARMONIUM_EFFECT_PIXEL_MAP[] = "HarmoniumEffectMask_Image";
struct GEHarmoniumEffectMaskParams {
    std::shared_ptr<Drawing::Image> image = nullptr;
};

constexpr char GE_FILTER_EDGE_LIGHT[] = "EdgeLight";
constexpr char GE_FILTER_EDGE_LIGHT_ALPHA[] = "EdgeLight_Alpha";
constexpr char GE_FILTER_EDGE_LIGHT_BLOOM[] = "EdgeLight_Bloom";
constexpr char GE_FILTER_EDGE_LIGHT_COLOR[] = "EdgeLight_Color";
constexpr char GE_FILTER_EDGE_LIGHT_MASK[] = "EdgeLight_Mask";
constexpr char GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR[] = "EdgeLight_UseRawColor";
struct GEEdgeLightShaderFilterParams {
    float alpha = 1.0f;
    bool bloom = true;
    Vector4f color;
    std::shared_ptr<GEShaderMask> mask = nullptr;
    bool useRawColor = false;
};
REGISTER_GEFILTERPARAM_TYPEINFO(EDGE_LIGHT, GEEdgeLightShaderFilterParams, GE_FILTER_EDGE_LIGHT);

constexpr char GE_FILTER_BEZIER_WARP[] = "BezierWarp";
constexpr char GE_FILTER_BEZIER_WARP_DESTINATION_PATCH[] = "BEZIER_WARP_DESTINATION_PATCH";
constexpr size_t GE_FILTER_BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT0[] = "BezierWarp_ControlPoint0";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT1[] = "BezierWarp_ControlPoint1";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT2[] = "BezierWarp_ControlPoint2";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT3[] = "BezierWarp_ControlPoint3";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT4[] = "BezierWarp_ControlPoint4";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT5[] = "BezierWarp_ControlPoint5";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT6[] = "BezierWarp_ControlPoint6";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT7[] = "BezierWarp_ControlPoint7";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT8[] = "BezierWarp_ControlPoint8";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT9[] = "BezierWarp_ControlPoint9";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT10[] = "BezierWarp_ControlPoint10";
constexpr char GE_FILTER_BEZIER_WARP_CONTROL_POINT11[] = "BezierWarp_ControlPoint11";
struct GEBezierWarpShaderFilterParams {
    std::array<Drawing::Point, GE_FILTER_BEZIER_WARP_POINT_NUM> destinationPatch;
};
REGISTER_GEFILTERPARAM_TYPEINFO(BEZIER_WARP, GEBezierWarpShaderFilterParams, GE_FILTER_BEZIER_WARP);

constexpr char GE_MASK_PIXEL_MAP[] = "PixelMapMask";
constexpr char GE_MASK_PIXEL_MAP_PIXEL_MAP[] = "PixelMapMask_Image";
constexpr char GE_MASK_PIXEL_MAP_SRC[] = "PixelMapMask_Src";
constexpr char GE_MASK_PIXEL_MAP_DST[] = "PixelMapMask_Dst";
constexpr char GE_MASK_PIXEL_MAP_FILL_COLOR[] = "PixelMapMask_FillColor";
struct GEPixelMapMaskParams {
    std::shared_ptr<Drawing::Image> image = nullptr;
    RectF src;
    RectF dst;
    Vector4f fillColor;
};
REGISTER_GEFILTERPARAM_TYPEINFO(PIXEL_MAP_MASK, GEPixelMapMaskParams, GE_MASK_PIXEL_MAP);

constexpr char GE_FILTER_DISPERSION[] = "Dispersion";
constexpr char GE_FILTER_DISPERSION_MASK[] = "Dispersion_Mask";
constexpr char GE_FILTER_DISPERSION_OPACITY[] = "Dispersion_Opacity";
constexpr char GE_FILTER_DISPERSION_RED_OFFSET[] = "Dispersion_RedOffset";
constexpr char GE_FILTER_DISPERSION_GREEN_OFFSET[] = "Dispersion_GreenOffset";
constexpr char GE_FILTER_DISPERSION_BLUE_OFFSET[] = "Dispersion_BlueOffset";
struct GEDispersionShaderFilterParams {
    std::shared_ptr<GEShaderMask> mask;
    float opacity;
    std::pair<float, float> redOffset;
    std::pair<float, float> greenOffset;
    std::pair<float, float> blueOffset;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DISPERSION, GEDispersionShaderFilterParams, GE_FILTER_DISPERSION);

constexpr char GE_FILTER_DIRECTION_LIGHT[] = "DirectionLight";
constexpr char GE_FILTER_DIRECTION_LIGHT_MASK[] = "DirectionLight_Mask";
constexpr char GE_FILTER_DIRECTION_LIGHT_FACTOR[] = "DirectionLight_Factor";
constexpr char GE_FILTER_DIRECTION_LIGHT_DIRECTION[] = "DirectionLight_Direction";
constexpr char GE_FILTER_DIRECTION_LIGHT_COLOR[] = "DirectionLight_Color";
constexpr char GE_FILTER_DIRECTION_LIGHT_INTENSITY[] = "DirectionLight_Intensity";
struct GEDirectionLightShaderFilterParams {
    std::shared_ptr<GEShaderMask> mask;
    float maskFactor = 1.0f;
    Vector3f lightDirection = Vector3f(0.0f, 0.0f, 1.0f);
    Vector4f lightColor = Vector4f(2.0f, 2.0f, 2.0f, 1.0f);
    float lightIntensity = 0.5f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DIRECTION_LIGHT, GEDirectionLightShaderFilterParams, GE_FILTER_DIRECTION_LIGHT);

constexpr char GE_MASK_RADIAL_GRADIENT[] = "RadialGradientMask";
constexpr char GE_MASK_RADIAL_GRADIENT_CENTER[] = "RadialGradientMask_Center";
constexpr char GE_MASK_RADIAL_GRADIENT_RADIUSX[] = "RadialGradientMask_RadiusX";
constexpr char GE_MASK_RADIAL_GRADIENT_RADIUSY[] = "RadialGradientMask_RadiusY";
constexpr char GE_MASK_RADIAL_GRADIENT_COLORS[] = "RadialGradientMask_Colors";
constexpr char GE_MASK_RADIAL_GRADIENT_POSITIONS[] = "RadialGradientMask_Positions";
struct GERadialGradientShaderMaskParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radiusX_ = 0.f;
    float radiusY_ = 0.f;
    std::vector<float> colors_;
    std::vector<float> positions_;
};
REGISTER_GEFILTERPARAM_TYPEINFO(RADIAL_GRADIENT_MASK, GERadialGradientShaderMaskParams, GE_MASK_RADIAL_GRADIENT);

constexpr char GE_MASK_WAVE_GRADIENT[] = "WaveGradientMask";
constexpr char GE_MASK_WAVE_GRADIENT_CENTER[] = "WaveGradientMask_WaveCenter";
constexpr char GE_MASK_WAVE_GRADIENT_WIDTH[] = "WaveGradientMask_WaveWidth";
constexpr char GE_MASK_WAVE_GRADIENT_BLUR_RADIUS[] = "WaveGradientMask_BlurRadius";
constexpr char GE_MASK_WAVE_GRADIENT_TURBULENCE_STRENGTH[] = "WaveGradientMask_TurbulenceStrength";
constexpr char GE_MASK_WAVE_GRADIENT_PROPAGATION_RADIUS[] = "WaveGradientMask_PropagationRadius";
struct GEWaveGradientShaderMaskParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float width_ = 0.f;
    float propagationRadius_ = 0.f;
    float blurRadius_ = 0.f;
    float turbulenceStrength_ = 0.f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(WAVE_GRADIENT_MASK, GEWaveGradientShaderMaskParams, GE_MASK_WAVE_GRADIENT);

constexpr char GE_FILTER_CONTENT_LIGHT[] = "ContentLight";
constexpr char GE_FILTER_CONTENT_LIGHT_POSITION[] = "ContentLightPosition";
constexpr char GE_FILTER_CONTENT_LIGHT_COLOR[] = "ContentLightColor";
constexpr char GE_FILTER_CONTENT_LIGHT_INTENSITY[] = "ContentLightIntensity";
constexpr char GE_FILTER_CONTENT_LIGHT_ROTATION_ANGLE[] = "ContentLightRotationAngle";
struct GEContentLightFilterParams {
    Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);
    Vector4f color = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    float intensity = 0.0f;
    Vector3f rotationAngle = Vector3f(0.0f, 0.0f, 0.0f);
};
REGISTER_GEFILTERPARAM_TYPEINFO(CONTENT_LIGHT, GEContentLightFilterParams, GE_FILTER_CONTENT_LIGHT);

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT[] = "ContourDiagonalFlowLight";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_CONTOUR[] = "ContourDiagonalFlowLight_Contour";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_START[] = "ContourDiagonalFlowLight_Line1Start";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_LENGTH[] = "ContourDiagonalFlowLight_Line1Length";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_COLOR[] = "ContourDiagonalFlowLight_Line1Color";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_START[] = "ContourDiagonalFlowLight_Line2Start";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_LENGTH[] = "ContourDiagonalFlowLight_Line2Length";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_COLOR[] = "ContourDiagonalFlowLight_Line2Color";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_THICKNESS[] = "ContourDiagonalFlowLight_Thickness";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_RADIUS[] = "ContourDiagonalFlowLight_HaloRadius";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LIGHTWEIGHT[] = "ContourDiagonalFlowLight_LightWeight";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_HALOWEIGHT[] = "ContourDiagonalFlowLight_HaloWeight";
struct GEContentDiagonalFlowLightShaderParams {
    std::vector<Vector2f> contour_{};
    float line1Start_ = 0.f;
    float line1Length_ = 0.f;
    Vector4f line1Color_ = Vector4f(0.f, 0.f, 0.f, 0.f);
    float line2Start_ = 0.f;
    float line2Length_ = 0.f;
    Vector4f line2Color_ = Vector4f(0.f, 0.f, 0.f, 0.f);
    float thickness_ = 0.0f;
    float haloRadius_ = 25.f;
    float lightWeight_ = 1.f;
    float haloWeight_ = 2.f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(CONTOUR_DIAGONAL_FLOW_LIGHT, GEContentDiagonalFlowLightShaderParams,
                                GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT);

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT[] = "WavyRippleLight";
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT_CENTER[] = "WavyRippleLight_Center";
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT_RIADIUS[] = "WavyRippleLight_Radius";
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT_THICKNESS[] = "WavyRippleLight_Thickness";
struct GEWavyRippleLightShaderParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radius_ = 0.f;
    float thickness_ = 0.2f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(WAVY_RIPPLE_LIGHT, GEWavyRippleLightShaderParams,
                                GE_SHADER_WAVY_RIPPLE_LIGHT);

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_AURORA_NOISE[] = "AuroraNoise";
constexpr char GE_SHADER_AURORA_NOISE_VALUE[] = "AuroraNoise_Noise";
struct GEAuroraNoiseShaderParams {
    float noise_ = 0.f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(AURORA_NOISE, GEAuroraNoiseShaderParams, GE_SHADER_AURORA_NOISE);

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_PARTICLE_CIRCULAR_HALO[] = "ParticleCircularHalo";
constexpr char GE_SHADER_PARTICLE_CIRCULAR_HALO_CENTER[] = "ParticleCircularHalo_Center";
constexpr char GE_SHADER_PARTICLE_CIRCULAR_HALO_RADIUS[] = "ParticleCircularHalo_Radius";
constexpr char GE_SHADER_PARTICLE_CIRCULAR_HALO_NOISE[] = "ParticleCircularHalo_Noise";
struct GEParticleCircularHaloShaderParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radius_ = 0.f;
    float noise_ = 0.f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(PARTICLE_CIRCULAR_HALO, GEParticleCircularHaloShaderParams,
                                GE_SHADER_PARTICLE_CIRCULAR_HALO);

constexpr char GE_FILTER_MASK_TRANSITION[] = "MaskTransition";
constexpr char GE_FILTER_MASK_TRANSITION_MASK[] = "MaskTransition_Mask";
constexpr char GE_FILTER_MASK_TRANSITION_FACTOR[] = "MaskTransition_Factor";
constexpr char GE_FILTER_MASK_TRANSITION_INVERSE[] = "MaskTransition_Inverse";
struct GEMaskTransitionShaderFilterParams {
    std::shared_ptr<GEShaderMask> mask = nullptr;
    float factor = 1.0f;
    bool inverse = false;
};
REGISTER_GEFILTERPARAM_TYPEINFO(MASK_TRANSITION, GEMaskTransitionShaderFilterParams, GE_FILTER_MASK_TRANSITION);

constexpr char GE_FILTER_VARIABLE_RADIUS_BLUR[] = "VariableRadiusBlur";
constexpr char GE_FILTER_VARIABLE_RADIUS_BLUR_RADIUS[] = "VariableRadiusBlur_Radius";
constexpr char GE_FILTER_VARIABLE_RADIUS_BLUR_MASK[] = "VariableRadiusBlur_Mask";
struct GEVariableRadiusBlurShaderFilterParams {
    std::shared_ptr<GEShaderMask> mask;
    float blurRadius;
};
REGISTER_GEFILTERPARAM_TYPEINFO(VARIABLE_RADIUS_BLUR, GEVariableRadiusBlurShaderFilterParams,
                                GE_FILTER_VARIABLE_RADIUS_BLUR);

constexpr char GE_MASK_LINEAR_GRADIENT[] = "MaskLinearGradient";
constexpr char GE_MASK_LINEAR_GRADIENT_START_POSITION[] = "MaskLinearGradient_StartPosition";
constexpr char GE_MASK_LINEAR_GRADIENT_END_POSITION[] = "MaskLinearGradient_EndPosition";
struct GELinearGradientShaderMaskParams {
    std::vector<std::pair<float, float>> fractionStops;
    Drawing::Point startPosition;
    Drawing::Point endPosition;
};
REGISTER_GEFILTERPARAM_TYPEINFO(LINEAR_GRADIENT_MASK, GELinearGradientShaderMaskParams, GE_MASK_LINEAR_GRADIENT);

constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT[] = "ColorGradientEffect";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR0[] = "ColorGradientEffect_Color0";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR1[] = "ColorGradientEffect_Color1";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR2[] = "ColorGradientEffect_Color2";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR3[] = "ColorGradientEffect_Color3";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR4[] = "ColorGradientEffect_Color4";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR5[] = "ColorGradientEffect_Color5";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR6[] = "ColorGradientEffect_Color6";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR7[] = "ColorGradientEffect_Color7";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR8[] = "ColorGradientEffect_Color8";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR9[] = "ColorGradientEffect_Color9";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR10[] = "ColorGradientEffect_Color10";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR11[] = "ColorGradientEffect_Color11";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS0[] = "ColorGradientEffect_Position0";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS1[] = "ColorGradientEffect_Position1";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS2[] = "ColorGradientEffect_Position2";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS3[] = "ColorGradientEffect_Position3";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS4[] = "ColorGradientEffect_Position4";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS5[] = "ColorGradientEffect_Position5";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS6[] = "ColorGradientEffect_Position6";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS7[] = "ColorGradientEffect_Position7";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS8[] = "ColorGradientEffect_Position8";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS9[] = "ColorGradientEffect_Position9";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS10[] = "ColorGradientEffect_Position10";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_POS11[] = "ColorGradientEffect_Position11";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH0[] = "ColorGradientEffect_Strength0";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH1[] = "ColorGradientEffect_Strength1";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH2[] = "ColorGradientEffect_Strength2";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH3[] = "ColorGradientEffect_Strength3";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH4[] = "ColorGradientEffect_Strength4";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH5[] = "ColorGradientEffect_Strength5";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH6[] = "ColorGradientEffect_Strength6";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH7[] = "ColorGradientEffect_Strength7";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH8[] = "ColorGradientEffect_Strength8";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH9[] = "ColorGradientEffect_Strength9";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH10[] = "ColorGradientEffect_Strength10";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH11[] = "ColorGradientEffect_Strength11";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_MASK[] = "ColorGradientEffect_Mask";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR_NUMBER[] = "ColorGradientEffect_ColorNumber";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_BLEND[] = "ColorGradientEffect_Blend";
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_BLEND_K[] = "ColorGradientEffect_BlendK";

constexpr int COLOR_GRADIENT_ARRAY_SIZE = 12;
struct GEXColorGradientEffectParams {
    std::array<Drawing::Color4f, COLOR_GRADIENT_ARRAY_SIZE> colors_;
    std::array<Drawing::Point, COLOR_GRADIENT_ARRAY_SIZE> positions_;
    std::array<float, COLOR_GRADIENT_ARRAY_SIZE> strengths_;
    float colorNum_ = 0.0f;
    float blend_ = 0.0f;
    float blendk_ = 0.0f;
    std::shared_ptr<GEShaderMask> mask_;
};
REGISTER_GEFILTERPARAM_TYPEINFO(COLOR_GRADIENT_EFFECT, GEXColorGradientEffectParams, GEX_SHADER_COLOR_GRADIENT_EFFECT);

constexpr char GEX_SHADER_LIGHT_CAVE[] = "LightCave";
constexpr char GEX_SHADER_LIGHT_CAVE_COLORA[] = "LightCave_ColorA";
constexpr char GEX_SHADER_LIGHT_CAVE_COLORB[] = "LightCave_ColorB";
constexpr char GEX_SHADER_LIGHT_CAVE_COLORC[] = "LightCave_ColorC";
constexpr char GEX_SHADER_LIGHT_CAVE_POSITION[] = "LightCave_Position";
constexpr char GEX_SHADER_LIGHT_CAVE_RADIUSXY[] = "LightCave_RadiusXY";
constexpr char GEX_SHADER_LIGHT_CAVE_PROGRESS[] = "LightCave_Progress";
struct GEXLightCaveShaderParams {
    Vector4f colorA = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f colorB = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f colorC = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    Vector2f position = Vector2f(0.0f, 0.0f);
    Vector2f radiusXY = Vector2f(0.0f, 0.0f);
    float progress = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(LIGHT_CAVE, GEXLightCaveShaderParams, GEX_SHADER_LIGHT_CAVE);

constexpr char GE_SHADER_BORDER_LIGHT[] = "BorderLight";
constexpr char GE_SHADER_BORDER_LIGHT_POSITION[] = "BorderLightPosition";
constexpr char GE_SHADER_BORDER_LIGHT_COLOR[] = "BorderLightColor";
constexpr char GE_SHADER_BORDER_LIGHT_INTENSITY[] = "BorderLightIntensity";
constexpr char GE_SHADER_BORDER_LIGHT_WIDTH[] = "BorderLightWidth";
constexpr char GE_SHADER_BORDER_LIGHT_ROTATION_ANGLE[] = "BorderLightRotationAngle";
constexpr char GE_SHADER_BORDER_LIGHT_CORNER_RADIUS[] = "BorderLightCornerRadius";
struct GEBorderLightShaderParams {
    Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);
    Vector4f color = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    float intensity = 0.0f;
    float width = 0.0f;
    Vector3f rotationAngle = Vector3f(0.0f, 0.0f, 0.0f);
    float cornerRadius = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(BORDER_LIGHT, GEBorderLightShaderParams, GE_SHADER_BORDER_LIGHT);

enum class GESDFUnionOp : uint8_t {
    UNION = 0,
    SMOOTH_UNION,
    MAX = SMOOTH_UNION,
};

class GERRect {
public:
    GERRect(float l = 0.f, float t = 0.f, float w = 0.f,
            float h = 0.f, float rx = 0.f, float ry = 0.f) {}
    float left_ = 0.f;
    float top_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float radiusX_ = 0.f;
    float radiusY_ = 0.f;
};

using GESDFShapeNode = std::variant<GERRect, GESDFUnionOp>;

constexpr char GE_MASK_SDF_UNION_OP[] = "SDFUnionOpMask";
constexpr char GE_MASK_SDF_UNION_OP_MASKX[] = "SDFUnionOpMask_MaskX";
constexpr char GE_MASK_SDF_UNION_OP_MASKY[] = "SDFUnionOpMask_MaskY";
constexpr char GE_MASK_SDF_UNION_OP_TYPE[] = "SDFUnionOpMask_Type";
// for smooth union op
constexpr char GE_MASK_SDF_SMOOTH_UNION_OP[] = "SDFSmoothUnionOpMask";
constexpr char GE_MASK_SDF_SMOOTH_UNION_OP_SPACING[] = "SDFSmoothUnionOpMask_Spacing";
constexpr char GE_MASK_SDF_SMOOTH_UNION_OP_MASKX[] = "SDFSmoothUnionOpMask_MaskX";
constexpr char GE_MASK_SDF_SMOOTH_UNION_OP_MASKY[] = "SDFSmoothUnionOpMask_MaskY";
struct GESDFUnionOpMaskParams {
    float spacing = 0.f;
    std::shared_ptr<GESDFShaderMask> left;
    std::shared_ptr<GESDFShaderMask> right;
    GESDFUnionOp op;
};

constexpr char GE_MASK_SDF_RRECT_MASK[] = "SDFRRectMask";
constexpr char GE_MASK_SDF_RRECT_MASK_RRECT[] = "SDFRRectMask_RRect";
struct GESDFRRectMaskParams {
    GERRect rrect;
};

constexpr char GE_FILTER_SDF[] = "SDFFilter";
constexpr char GE_FILTER_SDF_MASK[] = "SDFFilter_Mask";
struct GESDFBorderParams final {
    Color color;
    float width = 0.0f;
};

struct GESDFShadowParams final {
    Drawing::Color color;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float radius = 0.0;
    Drawing::Path path;
    bool isFilled = false;
};

struct GESDFFilterParams {
    std::shared_ptr<GESDFShaderMask> mask;
    std::optional<GESDFBorderParams> border;
    std::optional<GESDFShadowParams> shadow;
};

constexpr int ARRAY_SIZE_FOUR = 4;

constexpr char GEX_SHADER_AIBAR_GLOW[] = "AIBarGlow";
constexpr char GEX_SHADER_AIBAR_GLOW_LTWH[] = "AIBarGlow_LTWH";
constexpr char GEX_SHADER_AIBAR_GLOW_STRECTCH_FACTOR[] = "AIBarGlow_StretchFactor";
constexpr char GEX_SHADER_AIBAR_GLOW_BAR_ANGLE[] = "AIBarGlow_BarAngle";
constexpr char GEX_SHADER_AIBAR_GLOW_COLOR0[] = "AIBarGlow_Color0";
constexpr char GEX_SHADER_AIBAR_GLOW_COLOR1[] = "AIBarGlow_Color1";
constexpr char GEX_SHADER_AIBAR_GLOW_COLOR2[] = "AIBarGlow_Color2";
constexpr char GEX_SHADER_AIBAR_GLOW_COLOR3[] = "AIBarGlow_Color3";
constexpr char GEX_SHADER_AIBAR_GLOW_POS0[] = "AIBarGlow_Position0";
constexpr char GEX_SHADER_AIBAR_GLOW_POS1[] = "AIBarGlow_Position1";
constexpr char GEX_SHADER_AIBAR_GLOW_POS2[] = "AIBarGlow_Position2";
constexpr char GEX_SHADER_AIBAR_GLOW_POS3[] = "AIBarGlow_Position3";
constexpr char GEX_SHADER_AIBAR_GLOW_STRENGTH[] = "AIBarGlow_Strength";
constexpr char GEX_SHADER_AIBAR_GLOW_BRIGHTNESS[] = "AIBarGlow_Brightness";
constexpr char GEX_SHADER_AIBAR_GLOW_PROGRESS[] = "AIBarGlow_Progress";
struct GEXAIBarGlowEffectParams {
    Vector4f LTWH;
    float stretchFactor = 0.0f;
    float barAngle = 0.0f;
    std::array<Vector4f, ARRAY_SIZE_FOUR> colors;
    std::array<std::pair<float, float>, ARRAY_SIZE_FOUR> positions;
    Vector4f strengths;
    float brightness;
    float progress;
};

constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT[] = "RoundedRectFlowlight";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_START_END_POS[] = "RoundedRectFlowlight_StartEndPosition";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_WAVE_LENGTH[] = "RoundedRectFlowlight_WaveLength";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_WAVE_TOP[] = "RoundedRectFlowlight_WaveTop";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_CORNER_RADIUS[] = "RoundedRectFlowlight_CornerRadius";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_BRIGHTNESS[] = "RoundedRectFlowlight_Brightness";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_SCALE[] = "RoundedRectFlowlight_Scale";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_SHARPING[] = "RoundedRectFlowlight_Sharping";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_FEATHERING[] = "RoundedRectFlowlight_Feathering";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_FEATHERING_BEZIER_CONTROL_POINTS[] =
    "RoundedRectFlowlight_FeatheringBezierControlPoints";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_GRADIENT_BEZIER_CONTROL_POINTS[] =
    "RoundedRectFlowlight_GradientBezierControlPoints";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_COLOR[] = "RoundedRectFlowlight_Color";
constexpr char GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_PROGRESS[] = "RoundedRectFlowlight_Progress";
struct GEXRoundedRectFlowlightEffectParams {
    std::pair<float, float> startEndPosition;
    float waveLength = 0.0f;
    float waveTop = 0.0f;
    float cornerRadius = 0.0f;
    float brightness = 0.0f;
    float scale = 0.0f;
    float sharping = 0.0f;
    float feathering = 0.0f;
    Vector4f featheringBezierControlPoints;
    Vector4f gradientBezierControlPoints;
    Vector4f color;
    float progress = 0.0f;
};

constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS[] = "GradientFlowColors";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR0[] = "GradientFlowColors_Color0";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR1[] = "GradientFlowColors_Color1";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR2[] = "GradientFlowColors_Color2";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR3[] = "GradientFlowColors_Color3";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_GRADIENT_BEGIN[] = "GradientFlowColors_GradientBegin";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_GRADIENT_END[] = "GradientFlowColors_GradientEnd";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_EFFECT_ALPHA[] = "GradientFlowColors_EffectAlpha";
constexpr char GEX_SHADER_GRADIENT_FLOW_COLORS_PROGRESS[] = "GradientFlowColors_Progress";
struct GEXGradientFlowColorsEffectParams {
    std::array<Vector4f, ARRAY_SIZE_FOUR> colors;
    float gradientBegin = 0.0f;
    float gradientEnd = 0.0f;
    float effectAlpha = 0.0f;
    float progress = 0.0f;
};

constexpr char GE_MASK_FRAME_GRADIENT[] = "FrameGradientMask";
constexpr char GE_MASK_FRAME_GRADIENT_GRADIENT_BEZIER_CONTROL_POINTS[] =
    "FrameGradientMask_GradientBezierControlPoints";
constexpr char GE_MASK_FRAME_GRADIENT_CORNER_RADIUS[] = "FrameGradientMask_CornerRadius";
constexpr char GE_MASK_FRAME_GRADIENT_FRAME_WIDTH[] = "FrameGradientMask_FrameWidth";
struct GEFrameGradientMaskParams {
    Vector4f gradientBezierControlPoints;
    float cornerRadius = 0.0f;
    float frameWidth = 0.0f;
};

constexpr char GE_FILTER_GASIFY_SCALE_TWIST[] = "GasifyScaleTwist";
constexpr char GE_FILTER_GASIFY_SCALE_TWIST_PROGRESS[] = "GasifyScaleTwist_Progress";
constexpr char GE_FILTER_GASIFY_SCALE_TWIST_SOURCEIMAGE[] = "GasifyScaleTwist_SourceImage";
constexpr char GE_FILTER_GASIFY_SCALE_TWIST_SCALE[] = "GasifyScaleTwist_Scale";
constexpr char GE_FILTER_GASIFY_SCALE_TWIST_MASK[] = "GasifyScaleTwist_Mask";
struct GEGasifyScaleTwistFilterParams {
    std::pair<float, float> scale_ = {1.0f, 1.0f};
    std::shared_ptr<Drawing::Image> sourceImage_ = nullptr;
    std::shared_ptr<Drawing::Image> maskImage_ = nullptr;
    float progress_ = 0.f;
};

constexpr char GE_FILTER_GASIFY_BLUR[] = "GasifyBlur";
constexpr char GE_FILTER_GASIFY_BLUR_PROGRESS[] = "GasifyBlur_Progress";
constexpr char GE_FILTER_GASIFY_BLUR_SOURCEIMAGE[] = "GasifyBlur_SourceImage";
constexpr char GE_FILTER_GASIFY_BLUR_MASK[] = "GasifyBlur_Mask";
struct GEGasifyBlurFilterParams {
    std::shared_ptr<Drawing::Image> sourceImage_ = nullptr;
    std::shared_ptr<Drawing::Image> maskImage_ = nullptr;
    float progress_ = 0.f;
};

constexpr char GE_FILTER_GASIFY[] = "Gasify";
constexpr char GE_FILTER_GASIFY_PROGRESS[] = "Gasify_Progress";
constexpr char GE_FILTER_GASIFY_SOURCEIMAGE[] = "Gasify_SourceImage";
constexpr char GE_FILTER_GASIFY_MASK[] = "Gasify_Mask";
struct GEGasifyFilterParams {
    std::shared_ptr<Drawing::Image> sourceImage_ = nullptr;
    std::shared_ptr<Drawing::Image> maskImage_ = nullptr;
    float progress_ = 0.f;
};

#undef REGISTER_GEFILTERPARAM_TYPEINFO
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
