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

#include "ge_shader_filter_params.h"
#include "ge_visual_effect.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectTest::SetUpTestCase(void) {}
void GEVisualEffectTest::TearDownTestCase(void) {}

void GEVisualEffectTest::SetUp() {}
void GEVisualEffectTest::TearDown() {}

/**
 * @tc.name: SetParam_001
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_001 end";
}

/**
 * @tc.name: SetParam_002
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_002 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<double>(1));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_002 end";
}

/**
 * @tc.name: SetParam_003
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_003 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast< const char* const>("1"));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_003 end";
}

/**
 * @tc.name: SetParam_004
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_004 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<int64_t>(1));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_004 end";
}

/**
 * @tc.name: SetParam_005
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_005 start";

    std::pair<float, float> factor = {0.5f, 0.5f};
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_RIPPLE);
    visualEffect->visualEffectImpl_->MakeRippleMaskParams();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::RIPPLE_MASK;
    visualEffect->SetParam(GE_MASK_RIPPLE_CENTER, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->GetRippleMaskParams()->center_, factor);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_005 end";
}

/**
 * @tc.name: SetParam_006
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_006 start";
    constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points = {{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {11.0f, 12.0f},
        {13.0f, 14.0f},
        {15.0f, 16.0f},
        {17.0f, 18.0f},
        {19.0f, 20.0f},
        {21.0f, 22.0f},
        {23.0f, 24.0f}
    }};
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_BEZIER_WARP);
    visualEffect->SetParam(GE_FILTER_BEZIER_WARP_DESTINATION_PATCH, points);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_006 end";
}

/**
 * @tc.name: SetParam_007
 * @tc.desc: Verify the SetParam Vector3f Vector4f
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_007 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_CONTENT_LIGHT);
    visualEffect->visualEffectImpl_->MakeContentLightParams();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::CONTENT_LIGHT;

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    visualEffect->SetParam(GE_FILTER_CONTENT_LIGHT_POSITION, lightPosition);
    bool ret = visualEffect->visualEffectImpl_->GetContentLightParams()->position == lightPosition;
    EXPECT_TRUE(ret);

    Vector4f lightColor = Vector4f(0.2f, 0.4f, 0.6f, 0.5f);
    visualEffect->SetParam(GE_FILTER_CONTENT_LIGHT_COLOR, lightColor);
    ret = visualEffect->visualEffectImpl_->GetContentLightParams()->color == lightColor;
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_007 end";
}

/**
 * @tc.name: GetCanvasInfo_001
 * @tc.desc: Verify the GetCanvasInfo
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GetCanvasInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetCanvasInfo_001 start";
 
    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    Drawing::CanvasInfo canvasInfo1 = visualEffect1->GetCanvasInfo();
    EXPECT_FLOAT_EQ(canvasInfo1.geoWidth, 0.0f);
    EXPECT_FLOAT_EQ(canvasInfo1.geoHeight, 0.0f);
    EXPECT_FLOAT_EQ(canvasInfo1.tranX, 0.0f);
    EXPECT_FLOAT_EQ(canvasInfo1.tranY, 0.0f);

    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Drawing::CanvasInfo canvasInfo2 = {100.0f, 100.0f, 1.0f, -1.0f, matrix};
    auto visualEffect2 =
        std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR, Drawing::DrawingPaintType::BRUSH, canvasInfo2);
    Drawing::CanvasInfo canvasInfo3 = visualEffect2->GetCanvasInfo();
    EXPECT_FLOAT_EQ(canvasInfo3.geoWidth, canvasInfo2.geoWidth);
    EXPECT_FLOAT_EQ(canvasInfo3.geoHeight, canvasInfo2.geoHeight);
    EXPECT_FLOAT_EQ(canvasInfo3.tranX, canvasInfo2.tranX);
    EXPECT_FLOAT_EQ(canvasInfo3.tranY, canvasInfo2.tranY);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetCanvasInfo_001 end";
}

/**
 * @tc.name: SetParam_008
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_008 start";

    std::pair<float, float> factor = {0.5f, 0.5f};
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_WAVE_GRADIENT);
    visualEffect->visualEffectImpl_->MakeWaveGradientMaskParams();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK;
    visualEffect->SetParam(GE_MASK_WAVE_GRADIENT_CENTER, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->GetWaveGradientMaskParams()->center_, factor);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_008 end";
}

/**
 * @tc.name: SetParam_009
 * @tc.desc: Verify the SetParam Vector2f
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_009 start";

    std::pair<float, float> factor = {0.5f, 0.5f};
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_DOUBLE_RIPPLE);
    visualEffect->visualEffectImpl_->MakeDoubleRippleMaskParams();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK;
    visualEffect->SetParam(GE_MASK_DOUBLE_RIPPLE_CENTER1, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->GetDoubleRippleMaskParams()->center1_, factor);
    visualEffect->SetParam(GE_MASK_DOUBLE_RIPPLE_CENTER2, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->GetDoubleRippleMaskParams()->center2_, factor);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_009 end";
}

/**
 * @tc.name: GetName_VariousEffectTypes
 * @tc.desc: Verify function GetName returns correct effect name for different effect types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GetName_VariousEffectTypes, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetName_VariousEffectTypes start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(visualEffect->GetName(), GE_FILTER_KAWASE_BLUR);

    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    EXPECT_EQ(visualEffect2->GetName(), GE_FILTER_MESA_BLUR);

    auto visualEffect3 = std::make_shared<GEVisualEffect>("custom_effect");
    EXPECT_EQ(visualEffect3->GetName(), "custom_effect");

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetName_VariousEffectTypes end";
}

/**
 * @tc.name: SetCanvasInfo_ValidGeometryAndTransform
 * @tc.desc: Verify function SetCanvasInfo with valid geometry and transform values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetCanvasInfo_ValidGeometryAndTransform, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_ValidGeometryAndTransform start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 100.0f;
    canvasInfo.geoHeight = 200.0f;
    canvasInfo.tranX = 10.0f;
    canvasInfo.tranY = 20.0f;

    visualEffect->SetCanvasInfo(canvasInfo);

    Drawing::CanvasInfo result = visualEffect->GetCanvasInfo();
    EXPECT_FLOAT_EQ(result.geoWidth, 100.0f);
    EXPECT_FLOAT_EQ(result.geoHeight, 200.0f);
    EXPECT_FLOAT_EQ(result.tranX, 10.0f);
    EXPECT_FLOAT_EQ(result.tranY, 20.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_ValidGeometryAndTransform end";
}

/**
 * @tc.name: SetSupportHeadroom_MultipleValues
 * @tc.desc: Verify function SetSupportHeadroom and GetSupportHeadroom with multiple values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetSupportHeadroom_MultipleValues, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_MultipleValues start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 0.0f);

    visualEffect->SetSupportHeadroom(1.5f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 1.5f);

    visualEffect->SetSupportHeadroom(0.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 0.0f);

    visualEffect->SetSupportHeadroom(2.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 2.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_MultipleValues end";
}

/**
 * @tc.name: GenerateShaderMask_RippleMaskType
 * @tc.desc: Verify function GenerateShaderMask generates valid ripple mask shader
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_RippleMaskType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_RippleMaskType start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_RIPPLE);
    visualEffect->visualEffectImpl_->MakeRippleMaskParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::RIPPLE_MASK);

    auto shaderMask = visualEffect->GenerateShaderMask();
    EXPECT_NE(shaderMask, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_RippleMaskType end";
}

/**
 * @tc.name: GenerateShaderMask_NonMaskFilterType
 * @tc.desc: Verify function GenerateShaderMask returns null for non-mask filter types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_NonMaskFilterType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_NonMaskFilterType start";
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->visualEffectImpl_->MakeKawaseParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    auto shaderMask = visualEffect->GenerateShaderMask();
    EXPECT_EQ(shaderMask, nullptr);
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_NonMaskFilterType end";
}

/**
 * @tc.name: GenerateShaderShape_SDFRRectShapeType
 * @tc.desc: Verify function GenerateShaderShape generates valid SDF RRect shape shader
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderShape_SDFRRectShapeType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFRRectShapeType start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_SHAPE_SDF_RRECT_SHAPE);
    visualEffect->visualEffectImpl_->MakeSDFRRectShapeParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE);

    Drawing::GERRect rrect { 10.0f, 20.0f, 100.0f, 200.0f };
    rrect.SetCornerRadius(15.0f, 15.0f);
    visualEffect->SetParam(GE_SHAPE_SDF_RRECT_SHAPE_RRECT, rrect);

    auto shaderShape = visualEffect->GenerateShaderShape();
    EXPECT_NE(shaderShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFRRectShapeType end";
}

/**
 * @tc.name: GenerateShaderShape_NonShapeFilterType
 * @tc.desc: Verify function GenerateShaderShape returns null for non-shape filter types
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderShape_NonShapeFilterType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_NonShapeFilterType start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->visualEffectImpl_->MakeKawaseParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    auto shaderShape = visualEffect->GenerateShaderShape();
    EXPECT_EQ(shaderShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_NonShapeFilterType end";
}

/**
 * @tc.name: Constructor_WithCanvasInfoParameter
 * @tc.desc: Verify constructor with CanvasInfo parameter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, Constructor_WithCanvasInfoParameter, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_WithCanvasInfoParameter start";

    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 100.0f;
    canvasInfo.geoHeight = 200.0f;

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR, DrawingPaintType::BRUSH, canvasInfo);
    EXPECT_EQ(visualEffect->GetName(), GE_FILTER_KAWASE_BLUR);

    auto result = visualEffect->GetCanvasInfo();
    EXPECT_FLOAT_EQ(result.geoWidth, 100.0f);
    EXPECT_FLOAT_EQ(result.geoHeight, 200.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_WithCanvasInfoParameter end";
}

/**
 * @tc.name: Constructor_DifferentPaintTypes
 * @tc.desc: Verify constructor with different DrawingPaintType values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, Constructor_DifferentPaintTypes, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_DifferentPaintTypes start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR, DrawingPaintType::NONE);
    EXPECT_EQ(visualEffect1->GetName(), GE_FILTER_KAWASE_BLUR);

    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR, DrawingPaintType::BRUSH);
    EXPECT_EQ(visualEffect2->GetName(), GE_FILTER_MESA_BLUR);

    auto visualEffect3 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY, DrawingPaintType::PEN);
    EXPECT_EQ(visualEffect3->GetName(), GE_FILTER_GREY);

    auto visualEffect4 = std::make_shared<GEVisualEffect>(GE_FILTER_AI_BAR, DrawingPaintType::BRUSH_PEN);
    EXPECT_EQ(visualEffect4->GetName(), GE_FILTER_AI_BAR);

    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_DifferentPaintTypes end";
}

/**
 * @tc.name: Constructor_EmptyEffectName
 * @tc.desc: Verify constructor handles empty effect name
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, Constructor_EmptyEffectName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_EmptyEffectName start";

    auto visualEffect = std::make_shared<GEVisualEffect>("", DrawingPaintType::BRUSH);
    EXPECT_EQ(visualEffect->GetName(), "");

    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_EmptyEffectName end";
}

/**
 * @tc.name: GetImpl_ValidImplPointer
 * @tc.desc: Verify function GetImpl returns valid implementation pointer
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GetImpl_ValidImplPointer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetImpl_ValidImplPointer start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto impl = visualEffect->GetImpl();
    EXPECT_NE(impl, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetImpl_ValidImplPointer end";
}

/**
 * @tc.name: SetCanvasInfo_ZeroValues
 * @tc.desc: Verify function SetCanvasInfo handles zero values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetCanvasInfo_ZeroValues, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_ZeroValues start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 0.0f;
    canvasInfo.geoHeight = 0.0f;
    canvasInfo.tranX = 0.0f;
    canvasInfo.tranY = 0.0f;

    visualEffect->SetCanvasInfo(canvasInfo);

    auto result = visualEffect->GetCanvasInfo();
    EXPECT_FLOAT_EQ(result.geoWidth, 0.0f);
    EXPECT_FLOAT_EQ(result.geoHeight, 0.0f);
    EXPECT_FLOAT_EQ(result.tranX, 0.0f);
    EXPECT_FLOAT_EQ(result.tranY, 0.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_ZeroValues end";
}

/**
 * @tc.name: SetCanvasInfo_NegativeValues
 * @tc.desc: Verify function SetCanvasInfo handles negative values
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetCanvasInfo_NegativeValues, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_NegativeValues start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = -100.0f;
    canvasInfo.geoHeight = -200.0f;
    canvasInfo.tranX = -10.0f;
    canvasInfo.tranY = -20.0f;

    visualEffect->SetCanvasInfo(canvasInfo);

    auto result = visualEffect->GetCanvasInfo();
    EXPECT_FLOAT_EQ(result.geoWidth, -100.0f);
    EXPECT_FLOAT_EQ(result.geoHeight, -200.0f);
    EXPECT_FLOAT_EQ(result.tranX, -10.0f);
    EXPECT_FLOAT_EQ(result.tranY, -20.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetCanvasInfo_NegativeValues end";
}

/**
 * @tc.name: SetSupportHeadroom_NegativeValue
 * @tc.desc: Verify function SetSupportHeadroom handles negative value
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetSupportHeadroom_NegativeValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_NegativeValue start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    visualEffect->SetSupportHeadroom(-1.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), -1.0f);

    visualEffect->SetSupportHeadroom(-100.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), -100.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_NegativeValue end";
}

/**
 * @tc.name: SetSupportHeadroom_VeryLargeValue
 * @tc.desc: Verify function SetSupportHeadroom handles very large value
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetSupportHeadroom_VeryLargeValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_VeryLargeValue start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);

    visualEffect->SetSupportHeadroom(1000.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 1000.0f);

    visualEffect->SetSupportHeadroom(10000.0f);
    EXPECT_FLOAT_EQ(visualEffect->GetSupportHeadroom(), 10000.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetSupportHeadroom_VeryLargeValue end";
}

/**
 * @tc.name: GenerateShaderMask_RadialGradientMask
 * @tc.desc: Verify function GenerateShaderMask generates valid radial gradient mask
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_RadialGradientMask, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_RadialGradientMask start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_RADIAL_GRADIENT);
    visualEffect->visualEffectImpl_->MakeRadialGradientMaskParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::RADIAL_GRADIENT_MASK);

    auto shaderMask = visualEffect->GenerateShaderMask();
    EXPECT_NE(shaderMask, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_RadialGradientMask end";
}

/**
 * @tc.name: GenerateShaderMask_DoubleRippleMask
 * @tc.desc: Verify function GenerateShaderMask generates valid double ripple mask
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_DoubleRippleMask, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_DoubleRippleMask start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_DOUBLE_RIPPLE);
    visualEffect->visualEffectImpl_->MakeDoubleRippleMaskParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK);

    auto shaderMask = visualEffect->GenerateShaderMask();
    EXPECT_NE(shaderMask, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_DoubleRippleMask end";
}

/**
 * @tc.name: GenerateShaderMask_WaveGradientMask
 * @tc.desc: Verify function GenerateShaderMask generates valid wave gradient mask
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_WaveGradientMask, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_WaveGradientMask start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_WAVE_GRADIENT);
    visualEffect->visualEffectImpl_->MakeWaveGradientMaskParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK);

    auto shaderMask = visualEffect->GenerateShaderMask();
    EXPECT_NE(shaderMask, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_WaveGradientMask end";
}

/**
 * @tc.name: GenerateShaderShape_SDFPixelmapShape
 * @tc.desc: Verify function GenerateShaderShape generates valid SDF pixelmap shape
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderShape_SDFPixelmapShape, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFPixelmapShape start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_SHAPE_SDF_PIXELMAP_SHAPE);
    visualEffect->visualEffectImpl_->MakeSDFPixelmapShapeParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::SDF_PIXELMAP_SHAPE);

    auto shaderShape = visualEffect->GenerateShaderShape();
    EXPECT_NE(shaderShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFPixelmapShape end";
}

/**
 * @tc.name: GenerateShaderShape_SDFTransformShape
 * @tc.desc: Verify function GenerateShaderShape generates valid SDF transform shape
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderShape_SDFTransformShape, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFTransformShape start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_SHAPE_SDF_TRANSFORM_SHAPE);
    visualEffect->visualEffectImpl_->MakeSDFTransformShapeParams();
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::SDF_TRANSFORM_SHAPE);

    auto shaderShape = visualEffect->GenerateShaderShape();
    EXPECT_NE(shaderShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_SDFTransformShape end";
}

/**
 * @tc.name: CanBeContinuous_UniformCircularRadii
 * @tc.desc: Verify static function CanBeContinuous with uniform circular corner radii
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, CanBeContinuous_UniformCircularRadii, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_UniformCircularRadii start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(10.0f, 10.0f);

    bool result = GEVisualEffect::CanBeContinuous(params);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_UniformCircularRadii end";
}

/**
 * @tc.name: CanBeContinuous_NonUniformRadii
 * @tc.desc: Verify static function CanBeContinuous with non-uniform corner radii returns false
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, CanBeContinuous_NonUniformRadii, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_NonUniformRadii start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(10.0f, 20.0f);

    bool result = GEVisualEffect::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_NonUniformRadii end";
}

/**
 * @tc.name: CanBeContinuous_LargeRadiusExceedsLimit
 * @tc.desc: Verify static function CanBeContinuous when radius exceeds size limit
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, CanBeContinuous_LargeRadiusExceedsLimit, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_LargeRadiusExceedsLimit start";

    auto params = std::make_shared<GESDFRRectShapeParams>();
    params->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    params->rrect.SetCornerRadius(80.0f, 80.0f);

    bool result = GEVisualEffect::CanBeContinuous(params);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "GEVisualEffectTest CanBeContinuous_LargeRadiusExceedsLimit end";
}

/**
 * @tc.name: GetGEShaderShape_AfterSetParam
 * @tc.desc: Verify function GetGEShaderShape retrieves shape after SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GetGEShaderShape_AfterSetParam, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetGEShaderShape_AfterSetParam start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    auto rrectParams = std::make_shared<GESDFRRectShapeParams>();
    rrectParams->rrect = { 0.0f, 0.0f, 100.0f, 100.0f };
    rrectParams->rrect.SetCornerRadius(10.0f, 10.0f);

    auto rrectShape = std::make_shared<GESDFRRectShaderShape>(*rrectParams);
    visualEffect->SetParam(GE_FILTER_FROSTED_GLASS_SHAPE, std::static_pointer_cast<GEShaderShape>(rrectShape));

    auto retrievedShape = visualEffect->GetGEShaderShape(GE_FILTER_FROSTED_GLASS_SHAPE);
    EXPECT_NE(retrievedShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetGEShaderShape_AfterSetParam end";
}

/**
 * @tc.name: GetName_CustomEffectName
 * @tc.desc: Verify GetName returns custom effect name correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GetName_CustomEffectName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetName_CustomEffectName start";

    std::string customName = "my_custom_effect_v2";
    auto visualEffect = std::make_shared<GEVisualEffect>(customName);
    EXPECT_EQ(visualEffect->GetName(), customName);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetName_CustomEffectName end";
}

/**
 * @tc.name: GenerateShaderMask_WithoutMakeParams
 * @tc.desc: Verify GenerateShaderMask without MakeParams returns null or handles gracefully
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderMask_WithoutMakeParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_WithoutMakeParams start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_MASK_RIPPLE);
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::RIPPLE_MASK);

    auto shaderMask = visualEffect->GenerateShaderMask();
    // Without MakeParams, should return nullptr or handle gracefully
    EXPECT_EQ(shaderMask, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderMask_WithoutMakeParams end";
}

/**
 * @tc.name: GenerateShaderShape_WithoutMakeParams
 * @tc.desc: Verify GenerateShaderShape without MakeParams returns null or handles gracefully
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, GenerateShaderShape_WithoutMakeParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_WithoutMakeParams start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_SHAPE_SDF_RRECT_SHAPE);
    visualEffect->visualEffectImpl_->SetFilterType(GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE);

    auto shaderShape = visualEffect->GenerateShaderShape();
    // Without MakeParams, should return nullptr or handle gracefully
    EXPECT_EQ(shaderShape, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateShaderShape_WithoutMakeParams end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
