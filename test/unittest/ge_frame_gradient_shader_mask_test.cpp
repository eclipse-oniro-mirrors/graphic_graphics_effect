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

#include "ge_frame_gradient_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEFrameGradientShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEFrameGradientShaderMaskTest::SetUpTestCase(void) {}

void GEFrameGradientShaderMaskTest::TearDownTestCase(void) {}

void GEFrameGradientShaderMaskTest::SetUp() {}

void GEFrameGradientShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = 0.0f;
    param.frameWidth = 0.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(0.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = -1.0f;
    param.frameWidth = 0.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_003 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = 0.0f;
    param.frameWidth = -1.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_003 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_001 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = 0.0f;
    param.frameWidth = 0.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(0.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_002 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = -1.0f;
    param.frameWidth = 0.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_002 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_003 start";

    GEFrameGradientMaskParams param;
    param.gradientBezierControlPoints = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = 0.0f;
    param.frameWidth = -1.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_003 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
