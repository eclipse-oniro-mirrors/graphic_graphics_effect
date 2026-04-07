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

#include "ge_sdf_transform_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFTransformShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<GESDFShaderShape> CreateTestShape() const;
};

void GESDFTransformShaderShapeTest::SetUpTestCase(void) {}
void GESDFTransformShaderShapeTest::TearDownTestCase(void) {}

void GESDFTransformShaderShapeTest::SetUp() {}
void GESDFTransformShaderShapeTest::TearDown() {}

std::shared_ptr<GESDFShaderShape> GESDFTransformShaderShapeTest::CreateTestShape() const
{
    GESDFRRectShapeParams param;
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);
    return std::make_shared<GESDFRRectShaderShape>(param);
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null when shape is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_002 start";
    GESDFTransformShapeParams param;
    param.shape = nullptr;
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with non-identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();

    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null when shape is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFTransformShapeParams param;
    param.shape = nullptr;
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with non-identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();

    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns correct type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetSDFShapeType_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::TRANSFORM);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetMatrix_001
 * @tc.desc: Verify GetMatrix returns correct matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetMatrix_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetMatrix_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();

    // Create a specific matrix
    Drawing::Matrix expectedMatrix;
    expectedMatrix.Translate(30.0f, 40.0f);
    expectedMatrix.Scale(2.0f, 1.5f, 1.0f, 1.0f);
    param.matrix = expectedMatrix;

    GESDFTransformShaderShape shape(param);
    const auto& matrix = shape.GetMatrix();

    EXPECT_EQ(matrix, param.matrix);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetMatrix_001 end";
}

/**
 * @tc.name: GetMatrix_002
 * @tc.desc: Verify GetMatrix returns identity matrix when not set
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetMatrix_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetMatrix_002 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    // matrix is default constructed (identity)

    GESDFTransformShaderShape shape(param);
    const auto& matrix = shape.GetMatrix();

    // Should return identity matrix
    EXPECT_EQ(matrix, Drawing::Matrix());
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetMatrix_002 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest CopyState_001 start";

    // Create first shape
    GESDFTransformShapeParams param1;
    param1.shape = CreateTestShape();
    Drawing::Matrix matrix1;
    matrix1.Translate(10.0f, 20.0f);
    param1.matrix = matrix1;

    // Create second shape with different parameters
    GESDFTransformShapeParams param2;
    param2.shape = nullptr;
    Drawing::Matrix matrix2;
    matrix2.Scale(2.0f, 3.0f, 1.0f, 1.0f);
    param2.matrix = matrix2;

    GESDFTransformShaderShape shape1(param1);
    GESDFTransformShaderShape shape2(param2);

    // Copy state and verify
    shape1.CopyState(shape2);
    EXPECT_EQ(shape1.GetMatrix(), param2.matrix);

    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: GetSDFTransformShaderShapeBuilder_001
 * @tc.desc: Verify GetSDFTransformShaderShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetSDFTransformShaderShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetSDFTransformShaderShapeBuilder_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();
    param.matrix.Translate(30.0f, 40.0f);

    GESDFTransformShaderShape shape(param);
    auto builder = shape.GetSDFTransformShaderShapeBuilder();
    EXPECT_NE(builder, nullptr);
    
    // If created, it will reuse builder (thread_local)
    EXPECT_NE(shape.GetSDFTransformShaderShapeBuilder(), nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetSDFTransformShaderShapeBuilder_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_001
 * @tc.desc: Verify GenerateShaderEffect returns shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateShaderEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();
    
    GESDFTransformShaderShape shape(param);
    auto builder = shape.GetSDFTransformShaderShapeBuilder();
    auto shapeShader = param.shape->GenerateDrawingShader(100.0f, 100.0f);
    
    auto shader = shape.GenerateShaderEffect(100.0f, 100.0f, shapeShader, builder);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_002
 * @tc.desc: Verify GenerateShaderEffect returns null for invalid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_002 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();
    
    GESDFTransformShaderShape shape(param);
    auto shapeShader = param.shape->GenerateDrawingShader(100.0f, 100.0f);
    
    auto shader = shape.GenerateShaderEffect(100.0f, 100.0f, shapeShader, nullptr);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_002 end";
}

/**
 * @tc.name: GenerateShaderEffect_003
 * @tc.desc: Verify GenerateShaderEffect returns original shader for identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateShaderEffect_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix(); // Identity matrix
    
    GESDFTransformShaderShape shape(param);
    auto builder = shape.GetSDFTransformShaderShapeBuilder();
    auto shapeShader = param.shape->GenerateDrawingShader(100.0f, 100.0f);
    
    auto shader = shape.GenerateShaderEffect(100.0f, 100.0f, shapeShader, builder);
    EXPECT_EQ(shader, shapeShader);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateShaderEffect_003 end";
}

/**
 * @tc.name: MatrixBoundaryTest_001
 * @tc.desc: Verify maximum of matrix value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, MatrixBoundaryTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_001 start";
    float infinity = std::numeric_limits<float>::infinity();
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(infinity, -infinity);
    param.matrix = matrix;
    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_001 end";
}

/**
 * @tc.name: MatrixBoundaryTest_002
 * @tc.desc: Verify maximum of matrix value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, MatrixBoundaryTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_002 start";
    float infinity = std::numeric_limits<float>::infinity();
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Rotate(infinity, 0, 0); // degree, px, py
    param.matrix = matrix;
    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_002 end";
}

/**
 * @tc.name: MatrixBoundaryTest_003
 * @tc.desc: Verify maximum of matrix value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, MatrixBoundaryTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_003 start";
    float infinity = std::numeric_limits<float>::infinity();
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Scale(infinity, -infinity, 0, 0); // sx, sy, px, py
    param.matrix = matrix;
    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_003 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_001
 * @tc.desc: Verify GenerateGravityPullDrawingShader with valid parameters and unionMode=1
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1; // Enable gravity pull mode
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_001 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with unionMode=1 and null shape returns null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_002 start";
    GESDFTransformShapeParams param;
    param.shape = nullptr;
    param.unionMode = 1; // Enable gravity pull mode
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    // GenerateDrawingShader checks for null shape before calling GenerateGravityPullDrawingShader
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_002 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_003
 * @tc.desc: Verify GenerateGravityPullDrawingShader with zero spacing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 0.0f; // Zero spacing, should be clamped to 0.0001f
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_003 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_004
 * @tc.desc: Verify GenerateGravityPullDrawingShader with negative spacing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_004 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = -10.0f; // Negative spacing, should be clamped to 0.0001f
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_004 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_005
 * @tc.desc: Verify GenerateGravityPullDrawingShader with zero warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_005 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 0.0f; // Zero warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_005 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_006
 * @tc.desc: Verify GenerateGravityPullDrawingShader with positive warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_006 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 10.0f; // Positive warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_006 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_007
 * @tc.desc: Verify GenerateGravityPullDrawingShader with negative warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_007 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = -5.0f; // Negative warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_007 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_008
 * @tc.desc: Verify GenerateGravityPullDrawingShader with different center positions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_008 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(0.0f, 0.0f); // Center at origin
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    param.centerPosition = Vector2f(100.0f, 100.0f); // Center at corner
    GESDFTransformShaderShape shape2(param);
    auto shader2 = shape2.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader2, nullptr);

    param.centerPosition = Vector2f(-50.0f, -50.0f); // Center outside bounds
    GESDFTransformShaderShape shape3(param);
    auto shader3 = shape3.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader3, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_008 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_009
 * @tc.desc: Verify GenerateGravityPullDrawingShader with boundary dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_009 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);

    // Test with minimum dimensions
    auto shader1 = shape.GenerateGravityPullDrawingShader(1.0f, 1.0f);
    EXPECT_NE(shader1, nullptr);

    // Test with large dimensions
    auto shader2 = shape.GenerateGravityPullDrawingShader(10000.0f, 10000.0f);
    EXPECT_NE(shader2, nullptr);

    // Test with zero dimensions
    auto shader3 = shape.GenerateGravityPullDrawingShader(0.0f, 0.0f);
    EXPECT_NE(shader3, nullptr);

    // Test with negative dimensions
    auto shader4 = shape.GenerateGravityPullDrawingShader(-100.0f, -100.0f);
    EXPECT_NE(shader4, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_009 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_010
 * @tc.desc: Verify GenerateGravityPullDrawingShader with identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_010 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_010 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShader_011
 * @tc.desc: Verify GenerateGravityPullDrawingShader with non-identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShader_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_011 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);

    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    // Test with scale matrix
    matrix = Drawing::Matrix();
    matrix.Scale(2.0f, 1.5f, 1.0f, 1.0f);
    param.matrix = matrix;
    GESDFTransformShaderShape shape2(param);
    auto shader2 = shape2.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader2, nullptr);

    // Test with rotate matrix
    matrix = Drawing::Matrix();
    matrix.Rotate(45.0f, 50.0f, 50.0f);
    param.matrix = matrix;
    GESDFTransformShaderShape shape3(param);
    auto shader3 = shape3.GenerateGravityPullDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader3, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShader_011 end";
}

/**
 * @tc.name: GetGravityPullDrawingShaderBuilder_001
 * @tc.desc: Verify GetGravityPullDrawingShaderBuilder returns valid builder on first call
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravityPullDrawingShaderBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder = shape.GetGravityPullDrawingShaderBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_001 end";
}

/**
 * @tc.name: GetGravityPullDrawingShaderBuilder_002
 * @tc.desc: Verify GetGravityPullDrawingShaderBuilder returns cached builder on second call
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravityPullDrawingShaderBuilder_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_002 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder1 = shape.GetGravityPullDrawingShaderBuilder();
    EXPECT_NE(builder1, nullptr);

    // Second call should return the same cached builder (thread_local)
    auto builder2 = shape.GetGravityPullDrawingShaderBuilder();
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_002 end";
}

/**
 * @tc.name: GetGravityPullDrawingShaderBuilder_003
 * @tc.desc: Verify GetGravityPullDrawingShaderBuilder returns same builder on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravityPullDrawingShaderBuilder_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder1 = shape.GetGravityPullDrawingShaderBuilder();
    auto builder2 = shape.GetGravityPullDrawingShaderBuilder();
    auto builder3 = shape.GetGravityPullDrawingShaderBuilder();

    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_NE(builder3, nullptr);

    // All calls should return the same cached builder
    EXPECT_EQ(builder1, builder2);
    EXPECT_EQ(builder2, builder3);
    EXPECT_EQ(builder1, builder3);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_003 end";
}

/**
 * @tc.name: GetGravityPullDrawingShaderBuilder_004
 * @tc.desc: Verify GetGravityPullDrawingShaderBuilder returns non-null for different shape instances
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravityPullDrawingShaderBuilder_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_004 start";
    GESDFTransformShapeParams param1;
    param1.shape = CreateTestShape();
    param1.matrix = Drawing::Matrix();

    GESDFTransformShapeParams param2;
    param2.shape = CreateTestShape();
    param2.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape1(param1);
    GESDFTransformShaderShape shape2(param2);

    auto builder1 = shape1.GetGravityPullDrawingShaderBuilder();
    auto builder2 = shape2.GetGravityPullDrawingShaderBuilder();

    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);

    // Due to thread_local, both instances should return the same cached builder
    EXPECT_EQ(builder1, builder2);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravityPullDrawingShaderBuilder_004 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with valid parameters and unionMode=1
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1; // Enable gravity pull mode
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with unionMode=1 and null shape returns null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_002 start";
    GESDFTransformShapeParams param;
    param.shape = nullptr;
    param.unionMode = 1; // Enable gravity pull mode
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    // GenerateDrawingShaderHasNormal checks for null shape before calling GenerateGravityPullDrawingShaderHasNormal
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with zero spacing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 0.0f; // Zero spacing, should be clamped to 0.0001f
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_003 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_004
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with negative spacing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_004 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = -10.0f; // Negative spacing, should be clamped to 0.0001f
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_004 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_005
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with zero warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_005 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 0.0f; // Zero warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_005 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_006
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with positive warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_006 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 10.0f; // Positive warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_006 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_007
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with negative warp strength
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_007 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = -5.0f; // Negative warp strength
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_007 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_008
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with different center positions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_008 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(0.0f, 0.0f); // Center at origin
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    param.centerPosition = Vector2f(100.0f, 100.0f); // Center at corner
    GESDFTransformShaderShape shape2(param);
    auto shader2 = shape2.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader2, nullptr);

    param.centerPosition = Vector2f(-50.0f, -50.0f); // Center outside bounds
    GESDFTransformShaderShape shape3(param);
    auto shader3 = shape3.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader3, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_008 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_009
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with boundary dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_009 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);

    // Test with minimum dimensions
    auto shader1 = shape.GenerateGravityPullDrawingShaderHasNormal(1.0f, 1.0f);
    EXPECT_NE(shader1, nullptr);

    // Test with large dimensions
    auto shader2 = shape.GenerateGravityPullDrawingShaderHasNormal(10000.0f, 10000.0f);
    EXPECT_NE(shader2, nullptr);

    // Test with zero dimensions
    auto shader3 = shape.GenerateGravityPullDrawingShaderHasNormal(0.0f, 0.0f);
    EXPECT_NE(shader3, nullptr);

    // Test with negative dimensions
    auto shader4 = shape.GenerateGravityPullDrawingShaderHasNormal(-100.0f, -100.0f);
    EXPECT_NE(shader4, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_009 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_010
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_010 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_010 end";
}

/**
 * @tc.name: GenerateGravityPullDrawingShaderHasNormal_011
 * @tc.desc: Verify GenerateGravityPullDrawingShaderHasNormal with non-identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GenerateGravityPullDrawingShaderHasNormal_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_011 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.unionMode = 1;
    param.spacing = 10.0f;
    param.warpStrength = 5.0f;
    param.centerPosition = Vector2f(50.0f, 50.0f);

    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    auto shader = shape.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    // Test with scale matrix
    matrix = Drawing::Matrix();
    matrix.Scale(2.0f, 1.5f, 1.0f, 1.0f);
    param.matrix = matrix;
    GESDFTransformShaderShape shape2(param);
    auto shader2 = shape2.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader2, nullptr);

    // Test with rotate matrix
    matrix = Drawing::Matrix();
    matrix.Rotate(45.0f, 50.0f, 50.0f);
    param.matrix = matrix;
    GESDFTransformShaderShape shape3(param);
    auto shader3 = shape3.GenerateGravityPullDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader3, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GenerateGravityPullDrawingShaderHasNormal_011 end";
}

/**
 * @tc.name: GetGravPullDrawingShaderHasNormBuilder_001
 * @tc.desc: Verify GetGravPullDrawingShaderHasNormBuilder returns valid builder on first call
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravPullDrawingShaderHasNormBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder = shape.GetGravPullDrawingShaderHasNormBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_001 end";
}

/**
 * @tc.name: GetGravPullDrawingShaderHasNormBuilder_002
 * @tc.desc: Verify GetGravPullDrawingShaderHasNormBuilder returns cached builder on second call
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravPullDrawingShaderHasNormBuilder_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_002 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder1 = shape.GetGravPullDrawingShaderHasNormBuilder();
    EXPECT_NE(builder1, nullptr);

    // Second call should return same cached builder (thread_local)
    auto builder2 = shape.GetGravPullDrawingShaderHasNormBuilder();
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_002 end";
}

/**
 * @tc.name: GetGravPullPullDrawingShaderHasNormBuilder_003
 * @tc.desc: Verify GetGravPullDrawingShaderHasNormBuilder returns same builder on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravPullDrawingShaderHasNormBuilder_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    auto builder1 = shape.GetGravPullDrawingShaderHasNormBuilder();
    auto builder2 = shape.GetGravPullDrawingShaderHasNormBuilder();
    auto builder3 = shape.GetGravPullDrawingShaderHasNormBuilder();

    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_NE(builder3, nullptr);

    // All calls should return same cached builder
    EXPECT_EQ(builder1, builder2);
    EXPECT_EQ(builder2, builder3);
    EXPECT_EQ(builder1, builder3);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_003 end";
}

/**
 * @tc.name: GetGravPullDrawingShaderHasNormBuilder_004
 * @tc.desc: Verify GetGravPullDrawingShaderHasNormBuilder returns non-null for different shape instances
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, GetGravPullDrawingShaderHasNormBuilder_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_004 start";
    GESDFTransformShapeParams param1;
    param1.shape = CreateTestShape();
    param1.matrix = Drawing::Matrix();

    GESDFTransformShapeParams param2;
    param2.shape = CreateTestShape();
    param2.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape1(param1);
    GESDFTransformShaderShape shape2(param2);

    auto builder1 = shape1.GetGravPullDrawingShaderHasNormBuilder();
    auto builder2 = shape2.GetGravPullDrawingShaderHasNormBuilder();

    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);

    // Due to thread_local, both instances should return same cached builder
    EXPECT_EQ(builder1, builder2);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest GetGravPullDrawingShaderHasNormBuilder_004 end";
}

/**
 * @tc.name: TryGetCenter_001
 * @tc.desc: Verify TryGetCenter returns true with valid shape and identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_001 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix(); // Identity matrix

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // For identity matrix, output should equal the center of the child shape
    // GESDFRRectShaderShape center is at (50, 50) for rect (0, 0, 100, 100)
    EXPECT_FLOAT_EQ(outX, 50.0f);
    EXPECT_FLOAT_EQ(outY, 50.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_001 end";
}

/**
 * @tc.name: TryGetCenter_002
 * @tc.desc: Verify TryGetCenter returns false when shape is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_002 start";
    GESDFTransformShapeParams param;
    param.shape = nullptr;
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_FALSE(result);
    EXPECT_FLOAT_EQ(outX, 0.f);
    EXPECT_FLOAT_EQ(outY, 0.f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_002 end";
}

/**
 * @tc.name: TryGetCenter_003
 * @tc.desc: Verify TryGetCenter with translation matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_003 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    
    // Create a translation matrix
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Center (50, 50) translated by (10, 20) should be (60, 70)
    EXPECT_FLOAT_EQ(outX, 60.0f);
    EXPECT_FLOAT_EQ(outY, 70.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_003 end";
}

/**
 * @tc.name: TryGetCenter_004
 * @tc.desc: Verify TryGetCenter with scale matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_004 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    
    // Create a scale matrix
    Drawing::Matrix matrix;
    matrix.Scale(2.0f, 1.5f, 1.0f, 1.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Center (50, 50) scaled by (2.0, 1.5) should be (100, 75)
    EXPECT_FLOAT_EQ(outX, 100.0f);
    EXPECT_FLOAT_EQ(outY, 75.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_004 end";
}

/**
 * @tc.name: TryGetCenter_005
 * @tc.desc: Verify TryGetCenter with rotation matrix
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_005 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    
    // Create a rotation matrix (90 degrees around origin)
    Drawing::Matrix matrix;
    matrix.Rotate(90.0f, 0.0f, 0.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Center (50, 50) rotated 90 degrees around origin should be (-50, 50)
    EXPECT_FLOAT_EQ(outX, -50.0f);
    EXPECT_FLOAT_EQ(outY, 50.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_005 end";
}

/**
 * @tc.name: TryGetCenter_006
 * @tc.desc: Verify TryGetCenter with combined transformation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_006 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    
    // Create combined transformation: translate then scale
    Drawing::Matrix matrix;
    matrix.Translate(10.0f, 20.0f);
    matrix.Scale(2.0f, 1.5f, 1.0f, 1.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Center (50, 50) -> translate (10, 20) -> (60, 70) -> scale (2.0, 1.5) -> (120, 105)
    EXPECT_FLOAT_EQ(outX, 120.0f);
    EXPECT_FLOAT_EQ(outY, 105.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_006 end";
}

/**
 * @tc.name: TryGetCenter_007
 * @tc.desc: Verify TryGetCenter with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_007 start";
    
    // Test with very large translation
    {
        GESDFTransformShapeParams param;
        param.shape = CreateTestShape();
        Drawing::Matrix matrix;
        matrix.Translate(10000.0f, 10000.0f);
        param.matrix = matrix;
        
        GESDFTransformShaderShape shape(param);
        float outX = 0.f;
        float outY = 0.f;
        bool result = shape.TryGetCenter(outX, outY);
        
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(outX, 10050.0f);
        EXPECT_FLOAT_EQ(outY, 10050.0f);
    }
    
    // Test with negative translation
    {
        GESDFTransformShapeParams param;
        param.shape = CreateTestShape();
        Drawing::Matrix matrix;
        matrix.Translate(-100.0f, -200.0f);
        param.matrix = matrix;
        
        GESDFTransformShaderShape shape(param);
        float outX = 0.f;
        float outY = 0.f;
        bool result = shape.TryGetCenter(outX, outY);
        
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(outX, -50.0f);
        EXPECT_FLOAT_EQ(outY, -150.0f);
    }
    
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_007 end";
}

/**
 * @tc.name: TryGetCenter_008
 * @tc.desc: Verify TryGetCenter with zero scale
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_008 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    
    // Create a scale matrix with zero scale
    Drawing::Matrix matrix;
    matrix.Scale(0.0f, 0.0f, 1.0f, 1.0f);
    param.matrix = matrix;

    GESDFTransformShaderShape shape(param);
    float outX = 0.f;
    float outY = 0.f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Center (50, 50) scaled by (0.0, 0.0) should be (0, 0)
    EXPECT_FLOAT_EQ(outX, 0.0f);
    EXPECT_FLOAT_EQ(outY, 0.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_008 end";
}

/**
 * @tc.name: TryGetCenter_009
 * @tc.desc: Verify TryGetCenter updates output parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTransformShaderShapeTest, TryGetCenter_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_009 start";
    GESDFTransformShapeParams param;
    param.shape = CreateTestShape();
    param.matrix = Drawing::Matrix();

    GESDFTransformShaderShape shape(param);
    
    // Test with initial non-zero values
    float outX = 999.0f;
    float outY = 888.0f;
    bool result = shape.TryGetCenter(outX, outY);
    
    EXPECT_TRUE(result);
    // Values should be updated to the correct center
    EXPECT_FLOAT_EQ(outX, 50.0f);
    EXPECT_FLOAT_EQ(outY, 50.0f);
    
    // Test multiple calls
    float outX2 = 0.f;
    float outY2 = 0.f;
    bool result2 = shape.TryGetCenter(outX2, outY2);
    
    EXPECT_TRUE(result2);
    EXPECT_FLOAT_EQ(outX2, 50.0f);
    EXPECT_FLOAT_EQ(outY2, 50.0f);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest TryGetCenter_009 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS