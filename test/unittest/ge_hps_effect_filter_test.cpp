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
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    visualEffectImpl->MakeKawaseParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::GREY);
    visualEffectImpl->MakeGreyParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::AIBAR);
    visualEffectImpl->MakeAIBarParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    visualEffectImpl->MakeLinearGradientBlurParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    visualEffectImpl->SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    visualEffectImpl->MakeWaterRippleParams();
    hpsEffectFilter->GenerateVisualEffectFromGE(visualEffectImpl, src_, dst_, saturationForHPS_, brightnessForHPS_);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest GenerateVisualEffectFromGE_001 end";
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
    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    EXPECT_EQ(hpsEffectFilter->HpsSupportEffectGE(veContainer), true);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest HpsSupportEffectGE_001 end";
}

/**
 * @tc.name: InitExtension_001
 * @tc.desc: Verify the InitExtension
 * @tc.type: FUNC
 */
HWTEST_F(GEHpsEffectFilterTest, InitExtension_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest InitExtension_001 start";

    auto hpsEffectFilter = std::make_unique<HpsEffectFilter>();
    ASSERT_TRUE(hpsEffectFilter != nullptr);
    hpsEffectFilter->InitExtension(canvas_);

    GTEST_LOG_(INFO) << "GEHpsEffectFilterTest InitExtension_001 end";
}

} // namespace Rosen
} // namespace OHOS
