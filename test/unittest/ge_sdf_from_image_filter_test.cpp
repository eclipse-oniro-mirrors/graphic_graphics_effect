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

#include <cmath>
#include <gtest/gtest.h>

#include "draw/color.h"
#include "image/bitmap.h"
#include "ge_sdf_from_image_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GESDFFromImageFilterTest : public testing::Test {
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

void GESDFFromImageFilterTest::SetUpTestCase(void) {}
void GESDFFromImageFilterTest::TearDownTestCase(void) {}

void GESDFFromImageFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GESDFFromImageFilterTest::TearDown() {}

/**
 * @tc.name: GetSpreadFactor_001
 * @tc.desc: Verify function GetSpreadFactor
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, GetSpreadFactor_001, TestSize.Level3)
{
    // 64, false: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params { 64, false };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    EXPECT_EQ(geSDFFromImageFilter->GetSpreadFactor(), params.spreadFactor);
}

/**
 * @tc.name: GetSpreadFactor_002
 * @tc.desc: Verify function GetSpreadFactor
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, GetSpreadFactor_002, TestSize.Level3)
{
    // 4096, true: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params { 4096, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    EXPECT_EQ(geSDFFromImageFilter->GetSpreadFactor(), params.spreadFactor);
}

/**
 * @tc.name: GetSpreadFactor_003
 * @tc.desc: Verify function GetSpreadFactor
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, GetSpreadFactor_003, TestSize.Level3)
{
    // 1, true: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params { 1, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    EXPECT_EQ(geSDFFromImageFilter->GetSpreadFactor(), params.spreadFactor);
}

/**
 * @tc.name: GetSpreadFactor_004
 * @tc.desc: Verify function GetSpreadFactor
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, GetSpreadFactor_004, TestSize.Level3)
{
    // 0, true: invalid SDF creation params, expect spreadFactor to change to 1
    Drawing::GESDFFromImageFilterParams params { 0, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    EXPECT_EQ(geSDFFromImageFilter->GetSpreadFactor(), 1);
}

/**
 * @tc.name: GetSpreadFactor_005
 * @tc.desc: Verify function GetSpreadFactor
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, GetSpreadFactor_005, TestSize.Level3)
{
    // 8000, false: invalid SDF creation params, expect spreadFactor to change to 4096
    Drawing::GESDFFromImageFilterParams params { 8000, false };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    EXPECT_EQ(geSDFFromImageFilter->GetSpreadFactor(), 4096);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_001, TestSize.Level0)
{
    // 64, true: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params { 64, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(geSDFFromImageFilter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_002, TestSize.Level0)
{
    // 64, true: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params1 { 64, true };
    auto geSDFFromImageFilter1 = std::make_shared<GESDFFromImageFilter>(params1);
    ASSERT_TRUE(geSDFFromImageFilter1 != nullptr);

    EXPECT_EQ(geSDFFromImageFilter1->OnProcessImage(canvas_, image_, src_, dst_), image_);

    // 4097, true: invalid SDF creation params, expect spreadFactor to change to 4096
    Drawing::GESDFFromImageFilterParams params2 { 4097, true };
    auto geSDFFromImageFilter2 = std::make_shared<GESDFFromImageFilter>(params2);
    ASSERT_TRUE(geSDFFromImageFilter2 != nullptr);

    EXPECT_EQ(geSDFFromImageFilter2->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_003, TestSize.Level0)
{
    // 1,7,12,18,21,55,81,120,240,360: valid sdf generation spread factors
    int spreadFactors[] = { 1, 7, 12, 18, 21, 55, 81, 120, 240, 360 };
    for (auto spreadFactor : spreadFactors) {
        // true: valid sdf generation generateDerivs param
        Drawing::GESDFFromImageFilterParams params { spreadFactor, true };
        auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
        ASSERT_TRUE(geSDFFromImageFilter != nullptr);

        EXPECT_EQ(geSDFFromImageFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }

    for (auto spreadFactor : spreadFactors) {
        // false: valid sdf generation generateDerivs param
        Drawing::GESDFFromImageFilterParams params { spreadFactor, false };
        auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
        ASSERT_TRUE(geSDFFromImageFilter != nullptr);

        EXPECT_EQ(geSDFFromImageFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, Type_001, TestSize.Level1)
{
    // // 64, true: valid SDF creation params
    Drawing::GESDFFromImageFilterParams params { 64, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    EXPECT_EQ(geSDFFromImageFilter->Type(), Drawing::GEFilterType::SDF_FROM_IMAGE);
    EXPECT_EQ(geSDFFromImageFilter->TypeName(), Drawing::GE_FILTER_SDF_FROM_IMAGE);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify OnProcessImage with RGBA_F16 color type for intermediate processing
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_004, TestSize.Level0)
{
    // Test that the filter handles RGBA_F16 intermediate color type correctly
    Drawing::GESDFFromImageFilterParams params { 64, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    // Create an image with RGBA_F16 format
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto f16Image = bmp.MakeImage();
    Drawing::Rect imgBounds = f16Image->GetImageInfo().GetBound();

    auto result = geSDFFromImageFilter->OnProcessImage(canvas_, f16Image, imgBounds, imgBounds);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify OnProcessImage with various color types
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_005, TestSize.Level0)
{
    Drawing::GESDFFromImageFilterParams params { 32, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    // Test with different color types
    Drawing::ColorType colorTypes[] = { Drawing::COLORTYPE_RGBA_8888, Drawing::COLORTYPE_BGRA_8888,
        Drawing::COLORTYPE_RGBA_F16 };

    for (auto colorType : colorTypes) {
        Drawing::Bitmap bmp;
        Drawing::BitmapFormat format { colorType, Drawing::ALPHATYPE_PREMUL };
        bmp.Build(50, 50, format);
        bmp.ClearWithColor(Drawing::Color::COLOR_RED);
        auto testImage = bmp.MakeImage();
        Drawing::Rect imgBounds = testImage->GetImageInfo().GetBound();

        auto result = geSDFFromImageFilter->OnProcessImage(canvas_, testImage, imgBounds, imgBounds);
        EXPECT_NE(result, nullptr);
        // Result should maintain the original color type
        EXPECT_EQ(result->GetColorType(), colorType);
    }
}

/**
 * @tc.name: OnProcessImage_009
 * @tc.desc: Verify OnProcessImage without derivative generation
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_009, TestSize.Level0)
{
    // Test with generateDerivs = false
    Drawing::GESDFFromImageFilterParams params { 64, false };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    auto result = geSDFFromImageFilter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
    // When generateDerivs is false, should still produce valid SDF
    EXPECT_EQ(result->GetColorType(), image_->GetColorType());
}

/**
 * @tc.name: OnProcessImage_010
 * @tc.desc: Verify OnProcessImage with non-uniform source and destination rectangles
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_010, TestSize.Level0)
{
    Drawing::GESDFFromImageFilterParams params { 64, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    // Test with different src and dst rectangles to verify stretch matrix calculation
    Drawing::Rect smallSrc { 10.0f, 10.0f, 20.0f, 20.0f };
    Drawing::Rect largeDst { 0.0f, 0.0f, 100.0f, 100.0f };

    auto result = geSDFFromImageFilter->OnProcessImage(canvas_, image_, smallSrc, largeDst);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_011
 * @tc.desc: Verify OnProcessImage with small images
 * @tc.type:FUNC
 */
HWTEST_F(GESDFFromImageFilterTest, OnProcessImage_011, TestSize.Level0)
{
    Drawing::GESDFFromImageFilterParams params { 32, true };
    auto geSDFFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    ASSERT_TRUE(geSDFFromImageFilter != nullptr);

    // Test with very small image (edge case)
    Drawing::Bitmap smallBmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    smallBmp.Build(4, 4, format);
    smallBmp.ClearWithColor(Drawing::Color::COLOR_GREEN);
    auto smallImage = smallBmp.MakeImage();
    Drawing::Rect smallBounds = smallImage->GetImageInfo().GetBound();

    auto result = geSDFFromImageFilter->OnProcessImage(canvas_, smallImage, smallBounds, smallBounds);
    EXPECT_NE(result, nullptr);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
