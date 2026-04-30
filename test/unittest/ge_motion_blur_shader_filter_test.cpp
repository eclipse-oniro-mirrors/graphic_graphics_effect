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

#include "ge_motion_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEMotionBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 1.0f, 1.0f, 51.0f, 51.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 51.0f, 51.0f };

    const int imageWidth_ = 50;
    const int imageHeight_ = 50;
};

void GEMotionBlurShaderFilterTest::SetUpTestCase() {}
void GEMotionBlurShaderFilterTest::TearDownTestCase() {}

void GEMotionBlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEMotionBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: CreateMotionBlurFilter_001
 * @tc.desc: Verify GEMotionBlurShaderFilter can be created with default params
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, CreateMotionBlurFilter_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
}

/**
 * @tc.name: CreateMotionBlurFilter_002
 * @tc.desc: Verify GEMotionBlurShaderFilter can be created with custom params
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, CreateMotionBlurFilter_002, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 50.0f;
    params.anchor = Vector2f(0.5f, 0.5f);
    params.sampleCount = 16;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage with valid image
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, OnProcessImage_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 10.0f;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with nullptr image
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, OnProcessImage_002, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 10.0f;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: MotionBlurParamsRadius_001
 * @tc.desc: Verify motion blur params radius can be set
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, MotionBlurParamsRadius_001, TestSize.Level2)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 100.0f;
    params.anchor = Vector2f(0.3f, 0.3f);
    params.sampleCount = 32;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    EXPECT_EQ(filter->radius_, 100.0f);
    EXPECT_EQ(filter->anchor_, Vector2f(0.3f, 0.3f));
    EXPECT_EQ(filter->sampleCount_, 32);
}

/**
 * @tc.name: RadiusBoundary_001
 * @tc.desc: Verify radius with negative value is corrected to 0.0f
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, RadiusBoundary_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = -10.0f;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->radius_, 0.0f);
}

/**
 * @tc.name: RadiusBoundary_002
 * @tc.desc: Verify radius with zero value
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, RadiusBoundary_002, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 0.0f;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->radius_, 0.0f);
}

/**
 * @tc.name: SampleCountBoundary_001
 * @tc.desc: Verify sampleCount below MIN_SAMPLE_COUNT is clamped
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, SampleCountBoundary_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 10.0f;
    params.sampleCount = 0;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->sampleCount_, 1);
}

/**
 * @tc.name: SampleCountBoundary_002
 * @tc.desc: Verify sampleCount above MAX_SAMPLE_COUNT is clamped
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, SampleCountBoundary_002, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 10.0f;
    params.sampleCount = 100;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->sampleCount_, 50);
}

/**
 * @tc.name: AnchorBoundary_001
 * @tc.desc: Verify anchor values are clamped to [0.0, 1.0]
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, AnchorBoundary_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 10.0f;
    params.anchor = Vector2f(-0.5f, 1.5f);
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->anchor_[0], 0.0f);
    EXPECT_EQ(filter->anchor_[1], 1.0f);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify GEMotionBlurShaderFilter Type returns correct GEFilterType
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, Type_001, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::MOTION_BLUR);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with radius zero returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GEMotionBlurShaderFilterTest, OnProcessImage_003, TestSize.Level1)
{
    Drawing::GEMotionBlurShaderFilterParams params;
    params.radius = 0.0f;
    auto filter = std::make_shared<GEMotionBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

} // namespace Rosen
} // namespace OHOS