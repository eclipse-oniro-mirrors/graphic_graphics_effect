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

#include "ge_frosted_glass_blur_shader_filter.h"

#include "draw/color.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
constexpr size_t NUM_50 = 50;
class GEFrostedGlassBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;          // CPU canvas (no GPU context)
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f → left, top, right, bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 200.0f, 200.0f };

private:
    Drawing::GEFrostedGlassBlurShaderFilterParams MakeParams();
    Drawing::CanvasInfo canvasInfo_;
};

void GEFrostedGlassBlurShaderFilterTest::SetUpTestCase() {}

void GEFrostedGlassBlurShaderFilterTest::TearDownTestCase() {}

void GEFrostedGlassBlurShaderFilterTest::SetUp()
{
    // Build a 50x50 RGBA8888 bitmap and fill with BLUE, then convert to Image
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(NUM_50, NUM_50, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEFrostedGlassBlurShaderFilterTest::TearDown()
{
    image_ = nullptr;
}

Drawing::GEFrostedGlassBlurShaderFilterParams GEFrostedGlassBlurShaderFilterTest::MakeParams()
{
    Drawing::GEFrostedGlassBlurShaderFilterParams p; // Default parameters
    p.radius = 100.0f;
    p.refractOutPx = 10.0f;
    return p;
}

/**
 * @tc.name: OnProcessImage_InvalidInputs
 * @tc.desc: Verify OnProcessImage returns nullptr when input image is invalid (nullptr or zero dimension).
 * @tc.type: FUNC
 */
HWTEST_F(GEFrostedGlassBlurShaderFilterTest, OnProcessImage_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GEFrostedGlassBlurShaderFilter>(params);

    // Case 1: image == nullptr
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    // Prepare a bitmap helper
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

    // Case 2: width > 0, height == 0
    bmp.Build(NUM_1, NUM_0, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto imgH0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgH0, src_, dst_), nullptr);

    // Case 3: width == 0, height > 0
    bmp.Build(NUM_0, NUM_1, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLACK);
    auto imgW0 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, imgW0, src_, dst_), nullptr);

    // Case 4: width == 0, height == 0
    bmp.Build(NUM_0, NUM_0, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto img00 = bmp.MakeImage();
    EXPECT_EQ(filter->OnProcessImage(canvas_, img00, src_, dst_), nullptr);
}

} // namespace Rosen
} // namespace OHOS
