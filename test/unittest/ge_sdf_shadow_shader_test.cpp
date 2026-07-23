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

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

#include "ge_sdf_shadow_shader.h"
#include "ge_sdf_rrect_shader_shape.h"

#include "draw/color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESDFShadowShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    static inline Drawing::Canvas canvas_;
};

/**
 * @tc.name: MakeSDFShadowShaderTest
 * @tc.desc: test MakeSDFShadowShader by sdfShape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, MakeSDFShadowShaderTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    GESDFShadowShader shadowShader(shadowParams);

    Drawing::Rect rect0;
    auto shader = shadowShader.MakeSDFShadowShader(canvas_, rect0);
    EXPECT_EQ(shader, nullptr);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect1 { 1.0f, 1.0f, 2.0f, 2.0f };
    shader = shadowShader.MakeSDFShadowShader(canvas_, rect1);
    EXPECT_EQ(shader, nullptr);

    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    rectShapeParams.rrect.SetCornerRadius(10.0f, 10.0f);
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    shadowParams.shape = sdfShape;
    shadowShader.SetSDFShadowParams(shadowParams);
    shader = shadowShader.MakeSDFShadowShader(canvas_, rect1);
    EXPECT_NE(shader, nullptr);

    shadowParams.shadow.color = Drawing::Color::COLOR_GRAY;
    shadowShader.SetSDFShadowParams(shadowParams);
    shader = shadowShader.MakeSDFShadowShader(canvas_, rect1);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: UpdateRectForShadowTest
 * @tc.desc: test UpdateRectForShadow by shadow Params
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, UpdateRectForShadowTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shadow.radius = 10.0f; // 10.0f: set radius > 0
    GESDFShadowShader shadowShader(shadowParams);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_NE(rect, newRect);
    EXPECT_EQ(newRect.GetLeft(), -14.0f);
    EXPECT_EQ(newRect.GetRight(), 17.0f);
    EXPECT_EQ(newRect.GetTop(), -14.0f);
    EXPECT_EQ(newRect.GetBottom(), 17.0f);

    shadowParams.shadow.offsetX = 5.0f; // 5.0f: set offsetX > 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_EQ(newRect.GetLeft(), -9.0f);
    EXPECT_EQ(newRect.GetRight(), 22.0f);

    shadowParams.shadow.offsetY = -5.0f; // -5.0f: set offsetY < 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_EQ(newRect.GetTop(), -19.0f);
    EXPECT_EQ(newRect.GetBottom(), 12.0f);

    Drawing::GESDFShadowShaderParams invalidShadowParams;
    invalidShadowParams.shadow.radius = 10.0f;
    GESDFShadowShader invalidShadowShader(invalidShadowParams);
    // 10.0f, 10.0f, 1.0f, 1.0f is left top right bottom (left > right, invalid)
    Drawing::Rect invalidRect { 10.0f, 10.0f, 1.0f, 1.0f };
    EXPECT_FALSE(invalidRect.IsValid());
    Drawing::Rect invalidRectCopy = invalidRect;
    invalidShadowShader.UpdateRectForShadow(invalidRect);
    EXPECT_EQ(invalidRect.GetLeft(), invalidRectCopy.GetLeft());
    EXPECT_EQ(invalidRect.GetRight(), invalidRectCopy.GetRight());
    EXPECT_EQ(invalidRect.GetTop(), invalidRectCopy.GetTop());
    EXPECT_EQ(invalidRect.GetBottom(), invalidRectCopy.GetBottom());

    // large radius clamped to MAX_SHADOW_RADIUS
    constexpr float LARGE_RADIUS = 10000.0f; // 10000.0f * 1.5 = 15000 > MAX_SHADOW_RADIUS
    Drawing::GESDFShadowShaderParams largeShadowParams;
    largeShadowParams.shadow.radius = LARGE_RADIUS;
    largeShadowParams.shadow.offsetX = 0.0f;
    largeShadowParams.shadow.offsetY = 0.0f;
    GESDFShadowShader largeShadowShader(largeShadowParams);
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect largeRect { 1.0f, 1.0f, 2.0f, 2.0f };
    largeShadowShader.UpdateRectForShadow(largeRect);
    EXPECT_FLOAT_EQ(largeRect.GetLeft(), -9999.0f);
    EXPECT_FLOAT_EQ(largeRect.GetRight(), 10002.0f);
    EXPECT_FLOAT_EQ(largeRect.GetTop(), -9999.0f);
    EXPECT_FLOAT_EQ(largeRect.GetBottom(), 10002.0f);
}

/**
 * @tc.name: MakeDrawingShaderElevationTest
 * @tc.desc: test MakeDrawingShader by elevation params
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, MakeDrawingShaderElevationTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shadow.elevation = 20.0f; // 20.0f: set elevation > 0
    GESDFShadowShader shadowShader(shadowParams);

    Drawing::Rect rect0;
    shadowShader.MakeDrawingShader(canvas_, rect0, -1.f);
    EXPECT_EQ(shadowShader.GetDrawingShader(), nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect rect1 { 1.0f, 1.0f, 200.0f, 200.0f };
    shadowShader.MakeDrawingShader(canvas_, rect1, -1.f);
    EXPECT_EQ(shadowShader.GetDrawingShader(), nullptr);

    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    shadowParams.shape = sdfShape;
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.MakeDrawingShader(canvas_, rect1, -1.f);
    EXPECT_NE(shadowShader.GetDrawingShader(), nullptr);

    shadowParams.shadow.color = Drawing::Color::COLOR_GRAY;
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.MakeDrawingShader(canvas_, rect1, -1.f);
    EXPECT_NE(shadowShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: UpdateRectForElevationShadowTest
 * @tc.desc: test UpdateRectForElevationShadow by elevation params
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, UpdateRectForElevationShadowTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shadow.elevation = 300.0f; // 300.0f: set elevation > 0
    GESDFShadowShader shadowShader(shadowParams);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect newRect = rect;
    shadowShader.UpdateRectForElevationShadow(newRect);
    EXPECT_NE(rect, newRect);
    EXPECT_EQ(newRect.GetLeft(), -759.0f);
    EXPECT_EQ(newRect.GetRight(), 762.0f);
    EXPECT_EQ(newRect.GetTop(), -759.0f);
    EXPECT_EQ(newRect.GetBottom(), 762.0f);

    shadowParams.shadow.offsetX = 5.0f; // 5.0f: set offsetX > 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForElevationShadow(newRect);
    EXPECT_EQ(newRect.GetLeft(), -754.0f);
    EXPECT_EQ(newRect.GetRight(), 767.0f);

    shadowParams.shadow.offsetY = -5.0f; // -5.0f: set offsetY < 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForElevationShadow(newRect);
    EXPECT_EQ(newRect.GetTop(), -764.0f);
    EXPECT_EQ(newRect.GetBottom(), 757.0f);

    Drawing::GESDFShadowShaderParams invalidElevParams;
    invalidElevParams.shadow.elevation = 300.0f;
    GESDFShadowShader invalidElevShader(invalidElevParams);
    // 10.0f, 10.0f, 1.0f, 1.0f is left top right bottom (left > right, invalid)
    Drawing::Rect invalidElevRect { 10.0f, 10.0f, 1.0f, 1.0f };
    EXPECT_FALSE(invalidElevRect.IsValid());
    Drawing::Rect invalidElevCopy = invalidElevRect;
    invalidElevShader.UpdateRectForElevationShadow(invalidElevRect);
    EXPECT_EQ(invalidElevRect.GetLeft(), invalidElevCopy.GetLeft());
    EXPECT_EQ(invalidElevRect.GetRight(), invalidElevCopy.GetRight());
    EXPECT_EQ(invalidElevRect.GetTop(), invalidElevCopy.GetTop());
    EXPECT_EQ(invalidElevRect.GetBottom(), invalidElevCopy.GetBottom());

    // large blur clamped to MAX_SHADOW_BLUR (force a blur radius beyond the max directly)
    Drawing::GESDFShadowShaderParams largeBlurParams;
    largeBlurParams.shadow.offsetX = 0.0f;
    largeBlurParams.shadow.offsetY = 0.0f;
    GESDFShadowShader largeBlurShader(largeBlurParams);
    constexpr float OVER_MAX_BLUR = 20000.0f;
    largeBlurShader.ambientBlurRadius_ = OVER_MAX_BLUR;
    EXPECT_GT(largeBlurShader.ambientBlurRadius_, 10000.0f);
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect largeBlurRect { 1.0f, 1.0f, 2.0f, 2.0f };
    largeBlurShader.UpdateRectForElevationShadow(largeBlurRect);
    EXPECT_FLOAT_EQ(largeBlurRect.GetLeft(), -9999.0f);
    EXPECT_FLOAT_EQ(largeBlurRect.GetRight(), 10002.0f);
    EXPECT_FLOAT_EQ(largeBlurRect.GetTop(), -9999.0f);
    EXPECT_FLOAT_EQ(largeBlurRect.GetBottom(), 10002.0f);
}

/**
 * @tc.name: OnDrawShaderRingDrawRegion
 * @tc.desc: Verify OnDrawShader takes ring draw path when not filled and shape supports GetInscribedRect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, OnDrawShaderRingDrawRegion, TestSize.Level1)
{
    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    rectShapeParams.rrect.SetCornerRadius(10.0f, 10.0f);
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shape = sdfShape;
    shadowParams.shadow.radius = 10.0f;
    shadowParams.shadow.isFilled = false; // !isFilled && shape supports → ring draw path
    GESDFShadowShader shadowShader(shadowParams);
    Drawing::Rect rect {1.0f, 1.0f, 200.0f, 200.0f};
    shadowShader.OnDrawShader(canvas_, rect);
    EXPECT_NE(shadowShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: OnDrawShaderDrawRectFallback
 * @tc.desc: Verify OnDrawShader falls back to DrawRect when shadow is filled
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, OnDrawShaderDrawRectFallback, TestSize.Level1)
{
    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    rectShapeParams.rrect.SetCornerRadius(10.0f, 10.0f);
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shape = sdfShape;
    shadowParams.shadow.radius = 10.0f;
    shadowParams.shadow.isFilled = true; // filled → condition fails → else path (DrawRect fallback)
    GESDFShadowShader shadowShader(shadowParams);
    Drawing::Rect rect {1.0f, 1.0f, 200.0f, 200.0f};
    shadowShader.OnDrawShader(canvas_, rect);
    EXPECT_NE(shadowShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: ComputeElevationParams_InvalidAndLargeElevation
 * @tc.desc: Verify ComputeElevationParams clamps NaN/Inf/negative to zero and large to MAX_ELEVATION
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, ComputeElevationParams_InvalidAndLargeElevation, TestSize.Level1)
{
    constexpr float ZERO_BLUR = 0.0f;
    constexpr float MAX_ELEVATION = 10000.0f;
    constexpr float OVER_MAX_ELEVATION = 20000.0f;
    Drawing::GESDFShadowShaderParams shadowParams;
    GESDFShadowShader shadowShader(shadowParams);

    shadowShader.ComputeElevationParams();
    EXPECT_FLOAT_EQ(shadowShader.ambientBlurRadius_, ZERO_BLUR);

    shadowParams.shadow.elevation = std::numeric_limits<float>::quiet_NaN();
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.ComputeElevationParams();
    EXPECT_FALSE(std::isnan(shadowShader.ambientBlurRadius_));
    EXPECT_FLOAT_EQ(shadowShader.ambientBlurRadius_, ZERO_BLUR);

    shadowParams.shadow.elevation = std::numeric_limits<float>::infinity();
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.ComputeElevationParams();
    EXPECT_FLOAT_EQ(shadowShader.ambientBlurRadius_, ZERO_BLUR);

    shadowParams.shadow.elevation = -10.0f;
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.ComputeElevationParams();
    EXPECT_FLOAT_EQ(shadowShader.ambientBlurRadius_, ZERO_BLUR);

    shadowParams.shadow.elevation = MAX_ELEVATION;
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.ComputeElevationParams();
    float atMax = shadowShader.ambientBlurRadius_;
    EXPECT_FALSE(std::isnan(atMax));
    EXPECT_GT(atMax, 0.0f);

    shadowParams.shadow.elevation = OVER_MAX_ELEVATION;
    shadowShader.SetSDFShadowParams(shadowParams);
    shadowShader.ComputeElevationParams();
    EXPECT_FLOAT_EQ(shadowShader.ambientBlurRadius_, atMax);
}
} // namespace Rosen
} // namespace OHOS