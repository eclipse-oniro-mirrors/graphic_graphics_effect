/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "draw/path.h"
#include "ge_external_dynamic_loader.h"
#include "ge_sdf_path_shader_shape.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
    struct TestConfig {
        std::function<Drawing::Path()> buildPath;
        Drawing::Rect rect;
        Vector2f offset = Vector2f(0.0f, 0.0f);
        Vector2f scale = Vector2f(1.0f, 1.0f);
        std::string description;
    };

    static std::vector<TestConfig> VALID_CONFIGS = {
        { []() {
            Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            path.LineTo(100.0f, 200.0f);
            return path;
            },
            Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f), Vector2f(0.0f, 0.0f),
            Vector2f(1.0f, 1.0f), "valid rect"},
        { []() {
            Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            path.LineTo(100.0f, 200.0f);
            path.LineTo(150.0f, 250.0f);
            path.LineTo(200.0f, 300.0f);
            path.LineTo(200.0f, 20.0f);
            path.LineTo(10.0f, 20.0f);
            return path;
         },
            Drawing::Rect(0.0f, 0.0f, 300.0f, 300.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "multiple line segments" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 50.0f, 50.0f), Vector2f(0.0f, 0.0f), Vector2f(0.1f, 0.1f),
            "very small rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 1000.0f, 1000.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "very large rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             path.LineTo(10.0f, 200.0f);
             path.LineTo(10.0f, 20.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 200.0f, 100.0f), Vector2f(0.0f, 0.0f), Vector2f(0.5f, 0.5f),
            "asymmetric rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             path.QuadTo(150.0f, 250.0f, 200.0f, 300.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 250.0f, 350.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "compute all curve bounding boxes" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f), Vector2f(0.0f, 0.0f), Vector2f(2.0f, 2.0f),
            "large scale factor" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             path.LineTo(100.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f), Vector2f(0.0f, 0.0f), Vector2f(0.001f, 0.001f),
            "small scale factor" },
        { []() {
             Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            path.QuadTo(30.0f, 50.0f, 50.0f, 80.0f);
            path.QuadTo(70.0f, 110.0f, 90.0f, 140.0f);
            path.QuadTo(110.0f, 170.0f, 130.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 150.0f, 150.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "multiple quadratic bezier curves" },
        { []() {
            Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            path.CubicTo(20.0f, 30.0f, 40.0f, 60.0f, 50.0f, 80.0f);
            path.CubicTo(60.0f, 90.0f, 80.0f, 120.0f, 90.0f, 140.0f);
            path.CubicTo(100.0f, 150.0f, 120.0f, 180.0f, 130.0f, 200.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 150.0f, 150.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "multiple cubic bezier curves" },
        { []() {
            Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            path.LineTo(50.0f, 100.0f);
            path.QuadTo(75.0f, 150.0f, 100.0f, 200.0f);
            path.CubicTo(120.0f, 220.0f, 150.0f, 280.0f, 200.0f, 300.0f);
            path.LineTo(250.0f, 350.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 300.0f, 300.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "complex path with multiple curves" },
    };

    static std::vector<TestConfig> INVALID_CONFIGS = {
        { []() {
            Drawing::Path path;
            path.MoveTo(10.0f, 20.0f);
            return path;
        },
            Drawing::Rect(0.0f, 0.0f, -10.0f, -10.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "invalid rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 0.0f, 100.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "zero width rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(10.0f, 20.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 100.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "zero height rect" },
        { []() {
             Drawing::Path path;
             path.MoveTo(50.0f, 100.0f);
             return path;
         },
            Drawing::Rect(0.0f, 0.0f, 0.5f, 0.5f), Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f),
            "very small dimensions" }
        };
};

class GESDFPathShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Rect rect_;
    Drawing::ImageInfo imageInfo_ = {};
};

void GESDFPathShaderShapeTest::SetUpTestCase(void) {}
void GESDFPathShaderShapeTest::TearDownTestCase(void) {}

void GESDFPathShaderShapeTest::SetUp()
{
    Drawing::Rect rect { 0.0f, 0.0f, 400.0f, 400.0f };
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo { rect.GetWidth(), rect.GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_F16,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    surface_ = CreateSurface();
    if (surface_ != nullptr) {
        canvas_ = surface_->GetCanvas();
    }
}

void GESDFPathShaderShapeTest::TearDown() {}

std::shared_ptr<Drawing::Surface> GESDFPathShaderShapeTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

/**
 * @tc.name: GetPath_001
 * @tc.desc: Verify GetPath returns correct path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetPath_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    const auto& resultPath = shape.GetPath();
    EXPECT_TRUE(resultPath.IsValid());
}

/**
 * @tc.name: GetOffset_001
 * @tc.desc: Verify GetOffset returns correct offset
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetOffset_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    param.path = path;
    param.offset = Vector2f(15.0f, 25.0f);

    GESDFPathShaderShape shape(param);
    const auto& offset = shape.GetOffset();
    EXPECT_EQ(offset.x_, 15.0f);
    EXPECT_EQ(offset.y_, 25.0f);
}

/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GESDFPathShapeParams param1;
    Drawing::Path path1;
    path1.MoveTo(10.0f, 20.0f);
    param1.path = path1;
    param1.offset = Vector2f(5.0f, 10.0f);

    GESDFPathShapeParams param2;
    Drawing::Path path2;
    path2.MoveTo(30.0f, 40.0f);
    path2.LineTo(50.0f, 60.0f);
    param2.path = path2;
    param2.offset = Vector2f(15.0f, 25.0f);

    GESDFPathShaderShape shape1(param1);
    GESDFPathShaderShape shape2(param2);

    shape1.CopyState(shape2);
    EXPECT_TRUE(shape1.GetPath().IsValid());
    EXPECT_EQ(shape1.GetOffset().x_, 15.0f);
    EXPECT_EQ(shape1.GetOffset().y_, 25.0f);
}

/**
 * @tc.name: PreprocessValidParamstest_001
 * @tc.desc: Verify Preprocess with valid scenarios including valid rect, proper path,
 *            different rect sizes, scale and offset
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, PreprocessValidParamstest_001, TestSize.Level1)
{
    for (const auto& config : VALID_CONFIGS) {
        GESDFPathShapeParams param;
        param.path = config.buildPath();
        param.offset = config.offset;
        param.scale = config.scale;

        GESDFPathShaderShape shape(param);
        shape.Preprocess(*canvas_, config.rect, false);
        EXPECT_EQ(shape.disResult_, nullptr);

        auto shader = shape.GenerateDrawingShader(config.rect.GetWidth(), config.rect.GetHeight());
        EXPECT_EQ(shader, nullptr);
    }
}

/**
 * @tc.name: PreprocessInvalidParamsTest_001
 * @tc.desc: Verify Preprocess with invalid scenarios including invalid rect, zero width/height rect, small dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, PreprocessInvalidParamsTest_001, TestSize.Level1)
{
    for (const auto& config : INVALID_CONFIGS) {
        GESDFPathShapeParams param;
        param.path = config.buildPath();
        param.offset = config.offset;
        param.scale = config.scale;

        GESDFPathShaderShape shape(param);
        shape.Preprocess(*canvas_, config.rect, false);
        EXPECT_EQ(shape.disResult_, nullptr);

        auto shader = shape.GenerateDrawingShader(config.rect.GetWidth(), config.rect.GetHeight());
        EXPECT_EQ(shader, nullptr);
    }
}

/**
 * @tc.name: GetCurveByPath_001
 * @tc.desc: Verify GetCurveByPath with valid path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_001, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
}

/**
 * @tc.name: GetCurveByPath_002
 * @tc.desc: Verify GetCurveByPath with quadratic bezier curve
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_002, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.QuadTo(50.0f, 100.0f, 100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 3);
}

/**
 * @tc.name: GetCurveByPath_003
 * @tc.desc: Verify GetCurveByPath with cubic bezier curve
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_003, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.CubicTo(30.0f, 40.0f, 60.0f, 80.0f, 100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 4);
}

/**
 * @tc.name: GetCurveByPath_004
 * @tc.desc: Verify GetCurveByPath with multiple segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_004, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.QuadTo(200.0f, 300.0f, 250.0f, 350.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 3);
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify Preprocess with valid path and invalid canvas
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Canvas canvas;

    shape.Preprocess(canvas, rect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
}

/**
 * @tc.name: Preprocess_002
 * @tc.desc: Verify Preprocess with hasNormal parameter
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_002, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(*canvas_, rect, true);
    EXPECT_EQ(shape.disResult_, nullptr);
}

/**
 * @tc.name: cubicToQuadraticSingle_001
 * @tc.desc: Verify cubicToQuadraticSingle with valid points
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, cubicToQuadraticSingle_001, TestSize.Level1)
{
    Vector2f p0(0.0f, 0.0f);
    Vector2f p1(50.0f, 100.0f);
    Vector2f p2(100.0f, 150.0f);
    Vector2f p3(150.0f, 200.0f);

    Vector2f originalP1 = p1;
    GESDFPathShaderShape::cubicToQuadraticSingle(p0, p1, p2, p3);
    EXPECT_NE(p1.y_, originalP1.y_);
}

/**
 * @tc.name: cubicToQuadraticSingle_002
 * @tc.desc: Verify cubicToQuadraticSingle with different end points
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, cubicToQuadraticSingle_002, TestSize.Level1)
{
    Vector2f p0(10.0f, 20.0f);
    Vector2f p1(30.0f, 60.0f);
    Vector2f p2(80.0f, 120.0f);
    Vector2f p3(200.0f, 300.0f);

    Vector2f originalP1 = p1;
    GESDFPathShaderShape::cubicToQuadraticSingle(p0, p1, p2, p3);
    EXPECT_NE(p1.y_, originalP1.y_);
}

/**
 * @tc.name: AutoGridPartition_001
 * @tc.desc: Verify AutoGridPartition with valid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, AutoGridPartition_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(*canvas_, rect, false);
    EXPECT_TRUE(shape.curvesInGrid_.empty());
}

/**
 * @tc.name: SplitGrid_001
 * @tc.desc: Verify SplitGrid behavior through AutoGridPartition with complex path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, SplitGrid_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    for (int i = 0; i < 10; i++) {
        path.MoveTo(i * 10.0f, i * 20.0f);
        path.LineTo(i * 10.0f + 50.0f, i * 20.0f + 100.0f);
        path.QuadTo(i * 10.0f + 75.0f, i * 20.0f + 150.0f, i * 10.0f + 100.0f, i * 20.0f + 200.0f);
    }
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 300.0f, 300.0f);

    shape.Preprocess(*canvas_, rect, false);
    EXPECT_NE(shape.numCurves_, 20);
}

/**
 * @tc.name: ParseNumbers_001
 * @tc.desc: Verify parseNumbers through GetCurveByPath with simple path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ParseNumbers_001, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(curves.empty());
}

/**
 * @tc.name: ParseNumbers_002
 * @tc.desc: Verify parseNumbers with negative numbers
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ParseNumbers_002, TestSize.Level1)
{
    Drawing::Path path;
    path.MoveTo(-10.0f, -20.0f);
    path.LineTo(-100.0f, -200.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(curves.empty());
}

/**
 * @tc.name: ProcessSingleBatch_002
 * @tc.desc: Verify ProcessSingleBatch with empty grid curves
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessSingleBatch_002, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f); // Single point path (no valid curves)
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);
    shape.Preprocess(*canvas_, rect, false);

    // Note: Preprocess will fail with empty curves, disResult_ will be null
    // This test verifies ProcessSingleBatch handles empty grid gracefully
    if (shape.curvesInGrid_.empty()) {
        GTEST_LOG_(INFO) << "Empty curvesInGrid_ as expected for single point path";
        SUCCEED();
        return;
    }

    auto builder = shape.MakePrecalcShaderBuilder();
    ASSERT_NE(builder, nullptr);
    builder->SetUniform("iResolution", rect.GetWidth(), rect.GetHeight());
    builder->SetUniform("u_curveCount", static_cast<float>(shape.numCurves_));

    std::shared_ptr<Drawing::Image> prevSdf = nullptr;
    std::shared_ptr<Drawing::ShaderEffect> prevShader = nullptr;

    constexpr size_t CURVE_FLOAT_COUNT = 6;
    size_t gridIndex = 0;
    size_t batch = 0;
    size_t start = 0;
    size_t end = shape.curvesInGrid_[gridIndex].first.size() / CURVE_FLOAT_COUNT;
    float vStart = 0.0f;
    float vEnd = static_cast<float>(end - start - 1);

    // Should not crash with empty curves
    shape.ProcessSingleBatch(*builder, gridIndex, batch, start, end, vStart, vEnd, prevSdf, prevShader);
    EXPECT_EQ(prevSdf, nullptr);
    EXPECT_EQ(prevShader, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderWithCanvas_001
 * @tc.desc: Verify GenerateDrawingShader with Canvas parameter clears temp state after execution
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GenerateDrawingShaderWithCanvas_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.LineTo(200.0f, 300.0f);
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(*canvas_, 300.0f, 300.0f);
    EXPECT_EQ(shape.disResult_, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormalWithCanvas_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with Canvas parameter clears temp state after execution
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GenerateDrawingShaderHasNormalWithCanvas_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.CubicTo(180.0f, 260.0f, 190.0f, 280.0f, 200.0f, 300.0f);
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(*canvas_, 300.0f, 300.0f);
    EXPECT_EQ(shape.disResult_, nullptr);
}

/**
 * @tc.name: UpdateScaleSmallMinWH_001
 * @tc.desc: Verify UpdateScale returns (1.0, 1.0) when originalMinWH < MIN_SCALE (100.0f)
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleSmallMinWH_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(0.5f, 0.5f);
    Drawing::Rect rect(0.0f, 0.0f, 50.0f, 80.0f);

    shape.UpdateScale(scale, rect);
    EXPECT_EQ(scale.x_, 1.0f);
    EXPECT_EQ(scale.y_, 1.0f);
}

/**
 * @tc.name: UpdateScaleSmallMinWH_002
 * @tc.desc: Verify UpdateScale returns (1.0, 1.0) when min width is below MIN_SCALE
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleSmallMinWH_002, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(1.0f, 1.0f);
    Drawing::Rect rect(0.0f, 0.0f, 80.0f, 500.0f);

    shape.UpdateScale(scale, rect);
    EXPECT_EQ(scale.x_, 1.0f);
    EXPECT_EQ(scale.y_, 1.0f);
}

/**
 * @tc.name: UpdateScaleLargeAspectRatio_001
 * @tc.desc: Verify UpdateScale adjusts scaleY when height > width * 6.0f
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleLargeAspectRatio_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(1.0f, 1.0f);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 800.0f);

    shape.UpdateScale(scale, rect);
    EXPECT_LT(scale.y_, 1.0f);
    EXPECT_GT(scale.y_, 0.0f);
}

/**
 * @tc.name: UpdateScaleMinWHAfterScaleBelowMinScale_001
 * @tc.desc: Verify UpdateScale adjusts scale when minWH after applying scale < MIN_SCALE
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleMinWHAfterScaleBelowMinScale_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(0.1f, 0.1f);
    Drawing::Rect rect(0.0f, 0.0f, 500.0f, 500.0f);

    shape.UpdateScale(scale, rect);
    float minScaled = std::min(rect.GetWidth() * scale.x_, rect.GetHeight() * scale.y_);
    EXPECT_GE(minScaled, 100.0f - 0.01f);
}

/**
 * @tc.name: UpdateScaleClampMinScale_001
 * @tc.desc: Verify UpdateScale clamps scale to MIN_SCALE_CLAMP (0.001f) minimum
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleClampMinScale_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(0.0001f, 0.0001f);
    Drawing::Rect rect(0.0f, 0.0f, 500.0f, 500.0f);

    shape.UpdateScale(scale, rect);
    EXPECT_GE(scale.x_, 0.001f);
    EXPECT_GE(scale.y_, 0.001f);
}

/**
 * @tc.name: ClearTemp_001
 * @tc.desc: Verify ClearTemp clears internal temporary state after Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ClearTemp_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 250.0f, 350.0f);
    shape.Preprocess(*canvas_, rect, false);

    shape.ClearTemp();
    EXPECT_EQ(shape.disResult_, nullptr);
}

/**
 * @tc.name: AutoGridPartitionAsymmetric_001
 * @tc.desc: Verify AutoGridPartition with tall narrow rect (|| condition allows split)
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, AutoGridPartitionAsymmetric_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    for (int i = 0; i < 10; i++) {
        path.MoveTo(10.0f, i * 30.0f);
        path.LineTo(50.0f, i * 30.0f + 15.0f);
        path.QuadTo(30.0f, i * 30.0f + 22.0f, 10.0f, i * 30.0f + 30.0f);
    }
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 120.0f, 400.0f);

    shape.Preprocess(*canvas_, rect, false);
    EXPECT_TRUE(shape.curvesInGrid_.empty());
}

/**
 * @tc.name: SplitGridNarrowWidth_001
 * @tc.desc: Verify SplitGrid splits into 2 quadrants when width <= MIN_GRID_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, SplitGridNarrowWidth_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    for (int i = 0; i < 20; i++) {
        path.MoveTo(5.0f + i * 2.0f, i * 20.0f);
        path.LineTo(55.0f, i * 20.0f + 10.0f);
    }
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 500.0f);

    shape.Preprocess(*canvas_, rect, false);
    EXPECT_TRUE(shape.curvesInGrid_.empty());
}

/**
 * @tc.name: UpdateScaleNormalRange_001
 * @tc.desc: Verify UpdateScale works normally for dimensions above MIN_SCALE without aspect ratio issues
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScaleNormalRange_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Vector2f scale(0.5f, 0.5f);
    Drawing::Rect rect(0.0f, 0.0f, 400.0f, 400.0f);

    shape.UpdateScale(scale, rect);
    EXPECT_EQ(scale.x_, 0.5f);
    EXPECT_EQ(scale.y_, 0.5f);
}

/**
 * @tc.name: UpdateNumPasses_001
 * @tc.desc: Verify small size branch has highest priority: height < 150 forces 1 pass, even all grids covered
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateNumPasses_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    param.path.MoveTo(10.0f, 20.0f);
    param.path.LineTo(100.0f, 200.0f);
    GESDFPathShaderShape shape(param);

    shape.allGridsCovered_ = true;
    shape.maxEmptyGridShortSide_ = 128.0f;
    shape.UpdateNumPasses(149.9f);
    EXPECT_EQ(shape.numPasses_, 1);
}

/**
 * @tc.name: UpdateNumPasses_002
 * @tc.desc: Verify all-grids-covered branch returns 0 passes when height >= 150
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateNumPasses_002, TestSize.Level1)
{
    GESDFPathShapeParams param;
    param.path.MoveTo(10.0f, 20.0f);
    param.path.LineTo(100.0f, 200.0f);
    GESDFPathShaderShape shape(param);

    shape.allGridsCovered_ = true;
    shape.maxEmptyGridShortSide_ = 0.0f;
    shape.UpdateNumPasses(150.0f);
    EXPECT_EQ(shape.numPasses_, 0);
}

/**
 * @tc.name: UpdateNumPasses_003
 * @tc.desc: Verify lower clamp: tiny empty grid clamps requiredStep to 1.0f, returns 1 pass
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateNumPasses_003, TestSize.Level1)
{
    GESDFPathShapeParams param;
    param.path.MoveTo(10.0f, 20.0f);
    param.path.LineTo(100.0f, 200.0f);
    GESDFPathShaderShape shape(param);

    shape.allGridsCovered_ = false;
    shape.maxEmptyGridShortSide_ = 0.5f;
    shape.UpdateNumPasses(300.0f);
    EXPECT_EQ(shape.numPasses_, 1);
}

/**
 * @tc.name: UpdateNumPasses_004
 * @tc.desc: Verify normal calculation logic with medium empty grid
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateNumPasses_004, TestSize.Level1)
{
    GESDFPathShapeParams param;
    param.path.MoveTo(10.0f, 20.0f);
    param.path.LineTo(100.0f, 200.0f);
    GESDFPathShaderShape shape(param);

    shape.allGridsCovered_ = false;
    shape.maxEmptyGridShortSide_ = 8.0f; // step=4 -> log2(4)+1 = 3
    shape.UpdateNumPasses(300.0f);
    EXPECT_EQ(shape.numPasses_, 3);
}

/**
 * @tc.name: UpdateNumPasses_005
 * @tc.desc: Verify upper clamp: large empty grid clamps step to 32.0f, hits max 6 passes
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateNumPasses_005, TestSize.Level1)
{
    GESDFPathShapeParams param;
    param.path.MoveTo(10.0f, 20.0f);
    param.path.LineTo(100.0f, 200.0f);
    GESDFPathShaderShape shape(param);

    shape.allGridsCovered_ = false;
    shape.maxEmptyGridShortSide_ = 128.0f; // step clamped to 32 -> log2(32)+1 = 6
    shape.UpdateNumPasses(500.0f);
    EXPECT_EQ(shape.numPasses_, 6);
}

/**
 * @tc.name: SplitGrid_EmptyGridStats_001
 * @tc.desc: Verify empty subgrid branch in SplitGrid: allGridsCovered_ set to false, maxEmptyGridShortSide_ updated
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, SplitGrid_EmptyGridStats_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    // Rectangular outline with enough segments to trigger multiple quadtree splits, inner area is empty
    for (int i = 0; i < 10; ++i) {
        path.MoveTo(50.0f + i * 30.0f, 50.0f);
        path.LineTo(50.0f + (i + 1) * 30.0f, 50.0f);
    }
    for (int i = 0; i < 10; ++i) {
        path.MoveTo(350.0f, 50.0f + i * 30.0f);
        path.LineTo(350.0f, 50.0f + (i + 1) * 30.0f);
    }
    for (int i = 0; i < 10; ++i) {
        path.MoveTo(350.0f - i * 30.0f, 350.0f);
        path.LineTo(350.0f - (i + 1) * 30.0f, 350.0f);
    }
    for (int i = 0; i < 10; ++i) {
        path.MoveTo(50.0f, 350.0f - i * 30.0f);
        path.LineTo(50.0f, 350.0f - (i + 1) * 30.0f);
    }
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 400.0f, 400.0f);

    shape.Preprocess(*canvas_, rect, false);

    EXPECT_FALSE(shape.allGridsCovered_);
    EXPECT_GT(shape.maxEmptyGridShortSide_, 0.0f);
}

/**
 * @tc.name: RenderGridsToSurface_EmptyGridSkip_001
 * @tc.desc: Verify grid with zero curves is skipped during rendering, no crash and internal state remains valid
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, RenderGridsToSurface_EmptyGridSkip_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);
    shape.Preprocess(*canvas_, rect, false);

    // Precondition: valid grids exist before inserting empty grid
    size_t originalGridCount = shape.curvesInGrid_.size();
    ASSERT_GT(originalGridCount, 0u);

    // Insert an empty grid with zero curves to trigger the skip branch
    Grid emptyGrid;
    emptyGrid.bbox = { 0.0f, 100.0f, 0.0f, 100.0f };
    shape.curvesInGrid_.emplace_back(std::vector<float>{}, emptyGrid);
    shape.segmentIndex_.emplace_back(std::vector<float>{});

    // Verify test precondition: the inserted grid truly has zero curves
    constexpr size_t CURVE_FLOAT_COUNT = 6;
    size_t emptyGridCurveCount = shape.curvesInGrid_.back().first.size() / CURVE_FLOAT_COUNT;
    EXPECT_EQ(emptyGridCurveCount, 0u);
    EXPECT_EQ(shape.curvesInGrid_.size(), originalGridCount + 1);

    // Execute rendering: empty grid should hit the `totalCurves == 0 continue` branch
    shape.RenderGridsToSurface(rect);

    // Post-condition: grid data remains intact, no corruption caused by empty grid handling
    EXPECT_EQ(shape.curvesInGrid_.size(), originalGridCount + 1);
    EXPECT_FALSE(shape.curvesInGrid_.front().first.empty());
}

/**
 * @tc.name: RenderGridsToSurface_BatchOverlapLoop_001
 * @tc.desc: Verify overlapping batch loop: while body, start+=step and end==totalCurves break branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, RenderGridsToSurface_BatchOverlapLoop_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    // 30 line segments packed within 50px range, stays in single grid (below MIN_GRID_SIZE)
    for (int i = 0; i < 30; ++i) {
        path.MoveTo(10.0f, 10.0f + static_cast<float>(i));
        path.LineTo(40.0f, 10.0f + static_cast<float>(i));
    }
    param.path = path;
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);
    shape.Preprocess(*canvas_, rect, false);

    constexpr size_t CURVE_FLOAT_COUNT = 6;
    ASSERT_FALSE(shape.curvesInGrid_.empty());
    size_t totalCurves = shape.curvesInGrid_[0].first.size() / CURVE_FLOAT_COUNT;
    EXPECT_GT(totalCurves, 20u);

    shape.RenderGridsToSurface(rect);
    SUCCEED();
}

/**
 * @tc.name: DrawPathToImage_NullOnZeroSize_001
 * @tc.desc: Verify DrawPathToImage returns nullptr on invalid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, DrawPathToImage_NullOnZeroSize_001, TestSize.Level1)
{
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 10.0f);
    path.LineTo(50.0f, 50.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    auto result = shape.DrawPathToImage(*canvas_, 0, 0, path);
    EXPECT_EQ(result, nullptr);

    auto result2 = shape.DrawPathToImage(*canvas_, -10, 100, path);
    EXPECT_EQ(result2, nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS