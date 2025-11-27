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

#include "sdf/ge_sdf_pixelmap_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFPixelmapShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> CreateTestImage() const;
};

void GESDFPixelmapShaderShapeTest::SetUpTestCase(void) {}
void GESDFPixelmapShaderShapeTest::TearDownTestCase(void) {}

void GESDFPixelmapShaderShapeTest::SetUp() {}
void GESDFPixelmapShaderShapeTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShader_001 start";
    GESDFPixelmapShapeParams param;
    param.image = std::make_shared<Drawing::Image>(); 

    GESDFPixelmapShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null when image is null
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShader_002 start";
    GESDFPixelmapShapeParams param;
    param.image = nullptr;

    GESDFPixelmapShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(200.0f, 150.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns same result as GenerateDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFPixelmapShapeParams param;
    param.image = std::make_shared<Drawing::Image>();

    GESDFPixelmapShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);

    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal handles different width/height parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShaderHasNormal_002 start";
    GESDFPixelmapShapeParams param;
    param.image = nullptr;
    GESDFPixelmapShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(300.0f, 400.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GetSDFShapeType_001
 * @tc.desc: Verify GetSDFShapeType returns correct type
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GetSDFShapeType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetSDFShapeType_001 start";
    GESDFPixelmapShapeParams param;
    param.image = std::make_shared<Drawing::Image>();
    GESDFPixelmapShaderShape shape(param);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::PIXELMAP);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetSDFShapeType_001 end";
}

/**
 * @tc.name: GetImage_001
 * @tc.desc: Verify GetImage returns correct image
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GetImage_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetImage_001 start";
    GESDFPixelmapShapeParams param;
    param.image = std::make_shared<Drawing::Image>();

    GESDFPixelmapShaderShape shape(param);
    const auto& image = shape.GetImage();
    EXPECT_EQ(image, param.image);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetImage_001 end";
}

/**
 * @tc.name: GetImage_002
 * @tc.desc: Verify GetImage returns null when no image is set
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, GetImage_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetImage_002 start";
    GESDFPixelmapShapeParams param;
    param.image = nullptr;

    GESDFPixelmapShaderShape shape(param);
    const auto& image = shape.GetImage();
    EXPECT_EQ(image, nullptr);
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest GetImage_002 end";
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPixelmapShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest CopyState_001 start";
    // Create first shape with test image
    GESDFPixelmapShapeParams param1;
    param1.image = std::make_shared<Drawing::Image>();

    // Create second shape with different parameters
    GESDFPixelmapShapeParams param2;
    param2.image = nullptr;

    GESDFPixelmapShaderShape shape1(param1);
    GESDFPixelmapShaderShape shape2(param2);

    // Verify initial state is different
    EXPECT_NE(shape1.GetImage(), shape2.GetImage());

    // Copy state and verify
    shape1.CopyState(shape2);
    EXPECT_EQ(shape1.GetImage(), shape2.GetImage());
    EXPECT_EQ(shape1.GetImage(), nullptr);

    GTEST_LOG_(INFO) << "GESDFPixelmapShaderShapeTest CopyState_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS