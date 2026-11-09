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

#include "ge_binarization_shader_filter.h"

#include "image/bitmap.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBinarizationShaderFilterTest : public testing::Test {
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

void GEBinarizationShaderFilterTest::SetUpTestCase() {}
void GEBinarizationShaderFilterTest::TearDownTestCase() {}

void GEBinarizationShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(IMAGE_WIDTH, IMAGE_HEIGHT, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_WHITE);
    image_ = bmp.MakeImage();
}

void GEBinarizationShaderFilterTest::TearDown() {}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor with normal params
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, Constructor_001, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: FilterType_001
 * @tc.desc: Verify Type() returns BINARIZATION
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, FilterType_001, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::BINARIZATION);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage with valid image
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_001, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    ASSERT_NE(renderContext, nullptr);
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto context = renderContext->GetSharedDrGPUContext();
    ASSERT_NE(context, nullptr);
    Drawing::ImageInfo imageInfo { IMAGE_WIDTH, IMAGE_HEIGHT, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    auto surface = Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo);
    ASSERT_NE(surface, nullptr);
    auto gpuCanvas = surface->GetCanvas();
    ASSERT_NE(gpuCanvas, nullptr);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(IMAGE_WIDTH, IMAGE_HEIGHT, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_WHITE);
    auto gpuImage = bmp.MakeImage();
    ASSERT_NE(gpuImage, nullptr);

    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(*gpuCanvas, gpuImage, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with null image returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_002, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with zero thresholdRange
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_003, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.0f;
    params.high = 1.0f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.0f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify OnProcessImage with out-of-range params returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_004, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = -0.5f;
    params.high = 1.5f;
    params.threshold = 2.0f;
    params.thresholdRange = -1.0f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify OnProcessImage with NaN params returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_005, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = NAN;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_006
 * @tc.desc: Verify OnProcessImage with Inf params returns original image
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_006, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = INFINITY;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_007
 * @tc.desc: Verify OnProcessImage with low > 1.0 triggers condition2
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_007, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 1.5f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_008
 * @tc.desc: Verify OnProcessImage with high < 0.0 triggers condition3
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_008, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = -0.5f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_009
 * @tc.desc: Verify OnProcessImage with threshold < 0.0 triggers condition5
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_009, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = -0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: OnProcessImage_010
 * @tc.desc: Verify OnProcessImage with thresholdRange > 1.0 returns triggers condition8
 * @tc.type: FUNC
 */
HWTEST_F(GEBinarizationShaderFilterTest, OnProcessImage_010, TestSize.Level1)
{
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 1.5f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, image_);
}

} // namespace Rosen
} // namespace OHOS
