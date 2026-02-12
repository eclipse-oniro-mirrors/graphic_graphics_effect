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
#include "ge_sdf_union_op_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFUnionOpShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<GESDFShaderShape> CreateTestShape() const
    {
        GESDFRRectShapeParams param;
        param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
        return std::make_shared<GESDFRRectShaderShape>(param);
    }
};

void GESDFUnionOpShaderShapeTest::SetUpTestCase(void) {}
void GESDFUnionOpShaderShapeTest::TearDownTestCase(void) {}

void GESDFUnionOpShaderShapeTest::SetUp() {}
void GESDFUnionOpShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader with both left and right shapes for UNION operation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with both left and right shapes for SMOOTH_UNION operation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_002 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;
    param.spacing = 5.0f;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader returns null when both shapes are null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_003 start";
    GESDFUnionOpShapeParams param;
    param.left = nullptr;
    param.right = nullptr;
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShader_004
 * @tc.desc: Verify GenerateDrawingShader with only left shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShader_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_004 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = nullptr;
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_004 end";
}

/**
 * @tc.name: GenerateDrawingShader_005
 * @tc.desc: Verify GenerateDrawingShader with only right shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShader_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_005 start";
    GESDFUnionOpShapeParams param;
    param.left = nullptr;
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShader_005 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with both shapes for UNION operation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with both shapes for SMOOTH_UNION operation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;
    param.spacing = 5.0f;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null when both shapes are null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_003 start";
    GESDFUnionOpShapeParams param;
    param.left = nullptr;
    param.right = nullptr;
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateDrawingShaderHasNormal_003 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns correct type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFShapeType_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::UNION_OP);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetSpacing_001
 * @tc.desc: Verify GetSpacing returns correct spacing value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSpacing_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSpacing_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;
    param.spacing = 8.5f;

    GESDFUnionOpShaderShape shape(param);
    EXPECT_FLOAT_EQ(shape.GetSpacing(), 8.5f);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSpacing_001 end";
}

/**
 * @tc.name: GetLeftSDFShape_001
 * @tc.desc: Verify GetLeftSDFShape returns correct left shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetLeftSDFShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetLeftSDFShape_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto leftShape = shape.GetLeftSDFShape();
    EXPECT_NE(leftShape, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetLeftSDFShape_001 end";
}

/**
 * @tc.name: GetRightSDFShape_001
 * @tc.desc: Verify GetRightSDFShape returns correct right shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetRightSDFShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetRightSDFShape_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto rightShape = shape.GetRightSDFShape();
    EXPECT_NE(rightShape, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetRightSDFShape_001 end";
}

/**
 * @tc.name: GetSDFUnionOp_001
 * @tc.desc: Verify GetSDFUnionOp returns correct operation type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSDFUnionOp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFUnionOp_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;

    GESDFUnionOpShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFUnionOp(), GESDFUnionOp::SMOOTH_UNION);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFUnionOp_001 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest CopyState_001 start";
    GESDFUnionOpShapeParams param1;
    param1.left = CreateTestShape();
    param1.right = CreateTestShape();
    param1.op = GESDFUnionOp::UNION;
    param1.spacing = 5.0f;

    GESDFUnionOpShapeParams param2;
    param2.left = CreateTestShape();
    param2.right = CreateTestShape();
    param2.op = GESDFUnionOp::SMOOTH_UNION;
    param2.spacing = 10.0f;

    GESDFUnionOpShaderShape shape1(param1);
    GESDFUnionOpShaderShape shape2(param2);

    // Verify initial state is different
    EXPECT_EQ(shape1.GetSDFUnionOp(), GESDFUnionOp::UNION);
    EXPECT_FLOAT_EQ(shape1.GetSpacing(), 5.0f);
    EXPECT_EQ(shape2.GetSDFUnionOp(), GESDFUnionOp::SMOOTH_UNION);
    EXPECT_FLOAT_EQ(shape2.GetSpacing(), 10.0f);

    // Copy state and verify
    shape1.CopyState(shape2);
    EXPECT_EQ(shape1.GetSDFUnionOp(), GESDFUnionOp::SMOOTH_UNION);
    EXPECT_FLOAT_EQ(shape1.GetSpacing(), 10.0f);

    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: GetSDFUnionBuilder_001
 * @tc.desc: Verify GetSDFUnionBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSDFUnionBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFUnionBuilder_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto builder = shape.GetSDFUnionBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFUnionBuilder_001 end";
}

/**
 * @tc.name: GetSDFSmoothUnionBuilder_001
 * @tc.desc: Verify GetSDFSmoothUnionBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSDFSmoothUnionBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFSmoothUnionBuilder_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;

    GESDFUnionOpShaderShape shape(param);
    auto builder = shape.GetSDFSmoothUnionBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFSmoothUnionBuilder_001 end";
}

/**
 * @tc.name: GetSDFNormalSmoothUnionBuilder_001
 * @tc.desc: Verify GetSDFNormalSmoothUnionBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GetSDFNormalSmoothUnionBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFNormalSmoothUnionBuilder_001 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::SMOOTH_UNION;

    GESDFUnionOpShaderShape shape(param);
    auto builder = shape.GetSDFNormalSmoothUnionBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GetSDFNormalSmoothUnionBuilder_001 end";
}

/**
 * @tc.name: GenerateUnionShaderEffect_002
 * @tc.desc: Verify GenerateUnionShaderEffect returns null for invalid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUnionOpShaderShapeTest, GenerateUnionShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateUnionShaderEffect_002 start";
    GESDFUnionOpShapeParams param;
    param.left = CreateTestShape();
    param.right = CreateTestShape();
    param.op = GESDFUnionOp::UNION;

    GESDFUnionOpShaderShape shape(param);
    auto leftShader = param.left->GenerateDrawingShader(100.0f, 100.0f);
    auto rightShader = param.right->GenerateDrawingShader(100.0f, 100.0f);

    auto shader = shape.GenerateUnionShaderEffect(leftShader, rightShader, nullptr);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFUnionOpShaderShapeTest GenerateUnionShaderEffect_002 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS