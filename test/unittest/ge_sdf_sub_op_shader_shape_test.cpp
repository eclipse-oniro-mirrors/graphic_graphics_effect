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
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_sub_op_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr float TEST_WIDTH = 100.0f;
constexpr float TEST_HEIGHT = 100.0f;
constexpr float TEST_RADIUS = 10.0f;
constexpr float TEST_SPACING = 5.0f;
constexpr float INVALID_SPACING = 0.0f;

class CountingSDFShaderShape : public GESDFShaderShape {
public:
    void Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal) override
    {
        (void)canvas;
        (void)rect;
        ++preprocessCount_;
        lastHasNormal_ = hasNormal;
    }

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::RRECT;
    }

    bool HasType(const GESDFShapeType type) const override
    {
        return type == GESDFShapeType::RRECT;
    }

    int preprocessCount_ = 0;
    bool lastHasNormal_ = false;
};
} // namespace

class GESDFSubOpShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<GESDFShaderShape> CreateTestShape() const
    {
        GESDFRRectShapeParams param;
        param.rrect = {0.0f, 0.0f, TEST_WIDTH, TEST_HEIGHT};
        param.rrect.SetCornerRadius(TEST_RADIUS, TEST_RADIUS);
        return std::make_shared<GESDFRRectShaderShape>(param);
    }
};

void GESDFSubOpShaderShapeTest::SetUpTestCase(void) {}
void GESDFSubOpShaderShapeTest::TearDownTestCase(void) {}
void GESDFSubOpShaderShapeTest::SetUp() {}
void GESDFSubOpShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify SDF sub drawing shader null and valid child branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest GenerateDrawingShader_001 start";
    GESDFSubOpShapeParams param;
    GESDFSubOpShaderShape emptyShape(param);
    EXPECT_EQ(emptyShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.right = CreateTestShape();
    GESDFSubOpShaderShape onlyRightShape(param);
    EXPECT_EQ(onlyRightShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.left = CreateTestShape();
    param.right = nullptr;
    GESDFSubOpShaderShape onlyLeftShape(param);
    EXPECT_NE(onlyLeftShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.right = CreateTestShape();
    GESDFSubOpShaderShape bothShape(param);
    EXPECT_NE(bothShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify SDF sub normal drawing shader null and valid child branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFSubOpShapeParams param;
    param.right = CreateTestShape();
    GESDFSubOpShaderShape onlyRightShape(param);
    EXPECT_EQ(onlyRightShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.left = CreateTestShape();
    param.right = nullptr;
    GESDFSubOpShaderShape onlyLeftShape(param);
    EXPECT_NE(onlyLeftShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.right = CreateTestShape();
    GESDFSubOpShaderShape bothShape(param);
    EXPECT_NE(bothShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: SmoothGenerateDrawingShader_001
 * @tc.desc: Verify smooth SDF sub drawing shader null, valid child, and spacing branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, SmoothGenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothGenerateDrawingShader_001 start";
    GESDFSmoothSubOpShapeParams param;
    param.right = CreateTestShape();
    GESDFSmoothSubOpShaderShape onlyRightShape(param);
    EXPECT_EQ(onlyRightShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.left = CreateTestShape();
    param.right = nullptr;
    GESDFSmoothSubOpShaderShape onlyLeftShape(param);
    EXPECT_NE(onlyLeftShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.right = CreateTestShape();
    param.spacing = INVALID_SPACING;
    GESDFSmoothSubOpShaderShape invalidSpacingShape(param);
    EXPECT_NE(invalidSpacingShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.spacing = TEST_SPACING;
    GESDFSmoothSubOpShaderShape bothShape(param);
    EXPECT_NE(bothShape.GenerateDrawingShader(TEST_WIDTH, TEST_HEIGHT), nullptr);
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothGenerateDrawingShader_001 end";
}

/**
 * @tc.name: SmoothGenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify smooth SDF sub normal drawing shader null, valid child, and spacing branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, SmoothGenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothGenerateDrawingShaderHasNormal_001 start";
    GESDFSmoothSubOpShapeParams param;
    param.right = CreateTestShape();
    GESDFSmoothSubOpShaderShape onlyRightShape(param);
    EXPECT_EQ(onlyRightShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.left = CreateTestShape();
    param.right = nullptr;
    GESDFSmoothSubOpShaderShape onlyLeftShape(param);
    EXPECT_NE(onlyLeftShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);

    param.right = CreateTestShape();
    param.spacing = INVALID_SPACING;
    GESDFSmoothSubOpShaderShape invalidSpacingShape(param);
    EXPECT_NE(invalidSpacingShape.GenerateDrawingShaderHasNormal(TEST_WIDTH, TEST_HEIGHT), nullptr);
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothGenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: ShapeState_001
 * @tc.desc: Verify sub shape type, HasType, Preprocess, and CopyState branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, ShapeState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest ShapeState_001 start";
    GESDFSubOpShapeParams param;
    param.left = CreateTestShape();
    GESDFSubOpShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::SUB_OP);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::SUB_OP));
    EXPECT_TRUE(shape.HasType(GESDFShapeType::RRECT));
    EXPECT_FALSE(shape.HasType(GESDFShapeType::DISTORT_OP));

    Canvas canvas;
    Rect rect = {0.0f, 0.0f, TEST_WIDTH, TEST_HEIGHT};
    shape.Preprocess(canvas, rect, false);

    GESDFSubOpShapeParams emptyParam;
    GESDFSubOpShaderShape emptyShape(emptyParam);
    emptyShape.CopyState(shape);
    EXPECT_TRUE(emptyShape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest ShapeState_001 end";
}

/**
 * @tc.name: BaseCopyStateSubOp_001
 * @tc.desc: Verify base SDF shader shape CopyState dispatches to sub op shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, BaseCopyStateSubOp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest BaseCopyStateSubOp_001 start";
    GESDFSubOpShapeParams param;
    param.left = CreateTestShape();
    GESDFSubOpShaderShape shape(param);

    GESDFSubOpShapeParams emptyParam;
    GESDFSubOpShaderShape emptyShape(emptyParam);
    EXPECT_FALSE(emptyShape.HasType(GESDFShapeType::RRECT));

    GESDFShaderShape& baseEmptyShape = emptyShape;
    const GESDFShaderShape& baseShape = shape;
    baseEmptyShape.CopyState(baseShape);
    EXPECT_TRUE(emptyShape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest BaseCopyStateSubOp_001 end";
}

/**
 * @tc.name: PreprocessRightShape_001
 * @tc.desc: Verify SDF sub Preprocess forwards to right child shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, PreprocessRightShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest PreprocessRightShape_001 start";
    auto rightShape = std::make_shared<CountingSDFShaderShape>();
    GESDFSubOpShapeParams param;
    param.right = rightShape;
    GESDFSubOpShaderShape shape(param);

    Canvas canvas;
    Rect rect = {0.0f, 0.0f, TEST_WIDTH, TEST_HEIGHT};
    shape.Preprocess(canvas, rect, true);
    EXPECT_EQ(rightShape->preprocessCount_, 1);
    EXPECT_TRUE(rightShape->lastHasNormal_);
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest PreprocessRightShape_001 end";
}

/**
 * @tc.name: SmoothShapeState_001
 * @tc.desc: Verify smooth sub shape type, HasType, Preprocess, and CopyState branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, SmoothShapeState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothShapeState_001 start";
    GESDFSmoothSubOpShapeParams param;
    param.left = CreateTestShape();
    GESDFSmoothSubOpShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::SMOOTH_SUB_OP);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::SMOOTH_SUB_OP));
    EXPECT_TRUE(shape.HasType(GESDFShapeType::RRECT));
    EXPECT_FALSE(shape.HasType(GESDFShapeType::DISTORT_OP));

    Canvas canvas;
    Rect rect = {0.0f, 0.0f, TEST_WIDTH, TEST_HEIGHT};
    shape.Preprocess(canvas, rect, true);

    GESDFSmoothSubOpShapeParams emptyParam;
    GESDFSmoothSubOpShaderShape emptyShape(emptyParam);
    emptyShape.CopyState(shape);
    EXPECT_TRUE(emptyShape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest SmoothShapeState_001 end";
}

/**
 * @tc.name: BaseCopyStateSmoothSubOp_001
 * @tc.desc: Verify base SDF shader shape CopyState dispatches to smooth sub op shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFSubOpShaderShapeTest, BaseCopyStateSmoothSubOp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest BaseCopyStateSmoothSubOp_001 start";
    GESDFSmoothSubOpShapeParams param;
    param.left = CreateTestShape();
    param.spacing = TEST_SPACING;
    GESDFSmoothSubOpShaderShape shape(param);

    GESDFSmoothSubOpShapeParams emptyParam;
    GESDFSmoothSubOpShaderShape emptyShape(emptyParam);
    EXPECT_FALSE(emptyShape.HasType(GESDFShapeType::RRECT));

    GESDFShaderShape& baseEmptyShape = emptyShape;
    const GESDFShaderShape& baseShape = shape;
    baseEmptyShape.CopyState(baseShape);
    EXPECT_TRUE(emptyShape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFSubOpShaderShapeTest BaseCopyStateSmoothSubOp_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
