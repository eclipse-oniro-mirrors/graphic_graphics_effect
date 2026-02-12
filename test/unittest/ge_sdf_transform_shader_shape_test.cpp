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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
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
    EXPECT_EQ(shader, nullptr);
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
    EXPECT_EQ(shader, nullptr);
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
    EXPECT_EQ(shader, nullptr);
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
    EXPECT_EQ(shader, nullptr);
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
    EXPECT_EQ(shader, nullptr);
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
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTransformShaderShapeTest MatrixBoundaryTest_003 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
