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
#include "ge_linear_gradient_shader_mask.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

namespace {
class GENullShaderFilter : public GEShaderFilter {
public:
    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas&,
        const std::shared_ptr<Drawing::Image>, const Drawing::Rect&, const Drawing::Rect&) override
    {
        return nullptr;
    }
};
}

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
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);

    // empty fractionStops -> constructor guard clears fractionStops_
    Drawing::GELinearGradientBlurShaderFilterParams emptyParams{1.f, {}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto emptyFilter = std::make_shared<GELinearGradientBlurShaderFilter>(emptyParams);
    ASSERT_TRUE(emptyFilter->linearGradientBlurPara_ != nullptr);
    EXPECT_TRUE(emptyFilter->linearGradientBlurPara_->fractionStops_.empty());

    // fractionStops size > MAX(1000) -> constructor guard clears fractionStops_
    constexpr size_t EXCEED_STOPS_SIZE = 1001;
    std::vector<std::pair<float, float>> stops(EXCEED_STOPS_SIZE, {0.1f, 0.1f});
    Drawing::GELinearGradientBlurShaderFilterParams overParams{1.f, stops, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto overFilter = std::make_shared<GELinearGradientBlurShaderFilter>(overParams);
    ASSERT_TRUE(overFilter->linearGradientBlurPara_ != nullptr);
    EXPECT_TRUE(overFilter->linearGradientBlurPara_->fractionStops_.empty());
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src, dst), nullptr);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 0 direction LEFT, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 0, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter0 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter0 != nullptr);
    EXPECT_NE(filter0->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 2; // RIGHT
    auto filter2 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter2 != nullptr);
    EXPECT_NE(filter2->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 3; // BOTTOM
    auto filter3 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter3 != nullptr);
    EXPECT_NE(filter3->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 4; // LEFT_TOP
    auto filter4 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter4 != nullptr);
    EXPECT_NE(filter4->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 5; // LEFT_BOTTOM
    auto filter5 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter5 != nullptr);
    EXPECT_NE(filter5->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 6; // RIGHT_TOP
    auto filter6 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter6 != nullptr);
    EXPECT_NE(filter6->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    params.direction = 7; // RIGHT_BOTTOM
    auto filter7 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter7 != nullptr);
    EXPECT_NE(filter7->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_005, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_006
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_006, TestSize.Level0)
{
    // blur params: -1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{-1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    // test para's blurRadius <= 0
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
    
        // test OnProcessImage with para being nullptr
    filter->linearGradientBlurPara_ = nullptr;
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_007
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_007, TestSize.Level0)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    // test OnProcessImage with para's linearGradientBlurPara being nullptr
    filter->linearGradientBlurPara_->linearGradientBlurFilter_ = nullptr;
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_008
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, OnProcessImage_008, TestSize.Level0)
{
    // blur params: 10.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{10.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_NE(filter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
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
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, Type_001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
}

/**
 * @tc.name: DrawMaskLinearGradientBlurInvalidDst
 * @tc.desc: Verify DrawMaskLinearGradientBlur returns original image when dst size is not positive or blur is null
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, DrawMaskLinearGradientBlurInvalidDst, TestSize.Level0)
{
    ASSERT_NE(image_, nullptr);
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto blurFilter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    std::shared_ptr<GEShaderFilter> filter = blurFilter;

    Drawing::Rect zeroWidthDst {0.0f, 0.0f, 0.0f, 200.0f};
    auto out = GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur(
        image_, canvas_, filter, nullptr, zeroWidthDst);
    EXPECT_EQ(out.get(), image_.get());

    Drawing::Rect zeroHeightDst {0.0f, 0.0f, 200.0f, 0.0f};
    out = GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur(
        image_, canvas_, filter, nullptr, zeroHeightDst);
    EXPECT_EQ(out.get(), image_.get());

    auto nullBlur = std::make_shared<GENullShaderFilter>();
    std::shared_ptr<GEShaderFilter> nullFilter = nullBlur;
    Drawing::Rect dst {0.0f, 0.0f, 100.0f, 100.0f};
    out = GELinearGradientBlurShaderFilter::DrawMaskLinearGradientBlur(
        image_, canvas_, nullFilter, nullptr, dst);
    EXPECT_EQ(out.get(), image_.get());
}

/**
 * @tc.name: ProcessImageDDGR_NullImage
 * @tc.desc: Verify ProcessImageDDGR returns input when image is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImageDDGR_NullImage, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> nullImage = nullptr;
    EXPECT_EQ(filter->ProcessImageDDGR(canvas_, nullImage, 0), nullImage);
}

/**
 * @tc.name: MakeMaskLinearGradientBlurShader_NullShaders
 * @tc.desc: Verify MakeMaskLinearGradientBlurShader returns nullptr when any shader parameter is null
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, MakeMaskLinearGradientBlurShader_NullShaders, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true, false};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto result = filter->MakeMaskLinearGradientBlurShader(nullptr, nullptr, nullptr);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: LinearGradientShaderMask_InvalidFractionStops
 * @tc.desc: Verify GELinearGradientShaderMask returns nullptr for empty or over-limit fractionStops
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, LinearGradientShaderMask_InvalidFractionStops, TestSize.Level1)
{
    Drawing::GELinearGradientShaderMaskParams emptyParam;
    emptyParam.fractionStops = {};
    emptyParam.startPosition = Drawing::Point(0.0f, 0.0f);
    emptyParam.endPosition = Drawing::Point(100.0f, 0.0f);
    Drawing::GELinearGradientShaderMask emptyMask(emptyParam);
    EXPECT_EQ(emptyMask.GenerateDrawingShader(100.0f, 100.0f), nullptr);

    constexpr size_t OVER_LIMIT_SIZE = 1001;
    std::vector<std::pair<float, float>> overStops(OVER_LIMIT_SIZE, {0.5f, 0.5f});
    Drawing::GELinearGradientShaderMaskParams overParam;
    overParam.fractionStops = overStops;
    overParam.startPosition = Drawing::Point(0.0f, 0.0f);
    overParam.endPosition = Drawing::Point(100.0f, 0.0f);
    Drawing::GELinearGradientShaderMask overMask(overParam);
    EXPECT_EQ(overMask.GenerateDrawingShader(100.0f, 100.0f), nullptr);

    Drawing::GELinearGradientShaderMaskParams validParam;
    validParam.fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    validParam.startPosition = Drawing::Point(0.0f, 0.0f);
    validParam.endPosition = Drawing::Point(100.0f, 0.0f);
    Drawing::GELinearGradientShaderMask validMask(validParam);
    EXPECT_NE(validMask.GenerateDrawingShader(100.0f, 100.0f), nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
