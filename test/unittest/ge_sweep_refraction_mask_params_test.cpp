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
#include <gtest/gtest.h>

#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESweepRefractionMaskParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESweepRefractionMaskParamsTest::SetUpTestCase() {}
void GESweepRefractionMaskParamsTest::TearDownTestCase() {}

void GESweepRefractionMaskParamsTest::SetUp() {}
void GESweepRefractionMaskParamsTest::TearDown() {}

/**
 * @tc.name: ParamsDefaultValues_001
 * @tc.desc: Verify GEXSweepRefractionMaskParams default values
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ParamsDefaultValues_001, TestSize.Level1)
{
    GEXSweepRefractionMaskParams param;
    EXPECT_FLOAT_EQ(param.maskRadius, 0.0f);
    EXPECT_FLOAT_EQ(param.edgeThickness, 300.0f);
    EXPECT_FLOAT_EQ(param.refractAmount, 0.3f);
    EXPECT_FLOAT_EQ(param.rippleWidth, 0.4f);
    EXPECT_FLOAT_EQ(param.sweepOffset, 0.0f);
    EXPECT_FLOAT_EQ(param.chromaDelta, 0.08f);
    EXPECT_EQ(param.shapeType, 0);
    EXPECT_FLOAT_EQ(param.cornerRadius, 0.16f);
    EXPECT_FLOAT_EQ(param.prismWidth, 1.0f);
    EXPECT_FLOAT_EQ(param.prismHeight, 1.0f);
    EXPECT_FLOAT_EQ(param.sweepCenterX, 0.0f);
    EXPECT_FLOAT_EQ(param.sweepCenterY, 0.0f);
}

/**
 * @tc.name: ParamsReflection_001
 * @tc.desc: Verify GEParamsBuilder for SWEEP_REFRACTION_MASK filter type
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ParamsReflection_001, TestSize.Level1)
{
    auto filterType = GEParamsBuilder::GetFilterTypeFromString("SweepRefractionMask");
    EXPECT_EQ(filterType, GEFilterType::SWEEP_REFRACTION_MASK);
}

/**
 * @tc.name: ParamsReflection_002
 * @tc.desc: Verify parameter names for SWEEP_REFRACTION_MASK
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ParamsReflection_002, TestSize.Level1)
{
    // Verify key parameter names are recognized
    auto maskRadiusTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_MaskRadius");
    EXPECT_NE(maskRadiusTag, GEParamsMemberTag::INVALID);

    auto edgeThicknessTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_EdgeThickness");
    EXPECT_NE(edgeThicknessTag, GEParamsMemberTag::INVALID);

    auto refractAmountTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_RefractAmount");
    EXPECT_NE(refractAmountTag, GEParamsMemberTag::INVALID);

    auto rippleWidthTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_RippleWidth");
    EXPECT_NE(rippleWidthTag, GEParamsMemberTag::INVALID);

    auto sweepOffsetTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_SweepOffset");
    EXPECT_NE(sweepOffsetTag, GEParamsMemberTag::INVALID);

    auto chromaDeltaTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_ChromaDelta");
    EXPECT_NE(chromaDeltaTag, GEParamsMemberTag::INVALID);

    auto shapeTypeTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_ShapeType");
    EXPECT_NE(shapeTypeTag, GEParamsMemberTag::INVALID);

    auto cornerRadiusTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_CornerRadius");
    EXPECT_NE(cornerRadiusTag, GEParamsMemberTag::INVALID);

    auto prismWidthTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_PrismWidth");
    EXPECT_NE(prismWidthTag, GEParamsMemberTag::INVALID);

    auto prismHeightTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_PrismHeight");
    EXPECT_NE(prismHeightTag, GEParamsMemberTag::INVALID);

    auto sweepCenterXTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_SweepCenterX");
    EXPECT_NE(sweepCenterXTag, GEParamsMemberTag::INVALID);

    auto sweepCenterYTag = GEParamsMemberHelper::GEParamsMemberTagFromString("SweepRefractionMask_SweepCenterY");
    EXPECT_NE(sweepCenterYTag, GEParamsMemberTag::INVALID);
}

/**
 * @tc.name: ValidateAndSet_001
 * @tc.desc: Verify GEVisualEffectImpl SetParam for SWEEP_REFRACTION_MASK parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ValidateAndSet_001, TestSize.Level1)
{
    GEVisualEffectImpl sweepRefraction("SweepRefractionMask");

    // Test setting maskRadius
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_MASK_RADIUS, 1.5f);

    // Test setting shapeType
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_SHAPE_TYPE, 1);

    // Test setting cornerRadius
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_CORNER_RADIUS, 16.0f);

    // Test setting prismWidth
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_PRISM_WIDTH, 360.0f);

    // Test setting prismHeight
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_PRISM_HEIGHT, 140.0f);

    // Test setting sweepCenterX
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_SWEEP_CENTER_X, 0.5f);

    // Test setting sweepCenterY
    sweepRefraction.SetParam(GE_SHADER_SWEEP_REFRACTION_MASK_SWEEP_CENTER_Y, 0.5f);

    auto params = sweepRefraction.GetParams<GEXSweepRefractionMaskParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(sweepRefraction.GetFilterType(), GEFilterType::SWEEP_REFRACTION_MASK);
    EXPECT_FLOAT_EQ(params->maskRadius, 1.5f);
    EXPECT_EQ(params->shapeType, 1);
    EXPECT_FLOAT_EQ(params->cornerRadius, 16.0f);
    EXPECT_FLOAT_EQ(params->prismWidth, 360.0f);
    EXPECT_FLOAT_EQ(params->prismHeight, 140.0f);
    EXPECT_FLOAT_EQ(params->sweepCenterX, 0.5f);
    EXPECT_FLOAT_EQ(params->sweepCenterY, 0.5f);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS