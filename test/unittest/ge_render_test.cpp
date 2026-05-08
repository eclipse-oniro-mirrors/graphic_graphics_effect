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

#include "ge_render.h"
#include "ge_visual_effect_impl.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render_context/render_context.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "effect/ge_shader_filter_params_namesonly.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GERenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);
    std::shared_ptr<Drawing::Image> MakeImage();

    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};

private:
    std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
};

std::shared_ptr<Drawing::Surface> GERenderTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GERenderTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

void GERenderTest::SetUpTestCase(void) {}
void GERenderTest::TearDownTestCase(void) {}

void GERenderTest::SetUp()
{
    Drawing::Rect rect { 0.0f, 0.0f, 10.0f, 10.0f };
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo { rect.GetWidth(), rect.GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    surface_ = CreateSurface();
    ASSERT_NE(surface_, nullptr);
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface_.get());
    ASSERT_NE(canvas_, nullptr);
    HpsEffectFilter::UnitTestSetExtensionProperties({}); // reset hps extensions to prevent side effects
}

void GERenderTest::TearDown() {}

std::shared_ptr<Drawing::RuntimeEffect> GERenderTest::MakeGreyAdjustmentEffect()
{
    static const std::string GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        vec4 main(vec2 drawing_coord) {
            vec3 color = vec3(imageShader(drawing_coord).r, imageShader(drawing_coord).g, imageShader(drawing_coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");
    if (!greyAdjustEffect_) {
        std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect =
            Drawing::RuntimeEffect::CreateForShader(GreyGradationString);
        if (!greyAdjustEffect) {
            return nullptr;
        }
        greyAdjustEffect_ = std::move(greyAdjustEffect);
    }

    return greyAdjustEffect_;
}

std::shared_ptr<Drawing::Image> GERenderTest::MakeImage(Drawing::Canvas& canvas)
{
    auto image = std::make_shared<Drawing::Image>();
    if (image == nullptr) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage image is null";
        return nullptr;
    }
    float greyX = 0.0f;
    float greyY = 1.0f;

    auto greyAdjustEffect = MakeGreyAdjustmentEffect();
    if (!greyAdjustEffect) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage greyAdjustEffect is null";
        return nullptr;
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(greyAdjustEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("coefficient1", greyX);
    builder->SetUniform("coefficient2", greyY);
    return builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
}

std::shared_ptr<Drawing::Image> GERenderTest::MakeImage()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

/**
 * @tc.name: DrawImageEffect_001
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    const std::shared_ptr<Drawing::Image> image = nullptr;
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_001 end";
}

/**
 * @tc.name: DrawImageEffect_002
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_002 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 0);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_002 end";
}

/**
 * @tc.name: DrawImageEffect_003
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_003 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_003 end";
}

/**
 * @tc.name: ApplyImageEffect_001
 * @tc.desc: Verify the ApplyImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    const std::shared_ptr<Drawing::Image> image = nullptr;
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    auto outImage = geRender->ApplyImageEffect(*canvas_, *veContainer, {image, src, dst}, sampling);
    EXPECT_TRUE(outImage == image);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_001 end";
}

/**
 * @tc.name: GenerateShaderFilters_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_AI_BAR);
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_LOW, 1.0f); // 1.0 AI bar blur low
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_HIGH, 1.0f); // 1.0 AI bar blur high
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_THRESHOLD, 1.0f); // 1.0 AI bar blur threshold
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_OPACITY, 1.0f); // 1.0 AI bar blur opacity
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_SATURATION, 1.0f); // 1.0 AI bar blur saturation
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_001 end";
}

/**
 * @tc.name: GenerateShaderFilters_002
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_002 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 1.0f); // 1.0 grey blur coff
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 1.0f); // 1.0 grey blur coff
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_002 end";
}

/**
 * @tc.name: GenerateShaderFilters_003
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_003 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 1); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_003 end";
}

/**
 * @tc.name: GenerateShaderFilters_004
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_004 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("");
    visualEffect->SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 1); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_004 end";
}

/**
 * @tc.name: GenerateShaderFilters_008
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_008 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("");
    std::pair<float, float> factor = {1.0f, 1.0f};
    visualEffect->SetParam(Drawing::GE_FILTER_DISPLACEMENT_DISTORT_FACTOR, factor); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_008 end";
}

/**
 * @tc.name: GenerateShaderFiltersMESA_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersMESA_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersMESA_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 1); // 1 blur directon
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, 1.0f); // 1.0 grey blur coff
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, 1.0f); // 1.0 grey blur coff
    // 0, 0.0: tileMode and stretch params
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.0f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersMESA_001 end";
}

/**
 * @tc.name: GenerateShaderFilters_005
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_005 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_WATER_RIPPLE);
    visualEffect->SetParam("PROGRESS", 0.5f);
    visualEffect->SetParam("WAVE_NUM", 1.0f);
    visualEffect->SetParam("RIPPLE_CENTER_X", 0.5f);
    visualEffect->SetParam("RIPPLE_CENTER_Y", 0.5f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_005 end";
}

/**
 * @tc.name: GenerateShaderFilters_006
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_006 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MAGNIFIER);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_006 end";
}

/**
 * @tc.name: GenerateShaderFilters_007
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_007 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_COLOR_GRADIENT);
    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<float> positions = { 1.0f, 1.0f }; // 1.0, 1.0 is positions of xy params
    std::vector<float> strengths = { 0.5f }; // 0.5 is strength params
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_COLOR, colors);
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_POSITION, positions);
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_STRENGTH, strengths);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_007 end";
}

/**
 * @tc.name: GenerateShaderFilters_009
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_009 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_BEZIER_WARP);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_009 end";
}

/**
 * @tc.name: GenerateShaderFilters_010
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_010 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_CONTENT_LIGHT);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_010 end";
}

/**
 * @tc.name: GenerateShaderFilters_011
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_011 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY_SCALE_TWIST);
    std::pair<float, float> scale = {1.0f, 1.0f};
    std::shared_ptr<Drawing::Image> sourceImage = nullptr;
    std::shared_ptr<Drawing::Image> maskImage = nullptr;
    float progress = 0.f;
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_SCALE_TWIST_PROGRESS, progress);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_SCALE_TWIST_SOURCEIMAGE, sourceImage);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_SCALE_TWIST_MASK, maskImage);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_SCALE_TWIST_SCALE, scale);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_011 end";
}

/**
 * @tc.name: GenerateShaderFilters_012
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_012 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY_BLUR);
    std::shared_ptr<Drawing::Image> sourceImage = nullptr;
    std::shared_ptr<Drawing::Image> maskImage = nullptr;
    float progress = 0.f;
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_BLUR_PROGRESS, progress);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_BLUR_SOURCEIMAGE, sourceImage);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_BLUR_MASK, maskImage);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_012 end";
}

/**
 * @tc.name: GenerateShaderFilters_013
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_013 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY);
    std::shared_ptr<Drawing::Image> sourceImage = nullptr;
    std::shared_ptr<Drawing::Image> maskImage = nullptr;
    float progress = 0.f;
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_PROGRESS, progress);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_SOURCEIMAGE, sourceImage);
    visualEffect->SetParam(Drawing::GE_FILTER_GASIFY_MASK, maskImage);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_013 end";
}

/**
 * @tc.name: GenerateShaderFilters_014
 * @tc.desc: Verify GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_014 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_PARTICLE_ABLATION);
    float progress = 0.5f;
    float ablationRate = 0.3f;
    std::pair<float, float> centers0 = {0.25f, 0.25f};
    std::pair<float, float> centers1 = {0.75f, 0.25f};
    std::pair<float, float> centers2 = {0.25f, 0.75f};
    std::pair<float, float> centers3 = {0.75f, 0.75f};
    float glowLevel = 2.0f;
    float glowBrightness = 1.5f;
    int32_t maxParticleCount = 1000;
    std::pair<float, float> wind = {0.1f, 0.05f};
    int32_t turbScale = 4;
    int32_t turbEvo = 2;
    int32_t turbAmp = 1;
    std::pair<float, float> expansionSize = {1.0f, 1.0f};
    
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_PROGRESS, progress);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_ABLATION_RATE, ablationRate);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_CENTERS0, centers0);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_CENTERS1, centers1);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_CENTERS2, centers2);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_CENTERS3, centers3);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_GLOW_LEVEL, glowLevel);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_GLOW_BRIGHTNESS, glowBrightness);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_MAX_PARTICLE_COUNT, maxParticleCount);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_WIND, wind);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_TURB_SCALE, turbScale);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_TURB_EVO, turbEvo);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_TURB_AMP, turbAmp);
    visualEffect->SetParam(Drawing::GE_FILTER_PARTICLE_ABLATION_EXPANSION_SIZE, expansionSize);
    
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_014 end";
}

/**
 * @tc.name: GenerateShaderFiltersEdgelight_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersEdgelight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersEdgelight_001 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);
    visualEffect->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 1.0);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersEdgelight_001 end";
}

/**
 * @tc.name: GenerateShaderFiltersDispersion_001
 * @tc.desc: Verify the GenerateShaderFiltersDispersion
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersDispersion_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersDispersion_001 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_DISPERSION);
    visualEffect->SetParam(Drawing::GE_FILTER_DISPERSION_OPACITY, 0.5f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersDispersion_001 end";
}

/**
 * @tc.name: ApplyHpsImageEffect_001
 * @tc.desc: Verify the ApplyHpsImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsImageEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsImageEffect_001 start";

    Drawing::GEVisualEffectContainer veContainer;
    const std::shared_ptr<Drawing::Image> image = nullptr;
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    float alpha = 1.0f;
    std::shared_ptr<Drawing::ColorFilter> colorFilter;
    uint32_t maskColor = 255;
    float saturationForHPS = 1.0f;
    float brightnessForHPS = 1.0f;
    GERender::HpsGEImageEffectContext context = {image, src, dst, Drawing::SamplingOptions(), alpha,
        colorFilter, maskColor, saturationForHPS, brightnessForHPS};

    auto geRender = std::make_shared<GERender>();

    /* image is nullptr*/
    geRender->ApplyHpsImageEffect(*canvas_, veContainer, context, outImage);

    /* no filter*/
    auto image2 = MakeImage();
    context.image = image2;
    geRender->ApplyHpsImageEffect(*canvas_, veContainer, context, outImage);

    /* normal case */
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);
    visualEffect->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 1.0);
    veContainer.AddToChainedFilter(visualEffect);
    EXPECT_EQ(geRender->ApplyHpsImageEffect(*canvas_, veContainer, context, outImage), false);

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsImageEffect_001 end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_NullImage
 * @tc.desc: Verify ApplyHpsGEImageEffect handles null image input
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_NullImage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NullImage start";

    Drawing::GEVisualEffectContainer veContainer;
    auto context = GERender::HpsGEImageEffectContext { nullptr, // image is null
        Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::SamplingOptions() };
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();
    auto result = geRender->ApplyHpsGEImageEffect(*canvas_, veContainer, context, outImage, brush);

    EXPECT_FALSE(result.isHpsBlurApplied);
    EXPECT_FALSE(result.hasDrawnOnCanvas);
    EXPECT_EQ(outImage, nullptr);

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NullImage end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_NoFilters
 * @tc.desc: Verify ApplyHpsGEImageEffect handles empty filter container
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_NoFilters, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NoFilters start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    auto context = GERender::HpsGEImageEffectContext { image, Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f),
        Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::SamplingOptions() };
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();
    auto result = geRender->ApplyHpsGEImageEffect(*canvas_, veContainer, context, outImage, brush);

    EXPECT_FALSE(result.isHpsBlurApplied);
    EXPECT_FALSE(result.hasDrawnOnCanvas);
    EXPECT_EQ(outImage, nullptr); // No Filters will return when no effect can be composed

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NoFilters end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_NormalCase
 * @tc.desc: Verify ApplyHpsGEImageEffect with valid edge light filter
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_NormalCase, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NormalCase start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 0.1f);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 0.1f);

    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);

    auto context = GERender::HpsGEImageEffectContext { image, Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f),
        Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::SamplingOptions() };
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();
    auto result = geRender->ApplyHpsGEImageEffect(*canvas_, veContainer, context, outImage, brush);

    EXPECT_FALSE(result.isHpsBlurApplied);
    EXPECT_FALSE(result.hasDrawnOnCanvas);
    // Currently, grey can only be consumed by hps build pass. If hps is not supported, it will be null.
    auto hpsEffectFilter = std::make_shared<HpsEffectFilter>(*canvas_);
    if (hpsEffectFilter->IsEffectSupported(visualEffect)) {
        EXPECT_NE(outImage, nullptr);
    } else {
        EXPECT_EQ(outImage, nullptr);
    }

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_NormalCase end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_DisabledHpsGrey
 * @tc.desc: Verify ApplyHpsGEImageEffect with disabled hps and grey effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_DisabledHpsGrey, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_DisabledHpsGrey start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 0.1f);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 0.1f);
    HpsEffectFilter::UnitTestSetExtensionProperties({ "" }); // disable hps features to test fallback

    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);

    auto context = GERender::HpsGEImageEffectContext { image, Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f),
        Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::SamplingOptions() };
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();
    auto result = geRender->ApplyHpsGEImageEffect(*canvas_, veContainer, context, outImage, brush);

    EXPECT_FALSE(result.isHpsBlurApplied);
    EXPECT_FALSE(result.hasDrawnOnCanvas);
    EXPECT_EQ(outImage, nullptr); // Currently, when HPS is disabled, no optimization can be applied for single grey

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_DisabledHpsGrey end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_DirectDrawOnCanvas
 * @tc.desc: Verify ApplyHpsGEImageEffect with direct draw
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_DirectDrawOnCanvas, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_DirectDrawOnCanvas start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_FROSTED_GLASS);
    Drawing::GESDFRRectShapeParams shapeParam;
    shapeParam.rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    shapeParam.rrect.SetCornerRadius(10.0f, 10.0f); // from GESDFRRectShaderShapeTest
    auto shape = std::make_shared<Drawing::GESDFRRectShaderShape>(shapeParam);
    visualEffect->SetParam(
        Drawing::GE_FILTER_FROSTED_GLASS_SHAPE, std::static_pointer_cast<Drawing::GESDFShaderShape>(shape));

    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);

    auto context = GERender::HpsGEImageEffectContext { image, Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f),
        Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f), Drawing::SamplingOptions() };
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();
    auto result = geRender->ApplyHpsGEImageEffect(*canvas_, veContainer, context, outImage, brush);

    EXPECT_FALSE(result.isHpsBlurApplied);
    EXPECT_TRUE(result.hasDrawnOnCanvas);
    EXPECT_NE(outImage, nullptr); // As long as the apply loop is entered, outImage will be assigned at least once

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_DirectDrawOnCanvas end";
}

/**
 * @tc.name: GenerateShaderEffectTest_LightCave
 * @tc.desc: Verify the GenerateShaderEffect with LightCave
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderEffectTest_LightCave, TestSize.Level1)
{
    auto geVisualEffectImpl = std::make_shared<Drawing::GEVisualEffectImpl>("");
    geVisualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LIGHT_CAVE);
    auto geRender = std::make_shared<GERender>();
    auto shader = geRender->GenerateShaderEffect(geVisualEffectImpl);
    EXPECT_EQ(shader, nullptr);

    geVisualEffectImpl->MakeLightCaveParams();
    shader = geRender->GenerateShaderEffect(geVisualEffectImpl);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: IsFrostedGlassFilter_ValidAndInvalidCases
 * @tc.desc: Verify function IsFrostedGlassFilter with frosted glass filter, other filters, and empty container
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, IsFrostedGlassFilter_ValidAndInvalidCases, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest IsFrostedGlassFilter_ValidAndInvalidCases start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);

    Drawing::GEVisualEffectContainer veContainer1;
    auto visualEffect1 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_FROSTED_GLASS);
    veContainer1.AddToChainedFilter(visualEffect1);
    EXPECT_TRUE(geRender->IsFrostedGlassFilter(veContainer1));

    Drawing::GEVisualEffectContainer veContainer2;
    auto visualEffect2 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    veContainer2.AddToChainedFilter(visualEffect2);
    EXPECT_FALSE(geRender->IsFrostedGlassFilter(veContainer2));

    Drawing::GEVisualEffectContainer veContainer3;
    EXPECT_FALSE(geRender->IsFrostedGlassFilter(veContainer3));

    GTEST_LOG_(INFO) << "GERenderTest IsFrostedGlassFilter_ValidAndInvalidCases end";
}

/**
 * @tc.name: SetMesablurAllEnabledByCCM_EnableAndDisable
 * @tc.desc: Verify function SetMesablurAllEnabledByCCM with enable and disable flags
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, SetMesablurAllEnabledByCCM_EnableAndDisable, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest SetMesablurAllEnabledByCCM_EnableAndDisable start";

    GERender::SetMesablurAllEnabledByCCM(true);
    GERender::SetMesablurAllEnabledByCCM(false);

    GTEST_LOG_(INFO) << "GERenderTest SetMesablurAllEnabledByCCM_EnableAndDisable end";
}

/**
 * @tc.name: DrawImageEffect_KawaseBlurSmallRadius
 * @tc.desc: Verify DrawImageEffect with Kawase blur effect with small radius
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_KawaseBlurSmallRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_KawaseBlurSmallRadius start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_KawaseBlurSmallRadius end";
}

/**
 * @tc.name: DrawImageEffect_MesaBlurWithGreyCoefs
 * @tc.desc: Verify DrawImageEffect with MESA blur effect with grey coefficients
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_MesaBlurWithGreyCoefs, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_MesaBlurWithGreyCoefs start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 10);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, 0.0f);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_MesaBlurWithGreyCoefs end";
}

/**
 * @tc.name: DrawImageEffect_LinearGradientBlur
 * @tc.desc: Verify DrawImageEffect with linear gradient blur effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_LinearGradientBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_LinearGradientBlur start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_LinearGradientBlur end";
}

/**
 * @tc.name: DrawImageEffect_EdgeLight
 * @tc.desc: Verify DrawImageEffect with edge light effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_EdgeLight, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_EdgeLight start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_EdgeLight end";
}

/**
 * @tc.name: DrawImageEffect_Dispersion
 * @tc.desc: Verify DrawImageEffect with dispersion effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_Dispersion, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_Dispersion start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_DISPERSION);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_Dispersion end";
}

/**
 * @tc.name: DrawImageEffect_GasifyScaleTwist
 * @tc.desc: Verify DrawImageEffect with gasify scale twist effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_GasifyScaleTwist, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_GasifyScaleTwist start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY_SCALE_TWIST);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_GasifyScaleTwist end";
}

/**
 * @tc.name: DrawImageEffect_GasifyBlur
 * @tc.desc: Verify DrawImageEffect with gasify blur effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_GasifyBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_GasifyBlur start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY_BLUR);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_GasifyBlur end";
}

/**
 * @tc.name: DrawImageEffect_Gasify
 * @tc.desc: Verify DrawImageEffect with gasify effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_Gasify, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_Gasify start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GASIFY);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_Gasify end";
}

/**
 * @tc.name: DrawImageEffect_VariableRadiusBlur
 * @tc.desc: Verify DrawImageEffect with variable radius blur effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_VariableRadiusBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_VariableRadiusBlur start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_VARIABLE_RADIUS_BLUR);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_VariableRadiusBlur end";
}

/**
 * @tc.name: DrawImageEffect_KawaseBlurMediumRadius
 * @tc.desc: Verify DrawImageEffect with Kawase blur effect with medium radius
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_KawaseBlurMediumRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_KawaseBlurMediumRadius start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 5);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_KawaseBlurMediumRadius end";
}

/**
 * @tc.name: DrawImageEffect_FrostedGlass
 * @tc.desc: Verify DrawImageEffect with frosted glass effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_FrostedGlass, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_FrostedGlass start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_FROSTED_GLASS);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_FrostedGlass end";
}

/**
 * @tc.name: DrawImageEffect_FrostedGlassBlur
 * @tc.desc: Verify DrawImageEffect with frosted glass blur effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_FrostedGlassBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_FrostedGlassBlur start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_FROSTED_GLASS_BLUR);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_FrostedGlassBlur end";
}

/**
 * @tc.name: DrawImageEffect_ValidFilterType
 * @tc.desc: Verify DrawImageEffect handles valid filter type gracefully
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_ValidFilterType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_ValidFilterType start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_ValidFilterType end";
}

/**
 * @tc.name: DrawShaderEffect_EmptyContainer
 * @tc.desc: Verify function DrawShaderEffect with empty visual effect container
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_EmptyContainer start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);

    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_EmptyContainer end";
}

/**
 * @tc.name: DrawShaderEffect_AuroraNoiseShader
 * @tc.desc: Verify function DrawShaderEffect with Aurora noise shader visual effect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_AuroraNoiseShader, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_AuroraNoiseShader start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_SHADER_AURORA_NOISE);
    visualEffect->SetParam(Drawing::GE_SHADER_AURORA_NOISE_VALUE, 0.5f);
    visualEffect->SetParam(Drawing::GE_SHADER_AURORA_FREQX_VALUE, 10.0f);
    visualEffect->SetParam(Drawing::GE_SHADER_AURORA_FREQY_VALUE, 10.0f);
    veContainer.AddToChainedFilter(visualEffect);

    Drawing::Rect bounds(0.0f, 0.0f, 100.0f, 100.0f);
    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_AuroraNoiseShader end";
}

/**
 * @tc.name: DrawShaderEffect_WithEmptyFilterList
 * @tc.desc: Verify function DrawShaderEffect with container but empty filter list
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_WithEmptyFilterList, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_WithEmptyFilterList start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    Drawing::Rect bounds(0.0f, 0.0f, 100.0f, 100.0f);

    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_WithEmptyFilterList end";
}

/**
 * @tc.name: IsNeedExpansionFilter_DefaultState
 * @tc.desc: Verify function IsNeedExpansionFilter returns default state
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, IsNeedExpansionFilter_DefaultState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest IsNeedExpansionFilter_DefaultState start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    EXPECT_FALSE(geRender->IsNeedExpansionFilter());

    GTEST_LOG_(INFO) << "GERenderTest IsNeedExpansionFilter_DefaultState end";
}

/**
 * @tc.name: GetExpansionRect_DefaultRect
 * @tc.desc: Verify function GetExpansionRect returns default rect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GetExpansionRect_DefaultRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GetExpansionRect_DefaultRect start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    auto rect = geRender->GetExpansionRect();
    EXPECT_FLOAT_EQ(rect.GetLeft(), 0.0f);
    EXPECT_FLOAT_EQ(rect.GetTop(), 0.0f);
    EXPECT_FLOAT_EQ(rect.GetRight(), 0.0f);
    EXPECT_FLOAT_EQ(rect.GetBottom(), 0.0f);

    GTEST_LOG_(INFO) << "GERenderTest GetExpansionRect_DefaultRect end";
}

/**
 * @tc.name: ApplyImageEffect_EmptyContainer
 * @tc.desc: Verify ApplyImageEffect with empty visual effect container
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_EmptyContainer start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    Drawing::GEVisualEffectContainer veContainer;
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    auto outImage = geRender->ApplyImageEffect(*canvas_, veContainer, {image, src, dst}, sampling);
    EXPECT_EQ(outImage, nullptr);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_EmptyContainer end";
}

/**
 * @tc.name: ApplyImageEffect_MultipleFiltersChain
 * @tc.desc: Verify ApplyImageEffect with multiple filters in chain
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect_MultipleFiltersChain, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_MultipleFiltersChain start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    Drawing::GEVisualEffectContainer veContainer;
    
    auto visualEffect1 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect1->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 2);
    veContainer.AddToChainedFilter(visualEffect1);
    
    auto visualEffect2 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect2->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 0.5f);
    visualEffect2->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 0.5f);
    veContainer.AddToChainedFilter(visualEffect2);
    
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    auto outImage = geRender->ApplyImageEffect(*canvas_, veContainer, {image, src, dst}, sampling);
    EXPECT_NE(outImage, nullptr);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_MultipleFiltersChain end";
}

/**
 * @tc.name: ApplyImageEffect_WithCacheProvider
 * @tc.desc: Verify ApplyImageEffect with cache provider
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect_WithCacheProvider, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_WithCacheProvider start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    Drawing::GEVisualEffectContainer veContainer;
    
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    veContainer.AddToChainedFilter(visualEffect);
    
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    IGECacheProvider* cacheProvider = nullptr;
    auto outImage = geRender->ApplyImageEffect(*canvas_, veContainer, {image, src, dst, cacheProvider}, sampling);
    EXPECT_NE(outImage, nullptr);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_WithCacheProvider end";
}

/**
 * @tc.name: DrawImageEffect_EmptyContainer
 * @tc.desc: Verify DrawImageEffect with empty visual effect container
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_EmptyContainer start";

    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    Drawing::GEVisualEffectContainer veContainer;
    const Drawing::Rect src(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    geRender->DrawImageEffect(*canvas_, veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_EmptyContainer end";
}

/**
 * @tc.name: DrawImageEffect_InvalidRectNegativeCoords
 * @tc.desc: Verify DrawImageEffect handles invalid rect with negative coordinates
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_InvalidRectNegativeCoords, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_InvalidRectNegativeCoords start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    
    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    const Drawing::Rect src(-1.0f, -1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(-1.0f, -1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_InvalidRectNegativeCoords end";
}

/**
 * @tc.name: DrawImageEffect_InvalidRectZeroSize
 * @tc.desc: Verify DrawImageEffect handles invalid rect with zero size
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_InvalidRectZeroSize, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_InvalidRectZeroSize start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    
    auto image = MakeImage();
    ASSERT_NE(image, nullptr);
    
    const Drawing::Rect src(0.0f, 0.0f, 0.0f, 0.0f);
    const Drawing::Rect dst(0.0f, 0.0f, 0.0f, 0.0f);
    const Drawing::SamplingOptions sampling;
    
    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    
    geRender->DrawImageEffect(*canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_InvalidRectZeroSize end";
}

/**
 * @tc.name: DrawShaderEffect_InvalidShaderType
 * @tc.desc: Verify DrawShaderEffect handles invalid shader type gracefully
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_InvalidShaderType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidShaderType start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("");
    veContainer.AddToChainedFilter(visualEffect);

    Drawing::Rect bounds(0.0f, 0.0f, 100.0f, 100.0f);
    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidShaderType end";
}

/**
 * @tc.name: DrawShaderEffect_InvalidBoundsNegativeCoords
 * @tc.desc: Verify DrawShaderEffect handles invalid bounds with negative coordinates
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_InvalidBoundsNegativeCoords, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidBoundsNegativeCoords start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    Drawing::Rect bounds(-10.0f, -10.0f, 10.0f, 10.0f);
    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidBoundsNegativeCoords end";
}

/**
 * @tc.name: DrawShaderEffect_InvalidBoundsZeroSize
 * @tc.desc: Verify DrawShaderEffect handles invalid bounds with zero size
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawShaderEffect_InvalidBoundsZeroSize, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidBoundsZeroSize start";

    auto geRender = std::make_shared<GERender>();
    ASSERT_NE(geRender, nullptr);
    ASSERT_NE(canvas_, nullptr);

    Drawing::GEVisualEffectContainer veContainer;
    Drawing::Rect bounds(0.0f, 0.0f, 0.0f, 0.0f);
    geRender->DrawShaderEffect(*canvas_, veContainer, bounds);

    GTEST_LOG_(INFO) << "GERenderTest DrawShaderEffect_InvalidBoundsZeroSize end";
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
