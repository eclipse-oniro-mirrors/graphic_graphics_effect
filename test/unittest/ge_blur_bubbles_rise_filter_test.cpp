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

#include "ge_blur_bubbles_rise_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBlurBubblesRiseFilterTest : public testing::Test {
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

void GEBlurBubblesRiseFilterTest::SetUpTestCase() {}
void GEBlurBubblesRiseFilterTest::TearDownTestCase() {}

void GEBlurBubblesRiseFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEBlurBubblesRiseFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImage001
 * @tc.desc: Verify the OnProcessImage when image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, OnProcessImage001, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: CheckBlurBubblesRiseParams001
 * @tc.desc: Verify parameter clamp ranges of blur bubbles rise
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, CheckBlurBubblesRiseParams001, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 2.0f;
    params.mixStrength = -1.0f;
    params.progress = 2.0f;

    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    EXPECT_EQ(filter->progress_, 2.0f);
    filter->CheckBlurBubblesRiseParams();

    EXPECT_EQ(filter->blurIntensity_, 1.0f);
    EXPECT_EQ(filter->mixStrength_, 0.0f);
    EXPECT_EQ(filter->progress_, 1.0f);
}

/**
 * @tc.name: GetRuntimeEffect001
 * @tc.desc: Verify runtime effect can be created
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, GetRuntimeEffect001, TestSize.Level2)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    EXPECT_NE(filter->GetBlurShaderEffect(), nullptr);
    EXPECT_NE(filter->GetMaskMixShaderEffect(), nullptr);
}

/**
 * @tc.name: Type001
 * @tc.desc: Verify Type and TypeName
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, Type001, TestSize.Level2)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::BLUR_BUBBLES_RISE);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: OnProcessImageWithLowBlurIntensity
 * @tc.desc: Verify behavior with low blur intensity that triggers early termination
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, OnProcessImageWithLowBlurIntensity, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 0.3f;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImageWithHighBlurIntensity
 * @tc.desc: Verify behavior with high blur intensity that completes full loop
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, OnProcessImageWithHighBlurIntensity, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 1.0f;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImageWithDifferentProgress
 * @tc.desc: Verify behavior with different progress values after timeScale change
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, OnProcessImageWithDifferentProgress, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.progress = 0.5f;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CheckBlurBubblesRiseParams002
 * @tc.desc: Verify parameter clamping with minimum and maximum values
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, CheckBlurBubblesRiseParams002, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 0.0f;
    params.mixStrength = 2.0f;
    params.progress = 0.0f;

    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    EXPECT_EQ(filter->blurIntensity_, 0.0f);
    EXPECT_EQ(filter->mixStrength_, 0.0f);
    EXPECT_EQ(filter->progress_, 0.0f);

    filter->CheckBlurBubblesRiseParams();

    EXPECT_EQ(filter->blurIntensity_, 0.0f);
    EXPECT_EQ(filter->mixStrength_, 1.0f);
    EXPECT_EQ(filter->progress_, 0.0f);
}

} // namespace Rosen
} // namespace OHOS
