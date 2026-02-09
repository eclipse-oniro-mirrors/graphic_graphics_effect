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
#include "ge_water_droplet_transition_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEWaterDropletTransitionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> CreateTestImage(int width, int height);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    std::shared_ptr<Drawing::Image> topLayerImage_ { nullptr };

    Drawing::Rect src_ { 0.0f, 0.0f, 100.0f, 100.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 100.0f, 100.0f };
};

void GEWaterDropletTransitionFilterTest::SetUpTestCase(void) {}

void GEWaterDropletTransitionFilterTest::TearDownTestCase(void) {}

std::shared_ptr<Drawing::Image> GEWaterDropletTransitionFilterTest::CreateTestImage(int width, int height)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

void GEWaterDropletTransitionFilterTest::SetUp()
{
    image_ = CreateTestImage(100, 100); // 100x100 blue bottom image
    topLayerImage_ = CreateTestImage(100, 100); // 100x100 blue top image
}

void GEWaterDropletTransitionFilterTest::TearDown()
{
    image_ = nullptr;
    topLayerImage_ = nullptr;
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage handles null image, different inverse values
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = topLayerImage_;
    params.inverse = false;
    params.progress = 0.5f;
    params.radius = 0.3f;
    params.transitionFadeWidth = 0.1f;
    params.distortionIntensity = 0.5f;
    params.distortionThickness = 0.1f;
    params.lightStrength = 0.5f;
    params.lightSoftness = 0.3f;
    params.noiseScaleX = 2.0f;
    params.noiseScaleY = 2.0f;
    params.noiseStrengthX = 0.1f;
    params.noiseStrengthY = 0.1f;

    // Test null image returns nullptr
    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    // Test null image returns nullptr
    params.topLayer = nullptr;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    // Test with inverse = true
    params.inverse = true;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage handles boundary parameter values (1.0, max, negative)
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = topLayerImage_;
    params.inverse = false;

    // Test with full boundary values (1.0/10.0)
    params.progress = 1.0f;
    params.radius = 1.0f;
    params.transitionFadeWidth = 1.0f;
    params.distortionIntensity = 1.0f;
    params.distortionThickness = 1.0f;
    params.lightStrength = 1.0f;
    params.lightSoftness = 1.0f;
    params.noiseScaleX = 10.0f;
    params.noiseScaleY = 10.0f;
    params.noiseStrengthX = 10.0f;
    params.noiseStrengthY = 10.0f;

    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with negative parameter values
    params.progress = -0.5f;
    params.radius = -0.3f;
    params.transitionFadeWidth = -0.1f;
    params.distortionIntensity = -0.5f;
    params.distortionThickness = -0.1f;
    params.lightStrength = -0.5f;
    params.lightSoftness = -0.3f;
    params.noiseScaleX = -2.0f;
    params.noiseScaleY = -2.0f;
    params.noiseStrengthX = -0.1f;
    params.noiseStrengthY = -0.1f;

    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with zero values for all parameters
    params.progress = 0.0f;
    params.radius = 0.0f;
    params.transitionFadeWidth = 0.0f;
    params.distortionIntensity = 0.0f;
    params.distortionThickness = 0.0f;
    params.lightStrength = 0.0f;
    params.lightSoftness = 0.0f;
    params.noiseScaleX = 0.0f;
    params.noiseScaleY = 0.0f;
    params.noiseStrengthX = 0.0f;
    params.noiseStrengthY = 0.0f;

    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage handles different image sizes
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = topLayerImage_;
    params.inverse = false;
    params.progress = 0.5f;
    params.radius = 0.3f;
    params.transitionFadeWidth = 0.1f;
    params.distortionIntensity = 0.5f;
    params.distortionThickness = 0.1f;
    params.lightStrength = 0.5f;
    params.lightSoftness = 0.3f;
    params.noiseScaleX = 2.0f;
    params.noiseScaleY = 2.0f;
    params.noiseStrengthX = 0.1f;
    params.noiseStrengthY = 0.1f;

    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);

    // Test with minimum valid image size (1x1)
    auto minImage = CreateTestImage(1, 1);
    auto minTopLayer = CreateTestImage(1, 1);
    params.topLayer = minTopLayer;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, minImage, src_, dst_), nullptr);

    // Test with invalid image size (0)
    auto invalidImage = CreateTestImage(0, 0);
    auto topLayer = CreateTestImage(100, 100); // 100x100 blue top image
    params.topLayer = topLayer;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, invalidImage, src_, dst_), nullptr);

    // Test with different image size (100x200)
    auto tallImage = CreateTestImage(100, 200); // 100x200 blue bottom image
    auto tallTopLayer = CreateTestImage(100, 200); // 100x200 blue top image
    params.topLayer = tallTopLayer;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, tallImage, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify OnProcessImage handles different top layer and bottom layer sizes
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, OnProcessImage_004, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = CreateTestImage(200, 200);
    params.inverse = false;
    params.progress = 0.5f;
    params.radius = 0.3f;
    params.transitionFadeWidth = 0.1f;
    params.distortionIntensity = 0.5f;
    params.distortionThickness = 0.1f;
    params.lightStrength = 0.5f;
    params.lightSoftness = 0.3f;
    params.noiseScaleX = 2.0f;
    params.noiseScaleY = 2.0f;
    params.noiseStrengthX = 0.1f;
    params.noiseStrengthY = 0.1f;

    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    auto smallImage = CreateTestImage(50, 50);
    EXPECT_NE(filter->OnProcessImage(canvas_, smallImage, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify OnProcessImage with various progress values (0.0, 0.25, 0.5, 0.75, 1.0)
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, OnProcessImage_005, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = topLayerImage_;
    params.inverse = false;
    params.radius = 0.3f;
    params.transitionFadeWidth = 0.1f;
    params.distortionIntensity = 0.5f;
    params.distortionThickness = 0.1f;
    params.lightStrength = 0.5f;
    params.lightSoftness = 0.3f;
    params.noiseScaleX = 2.0f;
    params.noiseScaleY = 2.0f;
    params.noiseStrengthX = 0.1f;
    params.noiseStrengthY = 0.1f;

    // Test with progress = 0.0
    params.progress = 0.0f;
    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with progress = 0.25
    params.progress = 0.25f;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with progress = 0.5
    params.progress = 0.5f;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with progress = 0.75
    params.progress = 0.75f;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    // Test with progress = 1.0
    params.progress = 1.0f;
    filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor creates filter successfully with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEWaterDropletTransitionFilterTest, Constructor_001, TestSize.Level0)
{
    Drawing::GEWaterDropletTransitionFilterParams params;
    params.topLayer = topLayerImage_;
    params.inverse = false;
    params.progress = 0.5f;
    params.radius = 0.3f;

    auto filter = std::make_shared<GEWaterDropletTransitionFilter>(params);
    EXPECT_NE(filter, nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
