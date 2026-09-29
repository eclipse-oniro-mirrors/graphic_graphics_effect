/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <memory>

#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFRRectShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESDFRRectShaderShapeTest::SetUpTestCase(void) {}
void GESDFRRectShaderShapeTest::TearDownTestCase(void) {}

void GESDFRRectShaderShapeTest::SetUp() {}
void GESDFRRectShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null when rectangle width is too small
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_002 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 0.00005f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader returns null when rectangle height is too small
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_003 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 0.00005f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShader_004
 * @tc.desc: Verify GenerateDrawingShader supports per-corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShader_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_004 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 120.0f, 90.0f};
    param.rrect.SetCornerRadius(Vector4f(4.0f, 12.0f, 20.0f, 8.0f));

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(120.0f, 90.0f);
    EXPECT_NE(shader, nullptr);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::TOP_LEFT].x_, 4.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::TOP_RIGHT].x_, 12.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::BOTTOM_RIGHT].x_, 20.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::BOTTOM_LEFT].x_, 8.0f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_004 end";
}

/**
 * @tc.name: GenerateDrawingShader_005
 * @tc.desc: Verify GenerateDrawingShader supports elliptical per-corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShader_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_005 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 120.0f, 90.0f};
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(6.0f, 12.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(18.0f, 10.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(14.0f, 22.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(8.0f, 16.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(120.0f, 90.0f);
    EXPECT_NE(shader, nullptr);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::TOP_LEFT].x_, 6.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::TOP_LEFT].y_, 12.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::BOTTOM_RIGHT].x_, 14.0f);
    EXPECT_FLOAT_EQ(shape.GetRRect().radius_[GERRect::BOTTOM_RIGHT].y_, 22.0f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_005 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null when rectangle dimensions are invalid
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 0.00005f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns correct type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFShapeType_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::RRECT);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetRRect_001
 * @tc.desc: Verify GetRRect returns correct rectangle parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GetRRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetRRect_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f};
    param.rrect.SetCornerRadius(15.0f, 15.0f);

    GESDFRRectShaderShape shape(param);
    const auto& rrect = shape.GetRRect();

    EXPECT_FLOAT_EQ(rrect.left_, 10.0f);
    EXPECT_FLOAT_EQ(rrect.top_, 20.0f);
    EXPECT_FLOAT_EQ(rrect.width_, 100.0f);
    EXPECT_FLOAT_EQ(rrect.height_, 200.0f);
    EXPECT_FLOAT_EQ(rrect.radius_[GERRect::TOP_LEFT].x_, 15.0f);
    EXPECT_FLOAT_EQ(rrect.radius_[GERRect::TOP_LEFT].y_, 15.0f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetRRect_001 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CopyState_001 start";
    GESDFRRectShapeParams param1;
    param1.rrect = {10.0f, 20.0f, 100.0f, 200.0f};
    param1.rrect.SetCornerRadius(15.0f, 15.0f);

    GESDFRRectShapeParams param2;
    param2.rrect = {5.0f, 10.0f, 50.0f, 100.0f};
    param2.rrect.SetCornerRadius(8.0f, 8.0f);

    GESDFRRectShaderShape shape1(param1);
    GESDFRRectShaderShape shape2(param2);

    // Verify initial state is different
    EXPECT_FLOAT_EQ(shape1.GetRRect().left_, 10.0f);
    EXPECT_FLOAT_EQ(shape2.GetRRect().left_, 5.0f);

    // Copy state and verify
    shape1.CopyState(shape2);
    EXPECT_FLOAT_EQ(shape1.GetRRect().left_, 5.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().top_, 10.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().width_, 50.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().height_, 100.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::TOP_LEFT].x_, 8.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::TOP_LEFT].y_, 8.0f);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: CopyState_002
 * @tc.desc: Verify CopyState preserves per-corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CopyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CopyState_002 start";
    GESDFRRectShapeParams param1;
    param1.rrect = {10.0f, 20.0f, 100.0f, 200.0f};
    param1.rrect.SetCornerRadius(15.0f, 15.0f);

    GESDFRRectShapeParams param2;
    param2.rrect = {5.0f, 10.0f, 50.0f, 100.0f};
    param2.rrect.SetCornerRadius(8.0f, 8.0f);
    param2.rrect.SetCornerRadius(Vector4f(2.0f, 4.0f, 6.0f, 8.0f));

    GESDFRRectShaderShape shape1(param1);
    GESDFRRectShaderShape shape2(param2);

    shape1.CopyState(shape2);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::TOP_LEFT].x_, 2.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::TOP_RIGHT].x_, 4.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::BOTTOM_RIGHT].x_, 6.0f);
    EXPECT_FLOAT_EQ(shape1.GetRRect().radius_[GERRect::BOTTOM_LEFT].x_, 8.0f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CopyState_002 end";
}

/**
 * @tc.name: GetSDFRRectShaderShapeBuilder_001
 * @tc.desc: Verify GetSDFRRectShaderShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GetSDFRRectShaderShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFRRectShaderShapeBuilder_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto builder = shape.GetSDFRRectShaderShapeBuilder();
    EXPECT_NE(builder, nullptr);
    // if created, it will reuse builder
    EXPECT_NE(shape.GetSDFRRectShaderShapeBuilder(), nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFRRectShaderShapeBuilder_001 end";
}

/**
 * @tc.name: GetSDFRRectNormalShapeBuilder_001
 * @tc.desc: Verify GetSDFRRectNormalShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GetSDFRRectNormalShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFRRectNormalShapeBuilder_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    auto builder = shape.GetSDFRRectNormalShapeBuilder();
    EXPECT_NE(builder, nullptr);
    // if created, it will reuse builder
    EXPECT_NE(shape.GetSDFRRectNormalShapeBuilder(), nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFRRectNormalShapeBuilder_001 end";
}

/**
 * @tc.name: UseUniformRadiusFastPath_001
 * @tc.desc: Verify the uniform circular case uses the uniform-radius fast path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, UseUniformRadiusFastPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);

    GESDFRRectShaderShape shape(param);
    EXPECT_TRUE(shape.UseUniformRadiusFastPath());
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_001 end";
}

/**
 * @tc.name: UseUniformRadiusFastPath_002
 * @tc.desc: Verify per-corner radii use the generic rounded-rect path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, UseUniformRadiusFastPath_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_002 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 120.0f, 90.0f};
    param.rrect.SetCornerRadius(Vector4f(4.0f, 12.0f, 20.0f, 8.0f));

    GESDFRRectShaderShape shape(param);
    EXPECT_FALSE(shape.UseUniformRadiusFastPath());
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_002 end";
}

/**
 * @tc.name: UseUniformRadiusFastPath_003
 * @tc.desc: Verify uniform elliptical radii still use the generic rounded-rect path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, UseUniformRadiusFastPath_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_003 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 120.0f, 90.0f};
    param.rrect.SetCornerRadius(12.0f, 20.0f);

    GESDFRRectShaderShape shape(param);
    EXPECT_FALSE(shape.UseUniformRadiusFastPath());
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest UseUniformRadiusFastPath_003 end";
}

/**
 * @tc.name: GenerateShaderEffect_001
 * @tc.desc: Verify GenerateShaderEffect sets uniforms correctly
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateShaderEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateShaderEffect_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f};
    param.rrect.SetCornerRadius(15.0f, 15.0f);

    GESDFRRectShaderShape shape(param);
    auto builder = shape.GetSDFRRectShaderShapeBuilder();
    auto shader = shape.GenerateShaderEffect(builder, false);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateShaderEffect_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_002
 * @tc.desc: Verify GenerateShaderEffect returns null for invalid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GenerateShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateShaderEffect_002 start";
    GESDFRRectShapeParams param;
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f};
    param.rrect.SetCornerRadius(15.0f, 15.0f);

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateShaderEffect(nullptr, true);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateShaderEffect_002 end";
}

/**
 * @tc.name: ResolveCornerRadii_001
 * @tc.desc: Verify oversized per-corner radii are clamped to maxRadius independently
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveCornerRadii_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadii_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 60.0f};
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(80.0f, 35.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(60.0f, 15.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(40.0f, 40.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(20.0f, 30.0f);

    GESDFRRectShaderShape shape(param);
    auto radii = shape.ResolveCornerRadii(50.0f + 0.5f, 30.0f + 0.5f);
    // maxRadius = min(50.5, 30.5) = 30.5
    EXPECT_LE(radii[GERRect::TOP_LEFT].x_ + radii[GERRect::TOP_RIGHT].x_, 101.0f);
    EXPECT_LE(radii[GERRect::BOTTOM_LEFT].x_ + radii[GERRect::BOTTOM_RIGHT].x_, 101.0f);
    EXPECT_LE(radii[GERRect::TOP_LEFT].y_ + radii[GERRect::BOTTOM_LEFT].y_, 61.0f);
    EXPECT_LE(radii[GERRect::TOP_RIGHT].y_ + radii[GERRect::BOTTOM_RIGHT].y_, 61.0f);
    // Circular corners (rx == ry before overflow) keep circular after clamp
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, radii[GERRect::TOP_LEFT].y_);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, radii[GERRect::BOTTOM_RIGHT].y_);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadii_001 end";
}

/**
 * @tc.name: ResolveCornerRadiiNegativeRadius
 * @tc.desc: Verify negative radius components are clamped to zero
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveCornerRadiiNegativeRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiNegativeRadius start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(-10.0f, -5.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(-3.0f, 10.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(10.0f, -20.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(0.0f, 0.0f);

    GESDFRRectShaderShape shape(param);
    auto radii = shape.ResolveCornerRadii(50.0f + 0.5f, 50.0f + 0.5f);
    // Negative + EXTEND < 0 → clamped to 0
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, 0.0f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].y_, 0.0f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].x_, 0.0f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].y_, 0.0f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, 10.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].y_, 10.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].x_, 0.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].y_, 0.5f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiNegativeRadius end";
}

/**
 * @tc.name: ResolveCornerRadiiSingleComponentOverflow
 * @tc.desc: Verify only the overflowing component is clamped, others remain unchanged
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveCornerRadiiSingleComponentOverflow, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiSingleComponentOverflow start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 60.0f};
    // Only TOP_LEFT.x overflows; all other components are within bounds
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(80.0f, 10.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(10.0f, 10.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(15.0f, 15.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(5.0f, 5.0f);

    GESDFRRectShaderShape shape(param);
    auto radii = shape.ResolveCornerRadii(50.0f + 0.5f, 30.0f + 0.5f);
    // maxRadius = min(50.5, 30.5) = 30.5
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, 30.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].y_, 30.5f);
    // Unaffected corners keep their value + EXTEND
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].x_, 10.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].y_, 10.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, 15.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].y_, 15.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].x_, 5.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].y_, 5.5f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiSingleComponentOverflow end";
}

/**
 * @tc.name: ResolveCornerRadiiMultiCornerIndependentClamp
 * @tc.desc: Verify each corner is independently clamped to maxRadius without cross-corner scaling
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveCornerRadiiMultiCornerIndependentClamp, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiMultiCornerIndependentClamp start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    // maxRadius = 50.5; different corners overflow on different axes
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(60.0f, 60.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(30.0f, 70.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(80.0f, 40.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(20.0f, 20.0f);

    GESDFRRectShaderShape shape(param);
    auto radii = shape.ResolveCornerRadii(50.0f + 0.5f, 50.0f + 0.5f);
    // Each corner clamped independently to [0, 50.5]
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, 50.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].y_, 50.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].x_, 30.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].y_, 30.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, 50.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].y_, 50.5f);
    // BOTTOM_LEFT within bounds, unaffected by other corners' overflow
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].x_, 20.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].y_, 20.5f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiMultiCornerIndependentClamp end";
}

/**
 * @tc.name: ResolveCornerRadiiCircularPreservation
 * @tc.desc: Verify circular corners (rx == ry) stay circular after clamping to maxRadius
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveCornerRadiiCircularPreservation, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiCircularPreservation start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 60.0f};
    // All corners are circular (rx == ry); three overflow, one does not
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(60.0f, 60.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(40.0f, 40.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(50.0f, 50.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(20.0f, 20.0f);

    GESDFRRectShaderShape shape(param);
    auto radii = shape.ResolveCornerRadii(50.0f + 0.5f, 30.0f + 0.5f);
    // maxRadius = 30.5; circular corners remain circular after independent clamp
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, radii[GERRect::TOP_LEFT].y_);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].x_, radii[GERRect::TOP_RIGHT].y_);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, radii[GERRect::BOTTOM_RIGHT].y_);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].x_, radii[GERRect::BOTTOM_LEFT].y_);
    // Overflowing circular corners clamp to maxRadius
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_LEFT].x_, 30.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::TOP_RIGHT].x_, 30.5f);
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_RIGHT].x_, 30.5f);
    // Non-overflowing corner keeps its value + EXTEND
    EXPECT_FLOAT_EQ(radii[GERRect::BOTTOM_LEFT].x_, 20.5f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveCornerRadiiCircularPreservation end";
}

/**
 * @tc.name: ResolveUniformRadius_001
 * @tc.desc: Verify oversized uniform radii are clamped to the rect bounds
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, ResolveUniformRadius_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveUniformRadius_001 start";
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 40.0f, 20.0f};
    param.rrect.SetCornerRadius(30.0f, 30.0f);

    GESDFRRectShaderShape shape(param);
    float radius = shape.ResolveUniformRadius(20.0f + 0.5f, 10.0f + 0.5f);
    EXPECT_FLOAT_EQ(radius, 10.5f);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest ResolveUniformRadius_001 end";
}

/**
 * @tc.name: GetInscribedRectWithCorners
 * @tc.desc: Verify GetInscribedRect returns true and computes inscribed rect from corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, GetInscribedRectWithCorners, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetInscribedRectWithCorners start";
    GESDFRRectShapeParams param;
    param.rrect = {10.0f, 20.0f, 200.0f, 100.0f};
    param.rrect.radius_[GERRect::TOP_LEFT] = Vector2f(20.0f, 15.0f);
    param.rrect.radius_[GERRect::TOP_RIGHT] = Vector2f(0.0f, 10.0f);
    param.rrect.radius_[GERRect::BOTTOM_RIGHT] = Vector2f(40.0f, 20.0f);
    param.rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(0.0f, 25.0f);

    GESDFRRectShaderShape shape(param);
    Drawing::Rect rect;
    EXPECT_TRUE(shape.GetInscribedRect(rect));
    EXPECT_FLOAT_EQ(rect.left_, 20.25f);
    EXPECT_FLOAT_EQ(rect.right_, 189.75f);

    shape.params_.rrect = {10.0f, 20.0f, 0.0f, 100.0f};
    EXPECT_FALSE(shape.GetInscribedRect(rect));
    shape.params_.rrect = {10.0f, 20.0f, 200.0f, 0.0f};
    EXPECT_FALSE(shape.GetInscribedRect(rect));

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetInscribedRectWithCorners end";
}

/**
 * @tc.name: CanBeContinuous_UniformCircularRadii
 * @tc.desc: Verify CanBeContinuous returns true for uniform circular corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_UniformCircularRadii, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_UniformCircularRadii start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(10.0f, 10.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_UniformCircularRadii end";
}

/**
 * @tc.name: CanBeContinuous_NonUniformRadii
 * @tc.desc: Verify CanBeContinuous returns false for non-circular (elliptical) corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_NonUniformRadii, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_NonUniformRadii start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(10.0f, 20.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_NonUniformRadii end";
}

/**
 * @tc.name: CanBeContinuous_LargeRadiusExceedsLimit
 * @tc.desc: Verify CanBeContinuous returns false when the radius turns the rect into a circle
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_LargeRadiusExceedsLimit, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_LargeRadiusExceedsLimit start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(80.0f, 80.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_LargeRadiusExceedsLimit end";
}

/**
 * @tc.name: CanBeContinuous_ZeroRadius
 * @tc.desc: Verify CanBeContinuous returns false when the corner radius is not positive
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_ZeroRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_ZeroRadius start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(0.0f, 0.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_ZeroRadius end";
}

/**
 * @tc.name: CanBeContinuous_NonUniformCornerRadii
 * @tc.desc: Verify CanBeContinuous returns false for non-uniform corner radii even if circular
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_NonUniformCornerRadii, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_NonUniformCornerRadii start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(10.0f, 10.0f);
    params->rrect.radius_[GERRect::BOTTOM_LEFT] = Vector2f(15.0f, 15.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_NonUniformCornerRadii end";
}

/**
 * @tc.name: CanBeContinuous_CircleByPixelTolerance
 * @tc.desc: Verify CanBeContinuous returns false when the radius is excluded by the pixel tolerance
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_CircleByPixelTolerance, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_CircleByPixelTolerance start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(49.5f, 49.5f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_CircleByPixelTolerance end";
}

/**
 * @tc.name: CanBeContinuous_CapsuleRadiusClamped
 * @tc.desc: Verify CanBeContinuous clamps the capsule radius to half of the min side and returns true
 * @tc.type: FUNC
 */
HWTEST_F(GESDFRRectShaderShapeTest, CanBeContinuous_CapsuleRadiusClamped, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_CapsuleRadiusClamped start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 50.0f };
    params->rrect.SetCornerRadius(26.0f, 26.0f);

    bool result = GESDFRRectShaderShape::CanBeContinuous(params);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(params->rrect.GetCommonRadiusX(), 25.0f);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CanBeContinuous_CapsuleRadiusClamped end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
