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
#include "sdf/ge_sdf_shader_shape.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class DotMatrixEffectType {
    NONE,
    ROTATE,
    RIPPLE,
};

enum class DotMatrixDirection {
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    TOP_LEFT,
    MAX = TOP_LEFT,
};
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
    RectF materialDst;
};

constexpr int ARRAY_SIZE_NINE = 9;
constexpr int ARRAY_SIZE_FOUR = 4;

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

constexpr char GE_FILTER_SDF_FROM_IMAGE[] = "SDF_FROM_IMAGE";
constexpr char GE_FILTER_SDF_FROM_IMAGE_SPREAD_FACTOR[] = "SDF_FROM_IMAGE_SPREAD_FACTOR";
constexpr char GE_FILTER_SDF_FROM_IMAGE_GENERATE_DERIVS[] = "SDF_FROM_IMAGE_GENERATE_DERIVS";
struct GESDFFromImageFilterParams {
    int spreadFactor;
    bool generateDerivs;
};
REGISTER_GEFILTERPARAM_TYPEINFO(SDF_FROM_IMAGE, GESDFFromImageFilterParams, GE_FILTER_SDF_FROM_IMAGE);

constexpr char GE_FILTER_MAGNIFIER[] = "Magnifier";
constexpr char GE_FILTER_MAGNIFIER_FACTOR[] = "Magnifier_Factor";
constexpr char GE_FILTER_MAGNIFIER_WIDTH[] = "Magnifier_Width";
constexpr char GE_FILTER_MAGNIFIER_HEIGHT[] = "Magnifier_Height";
constexpr char GE_FILTER_MAGNIFIER_CORNER_RADIUS[] = "Magnifier_CornerRadius";
constexpr char GE_FILTER_MAGNIFIER_BORDER_WIDTH[] = "Magnifier_BorderWidth";
constexpr char GE_FILTER_MAGNIFIER_ZOOM_OFFSET_X[] = "Magnifier_ZoomOffsetX";
constexpr char GE_FILTER_MAGNIFIER_ZOOM_OFFSET_Y[] = "Magnifier_ZoomOffsetY";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X[] = "Magnifier_ShadowOffsetX";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y[] = "Magnifier_ShadowOffsetY";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_SIZE[] = "Magnifier_ShadowSize";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_STRENGTH[] = "Magnifier_ShadowStrength";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_1[] = "Magnifier_GradientMaskColor1";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_2[] = "Magnifier_GradientMaskColor2";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_1[] = "Magnifier_OuterContourColor1";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_2[] = "Magnifier_OuterContourColor2";
constexpr char GE_FILTER_MAGNIFIER_ROTATE_DEGREE[] = "Magnifier_RotateDegree";
struct GEMagnifierShaderFilterParams {
    float factor = 0.f;
    float width = 0.f;
    float height = 0.f;
    float cornerRadius = 0.f;
    float borderWidth = 0.f;
    float zoomOffsetX = 0.f;
    float zoomOffsetY = 0.f;

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

constexpr char GE_FILTER_SDF_EDGE_LIGHT[] = "SDFEdgeLight";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_SDF_SPREAD_FACTOR[] = "SDFEdgeLight_SpreadFactor";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_BLOOM_INTENSITY_CUTOFF[] = "SDFEdgeLight_BloomIntensityCutoff";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_MAX_INTENSITY[] = "SDFEdgeLight_LightMaxIntensity";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_MAX_BLOOM_INTENSITY[] = "SDFEdgeLight_MaxBloomIntensity";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_BLOOM_FALLOFF_POW[] = "SDFEdgeLight_BloomFalloffPow";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_MIN_BORDER_WIDTH[] = "SDFEdgeLight_MinBorderWidth";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_MAX_BORDER_WIDTH[] = "SDFEdgeLight_MaxBorderWidth";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_INNER_BORDER_BLOOM_WIDTH[] = "SDFEdgeLight_InnerBorderBloomWidth";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_OUTER_BORDER_BLOOM_WIDTH[] = "SDFEdgeLight_OuterBorderBloomWidth";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_SDF_IMAGE[] = "SDFEdgeLight_SDFImage";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_LIGHT_MASK[] = "SDFEdgeLight_LightMask";
constexpr char GE_FILTER_SDF_EDGE_LIGHT_SDF_SHAPE[] = "SDFEdgeLight_SDFShape";
struct GESDFEdgeLightFilterParams {
    float sdfSpreadFactor = 64.0f;
    float bloomIntensityCutoff = 0.1f;
    float maxIntensity = 1.0f;
    float maxBloomIntensity = 1.0f;
    float bloomFalloffPow = 2.0f;
    float minBorderWidth = 2.0f;
    float maxBorderWidth = 5.0f;
    float innerBorderBloomWidth = 30.0f;
    float outerBorderBloomWidth = 30.0f;
    std::shared_ptr<Drawing::Image> sdfImage;
    std::shared_ptr<GEShaderMask> lightMask;
    std::shared_ptr<Drawing::GESDFShaderShape> sdfShape;
};
REGISTER_GEFILTERPARAM_TYPEINFO(SDF_EDGE_LIGHT, GESDFEdgeLightFilterParams, GE_FILTER_SDF_EDGE_LIGHT);

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
constexpr char GE_SHADER_HARMONIUM_EFFECT_MASKPROGRESS[] = "HarmoniumEffect_MaskProgress";
constexpr char GE_SHADER_HARMONIUM_EFFECT_USEEFFECTMASK[] = "HarmoniumEffect_UseEffectMask";
constexpr char GE_SHADER_HARMONIUM_EFFECT_TINTCOLOR[] = "HarmoniumEffect_TintColor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_RIPPLEPOSITION[] = "HarmoniumEffect_RipplePosition";
constexpr char GE_SHADER_HARMONIUM_EFFECT_RIPPLEPROGRESS[] = "HarmoniumEffect_RippleProgress";
constexpr char GE_SHADER_HARMONIUM_EFFECT_DISTORTPROGRESS[] = "HarmoniumEffect_DistortProgress";
constexpr char GE_SHADER_HARMONIUM_EFFECT_DISTORTFACTOR[] = "HarmoniumEffect_DistortFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_REFLECTIONFACTOR[] = "HarmoniumEffect_ReflectionFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_REFRACTIONFACTOR[] = "HarmoniumEffect_RefractionFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_BLURLEFT[] = "HarmoniumEffect_BlurLeft";
constexpr char GE_SHADER_HARMONIUM_EFFECT_BLURTOP[] = "HarmoniumEffect_BlurTop";
constexpr char GE_SHADER_HARMONIUM_EFFECT_MATERIALFACTOR[] = "HarmoniumEffect_MaterialFactor";
constexpr char GE_SHADER_HARMONIUM_EFFECT_CORNERRADIUS[] = "HarmoniumEffect_CornerRadius";
constexpr char GE_SHADER_HARMONIUM_EFFECT_RATE[] = "HarmoniumEffect_Rate";
constexpr char GE_SHADER_HARMONIUM_EFFECT_LIGHTUPDEGREE[] = "HarmoniumEffect_LightUpDegree";
constexpr char GE_SHADER_HARMONIUM_EFFECT_CUBICCOEFF[] = "HarmoniumEffect_CubicCoeff";
constexpr char GE_SHADER_HARMONIUM_EFFECT_QUADCOEFF[] = "HarmoniumEffect_QuadCoeff";
constexpr char GE_SHADER_HARMONIUM_EFFECT_SATURATION[] = "HarmoniumEffect_Saturation";
constexpr char GE_SHADER_HARMONIUM_EFFECT_POSRGB[] = "HarmoniumEffect_PosRGB";
constexpr char GE_SHADER_HARMONIUM_EFFECT_NEGRGB[] = "HarmoniumEffect_NegRGB";
constexpr char GE_SHADER_HARMONIUM_EFFECT_FRACTION[] = "HarmoniumEffect_Fraction";
constexpr char GE_SHADER_HARMONIUM_EFFECT_TOTALMATRIX[] = "HarmoniumEffect_TotalMatrix";

struct GEHarmoniumEffectShaderParams {
    std::shared_ptr<GEShaderMask> mask = nullptr;
    std::shared_ptr<GEShaderMask> maskClock = nullptr;
    float maskProgress = 1.f;
    std::shared_ptr<GEShaderMask> useEffectMask = nullptr;
    Vector4f tintColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    std::vector<Vector2f> ripplePosition = {};
    float rippleProgress = 0.f;
    float distortProgress = 0.f;
    float distortFactor = 0.f;
    float reflectionFactor = 0.f;
    float refractionFactor = 0.f;
    float blurLeft = 0.f;
    float blurTop = 0.f;
    float materialFactor = 0.f;
    float cornerRadius = 0.f;
    float rate = 0.f;
    float lightUpDegree = 0.f;
    float cubicCoeff = 0.f;
    float quadCoeff = 0.f;
    float saturation = 0.f;
    Vector3f posRGB = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f negRGB = Vector3f(0.0f, 0.0f, 0.0f);
    float fraction = 1.f; // 1 means default value
    Drawing::Matrix totalMatrix;
};

constexpr char GE_MASK_USE_EFFECT[] = "UseEffectMask";
constexpr char GE_MASK_USE_EFFECT_PIXEL_MAP[] = "UseEffectMask_Image";
constexpr char GE_MASK_USE_EFFECT_USE_EFFECT[] = "UseEffectMask_UseEffect";
struct GEUseEffectMaskParams {
    std::weak_ptr<Drawing::Image> image;
    bool useEffect = false;
};

constexpr char GE_MASK_IMAGE[] = "ImageMask";
constexpr char GE_MASK_IMAGE_IMAGE[] = "ImageMask_Image";
struct GEImageMaskParams {
    std::shared_ptr<Drawing::Image> image = nullptr;
};
REGISTER_GEFILTERPARAM_TYPEINFO(IMAGE_MASK, GEImageMaskParams, GE_MASK_IMAGE);

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

constexpr char GE_SHADER_DOT_MATRIX_SHADER[] = "DotMatrixShader";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_PATHDIRECTION[] = "DotMatrixShader_PathDirection";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_EFFECTCOLORS[] = "DotMatrixShader_EffectColors";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_COLORFRACTIONS[] = "DotMatrixShader_ColorFractions";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_STARTPOINTS[] = "DotMatrixShader_StartPoints";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_PATHWIDTH[] = "DotMatrixShader_PathWidth";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_INVERSEEFFECT[] = "DotMatrixShader_InverseEffect";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_DOTCOLOR[] = "DotMatrixShader_DotColor";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_DOTSPACING[] = "DotMatrixShader_DotSpacing";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_DOTRADIUS[] = "DotMatrixShader_DotRadius";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_BGCOLOR[] = "DotMatrixShader_BgColor";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_EFFECTTYPE[] = "DotMatrixShader_EffectType";
constexpr char GE_SHADER_DOT_MATRIX_SHADER_PROGRESS[] = "DotMatrixShader_Progress";
struct GEDotMatrixShaderParams {
    DotMatrixDirection pathDirection_ = DotMatrixDirection::TOP_LEFT;
    std::vector<Vector4f> effectColors_{};
    Vector2f colorFractions_ = Vector2f(0.0f, 0.0f);
    std::vector<Vector2f> startPoints_{};
    float pathWidth_ = 0.0f;
    bool inverseEffect_ = false;
    Vector4f dotColor_ = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    float dotSpacing_ = 0.0f;
    float dotRadius_ = 0.0f;
    Vector4f bgColor_ = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    DotMatrixEffectType effectType_ = DotMatrixEffectType::NONE;
    float progress_ = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DOT_MATRIX, GEDotMatrixShaderParams,
                                GE_SHADER_DOT_MATRIX_SHADER);

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
    bool applyInsideMask = false;
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
constexpr char GEX_SHADER_COLOR_GRADIENT_EFFECT_BRIGHTNESS[] = "ColorGradientEffect_Brightness";

constexpr int COLOR_GRADIENT_ARRAY_SIZE = 12;
struct GEXColorGradientEffectParams {
    std::array<Drawing::Color4f, COLOR_GRADIENT_ARRAY_SIZE> colors_;
    std::array<Drawing::Point, COLOR_GRADIENT_ARRAY_SIZE> positions_;
    std::array<float, COLOR_GRADIENT_ARRAY_SIZE> strengths_;
    float colorNum_ = 0.0f;
    float blend_ = 0.0f;
    float blendk_ = 0.0f;
    std::shared_ptr<GEShaderMask> mask_;
    float brightness_ = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(COLOR_GRADIENT_EFFECT, GEXColorGradientEffectParams, GEX_SHADER_COLOR_GRADIENT_EFFECT);

constexpr char GE_SHADER_FROSTED_GLASS_EFFECT[] = "FrostedGlassEffect";
// Common parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_WEIGHTSEMBOSS[] = "FrostedGlassEffect_WeightsEmboss"; // envLight, sd
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_WEIGHTSEDL[] = "FrostedGlassEffect_WeightsEdl";
// BG darken parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BGRATES[] = "FrostedGlassEffect_BgRates";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BGKBS[] = "FrostedGlassEffect_BgKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BGPOS[] = "FrostedGlassEffect_BgPos";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BGNEG[] = "FrostedGlassEffect_BgNeg";
// Refraction parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_REFRACTPARAMS[] = "FrostedGlassEffect_RefractParams";
// Inner shadow parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SDPARAMS[] = "FrostedGlassEffect_SdParams"; // width. featherPx
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SDRATES[] = "FrostedGlassEffect_SdRates";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SDKBS[] = "FrostedGlassEffect_SdKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SDPOS[] = "FrostedGlassEffect_SdPos";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SDNEG[] = "FrostedGlassEffect_SdNeg";
// Env refraction parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTPARAMS[] =
    "FrostedGlassEffect_EnvLightParams"; // width. featherPx, OutPx
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTRATES[] = "FrostedGlassEffect_EnvLightRates"; // 3, 2
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTKBS[] = "FrostedGlassEffect_EnvLightKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTPOS[] = "FrostedGlassEffect_EnvLightPos";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTNEG[] = "FrostedGlassEffect_EnvLightNeg";
// Edge highlights parameters
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLPARAMS[] = "FrostedGlassEffect_EdLightParams"; // width. featherPx
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLANGLES[] = "FrostedGlassEffect_EdLightAngles"; // AngleDeg, featherDeg
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLDIR[] = "FrostedGlassEffect_EdLightDir"; // x, y
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLRATES[] = "FrostedGlassEffect_EdLightRates";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLKBS[] = "FrostedGlassEffect_EdLightKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLPOS[] = "FrostedGlassEffect_EdLightPos";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_EDLNEG[] = "FrostedGlassEffect_EdLightNeg";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SHAPE[] = "FrostedGlassEffect_Shape";
// Adapt effect component
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BLURIMAGE[] = "FrostedGlassEffect_BlurImage";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_BLURIMAGEFOREDGE[] = "FrostedGlassEffect_BlurImageForEdge";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SNAPSHOTRECT[] = "FrostedGlassEffect_SnapshotRect";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_SNAPSHOTMATRIX[] = "FrostedGlassEffect_SnapshotMatrix";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_REFRACTOUTPX[] = "FrostedGlassEffect_RefractOutPx";

constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_MATERIALCOLOR[] = "FrostedGlassEffect_MaterialColor";

// Dark mode parameters and scale
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_WEIGHTSEMBOSS[] = "FrostedGlassEffect_DarkModeWeightsEmboss";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_BGRATES[] = "FrostedGlassEffect_DarkModeBgRates";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_BGKBS[] = "FrostedGlassEffect_DarkModeBgKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_BGPOS[] = "FrostedGlassEffect_DarkModeBgPos";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_BGNEG[] = "FrostedGlassEffect_DarkModeBgNeg";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_EDLIGHTANGLES[] = "FrostedGlassEffect_DarkModeEdLightAngles";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARKMODE_EDLIGHTKBS[] = "FrostedGlassEffect_DarkModeEdLightKBS";
constexpr char GE_SHADER_FROSTED_GLASS_EFFECT_DARK_SCALE[] = "FrostedGlassEffect_DarkScale";

struct GEFrostedGlassEffectParams {
    Vector2f weightsEmboss = Vector2f(1.0f, 1.0f); // (envLight, sd)
    Vector2f weightsEdl = Vector2f(1.0f, 1.0f); // (envLight, sd)
    // Background darken parameters
    Vector2f bgRates = Vector2f(-0.00003f, 1.2f);
    Vector3f bgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
    Vector3f bgPos = Vector3f(0.3f, 0.5f, 1.0f);
    Vector3f bgNeg = Vector3f(0.5f, 0.5f, 1.0f);
    // Refraction params
    Vector3f refractParams = Vector3f(0.0f, 0.0f, 0.0f);
    // Inner shadow parameters
    Vector3f sdParams = Vector3f(0.0f, 2.0f, 2.0f);
    Vector2f sdRates = Vector2f(0.0f, 0.0f);
    Vector3f sdKBS = Vector3f(-0.02f, 2.0f, 4.62f);
    Vector3f sdPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f sdNeg = Vector3f(1.7f, 3.0f, 1.0f);
    // Env refraction parameters
    Vector2f envLightParams = Vector2f(0.2745f, 2.0f);
    Vector2f envLightRates = Vector2f(0.0f, 0.0f);
    Vector3f envLightKBS = Vector3f(0.8f, 0.2745f, 2.0f);
    Vector3f envLightPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f envLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
    // Edge highlights parameters
    Vector2f edLightParams = Vector2f(2.0f, -1.0f);
    Vector2f edLightAngles = Vector2f(30.0f, 30.0f);
    Vector2f edLightDir = Vector2f(-1.0f, 1.0f);
    Vector2f edLightRates = Vector2f(0.0f, 0.0f);
    Vector3f edLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
    Vector3f edLightPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f edLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
    std::shared_ptr<GESDFShaderShape> sdfShape;
    // Adapt effect component
    std::weak_ptr<Drawing::Image> blurImage; // frosted glass blur by ec
    std::weak_ptr<Drawing::Image> blurImageForEdge; // frosted glass blur downsampled by ec
    RectF snapshotRect; // effect component snapshot total rect
    Drawing::Matrix snapshotMatrix; // effect component total matrix
    float refractOutPx = 0.8f; // envlight refraction outward sampling degree
    Vector4f materialColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    // Dark mode parameters and scale
    Vector2f darkModeWeightsEmboss = Vector2f(1.0f, 1.0f);
    Vector2f darkModeBgRates = Vector2f(-0.00003f, 1.2f);
    Vector3f darkModeBgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
    Vector3f darkModeBgPos = Vector3f(0.3f, 0.5f, 1.0f);
    Vector3f darkModeBgNeg = Vector3f(0.5f, 0.5f, 1.0f);
    Vector3f darkModeEdLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
    Vector2f darkModeEdLightAngles = Vector2f(30.0f, 30.0f);
    float darkScale = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(FROSTED_GLASS_EFFECT, GEFrostedGlassEffectParams, GE_SHADER_FROSTED_GLASS_EFFECT);

constexpr char GE_FILTER_FROSTED_GLASS_BLUR[] = "FrostedGlassBlur";
constexpr char GE_FILTER_FROSTED_GLASS_BLUR_RADIUS[] = "FrostedGlassBlur_Radius";
constexpr char GE_FILTER_FROSTED_GLASS_BLUR_RADIUSSCALE[] = "FrostedGlassBlur_RadiuslScale";
constexpr char GE_FILTER_FROSTED_GLASS_BLUR_REFRACTOUTPX[] = "FrostedGlassBlur_RefractOutPx";
struct GEFrostedGlassBlurShaderFilterParams {
    float radius = 0.0f; // blurParam[0]: blur radius
    float radiusScale = 4.0f; // blurParam[1]: scale ratio for the blur radius of baseblur and edgeblur
    float refractOutPx = 0.8f; // envlightParam[0]: envlight refraction outward sampling degree
};
REGISTER_GEFILTERPARAM_TYPEINFO(FROSTED_GLASS_BLUR, GEFrostedGlassBlurShaderFilterParams, GE_FILTER_FROSTED_GLASS_BLUR);

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

constexpr char GEX_MASK_DUPOLI_NOISE[] = "DupoliNoiseMask";
constexpr char GEX_MASK_DUPOLI_NOISE_PROGRESS[] = "DupoliNoiseMask_Progress";
constexpr char GEX_MASK_DUPOLI_NOISE_GRANULARITY[] = "DupoliNoiseMask_Granularity";
constexpr char GEX_MASK_DUPOLI_NOISE_VERTICALMOVEDISTANCE[] = "DupoliNoiseMask_VerticalMoveDistance";
struct GEXDupoliNoiseMaskParams {
    float progress = 0.0f;
    float granularity = 0.0f;
    float verticalMoveDistance = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DUPOLI_NOISE_MASK, GEXDupoliNoiseMaskParams, GEX_MASK_DUPOLI_NOISE);

constexpr char GEX_SHADER_DISTORT_CHROMA[] = "DistortChroma";
constexpr char GEX_SHADER_DISTORT_CHROMA_PROGRESS[] = "DistortChroma_Progress";
constexpr char GEX_SHADER_DISTORT_CHROMA_TURBULENTMASK[] = "DistortChroma_TurbulentMask";
constexpr char GEX_SHADER_DISTORT_CHROMA_MASK[] = "DistortChroma_Mask";
constexpr char GEX_SHADER_DISTORT_CHROMA_GRANULARITY[] = "DistortChroma_Granularity";
constexpr char GEX_SHADER_DISTORT_CHROMA_FREQUENCY[] = "DistortChroma_Frequency";
constexpr char GEX_SHADER_DISTORT_CHROMA_SHARPNESS[] = "DistortChroma_Sharpness";
constexpr char GEX_SHADER_DISTORT_CHROMA_BRIGHTNESS[] = "DistortChroma_Brightness";
constexpr char GEX_SHADER_DISTORT_CHROMA_DISPERSION[] = "DistortChroma_Dispersion";
constexpr char GEX_SHADER_DISTORT_CHROMA_DISTORTFACTOR[] = "DistortChroma_DistortFactor";
constexpr char GEX_SHADER_DISTORT_CHROMA_SATURATION[] = "DistortChroma_Saturation";
constexpr char GEX_SHADER_DISTORT_CHROMA_COLORSTRENGTH[] = "DistortChroma_ColorStrength";
constexpr char GEX_SHADER_DISTORT_CHROMA_VERTICALMOVEDISTANCE[] = "DistortChroma_VerticalMoveDistance";
struct GEXDistortChromaEffectParams {
    float progress = 0.0f;
    std::shared_ptr<GEShaderMask> turbulentMask;
    float frequency = 0.0f;
    float sharpness = 0.0f;
    float brightness = 0.0f;
    float dispersion = 0.0f;
    float saturation = 0.0f;
    float verticalMoveDistance = 0.0f;
    Vector2f granularity =  Vector2f(0.0f, 0.0f);
    Vector2f distortFactor = Vector2f(0.0f, 0.0f);
    Vector3f colorStrength = Vector3f(0.0f, 0.0f, 0.0f);
    std::shared_ptr<GEShaderMask> mask;
};
REGISTER_GEFILTERPARAM_TYPEINFO(DISTORT_CHROMA, GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA);

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

constexpr char GE_FILTER_GRID_WARP[] = "GridWarp";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT0[] = "GridWarp_GridPoint0";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT1[] = "GridWarp_GridPoint1";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT2[] = "GridWarp_GridPoint2";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT3[] = "GridWarp_GridPoint3";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT4[] = "GridWarp_GridPoint4";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT5[] = "GridWarp_GridPoint5";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT6[] = "GridWarp_GridPoint6";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT7[] = "GridWarp_GridPoint7";
constexpr char GE_FILTER_GRID_WARP_GRID_POINT8[] = "GridWarp_GridPoint8";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE0[] = "GridWarp_RotationAngle0";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE1[] = "GridWarp_RotationAngle1";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE2[] = "GridWarp_RotationAngle2";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE3[] = "GridWarp_RotationAngle3";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE4[] = "GridWarp_RotationAngle4";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE5[] = "GridWarp_RotationAngle5";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE6[] = "GridWarp_RotationAngle6";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE7[] = "GridWarp_RotationAngle7";
constexpr char GE_FILTER_GRID_WARP_ROTATION_ANGLE8[] = "GridWarp_RotationAngle8";
struct GEGridWarpShaderFilterParams {
    std::array<std::pair<float, float>, ARRAY_SIZE_NINE> gridPoints;
    std::array<std::pair<float, float>, ARRAY_SIZE_NINE> rotationAngles;
};
REGISTER_GEFILTERPARAM_TYPEINFO(GRID_WARP, GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP);

constexpr char GE_SHADER_CIRCLE_FLOWLIGHT[] = "CircleFlowlight";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_COLOR0[] = "CircleFlowlight_Color0";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_COLOR1[] = "CircleFlowlight_Color1";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_COLOR2[] = "CircleFlowlight_Color2";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_COLOR3[] = "CircleFlowlight_Color3";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_FREQUENCY[] = "CircleFlowlight_RotationFrequency";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_AMPLITUDE[] = "CircleFlowlight_RotationAmplitude";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_SEED[] = "CircleFlowlight_RotationSeed";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_GRADIENTX[] = "CircleFlowlight_GradientX";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_GRADIENTY[] = "CircleFlowlight_GradientY";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_PROGRESS[] = "CircleFlowlight_Progress";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_STRENGTH[] = "CircleFlowlight_Strength";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_DISTORT_STRENGTH[] = "CircleFlowlight_DistortStrength";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_BLEND_GRADIENT[] = "CircleFlowlight_BlendGradient";
constexpr char GE_SHADER_CIRCLE_FLOWLIGHT_MASK[] = "CircleFlowlight_Mask";
struct GECircleFlowlightEffectParams {
    std::array<Vector4f, ARRAY_SIZE_FOUR> colors;
    Vector4f rotationFrequency;
    Vector4f rotationAmplitude;
    Vector4f rotationSeed;
    Vector4f gradientX;
    Vector4f gradientY;
    float progress = 0.0f;
    Vector4f strength = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    float distortStrength = 4.0f;
    float blendGradient = 1.0f;
    std::shared_ptr<GEShaderMask> mask;
};
REGISTER_GEFILTERPARAM_TYPEINFO(CIRCLE_FLOWLIGHT, GECircleFlowlightEffectParams,
                                GE_SHADER_CIRCLE_FLOWLIGHT);

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

constexpr char GE_SHAPE_SDF_UNION_OP[] = "SDFUnionOpShape";
constexpr char GE_SHAPE_SDF_UNION_OP_SHAPEX[] = "SDFUnionOpShape_ShapeX";
constexpr char GE_SHAPE_SDF_UNION_OP_SHAPEY[] = "SDFUnionOpShape_ShapeY";
constexpr char GE_SHAPE_SDF_UNION_OP_TYPE[] = "SDFUnionOpShape_Type";
// for smooth union op
constexpr char GE_SHAPE_SDF_SMOOTH_UNION_OP[] = "SDFSmoothUnionOpShape";
constexpr char GE_SHAPE_SDF_SMOOTH_UNION_OP_SPACING[] = "SDFSmoothUnionOpShape_Spacing";
constexpr char GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEX[] = "SDFSmoothUnionOpShape_ShapeX";
constexpr char GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEY[] = "SDFSmoothUnionOpShape_ShapeY";
struct GESDFUnionOpShapeParams {
    float spacing = 0.f;
    std::shared_ptr<GESDFShaderShape> left;
    std::shared_ptr<GESDFShaderShape> right;
    GESDFUnionOp op = GESDFUnionOp::SMOOTH_UNION;
};

constexpr char GE_SHAPE_SDF_RRECT_SHAPE[] = "SDFRRectShape";
constexpr char GE_SHAPE_SDF_RRECT_SHAPE_RRECT[] = "SDFRRectShape_RRect";
struct GESDFRRectShapeParams {
    GERRect rrect;
};

constexpr char GE_SHAPE_SDF_TRANSFORM_SHAPE[] = "SDFTransformShape";
constexpr char GE_SHAPE_SDF_TRANSFORM_SHAPE_SHAPE[] = "SDFTransformShape_Shape";
constexpr char GE_SHAPE_SDF_TRANSFORM_SHAPE_MATRIX[] = "SDFTransformShape_Matrix";
struct GESDFTransformShapeParams {
    std::shared_ptr<GESDFShaderShape> shape;
    Drawing::Matrix matrix = Drawing::Matrix();
};

constexpr char GE_SHAPE_SDF_PIXELMAP_SHAPE[] = "SDFPixelmapShape";
constexpr char GE_SHAPE_SDF_PIXELMAP_SHAPE_IMAGE[] = "SDFPixelmapShape_Image";
struct GESDFPixelmapShapeParams {
    std::shared_ptr<Drawing::Image> image;
};

constexpr char GE_SHAPE_SDF_EMPTY_SHAPE[] = "SDFEmptyShape";

struct GESDFBorderParams final {
    Color color;
    float width = 0.0f;
};

constexpr char GE_SHADER_SDF_BORDER[] = "SDFBorder";
constexpr char GE_SHADER_SDF_BORDER_SHAPE[] = "SDFBorder_Shape";
constexpr char GE_SHADER_SDF_BORDER_BORDER[] = "SDFBorder_Border";
struct GESDFBorderShaderParams final {
    std::shared_ptr<GESDFShaderShape> shape = nullptr;
    GESDFBorderParams border;
};

struct GESDFShadowParams final {
    Drawing::Color color;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float radius = 0.0;
    Drawing::Path path;
    bool isFilled = false;
};

constexpr char GE_SHADER_SDF_SHADOW[] = "SDFShadow";
constexpr char GE_SHADER_SDF_SHADOW_SHAPE[] = "SDFShadow_Shape";
constexpr char GE_SHADER_SDF_SHADOW_SHADOW[] = "SDFShadow_Shadow";
struct GESDFShadowShaderParams final {
    std::shared_ptr<GESDFShaderShape> shape = nullptr;
    GESDFShadowParams shadow;
};

constexpr char GE_SHADER_SDF_CLIP[] = "SDFClip";
constexpr char GE_SHADER_SDF_CLIP_SHAPE[] = "SDFClip_Shape";
struct GESDFClipShaderParams final {
    std::shared_ptr<GESDFShaderShape> shape = nullptr;
};

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

constexpr char GEX_SHADER_AIBAR_RECT_HALO[] = "AIBarRectHalo";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_LTWH[] = "AIBarRectHalo_LTWH";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_COLOR0[] = "AIBarRectHalo_Color0";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_COLOR1[] = "AIBarRectHalo_Color1";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_COLOR2[] = "AIBarRectHalo_Color2";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_COLOR3[] = "AIBarRectHalo_Color3";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_POS0[] = "AIBarRectHalo_Position0";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_POS1[] = "AIBarRectHalo_Position1";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_POS2[] = "AIBarRectHalo_Position2";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_POS3[] = "AIBarRectHalo_Position3";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_STRENGTH[] = "AIBarRectHalo_Strength";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_BRIGHTNESS[] = "AIBarRectHalo_Brightness";
constexpr char GEX_SHADER_AIBAR_RECT_HALO_PROGRESS[] = "AIBarRectHalo_Progress";
struct GEXAIBarRectHaloEffectParams {
    Vector4f LTWH;
    std::array<Vector4f, ARRAY_SIZE_FOUR> colors;
    std::array<std::pair<float, float>, ARRAY_SIZE_FOUR> positions;
    Vector4f strengths;
    float brightness = 1.0f;
    float progress = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(AIBAR_RECT_HALO, GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO);

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
constexpr char GE_MASK_FRAME_GRADIENT_INNER_BEZIER[] = "FrameGradientMask_InnerBezier";
constexpr char GE_MASK_FRAME_GRADIENT_OUTER_BEZIER[] = "FrameGradientMask_OuterBezier";
constexpr char GE_MASK_FRAME_GRADIENT_CORNER_RADIUS[] = "FrameGradientMask_CornerRadius";
constexpr char GE_MASK_FRAME_GRADIENT_INNER_FRAME_WIDTH[] = "FrameGradientMask_InnerFrameWidth";
constexpr char GE_MASK_FRAME_GRADIENT_OUTER_FRAME_WIDTH[] = "FrameGradientMask_OuterFrameWidth";
constexpr char GE_MASK_FRAME_GRADIENT_RECT_WH[] = "FrameGradientMask_RectWH";
constexpr char GE_MASK_FRAME_GRADIENT_RECT_POS[] = "FrameGradientMask_RectPos";
struct GEFrameGradientMaskParams {
    Vector4f innerBezier;
    Vector4f outerBezier;
    float cornerRadius = 0.0f;
    float innerFrameWidth = 0.0f;
    float outerFrameWidth = 0.0f;
    std::pair<float, float> rectWH;
    std::pair<float, float> rectPos;
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

constexpr char GE_FILTER_FROSTED_GLASS[] = "FrostedGlass";
// Common parameters
constexpr char GE_FILTER_FROSTED_GLASS_BLURPARAMS[] = "FrostedGlass_BlurParams";
constexpr char GE_FILTER_FROSTED_GLASS_WEIGHTSEMBOSS[] = "FrostedGlass_WeightsEmboss"; // envLight, sd
constexpr char GE_FILTER_FROSTED_GLASS_WEIGHTSEDL[] = "FrostedGlass_WeightsEdl";
// BG darken parameters
constexpr char GE_FILTER_FROSTED_GLASS_BGRATES[] = "FrostedGlass_BgRates";
constexpr char GE_FILTER_FROSTED_GLASS_BGKBS[] = "FrostedGlass_BgKBS";
constexpr char GE_FILTER_FROSTED_GLASS_BGPOS[] = "FrostedGlass_BgPos";
constexpr char GE_FILTER_FROSTED_GLASS_BGNEG[] = "FrostedGlass_BgNeg";
// Refraction parameters
constexpr char GE_FILTER_FROSTED_GLASS_REFRACTPARAMS[] = "FrostedGlass_RefractParams";
// Inner shadow parameters
constexpr char GE_FILTER_FROSTED_GLASS_SDPARAMS[] = "FrostedGlass_SdParams"; // width. featherPx
constexpr char GE_FILTER_FROSTED_GLASS_SDRATES[] = "FrostedGlass_SdRates";
constexpr char GE_FILTER_FROSTED_GLASS_SDKBS[] = "FrostedGlass_SdKBS";
constexpr char GE_FILTER_FROSTED_GLASS_SDPOS[] = "FrostedGlass_SdPos";
constexpr char GE_FILTER_FROSTED_GLASS_SDNEG[] = "FrostedGlass_SdNeg";
// Env refraction parameters
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTPARAMS[] = "FrostedGlass_EnvLightParams"; // width. featherPx, OutPx
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTRATES[] = "FrostedGlass_EnvLightRates"; // 3, 2
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTKBS[] = "FrostedGlass_EnvLightKBS";
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTPOS[] = "FrostedGlass_EnvLightPos";
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTNEG[] = "FrostedGlass_EnvLightNeg";
// Edge highlights parameters
constexpr char GE_FILTER_FROSTED_GLASS_EDLPARAMS[] = "FrostedGlass_EdLightParams"; // width. featherPx
constexpr char GE_FILTER_FROSTED_GLASS_EDLANGLES[] = "FrostedGlass_EdLightAngles"; // AngleDeg, featherDeg
constexpr char GE_FILTER_FROSTED_GLASS_EDLDIR[] = "FrostedGlass_EdLightDir"; // x, y
constexpr char GE_FILTER_FROSTED_GLASS_EDLRATES[] = "FrostedGlass_EdLightRates";
constexpr char GE_FILTER_FROSTED_GLASS_EDLKBS[] = "FrostedGlass_EdLightKBS";
constexpr char GE_FILTER_FROSTED_GLASS_EDLPOS[] = "FrostedGlass_EdLightPos";
constexpr char GE_FILTER_FROSTED_GLASS_EDLNEG[] = "FrostedGlass_EdLightNeg";
constexpr char GE_FILTER_FROSTED_GLASS_BORDERSIZE[] = "FrostedGlass_BorderSize";
constexpr char GE_FILTER_FROSTED_GLASS_CORNERRADIUS[] = "FrostedGlass_CornerRadius";
constexpr char GE_FILTER_FROSTED_GLASS_SHAPE[] = "FrostedGlass_Shape";
// Switch params
constexpr char GE_FILTER_FROSTED_GLASS_BASEVIBRANCYENABLED[] = "FrostedGlass_BaseVibrancyEnabled";
constexpr char GE_FILTER_FROSTED_GLASS_BASEMATERIALTYPE[] = "FrostedGlass_BaseMaterialType";
constexpr char GE_FILTER_FROSTED_GLASS_MATERIALCOLOR[] = "FrostedGlass_MaterialColor";
constexpr char GE_FILTER_FROSTED_GLASS_REFRACTENABLED[] = "FrostedGlass_RefractEnabled";
constexpr char GE_FILTER_FROSTED_GLASS_INNERSHADOWENABLED[] = "FrostedGlass_InnerShadowEnabled";
constexpr char GE_FILTER_FROSTED_GLASS_ENVLIGHTENABLED[] = "FrostedGlass_EnvLightEnabled";
constexpr char GE_FILTER_FROSTED_GLASS_HIGHLIGHTENABLED[] = "FrostedGlass_HighLightEnabled";

// Dark mode parameters and scale
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_BLURPARAM[] = "FrostedGlass_DarkModeBlurParam";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_WEIGHTSEMBOSS[] = "FrostedGlass_DarkModeWeightsEmboss";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_BGRATES[] = "FrostedGlass_DarkModeBgRates";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_BGKBS[] = "FrostedGlass_DarkModeBgKBS";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_BGPOS[] = "FrostedGlass_DarkModeBgPos";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_BGNEG[] = "FrostedGlass_DarkModeBgNeg";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_EDLIGHTANGLES[] = "FrostedGlass_DarkModeEdLightAngles";
constexpr char GE_SHADER_FROSTED_GLASS_DARKMODE_EDLIGHTKBS[] = "FrostedGlass_DarkModeEdLightKBS";
constexpr char GE_SHADER_FROSTED_GLASS_DARK_SCALE[] = "FrostedGlass_DarkScale";

struct GEFrostedGlassShaderFilterParams {
    Vector2f blurParams = Vector2f(48.0f, 4.0f);
    Vector2f weightsEmboss = Vector2f(1.0f, 1.0f); // (envLight, sd)
    Vector2f weightsEdl = Vector2f(1.0f, 1.0f); // (envLight, sd)
    // Background darken parameters
    Vector2f bgRates = Vector2f(-0.00003f, 1.2f);
    Vector3f bgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
    Vector3f bgPos = Vector3f(0.3f, 0.5f, 1.0f);
    Vector3f bgNeg = Vector3f(0.5f, 0.5f, 1.0f);
    // Refraction params
    Vector3f refractParams = Vector3f(0.0f, 0.0f, 0.0f);
    // Inner shadow parameters
    Vector3f sdParams = Vector3f(0.0f, 2.0f, 2.0f);
    Vector2f sdRates = Vector2f(0.0f, 0.0f);
    Vector3f sdKBS = Vector3f(-0.02f, 2.0f, 4.62f);
    Vector3f sdPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f sdNeg = Vector3f(1.7f, 3.0f, 1.0f);
    // Env refraction parameters
    Vector3f envLightParams = Vector3f(0.8f, 0.2745f, 2.0f);
    Vector2f envLightRates = Vector2f(0.0f, 0.0f);
    Vector3f envLightKBS = Vector3f(0.8f, 0.2745f, 2.0f);
    Vector3f envLightPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f envLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
    // Edge highlights parameters
    Vector2f edLightParams = Vector2f(2.0f, -1.0f);
    Vector2f edLightAngles = Vector2f(30.0f, 30.0f);
    Vector2f edLightDir = Vector2f(-1.0f, 1.0f);
    Vector2f edLightRates = Vector2f(0.0f, 0.0f);
    Vector3f edLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
    Vector3f edLightPos = Vector3f(1.0f, 1.5f, 2.0f);
    Vector3f edLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
    Vector2f borderSize = Vector2f(200.0f, 100.0f);
    float cornerRadius = 100.0f;
    std::shared_ptr<GESDFShaderShape> sdfShape;
    bool baseVibrancyEnabled = true;
    float baseMaterialType = 0.0f;
    bool refractEnabled = true;
    bool innerShadowEnabled = true;
    bool envLightEnabled = true;
    bool highLightEnabled = true;
    Vector4f materialColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    // Dark mode parameters and scale
    Vector2f darkModeBlurParam = Vector2f(48.0f, 4.0f);
    Vector2f darkModeWeightsEmboss = Vector2f(1.0f, 1.0f);
    Vector2f darkModeBgRates = Vector2f(-0.00003f, 1.2f);
    Vector3f darkModeBgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
    Vector3f darkModeBgPos = Vector3f(0.3f, 0.5f, 1.0f);
    Vector3f darkModeBgNeg = Vector3f(0.5f, 0.5f, 1.0f);
    Vector3f darkModeEdLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
    Vector2f darkModeEdLightAngles = Vector2f(30.0f, 30.0f);
    float darkScale = 0.0f;
};
REGISTER_GEFILTERPARAM_TYPEINFO(FROSTED_GLASS, GEFrostedGlassShaderFilterParams, GE_FILTER_FROSTED_GLASS);

constexpr char GEX_MASK_NOISY_FRAME_GRADIENT[] = "NoisyFrameGradientMask";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_GRADIENT_BEZIER_COMTROL_POINTS[] =
    "NoisyFrameGradientMask_GradientBezierControlPoints";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_CORNER_RADIUS[] = "NoisyFrameGradientMask_CornerRadius";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_INNER_FRAME_WIDTH[] = "NoisyFrameGradientMask_InnerFrameWidth";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_MIDDLE_FRAME_WIDTH[] = "NoisyFrameGradientMask_MiddleFrameWidth";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_OUTSIDE_FRAME_WIDTH[] = "NoisyFrameGradientMask_OutsideFrameWidth";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_RRECT_WH[] = "NoisyFrameGradientMask_RRectWH";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_RRECT_POS[] = "NoisyFrameGradientMask_RRectPos";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_SLOPE[] = "NoisyFrameGradientMask_Slope";
constexpr char GEX_MASK_NOISY_FRAME_GRADIENT_PROGRESS[] = "NoisyFrameGradientMask_Progress";
struct GEXNoisyFrameGradientMaskParams {
    Vector4f gradientBezierControlPoints;
    float cornerRadius = 0.0f;
    std::pair<float, float> innerFrameWidth;
    std::pair<float, float> middleFrameWidth;
    std::pair<float, float> outsideFrameWidth;
    std::pair<float, float> RRectWH;
    std::pair<float, float> RRectPos;
    float slope;
    float progress;
};
REGISTER_GEFILTERPARAM_TYPEINFO(NOISY_FRAME_GRADIENT_MASK, GEXNoisyFrameGradientMaskParams,
                                GEX_MASK_NOISY_FRAME_GRADIENT);
#undef REGISTER_GEFILTERPARAM_TYPEINFO
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
