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

#include "ge_radial_gradient_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "ge_double_ripple_shader_mask.h"
#include "ge_visual_effect_impl.h"
#include "ge_wave_gradient_shader_mask.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "utils/rect.h"
#include "draw/color.h"
#include "ge_pixel_map_shader_mask.h"
#include "image/bitmap.h"
#include "ge_params_reflection.h"
#include "ge_effects_params.h"

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

    Drawing::GEVisualEffectImpl geVisualEffectImplLightCave(Drawing::GEX_SHADER_LIGHT_CAVE);
    EXPECT_EQ(geVisualEffectImplLightCave.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LIGHT_CAVE);

    Drawing::GEVisualEffectImpl geVisualEffectImplParticleAblation(Drawing::GE_FILTER_PARTICLE_ABLATION);
    EXPECT_EQ(geVisualEffectImplParticleAblation.GetFilterType(),
        Drawing::GEVisualEffectImpl::FilterType::PARTICLE_ABLATION);

    Drawing::GEVisualEffectImpl geVisualEffectImplGasifyScaleTwist(Drawing::GE_FILTER_GASIFY_SCALE_TWIST);
    EXPECT_EQ(geVisualEffectImplGasifyScaleTwist.GetFilterType(),
        Drawing::GEVisualEffectImpl::FilterType::GASIFY_SCALE_TWIST);

    Drawing::GEVisualEffectImpl geVisualEffectImplGasifyBlur(Drawing::GE_FILTER_GASIFY_BLUR);
    EXPECT_EQ(geVisualEffectImplGasifyBlur.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::GASIFY_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImplGasify(Drawing::GE_FILTER_GASIFY);
    EXPECT_EQ(geVisualEffectImplGasify.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::GASIFY);

    Drawing::GEVisualEffectImpl geVisualEffectImplEmpty(Drawing::GE_SHAPE_SDF_EMPTY_SHAPE);
    EXPECT_EQ(geVisualEffectImplEmpty.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::SDF_EMPTY_SHAPE);
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
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_AI_BAR_LOW, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetAIBarParams()->aiBarLow, 1.0f);
    geVisualEffectImpl.SetParam("", 2.0f);
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
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_GREY_COEF_1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetGreyParams()->greyCoef1, 1.0f);
    geVisualEffectImpl.SetParam("", 2.0f);
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
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->blurRadius, 1.0f);
    geVisualEffectImpl.SetParam("", 2.0f);
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
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
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
 * @tc.name: SetParam_013
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_013, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplRippleMask(Drawing::GE_MASK_RIPPLE);
    geVisualEffectImplRippleMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::RIPPLE_MASK);
    geVisualEffectImplRippleMask.MakeParams<Drawing::GERippleShaderMaskParams>();

    // test invalid params setting
    float rippleRadius = 0.5f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Radius", rippleRadius);
    EXPECT_EQ(geVisualEffectImplRippleMask.GetParams<Drawing::GERippleShaderMaskParams>()->radius_, rippleRadius);

    float rippleWidth = 0.6f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Width", rippleWidth);
    EXPECT_EQ(geVisualEffectImplRippleMask.GetParams<Drawing::GERippleShaderMaskParams>()->width_, rippleWidth);

    float centerOffset = 0.7f;
    geVisualEffectImplRippleMask.SetParam("RippleMask_Offset", centerOffset);
    EXPECT_EQ(
        geVisualEffectImplRippleMask.GetParams<Drawing::GERippleShaderMaskParams>()->widthCenterOffset_, centerOffset);
}

/**
 * @tc.name: SetParam_014
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_014, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplRippleMask(Drawing::GE_MASK_RIPPLE);
    geVisualEffectImplRippleMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::RIPPLE_MASK);
    geVisualEffectImplRippleMask.MakeParams<Drawing::GERippleShaderMaskParams>();

    std::pair<float, float> center = {0.5f, 0.5f};
    geVisualEffectImplRippleMask.SetParam("RippleMask_Center", center);
    EXPECT_EQ(geVisualEffectImplRippleMask.GetParams<Drawing::GERippleShaderMaskParams>()->center_, center);


    std::pair<float, float> factor = {0.5f, 0.5f};
    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
    geVisualEffectImplDisplaceDistort.MakeParams<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Factor", factor);
    EXPECT_EQ(
        geVisualEffectImplDisplaceDistort.GetParams<Drawing::GEDisplacementDistortFilterParams>()->factor_, factor);

    Drawing::GEVisualEffectImpl geVisualEffectImplRadialGradientMask(Drawing::GE_MASK_RADIAL_GRADIENT);
    geVisualEffectImplRadialGradientMask.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::RADIAL_GRADIENT_MASK);
    geVisualEffectImplRadialGradientMask.MakeParams<Drawing::GERadialGradientShaderMaskParams>();

    std::pair<float, float> radial_gradient_center = {0.5f, 0.5f};
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_Center", radial_gradient_center);
    EXPECT_EQ(geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->center_,
        radial_gradient_center);
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
    geVisualEffectImplDisplaceDistort.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
    geVisualEffectImplDisplaceDistort.MakeParams<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(
        geVisualEffectImplDisplaceDistort.GetParams<Drawing::GEDisplacementDistortFilterParams>()->mask_, shaderMask);
}

/**
 * @tc.name: SetSoundWaveParams001
 * @tc.desc: Verify function SetSoundWaveParams for float
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSoundWaveParams001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave(Drawing::GE_FILTER_SOUND_WAVE);
    float colorProgress = 0.5f;
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, colorProgress);
    EXPECT_NE(geVisualEffectImplSoundWave.GetSoundWaveParams(), nullptr);

    // test valid sound wave params setting
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->colorProgress, 0.5f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_SOUNDINTENSITY, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->soundIntensity, 1.0f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A, 0.6f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveAlphaA, 0.6f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveAlphaB, 1.0f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveProgressA, 1.0f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B, 1.0f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveProgressB, 1.0f);
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_TOTAL_ALPHA, 0.5f);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams()->shockWaveTotalAlpha, 0.5f);
}

/**
 * @tc.name: SetSoundWaveParam002
 * @tc.desc: Verify function SetSoundWaveParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSoundWaveParam002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave("");
    float param = 1.2f;

    // test invalid params setting
    geVisualEffectImplSoundWave.SetParam(Drawing::GE_FILTER_SOUND_WAVE_COLORPROGRESS, param);
    EXPECT_EQ(geVisualEffectImplSoundWave.GetSoundWaveParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImplSoundWave1(Drawing::GE_FILTER_SOUND_WAVE);
    EXPECT_NE(geVisualEffectImplSoundWave1.GetSoundWaveParams(), nullptr);

    geVisualEffectImplSoundWave1.SetParam("", param);
    EXPECT_NE(geVisualEffectImplSoundWave1.GetSoundWaveParams()->shockWaveProgressB, param);
}

/**
 * @tc.name: SetParamBezierWarp_001
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParamBezierWarp_001, TestSize.Level1)
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
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEBezierWarpShaderFilterParams>()->destinationPatch, points);
}

/**
 * @tc.name: SetParamEdgelight_001
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParamEdgelight_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_EDGE_LIGHT);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 0.5f);
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
    EXPECT_EQ(geVisualEffectImplTest.GetParams<Drawing::GEBezierWarpShaderFilterParams>(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_BEZIER_WARP);
    geVisualEffectImpl.MakeBezierWarpParams();
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GEBezierWarpShaderFilterParams>(), nullptr);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_BEZIER_WARP_CONTROL_POINT0, std::make_pair(0.5f, 0.5f));
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetBezierWarpParams()->destinationPatch[0].GetX(), 0.5f);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_BEZIER_WARP_CONTROL_POINT11, std::make_pair(1.f, 1.f));
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
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_DISPERSION_OPACITY, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->opacity, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->redOffset, std::make_pair(0.5f, 0.5f));
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(geVisualEffectImpl.GetDispersionParams()->greenOffset, std::make_pair(0.5f, 0.5f));
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET, std::make_pair(0.5f, 0.5f));
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
    ASSERT_NE(geVisualEffectImpl.GetParams<Drawing::GEWaterRippleFilterParams>(), nullptr);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
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
    geVisualEffectImpl.SetParam("GE_FILTER_WATER_RIPPLE_PROGRESS", 0.5f);
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
    Drawing::Color4f colorTestA = {0.0f, 1.0f, 1.0f, 1.0f};
    Drawing::Color4f colorTestB = {1.0f, 0.0f, 1.0f, 1.0f};
    Drawing::Color4f colorTestC = {1.0f, 1.0f, 0.0f, 1.0f};
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
 * @tc.name: SetParam_018
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_018, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplRadialGradientMask(Drawing::GE_MASK_RADIAL_GRADIENT);
    geVisualEffectImplRadialGradientMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::RADIAL_GRADIENT_MASK);
    geVisualEffectImplRadialGradientMask.MakeParams<Drawing::GERadialGradientShaderMaskParams>();

    // test invalid params setting
    float rippleRadiusX = 0.5f;
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_RadiusX", rippleRadiusX);
    EXPECT_EQ(geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->radiusX_,
        rippleRadiusX);

    float rippleRadiusY = 0.5f;
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_RadiusY", rippleRadiusY);
    EXPECT_EQ(geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->radiusY_,
        rippleRadiusY);

    std::pair<float, float> center = { -2.5f, -2.5f };
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_Center", center);
    EXPECT_EQ(
        geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->center_, center);

    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f }; // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_Colors", colors);
    EXPECT_EQ(
        geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->colors_, colors);

    std::vector<float> positions = { 1.0f, 1.0f }; // 1.0, 1.0 is poition xy params
    geVisualEffectImplRadialGradientMask.SetParam("RadialGradientMask_Positions", positions);
    EXPECT_EQ(geVisualEffectImplRadialGradientMask.GetParams<Drawing::GERadialGradientShaderMaskParams>()->positions_,
        positions);
}

/**
 * @tc.name: SetParam_019
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_19, TestSize.Level1)
{
    Drawing::GERadialGradientShaderMaskParams param;
    auto geRadialGradientShaderMask = std::make_shared<Drawing::GERadialGradientShaderMask>(param);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geRadialGradientShaderMask);

    Drawing::GEVisualEffectImpl geVisualEffectImplDisplaceDistort(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImplDisplaceDistort.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
    geVisualEffectImplDisplaceDistort.MakeParams<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(
        geVisualEffectImplDisplaceDistort.GetParams<Drawing::GEDisplacementDistortFilterParams>()->mask_, shaderMask);
}

/**
 * @tc.name: SetParam_020
 * @tc.desc: Verify function SetParam for param is content light
 * @tc.type:FUNC
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
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->intensity, lightIntensity);

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_POSITION, lightPosition);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->position, lightPosition);

    Vector4f lightColor = Vector4f(0.3f, 0.6f, 0.9f, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_CONTENT_LIGHT_COLOR, lightColor);
    EXPECT_EQ(geVisualEffectImpl.GetContentLightParams()->color, lightColor);
}

/**
 * @tc.name: SetParam_021
 * @tc.desc: Verify function SetParam for PixelMapMask parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_021, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplPixelMapMask(Drawing::GE_MASK_PIXEL_MAP);
    geVisualEffectImplPixelMapMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::PIXEL_MAP_MASK);
    geVisualEffectImplPixelMapMask.MakeParams<Drawing::GEPixelMapMaskParams>();

    // Test RectF parameters (src and dst)
    Drawing::RectF srcRect = Drawing::RectF(0.0f, 0.0f, 100.0f, 100.0f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Src", srcRect);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.GetParams<Drawing::GEPixelMapMaskParams>()->src, srcRect);

    Drawing::RectF dstRect = Drawing::RectF(10.0f, 10.0f, 200.0f, 200.0f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Dst", dstRect);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.GetParams<Drawing::GEPixelMapMaskParams>()->dst, dstRect);

    // Test Vector4f parameter (fillColor)
    Vector4f fillColor = Vector4f(1.0f, 0.5f, 0.2f, 0.8f);
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_FillColor", fillColor);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.GetParams<Drawing::GEPixelMapMaskParams>()->fillColor, fillColor);

    // Test Image parameter
    auto testImage = std::make_shared<Drawing::Image>();
    geVisualEffectImplPixelMapMask.SetParam("PixelMapMask_Image", testImage);
    EXPECT_EQ(geVisualEffectImplPixelMapMask.GetParams<Drawing::GEPixelMapMaskParams>()->image, testImage);
}

/**
 * @tc.name: SetParam_022
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_022, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplWaveGradientMask(Drawing::GE_MASK_WAVE_GRADIENT);
    geVisualEffectImplWaveGradientMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK);
    geVisualEffectImplWaveGradientMask.MakeParams<Drawing::GEWaveGradientShaderMaskParams>();

    // test invalid params setting
    float width = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_WaveWidth", width);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.GetParams<Drawing::GEWaveGradientShaderMaskParams>()->width_, width);

    float turbulenceStrength = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_TurbulenceStrength", turbulenceStrength);
    EXPECT_EQ(
        geVisualEffectImplWaveGradientMask.GetParams<Drawing::GEWaveGradientShaderMaskParams>()->turbulenceStrength_,
        turbulenceStrength);

    float blurRadius = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_BlurRadius", blurRadius);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.GetParams<Drawing::GEWaveGradientShaderMaskParams>()->blurRadius_,
        blurRadius);

    float propagationRadius = 0.5f;
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_PropagationRadius", propagationRadius);
    EXPECT_EQ(
        geVisualEffectImplWaveGradientMask.GetParams<Drawing::GEWaveGradientShaderMaskParams>()->propagationRadius_,
        propagationRadius);

    std::pair<float, float> center = {-2.5f, -2.5f};
    geVisualEffectImplWaveGradientMask.SetParam("WaveGradientMask_WaveCenter", center);
    EXPECT_EQ(geVisualEffectImplWaveGradientMask.GetParams<Drawing::GEWaveGradientShaderMaskParams>()->center_, center);
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
    geVisualEffectImplDisplaceDistort.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
    geVisualEffectImplDisplaceDistort.MakeParams<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(
        geVisualEffectImplDisplaceDistort.GetParams<Drawing::GEDisplacementDistortFilterParams>()->mask_, shaderMask);
}

/**
 * @tc.name: SetParam_024
 * @tc.desc: Verify function SetParam for DoubleRippleMask is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_024, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplDoubleRippleMask(Drawing::GE_MASK_DOUBLE_RIPPLE);
    geVisualEffectImplDoubleRippleMask.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK);
    geVisualEffectImplDoubleRippleMask.MakeParams<Drawing::GEDoubleRippleShaderMaskParams>();

    // test invalid params setting
    std::pair<float, float> center1 = {-2.5f, -2.5f};
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Center1", center1);
    EXPECT_EQ(
        geVisualEffectImplDoubleRippleMask.GetParams<Drawing::GEDoubleRippleShaderMaskParams>()->center1_, center1);

    std::pair<float, float> center2 = {2.5f, 2.5f};
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Center2", center2);
    EXPECT_EQ(
        geVisualEffectImplDoubleRippleMask.GetParams<Drawing::GEDoubleRippleShaderMaskParams>()->center2_, center2);

    float radius = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Radius", radius);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.GetParams<Drawing::GEDoubleRippleShaderMaskParams>()->radius_, radius);

    float width = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Width", width);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.GetParams<Drawing::GEDoubleRippleShaderMaskParams>()->width_, width);

    float turbulence = 0.5f;
    geVisualEffectImplDoubleRippleMask.SetParam("DoubleRippleMask_Turbulence", turbulence);
    EXPECT_EQ(geVisualEffectImplDoubleRippleMask.GetParams<Drawing::GEDoubleRippleShaderMaskParams>()->turbulence_,
        turbulence);
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
    geVisualEffectImplDisplaceDistort.SetFilterType(
        Drawing::GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
    geVisualEffectImplDisplaceDistort.MakeParams<Drawing::GEDisplacementDistortFilterParams>();
    geVisualEffectImplDisplaceDistort.SetParam("DispDistort_Mask", shaderMask);
    EXPECT_EQ(
        geVisualEffectImplDisplaceDistort.GetParams<Drawing::GEDisplacementDistortFilterParams>()->mask_, shaderMask);
}

/**
 * @tc.name: SetParam_026
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_026, TestSize.Level1)
{
    float paramFloat = 1.0f;
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GEX_SHADER_COLOR_GRADIENT_EFFECT);
    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR_NUMBER, paramFloat);

    Vector4f c4 = {0.0f, 0.0f, 0.0f, 0.0f};
    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR0, c4);

    std::pair<float, float> p2 = {0.0f, 0.0f};
    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_COLOR_GRADIENT_EFFECT_POS0, p2);

    ASSERT_NE(geVisualEffectImpl.GetColorGradientEffectParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->colorNum_, paramFloat);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->colors_[0].redF_, c4[0]);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->colors_[0].greenF_, c4[1]);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->colors_[0].blueF_, c4[2]);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->colors_[0].alphaF_, c4[3]);

    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->positions_[0].GetX(), p2.first);
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientEffectParams()->positions_[0].GetY(), p2.second);
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
 * @tc.desc: Verify function MakePixelMapMaskParams
 * @tc.type:FUNC
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
 * @tc.name: MakeSdfFromImageParams_001
 * @tc.desc: Verify function MakeSdfFromImageParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeSdfFromImageParams_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::SDF_FROM_IMAGE);
    ASSERT_EQ(geVisualEffectImpl.GetSdfFromImageParams(), nullptr);
    
    geVisualEffectImpl.MakeSdfFromImageParams();
    ASSERT_NE(geVisualEffectImpl.GetSdfFromImageParams(), nullptr);
}

/**
 * @tc.name: SetAllParam_001
 * @tc.desc: Verify function Set All Param for param is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetAllParam_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    uint32_t paramUint32 { 0 };
    float paramFloat { 1.0f };
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams(), nullptr);

    geVisualEffectImpl.SetParam("", paramFloat);
    geVisualEffectImpl.SetParam("", paramUint32);
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
    geVisualEffectImpl.SetParam("", paramFloat);

    geVisualEffectImpl.MakeMagnifierParams();
    EXPECT_NE(geVisualEffectImpl.GetMagnifierParams(), nullptr);
    geVisualEffectImpl.SetParam("", paramFloat);
    geVisualEffectImpl.SetParam("", paramUint32);

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
    EXPECT_EQ(geVisualEffectImpl.GetColorGradientParams(), nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
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

/**
 * @tc.name: SetLightCaveParamsTest
 * @tc.desc: Verify function Set LightCave Param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetLightCaveParamsTest, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LIGHT_CAVE);
    EXPECT_EQ(geVisualEffectImpl.GetLightCaveParams(), nullptr);

    // init param data
    Vector4f color = Vector4f(0.5f, 0.5f, 0.5f, 1.0f); // 0.5f, 0.5f, 0.5f, 1.0f is RGBA
    std::pair<float, float> position = {0.5f, 0.5f}; // 0.5f, 0.5f is random number
    std::pair<float, float> randerXY = {0.6f, 0.7f}; // 0.6f, 0.7f is random number
    float progress = 0.5f; // 0.5f is random number

    geVisualEffectImpl.SetParam("", color);
    geVisualEffectImpl.SetParam("", position);
    geVisualEffectImpl.SetParam("", progress);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>(), nullptr);

    geVisualEffectImpl.MakeLightCaveParams();
    ASSERT_NE(geVisualEffectImpl.GetLightCaveParams(), nullptr);

    geVisualEffectImpl.SetParam("", color);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->colorA, color);

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_COLORA, color);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->colorA, color);

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_COLORB, color);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->colorB, color);

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_COLORC, color);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->colorC, color);

    geVisualEffectImpl.SetParam("", position);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->position,
        Vector2f(position.first, position.second));

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_POSITION, position);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->position,
        Vector2f(position.first, position.second));

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_RADIUSXY, randerXY);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->radiusXY,
        Vector2f(randerXY.first, randerXY.second));

    geVisualEffectImpl.SetParam("", progress);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->progress, progress);

    geVisualEffectImpl.SetParam(Drawing::GEX_SHADER_LIGHT_CAVE_PROGRESS, progress);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GEXLightCaveShaderParams>()->progress, progress);
}

/**
 * @tc.name: SetParam_FrostedGlass_DarkMode_001
 * @tc.desc: Verify SetParam for FrostedGlass dark-mode properties updates params
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_FrostedGlass_DarkMode_001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl ve(Drawing::GE_FILTER_FROSTED_GLASS);
    ve.MakeFrostedGlassParams();
    ASSERT_NE(ve.GetFrostedGlassParams(), nullptr);

    // dark scale
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARK_SCALE, 0.7f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkScale, 0.7f);

    // dark blur params (radius, k)
    std::pair<float, float> darkBlur = {60.0f, 5.0f};
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BLURPARAM, darkBlur);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBlurParam[0], darkBlur.first);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBlurParam[1], darkBlur.second);

    // dark weights emboss
    std::pair<float, float> darkWeights = {0.2f, 0.8f};
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_WEIGHTSEMBOSS, darkWeights);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeWeightsEmboss[0], darkWeights.first);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeWeightsEmboss[1], darkWeights.second);

    // dark bg rates
    std::pair<float, float> darkRates = {-0.00003f, 1.2f};
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BGRATES, darkRates);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgRates[0], darkRates.first);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgRates[1], darkRates.second);

    // dark bg KBS
    Vector3f darkBgKBS = Vector3f(-5.0f, 7.0f, 10.0f);
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BGKBS, darkBgKBS);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[0], darkBgKBS[0]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[1], darkBgKBS[1]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[2], darkBgKBS[2]);

    // dark bg pos/neg
    Vector3f darkBgPos = Vector3f(1.0f, 2.0f, 3.0f);
    Vector3f darkBgNeg = Vector3f(4.0f, 5.0f, 6.0f);
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BGPOS, darkBgPos);
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BGNEG, darkBgNeg);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgPos[0], darkBgPos[0]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgPos[1], darkBgPos[1]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgPos[2], darkBgPos[2]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgNeg[0], darkBgNeg[0]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgNeg[1], darkBgNeg[1]);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgNeg[2], darkBgNeg[2]);
}

/**
 * @tc.name: SetParam_FrostedGlass_DarkMode_Clamp
 * @tc.desc: Verify clamping for FrostedGlass dark-mode setters
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam_FrostedGlass_DarkMode_Clamp, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl ve(Drawing::GE_FILTER_FROSTED_GLASS);
    ve.MakeFrostedGlassParams();
    ASSERT_NE(ve.GetFrostedGlassParams(), nullptr);

    // darkScale clamps to [0,1]
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARK_SCALE, -0.5f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkScale, 0.0f);
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARK_SCALE, 1.5f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkScale, 1.0f);

    // blurParam clamps: radius >= 0, k in [0,200]
    std::pair<float, float> badBlur = {0.0f, 500.0f};
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BLURPARAM, badBlur);
    EXPECT_GE(ve.GetFrostedGlassParams()->darkModeBlurParam[0], 0.0f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBlurParam[1], 200.0f);

    // weightsEmboss clamps to [0,1]
    std::pair<float, float> badWeights = {-1.0f, 0.8f};
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_WEIGHTSEMBOSS, badWeights);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeWeightsEmboss[0], -1.0f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeWeightsEmboss[1], 0.8f);

    // KBS clamps: KB in [-20,20], S in [0,20]
    Vector3f badBgKBS = Vector3f(-50.0f, 50.0f, -5.0f);
    ve.SetParam(Drawing::GE_SHADER_FROSTED_GLASS_DARKMODE_BGKBS, badBgKBS);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[0], -20.0f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[1], 20.0f);
    EXPECT_FLOAT_EQ(ve.GetFrostedGlassParams()->darkModeBgKBS[2], 0.0f);
}

/**
 * @tc.name: SetSDFRRectShapeParamsTest
 * @tc.desc: Verify function Set SDFRRectShape Param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSDFRRectShapeParamsTest, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE);
    EXPECT_EQ(geVisualEffectImpl.GetSDFRRectShapeParams(), nullptr);

    // init param data
    float left = 10.0f;
    float top = 20.0f;
    float width = 100.0f;
    float height = 200.0f;
    float radiusX = 15.0f;
    float radiusY = 15.0f;

    Drawing::GERRect rrect{left, top, width, height};
    rrect.SetCornerRadius(radiusX, radiusY);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_RRECT_SHAPE_RRECT, rrect);
    EXPECT_EQ(geVisualEffectImpl.GetSDFRRectShapeParams(), nullptr);

    geVisualEffectImpl.MakeSDFRRectShapeParams();
    ASSERT_NE(geVisualEffectImpl.GetSDFRRectShapeParams(), nullptr);

    // test invalid parameter names
    geVisualEffectImpl.SetParam("", rrect);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.left_, left);

    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_RRECT_SHAPE_RRECT, rrect);
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.left_, left);
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.top_, top);
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.width_, width);
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.height_, height);
    EXPECT_FLOAT_EQ(
        geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.radius_[Drawing::GERRect::TOP_LEFT].x_,
        radiusX);
    EXPECT_FLOAT_EQ(
        geVisualEffectImpl.GetParams<Drawing::GESDFRRectShapeParams>()->rrect.radius_[Drawing::GERRect::TOP_LEFT].y_,
        radiusY);
}

/**
 * @tc.name: SetUnionOpShapeParamsTest
 * @tc.desc: Verify function Set UnionOp Shape Param
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetUnionOpShapeParamsTest, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::SDF_UNION_OP);
    EXPECT_EQ(geVisualEffectImpl.GetSDFUnionOpShapeParams(), nullptr);

    // init param data
    float spacing = 5.0f;
    auto opType = Drawing::GESDFUnionOp::UNION;
    auto opTypeInt = static_cast<uint32_t>(opType);

    // Create test sub-shapes
    Drawing::GESDFRRectShapeParams leftRectParams;
    leftRectParams.rrect = { 10.0f, 10.0f, 50.0f, 50.0f };
    leftRectParams.rrect.SetCornerRadius(5.0f, 5.0f);
    auto leftShape = std::make_shared<Drawing::GESDFRRectShaderShape>(leftRectParams);

    Drawing::GESDFRRectShapeParams rightRectParams;
    rightRectParams.rrect = { 60.0f, 60.0f, 50.0f, 50.0f };
    rightRectParams.rrect.SetCornerRadius(5.0f, 5.0f);
    auto rightShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rightRectParams);

    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_SMOOTH_UNION_OP_SPACING, spacing);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_TYPE, opTypeInt);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_SHAPEX, leftShape);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_SHAPEY, rightShape);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>(), nullptr);

    geVisualEffectImpl.MakeSDFUnionOpShapeParams(Drawing::GESDFUnionOp::SMOOTH_UNION);
    ASSERT_NE(geVisualEffectImpl.GetSDFUnionOpShapeParams(), nullptr);

    // test invalid parameter names
    geVisualEffectImpl.SetParam("", spacing);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->spacing, spacing);

    geVisualEffectImpl.SetParam("", opTypeInt);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->op, opType);

    geVisualEffectImpl.SetParam("", leftShape);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->left, nullptr);

    geVisualEffectImpl.SetParam("", rightShape);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->right, nullptr);

    // test normal case
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_SMOOTH_UNION_OP_SPACING, spacing);
    EXPECT_FLOAT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->spacing, spacing);

    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_TYPE, opTypeInt);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->op, opType);

    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_SHAPEX, leftShape);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEX, leftShape);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->left, nullptr);

    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_UNION_OP_SHAPEY, rightShape);
    geVisualEffectImpl.SetParam(Drawing::GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEY, leftShape);
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESDFUnionOpShapeParams>()->right, nullptr);
}

/**
 * @tc.name: GetFilterType_SpatialPointLight
 * @tc.desc: Verify function GetFilterType for SPATIAL_POINT_LIGHT
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetFilterType_SpatialPointLight, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::SPATIAL_POINT_LIGHT);
}

/**
 * @tc.name: MakeSpatialPointLightParams
 * @tc.desc: Verify function MakeParams for GESpatialPointLightShaderParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeSpatialPointLightParams, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::SPATIAL_POINT_LIGHT);
    EXPECT_EQ(geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>(), nullptr);

    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    EXPECT_NE(geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>(), nullptr);
}

/**
 * @tc.name: SetSpatialPointLightParams_LightIntensity
 * @tc.desc: Verify function SetParam for SpatialPointLight lightIntensity
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_LightIntensity, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    float lightIntensity = 2.5f;
    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_LIGHT_INTENSITY,
        lightIntensity);
    EXPECT_FLOAT_EQ(params->lightIntensity, lightIntensity);
}

/**
 * @tc.name: SetSpatialPointLightParams_Attenuation
 * @tc.desc: Verify function SetParam for SpatialPointLight attenuation
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_Attenuation, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    float attenuation = 0.8f;
    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_ATTENUATION, attenuation);
    EXPECT_FLOAT_EQ(params->attenuation, attenuation);
}

/**
 * @tc.name: SetSpatialPointLightParams_LightPosition
 * @tc.desc: Verify function SetParam for SpatialPointLight Vector3f param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_LightPosition, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    Vector3f lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_LIGHT_POSITION,
        lightPosition);
    EXPECT_FLOAT_EQ(params->lightPosition.x_, lightPosition.x_);
    EXPECT_FLOAT_EQ(params->lightPosition.y_, lightPosition.y_);
    EXPECT_FLOAT_EQ(params->lightPosition.z_, lightPosition.z_);
}

/**
 * @tc.name: SetSpatialPointLightParams_LightColor
 * @tc.desc: Verify function SetParam for SpatialPointLight Vector4f param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_LightColor, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    Vector4f lightColor = Vector4f(1.0f, 0.8f, 0.6f, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_LIGHT_COLOR, lightColor);
    EXPECT_FLOAT_EQ(params->lightColor.x_, lightColor.x_);
    EXPECT_FLOAT_EQ(params->lightColor.y_, lightColor.y_);
    EXPECT_FLOAT_EQ(params->lightColor.z_, lightColor.z_);
    EXPECT_FLOAT_EQ(params->lightColor.w_, lightColor.w_);
}

/**
 * @tc.name: SetSpatialPointLightParams_Mask
 * @tc.desc: Verify function SetParam for SpatialPointLight mask param
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_Mask, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    Drawing::GERippleShaderMaskParams maskParam;
    auto geRippleShaderMask = std::make_shared<Drawing::GERippleShaderMask>(maskParam);
    auto shaderMask = std::static_pointer_cast<Drawing::GEShaderMask>(geRippleShaderMask);
    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_MASK, shaderMask);
    EXPECT_EQ(params->mask, shaderMask);
}

/**
 * @tc.name: SetSpatialPointLightParams_InvalidTag
 * @tc.desc: Verify function SetParam for SpatialPointLight with invalid tag
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_InvalidTag, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    float defaultValue = params->lightIntensity;
    geVisualEffectImpl.SetParam("", 5.0f);
    EXPECT_FLOAT_EQ(params->lightIntensity, defaultValue);
}

/**
 * @tc.name: SetSpatialPointLightParams_IntensityZero
 * @tc.desc: Verify function SetParam for SpatialPointLight with zero intensity
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_IntensityZero, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_LIGHT_INTENSITY, 0.0f);
    EXPECT_FLOAT_EQ(params->lightIntensity, 0.0f);
}

/**
 * @tc.name: SetSpatialPointLightParams_IntensityNegative
 * @tc.desc: Verify function SetParam for SpatialPointLight with negative intensity
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_IntensityNegative, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_LIGHT_INTENSITY, -1.0f);
    EXPECT_FLOAT_EQ(params->lightIntensity, -1.0f);
}

/**
 * @tc.name: SetSpatialPointLightParams_AttenuationZero
 * @tc.desc: Verify function SetParam for SpatialPointLight with zero attenuation
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_AttenuationZero, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_ATTENUATION, 0.0f);
    EXPECT_FLOAT_EQ(params->attenuation, 0.0f);
}

/**
 * @tc.name: SetSpatialPointLightParams_AttenuationLarge
 * @tc.desc: Verify function SetParam for SpatialPointLight with large attenuation
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetSpatialPointLightParams_AttenuationLarge, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("SpatialPointLight");
    geVisualEffectImpl.MakeParams<Drawing::GESpatialPointLightShaderParams>();
    auto params = geVisualEffectImpl.GetParams<Drawing::GESpatialPointLightShaderParams>();
    ASSERT_NE(params, nullptr);

    geVisualEffectImpl.SetParam(Drawing::GEParamsMemberTag::SPATIAL_POINT_LIGHT_ATTENUATION, 100.0f);
    EXPECT_FLOAT_EQ(params->attenuation, 100.0f);
}

/**
 * @tc.name: MakeGetParams_FilterEffects
 * @tc.desc: Verify Make and Get params methods for displacement, sound wave, and direction light filters
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_FilterEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
    geVisualEffectImpl.MakeDisplacementDistortParams();
    auto params = geVisualEffectImpl.GetDisplacementDistortParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_SOUND_WAVE);
    geVisualEffectImpl2.MakeSoundWaveParams();
    auto params2 = geVisualEffectImpl2.GetSoundWaveParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_DIRECTION_LIGHT);
    geVisualEffectImpl3.MakeDirectionLightParams();
    auto params3 = geVisualEffectImpl3.GetDirectionLightParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_ShaderEffects
 * @tc.desc: Verify Make and Get params methods for wavy ripple, aurora noise, and particle circular halo shaders
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_ShaderEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHADER_WAVY_RIPPLE_LIGHT);
    geVisualEffectImpl.MakeWavyRippleLightParams();
    auto params = geVisualEffectImpl.GetWavyRippleLightParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHADER_AURORA_NOISE);
    geVisualEffectImpl2.MakeAuroraNoiseParams();
    auto params2 = geVisualEffectImpl2.GetAuroraNoiseParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_SHADER_PARTICLE_CIRCULAR_HALO);
    geVisualEffectImpl3.MakeParticleCircularHaloParams();
    auto params3 = geVisualEffectImpl3.GetParticleCircularHaloParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_MaskEffects
 * @tc.desc: Verify Make and Get params methods for radial gradient, image, and use effect masks
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_MaskEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_MASK_RADIAL_GRADIENT);
    geVisualEffectImpl.MakeRadialGradientMaskParams();
    auto params = geVisualEffectImpl.GetRadialGradientMaskParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_MASK_IMAGE);
    geVisualEffectImpl2.MakeImageMaskParams();
    auto params2 = geVisualEffectImpl2.GetImageMaskParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_MASK_USE_EFFECT);
    geVisualEffectImpl3.MakeUseEffectMaskParams();
    auto params3 = geVisualEffectImpl3.GetUseEffectMaskParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_TransitionAndBlurEffects
 * @tc.desc: Verify Make and Get params methods for mask transition and variable radius blur
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_TransitionAndBlurEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_MASK_TRANSITION);
    geVisualEffectImpl.MakeMaskTransitionParams();
    auto params = geVisualEffectImpl.GetMaskTransitionParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_VARIABLE_RADIUS_BLUR);
    geVisualEffectImpl2.MakeVariableRadiusBlurParams();
    auto params2 = geVisualEffectImpl2.GetVariableRadiusBlurParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: MakeGetParams_SDFShapeEffects
 * @tc.desc: Verify Make and Get params methods for SDF pixelmap, transform, and shadow shapes
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_SDFShapeEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHAPE_SDF_PIXELMAP_SHAPE);
    geVisualEffectImpl.MakeSDFPixelmapShapeParams();
    auto params = geVisualEffectImpl.GetSDFPixelmapShapeParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHAPE_SDF_TRANSFORM_SHAPE);
    geVisualEffectImpl2.MakeSDFTransformShapeParams();
    auto params2 = geVisualEffectImpl2.GetSDFTransformShapeParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_SHADER_SDF_SHADOW);
    geVisualEffectImpl3.MakeSDFShadowShaderParams();
    auto params3 = geVisualEffectImpl3.GetSDFShadowShaderParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_SDFClipAndBorder
 * @tc.desc: Verify Make and Get params methods for SDF clip and border shaders
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_SDFClipAndBorder, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHADER_SDF_CLIP);
    geVisualEffectImpl.MakeSDFClipParams();
    auto params = geVisualEffectImpl.GetSDFClipShaderParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHADER_SDF_BORDER);
    geVisualEffectImpl2.MakeSDFBorderShaderParams();
    auto params2 = geVisualEffectImpl2.GetSDFBorderShaderParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: MakeGetParams_LightEffects
 * @tc.desc: Verify Make and Get params methods for border light, color gradient, and harmonium effects
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_LightEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHADER_BORDER_LIGHT);
    geVisualEffectImpl.MakeBorderLightParams();
    auto params = geVisualEffectImpl.GetBorderLightParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GEX_SHADER_COLOR_GRADIENT_EFFECT);
    geVisualEffectImpl2.MakeColorGradientEffectParams();
    auto params2 = geVisualEffectImpl2.GetColorGradientEffectParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_SHADER_HARMONIUM_EFFECT);
    geVisualEffectImpl3.MakeHarmoniumEffectParams();
    auto params3 = geVisualEffectImpl3.GetHarmoniumEffectParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_GlowEffects
 * @tc.desc: Verify Make and Get params methods for AI bar glow, rounded rect flowlight, and gradient flow colors
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_GlowEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GEX_SHADER_AIBAR_GLOW);
    geVisualEffectImpl.MakeAIBarGlowEffectParams();
    auto params = geVisualEffectImpl.GetAIBarGlowEffectParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GEX_SHADER_ROUNDED_RECT_FLOWLIGHT);
    geVisualEffectImpl2.MakeRoundedRectFlowlightEffectParams();
    auto params2 = geVisualEffectImpl2.GetRoundedRectFlowlightEffectParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GEX_SHADER_GRADIENT_FLOW_COLORS);
    geVisualEffectImpl3.MakeGradientFlowColorsEffectParams();
    auto params3 = geVisualEffectImpl3.GetGradientFlowColorsEffectParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_MaskAndWarpEffects
 * @tc.desc: Verify Make and Get params methods for frame gradient mask and grid warp filter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_MaskAndWarpEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_MASK_FRAME_GRADIENT);
    geVisualEffectImpl.MakeFrameGradientMaskParams();
    auto params = geVisualEffectImpl.GetFrameGradientMaskParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GRID_WARP);
    geVisualEffectImpl2.MakeGridWarpFilterParams();
    auto params2 = geVisualEffectImpl2.GetGridWarpFilterParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: MakeGetParams_FlowlightAndFrostedGlass
 * @tc.desc: Verify Make and Get params methods for circle flowlight and frosted glass effects
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_FlowlightAndFrostedGlass, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHADER_CIRCLE_FLOWLIGHT);
    geVisualEffectImpl.MakeCircleFlowlightEffectParams();
    auto params = geVisualEffectImpl.GetCircleFlowlightEffectParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHADER_FROSTED_GLASS_EFFECT);
    geVisualEffectImpl2.MakeFrostedGlassEffectParams();
    auto params2 = geVisualEffectImpl2.GetFrostedGlassEffectParams();
    EXPECT_NE(params2, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_FROSTED_GLASS_BLUR);
    geVisualEffectImpl3.MakeFrostedGlassBlurParams();
    auto params3 = geVisualEffectImpl3.GetFrostedGlassBlurParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: SetCache_ValidCacheData
 * @tc.desc: Verify function SetCache and GetCache with valid cache data
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCache_ValidCacheData, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);

    auto cacheData = std::make_shared<std::any>(std::string("test_cache"));
    geVisualEffectImpl.SetCache(cacheData);

    auto retrievedCache = geVisualEffectImpl.GetCache();
    EXPECT_NE(retrievedCache, nullptr);

    auto& cacheValue = std::any_cast<std::string&>(*retrievedCache);
    EXPECT_EQ(cacheValue, "test_cache");
}

/**
 * @tc.name: SetCache_NullCacheData
 * @tc.desc: Verify function SetCache with nullptr cache data
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCache_NullCacheData, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);

    geVisualEffectImpl.SetCache(nullptr);
    auto retrievedCache = geVisualEffectImpl.GetCache();
    EXPECT_EQ(retrievedCache, nullptr);
}

/**
 * @tc.name: SetCanvasInfo_ValidCanvasGeometry
 * @tc.desc: Verify function SetCanvasInfo and GetCanvasInfo with valid canvas geometry
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCanvasInfo_ValidCanvasGeometry, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);

    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 100.0f;
    canvasInfo.geoHeight = 200.0f;
    geVisualEffectImpl.SetCanvasInfo(canvasInfo);

    auto retrievedCanvasInfo = geVisualEffectImpl.GetCanvasInfo();
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoWidth, 100.0f);
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoHeight, 200.0f);
}

/**
 * @tc.name: MakeGetParams_ContourDiagonalFlowLight
 * @tc.desc: Verify Make and Get params methods for contour diagonal flow light shader
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_ContourDiagonalFlowLight, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT);
    geVisualEffectImpl.MakeContenDiagonalParams();
    auto params = geVisualEffectImpl.GetContenDiagonalParams();
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: MakeGetParams_RippleMasks
 * @tc.desc: Verify Make and Get params methods for ripple and double ripple masks
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_RippleMasks, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_MASK_RIPPLE);
    geVisualEffectImpl.MakeRippleMaskParams();
    auto params = geVisualEffectImpl.GetRippleMaskParams();
    EXPECT_NE(params, nullptr);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_MASK_DOUBLE_RIPPLE);
    geVisualEffectImpl2.MakeDoubleRippleMaskParams();
    auto params2 = geVisualEffectImpl2.GetDoubleRippleMaskParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: Constructor_EmptyFilterName
 * @tc.desc: Verify constructor handles empty filter name
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, Constructor_EmptyFilterName, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::NONE);
}

/**
 * @tc.name: Constructor_WithCanvasInfo
 * @tc.desc: Verify constructor with CanvasInfo parameter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, Constructor_WithCanvasInfo, TestSize.Level1)
{
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 50.0f;
    canvasInfo.geoHeight = 100.0f;
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR, canvasInfo);
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    
    auto retrievedCanvasInfo = geVisualEffectImpl.GetCanvasInfo();
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoWidth, 50.0f);
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoHeight, 100.0f);
}

/**
 * @tc.name: GetParams_WithoutMake
 * @tc.desc: Verify GetParams returns nullptr when MakeParams was not called
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetParams_WithoutMake, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    
    auto params = geVisualEffectImpl.GetKawaseParams();
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: MakeParams_MultipleCalls
 * @tc.desc: Verify multiple MakeParams calls work correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeParams_MultipleCalls, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    
    geVisualEffectImpl.MakeKawaseParams();
    auto params1 = geVisualEffectImpl.GetKawaseParams();
    EXPECT_NE(params1, nullptr);
    
    geVisualEffectImpl.MakeKawaseParams();
    auto params2 = geVisualEffectImpl.GetKawaseParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: SetCanvasInfo_ZeroValues
 * @tc.desc: Verify SetCanvasInfo handles zero values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCanvasInfo_ZeroValues, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 0.0f;
    canvasInfo.geoHeight = 0.0f;
    geVisualEffectImpl.SetCanvasInfo(canvasInfo);
    
    auto retrievedCanvasInfo = geVisualEffectImpl.GetCanvasInfo();
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoWidth, 0.0f);
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoHeight, 0.0f);
}

/**
 * @tc.name: SetCanvasInfo_NegativeValues
 * @tc.desc: Verify SetCanvasInfo handles negative values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCanvasInfo_NegativeValues, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = -100.0f;
    canvasInfo.geoHeight = -200.0f;
    geVisualEffectImpl.SetCanvasInfo(canvasInfo);
    
    auto retrievedCanvasInfo = geVisualEffectImpl.GetCanvasInfo();
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoWidth, -100.0f);
    EXPECT_FLOAT_EQ(retrievedCanvasInfo.geoHeight, -200.0f);
}

/**
 * @tc.name: SetCache_OverwriteExistingCache
 * @tc.desc: Verify SetCache overwrites existing cache data
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetCache_OverwriteExistingCache, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    
    auto cacheData1 = std::make_shared<std::any>(std::string("first_cache"));
    geVisualEffectImpl.SetCache(cacheData1);
    
    auto retrievedCache1 = geVisualEffectImpl.GetCache();
    EXPECT_NE(retrievedCache1, nullptr);
    EXPECT_EQ(std::any_cast<std::string>(*retrievedCache1), "first_cache");
    
    auto cacheData2 = std::make_shared<std::any>(std::string("second_cache"));
    geVisualEffectImpl.SetCache(cacheData2);
    
    auto retrievedCache2 = geVisualEffectImpl.GetCache();
    EXPECT_NE(retrievedCache2, nullptr);
    EXPECT_EQ(std::any_cast<std::string>(*retrievedCache2), "second_cache");
}

/**
 * @tc.name: SetFilterType_ChangeFilterType
 * @tc.desc: Verify SetFilterType changes filter type correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetFilterType_ChangeFilterType, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);
}

/**
 * @tc.name: MakeGetParams_AllRemainingFilters
 * @tc.desc: Verify Make and Get params for remaining filter types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_AllRemainingFilters, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_WATER_RIPPLE);
    geVisualEffectImpl1.MakeWaterRippleParams();
    auto params1 = geVisualEffectImpl1.GetWaterRippleParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_MAGNIFIER);
    geVisualEffectImpl2.MakeMagnifierParams();
    auto params2 = geVisualEffectImpl2.GetMagnifierParams();
    EXPECT_NE(params2, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_COLOR_GRADIENT);
    geVisualEffectImpl3.MakeColorGradientParams();
    auto params3 = geVisualEffectImpl3.GetColorGradientParams();
    EXPECT_NE(params3, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl4(Drawing::GE_FILTER_BEZIER_WARP);
    geVisualEffectImpl4.MakeBezierWarpParams();
    auto params4 = geVisualEffectImpl4.GetBezierWarpParams();
    EXPECT_NE(params4, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl5(Drawing::GE_FILTER_CONTENT_LIGHT);
    geVisualEffectImpl5.MakeContentLightParams();
    auto params5 = geVisualEffectImpl5.GetContentLightParams();
    EXPECT_NE(params5, nullptr);
}

/**
 * @tc.name: MakeGetParams_AllRemainingMasks
 * @tc.desc: Verify Make and Get params for remaining mask types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_AllRemainingMasks, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_MASK_WAVE_GRADIENT);
    geVisualEffectImpl1.MakeWaveGradientMaskParams();
    auto params1 = geVisualEffectImpl1.GetWaveGradientMaskParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_MASK_PIXEL_MAP);
    geVisualEffectImpl2.MakePixelMapMaskParams();
    auto params2 = geVisualEffectImpl2.GetPixelMapMaskParams();
    EXPECT_NE(params2, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_MASK_DISTURBANCE);
    geVisualEffectImpl3.MakeWaveDisturbanceMaskParams();
    auto params3 = geVisualEffectImpl3.GetWaveDisturbanceMaskParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_AllRemainingSDFShapes
 * @tc.desc: Verify Make and Get params for remaining SDF shape types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_AllRemainingSDFShapes, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_SHAPE_SDF_RRECT_SHAPE);
    geVisualEffectImpl1.MakeSDFRRectShapeParams();
    auto params1 = geVisualEffectImpl1.GetSDFRRectShapeParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHAPE_SDF_UNION_OP);
    geVisualEffectImpl2.MakeSDFUnionOpShapeParams();
    auto params2 = geVisualEffectImpl2.GetSDFUnionOpShapeParams();
    EXPECT_NE(params2, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_SHADER_SDF_COLOR);
    geVisualEffectImpl3.MakeSDFColorShaderParams();
    auto params3 = geVisualEffectImpl3.GetSDFColorShaderParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_ExtensionEffects
 * @tc.desc: Verify Make and Get params for extension effects
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_ExtensionEffects, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GEX_SHADER_LIGHT_CAVE);
    geVisualEffectImpl1.MakeLightCaveParams();
    auto params1 = geVisualEffectImpl1.GetLightCaveParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GEX_SHADER_DISTORT_CHROMA);
    geVisualEffectImpl2.MakeDistortChromaParams();
    auto params2 = geVisualEffectImpl2.GetDistortChromaParams();
    EXPECT_NE(params2, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GEX_MASK_DUPOLI_NOISE);
    geVisualEffectImpl3.MakeDupoliNoiseMaskParams();
    auto params3 = geVisualEffectImpl3.GetDupoliNoiseMaskParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_ParticleAblation
 * @tc.desc: Verify Make and Get params for particle ablation filter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_ParticleAblation, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_PARTICLE_ABLATION);
    geVisualEffectImpl.MakeParticleAblationFilterParams();
    auto params = geVisualEffectImpl.GetParticleAblationFilterParams();
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: MakeGetParams_GasifyFilters
 * @tc.desc: Verify Make and Get params for gasify filter types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_GasifyFilters, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_GASIFY_SCALE_TWIST);
    geVisualEffectImpl1.MakeGasifyScaleTwistFilterParams();
    auto params1 = geVisualEffectImpl1.GetGasifyScaleTwistFilterParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GASIFY_BLUR);
    geVisualEffectImpl2.MakeGasifyBlurFilterParams();
    auto params2 = geVisualEffectImpl2.GetGasifyBlurFilterParams();
    EXPECT_NE(params2, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_GASIFY);
    geVisualEffectImpl3.MakeGasifyFilterParams();
    auto params3 = geVisualEffectImpl3.GetGasifyFilterParams();
    EXPECT_NE(params3, nullptr);
}

/**
 * @tc.name: MakeGetParams_DistortionCollapse
 * @tc.desc: Verify Make and Get params for distortion collapse filter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_DistortionCollapse, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_DISTORTION_COLLAPSE);
    geVisualEffectImpl.MakeDistortionCollapseParams();
    auto params = geVisualEffectImpl.GetDistortionCollapseParams();
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: MakeGetParams_NoisyFrameGradientMask
 * @tc.desc: Verify Make and Get params for noisy frame gradient mask
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_NoisyFrameGradientMask, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GEX_MASK_NOISY_FRAME_GRADIENT);
    geVisualEffectImpl.MakeNoisyFrameGradientMaskParams();
    auto params = geVisualEffectImpl.GetNoisyFrameGradientMaskParams();
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: MakeGetParams_SDFTriangleAndDistort
 * @tc.desc: Verify Make and Get params for SDF triangle and distort op shapes
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_SDFTriangleAndDistort, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_SHAPE_SDF_TRIANGLE_SHAPE);
    geVisualEffectImpl1.MakeSDFTriangleShapeParams();
    auto params1 = geVisualEffectImpl1.GetSDFTriangleShapeParams();
    EXPECT_NE(params1, nullptr);
    
    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_SHAPE_SDF_DISTORT_OP_SHAPE);
    geVisualEffectImpl2.MakeSDFDistortOpShapeParams();
    auto params2 = geVisualEffectImpl2.GetSDFDistortOpShapeParams();
    EXPECT_NE(params2, nullptr);
}

/**
 * @tc.name: MakeGetParams_SDFEdgeLightEffect
 * @tc.desc: Verify Make and Get params for SDF edge light effect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeGetParams_SDFEdgeLightEffect, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_SDF_EDGE_LIGHT);
    geVisualEffectImpl.MakeSDFEdgeLightEffectParams();
    auto params = geVisualEffectImpl.GetSDFEdgeLightEffectParams();
    EXPECT_NE(params, nullptr);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
