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
#include "ge_direction_light_shader_filter.h"
#include "ge_ripple_shader_mask.h"
#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEDirectionLightShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    std::shared_ptr<Drawing::Image> imageEmpty_ { nullptr };
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEDirectionLightShaderFilterTest::SetUpTestCase(void) {}
void GEDirectionLightShaderFilterTest::TearDownTestCase(void) {}

void GEDirectionLightShaderFilterTest::SetUp()
{
    canvas_.Restore();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    bmp.Build(0, 0, format);
    imageEmpty_ = bmp.MakeImage();
}

void GEDirectionLightShaderFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImage_NullImage
 * @tc.desc: Verify OnProcessImage returns nullptr when image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, OnProcessImage_NullImage, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_InvalidDimensions
 * @tc.desc: Verify OnProcessImage returns nullptr when image has zero dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, OnProcessImage_InvalidDimensions, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->OnProcessImage(canvas_, imageEmpty_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_NoMask_NoNormalEffect
 * @tc.desc: Verify OnProcessImage uses NoNormalEffect path when mask is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, OnProcessImage_NoMask_NoNormalEffect, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_WithMask_NormalEffect
 * @tc.desc: Verify OnProcessImage uses DirectionLightEffect path when mask is set
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, OnProcessImage_WithMask_NormalEffect, TestSize.Level1)
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.5f, 0.5f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    auto mask = std::make_shared<Drawing::GERippleShaderMask>(maskParams);
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = mask;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: GetNormalMaskEffect
 * @tc.desc: Verify GetNormalMaskEffect returns non-null RuntimeEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, GetNormalMaskEffect, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_NE(filter->GetNormalMaskEffect(), nullptr);
}

/**
 * @tc.name: GetDirectionLightEffect
 * @tc.desc: Verify GetDirectionLightEffect returns non-null RuntimeEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, GetDirectionLightEffect, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_NE(filter->GetDirectionLightEffect(), nullptr);
}

/**
 * @tc.name: GetDirectionLightNoNormalEffect
 * @tc.desc: Verify GetDirectionLightNoNormalEffect returns non-null RuntimeEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEDirectionLightShaderFilterTest, GetDirectionLightNoNormalEffect, TestSize.Level1)
{
    Drawing::GEDirectionLightShaderFilterParams params;
    params.lightDirection = {0.0f, 0.0f, -1.0f};
    params.lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    params.lightIntensity = 1.0f;
    params.mask = nullptr;
    auto filter = std::make_shared<GEDirectionLightShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_NE(filter->GetDirectionLightNoNormalEffect(), nullptr);
}

} // namespace Rosen
} // namespace OHOS