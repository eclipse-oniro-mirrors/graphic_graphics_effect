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
 * @tc.desc: Verify behavior with low blur intensity that still processes image
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, OnProcessImageWithLowBlurIntensity, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 0.3f;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
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
    EXPECT_EQ(filter->mixStrength_, 2.0f);
    EXPECT_EQ(filter->progress_, 0.0f);

    filter->CheckBlurBubblesRiseParams();

    EXPECT_EQ(filter->blurIntensity_, 0.0f);
    EXPECT_EQ(filter->mixStrength_, 1.0f);
    EXPECT_EQ(filter->progress_, 0.0f);
}

/**
 * @tc.name: ShaderEffectCachingMechanism
 * @tc.desc: Verify shader effect caching mechanism works correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, ShaderEffectCachingMechanism, TestSize.Level2)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    auto filter1 = std::make_unique<GEBlurBubblesRiseFilter>(params);
    auto filter2 = std::make_unique<GEBlurBubblesRiseFilter>(params);

    // Verify that different filter instances return the same shader effect (thread_local caching)
    auto blurShader1 = filter1->GetBlurShaderEffect();
    auto blurShader2 = filter2->GetBlurShaderEffect();
    EXPECT_NE(blurShader1, nullptr);
    EXPECT_NE(blurShader2, nullptr);
    EXPECT_EQ(blurShader1, blurShader2);

    auto maskMixShader1 = filter1->GetMaskMixShaderEffect();
    auto maskMixShader2 = filter2->GetMaskMixShaderEffect();
    EXPECT_NE(maskMixShader1, nullptr);
    EXPECT_NE(maskMixShader2, nullptr);
    EXPECT_EQ(maskMixShader1, maskMixShader2);
}

/**
 * @tc.name: ShaderEffectMultiCallConsistency
 * @tc.desc: Verify multiple calls to shader effect functions return consistent results
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, ShaderEffectMultiCallConsistency, TestSize.Level2)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);

    // Verify that multiple calls return the same shader effect
    auto blurShader1 = filter->GetBlurShaderEffect();
    auto blurShader2 = filter->GetBlurShaderEffect();
    auto blurShader3 = filter->GetBlurShaderEffect();
    EXPECT_NE(blurShader1, nullptr);
    EXPECT_EQ(blurShader1, blurShader2);
    EXPECT_EQ(blurShader2, blurShader3);

    auto maskMixShader1 = filter->GetMaskMixShaderEffect();
    auto maskMixShader2 = filter->GetMaskMixShaderEffect();
    EXPECT_NE(maskMixShader1, nullptr);
    EXPECT_EQ(maskMixShader1, maskMixShader2);
}

/**
 * @tc.name: ShaderEffectInActualProcessing
 * @tc.desc: Verify shader effects work correctly in actual image processing
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurBubblesRiseFilterTest, ShaderEffectInActualProcessing, TestSize.Level1)
{
    Drawing::GEBlurBubblesRiseFilterParams params;
    params.blurIntensity = 0.8f;
    params.mixStrength = 0.5f;
    params.progress = 0.7f;

    auto filter = std::make_unique<GEBlurBubblesRiseFilter>(params);

    // Verify that shader effect is created successfully
    auto blurShader = filter->GetBlurShaderEffect();
    auto maskMixShader = filter->GetMaskMixShaderEffect();
    EXPECT_NE(blurShader, nullptr);
    EXPECT_NE(maskMixShader, nullptr);

    // Verify that shader effect works correctly in actual image processing
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

} // namespace Rosen
} // namespace OHOS
