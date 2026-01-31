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

#include "ge_mesa_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEMESABlurShaderFilterTest : public testing::Test {
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

void GEMESABlurShaderFilterTest::SetUpTestCase(void) {}

void GEMESABlurShaderFilterTest::TearDownTestCase(void) {}

void GEMESABlurShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEMESABlurShaderFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: GetRadius_001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius_001, TestSize.Level3)
{
    // 1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: GetRadius_002
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius_002, TestSize.Level3)
{
    // 8000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{8000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), params.radius);
}

/**
 * @tc.name: GetRadius_003
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius_003, TestSize.Level3)
{
    // 10000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{10000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 8000);
}

/**
 * @tc.name: GetRadius_004
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius_004, TestSize.Level3)
{
    // 0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 0);
}


/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    // 0, 1, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(geMESABlurShaderFilter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    // 0, 0.f, 1.f: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params1{0, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter1 = std::make_shared<GEMESABlurShaderFilter>(params1);
    ASSERT_TRUE(geMESABlurShaderFilter1 != nullptr);

    EXPECT_NE(geMESABlurShaderFilter1->OnProcessImage(canvas_, image_, src_, dst_), image_);

    // 8001: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params2{8001, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f};
    auto geMESABlurShaderFilter2 = std::make_shared<GEMESABlurShaderFilter>(params2);
    ASSERT_TRUE(geMESABlurShaderFilter2 != nullptr);

    EXPECT_NE(geMESABlurShaderFilter2->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    // 1,7,12,18,21,55,81,120,240,360: valid blur radius
    int blurRadius[] = {1, 7, 12, 18, 21, 55, 81, 120, 240, 360};
    for (auto radius : blurRadius) {
        // 0, 0.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_NE(geMESABlurShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }

    for (auto radius : blurRadius) {
        // 1, 1.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1, 1.f, 1.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_NE(geMESABlurShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }

    for (auto radius : blurRadius) {
        // 2, 2.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f, 2, 2.f, 2.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_NE(geMESABlurShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    // 1,7,12,18,21,55,81,120,240,360: valid blur radius
    int blurRadius[] = {1, 7, 12, 18, 21, 55, 81, 120, 240, 360};
    for (auto radius : blurRadius) {
        // 0, 0.f, 1.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_NE(geMESABlurShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: ScaleAndAddRandomColor_001
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ScaleAndAddRandomColor_001, TestSize.Level1)
{
    // 1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params {1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto filter = std::make_shared<GEMESABlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);

    // 88: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params2 {88, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto filter2 = std::make_shared<GEMESABlurShaderFilter>(params2);
    EXPECT_NE(filter2->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

/**
 * @tc.name: SetMesaModeByCCM_001
 * @tc.desc: Verify function SetMesaModeByCCM
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, SetMesaModeByCCM_001, TestSize.Level1)
{
    int num = GEMESABlurShaderFilter::g_isSimpleX;
    for (int mode = 0; mode < 4; mode++) {
        GEMESABlurShaderFilter::SetMesaModeByCCM(mode);
        EXPECT_EQ(GEMESABlurShaderFilter::g_isSimpleX, std::max(mode, num));
        GEMESABlurShaderFilter::SetMesaModeByCCM(num);
    }
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, Type_001, TestSize.Level1)
{
    // 0, 0.f, 1.f: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{0, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    EXPECT_EQ(geMESABlurShaderFilter->Type(), Drawing::GEFilterType::MESA_BLUR);
    EXPECT_EQ(geMESABlurShaderFilter->TypeName(), Drawing::GE_FILTER_MESA_BLUR);
}

/**
 * @tc.name: DirectionBlur_001
 * @tc.desc: test Blur by Direction
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, DirectionBlur_001, TestSize.Level1)
{
    // 10, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params {10, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    EXPECT_EQ(geMESABlurShaderFilter->isDirection_,  false);

    params.isDirection = true;
    geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    auto image = geMESABlurShaderFilter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(image, image_);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
