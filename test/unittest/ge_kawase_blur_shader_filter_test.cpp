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

#include "ge_kawase_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

struct OHOS::Rosen::OffsetInfo {
    float offsetX;
    float offsetY;
    int width;
    int height;
};

namespace {
constexpr uint32_t BLUR_SAMPLE_COUNT = 5;
}

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEKawaseBlurShaderFilterTest : public testing::Test {
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

void GEKawaseBlurShaderFilterTest::SetUpTestCase(void) {}
void GEKawaseBlurShaderFilterTest::TearDownTestCase(void) {}

void GEKawaseBlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEKawaseBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadius_001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, GetRadius_001, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    EXPECT_EQ(geKawaseBlurShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: ProcessImage_001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_001, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: ProcessImage_002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_002, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage_003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_003, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{0}; // 0 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage_004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_004, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{8001}; // 8001 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage_005
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_005, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{100}; // 100 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage_006
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_006, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{60}; // 60 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage_007
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage_007, TestSize.Level0)
{
    Drawing::GEKawaseBlurShaderFilterParams params{30}; // 30 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ScaleAndAddRandomColor_001
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor_001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

/**
 * @tc.name: ScaleAndAddRandomColor_002
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor_002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    filter->blurScale_ = 0;
    EXPECT_EQ(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), imageBlur);
}

/**
 * @tc.name: ScaleAndAddRandomColor_003
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor_003, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto imageBlur = std::make_shared<Drawing::Image>();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());

    EXPECT_EQ(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), imageBlur);
}

/**
 * @tc.name: ScaleAndAddRandomColor004
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor004, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto image = std::make_shared<Drawing::Image>();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    //imageBlur.GetHeight()=0,imageBlur.GetHeight()=0;
    EXPECT_EQ(filter->ScaleAndAddRandomColor(canvas_, image, imageBlur, src_, dst_, width, height), imageBlur);
}

/**
 * @tc.name: ScaleAndAddRandomColor005
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor005, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1000, 300, format); // 1000, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

/**
 * @tc.name: ScaleAndAddRandomColor006
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor006, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 30, format); // 50, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_GREEN);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, GetDescription_001, TestSize.Level3)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    geKawaseBlurShaderFilter->ComputeRadiusAndScale(200); // 200 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(50); // 50 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(20); // 20 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(9); // 9 blur radius
    std::string expect = "blur radius is " + std::to_string(float(36)); // 36 match result
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
}

/**
 * @tc.name: ComputeRadiusAndScale_002
 * @tc.desc: Verify function ComputeRadiusAndScale
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ComputeRadiusAndScale_002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    geKawaseBlurShaderFilter->ComputeRadiusAndScale(1); // 1 blur radius
    std::string expect = "blur radius is " + std::to_string(float(3));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);

    geKawaseBlurShaderFilter->ComputeRadiusAndScale(2); // 2 blur radius
    expect = "blur radius is " + std::to_string(float(6));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);

    geKawaseBlurShaderFilter->ComputeRadiusAndScale(3); // 3 blur radius
    expect = "blur radius is " + std::to_string(float(12));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
    
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(4); // 4 blur radius
    expect = "blur radius is " + std::to_string(float(15));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
    
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(5); // 5 blur radius
    expect = "blur radius is " + std::to_string(float(18));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
    
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(6); // 6 blur radius
    expect = "blur radius is " + std::to_string(float(24));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
    
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(7); // 7 blur radius
    expect = "blur radius is " + std::to_string(float(27));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
    
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(8); // 8 blur radius
    expect = "blur radius is " + std::to_string(float(30));
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
}


/**
 * @tc.name: InitSimpleFilter_001
 * @tc.desc: Verify function InitSimpleFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitSimpleFilter_001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitSimpleFilter());
}

/**
 * @tc.name: InitSimpleFilter_002
 * @tc.desc: Verify function InitSimpleFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitSimpleFilter_002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitSimpleFilter();
    //g_simpleFilter != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitSimpleFilter());
}

/**
 * @tc.name: InitSimpleFilter_003
 * @tc.desc: Verify function InitSimpleFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitSimpleFilter_003, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitSimpleFilter();
    //g_simpleFilter != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitSimpleFilter());
}

/**
 * @tc.name: InitSimpleFilter_004
 * @tc.desc: Verify function InitSimpleFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitSimpleFilter_004, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 100 }; // 100 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitSimpleFilter();
    //g_simpleFilter != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitSimpleFilter());
}

/**
 * @tc.name: InitBlurEffectForAdvancedFilter_001
 * @tc.desc: Verify function InitBlurEffectForAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitBlurEffectForAdvancedFilter001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter());
}

/**
 * @tc.name: InitBlurEffectForAdvancedFilter_002
 * @tc.desc: Verify function InitBlurEffectForAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitBlurEffectForAdvancedFilter_002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter();
    //g_blurEffectAf != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter());
}

/**
 * @tc.name: InitBlurEffectForAdvancedFilter_003
 * @tc.desc: Verify function InitBlurEffectForAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitBlurEffectForAdvancedFilter003, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter();
    //g_blurEffectAf != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter());
}

/**
 * @tc.name: InitBlurEffectForAdvancedFilter_004
 * @tc.desc: Verify function InitBlurEffectForAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitBlurEffectForAdvancedFilter_004, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 100 }; // 100 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter();
    //g_blurEffectAf != nullptr
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter());
}

/**
* @tc.name: CheckInputImage_001
* @tc.desc: Verify function CheckInputImage
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, CheckInputImage_001, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Rect src { 3.0f, 5.0f, 3.0f, 5.0f };
    geKawaseBlurShaderFilter->CheckInputImage(canvas_, image_, image_, src);
    EXPECT_NE(image_, nullptr);
}

/**
* @tc.name: CheckInputImage_002
* @tc.desc: Verify function CheckInputImage
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, CheckInputImage_002, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Rect src { 3.0f, 5.0f, 3.0f, 5.0f };
    geKawaseBlurShaderFilter->CheckInputImage(canvas_, image_, image_, src);
    EXPECT_NE(image_, nullptr);
}

/**
* @tc.name: CheckInputImage_003
* @tc.desc: Verify function CheckInputImage
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, CheckInputImage_003, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 100 }; // 100 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Rect src { 3.0f, 5.0f, 3.0f, 5.0f };
    geKawaseBlurShaderFilter->CheckInputImage(canvas_, image_, image_, src);
    EXPECT_NE(image_, nullptr);
}

/**
* @tc.name: CheckInputImage_004
* @tc.desc: Verify function CheckInputImage
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, CheckInputImage_004, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Rect src { 30.0f, 50.0f, 30.0f, 50.0f };
    geKawaseBlurShaderFilter->CheckInputImage(canvas_, image_, image_, src);
    EXPECT_NE(image_, nullptr);
}

/**
* @tc.name: CheckInputImage_005
* @tc.desc: Verify function CheckInputImage
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, CheckInputImage_005, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 10 }; // 10 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    Drawing::Rect src { 10.0f, 30.0f, 10.0f, 30.0f };
    geKawaseBlurShaderFilter->CheckInputImage(canvas_, image_, image_, src);
    EXPECT_NE(image_, nullptr);
}

/**
* @tc.name: GetNormalizedOffset_001
* @tc.desc: Verify function getNormalizedOffset
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, GetNormalizedOffset_001, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    const uint32_t offsetCount = BLUR_SAMPLE_COUNT;
    SkV2 offsets[offsetCount];
    OffsetInfo offsetInfo = { 1.f, 1.f, 2, 2 }; // 1.f, 1.f, 2, 2 -> offsetx, offsety, width, height

    GEKawaseBlurShaderFilter::GetNormalizedOffset(offsets, offsetCount, offsetInfo);
    EXPECT_TRUE(fabs(offsets[0].x - 0.f) < FLT_EPSILON); // 0.f init offset x
    EXPECT_TRUE(fabs(offsets[0].y - 0.f) < FLT_EPSILON); // 0.f init offset y
    EXPECT_TRUE(fabs(offsets[1].x - 0.5f) < FLT_EPSILON); // 0.5f offsetx/width
    EXPECT_TRUE(fabs(offsets[1].y - 0.5f) < FLT_EPSILON); // 0.5f offsety/height
}

/**
* @tc.name: GetNormalizedOffset_002
* @tc.desc: Verify function getNormalizedOffset
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, GetNormalizedOffset_002, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    const uint32_t offsetCount = BLUR_SAMPLE_COUNT;
    OffsetInfo offsetInfo = { 1.f, 1.f, 2, 2 }; // 1.f, 1.f, 2, 2 -> offsetx, offsety, width, height
    SkV2* offsets = nullptr;
    //offsets == nullptr
    GEKawaseBlurShaderFilter::GetNormalizedOffset(offsets, offsetCount, offsetInfo);
    EXPECT_EQ(offsets, nullptr);
}

/**
* @tc.name: GetNormalizedOffset_003
* @tc.desc: Verify function getNormalizedOffset
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, GetNormalizedOffset_003, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    const uint32_t offsetCount = 1; // 1 count for test
    SkV2 offsets[offsetCount] = { { 1.f, 1.f } }; // 1.f, 1.f init data: x, y
    OffsetInfo offsetInfo = { 1.f, 1.f, 2, 2 }; // 1.f, 1.f, 2, 2 -> offsetx, offsety, width, height
    //offsetCount != BLUR_SAMPLE_COUNT
    GEKawaseBlurShaderFilter::GetNormalizedOffset(offsets, offsetCount, offsetInfo);
    EXPECT_TRUE(fabs(offsets[0].x - 1.f) < FLT_EPSILON); // 1.f no changed value
    EXPECT_TRUE(fabs(offsets[0].y - 1.f) < FLT_EPSILON); // 1.f no changed value
}

/**
* @tc.name: GetNormalizedOffset_004
* @tc.desc: Verify function getNormalizedOffset
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, GetNormalizedOffset_004, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    const uint32_t offsetCount = BLUR_SAMPLE_COUNT;
    SkV2 offsets[offsetCount];
    offsets[0].x = 1.f; // 1.f, init data: x
    offsets[0].y = 1.f; // 1.f, init data: y
    OffsetInfo offsetInfo = { 1.f, 1.f, 0, 2 }; // 1.f, 1.f, 0, 2 -> offsetx, offsety, width, height
    // offsetInfo.width=0
    GEKawaseBlurShaderFilter::GetNormalizedOffset(offsets, offsetCount, offsetInfo);
    EXPECT_TRUE(fabs(offsets[0].x - 1.f) < FLT_EPSILON); // 1.f no changed value
    EXPECT_TRUE(fabs(offsets[0].y - 1.f) < FLT_EPSILON); // 1.f no changed value
}

/**
* @tc.name: GetNormalizedOffset_005
* @tc.desc: Verify function getNormalizedOffset
* @tc.type:FUNC
*/
HWTEST_F(GEKawaseBlurShaderFilterTest, GetNormalizedOffset_005, TestSize.Level2)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    const uint32_t offsetCount = BLUR_SAMPLE_COUNT;
    SkV2 offsets[offsetCount];
    offsets[0].x = 1.f; // 1.f, init data: x
    offsets[0].y = 1.f; // 1.f, init data: y
    OffsetInfo offsetInfo = { 1.f, 1.f, 2, 0 }; // 1.f, 1.f, 2, 0 -> offsetx, offsety, width, height
    // offsetInfo.height=0
    GEKawaseBlurShaderFilter::GetNormalizedOffset(offsets, offsetCount, offsetInfo);
    EXPECT_TRUE(fabs(offsets[0].x - 1.f) < FLT_EPSILON); // 1.f no changed value
    EXPECT_TRUE(fabs(offsets[0].y - 1.f) < FLT_EPSILON); // 1.f no changed value
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
