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

#include <cstring>
#include <memory>
#include "gevisualeffecttest1_fuzzer.h"
#include "ge_visual_effect.h"
#include "ge_shader_filter_params.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr uint8_t FUZZER_TEST_CASE_COUNT = 29;

enum TestCaseIndex {
    TEST_CASE_KAWASE_BLUR = 0,
    TEST_CASE_MESA_BLUR = 1,
    TEST_CASE_GREY = 2,
    TEST_CASE_AI_BAR = 3,
    TEST_CASE_LINEAR_GRADIENT_BLUR = 4,
    TEST_CASE_WATER_RIPPLE = 5,
    TEST_CASE_EDGE_LIGHT = 6,
    TEST_CASE_CONTENT_LIGHT = 7,
    TEST_CASE_DISPERSION = 8,
    TEST_CASE_MAGNIFIER = 9,
    TEST_CASE_SOUND_WAVE = 10,
    TEST_CASE_SDF_EDGE_LIGHT = 11,
    TEST_CASE_DIRECTION_LIGHT = 12,
    TEST_CASE_MASK_TRANSITION = 13,
    TEST_CASE_COLOR_GRADIENT = 14,
    TEST_CASE_HEAT_DISTORTION = 15,
    TEST_CASE_BLUR_BUBBLES_RISE = 16,
    TEST_CASE_VARIABLE_RADIUS_BLUR = 17,
    TEST_CASE_FROSTED_GLASS = 18,
    TEST_CASE_BORDER_LIGHT = 19,
    TEST_CASE_RIPPLE_MASK = 20,
    TEST_CASE_DOUBLE_RIPPLE_MASK = 21,
    TEST_CASE_RADIAL_GRADIENT_MASK = 22,
    TEST_CASE_WAVE_GRADIENT_MASK = 23,
    TEST_CASE_LINEAR_GRADIENT_MASK = 24,
    TEST_CASE_IMAGE_MASK = 25,
    TEST_CASE_PIXEL_MAP_MASK = 26,
    TEST_CASE_LIGHT_CAVE = 27,
    TEST_CASE_BEZIER_WARP = 28,
};
}

void FuzzTestKawaseBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    effect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, fdp.ConsumeIntegral<int32_t>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestMesaBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    effect->SetParam(GE_FILTER_MESA_BLUR_RADIUS, fdp.ConsumeIntegral<int32_t>());
    effect->SetParam(GE_FILTER_MESA_BLUR_GREY_COEF_1, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_GREY_COEF_2, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, fdp.ConsumeIntegral<int32_t>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MESA_BLUR_IS_DIRECTION, fdp.ConsumeBool());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestGrey(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);
    effect->SetParam(GE_FILTER_GREY_COEF_1, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_GREY_COEF_2, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestAIBar(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_AI_BAR);
    effect->SetParam(GE_FILTER_AI_BAR_LOW, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_AI_BAR_HIGH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_AI_BAR_THRESHOLD, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_AI_BAR_OPACITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_AI_BAR_SATURATION, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestLinearGradientBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_LINEAR_GRADIENT_BLUR);
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, fdp.ConsumeIntegral<int32_t>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN, fdp.ConsumeBool());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestWaterRipple(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_WATER_RIPPLE);
    effect->SetParam(GE_FILTER_WATER_RIPPLE_PROGRESS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_WATER_RIPPLE_WAVE_NUM, fdp.ConsumeIntegral<int32_t>());
    effect->SetParam(GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, fdp.ConsumeIntegral<int32_t>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestEdgeLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_EDGE_LIGHT);
    effect->SetParam(GE_FILTER_EDGE_LIGHT_ALPHA, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_EDGE_LIGHT_BLOOM, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR, fdp.ConsumeBool());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestContentLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_CONTENT_LIGHT);
    effect->SetParam(GE_FILTER_CONTENT_LIGHT_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestDispersion(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_DISPERSION);
    effect->SetParam(GE_FILTER_DISPERSION_OPACITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_DISPERSION_RED_OFFSET, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_DISPERSION_GREEN_OFFSET, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_DISPERSION_BLUE_OFFSET, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestMagnifier(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MAGNIFIER);
    effect->SetParam(GE_FILTER_MAGNIFIER_FACTOR, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_HEIGHT, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_CORNER_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_BORDER_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_ZOOM_OFFSET_X, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_ZOOM_OFFSET_Y, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_SHADOW_SIZE, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_SHADOW_STRENGTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MAGNIFIER_ROTATE_DEGREE, fdp.ConsumeIntegral<int32_t>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestSoundWave(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_SOUND_WAVE);
    effect->SetParam(GE_FILTER_SOUND_WAVE_SOUNDINTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SOUND_WAVE_COLORPROGRESS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SOUND_WAVE_TOTAL_ALPHA, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestSDFEdgeLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_SDF_EDGE_LIGHT);
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_SDF_SPREAD_FACTOR, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_BLOOM_INTENSITY_CUTOFF, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_MAX_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_MAX_BLOOM_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_MIN_BORDER_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_SDF_EDGE_LIGHT_MAX_BORDER_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestDirectionLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_DIRECTION_LIGHT);
    effect->SetParam(GE_FILTER_DIRECTION_LIGHT_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestMaskTransition(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MASK_TRANSITION);
    effect->SetParam(GE_FILTER_MASK_TRANSITION_FACTOR, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_MASK_TRANSITION_INVERSE, fdp.ConsumeBool());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestColorGradient(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_COLOR_GRADIENT);
    effect->SetParam(GE_FILTER_COLOR_GRADIENT_STRENGTH, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestHeatDistortion(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_HEAT_DISTORTION);
    effect->SetParam(GE_FILTER_HEAT_DISTORTION_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_HEAT_DISTORTION_NOISE_SCALE, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_HEAT_DISTORTION_RISE_WEIGHT, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_HEAT_DISTORTION_PROGRESS, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestBlurBubblesRise(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_BLUR_BUBBLES_RISE);
    effect->SetParam(GE_FILTER_BLUR_BUBBLES_RISE_BLUR_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_BLUR_BUBBLES_RISE_MIX_STRENGTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_BLUR_BUBBLES_RISE_PROGRESS, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestVariableRadiusBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_VARIABLE_RADIUS_BLUR);
    effect->SetParam(GE_FILTER_VARIABLE_RADIUS_BLUR_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_FILTER_VARIABLE_RADIUS_BLUR_APPLYINSIDEMASK, fdp.ConsumeBool());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestFrostedGlass(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    effect->SetParam(GE_FILTER_FROSTED_GLASS_BLURPARAMS, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestBorderLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_SHADER_BORDER_LIGHT);
    effect->SetParam(GE_SHADER_BORDER_LIGHT_INTENSITY, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_SHADER_BORDER_LIGHT_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_SHADER_BORDER_LIGHT_CORNER_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_SHADER_BORDER_LIGHT_ROTATION_ANGLE, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRippleMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_RIPPLE);
    effect->SetParam(GE_MASK_RIPPLE_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_RIPPLE_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestDoubleRippleMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_DOUBLE_RIPPLE);
    effect->SetParam(GE_MASK_DOUBLE_RIPPLE_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_DOUBLE_RIPPLE_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRadialGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_RADIAL_GRADIENT);
    effect->SetParam(GE_MASK_RADIAL_GRADIENT_RADIUSX, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_RADIAL_GRADIENT_RADIUSY, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestWaveGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_WAVE_GRADIENT);
    effect->SetParam(GE_MASK_WAVE_GRADIENT_WIDTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_WAVE_GRADIENT_BLUR_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_WAVE_GRADIENT_TURBULENCE_STRENGTH, fdp.ConsumeFloatingPoint<float>());
    effect->SetParam(GE_MASK_WAVE_GRADIENT_PROPAGATION_RADIUS, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestLinearGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_LINEAR_GRADIENT);
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestImageMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_IMAGE);
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestPixelMapMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_PIXEL_MAP);
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestLightCave(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GEX_SHADER_LIGHT_CAVE);
    effect->SetParam(GEX_SHADER_LIGHT_CAVE_PROGRESS, fdp.ConsumeFloatingPoint<float>());
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestBezierWarp(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_BEZIER_WARP);
    effect->GetName();
    effect->GetImpl();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);

    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::Drawing::FUZZER_TEST_CASE_COUNT;
    switch (choice) {
        case OHOS::Rosen::Drawing::TEST_CASE_KAWASE_BLUR:
            OHOS::Rosen::Drawing::FuzzTestKawaseBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_MESA_BLUR:
            OHOS::Rosen::Drawing::FuzzTestMesaBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_GREY:
            OHOS::Rosen::Drawing::FuzzTestGrey(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_AI_BAR:
            OHOS::Rosen::Drawing::FuzzTestAIBar(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_LINEAR_GRADIENT_BLUR:
            OHOS::Rosen::Drawing::FuzzTestLinearGradientBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_WATER_RIPPLE:
            OHOS::Rosen::Drawing::FuzzTestWaterRipple(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_EDGE_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestEdgeLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_CONTENT_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestContentLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_DISPERSION:
            OHOS::Rosen::Drawing::FuzzTestDispersion(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_MAGNIFIER:
            OHOS::Rosen::Drawing::FuzzTestMagnifier(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_SOUND_WAVE:
            OHOS::Rosen::Drawing::FuzzTestSoundWave(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_SDF_EDGE_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestSDFEdgeLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_DIRECTION_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestDirectionLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_MASK_TRANSITION:
            OHOS::Rosen::Drawing::FuzzTestMaskTransition(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_COLOR_GRADIENT:
            OHOS::Rosen::Drawing::FuzzTestColorGradient(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_HEAT_DISTORTION:
            OHOS::Rosen::Drawing::FuzzTestHeatDistortion(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_BLUR_BUBBLES_RISE:
            OHOS::Rosen::Drawing::FuzzTestBlurBubblesRise(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_VARIABLE_RADIUS_BLUR:
            OHOS::Rosen::Drawing::FuzzTestVariableRadiusBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_FROSTED_GLASS:
            OHOS::Rosen::Drawing::FuzzTestFrostedGlass(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_BORDER_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestBorderLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RIPPLE_MASK:
            OHOS::Rosen::Drawing::FuzzTestRippleMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_DOUBLE_RIPPLE_MASK:
            OHOS::Rosen::Drawing::FuzzTestDoubleRippleMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RADIAL_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestRadialGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_WAVE_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestWaveGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_LINEAR_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestLinearGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_IMAGE_MASK:
            OHOS::Rosen::Drawing::FuzzTestImageMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_PIXEL_MAP_MASK:
            OHOS::Rosen::Drawing::FuzzTestPixelMapMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_LIGHT_CAVE:
            OHOS::Rosen::Drawing::FuzzTestLightCave(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_BEZIER_WARP:
            OHOS::Rosen::Drawing::FuzzTestBezierWarp(fdp);
            break;
        default:
            break;
    }
    return 0;
}