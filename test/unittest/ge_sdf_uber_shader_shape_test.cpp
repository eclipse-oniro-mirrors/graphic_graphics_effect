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

#include <memory>
#include <vector>

#include "draw/color.h"
#include "ge_sdf_ellipse_shader_shape.h"
#include "ge_sdf_pixelmap_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_transform_shader_shape.h"
#include "ge_sdf_triangle_shader_shape.h"
#include "ge_sdf_uber_basic_shape.h"
#include "ge_sdf_uber_shader_shape.h"
#include "ge_sdf_union_op_shader_shape.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr float RECT_SIDE = 100.0f;
constexpr float SMALL_RADIUS = 10.0f;
constexpr float UNIFORM_RADIUS = 40.0f;
constexpr float CAPSULE_RADIUS = 60.0f;
constexpr float IMAGE_SIDE = 50.0f;
constexpr int WIDE_IMAGE_WIDTH = 2048;
constexpr int WIDE_IMAGE_HEIGHT = 2;
constexpr int TALL_IMAGE_SIDE = 1100;
constexpr int GRAVITY_PULL_UNION_MODE = 1;

std::shared_ptr<GESDFShaderShape> MakeRRect(float width, float height, float radius)
{
    GESDFRRectShapeParams param;
    param.rrect.width_ = width;
    param.rrect.height_ = height;
    param.rrect.SetCornerRadius(radius, radius);
    return std::make_shared<GESDFRRectShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeUnuniformRRect()
{
    GESDFRRectShapeParams param;
    param.rrect.width_ = RECT_SIDE;
    param.rrect.height_ = RECT_SIDE;
    param.rrect.SetCornerRadius(Vector4f { 10.0f, 20.0f, 30.0f, 40.0f });
    return std::make_shared<GESDFRRectShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeTriangleWithRadius(float size, float radius)
{
    GESDFTriangleShapeParams param;
    param.vertex0 = Vector2f(0.0f, 0.0f);
    param.vertex1 = Vector2f(size, 0.0f);
    param.vertex2 = Vector2f(0.0f, size);
    param.radius = radius;
    return std::make_shared<GESDFTriangleShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeTriangle(float size)
{
    return MakeTriangleWithRadius(size, 2.0f);
}

std::shared_ptr<GESDFShaderShape> MakeEllipse(float width, float height)
{
    GESDFEllipseShapeParams param;
    param.center = Vector2f(width * 0.5f, height * 0.5f);
    param.width = width;
    param.height = height;
    return std::make_shared<GESDFEllipseShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakePixelmap(const std::shared_ptr<Drawing::Image>& image)
{
    GESDFPixelmapShapeParams param;
    param.image = image;
    return std::make_shared<GESDFPixelmapShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeUnion(const std::shared_ptr<GESDFShaderShape>& left,
    const std::shared_ptr<GESDFShaderShape>& right, GESDFUnionOp op = GESDFUnionOp::UNION, float spacing = 0.0f)
{
    GESDFUnionOpShapeParams param;
    param.left = left;
    param.right = right;
    param.op = op;
    param.spacing = spacing;
    return std::make_shared<GESDFUnionOpShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeTransform(const std::shared_ptr<GESDFShaderShape>& shape,
    const Drawing::Matrix& matrix, int unionMode = 0)
{
    GESDFTransformShapeParams param;
    param.shape = shape;
    param.matrix = matrix;
    param.unionMode = unionMode;
    return std::make_shared<GESDFTransformShaderShape>(param);
}

std::shared_ptr<GESDFShaderShape> MakeUnionTree(const std::vector<std::shared_ptr<GESDFShaderShape>>& leaves,
    GESDFUnionOp op = GESDFUnionOp::UNION, float spacing = 0.0f)
{
    auto root = leaves.back();
    for (int index = static_cast<int>(leaves.size()) - 2; index >= 0; --index) {
        root = MakeUnion(leaves[index], root, op, spacing);
    }
    return root;
}

std::shared_ptr<Drawing::Image> MakeImage(int width, int height)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(width, height, format);
    bitmap.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bitmap.MakeImage();
}

std::vector<std::shared_ptr<GESDFShaderShape>> MakeRRectLeaves(int count, bool ununiform)
{
    std::vector<std::shared_ptr<GESDFShaderShape>> leaves;
    leaves.reserve(count);
    for (int index = 0; index < count; ++index) {
        leaves.push_back(ununiform ? MakeUnuniformRRect() : MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS));
    }
    return leaves;
}

std::vector<std::shared_ptr<GESDFShaderShape>> MakeMixedBasicLeaves()
{
    return {
        MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),       // G2
        MakeRRect(RECT_SIDE, RECT_SIDE, UNIFORM_RADIUS),     // G1_UNIFORM
        MakeRRect(RECT_SIDE, RECT_SIDE, CAPSULE_RADIUS),     // V capsule
        MakeRRect(RECT_SIDE * 2, RECT_SIDE, CAPSULE_RADIUS), // H capsule
        MakeUnuniformRRect(),
        MakeTriangle(RECT_SIDE),
        MakeEllipse(RECT_SIDE, RECT_SIDE * 0.8f),
    };
}

std::vector<std::shared_ptr<GESDFShaderShape>> MakePixelmapLeaves(int count, int width, int height)
{
    std::vector<std::shared_ptr<GESDFShaderShape>> leaves;
    leaves.reserve(count);
    for (int index = 0; index < count; ++index) {
        leaves.push_back(MakePixelmap(MakeImage(width, height)));
    }
    return leaves;
}

GESdfUberShaderShapeParams MakeUberParams(const std::shared_ptr<GESDFShaderShape>& root, GESDFUberType uberType)
{
    GESdfUberShaderShapeParams params;
    params.rootShape = root;
    params.uberType = static_cast<int>(uberType);
    return params;
}

std::shared_ptr<GESDFShaderShape> MakeMismatchedOpTree()
{
    return MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),
        MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),
            MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUnionOp::SMOOTH_UNION, 0.0f));
}

std::shared_ptr<GESDFShaderShape> MakeSpacingMismatchTree()
{
    return MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),
        MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),
            MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUnionOp::UNION, 5.0f));
}

std::shared_ptr<GESDFShaderShape> MakePerspectiveTree()
{
    Drawing::Matrix matrix;
    matrix.Set(Drawing::Matrix::Index::PERSP_0, 0.5f);
    return MakeTransform(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), matrix);
}
} // namespace

class GESDFUberShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESDFUberShaderShapeTest::SetUpTestCase(void) {}
void GESDFUberShaderShapeTest::TearDownTestCase(void) {}
void GESDFUberShaderShapeTest::SetUp() {}
void GESDFUberShaderShapeTest::TearDown() {}

/**
 * @tc.name: WalkUberLeaves_001
 * @tc.desc: Verify leaf collection across null / leaf / union branches
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, WalkUberLeaves_001, TestSize.Level1)
{
    GESDFUberTreeDesc desc;
    std::vector<GESDFUberLeafPos> leaves;
    EXPECT_TRUE(GESdfUberShaderShape::WalkUberLeaves(nullptr, Drawing::Matrix(), desc, leaves));
    EXPECT_EQ(desc.leafCount, 0);

    EXPECT_TRUE(GESdfUberShaderShape::WalkUberLeaves(
        MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), Drawing::Matrix(), desc, leaves));
    EXPECT_EQ(desc.leafCount, 1);
    EXPECT_EQ(leaves.size(), 1u);

    desc = GESDFUberTreeDesc();
    leaves.clear();
    auto smoothTree = MakeUnionTree(MakeMixedBasicLeaves(), GESDFUnionOp::SMOOTH_UNION, 20.0f);
    ASSERT_TRUE(GESdfUberShaderShape::WalkUberLeaves(smoothTree, Drawing::Matrix(), desc, leaves));
    EXPECT_TRUE(desc.opResolved);
    EXPECT_TRUE(desc.isSmoothUnion);
    EXPECT_FLOAT_EQ(desc.spacing, 20.0f);
    EXPECT_EQ(static_cast<int>(leaves.size()), BASIC_SHAPE_LEAF_MIN);

    desc = GESDFUberTreeDesc();
    EXPECT_FALSE(GESdfUberShaderShape::WalkUberLeaves(
        MakeMismatchedOpTree(), Drawing::Matrix(), desc, leaves));
    desc = GESDFUberTreeDesc();
    EXPECT_FALSE(GESdfUberShaderShape::WalkUberLeaves(
        MakeSpacingMismatchTree(), Drawing::Matrix(), desc, leaves));

    // A null union slot keeps the pass-through semantics and exercises the null-shape branch.
    desc = GESDFUberTreeDesc();
    leaves.clear();
    auto nullSlotTree = MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), nullptr);
    EXPECT_TRUE(GESdfUberShaderShape::WalkUberLeaves(nullSlotTree, Drawing::Matrix(), desc, leaves));
    EXPECT_EQ(desc.leafCount, 1);
    EXPECT_EQ(leaves.size(), 1u);

    // The union recursion short-circuits when the left subtree already fails.
    desc = GESDFUberTreeDesc();
    auto leftFailureTree = MakeUnion(
        MakeUnion(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS),
            MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUnionOp::SMOOTH_UNION, 0.0f),
        MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUnionOp::UNION, 0.0f);
    EXPECT_FALSE(GESdfUberShaderShape::WalkUberLeaves(leftFailureTree, Drawing::Matrix(), desc, leaves));
}

/**
 * @tc.name: WalkUberLeaves_002
 * @tc.desc: Verify the transform branch of leaf collection
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, WalkUberLeaves_002, TestSize.Level1)
{
    GESDFUberTreeDesc desc;
    std::vector<GESDFUberLeafPos> leaves;
    auto identityTransform = MakeTransform(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), Drawing::Matrix());
    ASSERT_TRUE(GESdfUberShaderShape::WalkUberLeaves(identityTransform, Drawing::Matrix(), desc, leaves));
    EXPECT_EQ(desc.leafCount, 1);

    Drawing::Matrix translate;
    translate.Translate(10.0f, 20.0f);
    ASSERT_TRUE(GESdfUberShaderShape::WalkUberLeaves(
        MakeTransform(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), translate), Drawing::Matrix(), desc, leaves));
    EXPECT_FALSE(leaves.back().parentToLocal.IsIdentity());

    EXPECT_FALSE(GESdfUberShaderShape::WalkUberLeaves(
        MakeTransform(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), Drawing::Matrix(), GRAVITY_PULL_UNION_MODE),
        Drawing::Matrix(), desc, leaves));

    Drawing::Matrix singular;
    singular.Scale(0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_FALSE(GESdfUberShaderShape::WalkUberLeaves(
        MakeTransform(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), singular), Drawing::Matrix(), desc, leaves));
}

/**
 * @tc.name: ClassifyUberRRect_001
 * @tc.desc: Verify every RRect uber kind and the non-uniform classification
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, ClassifyUberRRect_001, TestSize.Level1)
{
    GERRect rect;
    rect.width_ = RECT_SIDE;
    rect.height_ = RECT_SIDE;

    rect.SetCornerRadius(SMALL_RADIUS, SMALL_RADIUS);
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::G2_RRECT);

    rect.SetCornerRadius(UNIFORM_RADIUS, UNIFORM_RADIUS);
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::G1_UNIFORM_RRECT);

    rect.SetCornerRadius(CAPSULE_RADIUS, CAPSULE_RADIUS);
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::V_CAPSULE);

    rect.width_ = RECT_SIDE * 2;
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::H_CAPSULE);

    rect.width_ = RECT_SIDE;
    rect.SetCornerRadius(Vector4f { 10.0f, 20.0f, 30.0f, 40.0f });
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::G1_UNUNIFORM_RRECT);

    rect.radius_[GERRect::TOP_LEFT] = Vector2f(10.0f, 20.0f);
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::G1_UNUNIFORM_RRECT);

    // Uniform corners with an elliptical (non-circular) radius also fall back to the general path.
    rect.SetCornerRadius(10.0f, 20.0f);
    EXPECT_EQ(GESdfUberShaderShape::ClassifyUberRRect(rect), GEUberBasicShapeType::G1_UNUNIFORM_RRECT);
}

/**
 * @tc.name: AffineAndLeafRange_001
 * @tc.desc: Verify the affine guard and the leaf-count gate for both uber kinds
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, AffineAndLeafRange_001, TestSize.Level1)
{
    EXPECT_TRUE(GESdfUberShaderShape::IsAffineTransform(Drawing::Matrix()));
    Drawing::Matrix perspective;
    perspective.Set(Drawing::Matrix::Index::PERSP_0, 0.5f);
    EXPECT_FALSE(GESdfUberShaderShape::IsAffineTransform(perspective));
    perspective = Drawing::Matrix();
    perspective.Set(Drawing::Matrix::Index::PERSP_1, 0.5f);
    EXPECT_FALSE(GESdfUberShaderShape::IsAffineTransform(perspective));
    perspective = Drawing::Matrix();
    perspective.Set(Drawing::Matrix::Index::PERSP_2, 1.5f);
    EXPECT_FALSE(GESdfUberShaderShape::IsAffineTransform(perspective));

    EXPECT_FALSE(GESdfUberBasicShapeShape::IsLeafCountInRange(BASIC_SHAPE_LEAF_MIN - 1, BASIC_SHAPE_LEAF_MAX,
        BASIC_SHAPE_LEAF_MIN));
    EXPECT_TRUE(GESdfUberBasicShapeShape::IsLeafCountInRange(BASIC_SHAPE_LEAF_MIN, BASIC_SHAPE_LEAF_MAX,
        BASIC_SHAPE_LEAF_MIN));
    EXPECT_FALSE(GESdfUberBasicShapeShape::IsLeafCountInRange(BASIC_SHAPE_LEAF_MAX + 1, BASIC_SHAPE_LEAF_MAX,
        BASIC_SHAPE_LEAF_MIN));
    EXPECT_FALSE(GESdfUberBasicShapeShape::IsLeafCountInRange(BASIC_SHAPE_LEAF_MIN - 1, BASIC_SHAPE_LEAF_MAX));

    EXPECT_FALSE(GESdfUberPixelmapAtlasShape::IsLeafCountInRange(PIXELMAP_UBER_LEAF_MIN - 1, PIXELMAP_UBER_LEAF_MAX,
        PIXELMAP_UBER_LEAF_MIN));
    EXPECT_TRUE(GESdfUberPixelmapAtlasShape::IsLeafCountInRange(PIXELMAP_UBER_LEAF_MIN, PIXELMAP_UBER_LEAF_MAX,
        PIXELMAP_UBER_LEAF_MIN));
    EXPECT_FALSE(GESdfUberPixelmapAtlasShape::IsLeafCountInRange(PIXELMAP_UBER_LEAF_MAX + 1, PIXELMAP_UBER_LEAF_MAX,
        PIXELMAP_UBER_LEAF_MIN));
}

/**
 * @tc.name: BaseUberShape_001
 * @tc.desc: Verify the base uber classification, transparent fallback and state copy
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, BaseUberShape_001, TestSize.Level1)
{
    GESdfUberShaderShapeParams params = MakeUberParams(
        MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUberType::NONE);
    GESdfUberShaderShape shape(params);
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::UBER);
    EXPECT_EQ(shape.GetUberType(), GESDFUberType::NONE);
    EXPECT_EQ(shape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    EXPECT_NE(shape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_NE(shape.GenerateDrawingShaderHasNormal(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_TRUE(shape.HasType(GESDFShapeType::RRECT));
    EXPECT_FALSE(shape.HasType(GESDFShapeType::ELLIPSE));

    GESdfUberShaderShapeParams nullParams;
    GESdfUberShaderShape nullShape(nullParams);
    EXPECT_EQ(nullShape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_EQ(nullShape.GenerateDrawingShaderHasNormal(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_FALSE(nullShape.HasType(GESDFShapeType::RRECT));

    GESdfUberShaderShapeParams copyParams;
    GESdfUberShaderShape copyShape(copyParams);
    copyShape.CopyState(shape);
    EXPECT_EQ(copyShape.GetRootShape(), shape.GetRootShape());
}

/**
 * @tc.name: CopyUberState_ThroughBaseDispatcher
 * @tc.desc: Verify GESDFShaderShape::CopyState dispatches the UBER branch
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, CopyUberState_ThroughBaseDispatcher, TestSize.Level1)
{
    GESdfUberShaderShape src(MakeUberParams(
        MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), GESDFUberType::BASIC_SHAPE_UBER));
    GESdfUberShaderShapeParams dstParams;
    GESdfUberShaderShape dst(dstParams);
 
    GESDFShaderShape& baseDst = dst;
    const GESDFShaderShape& baseSrc = src;
    baseDst.CopyState(baseSrc);
 
    EXPECT_EQ(dst.GetRootShape(), src.GetRootShape());
    EXPECT_EQ(dst.GetUberType(), src.GetUberType());
}

/**
 * @tc.name: BasicUberShape_001
 * @tc.desc: Verify the basic-shape uber collapse for smooth/plain trees and its leaf collection
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, BasicUberShape_001, TestSize.Level1)
{
    auto leaves = MakeMixedBasicLeaves();
    auto root = MakeUnionTree(leaves);
    GESdfUberBasicShapeShape shape(MakeUberParams(root, GESDFUberType::BASIC_SHAPE_UBER));

    std::vector<GESDFBasicShapeUberLeafInfo> infos;
    GESDFUberTreeDesc desc;
    ASSERT_TRUE(shape.CollectLeaves(root, infos, desc));
    EXPECT_EQ(static_cast<int>(infos.size()), BASIC_SHAPE_LEAF_MIN);
    EXPECT_EQ(infos[0].shapeType, GEUberBasicShapeType::G2_RRECT);
    EXPECT_EQ(infos[1].shapeType, GEUberBasicShapeType::G1_UNIFORM_RRECT);
    EXPECT_EQ(infos[2].shapeType, GEUberBasicShapeType::V_CAPSULE);
    EXPECT_EQ(infos[4].shapeType, GEUberBasicShapeType::G1_UNUNIFORM_RRECT);
    EXPECT_EQ(infos[5].shapeType, GEUberBasicShapeType::TRIANGLE);
    EXPECT_EQ(infos[6].shapeType, GEUberBasicShapeType::ELLIPSE);

    auto builder = GESdfUberBasicShapeShape::GetBuilder(false, desc.isSmoothUnion);
    ASSERT_NE(builder, nullptr);
    shape.PackUniforms(builder, infos, desc);
    desc.isSmoothUnion = true;
    desc.spacing = 0.0f;
    shape.PackUniforms(builder, infos, desc);

    // Empty packing skips the loop; a positive smooth spacing takes the other std::max edge.
    std::vector<GESDFBasicShapeUberLeafInfo> emptyInfos;
    desc.spacing = 5.0f;
    shape.PackUniforms(builder, emptyInfos, desc);

    EXPECT_NE(shape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_NE(shape.GenerateDrawingShaderHasNormal(RECT_SIDE, RECT_SIDE), nullptr);

    auto smoothRoot = MakeUnionTree(MakeMixedBasicLeaves(), GESDFUnionOp::SMOOTH_UNION, 20.0f);
    GESdfUberBasicShapeShape smoothShape(MakeUberParams(smoothRoot, GESDFUberType::BASIC_SHAPE_UBER));
    EXPECT_NE(smoothShape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);

    // Negative radius, tall rect and negative triangle radius exercise the CPU pack clamps.
    auto clampLeaves = MakeMixedBasicLeaves();
    clampLeaves.push_back(MakeRRect(RECT_SIDE, RECT_SIDE, -5.0f));
    clampLeaves.push_back(MakeRRect(RECT_SIDE * 0.5f, RECT_SIDE, SMALL_RADIUS));
    clampLeaves.push_back(MakeTriangleWithRadius(RECT_SIDE, -1.0f));
    std::vector<GESDFBasicShapeUberLeafInfo> clampInfos;
    GESDFUberTreeDesc clampDesc;
    ASSERT_TRUE(shape.CollectLeaves(MakeUnionTree(clampLeaves), clampInfos, clampDesc));
    EXPECT_EQ(static_cast<int>(clampInfos.size()), BASIC_SHAPE_LEAF_MIN + 3);
}

/**
 * @tc.name: BasicUberShape_002
 * @tc.desc: Verify basic-shape collection skips, rejects and range-gates
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, BasicUberShape_002, TestSize.Level1)
{
    GESdfUberBasicShapeShape shape(MakeUberParams(nullptr, GESDFUberType::BASIC_SHAPE_UBER));
    std::vector<GESDFBasicShapeUberLeafInfo> infos;
    GESDFUberTreeDesc desc;
    EXPECT_TRUE(shape.CollectLeaves(nullptr, infos, desc));
    EXPECT_TRUE(infos.empty());

    auto withDegenerate = MakeMixedBasicLeaves();
    withDegenerate.push_back(MakeRRect(0.0f, RECT_SIDE, SMALL_RADIUS));
    withDegenerate.push_back(MakeRRect(RECT_SIDE, 0.0f, SMALL_RADIUS));
    withDegenerate.push_back(MakeEllipse(0.0f, RECT_SIDE));
    withDegenerate.push_back(MakeEllipse(RECT_SIDE, 0.0f));
    infos.clear();
    ASSERT_TRUE(shape.CollectLeaves(MakeUnionTree(withDegenerate), infos, desc));
    EXPECT_EQ(static_cast<int>(infos.size()), BASIC_SHAPE_LEAF_MIN);

    auto withPixelmap = MakeMixedBasicLeaves();
    withPixelmap.push_back(MakePixelmap(nullptr));
    EXPECT_FALSE(shape.CollectLeaves(MakeUnionTree(withPixelmap), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakeMismatchedOpTree(), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakePerspectiveTree(), infos, desc));

    EXPECT_EQ(shape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    GESdfUberBasicShapeShape tooFewShape(MakeUberParams(
        MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN - 1, false)), GESDFUberType::BASIC_SHAPE_UBER));
    EXPECT_EQ(tooFewShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    GESdfUberBasicShapeShape tooManyShape(MakeUberParams(
        MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MAX + 1, false)), GESDFUberType::BASIC_SHAPE_UBER));
    EXPECT_EQ(tooManyShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    EXPECT_EQ(shape.GenerateUberShader(RECT_SIDE, RECT_SIDE, true), nullptr);
}

/**
 * @tc.name: HasNonG1RRectLeaf_001
 * @tc.desc: Verify the GEX eligibility scan for uniform and non-uniform basic trees
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, HasNonG1RRectLeaf_001, TestSize.Level1)
{
    EXPECT_FALSE(HasNonG1RRectLeaf(nullptr));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnion(nullptr, nullptr)));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakePixelmap(nullptr)));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeMismatchedOpTree()));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN, true))));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN - 1, false))));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MAX + 1, false))));
    EXPECT_TRUE(HasNonG1RRectLeaf(MakeUnionTree(MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN, false))));

    auto mixed = MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN - 1, true);
    mixed.push_back(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS));
    EXPECT_TRUE(HasNonG1RRectLeaf(MakeUnionTree(mixed)));

    auto withDegenerate = MakeRRectLeaves(BASIC_SHAPE_LEAF_MIN, false);
    withDegenerate.push_back(MakeRRect(0.0f, RECT_SIDE, SMALL_RADIUS));
    withDegenerate.push_back(MakeRRect(RECT_SIDE, 0.0f, SMALL_RADIUS));
    withDegenerate.push_back(MakeEllipse(0.0f, RECT_SIDE));
    withDegenerate.push_back(MakeEllipse(RECT_SIDE, 0.0f));
    EXPECT_TRUE(HasNonG1RRectLeaf(MakeUnionTree(withDegenerate)));

    std::vector<std::shared_ptr<GESDFShaderShape>> triangles;
    std::vector<std::shared_ptr<GESDFShaderShape>> ellipses;
    for (int index = 0; index < BASIC_SHAPE_LEAF_MIN; ++index) {
        triangles.push_back(MakeTriangle(RECT_SIDE));
        ellipses.push_back(MakeEllipse(RECT_SIDE, RECT_SIDE));
    }
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnionTree(triangles)));
    EXPECT_FALSE(HasNonG1RRectLeaf(MakeUnionTree(ellipses)));
}

/**
 * @tc.name: GetBuilder_001
 * @tc.desc: Verify all four builder variants are created and cached for both uber shapes
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, GetBuilder_001, TestSize.Level1)
{
    for (int hasNormal = 0; hasNormal <= 1; ++hasNormal) {
        for (int smooth = 0; smooth <= 1; ++smooth) {
            auto basicBuilder = GESdfUberBasicShapeShape::GetBuilder(hasNormal != 0, smooth != 0);
            ASSERT_NE(basicBuilder, nullptr);
            EXPECT_EQ(GESdfUberBasicShapeShape::GetBuilder(hasNormal != 0, smooth != 0), basicBuilder);
            auto atlasBuilder = GESdfUberPixelmapAtlasShape::GetBuilder(hasNormal != 0, smooth != 0);
            ASSERT_NE(atlasBuilder, nullptr);
            EXPECT_EQ(GESdfUberPixelmapAtlasShape::GetBuilder(hasNormal != 0, smooth != 0), atlasBuilder);
        }
    }
}

/**
 * @tc.name: PixelmapAtlasUberShape_001
 * @tc.desc: Verify pixelmap atlas collection, packing and the collapsed shader
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, PixelmapAtlasUberShape_001, TestSize.Level1)
{
    auto root = MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MIN,
        static_cast<int>(IMAGE_SIDE), static_cast<int>(IMAGE_SIDE)));
    GESdfUberPixelmapAtlasShape shape(MakeUberParams(root, GESDFUberType::PIXELMAP_ATLAS_UBER));

    std::vector<GESDFUberPixelmapLeafInfo> infos;
    GESDFUberTreeDesc desc;
    ASSERT_TRUE(shape.CollectLeaves(root, infos, desc));
    EXPECT_EQ(static_cast<int>(infos.size()), PIXELMAP_UBER_LEAF_MIN);
    EXPECT_FLOAT_EQ(infos[0].width, IMAGE_SIDE);

    auto builder = GESdfUberPixelmapAtlasShape::GetBuilder(false, desc.isSmoothUnion);
    ASSERT_NE(builder, nullptr);
    auto atlas = MakeImage(static_cast<int>(IMAGE_SIDE) * PIXELMAP_UBER_LEAF_MIN, static_cast<int>(IMAGE_SIDE));
    ASSERT_NE(atlas, nullptr);
    const std::vector<float> offsets { 1.0f, 1.0f, IMAGE_SIDE + 2.0f, 1.0f };
    shape.PackUniforms(builder, infos, *atlas, offsets, desc, RECT_SIDE, RECT_SIDE);
    desc.isSmoothUnion = true;
    desc.spacing = 0.0f;
    shape.PackUniforms(builder, infos, *atlas, offsets, desc, RECT_SIDE, RECT_SIDE);

    // Empty packing skips the loop; a positive smooth spacing takes the other std::max edge.
    std::vector<GESDFUberPixelmapLeafInfo> emptyInfos;
    const std::vector<float> emptyOffsets;
    desc.spacing = 5.0f;
    shape.PackUniforms(builder, emptyInfos, *atlas, emptyOffsets, desc, RECT_SIDE, RECT_SIDE);

    EXPECT_NE(shape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);
    EXPECT_NE(shape.GenerateDrawingShaderHasNormal(RECT_SIDE, RECT_SIDE), nullptr);

    auto smoothRoot = MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MIN,
        static_cast<int>(IMAGE_SIDE), static_cast<int>(IMAGE_SIDE)), GESDFUnionOp::SMOOTH_UNION, 20.0f);
    GESdfUberPixelmapAtlasShape smoothShape(MakeUberParams(smoothRoot, GESDFUberType::PIXELMAP_ATLAS_UBER));
    EXPECT_NE(smoothShape.GenerateDrawingShader(RECT_SIDE, RECT_SIDE), nullptr);
}

/**
 * @tc.name: PixelmapAtlasUberShape_002
 * @tc.desc: Verify pixelmap collection rejects and the atlas budget gate
 * @tc.type: FUNC
 */
HWTEST_F(GESDFUberShaderShapeTest, PixelmapAtlasUberShape_002, TestSize.Level1)
{
    GESdfUberPixelmapAtlasShape shape(MakeUberParams(nullptr, GESDFUberType::PIXELMAP_ATLAS_UBER));
    std::vector<GESDFUberPixelmapLeafInfo> infos;
    GESDFUberTreeDesc desc;
    EXPECT_TRUE(shape.CollectLeaves(nullptr, infos, desc));
    EXPECT_TRUE(infos.empty());

    EXPECT_FALSE(shape.CollectLeaves(MakePixelmap(nullptr), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakePixelmap(std::make_shared<Drawing::Image>()), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakeRRect(RECT_SIDE, RECT_SIDE, SMALL_RADIUS), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakeMismatchedOpTree(), infos, desc));
    EXPECT_FALSE(shape.CollectLeaves(MakePerspectiveTree(), infos, desc));

    EXPECT_EQ(shape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    GESdfUberPixelmapAtlasShape tooFewShape(MakeUberParams(
        MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MIN - 1,
            static_cast<int>(IMAGE_SIDE), static_cast<int>(IMAGE_SIDE))), GESDFUberType::PIXELMAP_ATLAS_UBER));
    EXPECT_EQ(tooFewShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
    GESdfUberPixelmapAtlasShape tooManyShape(MakeUberParams(
        MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MAX + 1,
            static_cast<int>(IMAGE_SIDE), static_cast<int>(IMAGE_SIDE))), GESDFUberType::PIXELMAP_ATLAS_UBER));
    EXPECT_EQ(tooManyShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);

    GESdfUberPixelmapAtlasShape wideShape(MakeUberParams(
        MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MIN, WIDE_IMAGE_WIDTH, WIDE_IMAGE_HEIGHT)),
        GESDFUberType::PIXELMAP_ATLAS_UBER));
    EXPECT_EQ(wideShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);

    // Height-only over-budget: two rows fit the width but exceed ATLAS_MAX_SIZE in height.
    GESdfUberPixelmapAtlasShape tallShape(MakeUberParams(
        MakeUnionTree(MakePixelmapLeaves(PIXELMAP_UBER_LEAF_MIN, TALL_IMAGE_SIDE, TALL_IMAGE_SIDE)),
        GESDFUberType::PIXELMAP_ATLAS_UBER));
    EXPECT_EQ(tallShape.GenerateUberShader(RECT_SIDE, RECT_SIDE, false), nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
