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

#include "ge_frosted_glass_shader_filter.h"

#include "draw/color.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
constexpr size_t NUM_2 = 2;
constexpr size_t NUM_50 = 50;

class GEFrostedGlassShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;          // CPU canvas (no GPU context)
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f → left, top, right, bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 200.0f, 200.0f };

private:
    Drawing::GEFrostedGlassShaderFilterParams MakeParams();
    Drawing::CanvasInfo canvasInfo_;
};

void GEFrostedGlassShaderFilterTest::SetUpTestCase() {}

void GEFrostedGlassShaderFilterTest::TearDownTestCase() {}

void GEFrostedGlassShaderFilterTest::SetUp()
{
    // Build a 50x50 RGBA8888 bitmap and fill with BLUE, then convert to Image
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(NUM_50, NUM_50, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEFrostedGlassShaderFilterTest::TearDown()
{
    image_ = nullptr;
}

Drawing::GEFrostedGlassShaderFilterParams GEFrostedGlassShaderFilterTest::MakeParams()
{
    Drawing::GEFrostedGlassShaderFilterParams p; // Default parameters
    // COMMON
    p.borderSize = {80.0, 50.f};
    p.cornerRadius = 20.0f;
    p.borderWidth = 6.0f;
    p.offset = 2.0f;
    p.downSampleFactor = 2.0f;
    // BACKGROUND
    p.bgFactor = 0.85f;
    // INNER SHADOW
    p.innerShadowParams= {3.5f, 1.25f, 1.75f};
    p.sdParams = {1.0f, 2.0f, 1.2f};
    // ENV LIGHT
    p.refractOutPx = 4.0f;
    p.envParams = {1.1f, 0.0, 1.1f};
    // EDGELIGHT
    p.edgeLightAngle= {30.0f, 30.0f, 3.0f};
    p.edgeLightBlur = {2.0f, -1.0f};
    p.edgeLightDir = {-1.0f, 1.0f};
    // BLUR
    p.blurParams = {48.0f, 4.0f};
    return p;
}

/**
 * @tc.name: MakeLargeRadiusBlurImg_InvalidInputs
 * @tc.desc: Verify MakeLargeRadiusBlurImg return nullptr when input image is invalid (nullptr).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeLargeRadiusBlurImg_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    // Case: image == nullptr
    EXPECT_EQ(filter->MakeLargeRadiusBlurImg(canvas_, src_, dst_, nullptr), nullptr);
}

/**
 * @tc.name: MakeSmallRadiusBlurImg_InvalidInputs
 * @tc.desc: Verify MakeSmallRadiusBlurImg return nullptr when input image is invalid (nullptr).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeSmallRadiusBlurImg_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    // Case: image == nullptr
    EXPECT_EQ(filter->MakeSmallRadiusBlurImg(canvas_, src_, dst_, nullptr), nullptr);
}

/**
 * @tc.name: CreateLargeRadiusBlurShader_InvalidInputs
 * @tc.desc: Verify CreateLargeRadiusBlurShader return nullptr when input image is invalid (nullptr).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, CreateLargeRadiusBlurShader_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    Drawing::Matrix invertMatrix;

    // Case 1: image == nullptr caused largeRBlurImg == nullptr
    EXPECT_EQ(filter->CreateLargeRadiusBlurShader(canvas_, nullptr, src_, dst_, invertMatrix), nullptr);
}

/**
 * @tc.name: CreateSmallRadiusBlurShader_InvalidInputs
 * @tc.desc: Verify CreateSmallRadiusBlurShader return nullptr when input image is invalid (nullptr).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, CreateSmallRadiusBlurShader_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    Drawing::Matrix invertMatrix;

    // Case 1: image == nullptr caused largeRBlurImg == nullptr
    EXPECT_EQ(filter->CreateSmallRadiusBlurShader(canvas_, nullptr, src_, dst_, invertMatrix), nullptr);
}

/**
 * @tc.name: OnProcessImage_InvalidInputs
 * @tc.desc: Verify OnProcessImage returns nullptr when input image is invalid (nullptr or zero dimension).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    // Case 1: image == nullptr
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    // Prepare a bitmap helper
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

    // Case 2: width > 0, height == 0
    bmp.Build(NUM_1, NUM_0, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto imgH0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgH0, src_, dst_), nullptr);

    // Case 3: width == 0, height > 0
    bmp.Build(NUM_0, NUM_1, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLACK);
    auto imgW0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgW0, src_, dst_), nullptr);

    // Case 4: width == 0, height == 0
    bmp.Build(NUM_0, NUM_0, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto img00 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, img00, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_CPUFallback
 * @tc.desc: On CPU Canvas (no GPU context), final image build is likely nullptr → filter returns original image.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_CPUFallback, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    ASSERT_NE(image_, nullptr);
    // On CPU canvas, builder->MakeImage usually returns nullptr, so OnProcessImage should return the input image.
    auto out = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out.get(), image_.get());
}

/**
 * @tc.name: InitEffect_Twice
 * @tc.desc: Verify InitFrostedGlassEffect twice (first time creates effect, second time returns true directly).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, InitEffect_Twice, TestSize.Level0)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    EXPECT_TRUE(filter.InitFrostedGlassEffect()); // first time (create)
    EXPECT_TRUE(filter.InitFrostedGlassEffect()); // second time (already exists)
}

/**
 * @tc.name: MakeFrostedGlassShader_Smoke
 * @tc.desc: Build a RuntimeShaderBuilder with tiny images as children (CPU path).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeFrostedGlassShader_Smoke, TestSize.Level0)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);
    ASSERT_TRUE(filter.InitFrostedGlassEffect());

    // Create three tiny images from bitmaps to be used as child shaders
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

    Drawing::Bitmap bmpImg;
    bmpImg.Build(NUM_2, NUM_2, fmt);
    bmpImg.ClearWithColor(Drawing::Color::COLOR_WHITE);
    Drawing::Bitmap bmpBig;
    bmpBig.Build(NUM_2, NUM_2, fmt);
    bmpBig.ClearWithColor(Drawing::Color::COLOR_GREEN);
    Drawing::Bitmap bmpSml;
    bmpSml.Build(NUM_2, NUM_2, fmt);
    bmpSml.ClearWithColor(Drawing::Color::COLOR_RED);

    auto imgShaderImg = bmpImg.MakeImage();
    auto imgShaderBig = bmpBig.MakeImage();
    auto imgShaderSml = bmpSml.MakeImage();

    ASSERT_NE(imgShaderImg, nullptr);
    ASSERT_NE(imgShaderBig, nullptr);
    ASSERT_NE(imgShaderSml, nullptr);

    // Build child ShaderEffects with identity matrix
    Drawing::Matrix m = canvasInfo_.mat;
    auto childImage = Drawing::ShaderEffect::CreateImageShader(
        *imgShaderImg, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto childBig = Drawing::ShaderEffect::CreateImageShader(
        *imgShaderBig, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto childSml = Drawing::ShaderEffect::CreateImageShader(
        *imgShaderSml, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);

    ASSERT_NE(childImage, nullptr);
    ASSERT_NE(childBig, nullptr);
    ASSERT_NE(childSml, nullptr);

    auto builder = filter.MakeFrostedGlassShader(
        childImage, childBig, childSml,
        static_cast<float>(imgShaderImg->GetWidth()),
        static_cast<float>(imgShaderImg->GetHeight()));
    EXPECT_NE(builder, nullptr);
}

} // namespace Rosen
} // namespace OHOS
