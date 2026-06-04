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
#include "gevisualeffecttest2_fuzzer.h"
#include "ge_visual_effect.h"
#include "ge_shader_filter_params.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr size_t STR_LEN = 32;
constexpr size_t VEC_SIZE_MAX = 4;
constexpr uint8_t FUZZER_TEST_CASE_COUNT = 29;

enum TestCaseIndex {
    TEST_CASE_RAND_PARAM_KAWASE_BLUR = 0,
    TEST_CASE_RAND_PARAM_MESA_BLUR = 1,
    TEST_CASE_RAND_PARAM_GREY = 2,
    TEST_CASE_RAND_PARAM_AI_BAR = 3,
    TEST_CASE_RAND_PARAM_LINEAR_GRADIENT_BLUR = 4,
    TEST_CASE_RAND_PARAM_WATER_RIPPLE = 5,
    TEST_CASE_RAND_PARAM_EDGE_LIGHT = 6,
    TEST_CASE_RAND_PARAM_CONTENT_LIGHT = 7,
    TEST_CASE_RAND_PARAM_DISPERSION = 8,
    TEST_CASE_RAND_PARAM_MAGNIFIER = 9,
    TEST_CASE_RAND_PARAM_SOUND_WAVE = 10,
    TEST_CASE_RAND_PARAM_SDF_EDGE_LIGHT = 11,
    TEST_CASE_RAND_PARAM_DIRECTION_LIGHT = 12,
    TEST_CASE_RAND_PARAM_MASK_TRANSITION = 13,
    TEST_CASE_RAND_PARAM_COLOR_GRADIENT = 14,
    TEST_CASE_RAND_PARAM_HEAT_DISTORTION = 15,
    TEST_CASE_RAND_PARAM_BLUR_BUBBLES_RISE = 16,
    TEST_CASE_RAND_PARAM_VARIABLE_RADIUS_BLUR = 17,
    TEST_CASE_RAND_PARAM_FROSTED_GLASS = 18,
    TEST_CASE_RAND_PARAM_BORDER_LIGHT = 19,
    TEST_CASE_RAND_PARAM_RIPPLE_MASK = 20,
    TEST_CASE_RAND_PARAM_DOUBLE_RIPPLE_MASK = 21,
    TEST_CASE_RAND_PARAM_RADIAL_GRADIENT_MASK = 22,
    TEST_CASE_RAND_PARAM_WAVE_GRADIENT_MASK = 23,
    TEST_CASE_RAND_PARAM_LINEAR_GRADIENT_MASK = 24,
    TEST_CASE_RAND_PARAM_IMAGE_MASK = 25,
    TEST_CASE_RAND_PARAM_PIXEL_MAP_MASK = 26,
    TEST_CASE_RAND_PARAM_LIGHT_CAVE = 27,
    TEST_CASE_RAND_PARAM_BEZIER_WARP = 28,
};
}

void FuzzTestRandParamKawaseBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeIntegral<int32_t>());
        } else if (typeChoice == 1) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamMesaBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeIntegral<int32_t>());
        } else if (typeChoice == 1) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamGrey(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamAIBar(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_AI_BAR);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamLinearGradientBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_LINEAR_GRADIENT_BLUR);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeIntegral<int32_t>());
        } else if (typeChoice == 1) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamWaterRipple(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_WATER_RIPPLE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeIntegral<int32_t>());
        } else if (typeChoice == 1) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamEdgeLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_EDGE_LIGHT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamContentLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_CONTENT_LIGHT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamDispersion(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_DISPERSION);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamMagnifier(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MAGNIFIER);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeIntegral<int32_t>());
        } else if (typeChoice == 1) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamSoundWave(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_SOUND_WAVE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamSDFEdgeLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_SDF_EDGE_LIGHT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamDirectionLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_DIRECTION_LIGHT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamMaskTransition(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_MASK_TRANSITION);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamColorGradient(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_COLOR_GRADIENT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamHeatDistortion(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_HEAT_DISTORTION);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamBlurBubblesRise(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_BLUR_BUBBLES_RISE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamVariableRadiusBlur(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_VARIABLE_RADIUS_BLUR);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        uint8_t typeChoice = fdp.ConsumeIntegral<uint8_t>() % 3;
        if (typeChoice == 0) {
            effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
        } else {
            effect->SetParam(randomTag, fdp.ConsumeBool());
        }
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamFrostedGlass(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamBorderLight(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_SHADER_BORDER_LIGHT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamRippleMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_RIPPLE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamDoubleRippleMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_DOUBLE_RIPPLE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamRadialGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_RADIAL_GRADIENT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamWaveGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_WAVE_GRADIENT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamLinearGradientMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_LINEAR_GRADIENT);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamImageMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_IMAGE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamPixelMapMask(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_MASK_PIXEL_MAP);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamLightCave(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GEX_SHADER_LIGHT_CAVE);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
    effect->GetName();
    effect->GetImpl();
}

void FuzzTestRandParamBezierWarp(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<GEVisualEffect>(GE_FILTER_BEZIER_WARP);
    size_t paramCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= paramCount; i++) {
        std::string randomTag = fdp.ConsumeRandomLengthString(STR_LEN);
        effect->SetParam(randomTag, fdp.ConsumeFloatingPoint<float>());
    }
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
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_KAWASE_BLUR:
            OHOS::Rosen::Drawing::FuzzTestRandParamKawaseBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_MESA_BLUR:
            OHOS::Rosen::Drawing::FuzzTestRandParamMesaBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_GREY:
            OHOS::Rosen::Drawing::FuzzTestRandParamGrey(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_AI_BAR:
            OHOS::Rosen::Drawing::FuzzTestRandParamAIBar(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_LINEAR_GRADIENT_BLUR:
            OHOS::Rosen::Drawing::FuzzTestRandParamLinearGradientBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_WATER_RIPPLE:
            OHOS::Rosen::Drawing::FuzzTestRandParamWaterRipple(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_EDGE_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestRandParamEdgeLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_CONTENT_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestRandParamContentLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_DISPERSION:
            OHOS::Rosen::Drawing::FuzzTestRandParamDispersion(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_MAGNIFIER:
            OHOS::Rosen::Drawing::FuzzTestRandParamMagnifier(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_SOUND_WAVE:
            OHOS::Rosen::Drawing::FuzzTestRandParamSoundWave(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_SDF_EDGE_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestRandParamSDFEdgeLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_DIRECTION_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestRandParamDirectionLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_MASK_TRANSITION:
            OHOS::Rosen::Drawing::FuzzTestRandParamMaskTransition(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_COLOR_GRADIENT:
            OHOS::Rosen::Drawing::FuzzTestRandParamColorGradient(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_HEAT_DISTORTION:
            OHOS::Rosen::Drawing::FuzzTestRandParamHeatDistortion(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_BLUR_BUBBLES_RISE:
            OHOS::Rosen::Drawing::FuzzTestRandParamBlurBubblesRise(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_VARIABLE_RADIUS_BLUR:
            OHOS::Rosen::Drawing::FuzzTestRandParamVariableRadiusBlur(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_FROSTED_GLASS:
            OHOS::Rosen::Drawing::FuzzTestRandParamFrostedGlass(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_BORDER_LIGHT:
            OHOS::Rosen::Drawing::FuzzTestRandParamBorderLight(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_RIPPLE_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamRippleMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_DOUBLE_RIPPLE_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamDoubleRippleMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_RADIAL_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamRadialGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_WAVE_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamWaveGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_LINEAR_GRADIENT_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamLinearGradientMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_IMAGE_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamImageMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_PIXEL_MAP_MASK:
            OHOS::Rosen::Drawing::FuzzTestRandParamPixelMapMask(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_LIGHT_CAVE:
            OHOS::Rosen::Drawing::FuzzTestRandParamLightCave(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RAND_PARAM_BEZIER_WARP:
            OHOS::Rosen::Drawing::FuzzTestRandParamBezierWarp(fdp);
            break;
        default:
            break;
    }
    return 0;
}