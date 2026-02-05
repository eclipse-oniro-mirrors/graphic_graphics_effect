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
    GEFrameGradientMaskParams CreateValidParams();
};

void GEFrameGradientShaderMaskTest::SetUpTestCase(void) {}

void GEFrameGradientShaderMaskTest::TearDownTestCase(void) {}

void GEFrameGradientShaderMaskTest::SetUp() {}

void GEFrameGradientShaderMaskTest::TearDown() {}

GEFrameGradientMaskParams GEFrameGradientShaderMaskTest::CreateValidParams()
{
    GEFrameGradientMaskParams param;
    param.innerBezier = {0.0f, 0.0f, 0.0f, 0.0f};
    param.outerBezier = {0.0f, 0.0f, 0.0f, 0.0f};
    param.cornerRadius = 0.5f;
    param.innerFrameWidth = 0.1f;
    param.outerFrameWidth = 0.1f;
    param.rectWH = {1.0f, 1.0f};
    param.rectPos = {0.0f, 0.0f};
    return param;
}

/**
 * @tc.name: GenerateDrawingShaderWrongWidth
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderWrongWidth, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongWidth start";

    GEFrameGradientMaskParams param = CreateValidParams();
    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(0.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongWidth end";
}

/**
 * @tc.name: GenerateDrawingShaderWrongRectWH
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderWrongRectWH, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongRectWH start";

    GEFrameGradientMaskParams param = CreateValidParams();
    param.rectWH = {-1.0f, -1.0f};
    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongRectWH end";
}

/**
 * @tc.name: GenerateDrawingShaderWrongFrameWidth
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderWrongFrameWidth, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongFrameWidth start";

    GEFrameGradientMaskParams param = CreateValidParams();
    param.innerFrameWidth = -1.0f;
    param.outerFrameWidth = -1.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderWrongFrameWidth end";
}

/**
 * @tc.name: GenerateDrawingShaderSuccess
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderSuccess, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderSuccess start";

    GEFrameGradientMaskParams param = CreateValidParams();

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderSuccess end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormalWrongWidth
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormalWrongWidth, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalWrongWidth start";

    GEFrameGradientMaskParams param = CreateValidParams();

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(0.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalWrongWidth end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormalRectWH
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormalRectWH, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalRectWH start";

    GEFrameGradientMaskParams param = CreateValidParams();
    param.rectWH = {-1.0f, -1.0f};
    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalRectWH end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormalWrongFrameWidth
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormalWrongFrameWidth, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalWrongFrameWidth start";

    GEFrameGradientMaskParams param = CreateValidParams();
    param.innerFrameWidth = -1.0f;
    param.outerFrameWidth = -1.0f;

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalWrongFrameWidth end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormalSuccess
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameGradientShaderMaskTest, GenerateDrawingShaderHasNormalSuccess, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalSuccess start";

    GEFrameGradientMaskParams param = CreateValidParams();

    GEFrameGradientShaderMask mask(param);
    auto shaderEffect = mask.GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormalSuccess end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS