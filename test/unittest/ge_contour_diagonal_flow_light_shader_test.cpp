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
#include "ge_contour_diagonal_flow_light_shader.h"
#include "ge_log.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "utils/rect.h"
#include "draw/color.h"
#include "draw/path.h"
#include "ge_shader_filter_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {

using CacheDataType = struct TestCacheData {
    std::shared_ptr<Drawing::Image> precalculationImg = nullptr;
    uint32_t hash = 0;
    float blurRadius = 0.0f;
};

class GEContourDiagonalFlowLightShaderTest : public Test {
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

    static std::vector<Vector2f> ConvertUVToNDC(const std::vector<Vector2f>& uvPoints, int width, int height)
    {
        if (height < 1 || width < 1 || uvPoints.empty()) {
            return {};
        }
        std::vector<Vector2f> ndcPoints;
        ndcPoints.reserve(uvPoints.size());
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        for (const auto& uv : uvPoints) {
            float ndcX = (uv[0] * 2.0f - 1.0f) * aspect;
            float ndcY = uv[1] * 2.0f - 1.0f;
            ndcPoints.emplace_back(ndcX, ndcY);
        }
        return ndcPoints;
    }

    static void ConvertPointsTo(const std::vector<Vector2f>& in, std::vector<float>& out)
    {
        out.clear();
        for (const auto& p : in) {
            out.push_back(p[0]);
            out.push_back(p[1]);
        }
    }
};

void GEContourDiagonalFlowLightShaderTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 100.0f, 100.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    canvas_ = surface_->GetCanvas();
}

std::shared_ptr<Drawing::Surface> GEContourDiagonalFlowLightShaderTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEContourDiagonalFlowLightShaderTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

void GEContourDiagonalFlowLightShaderTest::TearDown()
{
    surface_ = nullptr;
    canvas_ = nullptr;
}

/**
 * @tc.name: CreateSurfaceCanvasTest_001
 * @tc.desc: Verify function SetUp in GEContourDiagonalFlowLightShaderTest
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, CreateSurfaceCanvasTest_001, TestSize.Level3)
{
    EXPECT_NE(surface_, nullptr);
    EXPECT_NE(canvas_, nullptr);
}

/**
 * @tc.name: GEContourDiagonalFlowLightShaderConstructor_001
 * @tc.desc: Verify the constructor of GEContourDiagonalFlowLightShader
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, GEContourDiagonalFlowLightShaderConstructor_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams testParams;
    testParams.line1Start_ = 0.2f;
    testParams.line1Length_ = 0.3f;
    testParams.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    testParams.line2Start_ = 0.7f;
    testParams.line2Length_ = 0.5f;
    testParams.line2Color_ = Vector4f(0.5f, 0.6f, 0.7f, 0.8f);
    testParams.thickness_ = 0.2f;
    testParams.haloRadius_ = 10.0f;
    testParams.lightWeight_ = 1.0f;
    testParams.haloWeight_ = 5.0f;
    testParams.contour_ = std::vector<Vector2f>(256, Vector2f(0.5f, 0.5f));

    GEContourDiagonalFlowLightShader shader(testParams);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Start_, testParams.line1Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Length_, testParams.line1Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[0], testParams.line1Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[1], testParams.line1Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[2], testParams.line1Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[3], testParams.line1Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Start_, testParams.line2Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Length_, testParams.line2Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[0], testParams.line2Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[1], testParams.line2Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[2], testParams.line2Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[3], testParams.line2Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.thickness_, testParams.thickness_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.haloRadius_, testParams.haloRadius_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.lightWeight_, testParams.lightWeight_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.haloWeight_, testParams.haloWeight_);
    for (size_t i = 0; i < testParams.contour_.size(); ++i) {
        EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.contour_[i][0], testParams.contour_[i][0]);
        EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.contour_[i][1], testParams.contour_[i][1]);
    }
}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: Verify function MakeDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, MakeDrawingShader_001, TestSize.Level1)
{
    float progress = 0.5f;
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.0f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.35f, 0.8f);
    params.thickness_ = 0.25f;
    params.haloRadius_ = 105.0f;
    params.lightWeight_ = 0.5f;
    params.haloWeight_ = 50.0f;
    params.contour_ = std::vector<Vector2f>(128, Vector2f(0.2f, 0.5f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.Preprocess(*canvas_, rect_);
    shader.MakeDrawingShader(rect_, progress);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr); // No implementation of MakeDrawingShader
}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, GetDescription_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.05f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.8f, 0.5f, 1.0f, 1.0f);
    params.line2Start_ = 0.55f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.2f, 0.6f, 1.0f, 0.8f);
    params.thickness_ = 0.5f;
    params.haloRadius_ = 13.0f;
    params.lightWeight_ = 1.0f;
    params.haloWeight_ = 5.0f;
    params.contour_ = std::vector<Vector2f>(78, Vector2f(0.153f, 0.345f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    std::string expectStr = "GEContourDiagonalFlowLightShader";
    EXPECT_EQ(shader.GetDescription(), expectStr);
}

/**
 * @tc.name: SetGEContentDiagonalFlowLightShaderParams_001
 * @tc.desc: Verify function SetGEContentDiagonalFlowLightShaderParams
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, SetGEContentDiagonalFlowLightShaderParams_001, TestSize.Level1)
{
    // Test case for SetGEContentDiagonalFlowLightShaderParams method
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.1f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.6f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.9f, 0.8f);
    params.thickness_ = 0.25f;
    params.haloRadius_ = 10.0f;
    params.lightWeight_ = 1.0f;
    params.haloWeight_ = 5.0f;
    params.contour_ = std::vector<Vector2f>(64, Vector2f(0.1f, 0.5f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    GEContentDiagonalFlowLightShaderParams params1;
    params1.line1Start_ = 0.4f;
    params1.line1Length_ = 0.3f;
    params1.line1Color_ = Vector4f(0.1f, 0.8f, 0.5f, 1.0f);
    params1.line2Start_ = 0.9f;
    params1.line2Length_ = 0.4f;
    params1.line2Color_ = Vector4f(0.8f, 0.9f, 0.1f, 1.0f);
    params1.thickness_ = 0.2f;
    params1.haloRadius_ = 5.0f;
    params1.lightWeight_ = 2.0f;
    params1.haloWeight_ = 10.0f;
    params1.contour_ = std::vector<Vector2f>(128, Vector2f(0.2f, 0.5f));

    shader.SetGEContentDiagonalFlowLightShaderParams(params1);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Start_, params1.line1Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Length_, params1.line1Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[0], params1.line1Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[1], params1.line1Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[2], params1.line1Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[3], params1.line1Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Start_, params1.line2Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Length_, params1.line2Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[0], params1.line2Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[1], params1.line2Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[2], params1.line2Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[3], params1.line2Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.thickness_, params1.thickness_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.haloRadius_, params1.haloRadius_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.lightWeight_, params1.lightWeight_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.haloWeight_, params1.haloWeight_);
    for (size_t i = 0; i < params1.contour_.size(); ++i) {
        EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.contour_[i][0], params1.contour_[i][0]);
        EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.contour_[i][1], params1.contour_[i][1]);
    }
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify function Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, Preprocess_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.1f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.6f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.9f, 0.8f);
    params.thickness_ = 0.25f;
    params.haloRadius_ = 100.0f;
    params.lightWeight_ = 10.0f;
    params.haloWeight_ = 0.5f;
    params.contour_ = std::vector<Vector2f>(80, Vector2f(0.2f, 0.3f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    EXPECT_NE(shader.cacheAnyPtr_, nullptr);

    params.contour_ = std::vector<Vector2f>(4, Vector2f(0.2f, 0.3f));
    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    shader2.Preprocess(*canvas_, rect_);
    EXPECT_EQ(shader2.cacheAnyPtr_, nullptr); // the number of curves deficient, which is 6 in minimum

    params.contour_ = std::vector<Vector2f>(3, Vector2f(0.2f, 0.3f));
    auto shader3 = GEContourDiagonalFlowLightShader(params);
    shader3.offscreenSurface_ = surface_;
    shader3.offscreenCanvas_ = canvas_;
    shader3.Preprocess(*canvas_, rect_);
    EXPECT_EQ(shader3.cacheAnyPtr_, nullptr); // the number of curves is sufficient but odd

    params.contour_ = std::vector<Vector2f>(80, Vector2f(0.2f, 0.3f));
    auto shader4 = GEContourDiagonalFlowLightShader(params);
    shader4.offscreenSurface_ = surface_;
    shader4.offscreenCanvas_ = canvas_;
    Drawing::Rect rect(0, 0, 0, 0);
    shader4.Preprocess(*canvas_, rect);
    EXPECT_EQ(shader4.cacheAnyPtr_, nullptr); // rect is not valid
}

/**
 * @tc.name: DrawShader_001
 * @tc.desc: Verify function DrawShader
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, DrawShader_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.2f;
    params.line1Length_ = 0.8f;
    params.line1Color_ = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
    params.line2Start_ = 0.7f;
    params.line2Length_ = 0.3f;
    params.line2Color_ = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
    params.thickness_ = 0.4f;
    params.haloRadius_ = 10.0f;
    params.lightWeight_ = 1.2f;
    params.haloWeight_ = 25.0f;
    params.contour_ = std::vector<Vector2f>(46, Vector2f(0.15f, 0.57f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.DrawShader(*canvas_, rect_);
    EXPECT_NE(shader.cacheAnyPtr_, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    Drawing::Rect rect(0, 0, 0, 0);
    shader2.DrawShader(*canvas_, rect);
    EXPECT_EQ(shader2.cacheAnyPtr_, nullptr); // rect is not valid
}

/**
 * @tc.name: DrawRuntimeShader_001
 * @tc.desc: Verify function DrawRuntimeShader
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, DrawRuntimeShader_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.05f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.8f, 0.5f, 1.0f, 1.0f);
    params.line2Start_ = 0.55f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.2f, 0.6f, 1.0f, 0.8f);
    params.thickness_ = 0.5f;
    params.haloRadius_ = 20.0f;
    params.lightWeight_ = 0.9f;
    params.haloWeight_ = 15.0f;
    params.contour_ = std::vector<Vector2f>(98, Vector2f(0.45f, 0.88f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_EQ(img, nullptr); // cache is empty

    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    Drawing::Rect rect(0, 0, 0, 0);
    img = shader2.DrawRuntimeShader(*canvas_, rect);
    EXPECT_EQ(img, nullptr); // rect is invalid

    params.line1Length_ = -0.6f;
    auto shader3 = GEContourDiagonalFlowLightShader(params);
    shader3.offscreenSurface_ = surface_;
    shader3.offscreenCanvas_ = canvas_;
    shader3.Preprocess(*canvas_, rect_);
    img = shader3.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr); //lineLength is negative

    params.line1Length_ = 0.0f;
    auto shader4 = GEContourDiagonalFlowLightShader(params);
    shader4.offscreenSurface_ = surface_;
    shader4.offscreenCanvas_ = canvas_;
    shader4.Preprocess(*canvas_, rect_);
    img = shader4.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr); // lineLength is zero
}

/**
 * @tc.name: BlendImg_001
 * @tc.desc: Verify function BlendImg
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, BlendImg_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.05f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.8f, 0.5f, 1.0f, 1.0f);
    params.line2Start_ = 0.55f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.2f, 0.6f, 1.0f, 0.8f);
    params.thickness_ = 0.5f;
    params.haloRadius_ = 20.0f;
    params.lightWeight_ = 0.9f;
    params.haloWeight_ = 15.0f;
    params.contour_ = std::vector<Vector2f>(98, Vector2f(0.45f, 0.88f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bmp.Build(100, 100, format); // 100, 100  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    std::shared_ptr<Drawing::Image> precalc = bmp.MakeImage();
    std::shared_ptr<Drawing::Image> light = bmp.MakeImage();
    std::shared_ptr<Drawing::Image> halo = bmp.MakeImage();
    auto image = shader.BlendImg(*canvas_, precalc, light, halo);
    EXPECT_EQ(image, nullptr); // cache is empty

    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    precalc = bmp.MakeImage();
    light = bmp.MakeImage();
    halo = bmp.MakeImage();
    image = shader.BlendImg(*canvas_, precalc, light, halo);
    EXPECT_NE(image, nullptr);

    precalc = nullptr;
    light = nullptr;
    halo = nullptr;
    image = shader.BlendImg(*canvas_, precalc, light, halo);
    EXPECT_EQ(image, nullptr); // input images are nullptr

    precalc = bmp.MakeImage();
    light   = nullptr;
    halo    = bmp.MakeImage();
    image = shader.BlendImg(*canvas_, precalc, light, halo);
    EXPECT_EQ(image, nullptr); // exists null input image

    precalc = bmp.MakeImage();
    light   = bmp.MakeImage();
    halo    = nullptr;
    image = shader.BlendImg(*canvas_, precalc, light, halo);
    EXPECT_EQ(image, nullptr); // exists null input image
}

/**
 * @tc.name: GetContourDiagonalFlowLightBuilder_001
 * @tc.desc: Verify function GetContourDiagonalFlowLightBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, GetContourDiagonalFlowLightBuilder_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.2f;
    params.line1Length_ = 0.8f;
    params.line1Color_ = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
    params.line2Start_ = 0.7f;
    params.line2Length_ = 0.3f;
    params.line2Color_ = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
    params.thickness_ = 0.4f;
    params.haloRadius_ = 80.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 15.0f;
    params.contour_ = std::vector<Vector2f>(98, Vector2f(0.6f, 0.5f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    auto builder = shader.GetContourDiagonalFlowLightBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: GetFlowLightPrecalBuilder_001
 * @tc.desc: Verify function GetFlowLightPrecalBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, GetFlowLightPrecalBuilder_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.1f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.6f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.9f, 0.8f);
    params.thickness_ = 0.25f;
    params.haloRadius_ = 50.0f;
    params.lightWeight_ = 2.0f;
    params.haloWeight_ = 3.0f;
    params.contour_ = std::vector<Vector2f>(204, Vector2f(0.1f, 0.59f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    auto builder = shader.GetFlowLightPrecalBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: FlowLightConvertBuilder_001
 * @tc.desc: Verify function FlowLightConvertBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, FlowLightConvertBuilder_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    auto builder = shader.FlowLightConvertBuilder();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: ComputeCurveBoundingBox_001
 * @tc.desc: Verify function ComputeCurveBoundingBox with normal canvas size
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ComputeCurveBoundingBox_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(32, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, 100, 100);
    ConvertPointsTo(ndcPoints, shader.controlPoints_);
    size_t numCurves = 16;
    shader.controlPoints_.resize(numCurves * 2);
    float maxThickness = 0.05f;
    float approxLen = 0.0f;

    for (size_t i = 0; i < numCurves; ++i) {
        Box4f bbox = shader.ComputeCurveBoundingBox(i, maxThickness, 100, 100, approxLen);
        EXPECT_FLOAT_EQ(bbox[0], 24.0f);
        EXPECT_FLOAT_EQ(bbox[1], 30.0f);
        EXPECT_FLOAT_EQ(bbox[2], 62.0f);
        EXPECT_FLOAT_EQ(bbox[3], 68.0f);
    }
}

/**
 * @tc.name: ComputeCurveBoundingBox_002
 * @tc.desc: Verify ComputeCurveBoundingBox with canvas height=0
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ComputeCurveBoundingBox_002, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(32, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, 100, 100);
    ConvertPointsTo(ndcPoints, shader.controlPoints_);
    size_t numCurves = 16;
    shader.controlPoints_.resize(numCurves * 2);
    float maxThickness = 0.05f;
    float approxLen = 0.0f;

    for (size_t i = 0; i < numCurves; ++i) {
        Box4f bbox = shader.ComputeCurveBoundingBox(i, maxThickness, 100, 0, approxLen);
        EXPECT_FLOAT_EQ(bbox[0], 24.0f);
        EXPECT_FLOAT_EQ(bbox[1], 30.0f);
        EXPECT_FLOAT_EQ(bbox[2], 0.0f);
        EXPECT_FLOAT_EQ(bbox[3], 0.0f);
    }
}

/**
 * @tc.name: ComputeCurveBoundingBox_003
 * @tc.desc: Verify ComputeCurveBoundingBox with canvas width=0
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ComputeCurveBoundingBox_003, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(32, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, 100, 100);
    ConvertPointsTo(ndcPoints, shader.controlPoints_);
    size_t numCurves = 16;
    shader.controlPoints_.resize(numCurves * 2);
    float maxThickness = 0.05f;
    float approxLen = 0.0f;

    for (size_t i = 0; i < numCurves; ++i) {
        Box4f bbox = shader.ComputeCurveBoundingBox(i, maxThickness, 0, 100, approxLen);
        EXPECT_FLOAT_EQ(bbox[0], 0.0f);
        EXPECT_FLOAT_EQ(bbox[1], 0.0f);
        EXPECT_FLOAT_EQ(bbox[2], 62.0f);
        EXPECT_FLOAT_EQ(bbox[3], 68.0f);
    }
}

/**
 * @tc.name: ComputeCurveBoundingBox_004
 * @tc.desc: Verify ComputeCurveBoundingBox with canvas width=0 and height=0
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ComputeCurveBoundingBox_004, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(32, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, 100, 100);
    ConvertPointsTo(ndcPoints, shader.controlPoints_);
    size_t numCurves = 16;
    shader.controlPoints_.resize(numCurves * 2);
    float maxThickness = 0.05f;
    float approxLen = 0.0f;

    for (size_t i = 0; i < numCurves; ++i) {
        Box4f bbox = shader.ComputeCurveBoundingBox(i, maxThickness, 0, 0, approxLen);
        EXPECT_FLOAT_EQ(bbox[0], 0.0f);
        EXPECT_FLOAT_EQ(bbox[1], 0.0f);
        EXPECT_FLOAT_EQ(bbox[2], 0.0f);
        EXPECT_FLOAT_EQ(bbox[3], 0.0f);
    }
}

/**
 * @tc.name: CreateSurfaceAndCanvas_001
 * @tc.desc: Verify function CreateSurfaceAndCanvas
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, CreateSurfaceAndCanvas_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.CreateSurfaceAndCanvas(*canvas_, rect_);
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    Drawing::Rect rect(0, 0, 0, 0);
    shader2.CreateSurfaceAndCanvas(*canvas_, rect);
    shader2.Preprocess(*canvas_, rect);
    img = shader2.DrawRuntimeShader(*canvas_, rect);
    EXPECT_EQ(img, nullptr); // rect is invalid
}

/**
 * @tc.name: PreCalculateRegion_001
 * @tc.desc: Verify function PreCalculateRegion
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, PreCalculateRegion_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenCanvas_ = canvas_;
    size_t curvesInGrid = 4;
    std::vector<Drawing::Rect> rectArr;
    rectArr.reserve(4);
    rectArr.emplace_back(Drawing::Rect(0, 0, 50, 50));
    rectArr.emplace_back(Drawing::Rect(50, 0, 50, 0));
    rectArr.emplace_back(Drawing::Rect(0, 50, 50, 50));
    rectArr.emplace_back(Drawing::Rect(50, 50, 50, 50));
    for (size_t i = 0; i < curvesInGrid; i++) {
        shader.PreCalculateRegion(*canvas_, *(shader.offscreenCanvas_), i, rect_, rectArr[i]);
    }
    shader.offscreenSurface_ = surface_;
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);
}

/**
 * @tc.name: AutoPartitionCal_001
 * @tc.desc: Verify function AutoPartitionCal
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, AutoPartitionCal_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.AutoPartitionCal(*canvas_, rect_);
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.AutoPartitionCal(*canvas_, rect_);
    shader2.Preprocess(*canvas_, rect_);
    img = shader2.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr); // offscreenCanvas_ is nullptr

    auto shader3 = GEContourDiagonalFlowLightShader(params);
    shader3.Preprocess(*canvas_, rect_);
    Drawing::Rect rect(0, 0, 0, 0);
    shader3.curvesInGrid_.clear();
    shader3.AutoPartitionCal(*canvas_, rect);
    img = shader3.DrawRuntimeShader(*canvas_, rect);
    EXPECT_EQ(img, nullptr); // rect is invalid
}

/**
 * @tc.name: AutoGridPartition_001
 * @tc.desc: Verify function AutoGridPartition
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, AutoGridPartition_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    float maxThickness = 0.05f;
    shader.AutoGridPartition(rect_.GetWidth(), rect_.GetHeight(), maxThickness);
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    shader2.AutoGridPartition(0, 50, maxThickness);
    img = shader2.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_EQ(img, nullptr); // width is invalid

    auto shader3 = GEContourDiagonalFlowLightShader(params);
    shader3.offscreenSurface_ = surface_;
    shader3.offscreenCanvas_ = canvas_;
    shader3.AutoGridPartition(50, 0, maxThickness);
    img = shader3.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_EQ(img, nullptr); // height is invalid
}

/**
 * @tc.name: AutoGridPartition_002
 * @tc.desc: Verify function AutoGridPartition if needs split
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, AutoGridPartition_002, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    const size_t K = 24;
    std::vector<Vector2f> contour;
    contour.reserve(2 * K);
    for (size_t i = 0; i < K; ++i) {
        float y  = 0.05f + 0.90f * (float(i) / float(K - 1)); // 0.05 → 0.95，fill the height
        float x0 = (i & 1) ? 0.95f : 0.05f; // widen the bbox
        float xc = 0.50f; // middle control point
        contour.emplace_back(x0, y);  // P0 (UV)
        contour.emplace_back(xc, y);  // C  (UV)
    }
    params.contour_ = std::move(contour);
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    float maxThickness = 0.05f;
    Drawing::Rect rect(0, 0, 512, 512);
    shader.Preprocess(*canvas_, rect);
    shader.AutoGridPartition(rect.GetWidth(), rect.GetHeight(), maxThickness);
    auto img = shader.DrawRuntimeShader(*canvas_, rect);
    EXPECT_NE(img, nullptr);
}

/**
 * @tc.name: ComputeAllCurveBoundingBoxes_001
 * @tc.desc: Verify function ComputeAllCurveBoundingBoxes
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ComputeAllCurveBoundingBoxes_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBox;
    shader.ComputeAllCurveBoundingBoxes(100, 100, 0.05f, canvasBBox, curveBBoxes); // width, height, maxThickness
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(img, nullptr);
}

/**
 * @tc.name: InitializeWorkQueue_001
 * @tc.desc: Verify function InitializeWorkQueue
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, InitializeWorkQueue_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_  = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_  = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_  = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_  = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_   = 0.2f;
    params.haloRadius_  = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_  = 53.0f;

    const int w = 200, h = 200;
    const size_t K = 20;
    std::vector<Vector2f> contour; contour.reserve(2 * K);
    for (size_t i = 0; i < K; ++i) {
        float y  = 0.05f + 0.90f * (float(i) / float(K - 1)); // 0.05→0.95
        float x0 = (i & 1) ? 0.90f : 0.10f;
        float xc = 0.50f;
        contour.emplace_back(x0, y);  // P0 (UV)
        contour.emplace_back(xc, y);  // C  (UV)
    }
    params.contour_ = std::move(contour);
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, w, h);
    shader.pointCnt_ = ndcPoints.size();
    shader.numCurves_ = shader.pointCnt_ / 2;
    ConvertPointsTo(ndcPoints, shader.controlPoints_);

    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBoxUnion;
    shader.ComputeAllCurveBoundingBoxes(w, h, 0.05f, canvasBBoxUnion, curveBBoxes); // 0.05: maxThickness
    Box4f leftHalfCanvas{0.0f, float(w) * 0.5f, 0.0f, float(h)};

    std::queue<Grid> workQueue;
    shader.InitializeWorkQueue(leftHalfCanvas, curveBBoxes, workQueue);

    ASSERT_FALSE(workQueue.empty());
    Grid root = workQueue.front();
    EXPECT_GT(root.curveIndices.size(), 0u);
    EXPECT_EQ(root.curveIndices.size(), static_cast<size_t>(shader.numCurves_));
}

/**
 * @tc.name: SplitGrid_001
 * @tc.desc: Verify function SplitGrid
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, SplitGrid_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_  = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_  = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_  = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_  = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_   = 0.2f;
    params.haloRadius_  = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_  = 53.0f;
    const int w = 100, h = 100;
    const size_t K = 12;
    std::vector<Vector2f> contour; contour.reserve(2 * K);
    for (size_t i = 0; i < K; ++i) {
        float y  = 0.05f + 0.90f * (float(i) / float(K - 1));
        float x0 = (i & 1) ? 0.90f : 0.10f;
        float xc = 0.50f;
        contour.emplace_back(x0, y);  // P0
        contour.emplace_back(xc, y);  // C
    }
    params.contour_ = contour;

    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, w, h);
    shader.pointCnt_ = ndcPoints.size();
    shader.numCurves_ = shader.pointCnt_ / 2;
    ConvertPointsTo(ndcPoints, shader.controlPoints_);

    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBox;
    shader.ComputeAllCurveBoundingBoxes(w, h, 0.05f, canvasBBox, curveBBoxes); // 0.05f: maxThickness

    std::queue<Grid> workQueue;
    shader.InitializeWorkQueue(canvasBBox, curveBBoxes, workQueue);
    ASSERT_FALSE(workQueue.empty());
    Grid current = workQueue.front();

    size_t q0 = workQueue.size();
    shader.SplitGrid(current, curveBBoxes, workQueue, 128); // 128: minGridSize
    EXPECT_GT(workQueue.size(), q0);
}

/**
 * @tc.name: SplitGrid_002
 * @tc.desc: Verify function SplitGrid sparse quadrants for empty child.curveIndices
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, SplitGrid_002, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_  = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_  = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_  = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_  = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_   = 0.2f;
    params.haloRadius_  = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_  = 53.0f;
    const int w = 200, h = 200;
    const size_t K = 6;
    std::vector<Vector2f> contour; contour.reserve(2 * K);
    for (size_t i = 0; i < K; ++i) {
        float y  = 0.08f + 0.01f * static_cast<float>(i); // 0.08 ~ 0.13
        float x0 = 0.10f;
        float xc = 0.12f;
        contour.emplace_back(x0, y);  // P0 (UV)
        contour.emplace_back(xc, y);  // C  (UV)
    }
    params.contour_ = std::move(contour);
    auto shader = GEContourDiagonalFlowLightShader(params);

    auto ndcPoints = ConvertUVToNDC(shader.contourDiagonalFlowLightParams_.contour_, w, h);
    shader.pointCnt_  = ndcPoints.size();
    shader.numCurves_ = shader.pointCnt_ / 2;
    ConvertPointsTo(ndcPoints, shader.controlPoints_);
    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBox;
    shader.ComputeAllCurveBoundingBoxes(w, h, 0.05f, canvasBBox, curveBBoxes); // 0.05:maxThickness

    std::queue<Grid> workQueue;
    shader.InitializeWorkQueue(canvasBBox, curveBBoxes, workQueue);
    ASSERT_FALSE(workQueue.empty());
    size_t q0 = workQueue.size();
    Grid current = workQueue.front(); // entire curveIndices
    current.bbox[1] += 1000.0f;  // XMAX_I, expand to the right
    current.bbox[3] += 1000.0f;  // YMAX_I, expand downwards

    shader.SplitGrid(current, curveBBoxes, workQueue, 128); // 128: minGridSize
    size_t pushed = workQueue.size() - q0;
    // only push the lefttop corner to the workQueue，child.curveIndices.empty() is true in the other three corners
    EXPECT_EQ(pushed, 1u);
}

/**
 * @tc.name: ProcessFinalGrid_001
 * @tc.desc: Verify function ProcessFinalGrid
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ProcessFinalGrid_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBox;
    shader.ComputeAllCurveBoundingBoxes(100, 100, 0.05f, canvasBBox, curveBBoxes); // width, height, maxThickness
    std::queue<Grid> workQueue;
    shader.InitializeWorkQueue(canvasBBox, curveBBoxes, workQueue);
    Grid current = workQueue.front();
    shader.ProcessFinalGrid(current, curveBBoxes, 100); // 100: height
    ASSERT_FALSE(shader.curvesInGrid_.empty());
    ASSERT_FALSE(shader.segmentIndex_.empty());
}

/**
 * @tc.name: CreateImg_001
 * @tc.desc: Verify function CreateImg
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, CreateImg_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    auto img = shader.CreateImg(*canvas_, rect_);
    EXPECT_NE(img, nullptr);

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    shader2.Preprocess(*canvas_, rect_);
    Drawing::Rect rect(0, 0, 0, 0);
    img = shader2.CreateImg(*canvas_, rect);
    EXPECT_EQ(img, nullptr); // rect is invalid
}

/**
 * @tc.name: CreateDrawImg_001
 * @tc.desc: Verify function CreateDrawImg
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, CreateDrawImg_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    Drawing::Brush brush;
    auto img = shader.CreateDrawImg(*canvas_, rect_, brush);
    EXPECT_EQ(img, nullptr); // could not get surface from canvas

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenSurface_ = surface_;
    shader2.offscreenCanvas_ = canvas_;
    shader2.Preprocess(*canvas_, rect_);
    Drawing::Rect rect(0, 0, 0, 0);
    img = shader2.CreateDrawImg(*canvas_, rect, brush);
    EXPECT_EQ(img, nullptr); // rect is invalid
}

/**
 * @tc.name: ResizeCurvesData_001
 * @tc.desc: Verify function ResizeCurvesData
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ResizeCurvesData_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    const int gridIndex = 24;
    const size_t subCurveCnt = 2;
    const size_t perSubCurveSize = 96;
    shader.curvesInGrid_.resize(static_cast<size_t>(gridIndex) + 1);
    shader.segmentIndex_.resize(static_cast<size_t>(gridIndex) + 1);
    shader.ResizeCurvesData(gridIndex, subCurveCnt, perSubCurveSize);
    EXPECT_EQ(shader.curvesInGrid_[gridIndex].first.size(), static_cast<size_t>(subCurveCnt * perSubCurveSize));
    EXPECT_EQ(shader.segmentIndex_[gridIndex].size(), static_cast<size_t>(subCurveCnt * 16)); // 16: MAX_CURVES_PER_GRID
}

/**
 * @tc.name: LoopAllCurvesInBatches_001
 * @tc.desc: Verify function LoopAllCurvesInBatches
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, LoopAllCurvesInBatches_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenCanvas_ = canvas_;
    size_t curvesInGrid = 4;
    std::vector<Drawing::Rect> rectArr;
    rectArr.reserve(4);
    rectArr.emplace_back(Drawing::Rect(0, 0, 50, 50));
    rectArr.emplace_back(Drawing::Rect(50, 0, 50, 0));
    rectArr.emplace_back(Drawing::Rect(0, 50, 50, 50));
    rectArr.emplace_back(Drawing::Rect(50, 50, 50, 50));
    for (size_t i = 0; i < curvesInGrid; i++) {
        auto img = shader.LoopAllCurvesInBatches(*canvas_, *(shader.offscreenCanvas_), i, rect_, rectArr[i]);
        EXPECT_EQ(img, nullptr); // could not get surface from canvas in CreateImg
    }

    auto shader2 = GEContourDiagonalFlowLightShader(params);
    shader2.offscreenCanvas_ = canvas_;
    Drawing::Rect rect(0, 0, 0, 0);
    for (size_t i = 0; i < curvesInGrid; i++) {
        auto img = shader2.LoopAllCurvesInBatches(*canvas_, *(shader.offscreenCanvas_), i, rect, rectArr[i]);
        EXPECT_EQ(img, nullptr); // rect is invalid
    }
}

/**
 * @tc.name: ConvertImage_001
 * @tc.desc: Verify function ConvertImage
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, ConvertImage_001, TestSize.Level1)
{
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.4f;
    params.line1Color_ = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.line2Start_ = 0.2f;
    params.line2Length_ = 0.5f;
    params.line2Color_ = Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
    params.thickness_ = 0.2f;
    params.haloRadius_ = 56.0f;
    params.lightWeight_ = 1.5f;
    params.haloWeight_ = 53.0f;
    params.contour_ = std::vector<Vector2f>(166, Vector2f(0.27f, 0.65f));
    auto shader = GEContourDiagonalFlowLightShader(params);
    shader.offscreenSurface_ = surface_;
    shader.offscreenCanvas_ = canvas_;
    shader.Preprocess(*canvas_, rect_);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 100, format); // 100, 100  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    std::shared_ptr<Drawing::Image> img = bmp.MakeImage();
    shader.ConvertImage(*canvas_, rect_, img);
    auto image = shader.DrawRuntimeShader(*canvas_, rect_);
    EXPECT_NE(image, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEContourDiagonalFlowLightShaderTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEContourDiagonalFlowLightShaderTest Type_001 start";

    auto filter = std::make_unique<GEContourDiagonalFlowLightShader>();
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT);

    GTEST_LOG_(INFO) << "GEContourDiagonalFlowLightShaderTest Type_001 end";
}

} // namespace Rosen
} // namespace OHOS
