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

#include "ge_gamma_correction_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEGammaCorrectionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEGammaCorrectionFilterTest::SetUpTestCase(void) {}

void GEGammaCorrectionFilterTest::TearDownTestCase(void) {}

void GEGammaCorrectionFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEGammaCorrectionFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage widht nullptr image
 * @tc.type:FUNC
 */
HWTEST_F(GEGammaCorrectionFilterTest, OnProcessImage_001, TestSize.Level0)
{
    float gamma = 1.5f; // 1.5 is valid gamma value
    auto gammaCorrectionFilter = std::make_shared<GEGammaCorrectionFilter>(gamma);
    ASSERT_TRUE(gammaCorrectionFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(gammaCorrectionFilter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage width image not is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEGammaCorrectionFilterTest, OnProcessImage_002, TestSize.Level0)
{
    float gamma = 1.5f; // 1.5 is valid gamma value
    auto gammaCorrectionFilter = std::make_shared<GEGammaCorrectionFilter>(gamma);
    ASSERT_TRUE(gammaCorrectionFilter != nullptr);
    EXPECT_EQ(gammaCorrectionFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage width invalid params
 * @tc.type:FUNC
 */
HWTEST_F(GEGammaCorrectionFilterTest, OnProcessImage_003, TestSize.Level0)
{
    float gamma = -1.5f; // -1.5 is invalid gamma value
    auto gammaCorrectionFilter = std::make_shared<GEGammaCorrectionFilter>(gamma);
    ASSERT_TRUE(gammaCorrectionFilter != nullptr);
    EXPECT_EQ(gammaCorrectionFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
