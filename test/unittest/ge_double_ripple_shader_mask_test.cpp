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

#include "ge_double_ripple_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEDoubleRippleShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEDoubleRippleShaderMaskTest::SetUpTestCase(void) {}
void GEDoubleRippleShaderMaskTest::TearDownTestCase(void) {}

void GEDoubleRippleShaderMaskTest::SetUp() {}
void GEDoubleRippleShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.3f, 0.3f};
    param.center2_ = {0.7f, 0.7f};
    param.radius_ = 0.5f;
    param.width_ = 0.1f;
    param.turbulence_ = 0.2f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with minimum valid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.5f, 0.5f};
    param.center2_ = {0.5f, 0.5f};
    param.radius_ = 0.1f;
    param.width_ = 0.05f;
    param.turbulence_ = 0.1f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(1.0f, 1.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with zero turbulence
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_003 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.2f, 0.2f};
    param.center2_ = {0.8f, 0.8f};
    param.radius_ = 0.3f;
    param.width_ = 0.2f;
    param.turbulence_ = 0.0f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(200.0f, 200.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.4f, 0.4f};
    param.center2_ = {0.6f, 0.6f};
    param.radius_ = 0.4f;
    param.width_ = 0.15f;
    param.turbulence_ = 0.25f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(150.0f, 150.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with overlapping centers
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.5f, 0.5f};
    param.center2_ = {0.5f, 0.5f};
    param.radius_ = 0.6f;
    param.width_ = 0.1f;
    param.turbulence_ = 0.3f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with maximum turbulence
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_003 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.1f, 0.1f};
    param.center2_ = {0.9f, 0.9f};
    param.radius_ = 0.8f;
    param.width_ = 0.3f;
    param.turbulence_ = 1.0f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(300.0f, 300.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GenerateDrawingShaderHasNormal_003 end";
}

/**
 * @tc.name: GetDoubleRippleShaderMaskBuilder_001
 * @tc.desc: Verify GetDoubleRippleShaderMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GetDoubleRippleShaderMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderMaskBuilder_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.3f, 0.3f};
    param.center2_ = {0.7f, 0.7f};
    param.radius_ = 0.5f;
    param.width_ = 0.1f;
    param.turbulence_ = 0.2f;

    GEDoubleRippleShaderMask mask(param);
    auto builder = mask.GetDoubleRippleShaderMaskBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderMaskBuilder_001 end";
}

/**
 * @tc.name: GetDoubleRippleShaderMaskBuilder_002
 * @tc.desc: Verify GetDoubleRippleShaderMaskBuilder returns same instance on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GetDoubleRippleShaderMaskBuilder_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderMaskBuilder_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.4f, 0.4f};
    param.center2_ = {0.6f, 0.6f};
    param.radius_ = 0.3f;
    param.width_ = 0.2f;
    param.turbulence_ = 0.15f;

    GEDoubleRippleShaderMask mask(param);
    auto builder1 = mask.GetDoubleRippleShaderMaskBuilder();
    auto builder2 = mask.GetDoubleRippleShaderMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2); // Should return the same thread_local instance
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderMaskBuilder_002 end";
}

/**
 * @tc.name: GetDoubleRippleShaderNormalMaskBuilder_001
 * @tc.desc: Verify GetDoubleRippleShaderNormalMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GetDoubleRippleShaderNormalMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderNormalMaskBuilder_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.25f, 0.25f};
    param.center2_ = {0.75f, 0.75f};
    param.radius_ = 0.4f;
    param.width_ = 0.12f;
    param.turbulence_ = 0.18f;

    GEDoubleRippleShaderMask mask(param);
    auto builder = mask.GetDoubleRippleShaderNormalMaskBuilder();
    EXPECT_NE(builder, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderNormalMaskBuilder_001 end";
}

/**
 * @tc.name: GetDoubleRippleShaderNormalMaskBuilder_002
 * @tc.desc: Verify GetDoubleRippleShaderNormalMaskBuilder returns same instance on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, GetDoubleRippleShaderNormalMaskBuilder_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderNormalMaskBuilder_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.35f, 0.35f};
    param.center2_ = {0.65f, 0.65f};
    param.radius_ = 0.45f;
    param.width_ = 0.08f;
    param.turbulence_ = 0.22f;

    GEDoubleRippleShaderMask mask(param);
    auto builder1 = mask.GetDoubleRippleShaderNormalMaskBuilder();
    auto builder2 = mask.GetDoubleRippleShaderNormalMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2); // Should return the same thread_local instance
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest GetDoubleRippleShaderNormalMaskBuilder_002 end";
}

/**
 * @tc.name: ParameterValidation_001
 * @tc.desc: Verify shader generation with edge case parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, ParameterValidation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.0f, 0.0f};
    param.center2_ = {1.0f, 1.0f};
    param.radius_ = 0.0f;
    param.width_ = 0.0f;
    param.turbulence_ = 0.0f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr); // Should still generate shader even with zero values
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_001 end";
}

/**
 * @tc.name: ParameterValidation_002
 * @tc.desc: Verify shader generation with maximum parameter values
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, ParameterValidation_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {1.0f, 1.0f};
    param.center2_ = {0.0f, 0.0f};
    param.radius_ = 1.0f;
    param.width_ = 1.0f;
    param.turbulence_ = 1.0f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(500.0f, 500.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_002 end";
}

/**
 * @tc.name: ParameterValidation_003
 * @tc.desc: Verify shader generation with negative parameter values
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, ParameterValidation_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_003 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {-0.5f, -0.5f};
    param.center2_ = {1.5f, 1.5f};
    param.radius_ = -0.1f;
    param.width_ = -0.05f;
    param.turbulence_ = -0.2f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr); // Should handle negative values gracefully
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ParameterValidation_003 end";
}

/**
 * @tc.name: DimensionValidation_001
 * @tc.desc: Verify shader generation with very small dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, DimensionValidation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.5f, 0.5f};
    param.center2_ = {0.5f, 0.5f};
    param.radius_ = 0.1f;
    param.width_ = 0.05f;
    param.turbulence_ = 0.1f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(0.1f, 0.1f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_001 end";
}

/**
 * @tc.name: DimensionValidation_002
 * @tc.desc: Verify shader generation with very large dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, DimensionValidation_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_002 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.3f, 0.3f};
    param.center2_ = {0.7f, 0.7f};
    param.radius_ = 0.5f;
    param.width_ = 0.2f;
    param.turbulence_ = 0.3f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(10000.0f, 10000.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_002 end";
}

/**
 * @tc.name: DimensionValidation_003
 * @tc.desc: Verify shader generation with asymmetric dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, DimensionValidation_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_003 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.2f, 0.8f};
    param.center2_ = {0.8f, 0.2f};
    param.radius_ = 0.4f;
    param.width_ = 0.15f;
    param.turbulence_ = 0.25f;

    GEDoubleRippleShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(1920.0f, 1080.0f); // 16:9 aspect ratio
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest DimensionValidation_003 end";
}

/**
 * @tc.name: ShaderConsistency_001
 * @tc.desc: Verify both normal and regular shaders are generated consistently
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, ShaderConsistency_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ShaderConsistency_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.4f, 0.3f};
    param.center2_ = {0.6f, 0.7f};
    param.radius_ = 0.35f;
    param.width_ = 0.12f;
    param.turbulence_ = 0.18f;

    GEDoubleRippleShaderMask mask(param);
    auto regularShader = mask.GenerateDrawingShader(200.0f, 200.0f);
    auto normalShader = mask.GenerateDrawingShaderHasNormal(200.0f, 200.0f);

    EXPECT_NE(regularShader, nullptr);
    EXPECT_NE(normalShader, nullptr);
    EXPECT_NE(regularShader, normalShader); // Should be different shader instances
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest ShaderConsistency_001 end";
}

/**
 * @tc.name: BuilderCaching_001
 * @tc.desc: Verify builder caching behavior for thread_local instances
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, BuilderCaching_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest BuilderCaching_001 start";
    GEDoubleRippleShaderMaskParams param1;
    param1.center1_ = {0.3f, 0.3f};
    param1.center2_ = {0.7f, 0.7f};
    param1.radius_ = 0.5f;
    param1.width_ = 0.1f;
    param1.turbulence_ = 0.2f;

    GEDoubleRippleShaderMaskParams param2;
    param2.center1_ = {0.2f, 0.2f};
    param2.center2_ = {0.8f, 0.8f};
    param2.radius_ = 0.6f;
    param2.width_ = 0.15f;
    param2.turbulence_ = 0.25f;

    GEDoubleRippleShaderMask mask1(param1);
    GEDoubleRippleShaderMask mask2(param2);

    auto builder1 = mask1.GetDoubleRippleShaderMaskBuilder();
    auto builder2 = mask2.GetDoubleRippleShaderMaskBuilder();

    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2); // Should be same thread_local instance
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest BuilderCaching_001 end";
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor properly initializes parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, Constructor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest Constructor_001 start";
    GEDoubleRippleShaderMaskParams param;
    param.center1_ = {0.1f, 0.2f};
    param.center2_ = {0.8f, 0.9f};
    param.radius_ = 0.7f;
    param.width_ = 0.25f;
    param.turbulence_ = 0.35f;

    GEDoubleRippleShaderMask mask(param);

    // Test that the mask can generate shaders, indicating proper initialization
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "GEDoubleRippleShaderMaskTest Constructor_001 end";
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify type function
 * @tc.type:FUNC
 */
HWTEST_F(GEDoubleRippleShaderMaskTest, Type_001, TestSize.Level1)
{
    GEDoubleRippleShaderMaskParams params {};
    auto filter = std::make_shared<GEDoubleRippleShaderMask>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::DOUBLE_RIPPLE_MASK);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_MASK_DOUBLE_RIPPLE);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS