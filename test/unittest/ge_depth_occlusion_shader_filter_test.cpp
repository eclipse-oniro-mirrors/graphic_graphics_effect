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
#include "ge_depth_occlusion_shader_filter.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace GraphicsEffectEngine {

class GEDepthOcclusionShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Surface> CreateSurface();

    static inline Rosen::Drawing::Canvas canvas_;
    std::shared_ptr<Rosen::Drawing::Image> image_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> imageEmpty_ { nullptr };
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvasGpu_ = nullptr;
    Drawing::ImageInfo imageInfo_ = {};

    Rosen::Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Rosen::Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEDepthOcclusionShaderFilterTest::SetUpTestCase(void) {}
void GEDepthOcclusionShaderFilterTest::TearDownTestCase(void) {}

void GEDepthOcclusionShaderFilterTest::SetUp()
{
    canvas_.Restore();
 
    Rosen::Drawing::Bitmap bmp;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::COLORTYPE_RGBA_8888, Rosen::Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Rosen::Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    bmp.Build(0, 0, format);
    imageEmpty_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
    dst_ = src_;
    imageInfo_ = Drawing::ImageInfo { dst_.GetWidth(), dst_.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    surface_ = CreateSurface();
    if (surface_) {
        canvasGpu_ = surface_->GetCanvas();
    }
}

void GEDepthOcclusionShaderFilterTest::TearDown()
{
    image_ = nullptr;
    imageEmpty_ = nullptr;
}

std::shared_ptr<Drawing::Surface> GEDepthOcclusionShaderFilterTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEDepthOcclusionShaderFilterTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

std::shared_ptr<GEDepthOcclusionShaderFilter> CreateDepthOcclusionShaderFilter(
    Drawing::GEDepthOcclusionShaderFilterParams params)
{
    return std::make_shared<GEDepthOcclusionShaderFilter>(params);
}

std::shared_ptr<GEDepthOcclusionShaderFilter> CreateDepthOcclusionShaderFilter()
{
    Drawing::GEDepthOcclusionShaderFilterParams params = {
        Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
        Drawing::Matrix(),
        nullptr,
        1.0f,
        0.1f,
        100.f,
        false
    };
    return CreateDepthOcclusionShaderFilter(params);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, Type_001, TestSize.Level0)
{
    auto geDepthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter();
    EXPECT_EQ(geDepthOcclusionShaderFilter->Type(), Drawing::GEFilterType::DEPTH_OCCLUSION);
    EXPECT_EQ(geDepthOcclusionShaderFilter->TypeName(), Drawing::GE_FILTER_DEPTH_OCCLUSION);
}

/**
 * @tc.name: GEDepthOcclusionShaderFilterCreateTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.GEDepthOcclusionShaderFilterCreateTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, GEDepthOcclusionShaderFilterCreateTest, TestSize.Level1)
{
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter();
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    // thread_local instance
    EXPECT_TRUE(depthOcclusionShaderFilter->MakeDepthOcclusionShaderEffect());
    EXPECT_TRUE(depthOcclusionShaderFilter->MakeDepthOcclusionMaskShaderEffect());
    // thread_local return
    EXPECT_TRUE(depthOcclusionShaderFilter->MakeDepthOcclusionShaderEffect());
    EXPECT_TRUE(depthOcclusionShaderFilter->MakeDepthOcclusionMaskShaderEffect());
}

/**
 * @tc.name: IsInputImageValidTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.IsInputImageValidTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, IsInputImageValidTest, TestSize.Level1)
{
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter();
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    EXPECT_FALSE(depthOcclusionShaderFilter->IsInputImageValid(nullptr));
    EXPECT_TRUE(depthOcclusionShaderFilter->IsInputImageValid(image_));
    EXPECT_FALSE(depthOcclusionShaderFilter->IsInputImageValid(imageEmpty_));
}

/**
 * @tc.name: OnProcessImageTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.OnProcessImageTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, OnProcessImageTest, TestSize.Level1)
{
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter();
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    EXPECT_EQ(depthOcclusionShaderFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    EXPECT_EQ(depthOcclusionShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImageWithDepthMapTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.OnProcessImageWithDepthMapTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, OnProcessImageWithDepthMapTest, TestSize.Level1)
{
    Rosen::Drawing::Bitmap depthBmp;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::COLORTYPE_RGBA_8888, Rosen::Drawing::ALPHATYPE_PREMUL };
    depthBmp.Build(50, 50, format);
    depthBmp.ClearWithColor(Rosen::Drawing::Color::COLOR_RED);
    auto depthMap = depthBmp.MakeImage();

    Drawing::GEDepthOcclusionShaderFilterParams params = {
        Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
        Drawing::Matrix(),
        depthMap,
        1.0f,
        0.1f,
        100.f,
        false
    };
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter(params);
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    EXPECT_EQ(depthOcclusionShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImageWithReverseTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.OnProcessImageWithReverseTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, OnProcessImageWithReverseTest, TestSize.Level1)
{
    Drawing::GEDepthOcclusionShaderFilterParams params = {
        Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
        Drawing::Matrix(),
        nullptr,
        1.0f,
        0.1f,
        100.f,
        true
    };
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter(params);
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    EXPECT_EQ(depthOcclusionShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImageOnGPUTest
 * @tc.desc: GEDepthOcclusionShaderFilterTest.OnProcessImageOnGPUTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDepthOcclusionShaderFilterTest, OnProcessImageOnGPUTest, TestSize.Level1)
{
    ASSERT_NE(canvasGpu_, nullptr);
    Rosen::Drawing::Bitmap depthBmp;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::COLORTYPE_RGBA_8888, Rosen::Drawing::ALPHATYPE_PREMUL };
    depthBmp.Build(dst_.GetWidth(), dst_.GetHeight(), format);
    depthBmp.ClearWithColor(Rosen::Drawing::Color::COLOR_RED);
    auto depthMap = depthBmp.MakeImage();

    Drawing::GEDepthOcclusionShaderFilterParams params = {
        Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
        Drawing::Matrix(),
        depthMap,
        1.0f,
        0.1f,
        100.f,
        false
    };
    auto depthOcclusionShaderFilter = CreateDepthOcclusionShaderFilter(params);
    ASSERT_NE(depthOcclusionShaderFilter, nullptr);

    ASSERT_NE(depthOcclusionShaderFilter->OnProcessImage(*canvasGpu_, image_, src_, dst_), nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace Rosen
} // namespace OHOS
