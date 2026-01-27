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

#include "ge_color_remap_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEColorRemapShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEColorRemapShaderFilterTest::SetUpTestCase(void) {}

void GEColorRemapShaderFilterTest::TearDownTestCase(void) {}

void GEColorRemapShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEColorRemapShaderFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage with nullptr image
 * @tc.type:FUNC
 */
HWTEST_F(GEColorRemapShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEColorRemapFilterParams colorParams;
    auto colorRemapFilter = std::make_shared<GEColorRemapShaderFilter>(colorParams);
    ASSERT_TRUE(colorRemapFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(colorRemapFilter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage width valid params
 * @tc.type:FUNC
 */
HWTEST_F(GEColorRemapShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    // init valid colors params
    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    Vector4f color3 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    std::vector<Vector4f> colors = {color1, color3, color3};
    std::vector<float> positions = {0.2f, 0.1f, 0.7f};
    Drawing::GEColorRemapFilterParams colorParams = {colors, positions};
    auto colorRemapFilter = std::make_shared<GEColorRemapShaderFilter>(colorParams);
    ASSERT_TRUE(colorRemapFilter != nullptr);
    EXPECT_EQ(colorRemapFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage width invalid params
 * @tc.type:FUNC
 */
HWTEST_F(GEColorRemapShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    // init valid colors params
    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    Vector4f color3 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    Vector4f color4 = {1.0f, 1.0f, 0.5f, 1.0f}; // color rgba
    std::vector<Vector4f> colors = {color1, color3, color3, color4};
    std::vector<float> positions = {0.2f, 0.1f, 0.7f, 0.8f};
    Drawing::GEColorRemapFilterParams colorParams = {colors, positions};
    auto colorRemapFilter = std::make_shared<GEColorRemapShaderFilter>(colorParams);
    ASSERT_TRUE(colorRemapFilter != nullptr);
    EXPECT_EQ(colorRemapFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage width invalid params with positions is empty
 * @tc.type:FUNC
 */
HWTEST_F(GEColorRemapShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    // init colors params
    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    std::vector<Vector4f> colors = {color1, color2};
    std::vector<float> positions = {};
    Drawing::GEColorRemapFilterParams colorParams = {colors, positions};
    auto colorRemapFilter = std::make_shared<GEColorRemapShaderFilter>(colorParams);
    ASSERT_TRUE(colorRemapFilter != nullptr);
    EXPECT_EQ(colorRemapFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify function OnProcessImage width invalid params with colors is empty
 * @tc.type:FUNC
 */
HWTEST_F(GEColorRemapShaderFilterTest, OnProcessImage_005, TestSize.Level0)
{
    // init colors params
    std::vector<Vector4f> colors = {};
    std::vector<float> positions = {0.5f, 1.0f};
    Drawing::GEColorRemapFilterParams colorParams = {colors, positions};
    auto colorRemapFilter = std::make_shared<GEColorRemapShaderFilter>(colorParams);
    ASSERT_TRUE(colorRemapFilter != nullptr);
    EXPECT_EQ(colorRemapFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
