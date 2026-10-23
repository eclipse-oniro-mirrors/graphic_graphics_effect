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

    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 0.0f, 0.0f, 50.0f, 50.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 50.0f, 50.0f };

    const int imageWidth_ = 50;
    const int imageHeight_ = 50;
};

void GEBinarizationShaderFilterTest::SetUpTestCase() {}
void GEBinarizationShaderFilterTest::TearDownTestCase() {}

std::shared_ptr<Drawing::Surface> GEBinarizationShaderFilterTest::CreateSurface()
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto gpuContext = renderContext->GetSharedDrGPUContext();
    if (gpuContext == nullptr) {
        return nullptr;
    }
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    Drawing::Bitmap bmp;
    bmp.Build(imageWidth_, imageHeight_, format);
    auto surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, bmp.GetImageInfo());
    return surface;
}

void GEBinarizationShaderFilterTest::SetUp()
{
    surface_ = CreateSurface();
    ASSERT_NE(surface_, nullptr);
    canvas_ = surface_->GetCanvas();
    ASSERT_NE(canvas_, nullptr);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_WHITE);
    image_ = bmp.MakeImage();
    ASSERT_NE(image_, nullptr);
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
    Drawing::GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(*canvas_, image_, src_, dst_);
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
    auto result = filter->OnProcessImage(*canvas_, nullptr, src_, dst_);
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
    auto result = filter->OnProcessImage(*canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

} // namespace Rosen
} // namespace OHOS
