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
#include "ge_visual_effect_impl.h"

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
    visualEffect->visualEffectImpl_->kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::KAWASE_BLUR;
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    EXPECT_EQ(visualEffect->visualEffectImpl_->kawaseParams_->radius, 1);

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
    visualEffect->visualEffectImpl_->kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::KAWASE_BLUR;
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<double>(1));
    EXPECT_NE(visualEffect->visualEffectImpl_->kawaseParams_->radius, 1);

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
    visualEffect->visualEffectImpl_->kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::KAWASE_BLUR;
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast< const char* const>("1"));
    EXPECT_NE(visualEffect->visualEffectImpl_->kawaseParams_->radius, 1);

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
    visualEffect->visualEffectImpl_->kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::KAWASE_BLUR;
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<int64_t>(1));
    EXPECT_NE(visualEffect->visualEffectImpl_->kawaseParams_->radius, 1);

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
    visualEffect->visualEffectImpl_->rippleMaskParams_ = std::make_shared<GERippleShaderMaskParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::RIPPLE_MASK;
    visualEffect->SetParam(GE_MASK_RIPPLE_CENTER, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->rippleMaskParams_->center_, factor);

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
    visualEffect->visualEffectImpl_->bezierWarpParams_ = std::make_shared<GEBezierWarpShaderFilterParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::BEZIER_WARP;
    visualEffect->SetParam(GE_FILTER_BEZIER_WARP_DESTINATION_PATCH, points);
    EXPECT_EQ(visualEffect->visualEffectImpl_->bezierWarpParams_->destinationPatch, points);

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
    
    Vector3f lightPosition = Vector3f(0.0f, 0.f, 0.0f);
    visualEffect->SetParam(GE_FILTER_CONTENT_LIGHT_POSITION, lightPosition);
    bool ret = visualEffect->visualEffectImpl_->contentLightParams_->lightPosition == lightPosition;
    EXPECT_TRUE(ret);
 
    Vector4f lightColor = Vector4f(0.2f, 0.4f, 0.6f, 0.5f);
    visualEffect->SetParam(GE_FILTER_CONTENT_LIGHT_COLOR, lightColor);
    ret = visualEffect->visualEffectImpl_->contentLightParams_->lightColor == lightColor;
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
    EXPECT_EQ(canvasInfo1.geoWidth_, 0.0f);

    Drawing::CanvasInfo canvasInfo2 = {100.0f, 100.0f, 0.0f, 0.0f, Drawing::Matrix()};
    auto visualEffect2 =
        std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR, Drawing::DrawingPaintType::BRUSH, canvasInfo2);
    Drawing::CanvasInfo canvasInfo3 = visualEffect2->GetCanvasInfo();
    EXPECT_EQ(canvasInfo3.geoWidth_, canvasInfo2.geoWidth_);

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
    visualEffect->visualEffectImpl_->waveGradientMaskParams_ = std::make_shared<GEWaveGradientShaderMaskParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK;
    visualEffect->SetParam(GE_MASK_WAVE_GRADIENT_CENTER, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->waveGradientMaskParams_->center_, factor);

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
    visualEffect->visualEffectImpl_->doubleRippleMaskParams_ = std::make_shared<GEDoubleRippleShaderMaskParams>();
    visualEffect->visualEffectImpl_->filterType_ = GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK;
    visualEffect->SetParam(GE_MASK_DOUBLE_RIPPLE_CENTER1, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->doubleRippleMaskParams_->center1_, factor);
    visualEffect->SetParam(GE_MASK_DOUBLE_RIPPLE_CENTER2, factor);
    EXPECT_EQ(visualEffect->visualEffectImpl_->doubleRippleMaskParams_->center2_, factor);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_009 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
