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

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "utils/matrix.h"
#include "ge_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct CanvasInfo {
    float geoWidth_ = 0.0f;
    float geoHeight_ = 0.0f;
    float tranX_ = 0.0f;
    float tranY_ = 0.0f;
    Drawing::Matrix mat_;
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
constexpr char GE_FILTER_SOUND_WAVE_SOUNDINTENSITY[] = "SOUNDINTENSITY";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A[] = "SHOCKWAVEALPHAA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B[] = "SHOCKWAVEALPHAB";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A[] = "SHOCKWAVEPROGRESSA";
constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B[] = "SHOCKWAVEPROGRESSB";
constexpr char GE_FILTER_SOUND_WAVE_TOTAL_ALPHA[] = "SHOCKWAVETOTALALPHA";
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

constexpr char GE_MASK_DOUBLE_RIPPLE[] = "DoubleRippleMask";
constexpr char GE_MASK_DOUBLE_RIPPLE_CENTER1[] = "DoubleRippleMask_Center1";
constexpr char GE_MASK_DOUBLE_RIPPLE_CENTER2[] = "DoubleRippleMask_Center2";
constexpr char GE_MASK_DOUBLE_RIPPLE_RADIUS[] = "DoubleRippleMask_Radius";
constexpr char GE_MASK_DOUBLE_RIPPLE_WIDTH[] = "DoubleRippleMask_Width";
constexpr char GE_MASK_DOUBLE_RIPPLE_TURBULENCE[] = "DoubleRippleMask_Turbulence";
struct GEDoubleRippleShaderMaskParams {
    std::pair<float, float> center1_ = {0.f, 0.f};
    std::pair<float, float> center2_ = {0.f, 0.f};
    float radius_ = 0.f;
    float width_ = 0.f;
    float turbulence_ = 0.0f;
};

constexpr char GE_FILTER_DISPLACEMENT_DISTORT[] = "DispDistort";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_FACTOR[] = "DispDistort_Factor";
constexpr char GE_FILTER_DISPLACEMENT_DISTORT_MASK[] = "DispDistort_Mask";
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
constexpr char GE_FILTER_EDGE_LIGHT_BLOOM[] = "EDGE_LIGHT_BLOOM";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R[] = "EDGE_LIGHT_EDGE_COLOR_R";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G[] = "EDGE_LIGHT_EDGE_COLOR_G";
constexpr char GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B[] = "EDGE_LIGHT_EDGE_COLOR_B";
constexpr char GE_FILTER_EDGE_LIGHT_MASK[] = "EDGE_LIGHT_MASK";
constexpr char GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR[] = "EDGE_LIGHT_USE_RAW_COLOR";
struct GEEdgeLightShaderFilterParams {
    float alpha = 1.0f;
    bool bloom = true;
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
constexpr char GE_FILTER_DISPERSION[] = "DISPERSION";
constexpr char GE_FILTER_DISPERSION_MASK[] = "DISPERSION_MASK";
constexpr char GE_FILTER_DISPERSION_OPACITY[] = "DISPERSION_OPACITY";
constexpr char GE_FILTER_DISPERSION_RED_OFFSET_X[] = "DISPERSION_RED_OFFSET_X";
constexpr char GE_FILTER_DISPERSION_RED_OFFSET_Y[] = "DISPERSION_REF_OFFSET_Y";
constexpr char GE_FILTER_DISPERSION_GREEN_OFFSET_X[] = "DISPERSION_GREEN_OFFSET_X";
constexpr char GE_FILTER_DISPERSION_GREEN_OFFSET_Y[] = "DISPERSION_GREEN_OFFSET_Y";
constexpr char GE_FILTER_DISPERSION_BLUE_OFFSET_X[] = "DISPERSION_BLUE_OFFSET_X";
constexpr char GE_FILTER_DISPERSION_BLUE_OFFSET_Y[] = "DISPERSION_BLUE_OFFSET_Y";
struct GEDispersionShaderFilterParams {
    std::shared_ptr<GEShaderMask> mask;
    float opacity;
    float redOffsetX;
    float redOffsetY;
    float greenOffsetX;
    float greenOffsetY;
    float blueOffsetX;
    float blueOffsetY;
};

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

constexpr char GE_FILTER_CONTENT_LIGHT[] = "CONTENT_LIGHT";
constexpr char GE_FILTER_CONTENT_LIGHT_POSITION[] = "CONTENT_LIGHT_POSITION";
constexpr char GE_FILTER_CONTENT_LIGHT_COLOR[] = "CONTENT_LIGHT_COLOR";
constexpr char GE_FILTER_CONTENT_LIGHT_INTENSITY[] = "CONTENT_LIGHT_INTENSITY";
constexpr char GE_FILTER_CONTENT_LIGHT_ROTATION_ANGLE[] = "CONTENT_LIGHT_ROTATION_ANGLE";
struct GEContentLightFilterParams {
    Vector3f lightPosition;
    Vector4f lightColor;
    float lightIntensity;
    Vector3f rotationAngle;
};

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT[] = "ContourDiagonalFlowLight";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_CONTOUR[] = "ContourDiagonalFlowLight_Contour";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_START[] = "ContourDiagonalFlowLight_Line1Start";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_LENGTH[] = "ContourDiagonalFlowLight_Line1Length";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_COLOR[] = "ContourDiagonalFlowLight_Line1Color";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_START[] = "ContourDiagonalFlowLight_Line2Start";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_LENGTH[] = "ContourDiagonalFlowLight_Line2Length";
constexpr char GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_COLOR[] = "ContourDiagonalFlowLight_Line2Color";
struct GEContentDiagonalFlowLightShaderParams {
    std::shared_ptr<Drawing::Path> contour_ = nullptr;
    float line1Start_ = 0.f;
    float line1Length_ = 0.f;
    Vector4f line1Color_ = Vector4f(0.f, 0.f, 0.f, 0.f);
    float line2Start_ = 0.f;
    float line2Length_ = 0.f;
    Vector4f line2Color_ = Vector4f(0.f, 0.f, 0.f, 0.f);
    float thickness_ = 0.5f;
};

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT[] = "WavyRippleLight";
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT_CENTER[] = "WavyRippleLight_Center";
constexpr char GE_SHADER_WAVY_RIPPLE_LIGHT_RIADIUS[] = "WavyRippleLight_Radius";
struct GEWavyRippleLightShaderParams {
    std::pair<float, float> center_ = {0.f, 0.f};
    float radius_ = 0.f;
};

// Declare same with in rs_render_shader_base.h
constexpr char GE_SHADER_AURORA_NOISE[] = "AuroraNoise";
constexpr char GE_SHADER_AURORA_NOISE_VALUE[] = "AuroraNoise_Noise";
struct GEAuroraNoiseShaderParams {
    float noise_ = 0.f;
};

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


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
