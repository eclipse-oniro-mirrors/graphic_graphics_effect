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
#include "ge_circle_flowlight_effect.h"
#include "ge_ripple_shader_mask.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GECircleFlowlightEffectTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::GERippleShaderMask> CreateRippleShaderMask();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GECircleFlowlightEffectTest::SetUp()
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

std::shared_ptr<Drawing::Surface> GECircleFlowlightEffectTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GECircleFlowlightEffectTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

void GECircleFlowlightEffectTest::TearDown()
{
    surface_ = nullptr;
    canvas_ = nullptr;
}

std::shared_ptr<Drawing::GERippleShaderMask> GECircleFlowlightEffectTest::CreateRippleShaderMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.f, 0.f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify the constructor of GECircleFlowlightEffect
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, Constructor_001, TestSize.Level1)
{
    Vector4f color1(1.0f, 0.0f, 0.0f, 1.0f);
    Vector4f color2(0.0f, 1.0f, 0.0f, 1.0f);
    Vector4f color3(0.0f, 0.0f, 1.0f, 1.0f);
 
    Drawing::GECircleFlowlightEffectParams param;
    param.colors[0] = color1;
    param.colors[1] = color2;
    param.colors[2] = color3;
    param.rotationFrequency = 1.0f;
    param.rotationAmplitude = 0.5f;
    param.rotationSeed = 100.0f;
    param.gradientX = 0.3f;
    param.gradientY = 0.7f;
    param.progress = 0.5f;
    param.mask = nullptr;
 
    GECircleFlowlightEffect effect(param);
 
    EXPECT_EQ(effect.rotationFrequency_, param.rotationFrequency);
    EXPECT_EQ(effect.rotationAmplitude_, param.rotationAmplitude);
    EXPECT_EQ(effect.rotationSeed_, param.rotationSeed);
    EXPECT_EQ(effect.gradientX_, param.gradientX);
    EXPECT_EQ(effect.gradientY_, param.gradientY);
    EXPECT_EQ(effect.progress_, param.progress);
    EXPECT_EQ(effect.mask_, nullptr);
}

/**
 * @tc.name: Constructor_002
 * @tc.desc: Verify the constructor with boundary color values
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, Constructor_002, TestSize.Level1)
{
    Vector4f color1(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f color2(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4f color3(0.5f, 0.5f, 0.5f, 0.5f);
    Vector4f color4(0.1f, 0.2f, 0.3f, 0.4f);

    Drawing::GECircleFlowlightEffectParams param;
    param.colors[0] = color1;
    param.colors[1] = color2;
    param.colors[2] = color3;
    param.colors[3] = color4;

    GECircleFlowlightEffect effect(param);

    EXPECT_EQ(effect.colors_[0], color1[0]);
    EXPECT_EQ(effect.colors_[1], color1[1]);
    EXPECT_EQ(effect.colors_[2], color1[2]);
    EXPECT_EQ(effect.colors_[3], color1[3]);

    EXPECT_EQ(effect.colors_[4], color2[0]);
    EXPECT_EQ(effect.colors_[5], color2[1]);
    EXPECT_EQ(effect.colors_[6], color2[2]);
    EXPECT_EQ(effect.colors_[7], color2[3]);

    EXPECT_EQ(effect.colors_[8], color3[0]);
    EXPECT_EQ(effect.colors_[9], color3[1]);
    EXPECT_EQ(effect.colors_[10], color3[2]);
    EXPECT_EQ(effect.colors_[11], color3[3]);

    EXPECT_EQ(effect.colors_[12], color4[0]);
    EXPECT_EQ(effect.colors_[13], color4[1]);
    EXPECT_EQ(effect.colors_[14], color4[2]);
    EXPECT_EQ(effect.colors_[15], color4[3]);
}

/**
 * @tc.name: Constructor_003
 * @tc.desc: Verify the constructor with all parameters set to zero
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, Constructor_003, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    param.rotationFrequency = 0.0f;
    param.rotationAmplitude = 0.0f;
    param.rotationSeed = 0.0f;
    param.gradientX = 0.0f;
    param.gradientY = 0.0f;
    param.progress = 0.0f;
    param.mask = nullptr;

    GECircleFlowlightEffect effect(param);

    EXPECT_EQ(effect.rotationFrequency_, 0.0f);
    EXPECT_EQ(effect.rotationAmplitude_, 0.0f);
    EXPECT_EQ(effect.rotationSeed_, 0.0f);
    EXPECT_EQ(effect.gradientX_, 0.0f);
    EXPECT_EQ(effect.gradientY_, 0.0f);
    EXPECT_EQ(effect.progress_, 0.0f);
    EXPECT_EQ(effect.mask_, nullptr);
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify Preprocess function (empty implementation)
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, Preprocess_001, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    EXPECT_NE(canvas_, nullptr);
    effect.Preprocess(*canvas_, rect_); // Should do nothing
    SUCCEED();
}

/**
 * @tc.name: IsValidParam_001
 * @tc.desc: Verify IsValidParam with valid input
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_001, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(100.0f, 100.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsValidParam_002
 * @tc.desc: Verify IsValidParam with invalid width
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_002, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(0.0f, 100.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_003
 * @tc.desc: Verify IsValidParam with invalid height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_003, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(100.0f, 0.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_004
 * @tc.desc: Verify IsValidParam with negative width
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_004, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(-100.0f, 100.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_005
 * @tc.desc: Verify IsValidParam with negative height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_005, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(100.0f, -100.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_006
 * @tc.desc: Verify IsValidParam with zero width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_006, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(0.0f, 0.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_007
 * @tc.desc: Verify IsValidParam with negative width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_007, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(-100.0f, -100.0f);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsValidParam_008
 * @tc.desc: Verify IsValidParam with very large width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, IsValidParam_008, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    bool result = effect.IsValidParam(1000000.0f, 1000000.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CreateRippleShaderMask_001
 * @tc.desc: Verify CreateRippleShaderMask does not return nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, CreateRippleShaderMask_001, TestSize.Level1) {
    Drawing::GECircleFlowlightEffectParams param;
    param.mask = CreateRippleShaderMask();
    EXPECT_NE(param.mask, nullptr);
}

/**
 * @tc.name: MakeCircleFlowlightEffectWithMask_001
 * @tc.desc: Verify MakeCircleFlowlightEffectWithMask with valid mask
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeCircleFlowlightEffectWithMask_001, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    param.mask = CreateRippleShaderMask();

    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffectWithMask();

    EXPECT_EQ(effect.builder_, nullptr);
}

/**
 * @tc.name: SetUniform_001
 * @tc.desc: Verify SetUniform with mask enabled
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, SetUniform_001, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    param.mask = CreateRippleShaderMask();
    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffectWithMask();
    effect.SetUniform(100.0f, 100.0f);
    EXPECT_NE(effect.builder_, nullptr);
}

/**
 * @tc.name: SetUniform_002
 * @tc.desc: Verify SetUniform without mask
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, SetUniform_002, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffect();
    effect.SetUniform(100.0f, 100.0f);
    EXPECT_NE(effect.builder_, nullptr);
}

/**
 * @tc.name: SetUniform_003
 * @tc.desc: Verify SetUniform with zero width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, SetUniform_003, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffect();
    effect.SetUniform(0.0f, 0.0f);
    EXPECT_NE(effect.builder_, nullptr);
}

/**
 * @tc.name: SetUniform_004
 * @tc.desc: Verify SetUniform with negative width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, SetUniform_004, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffect();
    effect.SetUniform(-100.0f, -100.0f);
    EXPECT_NE(effect.builder_, nullptr);
}

/**
 * @tc.name: SetUniform_005
 * @tc.desc: Verify SetUniform with very large width and height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, SetUniform_005, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    effect.MakeCircleFlowlightEffect();
    effect.SetUniform(1000000.0f, 1000000.0f);
    EXPECT_NE(effect.builder_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: Verify MakeDrawingShader with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_001, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    param.mask = nullptr;
    GECircleFlowlightEffect effect(param);
    effect.MakeDrawingShader(rect_, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_002
 * @tc.desc: Verify MakeDrawingShader with invalid rect (zero width)
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_002, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect zeroRect(0, 0, 0, 100);
    effect.MakeDrawingShader(zeroRect, 0.5f);
    EXPECT_EQ(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_003
 * @tc.desc: Verify MakeDrawingShader with mask but null shader
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_003, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    param.mask = CreateRippleShaderMask();
    GECircleFlowlightEffect effect(param);
    effect.MakeDrawingShader(rect_, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_004
 * @tc.desc: Verify MakeDrawingShader with negative rect width
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_004, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect negativeRect(0, 0, -100, 100);
    effect.MakeDrawingShader(negativeRect, 0.5f);
    EXPECT_EQ(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_005
 * @tc.desc: Verify MakeDrawingShader with negative rect height
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_005, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect negativeRect(0, 0, 100, -100);
    effect.MakeDrawingShader(negativeRect, 0.5f);
    EXPECT_EQ(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_006
 * @tc.desc: Verify MakeDrawingShader with very large rect
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_006, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect largeRect(0, 0, 1000000, 1000000);
    effect.MakeDrawingShader(largeRect, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_007
 * @tc.desc: Verify MakeDrawingShader with zero rect
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_007, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect zeroRect(0, 0, 0, 0);
    effect.MakeDrawingShader(zeroRect, 0.5f);
    EXPECT_EQ(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_008
 * @tc.desc: Verify MakeDrawingShader with negative rect
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_008, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect negativeRect(-100, -100, 100, 100);
    effect.MakeDrawingShader(negativeRect, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_009
 * @tc.desc: Verify MakeDrawingShader with very small rect
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_009, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect smallRect(0, 0, 0.0001f, 0.0001f);
    effect.MakeDrawingShader(smallRect, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_010
 * @tc.desc: Verify MakeDrawingShader with rect larger than surface
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_010, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect largeRect(0, 0, 200.0f, 200.0f);
    effect.MakeDrawingShader(largeRect, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_011
 * @tc.desc: Verify MakeDrawingShader with rect smaller than surface
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, MakeDrawingShader_011, TestSize.Level1)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    Drawing::Rect smallRect(0, 0, 50.0f, 50.0f);
    effect.MakeDrawingShader(smallRect, 0.5f);
    EXPECT_NE(effect.drShader_, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type function
 * @tc.type: FUNC
 */
HWTEST_F(GECircleFlowlightEffectTest, Type_001, TestSize.Level2)
{
    Drawing::GECircleFlowlightEffectParams param;
    GECircleFlowlightEffect effect(param);
    EXPECT_EQ(effect.Type(), Drawing::GEFilterType::CIRCLE_FLOWLIGHT);
    EXPECT_EQ(effect.TypeName(), Drawing::GE_SHADER_CIRCLE_FLOWLIGHT);
}

}
}