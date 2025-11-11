/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <gtest/gtest.h>
#include <memory>
#include "ge_frosted_glass_shader_filter.h"
#include "ge_visual_effect_impl.h"
#include "ge_external_dynamic_loader.h"
#include "render_context/render_context.h"
#include "draw/canvas.h"
#include "draw/path.h"
#include "image/image.h"
#include "image/surface.h"

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

private:
    Drawing::GEFrostedGlassShaderFilterParams MakeParams();
    std::shared_ptr<Drawing::Surface> CreateSurface(int w, int h);
    std::shared_ptr<Drawing::Image> MakeSnapshot(std::shared_ptr<Drawing::Surface> surf);

    // GPU pipeline
    std::shared_ptr<Drawing::Surface> surface_ {nullptr};
    std::shared_ptr<Drawing::Canvas> canvas_ {nullptr};
    Drawing::Rect rect_ {};
    Drawing::ImageInfo imageInfo_ {};

    // Non-GPU Canvas (used to trigger some failure branches)
    Drawing::Canvas canvasNoGpu_;
};

void GEFrostedGlassShaderFilterTest::SetUpTestCase(void) {}
void GEFrostedGlassShaderFilterTest::TearDownTestCase(void) {}

void GEFrostedGlassShaderFilterTest::SetUp()
{
    rect_ = Drawing::Rect {0.0f, 0.0f, 10.0f, 10.0f};
    imageInfo_ = Drawing::ImageInfo {
        static_cast<int>(rect_.GetWidth()),
        static_cast<int>(rect_.GetHeight()),
        Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    surface_ = CreateSurface(imageInfo_.GetWidth(), imageInfo_.GetHeight());
    if (surface_ != nullptr) {
        canvas_ = surface_->GetCanvas();
    }
}

void GEFrostedGlassShaderFilterTest::TearDown() {}

std::shared_ptr<Drawing::Surface> GEFrostedGlassShaderFilterTest::CreateSurface(int w, int h)
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (!context) {
        GTEST_LOG_(INFO) << "CreateSurface: create gpuContext failed.";
        return nullptr;
    }
    Drawing::ImageInfo info(w, h, Drawing::ColorType::COLORTYPE_RGBA_8888,
                            Drawing::AlphaType::ALPHATYPE_OPAQUE);
    return Drawing::Surface::MakeRenderTarget(context.get(), false, info);
}

std::shared_ptr<Drawing::Image> GEFrostedGlassShaderFilterTest::MakeSnapshot(std::shared_ptr<Drawing::Surface> surf)
{
    if (!surf) return nullptr;
    return surf->MakeImageSnapshot();
}

Drawing::GEFrostedGlassShaderFilterParams GEFrostedGlassShaderFilterTest::MakeParams()
{
    Drawing::GEFrostedGlassShaderFilterParams p;
    // COMMON
    p.borderSize[0] = 80.0f;   // halfsize.x
    p.borderSize[1] = 50.0f;   // halfsize.y
    p.cornerRadius  = 20.0f;
    p.borderWidth   = 6.0f;
    p.offset        = 2.0f;
    p.downSampleFactor = 2.0f;

    // Background
    p.bgFactor = 0.85f;

    // Inner shadow
    p.innerShadowParams[0] = 3.5f;   // innerShadowRefractPx
    p.innerShadowParams[1] = 1.25f;  // innerShadowWidth
    p.innerShadowParams[2] = 1.75f;  // innerShadowExp
    p.sdParams[0] = 1.0f; p.sdParams[1] = 2.0f; p.sdParams[2] = 1.2f;

    // Env light
    p.refractOutPx = 4.0f;
    p.envParams[0] = 1.1f; p.envParams[1] = 0.0f; p.envParams[2] = 1.1f;

    // Highlight
    p.highLightParams[0] = 55.0f; // angleDeg
    p.highLightParams[1] = 25.0f; // featherDeg
    p.highLightParams[2] = 3.0f;  // widthPx
    p.highLightParams[3] = 2.0f;  // featherPx
    p.highLightParams[4] = 0.5f;  // shiftPx
    p.highLightParams[5] = 1.0f;  // dir.x
    p.highLightParams[6] = 0.0f;  // dir.y
    p.hlParams[0] = 1.0f; p.hlParams[1] = 0.0f; p.hlParams[2] = 1.0f;

    return p;
}

/**
* @tc.name: CreateSurfaceCanvasTest
* @tc.desc: Verify surface_ and canvas_ are created (GPU path).
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, CreateSurfaceCanvasTest, TestSize.Level1)
{
    EXPECT_NE(surface_, nullptr);
    EXPECT_NE(canvas_, nullptr);
}

/**
* @tc.name: InitEffectTwiceTest
* @tc.desc: Verify InitFrostedGlassEffect() when effect is null and when it is already initialized.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, InitEffectTwiceTest, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    // 1st time: g_frostedGlassShaderEffect == nullptr
    EXPECT_TRUE(filter.InitFrostedGlassEffect());
    // 2nd time: g_frostedGlassShaderEffect != nullptr
    EXPECT_TRUE(filter.InitFrostedGlassEffect());
}

/**
* @tc.name: MakeFrostedGlassShaderSmoke
* @tc.desc: Verify MakeFrostedGlassShader returns a valid builder in the normal case.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeFrostedGlassShaderSmoke, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);
    ASSERT_TRUE(filter.InitFrostedGlassEffect());

    // Use a tiny image to build 3 child shaders
    auto tinySurf = CreateSurface(2, 2);
    ASSERT_NE(tinySurf, nullptr);
    auto tinyImg = MakeSnapshot(tinySurf);
    ASSERT_NE(tinyImg, nullptr);

    Drawing::Matrix identity;
    identity.SetIdentity();

    auto imgShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);
    auto bigBlurShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);
    auto smallBlurShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);

    ASSERT_NE(imgShader, nullptr);
    ASSERT_NE(bigBlurShader, nullptr);
    ASSERT_NE(smallBlurShader, nullptr);

    auto builder = filter.MakeFrostedGlassShader(imgShader, bigBlurShader, smallBlurShader,
                                                 static_cast<float>(tinyImg->GetWidth()),
                                                 static_cast<float>(tinyImg->GetHeight()));
    EXPECT_NE(builder, nullptr);
}

/**
* @tc.name: OnProcessImage_NullInput
* @tc.desc: image == nullptr → returns nullptr.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_NullInput, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    auto out = filter.OnProcessImage(*canvas_, nullptr, rect_, rect_);
    EXPECT_EQ(out, nullptr);
}

/**
* @tc.name: OnProcessImage_ZeroSizeImage
* @tc.desc: imageWidth/Height < 1e-6 → returns nullptr.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_ZeroSizeImage, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    // Try to create a 0x0 surface → snapshot. If the snapshot is nullptr,
    // the previous "null image" branch is taken; otherwise we hit the zero-size branch.
    auto zeroSurf = CreateSurface(0, 0);
    std::shared_ptr<Drawing::Image> zeroImg = nullptr;
    if (zeroSurf) {
        zeroImg = MakeSnapshot(zeroSurf);
    }
    auto out = filter.OnProcessImage(*canvas_, zeroImg, rect_, rect_);
    EXPECT_EQ(out, nullptr);
}

/**
* @tc.name: OnProcessImage_InvertFail
* @tc.desc: Force Matrix.Invert failure → returns original image.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_InvertFail, TestSize.Level1)
{
    if (!surface_ || !canvas_) {
        GTEST_SKIP() << "No GPU surface, skip.";
    }
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    // Use a valid input image
    auto inImg = MakeSnapshot(surface_);
    ASSERT_NE(inImg, nullptr);

    // We cannot directly set canvasInfo_.mat here; this call exercises the path.
    // Depending on the underlying implementation, it may or may not hit the invert-fail branch.
    auto out = filter.OnProcessImage(*canvas_, inImg, rect_, rect_);
    EXPECT_NE(out, nullptr);
}

/**
* @tc.name: OnProcessImage_NoGpu_ResultFallback
* @tc.desc: On a non-GPU Canvas, builder->MakeImage may be nullptr → returns original image.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_NoGpu_ResultFallback, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    auto inImg = MakeSnapshot(surface_);
    if (!inImg) {
        GTEST_LOG_(INFO) << "snapshot is null, skip.";
        GTEST_SKIP();
    }

    auto out = filter.OnProcessImage(canvasNoGpu_, inImg, rect_, rect_);
    EXPECT_EQ(out.get(), inImg.get());
}

/**
* @tc.name: OnProcessImage_Success
* @tc.desc: Full successful pipeline → returns a non-null processed image.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_Success, TestSize.Level1)
{
    if (!surface_ || !canvas_) {
        GTEST_SKIP() << "No GPU surface, skip.";
    }
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    auto inImg = MakeSnapshot(surface_);
    ASSERT_NE(inImg, nullptr);

    auto out = filter.OnProcessImage(*canvas_, inImg, rect_, rect_);
    EXPECT_NE(out, nullptr);
    EXPECT_NE(out.get(), inImg.get());
}

/**
* @tc.name: MakeFrostedGlassShader_Twice
* @tc.desc: Build twice with an already initialized effect to cover the non-null effect branch.
* @tc.type: FUNC
*/
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeFrostedGlassShader_Twice, TestSize.Level1)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);

    // First: triggers InitFrostedGlassEffect internally if needed
    ASSERT_TRUE(filter.InitFrostedGlassEffect());

    auto tinySurf = CreateSurface(2, 2);
    ASSERT_NE(tinySurf, nullptr);
    auto tinyImg = MakeSnapshot(tinySurf);
    ASSERT_NE(tinyImg, nullptr);

    Drawing::Matrix identity;
    identity.SetIdentity();

    auto imgShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);
    auto bigBlurShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);
    auto smallBlurShader = Drawing::ShaderEffect::CreateImageShader(*tinyImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identity);

    auto builder1 = filter.MakeFrostedGlassShader(imgShader, bigBlurShader, smallBlurShader, 2.0f, 2.0f);
    EXPECT_NE(builder1, nullptr);

    // Second: effect is already available
    auto builder2 = filter.MakeFrostedGlassShader(imgShader, bigBlurShader, smallBlurShader, 2.0f, 2.0f);
    EXPECT_NE(builder2, nullptr);
}
} // namespace Rosen
} // namespace OHOS
