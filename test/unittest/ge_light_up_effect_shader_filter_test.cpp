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

#include "ge_light_up_effect_shader_filter.h"

#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GELightUpEffectShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    static constexpr int32_t IMAGE_WIDTH = 50;
    static constexpr int32_t IMAGE_HEIGHT = 50;

    Drawing::Rect src_ { 0.0f, 0.0f, 50.0f, 50.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 50.0f, 50.0f };
};

void GELightUpEffectShaderFilterTest::SetUpTestCase() {}
void GELightUpEffectShaderFilterTest::TearDownTestCase() {}

void GELightUpEffectShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(IMAGE_WIDTH, IMAGE_HEIGHT, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_WHITE);
    image_ = bmp.MakeImage();
}

void GELightUpEffectShaderFilterTest::TearDown() {}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor with normal params
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, Constructor_001, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = 0.5f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: FilterType_001
 * @tc.desc: Verify Type() returns LIGHT_UP_EFFECT
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, FilterType_001, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::LIGHT_UP_EFFECT);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage with valid image
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_001, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = 0.5f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with null image returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_002, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with boundary value 0.0
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_003, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = 0.0f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify OnProcessImage with out-of-range value returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_004, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = 1.5f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify OnProcessImage with NaN value returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_005, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = NAN;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_006
 * @tc.desc: Verify OnProcessImage with Inf value returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GELightUpEffectShaderFilterTest, OnProcessImage_006, TestSize.Level1)
{
    Drawing::GELightUpEffectShaderFilterParams params;
    params.value = INFINITY;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

} // namespace Rosen
} // namespace OHOS
