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

#include "ge_linear_gradient_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GELinearGradientBlurShaderFilterTest : public testing::Test {
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

void GELinearGradientBlurShaderFilterTest::SetUpTestCase(void) {}
void GELinearGradientBlurShaderFilterTest::TearDownTestCase(void) {}

void GELinearGradientBlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GELinearGradientBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDescription_001, TestSize.Level3)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilter";
    EXPECT_EQ(filter->GetDescription(), expectStr);
}

/**
 * @tc.name: GetDetailedDescription_001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDetailedDescription_001, TestSize.Level3)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilterBlur, radius: " +std::to_string(params.blurRadius);
    EXPECT_EQ(filter->GetDetailedDescription(), expectStr);
}

/**
 * @tc.name: GetDetailedDescription_002
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDetailedDescription_002, TestSize.Level3)
{
    // blur params: 1.5 f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilterBlur, radius: " +std::to_string(params.blurRadius);
    EXPECT_EQ(filter->GetDetailedDescription(), expectStr);
}

/**
 * @tc.name: GetDetailedDescription_003
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDetailedDescription_003, TestSize.Level3)
{
    // blur params: 10.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{10.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilterBlur, radius: " +std::to_string(params.blurRadius);
    EXPECT_EQ(filter->GetDetailedDescription(), expectStr);
}

/**
 * @tc.name: ProcessImage_001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_001, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage_002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_002, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_NE(filter->ProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: ProcessImage_003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_003, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    EXPECT_NE(filter->ProcessImage(canvas_, image_, src, dst), nullptr);
}

/**
 * @tc.name: ProcessImage_004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_004, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 0 direction LEFT, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 0, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter0 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter0 != nullptr);
    EXPECT_NE(filter0->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 2; // RIGHT
    auto filter2 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter2 != nullptr);
    EXPECT_NE(filter2->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 3; // BOTTOM
    auto filter3 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter3 != nullptr);
    EXPECT_NE(filter3->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 4; // LEFT_TOP
    auto filter4 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter4 != nullptr);
    EXPECT_NE(filter4->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 5; // LEFT_BOTTOM
    auto filter5 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter5 != nullptr);
    EXPECT_NE(filter5->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 6; // RIGHT_TOP
    auto filter6 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter6 != nullptr);
    EXPECT_NE(filter6->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 7; // RIGHT_BOTTOM
    auto filter7 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter7 != nullptr);
    EXPECT_NE(filter7->ProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: ProcessImage_005
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_005, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage_006
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_006, TestSize.Level0)
{
    // blur params: -1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{-1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    // test para's blurRadius <= 0
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
    
        // test ProcessImage with para being nullptr
    filter->linearGradientBlurPara_ = nullptr;
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage_007
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_007, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    // test ProcessImage with para's linearGradientBlurPara being nullptr
    filter->linearGradientBlurPara_->linearGradientBlurFilter_ = nullptr;
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage_008
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage_008, TestSize.Level0)
{
    // blur params: 10.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{10.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_NE(filter->ProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: CalcDirectionBias_001
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.002f); // 0.002f skew x
    mat.Set(Drawing::Matrix::SKEW_Y, -0.002f); // -0.002f skew y
    EXPECT_EQ(filter->CalcDirectionBias(mat), 1); // 1 Bias
}

/**
 * @tc.name: CalcDirectionBias_002
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_002, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.0005f); // 0.0005f skew x
    mat.Set(Drawing::Matrix::SCALE_X, -0.002f); // -0.002f scale x
    mat.Set(Drawing::Matrix::SCALE_Y, -0.002f); // -0.002f scale y
    EXPECT_EQ(filter->CalcDirectionBias(mat), 2); // 2 Bias
}

/**
 * @tc.name: CalcDirectionBias_003
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_003, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, -0.002f); // 0.002f skew x
    mat.Set(Drawing::Matrix::SKEW_Y, 0.002f); // 0.002f skew y
    mat.Set(Drawing::Matrix::SCALE_X, 0.02f); // 0.02f scale x
    EXPECT_EQ(filter->CalcDirectionBias(mat), 3); // 2 Bias
}

/**
 * @tc.name: CalcDirectionBias_004
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_004, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.0005f); // 0.0005f skew x
    mat.Set(Drawing::Matrix::SKEW_Y, 0.0005f); // 0.0005f skew y
    mat.Set(Drawing::Matrix::SCALE_X, 0.02f); // 0.02f scale x
    EXPECT_EQ(filter->CalcDirectionBias(mat), 0); // 0 no Bias
}

/**
 * @tc.name: CalcDirectionBias_005
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_005, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.002f); // 0.002f skew x
    mat.Set(Drawing::Matrix::SKEW_Y, 0.002f); // 0.002f skew y
    EXPECT_EQ(filter->CalcDirectionBias(mat), 0); // 0 Bias
}

/**
 * @tc.name: CalcDirectionBias_006
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, CalcDirectionBias_006, TestSize.Level1)
{
    // blur params: 10.f blurRadius, {0.1f, 0.1f} fractionStops, 2 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{10.f, {{0.1f, 0.1f}}, 2, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, -0.002f); // 0.002f skew x
    mat.Set(Drawing::Matrix::SKEW_Y, 0.002f); // 0.002f skew y
    mat.Set(Drawing::Matrix::SCALE_X, 0.02f); // 0.02f scale x
    EXPECT_EQ(filter->CalcDirectionBias(mat), 3); // 2 Bias
}

/**
 * @tc.name: ProcessImageDDGR_001
 * @tc.desc: Verify function ProcessImageDDGR
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImageDDGR_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 2 valid Bias
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, image_, 2), image_);
}

/**
 * @tc.name: ProcessImageDDGR_002
 * @tc.desc: Verify function ProcessImageDDGR
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImageDDGR_002, TestSize.Level1)
{
    // blur params: 1001.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{1001.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 2 valid Bias
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, image_, 2), image_);
}

/**
 * @tc.name: ProcessImageDDGR_003
 * @tc.desc: Verify function ProcessImageDDGR
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImageDDGR_003, TestSize.Level1)
{
    // blur params: 1001.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1001.1f, {{0.1, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, false, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 2 valid Bias
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, image_, 2), image_);
}

/**
 * @tc.name: ComputeScale_001
 * @tc.desc: Verify function ComputeScale
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ComputeScale_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 100, 90 image size: width, height
    filter->ComputeScale(100, 90, false);

    // 2 valid Bias
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, image_, 2), image_);
}

/**
 * @tc.name: ComputeScale_002
 * @tc.desc: Verify function ComputeScale
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ComputeScale_002, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 100, 200 image size: width, height
    filter->ComputeScale(100, 200, false);

    // 2 valid Bias
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, image_, 2), image_);
}

/**
 * @tc.name: TransformGradientBlurDirection_001
 * @tc.desc: Verify function TransformGradientBlurDirection
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, TransformGradientBlurDirection_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    uint8_t direction = 5; // 5 direction value
    filter->TransformGradientBlurDirection(direction, 4); // 4 bias
    EXPECT_EQ(direction, 5); // 5 value after ransform

    direction = 5; // 5 direction value
    filter->TransformGradientBlurDirection(direction, 2); // 2 bias
    EXPECT_EQ(direction, 6); // 6 value after ransform

    direction = 6; // 6 direction value
    filter->TransformGradientBlurDirection(direction, 2); // 2 bias
    EXPECT_EQ(direction, 5); // 5 value after ransform

    direction = 6; // 6 direction value
    filter->TransformGradientBlurDirection(direction, 0); // 0 bias
    EXPECT_EQ(direction, 6); // 6 value after ransform

    direction = 8; // 8 direction value
    filter->TransformGradientBlurDirection(direction, 2); // 2 bias
    EXPECT_EQ(direction, 7); // 7 value after transform

    direction = 3; // 3 direction value
    filter->TransformGradientBlurDirection(direction, 0); // 4 bias
    EXPECT_EQ(direction, 3); // 3 value after transform
}

/**
 * @tc.name: DrawMeanLinearGradientBlur_001
 * @tc.desc: Verify function DrawMeanLinearGradientBlur
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, DrawMeanLinearGradientBlur_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    // test DrawMeanLinearGradientBlur with invalid imageScale
    GELinearGradientBlurShaderFilter::imageScale_ = 0.f;
    float radius = filter->linearGradientBlurPara_->blurRadius_;
    radius = std::clamp(radius, 0.0f, 60.0f); // 60.0 represents largest blur radius
    radius = radius / 2 * GELinearGradientBlurShaderFilter::imageScale_;        // 2 half blur radius
    
    std::shared_ptr<Drawing::Image> originalImage = image_;
    // without HorizontalMeanBlurEffect and VerticalMeanBlurEffect
    filter->DrawMeanLinearGradientBlur(image_, canvas_, radius, nullptr, dst_);

    // with HorizontalMeanBlurEffect
    filter->MakeHorizontalMeanBlurEffect();
    filter->DrawMeanLinearGradientBlur(image_, canvas_, radius, nullptr, dst_);

    // with VerticalMeanBlurEffect
    filter->MakeVerticalMeanBlurEffect();
    filter->DrawMeanLinearGradientBlur(image_, canvas_, radius, nullptr, dst_);
    filter->MakeHorizontalMeanBlurEffect();
    filter->MakeVerticalMeanBlurEffect();

    // test if image input is null
    filter->DrawMeanLinearGradientBlur(nullptr, canvas_, radius, nullptr, dst_);
}

/**
 * @tc.name: MakeAlphaGradientShader_001
 * @tc.desc: Verify function MakeAlphaGradientShader
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, MakeAlphaGradientShader_001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.005f}, {0.1f, 1.f}}, 7, 1.f, 1.f,
    Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    filter->isOffscreenCanvas_ = false;
    filter->ComputeScale(dst_.GetWidth(), dst_.GetHeight(), !filter->linearGradientBlurPara_->isRadiusGradient_);
    auto clipIPadding = Drawing::Rect(0, 0, filter->geoWidth_ * filter->imageScale_,
        filter->geoHeight_ * filter->imageScale_);
    uint8_t directionBias = 1;
    auto alphaGradientShader = filter->MakeAlphaGradientShader(clipIPadding, filter->linearGradientBlurPara_,
        directionBias);
    EXPECT_NE(alphaGradientShader, nullptr);
}

/**
 * @tc.name: DrawMaskLinearGradientBlur_001
 * @tc.desc: Verify function DrawMaskLinearGradientBlur
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, DrawMaskLinearGradientBlur_001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // image is null
    EXPECT_EQ(filter->DrawMaskLinearGradientBlur(nullptr, canvas_,
        filter->linearGradientBlurPara_->linearGradientBlurFilter_, nullptr, dst_), nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
