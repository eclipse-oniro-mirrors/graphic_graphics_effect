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

} // namespace Rosen
} // namespace OHOS
