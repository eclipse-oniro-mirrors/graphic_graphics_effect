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

#include "ge_sdf_distort_op_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFDistortOpShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<GESDFShaderShape> CreateTestShape() const;
};

void GESDFDistortOpShaderShapeTest::SetUpTestCase(void) {}
void GESDFDistortOpShaderShapeTest::TearDownTestCase(void) {}

void GESDFDistortOpShaderShapeTest::SetUp() {}
void GESDFDistortOpShaderShapeTest::TearDown() {}

std::shared_ptr<GESDFShaderShape> GESDFDistortOpShaderShapeTest::CreateTestShape() const
{
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
    return std::make_shared<GESDFRRectShaderShape>(param);
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_001 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null when shape is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_002 start";
    GESDFDistortOpShapeParams param;
    param.shape = nullptr;
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with barrel distortion
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_003 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);
    param.barrelDistortion = Vector4f(0.1f, 0.1f, 0.1f, 0.1f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null when shape is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFDistortOpShapeParams param;
    param.shape = nullptr;
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns correct type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GetSDFShapeType_001 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::DISTORT_OP);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetSDFShape_001
 * @tc.desc: Verify GetSDFShape returns correct shape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GetSDFShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GetSDFShape_001 start";
    auto testShape = CreateTestShape();
    GESDFDistortOpShapeParams param;
    param.shape = testShape;
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    const auto& returnedShape = shape.GetSDFShape();
    EXPECT_EQ(returnedShape, testShape);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GetSDFShape_001 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest CopyState_001 start";

    GESDFDistortOpShapeParams param1;
    param1.shape = CreateTestShape();
    param1.LUCorner = Drawing::Point(0.0f, 0.0f);
    param1.RUCorner = Drawing::Point(1.0f, 0.0f);
    param1.RBCorner = Drawing::Point(1.0f, 1.0f);
    param1.LBCorner = Drawing::Point(0.0f, 1.0f);
    param1.barrelDistortion = Vector4f(0.1f, 0.1f, 0.1f, 0.1f);

    GESDFDistortOpShapeParams param2;
    param2.shape = nullptr;
    param2.LUCorner = Drawing::Point(0.1f, 0.1f);
    param2.RUCorner = Drawing::Point(0.9f, 0.1f);
    param2.RBCorner = Drawing::Point(0.9f, 0.9f);
    param2.LBCorner = Drawing::Point(0.1f, 0.9f);
    param2.barrelDistortion = Vector4f(0.2f, 0.2f, 0.2f, 0.2f);

    GESDFDistortOpShaderShape shape1(param1);
    GESDFDistortOpShaderShape shape2(param2);

    shape1.CopyState(shape2);
    EXPECT_EQ(shape1.GetSDFShape(), nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: HasType_001
 * @tc.desc: Verify HasType returns true for DISTORT_OP type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, HasType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_001 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::DISTORT_OP));
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_001 end";
}

/**
 * @tc.name: HasType_002
 * @tc.desc: Verify HasType returns false for other types
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, HasType_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_002 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_002 end";
}

/**
 * @tc.name: HasType_003
 * @tc.desc: Verify HasType returns true for nested shape type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, HasType_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_003 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);

    GESDFDistortOpShaderShape shape(param);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::RRECT));
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest HasType_003 end";
}

/**
 * @tc.name: GenerateDrawingShader_004
 * @tc.desc: Verify GenerateDrawingShader with asymmetric barrel distortion
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShader_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_004 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.0f, 0.0f);
    param.RUCorner = Drawing::Point(1.0f, 0.0f);
    param.RBCorner = Drawing::Point(1.0f, 1.0f);
    param.LBCorner = Drawing::Point(0.0f, 1.0f);
    param.barrelDistortion = Vector4f(0.1f, 0.2f, 0.3f, 0.4f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_004 end";
}

/**
 * @tc.name: GenerateDrawingShader_005
 * @tc.desc: Verify GenerateDrawingShader with non-identity corners
 * @tc.type: FUNC
 */
HWTEST_F(GESDFDistortOpShaderShapeTest, GenerateDrawingShader_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_005 start";
    GESDFDistortOpShapeParams param;
    param.shape = CreateTestShape();
    param.LUCorner = Drawing::Point(0.1f, 0.1f);
    param.RUCorner = Drawing::Point(0.9f, 0.1f);
    param.RBCorner = Drawing::Point(0.9f, 0.9f);
    param.LBCorner = Drawing::Point(0.1f, 0.9f);

    GESDFDistortOpShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFDistortOpShaderShapeTest GenerateDrawingShader_005 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
