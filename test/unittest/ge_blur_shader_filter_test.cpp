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

#include "ge_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 1.0f, 1.0f, 51.0f, 51.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 51.0f, 51.0f };

    const int imageWidth_ = 50;
    const int imageHeight_ = 50;
};

void GEBlurShaderFilterTest::SetUpTestCase() {}
void GEBlurShaderFilterTest::TearDownTestCase() {}

void GEBlurShaderFilterTest::SetUp()
{
    surface_ = CreateSurface();
    ASSERT_NE(surface_, nullptr);
    canvas_ = surface_->GetCanvas();
    ASSERT_NE(canvas_, nullptr);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

std::shared_ptr<Drawing::Surface> GEBlurShaderFilterTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEBlurShaderFilterTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo {
        100,
        100,
        Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo);
}

void GEBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadiusX_001
 * @tc.desc: Verify function GetRadiusX returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, GetRadiusX_001, TestSize.Level2)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = false;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.radiusX, 10.0f);
}

/**
 * @tc.name: GetRadiusY_001
 * @tc.desc: Verify function GetRadiusY returns correct value
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, GetRadiusY_001, TestSize.Level2)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 15.0f;
    params.expandDrawRegion = false;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.radiusY, 15.0f);
}

/**
 * @tc.name: ExpandDrawRegion_001
 * @tc.desc: Verify expandDrawRegion parameter can be set to false
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ExpandDrawRegion_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = false;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.expandDrawRegion, false);
}

/**
 * @tc.name: ExpandDrawRegion_002
 * @tc.desc: Verify expandDrawRegion parameter can be set to true
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ExpandDrawRegion_002, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 20.0f;
    params.radiusY = 20.0f;
    params.expandDrawRegion = true;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.expandDrawRegion, true);
}

/**
 * @tc.name: ExpandDrawRegionDefault_001
 * @tc.desc: Verify expandDrawRegion default value is false
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ExpandDrawRegionDefault_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.expandDrawRegion, false);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage with valid image and no expandDrawRegion
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, OnProcessImage_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = false;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(*canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with valid image and expandDrawRegion enabled
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, OnProcessImage_002, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 20.0f;
    params.radiusY = 20.0f;
    params.expandDrawRegion = true;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(*canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with nullptr image returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, OnProcessImage_003, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    auto result = filter->OnProcessImage(*canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: RadiusBoundary_001
 * @tc.desc: Verify radiusX with zero value
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, RadiusBoundary_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 0.0f;
    params.radiusY = 0.0f;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.radiusX, 0.0f);
    EXPECT_EQ(filter->blurParams_.radiusY, 0.0f);
}

/**
 * @tc.name: RadiusBoundary_002
 * @tc.desc: Verify radiusX with negative value
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, RadiusBoundary_002, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = -10.0f;
    params.radiusY = -5.0f;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->blurParams_.radiusX, -10.0f);
    EXPECT_EQ(filter->blurParams_.radiusY, -5.0f);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify GEBlurShaderFilter Type returns correct GEFilterType
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, Type_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::BLUR);
}

/**
 * @tc.name: ProcessImageWithMesa_NullSurface_001
 * @tc.desc: Verify ProcessImageWithMesa returns original image when canvas has no surface
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ProcessImageWithMesa_NullSurface_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = true;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    Drawing::Canvas canvasNoSurface;
    auto result = filter->ProcessImageWithMesa(canvasNoSurface, image_, src_, 10);
    EXPECT_EQ(result, image_);
}

/**
 * @tc.name: ProcessImageWithMesa_NullImage_001
 * @tc.desc: Verify ProcessImageWithMesa returns nullptr when image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ProcessImageWithMesa_NullImage_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = true;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto result = filter->ProcessImageWithMesa(*canvas_, nullptr, src_, 10);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ProcessImageWithMesa_MakeSurfaceFail_001
 * @tc.desc: Verify ProcessImageWithMesa returns image when make surface failed
 * @tc.type: FUNC
 */
HWTEST_F(GEBlurShaderFilterTest, ProcessImageWithMesa_MakeSurfaceFail_001, TestSize.Level1)
{
    Drawing::GEBlurShaderFilterParams params;
    params.radiusX = 10.0f;
    params.radiusY = 10.0f;
    params.expandDrawRegion = true;
    auto filter = std::make_shared<GEBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto result = filter->ProcessImageWithMesa(*canvas_, image_, src_, -100);
    EXPECT_EQ(result, image_);
}
} // namespace Rosen
} // namespace OHOS