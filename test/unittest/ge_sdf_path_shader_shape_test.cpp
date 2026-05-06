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

#include "ge_sdf_path_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFPathShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::ImageInfo imageInfo_ = {};
};

void GESDFPathShaderShapeTest::SetUpTestCase(void) {}
void GESDFPathShaderShapeTest::TearDownTestCase(void) {}

void GESDFPathShaderShapeTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 300.0f, 300.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    canvas_ = surface_->GetCanvas();
}

void GEParticleCircularHaloShaderTest::TearDown() {}

std::shared_ptr<Drawing::Surface> GEParticleCircularHaloShaderTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest::CreateSurface create gpuContext failed.";
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
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetPath_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    const auto& resultPath = shape.GetPath();
    EXPECT_TRUE(resultPath.IsValid());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetPath_001 end";
}

/**
 * @tc.name: GetOffset_001
 * @tc.desc: Verify GetOffset returns correct offset
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetOffset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetOffset_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    param.path = path;
    param.offset = Vector2f(15.0f, 25.0f);

    GESDFPathShaderShape shape(param);
    const auto& offset = shape.GetOffset();
    EXPECT_EQ(offset.x_, 15.0f);
    EXPECT_EQ(offset.y_, 25.0f);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetOffset_001 end";
}


/**
 * @tc.name: CopyState_001
 * @tc.desc: Verify CopyState correctly copies parameters
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, CopyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CopyState_001 start";
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
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CopyState_001 end";
}

/**
 * @tc.name: GetCurveByPath_001
 * @tc.desc: Verify GetCurveByPath with valid path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_001 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_001 end";
}

/**
 * @tc.name: GetCurveByPath_002
 * @tc.desc: Verify GetCurveByPath with invalid path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_002 start";
    Drawing::Path invalidPath;

    auto result = GESDFPathShaderShape::GetCurveByPath(invalidPath);
    EXPECT_TRUE(result.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_002 end";
}

/**
 * @tc.name: GetCurveByPath_003
 * @tc.desc: Verify GetCurveByPath with quadratic bezier curve
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_003 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.QuadTo(50.0f, 100.0f, 100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 3);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_003 end";
}

/**
 * @tc.name: GetCurveByPath_004
 * @tc.desc: Verify GetCurveByPath with cubic bezier curve
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_004 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.CubicTo(30.0f, 40.0f, 60.0f, 80.0f, 100.0f, 200.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 4);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_004 end";
}

/**
 * @tc.name: GetCurveByPath_005
 * @tc.desc: Verify GetCurveByPath with multiple segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GetCurveByPath_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_005 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.QuadTo(200.0f, 300.0f, 250.0f, 350.0f);

    auto result = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 3);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GetCurveByPath_005 end";
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader without preprocessing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GenerateDrawingShader_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal without preprocessing
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify Preprocess with invalid path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_001 start";
    GESDFPathShapeParams param;
    Drawing::Path invalidPath;
    param.path = invalidPath;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
    EXPECT_EQ(shape.numCurves_, 0);
    EXPECT_TRUE(shape.controlPoints_.empty());
    EXPECT_TRUE(shape.curvesInGrid_.empty());

    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_001 end";
}

/**
 * @tc.name: Preprocess_002
 * @tc.desc: Verify Preprocess with valid path and invalid canvas
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_002 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 1);
    EXPECT_EQ(shape.controlPoints_.size(), 6);
    EXPECT_FALSE(shape.curvesInGrid_.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_002 end";
}

/**
 * @tc.name: Preprocess_003
 * @tc.desc: Verify Preprocess with hasNormal parameter
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_003 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, true);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_003 end";
}

/**
 * @tc.name: Preprocess_004
 * @tc.desc: Verify Preprocess with scale parameter
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, Preprocess_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_004 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;
    param.scale = Vector2f(0.5f, 0.5f);
    param.offset = Vector2f(10.5f, 10.5f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest Preprocess_004 end";
}

/**
 * @tc.name: cubicToQuadraticSingle_001
 * @tc.desc: Verify cubicToQuadraticSingle with valid points
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, cubicToQuadraticSingle_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_001 start";
    Vector2f p0(0.0f, 0.0f);
    Vector2f p1(50.0f, 100.0f);
    Vector2f p2(100.0f, 150.0f);
    Vector2f p3(150.0f, 200.0f);

    Vector2f originalP1 = p1;
    GESDFPathShaderShape::cubicToQuadraticSingle(p0, p1, p2, p3);

    EXPECT_NE(p1.x_, originalP1.x_);
    EXPECT_NE(p1.y_, originalP1.y_);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_001 end";
}

/**
 * @tc.name: cubicToQuadraticSingle_002
 * @tc.desc: Verify cubicToQuadraticSingle with collinear points
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, cubicToQuadraticSingle_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_002 start";
    Vector2f p0(0.0f, 0.0f);
    Vector2f p1(25.0f, 25.0f);
    Vector2f p2(50.0f, 50.0f);
    Vector2f p3(75.0f, 75.0f);

    Vector2f originalP1 = p1;
    GESDFPathShaderShape::cubicToQuadraticSingle(p0, p1, p2, p3);

    EXPECT_TRUE(std::abs(p1.x_ - originalP1.x_) < 100.0f);
    EXPECT_TRUE(std::abs(p1.y_ - originalP1.y_) < 100.0f);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_002 end";
}

/**
 * @tc.name: cubicToQuadraticSingle_003
 * @tc.desc: Verify cubicToQuadraticSingle with different end points
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, cubicToQuadraticSingle_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_003 start";
    Vector2f p0(10.0f, 20.0f);
    Vector2f p1(30.0f, 60.0f);
    Vector2f p2(80.0f, 120.0f);
    Vector2f p3(200.0f, 300.0f);

    Vector2f originalP1 = p1;
    GESDFPathShaderShape::cubicToQuadraticSingle(p0, p1, p2, p3);

    EXPECT_NE(p1.x_, originalP1.x_);
    EXPECT_NE(p1.y_, originalP1.y_);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest cubicToQuadraticSingle_003 end";
}


/**
 * @tc.name: UpdateScale_001
 * @tc.desc: Verify UpdateScale behavior through Preprocess with small rect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, UpdateScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest UpdateScale_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;
    param.scale = Vector2f(0.5f, 0.5f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect smallRect(0.0f, 0.0f, 10.0f, 10.0f);

    shape.Preprocess(canvas_, smallRect, false);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest UpdateScale_001 end";
}


/**
 * @tc.name: AutoGridPartition_001
 * @tc.desc: Verify AutoGridPartition with valid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, AutoGridPartition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AutoGridPartition_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_FALSE(shape.curvesInGrid_.empty());
    EXPECT_FALSE(shape.segmentIndex_.empty());
    EXPECT_GT(shape.curvesInGrid_.size(), 0);
    EXPECT_EQ(shape.numCurves_, 2);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AutoGridPartition_001 end";
}

/**
 * @tc.name: AutoGridPartition_002
 * @tc.desc: Verify AutoGridPartition with invalid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, AutoGridPartition_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AutoGridPartition_002 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect invalidRect(0.0f, 0.0f, 1.0f, 1.0f);

    shape.Preprocess(canvas_, invalidRect, false);
    EXPECT_EQ(shape.numCurves_, 0);
    EXPECT_TRUE(shape.controlPoints_.empty());
    EXPECT_TRUE(shape.curvesInGrid_.empty());
    EXPECT_TRUE(shape.segmentIndex_.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AutoGridPartition_002 end";
}

/**
 * @tc.name: ProcessCurveSegments_001
 * @tc.desc: Verify ProcessCurveSegments with line segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessCurveSegments_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_001 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_EQ(curves.size(), 2);
    EXPECT_EQ(curves[0].size(), 2);
    EXPECT_EQ(curves[1].size(), 2);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_001 end";
}

/**
 * @tc.name: ProcessCurveSegments_002
 * @tc.desc: Verify ProcessCurveSegments with quadratic bezier segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessCurveSegments_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_002 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.QuadTo(50.0f, 100.0f, 100.0f, 200.0f);
    path.QuadTo(150.0f, 250.0f, 200.0f, 300.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_EQ(curves.size(), 2);
    EXPECT_EQ(curves[0].size(), 3);
    EXPECT_EQ(curves[1].size(), 3);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_002 end";
}

/**
 * @tc.name: ProcessCurveSegments_003
 * @tc.desc: Verify ProcessCurveSegments with cubic bezier segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessCurveSegments_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_003 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.CubicTo(30.0f, 40.0f, 60.0f, 80.0f, 100.0f, 200.0f);
    path.CubicTo(120.0f, 220.0f, 150.0f, 280.0f, 200.0f, 300.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_EQ(curves.size(), 2);
    EXPECT_EQ(curves[0].size(), 4);
    EXPECT_EQ(curves[1].size(), 4);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_003 end";
}

/**
 * @tc.name: ProcessCurveSegments_004
 * @tc.desc: Verify ProcessCurveSegments with mixed segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessCurveSegments_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_004 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(50.0f, 100.0f);
    path.QuadTo(80.0f, 150.0f, 100.0f, 200.0f);
    path.CubicTo(120.0f, 220.0f, 150.0f, 280.0f, 200.0f, 300.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_EQ(curves.size(), 3);
    EXPECT_EQ(curves[0].size(), 2);
    EXPECT_EQ(curves[1].size(), 3);
    EXPECT_EQ(curves[2].size(), 4);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessCurveSegments_004 end";
}

/**
 * @tc.name: CreateSurfaceAndCanvas_001
 * @tc.desc: Verify CreateSurfaceAndCanvas with valid rect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, CreateSurfaceAndCanvas_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CreateSurfaceAndCanvas_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect validRect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_,validRect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CreateSurfaceAndCanvas_001 end";
}

/**
 * @tc.name: CreateSurfaceAndCanvas_002
 * @tc.desc: Verify CreateSurfaceAndCanvas with invalid rect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, CreateSurfaceAndCanvas_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CreateSurfaceAndCanvas_002 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect invalidRect(0.0f, 0.0f, -10.0f, -10.0f);

    shape.Preprocess(canvas_, invalidRect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest CreateSurfaceAndCanvas_002 end";
}


/**
 * @tc.name: RunSDFPropagation_001
 * @tc.desc: Verify RunSDFPropagation behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, RunSDFPropagation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest RunSDFPropagation_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.LineTo(200.0f, 300.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest RunSDFPropagation_001 end";
}

/**
 * @tc.name: ComputeDistanceField_001
 * @tc.desc: Verify ComputeDistanceField behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComputeDistanceField_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeDistanceField_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 150.0f, 150.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeDistanceField_001 end";
}

/**
 * @tc.name: ComputeCurveBoundingBox_001
 * @tc.desc: Verify ComputeCurveBoundingBox behavior through AutoGridPartition
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComputeCurveBoundingBox_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_001 end";
}

/**
 * @tc.name: ComputeCurveBoundingBox_002
 * @tc.desc: Verify ComputeCurveBoundingBox with zero width
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComputeCurveBoundingBox_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_002 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect zeroWidthRect(0.0f, 0.0f, 0.0f, 100.0f);

    shape.Preprocess(canvas_, zeroWidthRect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_002 end";
}

/**
 * @tc.name: ComputeCurveBoundingBox_003
 * @tc.desc: Verify ComputeCurveBoundingBox with zero height
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComputeCurveBoundingBox_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_003 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect zeroHeightRect(0.0f, 0.0f, 100.0f, 0.0f);

    shape.Preprocess(canvas_, zeroHeightRect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeCurveBoundingBox_003 end";
}

/**
 * @tc.name: RenderGridsToSurface_001
 * @tc.desc: Verify RenderGridsToSurface behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, RenderGridsToSurface_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest RenderGridsToSurface_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest RenderGridsToSurface_001 end";
}

/**
 * @tc.name: SplitGrid_001
 * @tc.desc: Verify SplitGrid behavior through AutoGridPartition with complex path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, SplitGrid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest SplitGrid_001 start";
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

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 20);
    EXPECT_EQ(shape.controlPoints_.size(), 120);
    EXPECT_GT(shape.curvesInGrid_.size(), 1);
    EXPECT_GT(shape.segmentIndex_.size(), 1);
    EXPECT_EQ(shape.numPasses_, 6);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest SplitGrid_001 end";
}

/**
 * @tc.name: InitializeWorkQueue_001
 * @tc.desc: Verify InitializeWorkQueue behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, InitializeWorkQueue_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest InitializeWorkQueue_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 150.0f, 150.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest InitializeWorkQueue_001 end";
}

/**
 * @tc.name: ProcessFinalGrid_001
 * @tc.desc: Verify ProcessFinalGrid behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ProcessFinalGrid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessFinalGrid_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ProcessFinalGrid_001 end";
}

/**
 * @tc.name: PreparePathForRendering_001
 * @tc.desc: Verify PreparePathForRendering behavior through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, PreparePathForRendering_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest PreparePathForRendering_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;
    param.offset = Vector2f(10.0f, 10.0f);
    param.scale = Vector2f(1.0f, 1.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest PreparePathForRendering_001 end";
}

/**
 * @tc.name: ComplexPath_001
 * @tc.desc: Verify handling of complex path with multiple curves
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComplexPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComplexPath_001 start";
    GESDFPathShapeParams param;
    Drawing::Path complexPath;
    complexPath.MoveTo(10.0f, 20.0f);
    complexPath.LineTo(50.0f, 100.0f);
    complexPath.QuadTo(75.0f, 150.0f, 100.0f, 200.0f);
    complexPath.CubicTo(120.0f, 220.0f, 150.0f, 280.0f, 200.0f, 300.0f);
    complexPath.LineTo(250.0f, 350.0f);
    param.path = complexPath;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 300.0f, 300.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 4);
    EXPECT_EQ(shape.controlPoints_.size(), 24);
    EXPECT_GT(shape.curvesInGrid_.size(), 0);
    EXPECT_GT(shape.segmentIndex_.size(), 0);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComplexPath_001 end";
}

/**
 * @tc.name: LargeScale_001
 * @tc.desc: Verify handling with large scale factor
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, LargeScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest LargeScale_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;
    param.scale = Vector2f(2.0f, 2.0f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest LargeScale_001 end";
}

/**
 * @tc.name: SmallScale_001
 * @tc.desc: Verify handling with small scale factor
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, SmallScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest SmallScale_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;
    param.scale = Vector2f(0.001f, 0.001f);

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest SmallScale_001 end";
}

/**
 * @tc.name: ClosedPath_001
 * @tc.desc: Verify handling of closed path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ClosedPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ClosedPath_001 start";
    GESDFPathShapeParams param;
    Drawing::Path closedPath;
    closedPath.MoveTo(10.0f, 20.0f);
    closedPath.LineTo(100.0f, 20.0f);
    closedPath.LineTo(100.0f, 200.0f);
    closedPath.LineTo(10.0f, 200.0f);
    closedPath.Close();
    param.path = closedPath;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 150.0f, 150.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 4);
    EXPECT_EQ(shape.controlPoints_.size(), 24);
    EXPECT_FALSE(shape.curvesInGrid_.empty());
    EXPECT_FALSE(shape.segmentIndex_.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ClosedPath_001 end";
}

/**
 * @tc.name: EmptySegments_001
 * @tc.desc: Verify handling of path with empty segments
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, EmptySegments_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest EmptySegments_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 0);
    EXPECT_TRUE(shape.controlPoints_.empty());
    EXPECT_TRUE(shape.curvesInGrid_.empty());
    EXPECT_EQ(shape.disResult_, nullptr);

    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest EmptySegments_001 end";
}

/**
 * @tc.name: VerySmallRect_001
 * @tc.desc: Verify handling with very small rect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, VerySmallRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest VerySmallRect_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect smallRect(0.0f, 0.0f, 50.0f, 50.0f);

    shape.Preprocess(canvas_, smallRect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest VerySmallRect_001 end";
}

/**
 * @tc.name: VeryLargeRect_001
 * @tc.desc: Verify handling with very large rect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, VeryLargeRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest VeryLargeRect_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect largeRect(0.0f, 0.0f, 1000.0f, 1000.0f);

    shape.Preprocess(canvas_, largeRect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest VeryLargeRect_001 end";
}

/**
 * @tc.name: AsymmetricRect_001
 * @tc.desc: Verify handling with asymmetric rect dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, AsymmetricRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AsymmetricRect_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect asymmetricRect(0.0f, 0.0f, 200.0f, 100.0f);

    shape.Preprocess(canvas_, asymmetricRect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest AsymmetricRect_001 end";
}

/**
 * @tc.name: MultipleQuadraticBeziers_001
 * @tc.desc: Verify handling of multiple quadratic bezier curves
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, MultipleQuadraticBeziers_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest MultipleQuadraticBeziers_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.QuadTo(30.0f, 50.0f, 50.0f, 80.0f);
    path.QuadTo(70.0f, 110.0f, 90.0f, 140.0f);
    path.QuadTo(110.0f, 170.0f, 130.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 150.0f, 150.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 3);
    EXPECT_EQ(shape.controlPoints_.size(), 18);
    EXPECT_FALSE(shape.curvesInGrid_.empty());
    EXPECT_FALSE(shape.segmentIndex_.empty());
    EXPECT_EQ(shape.numPasses_, 6);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest MultipleQuadraticBeziers_001 end";
}

/**
 * @tc.name: MultipleCubicBeziers_001
 * @tc.desc: Verify handling of multiple cubic bezier curves
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, MultipleCubicBeziers_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest MultipleCubicBeziers_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.CubicTo(20.0f, 30.0f, 40.0f, 60.0f, 50.0f, 80.0f);
    path.CubicTo(60.0f, 90.0f, 80.0f, 120.0f, 90.0f, 140.0f);
    path.CubicTo(100.0f, 150.0f, 120.0f, 180.0f, 130.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 150.0f, 150.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_EQ(shape.numCurves_, 3);
    EXPECT_EQ(shape.controlPoints_.size(), 18);
    EXPECT_FALSE(shape.curvesInGrid_.empty());
    EXPECT_FALSE(shape.segmentIndex_.empty());
    EXPECT_EQ(shape.distanceCalc_, true);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest MultipleCubicBeziers_001 end";
}

/**
 * @tc.name: ParseNumbers_001
 * @tc.desc: Verify parseNumbers through GetCurveByPath with simple path
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ParseNumbers_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_001 start";
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(curves.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_001 end";
}

/**
 * @tc.name: ParseNumbers_002
 * @tc.desc: Verify parseNumbers with negative numbers
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ParseNumbers_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_002 start";
    Drawing::Path path;
    path.MoveTo(-10.0f, -20.0f);
    path.LineTo(-100.0f, -200.0f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(curves.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_002 end";
}

/**
 * @tc.name: ParseNumbers_003
 * @tc.desc: Verify parseNumbers with decimal numbers
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ParseNumbers_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_003 start";
    Drawing::Path path;
    path.MoveTo(10.5f, 20.25f);
    path.LineTo(100.75f, 200.125f);

    auto curves = GESDFPathShaderShape::GetCurveByPath(path);
    EXPECT_FALSE(curves.empty());
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ParseNumbers_003 end";
}

/**
 * @tc.name: IntersectBBox_001
 * @tc.desc: Verify IntersectBBox through SplitGrid behavior
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, IntersectBBox_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest IntersectBBox_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.LineTo(150.0f, 250.0f);
    path.LineTo(200.0f, 300.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 250.0f, 350.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest IntersectBBox_001 end";
}

/**
 * @tc.name: ConvertPixelToNDC_001
 * @tc.desc: Verify ConvertPixelToNDC through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ConvertPixelToNDC_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPixelToNDC_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(50.0f, 100.0f);
    path.LineTo(150.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPixelToNDC_001 end";
}

/**
 * @tc.name: ConvertPixelToNDC_002
 * @tc.desc: Verify ConvertPixelToNDC with invalid base dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ConvertPixelToNDC_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPixelToNDC_002 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(50.0f, 100.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect invalidRect(0.0f, 0.0f, 0.5f, 0.5f);

    shape.Preprocess(canvas_, invalidRect, false);
    EXPECT_EQ(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPixelToNDC_002 end";
}

/**
 * @tc.name: ConvertPointsTo_001
 * @tc.desc: Verify ConvertPointsTo through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ConvertPointsTo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPointsTo_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(50.0f, 100.0f);
    path.LineTo(150.0f, 200.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 200.0f, 200.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ConvertPointsTo_001 end";
}

/**
 * @tc.name: ComputeAllCurveBoundingBoxes_001
 * @tc.desc: Verify ComputeAllCurveBoundingBoxes through Preprocess
 * @tc.type: FUNC
 */
HWTEST_F(GESDFPathShaderShapeTest, ComputeAllCurveBoundingBoxes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeAllCurveBoundingBoxes_001 start";
    GESDFPathShapeParams param;
    Drawing::Path path;
    path.MoveTo(10.0f, 20.0f);
    path.LineTo(100.0f, 200.0f);
    path.QuadTo(150.0f, 250.0f, 200.0f, 300.0f);
    param.path = path;

    GESDFPathShaderShape shape(param);
    Drawing::Rect rect(0.0f, 0.0f, 250.0f, 350.0f);

    shape.Preprocess(canvas_, rect, false);
    EXPECT_NE(shape.disResult_, nullptr);
    GTEST_LOG_(INFO) << "GESDFPathShaderShapeTest ComputeAllCurveBoundingBoxes_001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS