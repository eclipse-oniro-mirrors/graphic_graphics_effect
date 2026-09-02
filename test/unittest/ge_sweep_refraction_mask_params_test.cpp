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
#include "ge_params_reflection.h"

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
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsDefaultValues_001 start";
    GEXSweepRefractionMaskParams param;
    EXPECT_FLOAT_EQ(param.maskRadius_, 0.0f);
    EXPECT_FLOAT_EQ(param.edgeThickness_, 300.0f);
    EXPECT_FLOAT_EQ(param.refractAmount_, 0.3f);
    EXPECT_FLOAT_EQ(param.rippleWidth_, 0.4f);
    EXPECT_FLOAT_EQ(param.sweepOffset_, 0.0f);
    EXPECT_FLOAT_EQ(param.chromaDelta_, 0.08f);
    EXPECT_EQ(param.shapeType_, 0);
    EXPECT_FLOAT_EQ(param.cornerRadius_, 0.16f);
    EXPECT_FLOAT_EQ(param.prismWidth_, 1.0f);
    EXPECT_FLOAT_EQ(param.prismHeight_, 1.0f);
    EXPECT_FLOAT_EQ(param.sweepCenterX_, 0.0f);
    EXPECT_FLOAT_EQ(param.sweepCenterY_, 0.0f);
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsDefaultValues_001 end";
}

/**
 * @tc.name: ParamsReflection_001
 * @tc.desc: Verify GEParamsReflection for SWEEP_REFRACTION_MASK parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ParamsReflection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsReflection_001 start";
    auto filterType = GEGetFilterType(GEFilterType::SWEEP_REFRACTION_MASK);
    EXPECT_EQ(filterType, GEFilterType::SWEEP_REFRACTION_MASK);
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsReflection_001 end";
}

/**
 * @tc.name: ParamsReflection_002
 * @tc.desc: Verify parameter names for SWEEP_REFRACTION_MASK
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ParamsReflection_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsReflection_002 start";
    // Verify key parameter names are recognized
    auto maskRadiusTag = GEStringToTag("SweepRefractionMaskMaskRadius");
    EXPECT_NE(maskRadiusTag, static_cast<uint32_t>(-1));

    auto edgeThicknessTag = GEStringToTag("SweepRefractionMaskEdgeThickness");
    EXPECT_NE(edgeThicknessTag, static_cast<uint32_t>(-1));

    auto refractAmountTag = GEStringToTag("SweepRefractionMaskRefractAmount");
    EXPECT_NE(refractAmountTag, static_cast<uint32_t>(-1));

    auto rippleWidthTag = GEStringToTag("SweepRefractionMaskRippleWidth");
    EXPECT_NE(rippleWidthTag, static_cast<uint32_t>(-1));

    auto sweepOffsetTag = GEStringToTag("SweepRefractionMaskSweepOffset");
    EXPECT_NE(sweepOffsetTag, static_cast<uint32_t>(-1));

    auto chromaDeltaTag = GEStringToTag("SweepRefractionMaskChromaDelta");
    EXPECT_NE(chromaDeltaTag, static_cast<uint32_t>(-1));

    auto shapeTypeTag = GEStringToTag("SweepRefractionMaskShapeType");
    EXPECT_NE(shapeTypeTag, static_cast<uint32_t>(-1));

    auto cornerRadiusTag = GEStringToTag("SweepRefractionMaskCornerRadius");
    EXPECT_NE(cornerRadiusTag, static_cast<uint32_t>(-1));

    auto prismWidthTag = GEStringToTag("SweepRefractionMaskPrismWidth");
    EXPECT_NE(prismWidthTag, static_cast<uint32_t>(-1));

    auto prismHeightTag = GEStringToTag("SweepRefractionMaskPrismHeight");
    EXPECT_NE(prismHeightTag, static_cast<uint32_t>(-1));

    auto sweepCenterXTag = GEStringToTag("SweepRefractionMaskSweepCenterX");
    EXPECT_NE(sweepCenterXTag, static_cast<uint32_t>(-1));

    auto sweepCenterYTag = GEStringToTag("SweepRefractionMaskSweepCenterY");
    EXPECT_NE(sweepCenterYTag, static_cast<uint32_t>(-1));
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ParamsReflection_002 end";
}

/**
 * @tc.name: ValidateAndSet_001
 * @tc.desc: Verify GEValidateAndSet for SWEEP_REFRACTION_MASK parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESweepRefractionMaskParamsTest, ValidateAndSet_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ValidateAndSet_001 started";
    GEXSweepRefractionMaskParams param;
    auto visualEffect = std::make_shared<GEVisualEffect>("SweepRefractionMask");

    // Test setting maskRadius
    bool ret = GEValidateAndSet("SweepRefractionMaskMaskRadius", 1.5f, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting shapeType
    ret = GEValidateAndSet("SweepRefractionMaskShapeType", 1, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting cornerRadius
    ret = GEValidateAndSet("SweepRefractionMaskCornerRadius", 16.0f, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting prismWidth
    ret = GEValidateAndSet("SweepRefractionMaskPrismWidth", 360.0f, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting prismHeight
    ret = GEValidateAndSet("SweepRefractionMaskPrismHeight", 140.0f, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting sweepCenterX
    ret = GEValidateAndSet("SweepRefractionMaskSweepCenterX", 0.5f, visualEffect);
    EXPECT_TRUE(ret);

    // Test setting sweepCenterY
    ret = GEValidateAndSet("SweepRefractionMaskSweepCenterY", 0.5f, visualEffect);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "GESweepRefractionMaskParamsTest ValidateAndSet_001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS