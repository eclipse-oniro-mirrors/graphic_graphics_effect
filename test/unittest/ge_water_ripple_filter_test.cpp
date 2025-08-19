/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 
#include "ge_water_ripple_filter.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace GraphicsEffectEngine {
 
using namespace Rosen;
 
class GEWaterRippleFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);
 
    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
 
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};
 
void GEWaterRippleFilterTest::SetUpTestCase(void) {}

void GEWaterRippleFilterTest::TearDownTestCase(void) {}
 
void GEWaterRippleFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}
 
void GEWaterRippleFilterTest::TearDown() { image_ = nullptr; }
 
/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.7f, 1.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}
 
/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.7f, 1.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_EQ(geWaterRippleFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.0f, 2.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, OnProcessImage_004, TestSize.Level0)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.0f, 0.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, OnProcessImage_005, TestSize.Level0)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2, 0.5f, 0.0f, 3 };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_EQ(geWaterRippleFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, Type_001, TestSize.Level2)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2, 0.5f, 0.0f, 3 };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_EQ(geWaterRippleFilter->TypeName(), Drawing::GE_FILTER_WATER_RIPPLE);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS