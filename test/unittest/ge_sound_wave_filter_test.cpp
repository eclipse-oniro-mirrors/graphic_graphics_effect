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

#include "ge_sound_wave_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESoundWaveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 51.0f, 51.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 51.0f, 51.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 51.0f, 51.0f };
};

void GESoundWaveFilterTest::SetUpTestCase(void) {}
void GESoundWaveFilterTest::TearDownTestCase(void) {}

void GESoundWaveFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GESoundWaveFilterTest::TearDown() {}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify the ProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, ProcessImage001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage001 start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage001 end";
}


/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, ProcessImage002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage002 start";

    // init data
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1, 1, format); // 1, 1  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->ProcessImage(canvas_, image, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage002 end";
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, ProcessImage003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage003 start";

    // init data
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(10, 10, format); // 10, 10  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->ProcessImage(canvas_, image, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage003 end";
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, ProcessImage004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage004 start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest ProcessImage004 end";
}

/**
 * @tc.name: CheckSoundWaveParams
 * @tc.desc: Verify the CheckSoundWaveParams
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, CheckSoundWaveParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveParams start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    geSoundWaveFilterParams.colorA = {20.0f, -10.0f, 20.0f, -10.0f};
    geSoundWaveFilterParams.soundIntensity = 10.0f;
    geSoundWaveFilterParams.shockWaveAlphaA = 10.0f;
    geSoundWaveFilterParams.shockWaveAlphaB = 10.0f;
    geSoundWaveFilterParams.shockWaveTotalAlpha = 20.0f;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    geSoundWaveFilter->CheckSoundWaveParams();
    EXPECT_NE(geSoundWaveFilter->colorA_.greenF_, 0.0f);
    EXPECT_NE(geSoundWaveFilter->soundIntensity_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveProgressA_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveProgressB_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveTotalAlpha_, 20.0f);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveParams end";
}

} // namespace Rosen
} // namespace OHOS
