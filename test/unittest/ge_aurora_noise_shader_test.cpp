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
#include "ge_aurora_noise_shader.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {

class GEAuroraNoiseShaderTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GEAuroraNoiseShaderTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 100.0f, 100.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    EXPECT_NE(surface_, nullptr);
    canvas_ = surface_->GetCanvas();
    EXPECT_NE(canvas_, nullptr);
}

std::shared_ptr<Drawing::Surface> GEAuroraNoiseShaderTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEAuroraNoiseShaderTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

void GEAuroraNoiseShaderTest::TearDown()
{
    surface_ = nullptr;
    canvas_ = nullptr;
}

/**
 * @tc.name: CreateSurfaceCanvasTest_001
 * @tc.desc: Verify function SetUp in GEAuroraNoiseShaderTest
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, CreateSurfaceCanvasTest_001, TestSize.Level3)
{
    EXPECT_NE(surface_, nullptr);
    EXPECT_NE(canvas_, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderConstructor_001
 * @tc.desc: Verify the constructor of GEAuroraNoiseShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderConstructor_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.0};
    auto shader = GEAuroraNoiseShader(params);
    EXPECT_FLOAT_EQ(shader.auroraNoiseParams_.noise_, params.noise_);
}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: Verify function MakeDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeDrawingShader_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader(params);
    EXPECT_NE(canvas_, nullptr);
    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.verticalBlurImg_, nullptr);
    shader.MakeDrawingShader(rect_, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GetDescription_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.85};
    auto shader = GEAuroraNoiseShader(params);
    std::string expectStr = "GEAuroraNoiseShader";
    EXPECT_EQ(shader.GetDescription(), expectStr);
}

/**
 * @tc.name: SetAuroraNoiseParams_001
 * @tc.desc: Verify function SetAuroraNoiseParams
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, SetAuroraNoiseParams_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{5.0};
    auto shader = GEAuroraNoiseShader(params);
    shader.MakeDrawingShader(rect_, 0.75f);
    Drawing::GEAuroraNoiseShaderParams params1{0.3};
    shader.SetAuroraNoiseParams(params1);
    EXPECT_FLOAT_EQ(shader.auroraNoiseParams_.noise_, params1.noise_);
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify function Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, Preprocess_001, TestSize.Level1)
{
    GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader(params);
    EXPECT_NE(canvas_, nullptr);
    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.noiseImg_, nullptr);
    EXPECT_NE(shader.verticalBlurImg_, nullptr);
}

/**
 * @tc.name: MakeAuroraNoiseShader_001
 * @tc.desc: Verify function MakeAuroraNoiseShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeAuroraNoiseShader_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.2};
    auto shader = GEAuroraNoiseShader(params);
    EXPECT_NE(canvas_, nullptr);
    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.verticalBlurImg_, nullptr);
    auto effect = shader.MakeAuroraNoiseShader(rect_);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: MakeAuroraNoiseShader_002
 * @tc.desc: Verify function MakeAuroraNoiseShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeAuroraNoiseShader_002, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.7};
    auto shader = GEAuroraNoiseShader(params);
    Drawing::ImageInfo image1Info(100, 100, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_OPAQUE);
    EXPECT_NE(canvas_, nullptr);
    auto image1 = shader.MakeAuroraNoiseGeneratorShader(*canvas_, image1Info);
    EXPECT_NE(image1, nullptr);

    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.noiseImg_, nullptr);
    Drawing::ImageInfo image2Info(100, 100, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    auto image2 = shader.MakeAuroraNoiseVerticalBlurShader(*canvas_, image2Info);
    EXPECT_NE(image2, nullptr);

    EXPECT_NE(shader.verticalBlurImg_, nullptr);
    auto effect = shader.MakeAuroraNoiseShader(rect_);
    EXPECT_NE(effect, nullptr);

    auto shader2 = GEAuroraNoiseShader(params);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bmp.Build(100, 100, format); // 100, 100  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    shader2.verticalBlurImg_ = bmp.MakeImage();
    Drawing::Rect rect(0, 0, 0, 0);
    effect = shader2.MakeAuroraNoiseShader(rect);
    EXPECT_EQ(effect, nullptr); // rect is invalid
}

/**
 * @tc.name: MakeAuroraNoiseGeneratorShader_001
 * @tc.desc: Verify function MakeAuroraNoiseGeneratorShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeAuroraNoiseGeneratorShader_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.7};
    auto shader = GEAuroraNoiseShader(params);
    Drawing::ImageInfo imageInfo(50, 50, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_OPAQUE);
    EXPECT_NE(canvas_, nullptr);
    auto image = shader.MakeAuroraNoiseGeneratorShader(*canvas_, imageInfo);
    EXPECT_NE(image, nullptr);
}

/**
 * @tc.name: MakeAuroraNoiseVerticalBlurShader_001
 * @tc.desc: Verify function MakeAuroraNoiseVerticalBlurShader
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeAuroraNoiseVerticalBlurShader_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader(params);
    EXPECT_NE(canvas_, nullptr);
    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.noiseImg_, nullptr);
    Drawing::ImageInfo imageInfo(50, 50, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    auto image = shader.MakeAuroraNoiseVerticalBlurShader(*canvas_, imageInfo);
    EXPECT_NE(image, nullptr);
}

/**
 * @tc.name: MakeAuroraNoiseVerticalBlurShader_002
 * @tc.desc: Verify function MakeAuroraNoiseVerticalBlurShader if noiseImg_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, MakeAuroraNoiseVerticalBlurShader_002, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader(params);
    Drawing::ImageInfo imageInfo(50, 50, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    auto image = shader.MakeAuroraNoiseVerticalBlurShader(*canvas_, imageInfo);
    EXPECT_EQ(image, nullptr); // shader.noiseImg_ is nullptr
}

/**
 * @tc.name: GetAuroraNoiseBuilder_001
 * @tc.desc: Verify function GetAuroraNoiseBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GetAuroraNoiseBuilder_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{1.0};
    auto shader = GEAuroraNoiseShader(params);
    auto builder = shader.GetAuroraNoiseBuilder();
    EXPECT_NE(builder, nullptr);
    builder = shader.GetAuroraNoiseBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: GetAuroraNoiseVerticalBlurBuilder_001
 * @tc.desc: Verify function GetAuroraNoiseVerticalBlurBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GetAuroraNoiseVerticalBlurBuilder_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{1.0};
    auto shader = GEAuroraNoiseShader(params);
    auto builder1 = shader.GetAuroraNoiseVerticalBlurBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader.GetAuroraNoiseVerticalBlurBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GetAuroraNoiseUpSamplingBuilder_001
 * @tc.desc: Verify function GetAuroraNoiseUpSamplingBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GetAuroraNoiseUpSamplingBuilder_001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{1.0};
    auto shader = GEAuroraNoiseShader(params);

    auto builder1 = shader.GetAuroraNoiseUpSamplingBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader.GetAuroraNoiseUpSamplingBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAuroraNoiseShaderTest Type_001 start";

    auto filter = std::make_unique<GEAuroraNoiseShader>();
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::AURORA_NOISE);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_SHADER_AURORA_NOISE);

    GTEST_LOG_(INFO) << "GEAuroraNoiseShaderTest Type_001 end";
}
}  // namespace Rosen
}  // namespace OHOS
