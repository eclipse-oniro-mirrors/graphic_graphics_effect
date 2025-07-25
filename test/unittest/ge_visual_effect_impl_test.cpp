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

#include <gtest/gtest.h>

#include "ge_double_ripple_shader_mask.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "ge_visual_effect_impl.h"
#include "ge_wave_gradient_shader_mask.h"
#include "utils/rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEVisualEffectImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectImplTest::SetUpTestCase(void) {}

void GEVisualEffectImplTest::TearDownTestCase(void) {}

void GEVisualEffectImplTest::SetUp() {}

void GEVisualEffectImplTest::TearDown() {}

/**
 * @tc.name: GetFilterType_001
 * @tc.desc: Verify function GetFilterType
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetFilterType_001, TestSize.Level2)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(geVisualEffectImpl1.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GREY);
    EXPECT_EQ(geVisualEffectImpl2.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::GREY);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_AI_BAR);
    EXPECT_EQ(geVisualEffectImpl3.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::AIBAR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl4(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(geVisualEffectImpl4.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImplMESA(Drawing::GE_FILTER_MESA_BLUR);
    EXPECT_EQ(geVisualEffectImplMESA.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);

    Drawing::GEVisualEffectImpl geVisualEffectImplSound(Drawing::GE_FILTER_SOUND_WAVE);
    EXPECT_EQ(geVisualEffectImplSound.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::SOUND_WAVE);

    Drawing::GEVisualEffectImpl geVisualEffectImplEdgeLight(Drawing::GE_FILTER_EDGE_LIGHT);
    EXPECT_EQ(geVisualEffectImplEdgeLight.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT);

    Drawing::GEVisualEffectImpl geVisualEffectImplDispersion(Drawing::GE_FILTER_DISPERSION);
    EXPECT_EQ(geVisualEffectImplDispersion.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::DISPERSION);
}

/**
 * @tc.name: SetParam_001
 * @tc.desc: Verify function SetParam
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_001, TestSize.Level2)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_KAWASE_BLUR);

    // 1 set for Kawase blur RADIUS, linear gradient blur DIRECTION
    int32_t paramInt32 { 1 };
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, paramInt32);
    ASSERT_NE(geVisualEffectImpl1.GetKawaseParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl1.GetKawaseParams()->radius, paramInt32);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, paramInt32);
    ASSERT_NE(geVisualEffectImpl2.GetLinearGradientBlurParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->direction, paramInt32);

    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN, true);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->isOffscreenCanvas, true);

    // 1.f is linear gradient blur params：blurRadius,geoWidth,geoHeight,tranX,tranY
    float paramfloat { 1.f };
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->blurRadius, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->geoWidth, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->geoHeight, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->tranX, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->tranY, paramfloat);

    auto image = std::make_shared<Drawing::Image>();
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    Drawing::Matrix mat;
    int64_t paramInt64 { 1 }; // 1 is linear gradient blur params：CANVAS_MAT
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, paramInt64);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, image);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, colorFilter);

    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, mat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->mat, mat);

    // 0.1f, 0.1f is linear gradient blur params: FRACTION_STOPS
    std::vector<std::pair<float, float>> expectFraStops { { 0.1f, 0.1f } };
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS, expectFraStops);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->fractionStops, expectFraStops);
}

/**
 * @tc.name: SetParam_002
 * @tc.desc: Verify function SetParam
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_002, TestSize.Level1)
{
    // 1.0f is params of AI Bar blur, Grey blur
    float paramfloat { 1.0f };
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_AI_BAR);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_LOW, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_HIGH, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_THRESHOLD, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_OPACITY, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_SATURATION, paramfloat);
    ASSERT_NE(geVisualEffectImpl1.GetAIBarParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarLow, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarHigh, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarThreshold, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarOpacity, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarSaturation, paramfloat);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GREY);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_GREY_COEF_1, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_GREY_COEF_2, paramfloat);
    ASSERT_NE(geVisualEffectImpl2.GetGreyParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl2.GetGreyParams()->greyCoef1, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetGreyParams()->greyCoef2, paramfloat);
}

/**
 * @tc.name: SetParam_003
 * @tc.desc: Verify function SetParam  no filter type
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_003, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params
    geVisualEffectImpl.SetParam("", false);
    geVisualEffectImpl.SetParam("", 1.0f); // 1.0f invalid params
    geVisualEffectImpl.SetParam("", 1.0f); // 1.0f invalid params
    Drawing::Matrix blurMat;
    geVisualEffectImpl.SetParam("", blurMat);
    std::vector<std::pair<float, float>> blurFractionStops;
    geVisualEffectImpl.SetParam("", blurFractionStops);
    geVisualEffectImpl.SetAIBarParams("", 1.0f);              // 1.0f invalid params
    geVisualEffectImpl.SetGreyParams("", 1.0f);               // 1.0f invalid params
    geVisualEffectImpl.SetLinearGradientBlurParams("", 1.0f); // 1.0f invalid params
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::NONE);
}

/**
 * @tc.name: SetParam_004
 * @tc.desc: Verify function SetParam for param is nullptr when filtertype is NONE
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_004, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params
    geVisualEffectImpl.SetParam("", false);
    Drawing::Matrix blurMat;
    geVisualEffectImpl.SetParam("", blurMat);
    std::vector<std::pair<float, float>> blurFractionStops;
    geVisualEffectImpl.SetParam("", blurFractionStops);

    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
}

/**
 * @tc.name: SetParam_005
 * @tc.desc: Verify function SetParam for tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_005, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 2); // 2 blur radius
    EXPECT_EQ(geVisualEffectImpl.GetKawaseParams()->radius, 2);
    geVisualEffectImpl.SetParam("", 3); // 3 blur radius, but invalid
    EXPECT_NE(geVisualEffectImpl.GetKawaseParams()->radius, 3);
}

/**
 * @tc.name: SetParam_006
 * @tc.desc: Verify function SetParam for tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_006, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 2); // 2 blur direction
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->direction, 2);
    geVisualEffectImpl.SetParam("", 3); // 3 blur direction, but invalid
    EXPECT_NE(geVisualEffectImpl.GetLinearGradientBlurParams()->direction, 3);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN, true);
    EXPECT_TRUE(geVisualEffectImpl.GetLinearGradientBlurParams()->isOffscreenCanvas);
    geVisualEffectImpl.SetParam("", false);
    EXPECT_TRUE(geVisualEffectImpl.GetLinearGradientBlurParams()->isOffscreenCanvas);

    Drawing::Matrix blurMat;
    blurMat.Set(Drawing::Matrix::SKEW_X, 0.002f); // 0.002f skew x
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, blurMat);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->mat, blurMat);
    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.005f); // 0.005f skew x
    geVisualEffectImpl.SetParam("", mat);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->mat, blurMat);

    std::vector<std::pair<float, float>> blurFractionStops { { 0.1f, 0.1f } };
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS, blurFractionStops);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->fractionStops, blurFractionStops);
    std::vector<std::pair<float, float>> expectFractionStops { { 0.2f, 0.2f } };
    geVisualEffectImpl.SetParam("", expectFractionStops);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->fractionStops, blurFractionStops);
}

/**
 * @tc.name: SetParam_007
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_007, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_AI_BAR);
    geVisualEffectImpl.SetAIBarParams(Drawing::GE_FILTER_AI_BAR_LOW, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetAIBarParams()->aiBarLow, 1.0f);
    geVisualEffectImpl.SetAIBarParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetAIBarParams()->aiBarLow, 2.0f);
}

/**
 * @tc.name: SetParam_008
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_008, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_GREY);
    geVisualEffectImpl.SetGreyParams(Drawing::GE_FILTER_GREY_COEF_1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetGreyParams()->greyCoef1, 1.0f);
    geVisualEffectImpl.SetGreyParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetGreyParams()->greyCoef1, 2.0f);
}

/**
 * @tc.name: SetParam_009
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_009, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetLinearGradientBlurParams(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->blurRadius, 1.0f);
    geVisualEffectImpl.SetLinearGradientBlurParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetLinearGradientBlurParams()->blurRadius, 2.0f);
}

/**
 * @tc.name: SetParam_010
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_010, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleMode, 1.0f);
}

/**
 * @tc.name: SetParam_011
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_011, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplWaterRipple(Drawing::GE_FILTER_WATER_RIPPLE);
    Drawing::GEVisualEffectImpl geVisualEffectImplKawaseBulr(Drawing::GE_FILTER_KAWASE_BLUR);

    // test invalid params setting
    geVisualEffectImplWaterRipple.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5);
    EXPECT_NE(geVisualEffectImplWaterRipple.GetWaterRippleParams()->progress, 0.5);

    // test invalid params setting
    uint32_t paramUint32 { 1 };
    geVisualEffectImplKawaseBulr.SetParam("GE_FILTER_KAWASE_BLUR_RADIUS", paramUint32);
    ASSERT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams(), nullptr);
    EXPECT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams()->radius, paramUint32);

    geVisualEffectImplKawaseBulr.SetParam("GE_FILTER_KAWASE_BLUR_RADIUS", "1");
    ASSERT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams(), nullptr);
    EXPECT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams()->radius, paramUint32);
}

/**
 * @tc.name: SetSoundWaveParamsFloat001
 * @tc.desc: Verify function SetSoundWaveParamsFloat
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSoundWaveParamsFloat001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave(Drawing::GE_FILTER_SOUND_WAVE);
    float colorProgress = 0.5f;
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, colorProgress);
    EXPECT_NE(geVisualEffectImplSoundWave.GetSoundWaveParams(), nullptr);

    // test valid sound wave params setting
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->colorProgress, 0.5f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_SOUNDINTENSITY, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->soundIntensity, 1.0f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A, 0.6f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveAlphaA, 0.6f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveAlphaB, 1.0f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveProgressA, 1.0f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveProgressB, 1.0f);
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_TOTAL_ALPHA, 0.5f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveTotalAlpha, 0.5f);
}

/**
 * @tc.name: SetSoundWaveParam001
 * @tc.desc: Verify function SetSoundWaveParamUint32 and SetSoundWaveParamFloat
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSoundWaveParam001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave("");
    float param = 1.2f;

    // test invalid params setting
    geVisualEffectImplSoundWave.SetSoundWaveParamsFloat(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, param);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave1(Drawing::GE_FILTER_SOUND_WAVE);
    EXPECT_NE(geVisualEffectImplSoundWave1.GetSoundWaveParams(), nullptr);

    geVisualEffectImplSoundWave1.SetSoundWaveParamsFloat("", param);
    EXPECT_NE(geVisualEffectImplSoundWave1.GetSoundWaveParams()->shockWaveProgressB, param);
}

/**
 * @tc.name: SetParam_013
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_013, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplRippleMask(Drawing::GE_MASK_RIPPLE);
    geVisualEffectImplRippleMask.filterType_ = Drawing::GEVisualEffectImpl::FilterType::RIPPLE_MASK;
    geVisualEffectImplRippleMask.rippleMaskParams_ = std::make_shared<Drawing::GERippleShaderMaskParams>();

    // test invalid params setting
    float rippleRadius = 0.5f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Radius", rippleRadius);
    EXPECT_EQ(geVisualEffectImplRippleMask.rippleMaskParams_->radius_, rippleRadius);

    float rippleWidth = 0.6f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Width", rippleWidth);
    EXPECT_EQ(geVisualEffectImplRippleMask.rippleMaskParams_->width_, rippleWidth);

    float centerOffset = 0.7f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Offset", centerOffset);
    EXPECT_EQ(geVisualEffectImplRippleMask.rippleMaskParams_->widthCenterOffset_, centerOffset);
}

/**
 * @tc.name: SetParam_014
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_014, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplRippleMask(Drawing::GE_MASK_RIPPLE);
    geVisualEffectImplRippleMask.filterType_ = Drawing::GEVisualEffectImpl::FilterType::RIPPLE_MASK;
    geVisualEffectImplRippleMask.rippleMaskParams_ = std::make_shared<Drawing::GERippleShaderMaskParams>();

    std::pair<float, float> center = {0.5f, 0.5f};
    geVisualEffectImplRippleMask.SetParam("RippleMask_Center", center);
    EXPECT_EQ(geVisualEffectImplRippleMask.rippleMaskParams_->center_, center);


    std::pair<float, float> factor = {0.5f, 0.5f};
    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.filterType_ =
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER;
    geVisualEffectImplDisplaceDistort.displacementDistortParams_ =
        std::make_shared<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Factor", factor);
    EXPECT_EQ(geVisualEffectImplDisplaceDistort.displacementDistortParams_->factor_, factor);
}

/**
 * @tc.name: SetParam_015
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_015, TestSize.Level1)
{
    Drawing::GERippleShaderMaskParams param;
    auto geRippleShaderMask = std::make_shared<Drawing::GERippleShaderMask>(param);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geRippleShaderMask);
    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.filterType_ = 
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER;
    geVisualEffectImplDisplaceDistort.displacementDistortParams_ =
        std::make_shared<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(geVisualEffectImplDisplaceDistort.displacementDistortParams_->mask_, shaderMask);
}

/**
 * @tc.name: SetParam_016
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_016, TestSize.Level1)
{
    constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points = {{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {11.0f, 12.0f},
        {13.0f, 14.0f},
        {15.0f, 16.0f},
        {17.0f, 18.0f},
        {19.0f, 20.0f},
        {21.0f, 22.0f},
        {23.0f, 24.0f}
    }};
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_BEZIER_WARP);
    geVisualEffectImpl.MakeBezierWarpParams();
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_BEZIER_WARP_DESTINATION_PATCH, points);
    EXPECT_EQ(geVisualEffectImpl.bezierWarpParams_->destinationPatch, points);
}

/**
 * @tc.name: SetParamEdgelight_001
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParamEdgelight_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_EDGE_LIGHT);

    geVisualEffectImpl.SetEdgeLightParams(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->alpha, 0.5f);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_BLOOM, true);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->bloom, true);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_BLOOM, false);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->bloom, false);

    Vector4f color = Vector4f{0.2f, 0.7f, 0.1f, 0.0f};
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_COLOR, color);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->color, color);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR, true);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->useRawColor, true);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR, false);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->useRawColor, false);

    Drawing::GERadialGradientShaderMaskParams param;
    auto geRadialGradientShaderMask = std::make_shared<Drawing::GERadialGradientShaderMask>(param);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geRadialGradientShaderMask);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_MASK, shaderMask);
    EXPECT_EQ(geVisualEffectImpl.GetEdgeLightParams()->mask, shaderMask);
}

/**
 * @tc.name: SetParamBezierWarpTest_001
 * @tc.desc: Verify function SetParam for action is valid, bezier filter adapt ng filter
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParamBezierWarpTest_001, TestSize.Level1)
{
    constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points = {{
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f}
    }};
    Drawing::GEVisualEffectImpl geVisualEffectImplTest(Drawing::GE_FILTER_EDGE_LIGHT);
    EXPECT_EQ(geVisualEffectImplTest.bezierWarpParams_, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_BEZIER_WARP);
    geVisualEffectImpl.MakeBezierWarpParams();
    EXPECT_NE(geVisualEffectImpl.bezierWarpParams_, nullptr);

    geVisualEffectImpl.SetBezierWarpParams(Drawing::GE_FILTER_BEZIER_WARP_CONTROL_POINT0, std::make_pair(0.5f, 0.5f));
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetBezierWarpParams()->destinationPatch[0].GetX(), 0.5f);

    geVisualEffectImpl.SetBezierWarpParams(Drawing::GE_FILTER_BEZIER_WARP_CONTROL_POINT11, std::make_pair(1.f, 1.f));
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetBezierWarpParams()->destinationPatch[11].GetY(), 1.f);
}

/**
 * @tc.name: SetParamDispersion_001
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParamDispersion_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_DISPERSION);
    geVisualEffectImpl.SetDispersionParams(Drawing::GE_FILTER_DISPERSION_OPACITY, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->opacity, 0.5f);
    geVisualEffectImpl.SetDispersionParams(Drawing::GE_FILTER_DISPERSION_RED_OFFSET, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->redOffset, std::make_pair(0.5f, 0.5f));
    geVisualEffectImpl.SetDispersionParams(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->greenOffset, std::make_pair(0.5f, 0.5f));
    geVisualEffectImpl.SetDispersionParams(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->blueOffset, std::make_pair(0.5f, 0.5f));
}

/**
 * @tc.name: SetWaterRippleParams_001
 * @tc.desc: Verify function SetWaterRippleParams is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetWaterRippleParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    ASSERT_NE(geVisualEffectImpl.waterRippleParams_, nullptr);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleMode, 1.0f);
}

/**
 * @tc.name: SetWaterRippleParams_002
 * @tc.desc: Verify function SetWaterRippleParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetWaterRippleParams_002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);

    // test invalid params setting
    geVisualEffectImpl.SetWaterRippleParams("GE_FILTER_WATER_RIPPLE_PROGRESS", 0.5f);
    EXPECT_NE(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
}

/**
 * @tc.name: MakeWaterRippleParams_001
 * @tc.desc: Verify function MakeWaterRippleParams is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeWaterRippleParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    ASSERT_EQ(geVisualEffectImpl.GetWaterRippleParams(), nullptr);
    geVisualEffectImpl.MakeWaterRippleParams();
    ASSERT_NE(geVisualEffectImpl.GetWaterRippleParams(), nullptr);
}

/**
 * @tc.name: GetMESAParams_001
 * @tc.desc: Verify function GetMESAParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetMESAParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_MESA_BLUR);
    // 0.0f, 1, 1.0f: mesa blur params
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 1);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->radius, 1);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->greyCoef1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->greyCoef1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetX, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetY, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetZ, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetW, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->tileMode, 0);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->width, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->height, 0.0f);
}

/**
 * @tc.name: SetParam_012
 * @tc.desc: Verify function SetParam for param is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_012, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    int32_t paramInt32 { 0 }; // 0 invalid params
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER);
    geVisualEffectImpl.SetParam("", paramInt32);

    uint32_t paramUint32 { 0 }; // 0 invalid params
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    geVisualEffectImpl.SetParam("", paramUint32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
}

/**
 * @tc.name: SetParam_017
 * @tc.desc: Verify function SetParam for param is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_017, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplTest("");
    Drawing::Color4f colorTestA = {0.0, 1.0, 1.0, 1.0};
    Drawing::Color4f colorTestB = {1.0, 0.0, 1.0, 1.0};
    Drawing::Color4f colorTestC = {1.0, 1.0, 0.0, 1.0};
    geVisualEffectImplTest.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLOR_A, colorTestA);
    EXPECT_EQ(geVisualEffectImplTest.GetSoundWaveParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_SOUND_WAVE);
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::SOUND_WAVE);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLOR_A, colorTestA);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLOR_B, colorTestB);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLOR_C, colorTestC);
    EXPECT_EQ(geVisualEffectImpl.GetSoundWaveParams()->colorA.redF_, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetSoundWaveParams()->colorB.greenF_, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetSoundWaveParams()->colorC.blueF_, 0.0f);
}

/**
 * @tc.name: SetParam_020
 * @tc.desc: Verify function SetParam for param is content light
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_020, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplTest("");
    float lightIntensity = 0.5f;
    geVisualEffectImplTest.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_INTENSITY, lightIntensity);
    EXPECT_EQ(geVisualEffectImplTest.GetContentLightParams(), nullptr);
 
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_CONTENT_LIGHT);
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::CONTENT_LIGHT);
 
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_INTENSITY, lightIntensity);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->lightIntensity, lightIntensity);
 
    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_POSITION, lightPosition);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->lightPosition, lightPosition);
 
    Vector4f lightColor = Vector4f(0.3f, 0.6f, 0.9f, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_COLOR, lightColor);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->lightColor, lightColor);
}

/**
 * @tc.name: SetParam_021
 * @tc.desc: Verify function SetParam for pixelMapMask params
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_021, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplPixelMapMask(Drawing::GE_MASK_PIXEL_MAP);
    geVisualEffectImplPixelMapMask.filterType_ = Drawing::GEVisualEffectImpl::FilterType::PIXEL_MAP_MASK;
    geVisualEffectImplPixelMapMask.pixelMapMaskParams_ = std::make_shared<Drawing::GEPixelMapMaskParams>();

    Drawing::RectF srcRect = Drawing::RectF(0.0f, 0.0f, 100.0f, 100.0f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Src", srcRect);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.pixelMapMaskParams_->src, srcRect);

    Drawing::RectF dstRect = Drawing::RectF(0.0f, 0.0f, 200.0f, 200.0f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Dst", dstRect);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.pixelMapMaskParams_->dst, dstRect);

    Vector4f fillColor = Vector4f(1.0f, 0.5f, 0.2f, 0.8f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_FillColor", fillColor);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.pixelMapMaskParams_->fillColor, fillColor);

    auto testImage = std::make_shared<Drawing::Image>();
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Image", testImage);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.pixelMapMaskParams_->image, testImage);
}

/**
 * @tc.name: SetParam_022
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_022, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplWaveGradientMask(Drawing::GE_MASK_WAVE_GRADIENT);
    geVisualEffectImplWaveGradientMask.filterType_ = Drawing::GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK;
    geVisualEffectImplWaveGradientMask.waveGradientMaskParams_ =
        std::make_shared<Drawing::GEWaveGradientShaderMaskParams>();

    // test invalid params setting
    float width = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_WaveWidth", width);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.waveGradientMaskParams_->width_, width);

    float turbulenceStrength = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_TurbulenceStrength", turbulenceStrength);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.waveGradientMaskParams_->turbulenceStrength_, turbulenceStrength);

    float blurRadius = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_BlurRadius", blurRadius);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.waveGradientMaskParams_->blurRadius_, blurRadius);

    float propagationRadius = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_PropagationRadius", propagationRadius);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.waveGradientMaskParams_->propagationRadius_, propagationRadius);

    std::pair<float, float> center = {-2.5f, -2.5f};
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_WaveCenter", center);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.waveGradientMaskParams_->center_, center);
}

/**
 * @tc.name: SetParam_023
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_023, TestSize.Level1)
{
    Drawing::GEWaveGradientShaderMaskParams param;
    auto geWaveGradientShaderMask = std::make_shared<Drawing::GEWaveGradientShaderMask>(param);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geWaveGradientShaderMask);

    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.filterType_ =
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER;
    geVisualEffectImplDisplaceDistort.displacementDistortParams_ =
        std::make_shared<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(geVisualEffectImplDisplaceDistort.displacementDistortParams_->mask_, shaderMask);
}

/**
 * @tc.name: SetParam_024
 * @tc.desc: Verify function SetParam for DoubleRippleMask is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_024, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplDoubleRippleMask(Drawing::GE_MASK_DOUBLE_RIPPLE);
    geVisualEffectImplDoubleRippleMask.filterType_ = Drawing::GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK;
    geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_ =
        std::make_shared<Drawing::GEDoubleRippleShaderMaskParams>();

    // test invalid params setting
    std::pair<float, float> center1 = {-2.5f, -2.5f};
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Center1", center1);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_->center1_, center1);

    std::pair<float, float> center2 = {2.5f, 2.5f};
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Center2", center2);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_->center2_, center2);

    float radius = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Radius", radius);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_->radius_, radius);

    float width = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Width", width);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_->width_, width);

    float turbulence = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Turbulence", turbulence);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.doubleRippleMaskParams_->turbulence_, turbulence);
}

/**
 * @tc.name: SetParam_025
 * @tc.desc: Verify function SetParam for DoubleRippleMask is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_025, TestSize.Level1)
{
    Drawing::GEDoubleRippleShaderMaskParams param;
    auto geDoubleRippleShaderMask = std::make_shared<Drawing::GEDoubleRippleShaderMask>(param);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geDoubleRippleShaderMask);

    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.filterType_ =
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER;
    geVisualEffectImplDisplaceDistort.displacementDistortParams_ =
        std::make_shared<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(geVisualEffectImplDisplaceDistort.displacementDistortParams_->mask_, shaderMask);
}

/**
 * @tc.name: MakeDoubleRippleMaskParams_001
 * @tc.desc: Verify function DoubleRippleMaskParams
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, DoubleRippleMaskParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK);
    EXPECT_EQ(geVisualEffectImpl.GetDoubleRippleMaskParams(), nullptr);

    geVisualEffectImpl.MakeDoubleRippleMaskParams();
    EXPECT_NE(geVisualEffectImpl.GetDoubleRippleMaskParams(), nullptr);
}

/**
 * @tc.name: MakeWaveGradientMaskParams_001
 * @tc.desc: Verify function WaveGradientMaskParams
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, WaveGradientMaskParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK);
    EXPECT_EQ(geVisualEffectImpl.GetWaveGradientMaskParams(), nullptr);
    
    geVisualEffectImpl.MakeWaveGradientMaskParams();
    EXPECT_NE(geVisualEffectImpl.GetWaveGradientMaskParams(), nullptr);
}

/**
 * @tc.name: MakePixelMapMaskParams_001
 * @tc.desc: Verify function MakePixelMapMaskParams is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakePixelMapMaskParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::PIXEL_MAP_MASK);
    ASSERT_EQ(geVisualEffectImpl.GetPixelMapMaskParams(), nullptr);

    geVisualEffectImpl.MakePixelMapMaskParams();
    ASSERT_NE(geVisualEffectImpl.GetPixelMapMaskParams(), nullptr);
}

/**
 * @tc.name: SetAllParam_001
 * @tc.desc: Verify function Set All Param for param is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetAllParam_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    uint32_t paramUint32 { 0 };
    float paramFloat { 1.0f };
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams(), nullptr);

    geVisualEffectImpl.SetMESABlurParams("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsFloat("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsUint32("", paramUint32);
    geVisualEffectImpl.SetWaterRippleParams("", paramFloat);
    geVisualEffectImpl.SetRippleMaskParamsFloat("", paramFloat);
    geVisualEffectImpl.SetWaveGradientMaskParamsFloat("", paramFloat);
    geVisualEffectImpl.SetDoubleRippleMaskParamsFloat("", paramFloat);
}

/**
 * @tc.name: SetAllParam_002
 * @tc.desc: Verify function Set XXX Param for param is not nullptr and tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetAllParam_002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    uint32_t paramUint32 { 0 };
    float paramFloat { 1.0f };

    geVisualEffectImpl.MakeMESAParams();
    EXPECT_NE(geVisualEffectImpl.GetMESAParams(), nullptr);
    geVisualEffectImpl.SetMESABlurParams("", paramFloat);

    geVisualEffectImpl.MakeMagnifierParams();
    EXPECT_NE(geVisualEffectImpl.GetMagnifierParams(), nullptr);
    geVisualEffectImpl.SetMagnifierParamsFloat("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsUint32("", paramUint32);

    int32_t paramInt32 { 0 };
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER);
    geVisualEffectImpl.SetParam("", paramInt32);
}

/**
 * @tc.name: SetColorGradientParam_001
 * @tc.desc: Verify function Set ColorGradient Param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetColorGradientParam_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");

    geVisualEffectImpl.SetParam("", nullptr);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImpl.SetParam("", nullptr);
    EXPECT_EQ(geVisualEffectImpl1.GetColorGradientParams(), nullptr);  
}

/**
 * @tc.name: SetColorGradientParam_002
 * @tc.desc: Verify function Set ColorGradient Param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetColorGradientParam_002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    std::vector<float> param;
    geVisualEffectImpl.SetParam("", param);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImpl.SetParam("", param);
    EXPECT_EQ(geVisualEffectImpl1.GetColorGradientParams(), nullptr);

    geVisualEffectImpl1.MakeColorGradientParams();
    param = {0.1f, 0.5f}; // 0.1f, 0.5f is param of ColorGradient
    geVisualEffectImpl.SetParam("", param);
    EXPECT_NE(geVisualEffectImpl1.GetColorGradientParams(), nullptr);
    EXPECT_TRUE(geVisualEffectImpl1.GetColorGradientParams()->colors.empty());
}

/**
 * @tc.name: SetContentLightParams_001
 * @tc.desc: Verify function Set ContentLight Param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetContentLightParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    float param = 0.5f;
    geVisualEffectImpl.SetParam("", param);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams(), nullptr);
 
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImpl.SetParam("", param);
    EXPECT_EQ(geVisualEffectImpl1.GetContentLightParams(), nullptr);
 
    geVisualEffectImpl1.MakeContentLightParams();
    geVisualEffectImpl.SetParam("", param);
    EXPECT_NE(geVisualEffectImpl1.GetContentLightParams(), nullptr);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
