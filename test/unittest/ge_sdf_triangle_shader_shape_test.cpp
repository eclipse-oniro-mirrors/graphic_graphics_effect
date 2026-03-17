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
#include "ge_sdf_triangle_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFTriangleShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESDFTriangleShaderShapeTest::SetUpTestCase(void) {}
void GESDFTriangleShaderShapeTest::TearDownTestCase(void) {}
void GESDFTriangleShaderShapeTest::SetUp() {}
void GESDFTriangleShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShader_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 10.0f;

    GESDFTriangleShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(300.0f, 300.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with zero width
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShader_002 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(100.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 0.0f;

    GESDFTriangleShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(0.0f, 300.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 10.0f;

    GESDFTriangleShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(300.0f, 300.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(200.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(200.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(300.0f, 300.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns TRIANGLE type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFShapeType_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::TRIANGLE);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetVertex0_001
 * @tc.desc: Verify GetVertex0 returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetVertex0_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex0_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(200.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    const auto& vertex0 = shape.GetVertex0();
    EXPECT_EQ(vertex0.x_, 200.0f);
    EXPECT_EQ(vertex0.y_, 100.0f);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex0_001 end";
}

/**
 * @tc.name: GetVertex1_001
 * @tc.desc: Verify GetVertex1 returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetVertex1_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex1_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    const auto& vertex1 = shape.GetVertex1();
    EXPECT_EQ(vertex1.x_, 200.0f);
    EXPECT_EQ(vertex1.y_, 100.0f);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex1_001 end";
}

/**
 * @tc.name: GetVertex2_001
 * @tc.desc: Verify GetVertex2 returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetVertex2_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex2_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    const auto& vertex2 = shape.GetVertex2();
    EXPECT_EQ(vertex2.x_, 150.0f);
    EXPECT_EQ(vertex2.y_, 200.0f);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetVertex2_001 end";
}

/**
 * @tc.name: GetRadius_001
 * @tc.desc: Verify GetRadius returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetRadius_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetRadius_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 50.0f;

    GESDFTriangleShaderShape shape(param);
    EXPECT_EQ(shape.GetRadius(), 50.0f);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetRadius_001 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest CopyState_001 start";
    GESDFTriangleShapeParams param1;
    param1.vertex0 = Vector2f(100.0f, 200.0f);
    param1.vertex1 = Vector2f(200.0f, 100.0f);
    param1.vertex2 = Vector2f(150.0f, 200.0f);
    param1.radius = 50.0f;

    GESDFTriangleShapeParams param2;
    param2.vertex0 = Vector2f(50.0f, 100.0f);
    param2.vertex1 = Vector2f(100.0f, 50.0f);
    param2.vertex2 = Vector2f(75.0f, 100.0f);
    param2.radius = 25.0f;

    GESDFTriangleShaderShape shape1(param1);
    GESDFTriangleShaderShape shape2(param2);

    shape1.CopyState(shape2);

    const auto& v0 = shape1.GetVertex0();
    EXPECT_EQ(v0.x_, 50.0f);
    EXPECT_EQ(v0.y_, 100.0f);

    const auto& v1 = shape1.GetVertex1();
    EXPECT_EQ(v1.x_, 100.0f);
    EXPECT_EQ(v1.y_, 50.0f);

    const auto& v2 = shape1.GetVertex2();
    EXPECT_EQ(v2.x_, 75.0f);
    EXPECT_EQ(v2.y_, 100.0f);

    EXPECT_EQ(shape1.GetRadius(), 25.0f);

    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: GetSDFTriangleShaderShapeBuilder_001
 * @tc.desc: Verify GetSDFTriangleShaderShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetSDFTriangleShaderShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFTriangleShaderShapeBuilder_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    auto builder = shape.GetSDFTriangleShaderShapeBuilder();
    EXPECT_NE(builder, nullptr);
    EXPECT_EQ(shape.GetSDFTriangleShaderShapeBuilder(), builder);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFTriangleShaderShapeBuilder_001 end";
}

/**
 * @tc.name: GetSDFTriangleNormalShapeBuilder_001
 * @tc.desc: Verify GetSDFTriangleNormalShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GetSDFTriangleNormalShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFTriangleNormalShapeBuilder_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 1.0f;

    GESDFTriangleShaderShape shape(param);
    auto builder = shape.GetSDFTriangleNormalShapeBuilder();
    EXPECT_NE(builder, nullptr);
    EXPECT_EQ(shape.GetSDFTriangleNormalShapeBuilder(), builder);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GetSDFTriangleNormalShapeBuilder_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_001
 * @tc.desc: Verify GenerateShaderEffect with valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateShaderEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateShaderEffect_001 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 10.0f;

    GESDFTriangleShaderShape shape(param);
    auto builder = shape.GetSDFTriangleShaderShapeBuilder();
    auto shader = shape.GenerateShaderEffect(builder);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateShaderEffect_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_002
 * @tc.desc: Verify GenerateShaderEffect returns null for null builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFTriangleShaderShapeTest, GenerateShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateShaderEffect_002 start";
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(100.0f, 100.0f);
    param.vertex1 = Vector2f(200.0f, 100.0f);
    param.vertex2 = Vector2f(150.0f, 200.0f);
    param.radius = 10.0f;

    GESDFTriangleShaderShape shape(param);
    auto shader = shape.GenerateShaderEffect(nullptr);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFTriangleShaderShapeTest GenerateShaderEffect_002 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
