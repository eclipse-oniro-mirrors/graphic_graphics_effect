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

#include "ge_radial_gradient_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GERadialGradientShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GERadialGradientShaderMaskTest::SetUpTestCase(void) {}
void GERadialGradientShaderMaskTest::TearDownTestCase(void) {}

void GERadialGradientShaderMaskTest::SetUp() {}
void GERadialGradientShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_001 start";
    GERadialGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.radiusX_ = 1.0f;
    param.radiusY_ = 1.0f;
    param.colors_ = {0.0f, 1.0f};
    param.positions_ = {0.0f, 1.0f};

    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(0.5f, 0.5f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns null when radius is too small
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_002 start";
    GERadialGradientShaderMaskParams param;
    param.radiusX_ = 0.0f;
    param.radiusY_ = 0.0f;
    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader handles invalid colors and positions
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_003 start";
    GERadialGradientShaderMaskParams param;
    param.colors_.clear();
    param.positions_.clear();
    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShaderHasNormal_001 start";
    GERadialGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.radiusX_ = 1.0f;
    param.radiusY_ = 1.0f;
    param.colors_.push_back(0.0f);
    param.colors_.push_back(1.0f);
    param.positions_.push_back(0.0f);
    param.positions_.push_back(1.0f);

    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns null when parameters are invalid
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShaderHasNormal_002 start";
    GERadialGradientShaderMaskParams param;
    param.radiusX_ = 0.0f;
    param.radiusY_ = 0.0f;
    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateShaderEffect_001
 * @tc.desc: Verify GenerateShaderEffect sets uniforms correctly
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateShaderEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateShaderEffect_001 start";
    GERadialGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.radiusX_ = 1.0f;
    param.radiusY_ = 1.0f;
    param.colors_ = {0.0f, 1.0f};
    param.positions_ = {0.0f, 1.0f};

    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateShaderEffect_001 end";
}

/**
 * @tc.name: GenerateShaderEffect_002
 * @tc.desc: Verify GenerateShaderEffect returns null for invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GenerateShaderEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateShaderEffect_002 start";
    // colorSize <= 0
    GERadialGradientShaderMaskParams param;
    param.colors_.clear();
    param.positions_.clear();
    GERadialGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);

    // colorSize != positionSize
    GERadialGradientShaderMaskParams param2;
    param2.center_ = {0.5f, 0.5f};
    param2.radiusX_ = 1.0f;
    param2.radiusY_ = 1.0f;
    param2.colors_.assign(12, 1.0f);
    param2.positions_ = {0.0f, 1.0f};
    GERadialGradientShaderMask mask2(param2);
    auto shader2 = mask2.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader2, nullptr);

    // colorSize > SIZE_ARRAY
    param2.colors_.assign(15, 1.0f);
    GERadialGradientShaderMask mask3(param2);
    auto shader3 = mask3.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader3, nullptr);

    // width < 0.01f || height < 0.01f
    param2.colors_ = {0.0f, 1.0f};
    param2.positions_ = {0.0f, 1.0f};
    GERadialGradientShaderMask mask4(param2);
    auto shader4 = mask4.GenerateDrawingShader(0.0f, 100.0f);
    EXPECT_EQ(shader4, nullptr);
    GERadialGradientShaderMask mask5(param2);
    auto shader5 = mask5.GenerateDrawingShader(100.0f, 0.0f);
    EXPECT_EQ(shader5, nullptr);

    // param_.radiusX_ < 0.001f || param_.radiusY_ < 0.001f
    param2.radiusX_ = 0.0f;
    GERadialGradientShaderMask mask6(param2);
    auto shader6 = mask6.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader6, nullptr);
    param2.radiusX_ = 1.0f;
    param2.radiusY_ = 0.0f;
    GERadialGradientShaderMask mask7(param2);
    auto shader7 = mask7.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader7, nullptr);

    // position[i] < position[i - 1]
    param2.radiusY_ = 1.0f;
    param2.positions_ = {1.0f, 0.0f};
    GERadialGradientShaderMask mask8(param2);
    auto shader8 = mask8.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader8, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GenerateShaderEffect_002 end";
}

/**
 * @tc.name: GetRadialGradientShaderMaskBuilder_001
 * @tc.desc: Verify GetRadialGradientShaderMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GetRadialGradientShaderMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GetRadialGradientShaderMaskBuilder_001 start";
    GERadialGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.radiusX_ = 1.0f;
    param.radiusY_ = 1.0f;
    param.colors_.push_back(0.0f);
    param.colors_.push_back(1.0f);
    param.positions_.push_back(0.0f);
    param.positions_.push_back(1.0f);

    GERadialGradientShaderMask mask(param);
    auto builder = mask.GetRadialGradientShaderMaskBuilder();
    EXPECT_NE(builder, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GetRadialGradientShaderMaskBuilder_001 end";
}

/**
 * @tc.name: GetRadialGradientNormalMaskBuilder_001
 * @tc.desc: Verify GetRadialGradientNormalMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, GetRadialGradientNormalMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GetRadialGradientNormalMaskBuilder_001 start";
    GERadialGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.radiusX_ = 1.0f;
    param.radiusY_ = 1.0f;
    param.colors_.push_back(0.0f);
    param.colors_.push_back(1.0f);
    param.positions_.push_back(0.0f);
    param.positions_.push_back(1.0f);

    GERadialGradientShaderMask mask(param);
    auto builder = mask.GetRadialGradientNormalMaskBuilder();
    EXPECT_NE(builder, nullptr);

    GTEST_LOG_(INFO) << "GERadialGradientShaderMaskTest GetRadialGradientNormalMaskBuilder_001 end";
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GERadialGradientShaderMaskTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest Type_001 start";
    GERadialGradientShaderMaskParams param {};
    auto filter = std::make_unique<GERadialGradientShaderMask>(param);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_MASK_RADIAL_GRADIENT);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest Type_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
