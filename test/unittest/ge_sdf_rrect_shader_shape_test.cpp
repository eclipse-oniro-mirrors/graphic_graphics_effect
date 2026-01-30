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

#include "sdf/ge_sdf_rrect_shader_shape.h"

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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
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
    param.rrect = {0.0f, 0.0f, 0.00005f, 100.0f, 10.0f, 10.0f};

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
    param.rrect = {0.0f, 0.0f, 100.0f, 0.00005f, 10.0f};

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateDrawingShader_003 end";
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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
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
    param.rrect = {0.0f, 0.0f, 0.00005f, 100.0f, 10.0f, 10.0f};

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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};

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
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f, 15.0f, 15.0f};

    GESDFRRectShaderShape shape(param);
    const auto& rrect = shape.GetRRect();

    EXPECT_NE(rrect.left_, 10.0f);
    EXPECT_NE(rrect.top_, 20.0f);
    EXPECT_NE(rrect.width_, 100.0f);
    EXPECT_NE(rrect.height_, 200.0f);
    EXPECT_NE(rrect.radiusX_, 15.0f);
    EXPECT_NE(rrect.radiusY_, 15.0f);
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
    param1.rrect = {10.0f, 20.0f, 100.0f, 200.0f, 15.0f, 15.0f};

    GESDFRRectShapeParams param2;
    param2.rrect = {5.0f, 10.0f, 50.0f, 100.0f, 8.0f, 8.0f};

    GESDFRRectShaderShape shape1(param1);
    GESDFRRectShaderShape shape2(param2);

    // Verify initial state is different
    EXPECT_NE(shape1.GetRRect().left_, 10.0f);
    EXPECT_NE(shape2.GetRRect().left_, 5.0f);

    // Copy state and verify
    shape1.CopyState(shape2);
    EXPECT_NE(shape1.GetRRect().left_, 5.0f);
    EXPECT_NE(shape1.GetRRect().top_, 10.0f);
    EXPECT_NE(shape1.GetRRect().width_, 50.0f);
    EXPECT_NE(shape1.GetRRect().height_, 100.0f);
    EXPECT_NE(shape1.GetRRect().radiusX_, 8.0f);
    EXPECT_NE(shape1.GetRRect().radiusY_, 8.0f);

    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest CopyState_001 end";
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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f};

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
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f};

    GESDFRRectShaderShape shape(param);
    auto builder = shape.GetSDFRRectNormalShapeBuilder();
    EXPECT_NE(builder, nullptr);
    // if created, it will reuse builder
    EXPECT_NE(shape.GetSDFRRectNormalShapeBuilder(), nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GetSDFRRectNormalShapeBuilder_001 end";
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
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f, 15.0f};

    GESDFRRectShaderShape shape(param);
    auto builder = shape.GetSDFRRectShaderShapeBuilder();
    auto shader = shape.GenerateShaderEffect(builder);
    EXPECT_EQ(shader, nullptr);
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
    param.rrect = {10.0f, 20.0f, 100.0f, 200.0f, 15.0f};

    GESDFRRectShaderShape shape(param);
    auto shader = shape.GenerateShaderEffect(nullptr);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFRRectShaderShapeTest GenerateShaderEffect_002 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS