/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
    bmp.Build(50, 50, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEFrostedGlassShaderFilterTest::TearDown()
{
    image_ = nullptr;
}

Drawing::GEFrostedGlassShaderFilterParams GEFrostedGlassShaderFilterTest::MakeParams()
{
    Drawing::GEFrostedGlassShaderFilterParams p;

    // COMMON
    p.borderSize[0] = 80.0f;  // halfsize.x
    p.borderSize[1] = 50.0f;  // halfsize.y
    p.cornerRadius = 20.0f;
    p.borderWidth = 6.0f;
    p.offset = 2.0f;
    p.downSampleFactor = 2.0f;

    // BACKGROUND
    p.bgFactor = 0.85f;

    // INNER SHADOW
    p.innerShadowParams[0] = 3.5f;  // innerShadowRefractPx
    p.innerShadowParams[1] = 1.25f; // innerShadowWidth
    p.innerShadowParams[2] = 1.75f; // innerShadowExp
    p.sdParams[0] = 1.0f;
    p.sdParams[1] = 2.0f;
    p.sdParams[2] = 1.2f;

    // ENV LIGHT
    p.refractOutPx = 4.0f;
    p.envParams[0] = 1.1f;
    p.envParams[1] = 0.0f;
    p.envParams[2] = 1.1f;

    // HIGHLIGHT
    p.highLightParams[0] = 55.0f; // angleDeg
    p.highLightParams[1] = 25.0f; // featherDeg
    p.highLightParams[2] = 3.0f;  // widthPx
    p.highLightParams[3] = 2.0f;  // featherPx
    p.highLightParams[4] = 0.5f;  // shiftPx
    p.highLightParams[5] = 1.0f;  // dir.x
    p.highLightParams[6] = 0.0f;  // dir.y
    p.hlParams[0] = 1.0f;
    p.hlParams[1] = 0.0f;
    p.hlParams[2] = 1.0f;

    // BLUR (largeRadius = blurParams[0], smallRadius = blurParams[0]/blurParams[1])
    p.blurParams[0] = 16.0f; // Radius
    p.blurParams[1] = 4.0f;  // k (avoid 0 to prevent division by zero)

    return p;
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
    bmp.Build(1, 0, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto imgH0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgH0, src_, dst_), nullptr);

    // Case 3: width == 0, height > 0
    bmp.Build(0, 1, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLACK);
    auto imgW0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgW0, src_, dst_), nullptr);

    // Case 4: width == 0, height == 0
    bmp.Build(0, 0, fmt);
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
    bmpImg.Build(2, 2, fmt);
    bmpImg.ClearWithColor(Drawing::Color::COLOR_WHITE);
    Drawing::Bitmap bmpBig;
    bmpBig.Build(2, 2, fmt);
    bmpBig.ClearWithColor(Drawing::Color::COLOR_GREEN);
    Drawing::Bitmap bmpSml;
    bmpSml.Build(2, 2, fmt);
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
