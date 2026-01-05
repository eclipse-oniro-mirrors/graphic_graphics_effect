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

#include "draw/canvas.h"
#include "draw/color.h"
#include "ge_double_ripple_shader_mask.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "sdf/ge_sdf_edge_light.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr size_t NUM_2 = 2;
constexpr size_t NUM_50 = 50;

class GESDFEdgeLightTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };    // input image
    std::shared_ptr<Drawing::Image> sdfImage_ { nullptr }; // sdf image

    Drawing::Rect src_ { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 200.0f, 200.0f };

private:
    Drawing::GESDFEdgeLightFilterParams MakeParams();
};

void GESDFEdgeLightTest::SetUpTestCase() {}

void GESDFEdgeLightTest::TearDownTestCase() {}

void GESDFEdgeLightTest::SetUp()
{
    // Build a 50x50 RGBA8888 bitmap and fill with BLUE, then convert to Image
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(NUM_50, NUM_50, fmt);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();

    // Build a 50x50 SDF image (use RED just as placeholder content)
    Drawing::Bitmap sdfBmp;
    sdfBmp.Build(NUM_50, NUM_50, fmt);
    sdfBmp.ClearWithColor(Drawing::Color::COLOR_RED);
    sdfImage_ = sdfBmp.MakeImage();
}

void GESDFEdgeLightTest::TearDown()
{
    image_ = nullptr;
    sdfImage_ = nullptr;
}

Drawing::GESDFEdgeLightFilterParams GESDFEdgeLightTest::MakeParams()
{
    Drawing::GESDFEdgeLightFilterParams p;
    p.sdfSpreadFactor = 64.0f;
    p.bloomIntensityCutoff = 0.1f;
    p.maxIntensity = 1.0f;
    p.maxBloomIntensity = 1.0f;
    p.bloomFalloffPow = 2.0f;
    p.minBorderWidth = 2.0f;
    p.maxBorderWidth = 5.0f;
    p.innerBorderBloomWidth = 30.0f;
    p.outerBorderBloomWidth = 30.0f;
    // sdfImage and lightMask can be set in the params before constructing the filter
    return p;
}

/**
 * @tc.name: OnProcessImage_InvalidInputs
 * @tc.desc: Verify OnProcessImage returns nullptr when input image is invalid (nullptr),
 *           and returns original image when required resources (sdf or mask) are missing.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightTest, OnProcessImage_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto filter = std::make_unique<GESDFEdgeLight>(params);

    // Case 1: image == nullptr
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    ASSERT_NE(image_, nullptr);

    // Case 2: sdfImage_ not set → should return original image
    auto out2 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out2.get(), image_.get());

    // Case 3: sdf set but mask missing → should return original image
    auto params3 = MakeParams();
    params3.sdfImage = sdfImage_;
    // lightMask is not set (nullptr)
    auto filter3 = std::make_unique<GESDFEdgeLight>(params3);
    auto out3 = filter3->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out3.get(), image_.get());

    // Case 4: mask set but sdf missing → should return original image
    Drawing::GEDoubleRippleShaderMaskParams mp;
    auto mask = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
    auto params4 = MakeParams();
    params4.lightMask = mask;
    // sdfImage and sdfShape are not set (nullptr)
    auto filter4 = std::make_unique<GESDFEdgeLight>(params4);
    auto out4 = filter4->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out4.get(), image_.get());
}

/**
 * @tc.name: OnProcessImage_CPUFallback
 * @tc.desc: On CPU Canvas (no GPU context), final image build is likely nullptr → filter returns original image.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightTest, OnProcessImage_CPUFallback, TestSize.Level0)
{
    ASSERT_NE(image_, nullptr);
    ASSERT_NE(sdfImage_, nullptr);

    // Provide required resources via constructor params
    auto params = MakeParams();
    params.sdfImage = sdfImage_;
    Drawing::GEDoubleRippleShaderMaskParams mp; // default mask params
    params.lightMask = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
    auto filter = std::make_unique<GESDFEdgeLight>(params);

    // On CPU canvas, builder->MakeImage usually returns nullptr, so OnProcessImage should return the input image.
    auto out = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out.get(), image_.get());
}

/**
 * @tc.name: ParameterVariations_Smoke
 * @tc.desc: Verify different parameter combinations work correctly on CPU (returns original image).
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightTest, ParameterVariations_Smoke, TestSize.Level0)
{
    ASSERT_NE(image_, nullptr);
    ASSERT_NE(sdfImage_, nullptr);

    // Test with initial SDF image
    auto params1 = MakeParams();
    params1.sdfImage = sdfImage_;
    Drawing::GEDoubleRippleShaderMaskParams mp;
    params1.lightMask = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
    auto filter1 = std::make_unique<GESDFEdgeLight>(params1);
    auto out1 = filter1->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out1.get(), image_.get());

    // Test with different SDF image
    Drawing::Bitmap newSdfBmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    newSdfBmp.Build(NUM_2, NUM_2, fmt);
    newSdfBmp.ClearWithColor(Drawing::Color::COLOR_GREEN);
    auto newSdfImage = newSdfBmp.MakeImage();
    ASSERT_NE(newSdfImage, nullptr);

    auto params2 = MakeParams();
    params2.sdfImage = newSdfImage;
    params2.lightMask = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
    auto filter2 = std::make_unique<GESDFEdgeLight>(params2);
    auto out2 = filter2->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(out2.get(), image_.get());
}

} // namespace Rosen
} // namespace OHOS
