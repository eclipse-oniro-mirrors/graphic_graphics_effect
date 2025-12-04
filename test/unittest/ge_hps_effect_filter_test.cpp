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

#include "ge_hps_effect_filter.h"
#include "ge_pixel_map_shader_mask.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEHpsEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    std::shared_ptr<Drawing::GEPixelMapShaderMask> CreatePixelMapShaderMask();
    std::shared_ptr<Drawing::GERadialGradientShaderMask> CreateRadialGradientShaderMask();
    std::shared_ptr<Drawing::GERippleShaderMask> CreateRippleShaderMask();

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
    float saturationForHPS_ = 1.0f;
    float brightnessForHPS_ = 1.0f;
};

void GEHpsEffectFilterTest::SetUpTestCase(void) {}
void GEHpsEffectFilterTest::TearDownTestCase(void) {}

void GEHpsEffectFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEHpsEffectFilterTest::TearDown() {}

std::shared_ptr<Drawing::Image> MakeImage()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

std::shared_ptr<Drawing::GEPixelMapShaderMask> GEHpsEffectFilterTest::CreatePixelMapShaderMask()
{
    Drawing::GEPixelMapMaskParams maskParams;
    maskParams.image = ::OHOS::Rosen::MakeImage();
    maskParams.src = { 0.5, 1.0, 0.1, 0.2 };
    maskParams.dst = { 1.3, 1.2, 1.6, 2.0 };
    maskParams.fillColor = Vector4f(0.2, 0.8, 0.1, 0.9);
    return std::make_shared<Drawing::GEPixelMapShaderMask>(maskParams);
}

std::shared_ptr<Drawing::GERadialGradientShaderMask> GEHpsEffectFilterTest::CreateRadialGradientShaderMask()
{
    Drawing::GERadialGradientShaderMaskParams maskParams;
    maskParams.center_ = {0.5f, 0.5f};
    maskParams.radiusX_ = 1.0f;
    maskParams.radiusY_ = 1.0f;
    maskParams.colors_ = {0.0f, 1.0f};
    maskParams.positions_ = {0.0f, 1.0f};
    return std::make_shared<Drawing::GERadialGradientShaderMask>(maskParams);
}

std::shared_ptr<Drawing::GERippleShaderMask> GEHpsEffectFilterTest::CreateRippleShaderMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.5f, 0.5f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

/**
 * @tc.name: GenerateVisualEffectFromGE_001
 * @tc.desc: Verify the GenerateVisualEffectFromGE
 * @tc.type: FUNC
 */
HWTEST_F(GEHpsEffectFilterTest, GenerateVisualEffectFromGE_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest GenerateVisualEffectFromGE_001 start";

    auto visualEffectImpl = std::make_shared<Drawing::GEVisualEffectImpl>("");
    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    ASSERT_TRUE(hpsEffectFilter != nullptr);
    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);
    visualEffectImpl->MakeMESAParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    visualEffectImpl->MakeKawaseParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::GREY);
    visualEffectImpl->MakeGreyParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::AIBAR);
    visualEffectImpl->MakeAIBarParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    visualEffectImpl->MakeLinearGradientBlurParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    visualEffectImpl->MakeWaterRippleParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::EDGE_LIGHT);
    visualEffectImpl->MakeEdgeLightParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);
    auto edgeLightParams = visualEffectImpl->GetEdgeLightParams();
    edgeLightParams->mask = CreatePixelMapShaderMask();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_,
        brightnessForHPS_, image_);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest GenerateVisualEffectFromGE_001 end";
}

/**
 * @tc.name: GenerateMaskParameter_001
 * @tc.desc: Verify the GenerateMaskParameter
 * @tc.type: FUNC
 */
HWTEST_F(GEHpsEffectFilterTest, GenerateMaskParameter_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest GenerateMaskParameter_001 start";

    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    ASSERT_TRUE(hpsEffectFilter != nullptr);

    std::shared_ptr<Drawing::GEShaderMask> shaderMask = CreatePixelMapShaderMask();
    auto maskParams = hpsEffectFilter->GenerateMaskParameter(shaderMask);
    EXPECT_NE(maskParams, nullptr);

    shaderMask = CreateRadialGradientShaderMask();
    maskParams = hpsEffectFilter->GenerateMaskParameter(shaderMask);
    EXPECT_NE(maskParams, nullptr);

    shaderMask = CreateRippleShaderMask();
    maskParams = hpsEffectFilter->GenerateMaskParameter(shaderMask);
    EXPECT_EQ(maskParams, nullptr);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest GenerateMaskParameter_001 end";
}

/**
 * @tc.name: ApplyHpsEffect_001
 * @tc.desc: Verify the ApplyHpsEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEHpsEffectFilterTest, ApplyHpsEffect_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest ApplyHpsEffect_001 start";

    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    float alpha = 1.0f;
    std::shared_ptr<Drawing::ColorFilter> colorFilter;
    uint32_t maskColor = 255;
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    HpsEffectFilter::HpsEffectContext hpsEffectContext = {alpha, colorFilter, maskColor};
    EXPECT_EQ(hpsEffectFilter->ApplyHpsEffect(canvas_, image_, outImage, hpsEffectContext), false);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest ApplyHpsEffect_001 end";
}

/**
 * @tc.name: HpsSupportEffectGE_001
 * @tc.desc: Verify the HpsSupportEffectGE
 * @tc.type: FUNC
 */
HWTEST_F(GEHpsEffectFilterTest, HpsSupportEffectGE_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest HpsSupportEffectGE_001 start";

    Drawing::GEVisualEffectContainer veContainer;
    std::pair<float, float> factor = {0.5f, 0.5f};
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_MASK_RIPPLE);
    visualEffect->visualEffectImpl_->rippleMaskParams_ = std::make_shared<Drawing::GERippleShaderMaskParams>();
    visualEffect->visualEffectImpl_->filterType_ = Drawing::GEVisualEffectImpl::FilterType::RIPPLE_MASK;
    visualEffect->SetParam(Drawing::GE_MASK_RIPPLE_CENTER, factor);
    veContainer.AddToChainedFilter(visualEffect);
    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    EXPECT_EQ(hpsEffectFilter->HpsSupportEffectGE(veContainer), false);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest HpsSupportEffectGE_001 end";
}

} // namespace Rosen
} // namespace OHOS
