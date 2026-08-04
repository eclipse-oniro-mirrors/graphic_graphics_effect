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

#include "draw/canvas.h"
#include "draw/color.h"
#include "ge_frosted_glass_shader_filter.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "image/bitmap.h"

#include "render_context/render_context.h"

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
    static inline Drawing::Canvas canvas_; // CPU canvas (no GPU context)
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> gpuCanvas_ = nullptr;

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

    surface_ = CreateSurface();
    if (surface_ != nullptr) {
        gpuCanvas_ = surface_->GetCanvas();
    }
}

void GEFrostedGlassShaderFilterTest::TearDown()
{
    image_ = nullptr;
    gpuCanvas_ = nullptr;
    surface_ = nullptr;
}

std::shared_ptr<Drawing::Surface> GEFrostedGlassShaderFilterTest::CreateSurface()
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo imageInfo { NUM_50, NUM_50, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo);
}

Drawing::GEFrostedGlassShaderFilterParams GEFrostedGlassShaderFilterTest::MakeParams()
{
    Drawing::GEFrostedGlassShaderFilterParams p; // Default parameters
    p.weightsEmboss = { 1.0f, 1.0f };
    p.weightsEdl = { 1.0f, 1.0f };
    p.refractParams = { 0.5f, 0.5f, 1.0f };
    p.bgRates = { -0.00003f, 1.2f };
    p.bgKBS = { 0.010834f, 0.007349f, 1.2f };
    p.bgPos = { 0.3f, 0.5f, 1.0f };
    p.bgNeg = { 0.5f, 0.5f, 1.0f };

    p.sdParams = { 0.0f, 2.0f, 0.0f };
    p.sdRates = { 0.0f, 0.0f };
    p.sdKBS = { -0.02f, 2.0f, 4.62f };
    p.sdPos = { 1.0f, 1.5f, 2.0f };
    p.sdNeg = { 1.7f, 3.0f, 1.0f };

    p.envLightParams = { 0.8f, 0.2745f, 2.0f };
    p.envLightRates = { 0.0f, 0.0f };
    p.envLightKBS = { 0.8f, 0.2745f, 2.0f };
    p.envLightPos = { 1.0f, 1.5f, 2.0f };
    p.envLightNeg = { 1.7f, 3.0f, 1.0f };

    p.edLightParams = { 2.0f, -1.0f };
    p.edLightAngles = { 30.0f, 30.0f };
    p.edLightDir = { -1.0f, 1.0f };
    p.edLightRates = { 0.0f, 0.0f };
    p.edLightKBS = { 0.6027f, 0.64f, 2.0f };
    p.edLightPos = { 1.0f, 1.5f, 2.0f };
    p.edLightNeg = { 1.7f, 3.0f, 1.0f };
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
 * @tc.desc: Without an SDF shape, PrepareDrawing fails and OnProcessImage falls back to
 *           GetOriginImage, which builds a new subset image (or returns nullptr when the
 *           canvas has no GPU context). Either way the filter must not echo the input image.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_CPUFallback, TestSize.Level0)
{
    auto params = MakeParams(); // no sdfShape set -> PrepareDrawing fails
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    ASSERT_NE(image_, nullptr);
    auto out = filter->OnProcessImage(canvas_, image_, src_, dst_);
    // GetOriginImage never returns the input image: it returns nullptr (no GPU context)
    // or a newly built subset image. Verify the fallback path did not echo the input.
    EXPECT_NE(out.get(), image_.get());
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

    // Create a tiny image to use as the image child shader
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

    Drawing::Bitmap bmpImg;
    bmpImg.Build(NUM_2, NUM_2, fmt);
    bmpImg.ClearWithColor(Drawing::Color::COLOR_WHITE);

    auto imgShaderImg = bmpImg.MakeImage();
    ASSERT_NE(imgShaderImg, nullptr);

    // Build child ShaderEffects with identity matrix
    Drawing::Matrix m = canvasInfo_.mat;
    auto childImage = Drawing::ShaderEffect::CreateImageShader(*imgShaderImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    Drawing::GESDFRRectShapeParams shapeParam;
    shapeParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    shapeParam.rrect.SetCornerRadius(10.0f, 10.0f);
    auto shape = std::make_shared<Drawing::GESDFRRectShaderShape>(shapeParam);
    filter.frostedGlassParams_.sdfShape = shape;
    auto sdfNormalShader = filter.MakeSDFNormalShader(Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f));
    ASSERT_NE(childImage, nullptr);
    ASSERT_NE(sdfNormalShader, nullptr);

    auto builder = filter.MakeFrostedGlassShader(childImage, sdfNormalShader);
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: MakeSDFNormalShader_WO_SDFShape
 * @tc.desc: Build a RuntimeShaderBuilder without sdfShape.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeSDFNormalShader_WO_SDFShape, TestSize.Level0)
{
    auto params = MakeParams();
    GEFrostedGlassShaderFilter filter(params);
    ASSERT_TRUE(filter.InitFrostedGlassEffect());
    auto sdfNormalShader = filter.MakeSDFNormalShader(Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f));
    EXPECT_EQ(sdfNormalShader, nullptr);
}

/**
 * @tc.name: MakeSDFNormalShader_W_SDFShape
 * @tc.desc: Build a RuntimeShaderBuilder with sdfShape.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, MakeSDFNormalShader_W_SDFShape, TestSize.Level0)
{
    auto params = MakeParams();
    Drawing::GESDFRRectShapeParams sdfParam;
    sdfParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    sdfParam.rrect.SetCornerRadius(10.0f, 10.0f);
    params.sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParam);
    GEFrostedGlassShaderFilter filter(params);
    ASSERT_TRUE(filter.InitFrostedGlassEffect());
    auto sdfNormalShader = filter.MakeSDFNormalShader(Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f));
    EXPECT_NE(sdfNormalShader, nullptr);
}

/**
 * @tc.name: Type_Identity
 * @tc.desc: Verify Type() returns FROSTED_GLASS and TypeName() returns the registered name.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, Type_Identity, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::FROSTED_GLASS);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_FROSTED_GLASS);
}

/**
 * @tc.name: Constructor_StoresParams
 * @tc.desc: Verify the constructor copies all params into frostedGlassParams_.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, Constructor_StoresParams, TestSize.Level0)
{
    auto params = MakeParams();
    params.materialColor = { 0.1f, 0.2f, 0.3f, 0.4f };
    params.maxColor = 7.5f;

    GEFrostedGlassShaderFilter filter(params);
    EXPECT_EQ(filter.frostedGlassParams_.materialColor.x_, 0.1f);
    EXPECT_EQ(filter.frostedGlassParams_.materialColor.y_, 0.2f);
    EXPECT_EQ(filter.frostedGlassParams_.materialColor.z_, 0.3f);
    EXPECT_EQ(filter.frostedGlassParams_.materialColor.w_, 0.4f);
    EXPECT_EQ(filter.frostedGlassParams_.maxColor, 7.5f);
    EXPECT_EQ(filter.frostedGlassParams_.weightsEmboss.x_, 1.0f);
    EXPECT_EQ(filter.frostedGlassParams_.bgKBS.y_, 0.007349f);
}

/**
 * @tc.name: OnDrawImage_NullInput
 * @tc.desc: OnDrawImage returns false when the input image is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnDrawImage_NullInput, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    Drawing::Brush brush;
    EXPECT_FALSE(filter->OnDrawImage(canvas_, nullptr, src_, dst_, brush));
}

/**
 * @tc.name: InterpolateAdaptiveParams_DarkScale
 * @tc.desc: Verify InterpolateAdaptiveParams blends base→dark by darkScale (0, 0.5, 1.0).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, InterpolateAdaptiveParams_DarkScale, TestSize.Level0)
{
    auto params = MakeParams();
    params.darkScale = 0.0f;
    params.darkModeWeightsEmboss = { 5.0f, 6.0f };
    params.darkModeBgRates = { 2.0f, 3.0f };
    params.darkModeBgKBS = { 1.0f, 2.0f, 3.0f };
    auto base = params;
    FrostedGlass::InterpolateAdaptiveParams(params);
    // darkScale=0 → unchanged
    EXPECT_EQ(params.weightsEmboss.x_, base.weightsEmboss.x_);
    EXPECT_EQ(params.bgRates.y_, base.bgRates.y_);
    EXPECT_EQ(params.bgKBS.z_, base.bgKBS.z_);

    // darkScale=1.0 → fully dark
    params = MakeParams();
    params.darkScale = 1.0f;
    params.darkModeWeightsEmboss = { 5.0f, 6.0f };
    params.darkModeBgRates = { 2.0f, 3.0f };
    params.darkModeBgKBS = { 1.0f, 2.0f, 3.0f };
    FrostedGlass::InterpolateAdaptiveParams(params);
    EXPECT_EQ(params.weightsEmboss.x_, 5.0f);
    EXPECT_EQ(params.weightsEmboss.y_, 6.0f);
    EXPECT_EQ(params.bgRates.x_, 2.0f);
    EXPECT_EQ(params.bgKBS.x_, 1.0f);

    // darkScale=0.5 → midpoint
    params = MakeParams();
    params.darkScale = 0.5f;
    params.weightsEmboss = { 0.0f, 0.0f };
    params.darkModeWeightsEmboss = { 10.0f, 20.0f };
    FrostedGlass::InterpolateAdaptiveParams(params);
    EXPECT_EQ(params.weightsEmboss.x_, 5.0f);
    EXPECT_EQ(params.weightsEmboss.y_, 10.0f);
}

/**
 * @tc.name: GetSampleMatrix_SingularCanvasMatrix
 * @tc.desc: GetSampleMatrix returns identity when canvasInfo_.mat is non-invertible.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, GetSampleMatrix_SingularCanvasMatrix, TestSize.Level0)
{
    GEFrostedGlassShaderFilter filter(MakeParams());
    // SetScale(0,0) is singular (det=0) → canvasMatrix.Invert fails → returns identity
    Drawing::Matrix singular;
    singular.SetScale(0.0f, 0.0f);
    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    ci.mat = singular;
    filter.SetShaderFilterCanvasinfo(ci);
    EXPECT_EQ(filter.GetSampleMatrix(), Drawing::Matrix());
}

/**
 * @tc.name: PrepareDrawing_WithSDFShape
 * @tc.desc: PrepareDrawing succeeds when image is valid and SDF shape is set.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, PrepareDrawing_WithSDFShape, TestSize.Level0)
{
    auto params = MakeParams();
    Drawing::GESDFRRectShapeParams sdfParam;
    sdfParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    sdfParam.rrect.SetCornerRadius(10.0f, 10.0f);
    params.sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParam);
    GEFrostedGlassShaderFilter filter(params);
    ASSERT_TRUE(filter.InitFrostedGlassEffect());

    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    filter.SetShaderFilterCanvasinfo(ci);

    Drawing::Matrix matrix;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder;
    ASSERT_NE(image_, nullptr);
    EXPECT_TRUE(filter.PrepareDrawing(image_, matrix, builder));
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: OnProcessImage_WithSDFShape
 * @tc.desc: With SDF shape set, PrepareDrawing succeeds; OnProcessImage reaches builder->MakeImage.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_WithSDFShape, TestSize.Level0)
{
    auto params = MakeParams();
    Drawing::GESDFRRectShapeParams sdfParam;
    sdfParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    sdfParam.rrect.SetCornerRadius(10.0f, 10.0f);
    params.sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParam);
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    ASSERT_TRUE(filter->InitFrostedGlassEffect());

    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    filter->SetShaderFilterCanvasinfo(ci);

    ASSERT_NE(image_, nullptr);
    auto out = filter->OnProcessImage(canvas_, image_, src_, dst_);
    // PrepareDrawing succeeds; MakeImage on CPU likely null → GetOriginImage fallback
    EXPECT_NE(out.get(), image_.get());
}

/**
 * @tc.name: OnDrawImage_WithSDFShape
 * @tc.desc: OnDrawImage with valid image + SDF shape draws and returns true.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnDrawImage_WithSDFShape, TestSize.Level0)
{
    auto params = MakeParams();
    Drawing::GESDFRRectShapeParams sdfParam;
    sdfParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    sdfParam.rrect.SetCornerRadius(10.0f, 10.0f);
    params.sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParam);
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    ASSERT_TRUE(filter->InitFrostedGlassEffect());

    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    filter->SetShaderFilterCanvasinfo(ci);

    ASSERT_NE(image_, nullptr);
    Drawing::Brush brush;
    EXPECT_TRUE(filter->OnDrawImage(canvas_, image_, src_, dst_, brush));
}

/**
 * @tc.name: OnDrawImage_PrepareFails
 * @tc.desc: OnDrawImage without SDF shape returns false (PrepareDrawing fails).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnDrawImage_PrepareFails, TestSize.Level0)
{
    auto params = MakeParams(); // no sdfShape
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);

    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    filter->SetShaderFilterCanvasinfo(ci);

    ASSERT_NE(image_, nullptr);
    Drawing::Brush brush;
    EXPECT_FALSE(filter->OnDrawImage(canvas_, image_, src_, dst_, brush));
}

/**
 * @tc.name: OnProcessImage_GPUFallback
 * @tc.desc: On GPU canvas without SDF shape, GetOriginImage calls BuildSubset, returns a new image.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_GPUFallback, TestSize.Level0)
{
    if (gpuCanvas_ == nullptr) {
        return; // GPU context unavailable in this environment
    }
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(MakeParams());
    ASSERT_NE(image_, nullptr);
    auto out = filter->OnProcessImage(*gpuCanvas_, image_, src_, dst_);
    // PrepareDrawing fails (no SDF) → GetOriginImage with GPU ctx → BuildSubset new image
    EXPECT_NE(out.get(), image_.get());
}

/**
 * @tc.name: OnProcessImage_GPUWithSDFShape
 * @tc.desc: On GPU canvas with SDF shape, PrepareDrawing succeeds and MakeImage produces a new image.
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, OnProcessImage_GPUWithSDFShape, TestSize.Level0)
{
    if (gpuCanvas_ == nullptr) {
        return; // GPU context unavailable in this environment
    }
    Drawing::GESDFRRectShapeParams sdfParam;
    sdfParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    sdfParam.rrect.SetCornerRadius(10.0f, 10.0f);
    auto params = MakeParams();
    params.sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParam);
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(params);
    ASSERT_TRUE(filter->InitFrostedGlassEffect());

    Drawing::CanvasInfo ci;
    ci.geoWidth = 100.0f;
    ci.geoHeight = 100.0f;
    filter->SetShaderFilterCanvasinfo(ci);

    ASSERT_NE(image_, nullptr);
    auto out = filter->OnProcessImage(*gpuCanvas_, image_, src_, dst_);
    // PrepareDrawing succeeds → builder->MakeImage on GPU → new image (not input)
    EXPECT_NE(out.get(), image_.get());
}

/**
 * @tc.name: PrepareDrawing_ZeroDimImage
 * @tc.desc: PrepareDrawing returns false when image has zero dimensions (calls image->GetHeight < 1e-6).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassShaderFilterTest, PrepareDrawing_ZeroDimImage, TestSize.Level0)
{
    auto filter = std::make_unique<GEFrostedGlassShaderFilter>(MakeParams());
    // Default-constructed Image is non-null but has width=0, height=0
    auto zeroImg = std::make_shared<Drawing::Image>();
    Drawing::Matrix matrix;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder;
    EXPECT_FALSE(filter->PrepareDrawing(zeroImg, matrix, builder));
}

} // namespace Rosen
} // namespace OHOS
