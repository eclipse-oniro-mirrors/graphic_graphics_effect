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

#include "ge_heat_distortion_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEHeatDistortionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 1.0f, 1.0f, 51.0f, 51.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 51.0f, 51.0f };

    const int imageWidth_  = 50;
    const int imageHeight_ = 50;
};

void GEHeatDistortionFilterTest::SetUpTestCase() {}
void GEHeatDistortionFilterTest::TearDownTestCase() {}

void GEHeatDistortionFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEHeatDistortionFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImage001
 * @tc.desc: Verify the OnProcessImage when image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImage001, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage002
 * @tc.desc: Verify the OnProcessImage when image size is minimal
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImage002, TestSize.Level1)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1, 1, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GEHeatDistortionFilterParams params;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: CheckHeatDistortionParams001
 * @tc.desc: Verify parameter clamp ranges of heat distortion
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, CheckHeatDistortionParams001, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 10.0f;
    params.noiseScale = 0.0f;
    params.riseWeight = -1.0f;
    params.progress = 2.0f;

    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_EQ(filter->progress_, 2.0f);
    filter->CheckHeatDistortionParams();

    EXPECT_EQ(filter->intensity_, 1.0f);
    EXPECT_EQ(filter->noiseScale_, 0.1f);
    EXPECT_EQ(filter->riseWeight_, 0.0f);
    EXPECT_EQ(filter->progress_, 1.0f);
}

/**
 * @tc.name: GetHeatDistortionEffect001
 * @tc.desc: Verify runtime effect can be created
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, GetHeatDistortionEffect001, TestSize.Level2)
{
    Drawing::GEHeatDistortionFilterParams params;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_NE(filter->GetHeatDistortionEffect(), nullptr);
}

/**
 * @tc.name: Type001
 * @tc.desc: Verify Type and TypeName
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, Type001, TestSize.Level2)
{
    Drawing::GEHeatDistortionFilterParams params;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::HEAT_DISTORTION);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_HEAT_DISTORTION);
}

/**
 * @tc.name: OnProcessImageWithDifferentNoiseScale
 * @tc.desc: Verify behavior with different noise scale values
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithDifferentNoiseScale, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.noiseScale = 0.1f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithMaxNoiseScale
 * @tc.desc: Verify behavior with maximum noise scale
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithMaxNoiseScale, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.noiseScale = 2.0f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithDifferentRiseWeight
 * @tc.desc: Verify behavior with different rise weight values
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithDifferentRiseWeight, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.riseWeight = 0.0f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithMaxRiseWeight
 * @tc.desc: Verify behavior with maximum rise weight
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithMaxRiseWeight, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.riseWeight = 1.0f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithDifferentProgress
 * @tc.desc: Verify behavior with different progress values after optimization
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithDifferentProgress, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.progress = 0.5f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithFullProgress
 * @tc.desc: Verify behavior with full progress
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithFullProgress, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.progress = 1.0f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CheckHeatDistortionParams002
 * @tc.desc: Verify parameter clamping with edge values
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, CheckHeatDistortionParams002, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.0f;
    params.noiseScale = 2.0f;
    params.riseWeight = 1.0f;
    params.progress = 0.0f;

    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    EXPECT_EQ(filter->intensity_, 0.0f);
    EXPECT_EQ(filter->noiseScale_, 2.0f);
    EXPECT_EQ(filter->riseWeight_, 1.0f);
    EXPECT_EQ(filter->progress_, 0.0f);

    filter->CheckHeatDistortionParams();

    EXPECT_EQ(filter->intensity_, 0.0f);
    EXPECT_EQ(filter->noiseScale_, 2.0f);
    EXPECT_EQ(filter->riseWeight_, 1.0f);
    EXPECT_EQ(filter->progress_, 0.0f);
}

/**
 * @tc.name: HeatDistortionShaderEffectCachingMechanism
 * @tc.desc: Verify heat distortion shader effect caching mechanism works correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, HeatDistortionShaderEffectCachingMechanism, TestSize.Level2)
{
    Drawing::GEHeatDistortionFilterParams params;
    auto filter1 = std::make_unique<GEHeatDistortionFilter>(params);
    auto filter2 = std::make_unique<GEHeatDistortionFilter>(params);

    // Verify that different filter instances return the same shader effect (thread_local caching)
    auto shaderEffect1 = filter1->GetHeatDistortionEffect();
    auto shaderEffect2 = filter2->GetHeatDistortionEffect();
    EXPECT_NE(shaderEffect1, nullptr);
    EXPECT_NE(shaderEffect2, nullptr);
    EXPECT_EQ(shaderEffect1, shaderEffect2);
}

/**
 * @tc.name: HeatDistortionShaderEffectMultiCallConsistency
 * @tc.desc: Verify multiple calls to heat distortion shader effect function return consistent results
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, HeatDistortionShaderEffectMultiCallConsistency, TestSize.Level2)
{
    Drawing::GEHeatDistortionFilterParams params;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);

    // Verify that multiple calls return the same shader effect
    auto shaderEffect1 = filter->GetHeatDistortionEffect();
    auto shaderEffect2 = filter->GetHeatDistortionEffect();
    auto shaderEffect3 = filter->GetHeatDistortionEffect();
    EXPECT_NE(shaderEffect1, nullptr);
    EXPECT_EQ(shaderEffect1, shaderEffect2);
    EXPECT_EQ(shaderEffect2, shaderEffect3);
}

/**
 * @tc.name: HeatDistortionShaderEffectInActualProcessing
 * @tc.desc: Verify heat distortion shader effect works correctly in actual image processing
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, HeatDistortionShaderEffectInActualProcessing, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.8f;
    params.noiseScale = 1.5f;
    params.riseWeight = 0.6f;
    params.progress = 0.7f;

    auto filter = std::make_unique<GEHeatDistortionFilter>(params);

    // Verify that shader effect is created successfully
    auto shaderEffect = filter->GetHeatDistortionEffect();
    EXPECT_NE(shaderEffect, nullptr);

    // Verify that shader effect works correctly in actual image processing
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithMinimalResolution
 * @tc.desc: Verify behavior with minimal image resolution
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithMinimalResolution, TestSize.Level1)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(2, 2, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.5f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithRectangularResolution
 * @tc.desc: Verify behavior with rectangular (non-square) resolution
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithRectangularResolution, TestSize.Level1)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto image = bmp.MakeImage();

    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.6f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithOddResolution
 * @tc.desc: Verify behavior with odd resolution dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithOddResolution, TestSize.Level1)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(51, 51, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_GREEN);
    auto image = bmp.MakeImage();

    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.4f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithHighIntensity
 * @tc.desc: Verify behavior with high intensity value
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithHighIntensity, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 1.0f;
    params.noiseScale = 2.0f;
    params.riseWeight = 0.8f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithLowIntensity
 * @tc.desc: Verify behavior with low intensity value
 * @tc.type: FUNC
 */
HWTEST_F(GEHeatDistortionFilterTest, OnProcessImageWithLowIntensity, TestSize.Level1)
{
    Drawing::GEHeatDistortionFilterParams params;
    params.intensity = 0.1f;
    params.noiseScale = 0.5f;
    params.riseWeight = 0.2f;
    auto filter = std::make_unique<GEHeatDistortionFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

} // namespace Rosen
} // namespace OHOS
