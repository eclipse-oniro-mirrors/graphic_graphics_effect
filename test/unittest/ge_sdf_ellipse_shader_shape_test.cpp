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
#include "ge_sdf_ellipse_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFEllipseShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESDFEllipseShaderShapeTest::SetUpTestCase(void) {}
void GESDFEllipseShaderShapeTest::TearDownTestCase(void) {}
void GESDFEllipseShaderShapeTest::SetUp() {}
void GESDFEllipseShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShader_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(300.0f, 300.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null for invalid radius
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShader_002 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(0.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(300.0f, 300.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(300.0f, 300.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null for invalid radius
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, -1.0f);

    GESDFEllipseShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(300.0f, 300.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns ELLIPSE type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFShapeType_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::ELLIPSE);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::ELLIPSE));
    EXPECT_FALSE(shape.HasType(GESDFShapeType::TRIANGLE));
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetCenter_001
 * @tc.desc: Verify GetCenter returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GetCenter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetCenter_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    const auto& center = shape.GetCenter();
    EXPECT_EQ(center.x_, 100.0f);
    EXPECT_EQ(center.y_, 120.0f);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetCenter_001 end";
}

/**
 * @tc.name: GetRadius_001
 * @tc.desc: Verify GetRadius returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GetRadius_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetRadius_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    const auto& radius = shape.GetRadius();
    EXPECT_EQ(radius.x_, 60.0f);
    EXPECT_EQ(radius.y_, 40.0f);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetRadius_001 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest CopyState_001 start";
    GESDFEllipseShapeParams param1;
    param1.center = Vector2f(100.0f, 120.0f);
    param1.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShapeParams param2;
    param2.center = Vector2f(50.0f, 70.0f);
    param2.radius = Vector2f(20.0f, 30.0f);

    GESDFEllipseShaderShape shape1(param1);
    GESDFEllipseShaderShape shape2(param2);
    shape1.CopyState(shape2);

    EXPECT_EQ(shape1.GetCenter().x_, 50.0f);
    EXPECT_EQ(shape1.GetCenter().y_, 70.0f);
    EXPECT_EQ(shape1.GetRadius().x_, 20.0f);
    EXPECT_EQ(shape1.GetRadius().y_, 30.0f);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: GetSDFEllipseShaderShapeBuilder_001
 * @tc.desc: Verify GetSDFEllipseShaderShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GetSDFEllipseShaderShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFEllipseShaderShapeBuilder_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto builder = shape.GetSDFEllipseShaderShapeBuilder();
    EXPECT_NE(builder, nullptr);
    EXPECT_EQ(shape.GetSDFEllipseShaderShapeBuilder(), builder);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFEllipseShaderShapeBuilder_001 end";
}

/**
 * @tc.name: GetSDFEllipseNormalShapeBuilder_001
 * @tc.desc: Verify GetSDFEllipseNormalShapeBuilder returns valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GetSDFEllipseNormalShapeBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFEllipseNormalShapeBuilder_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto builder = shape.GetSDFEllipseNormalShapeBuilder();
    EXPECT_NE(builder, nullptr);
    EXPECT_EQ(shape.GetSDFEllipseNormalShapeBuilder(), builder);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GetSDFEllipseNormalShapeBuilder_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_001
 * @tc.desc: Verify GenerateShaderEffect with valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateShaderEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateShaderEffect_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto builder = shape.GetSDFEllipseShaderShapeBuilder();
    auto shader = shape.GenerateShaderEffect(builder);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateShaderEffect_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_002
 * @tc.desc: Verify GenerateShaderEffect returns null for null builder
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, GenerateShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateShaderEffect_002 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    auto shader = shape.GenerateShaderEffect(nullptr);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest GenerateShaderEffect_002 end";
}

/**
 * @tc.name: TryGetCenter_001
 * @tc.desc: Verify TryGetCenter returns ellipse center
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEllipseShaderShapeTest, TryGetCenter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest TryGetCenter_001 start";
    GESDFEllipseShapeParams param;
    param.center = Vector2f(100.0f, 120.0f);
    param.radius = Vector2f(60.0f, 40.0f);

    GESDFEllipseShaderShape shape(param);
    float x = 0.0f;
    float y = 0.0f;
    EXPECT_TRUE(shape.TryGetCenter(x, y));
    EXPECT_EQ(x, 100.0f);
    EXPECT_EQ(y, 120.0f);
    GTEST_LOG_(INFO) << "GESDFEllipseShaderShapeTest TryGetCenter_001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
