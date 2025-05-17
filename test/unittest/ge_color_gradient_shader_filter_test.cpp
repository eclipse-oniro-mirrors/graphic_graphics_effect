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

#include "ge_color_gradient_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr static uint8_t COLOR_CHANNEL = 4; // 4 len of rgba
constexpr static uint8_t POSITION_CHANNEL = 2; // 2 len of rgba
constexpr static uint8_t ARRAY_SIZE = 12;  // 12 len of array

class GEColorGradientShaderFilterTest : public testing::Test {
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

void GEColorGradientShaderFilterTest::SetUpTestCase(void) {}
void GEColorGradientShaderFilterTest::TearDownTestCase(void) {}

void GEColorGradientShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEColorGradientShaderFilterTest::TearDown() {}

/**
 * @tc.name: ProcessImage_001
 * @tc.desc: Verify the ProcessImage: image is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEColorGradientShaderFilterTest, ProcessImage_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest ProcessImage_001 start";
    Drawing::GEColorGradientShaderFilterParams params;
    auto filter = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_EQ(filter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);
 
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1, 0, format); // 0, 1  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image1 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image1, src_, dst_), nullptr);

    bmp.Build(0, 1, format); // 1, 0  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLACK);
    auto image2 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image2, src_, dst_), nullptr);

    bmp.Build(0, 0, format); // 1, 0  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto image3 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image3, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest ProcessImage_001 end";
}

/**
 * @tc.name: ProcessImage_002
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GEColorGradientShaderFilterTest, ProcessImage_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest ProcessImage_002 start";

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<float> poitions = { 1.0f, 1.0f }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5f }; // 0.5 is strength params
    Drawing::GEColorGradientShaderFilterParams params;
    auto filter = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_EQ(filter->ProcessImage(canvas_, image_, src_, dst_), image_);

    Drawing::GEColorGradientShaderFilterParams params1 { colors, poitions, strengths, nullptr };
    auto filter1 = std::make_unique<GEColorGradientShaderFilter>(params1);
    EXPECT_EQ(filter1->ProcessImage(canvas_, image_, src_, dst_), image_);
 
    auto str = filter1->GetDescription();
    EXPECT_TRUE(str.length() > 0);

    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest ProcessImage_002 end";
}

/**
 * @tc.name: CheckInParams_001
 * @tc.desc: Verify the CheckInParams
 * @tc.type: FUNC
 */
HWTEST_F(GEColorGradientShaderFilterTest, CheckInParams_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest CheckInParams_001 start";
    float color[ARRAY_SIZE * COLOR_CHANNEL] = {0.0f}; // 0.0 default
    float pos[ARRAY_SIZE * POSITION_CHANNEL] = {0.0f}; // 0.0 default
    float strength[ARRAY_SIZE] = {1.0f}; // 0.0 default

    Drawing::GEColorGradientShaderFilterParams params0;
    auto filter0 = std::make_unique<GEColorGradientShaderFilter>(params0);
    EXPECT_FALSE(filter0->CheckInParams(color, pos, strength, ARRAY_SIZE));

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<float> positions = { 1.0f, 1.0f }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5f }; // 0.5 is strength params
    Drawing::GEColorGradientShaderFilterParams params { colors, positions, strengths, nullptr };
    auto filter = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_TRUE(filter->CheckInParams(color, pos, strength, ARRAY_SIZE));

    // 0.5 is strength params，strengths size > 12
    params.strengths = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    auto filter2 = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_FALSE(filter2->CheckInParams(color, pos, strength, ARRAY_SIZE));

    // 1.5 1.0 is strength params and strengths size != COLOR_CHANNEL * color size
    params.strengths = { 1.5f, 1.0f }; 
    auto filter3 = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_FALSE(filter3->CheckInParams(color, pos, strength, ARRAY_SIZE));

    // 1.5 is strength params and strengths size != POSITION_CHANNEL * positions size
    params.strengths = { 1.5f };
    params.positions = { 1.0f, 1.0f , 0.5f, 0.5f }; // 1.0, 1.0 is xy; 0.5, 0.5 is xy
    auto filter4 = std::make_unique<GEColorGradientShaderFilter>(params);
    EXPECT_FALSE(filter4->CheckInParams(color, pos, strength, ARRAY_SIZE));

    GTEST_LOG_(INFO) << "GEColorGradientShaderFilterTest CheckInParams_001 end";
}
} // namespace Rosen
} // namespace OHOS
