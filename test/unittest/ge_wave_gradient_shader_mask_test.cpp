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

#include "ge_wave_gradient_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEWaveGradientShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEWaveGradientShaderMaskTest::SetUpTestCase(void) {}
void GEWaveGradientShaderMaskTest::TearDownTestCase(void) {}

void GEWaveGradientShaderMaskTest::SetUp() {}
void GEWaveGradientShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with zero wave width
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.0f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with small dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(1.0f, 1.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns non-null shader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with zero wave width
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.0f;
    param.turbulenceStrength_ = 0.1f;
    param.blurRadius_ = 0.05f;
    param.propagationRadius_ = 0.5f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(150.0f, 150.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with high turbulence
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.3f, 0.7f};
    param.width_ = 0.2f;
    param.turbulenceStrength_ = 0.5f;
    param.blurRadius_ = 0.1f;
    param.propagationRadius_ = 0.8f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShaderHasNormal(300.0f, 200.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GetWaveGradientShaderMaskBuilder_001
 * @tc.desc: Verify GetWaveGradientShaderMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GetWaveGradientShaderMaskBuilder_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto builder = mask.GetWaveGradientShaderMaskBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: GetWaveGradientShaderMaskBuilder_002
 * @tc.desc: Verify GetWaveGradientShaderMaskBuilder returns same instance on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GetWaveGradientShaderMaskBuilder_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto builder1 = mask.GetWaveGradientShaderMaskBuilder();
    auto builder2 = mask.GetWaveGradientShaderMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2); // Should return the same thread_local instance
}

/**
 * @tc.name: GetWaveGradientShaderNormalMaskBuilder_001
 * @tc.desc: Verify GetWaveGradientShaderNormalMaskBuilder returns a valid builder
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GetWaveGradientShaderNormalMaskBuilder_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto builder = mask.GetWaveGradientShaderNormalMaskBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: GetWaveGradientShaderNormalMaskBuilder_002
 * @tc.desc: Verify GetWaveGradientShaderNormalMaskBuilder returns same instance on multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, GetWaveGradientShaderNormalMaskBuilder_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    auto builder1 = mask.GetWaveGradientShaderNormalMaskBuilder();
    auto builder2 = mask.GetWaveGradientShaderNormalMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2); // Should return the same thread_local instance
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, Constructor_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    // Constructor should not throw and object should be created successfully
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: Constructor_002
 * @tc.desc: Verify constructor with default parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, Constructor_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    // Use default values
    param.center_ = {0.0f, 0.0f};
    param.width_ = 0.0f;
    param.turbulenceStrength_ = 0.0f;
    param.blurRadius_ = 0.0f;
    param.propagationRadius_ = 0.0f;

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: ParameterBoundary_001
 * @tc.desc: Verify shader generation with boundary parameter values
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, ParameterBoundary_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.0f, 0.0f}; // Corner position
    param.width_ = 0.0001f; // Very small width
    param.turbulenceStrength_ = 0.0001f; // Very small turbulence
    param.blurRadius_ = 0.0001f; // Very small blur
    param.propagationRadius_ = 0.0001f; // Very small radius

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: ParameterBoundary_002
 * @tc.desc: Verify shader generation with maximum parameter values
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, ParameterBoundary_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {1.0f, 1.0f}; // Maximum position
    param.width_ = 2.0f; // Large width
    param.turbulenceStrength_ = 1.0f; // Maximum turbulence
    param.blurRadius_ = 1.0f; // Large blur
    param.propagationRadius_ = 2.0f; // Large radius

    GEWaveGradientShaderMask mask(param);
    auto shader = mask.GenerateDrawingShader(500.0f, 500.0f);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: AspectRatio_001
 * @tc.desc: Verify shader generation with different aspect ratios
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, AspectRatio_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    
    // Test wide aspect ratio
    auto shader1 = mask.GenerateDrawingShader(400.0f, 100.0f);
    EXPECT_NE(shader1, nullptr);
    
    // Test tall aspect ratio
    auto shader2 = mask.GenerateDrawingShader(100.0f, 400.0f);
    EXPECT_NE(shader2, nullptr);
    
    // Test square aspect ratio
    auto shader3 = mask.GenerateDrawingShader(200.0f, 200.0f);
    EXPECT_NE(shader3, nullptr);
}

/**
 * @tc.name: AspectRatio_002
 * @tc.desc: Verify normal shader generation with different aspect ratios
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, AspectRatio_002, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    
    // Test wide aspect ratio with normal shader
    auto shader1 = mask.GenerateDrawingShaderHasNormal(400.0f, 100.0f);
    EXPECT_NE(shader1, nullptr);
    
    // Test tall aspect ratio with normal shader
    auto shader2 = mask.GenerateDrawingShaderHasNormal(100.0f, 400.0f);
    EXPECT_NE(shader2, nullptr);
}

/**
 * @tc.name: MultipleInstances_001
 * @tc.desc: Verify multiple instances work independently
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, MultipleInstances_001, TestSize.Level1)
{
    // Create first instance
    GEWaveGradientShaderMaskParams param1;
    param1.center_ = {0.3f, 0.3f};
    param1.width_ = 0.05f;
    param1.turbulenceStrength_ = 0.02f;
    param1.blurRadius_ = 0.01f;
    param1.propagationRadius_ = 0.2f;
    
    // Create second instance with different parameters
    GEWaveGradientShaderMaskParams param2;
    param2.center_ = {0.7f, 0.7f};
    param2.width_ = 0.15f;
    param2.turbulenceStrength_ = 0.08f;
    param2.blurRadius_ = 0.03f;
    param2.propagationRadius_ = 0.4f;

    GEWaveGradientShaderMask mask1(param1);
    GEWaveGradientShaderMask mask2(param2);
    
    auto shader1 = mask1.GenerateDrawingShader(100.0f, 100.0f);
    auto shader2 = mask2.GenerateDrawingShader(100.0f, 100.0f);
    
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);
    EXPECT_NE(shader1, shader2); // Should be different instances
}

/**
 * @tc.name: ShaderConsistency_001
 * @tc.desc: Verify shader generation consistency across multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, ShaderConsistency_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    
    // Generate multiple shaders with same parameters
    auto shader1 = mask.GenerateDrawingShader(100.0f, 100.0f);
    auto shader2 = mask.GenerateDrawingShader(100.0f, 100.0f);
    auto shader3 = mask.GenerateDrawingShader(100.0f, 100.0f);
    
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);
    EXPECT_NE(shader3, nullptr);
}

/**
 * @tc.name: NormalShaderConsistency_001
 * @tc.desc: Verify normal shader generation consistency across multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(GEWaveGradientShaderMaskTest, NormalShaderConsistency_001, TestSize.Level1)
{
    GEWaveGradientShaderMaskParams param;
    param.center_ = {0.5f, 0.5f};
    param.width_ = 0.1f;
    param.turbulenceStrength_ = 0.05f;
    param.blurRadius_ = 0.02f;
    param.propagationRadius_ = 0.3f;

    GEWaveGradientShaderMask mask(param);
    
    // Generate multiple normal shaders with same parameters
    auto shader1 = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    auto shader2 = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    auto shader3 = mask.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);
    EXPECT_NE(shader3, nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS