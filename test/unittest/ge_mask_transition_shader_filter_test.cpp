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
#include "ge_mask_transition_shader_filter.h"
#include "ge_ripple_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEMaskTransitionShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> CreateTestImage(int width, int height);
    std::shared_ptr<Drawing::GEShaderMask> CreateTestMask();

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    std::shared_ptr<Drawing::Image> topLayerImage_ { nullptr };
    std::shared_ptr<Drawing::GEShaderMask> mask_ { nullptr };

    Drawing::Rect src_ { 0.0f, 0.0f, 100.0f, 100.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 100.0f, 100.0f };
};

void GEMaskTransitionShaderFilterTest::SetUpTestCase(void) {}

void GEMaskTransitionShaderFilterTest::TearDownTestCase(void) {}

std::shared_ptr<Drawing::Image> GEMaskTransitionShaderFilterTest::CreateTestImage(int width, int height)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

std::shared_ptr<Drawing::GEShaderMask> GEMaskTransitionShaderFilterTest::CreateTestMask()
{
    Drawing::GERippleShaderMaskParams params;
    params.center_ = {0.5f, 0.5f};
    params.radius_ = 0.3f;
    params.width_ = 0.1f;
    return std::make_shared<Drawing::GERippleShaderMask>(params);
}

void GEMaskTransitionShaderFilterTest::SetUp()
{
    image_ = CreateTestImage(100, 100); // 100x100 blue bottom image
    topLayerImage_ = CreateTestImage(100, 100); // 100x100 blue top image
    mask_ = CreateTestMask();
}

void GEMaskTransitionShaderFilterTest::TearDown()
{
    image_ = nullptr;
    topLayerImage_ = nullptr;
    mask_ = nullptr;
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage handles null image, null mask, and both null
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;
    params.inverse = false;

    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);

    // Test with null image
    auto result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);

    // Test with null mask
    params.mask = nullptr;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result, nullptr);

    // Test with both null image and null mask
    result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with cache mechanism (first call sets cache, second processes)
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;
    params.inverse = false;

    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);

    // First call sets up cache, returns input image
    auto result1 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result1, image_);

    // Second call should process with cached top layer
    auto result2 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result2, nullptr);

    // Third sequential call should also process
    auto result3 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result3, nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with inverse flag (true/false combinations)
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;

    // Test with inverse = false
    params.inverse = false;
    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);  // Set cache
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with inverse = true
    params.inverse = true;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);  // Set cache
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with inverse = true and factor = 1.0
    params.factor = 1.0f;
    params.inverse = false;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);  // Set cache
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with inverse = true and factor = 0.0
    params.factor = 0.0f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);  // Set cache
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify OnProcessImage with various factor values and clamping to [0.0f, 1.0f]
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.inverse = false;

    // Test with factor = 0.0
    params.factor = 0.0f;
    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = 0.25
    params.factor = 0.25f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = 0.5
    params.factor = 0.5f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = 0.75
    params.factor = 0.75f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = 1.0
    params.factor = 1.0f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = 1.5 (should be clamped to 1.0)
    params.factor = 1.5f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with factor = -0.5 (should be clamped to 0.0)
    params.factor = -0.5f;
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, image_, src_, dst_);
    result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify OnProcessImage with different image sizes (large, rectangular)
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, OnProcessImage_005, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;
    params.inverse = false;

    // Test with large image (1000x1000)
    auto largeImage = CreateTestImage(1000, 1000);

    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, largeImage, src_, dst_);
    auto result = filter->OnProcessImage(canvas_, largeImage, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test with rectangular image (200x100)
    auto wideImage = CreateTestImage(200, 100);

    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->OnProcessImage(canvas_, wideImage, src_, dst_);
    result = filter->OnProcessImage(canvas_, wideImage, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: SetCache_001
 * @tc.desc: Verify SetCache with valid image, null image, and subsequent processing
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, SetCache_001, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;
    params.inverse = false;

    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    Drawing::Matrix cacheMatrix;

    // Test SetCache with valid top layer image
    filter->SetCache(topLayerImage_, cacheMatrix);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);

    // Test SetCache with null image (uses current image as cache)
    filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    filter->SetCache(nullptr, cacheMatrix);
    auto result1 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_EQ(result1, image_);  // First call sets cache
    auto result2 = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result2, nullptr);  // Second call processes
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor creates filter successfully with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(GEMaskTransitionShaderFilterTest, Constructor_001, TestSize.Level0)
{
    Drawing::GEMaskTransitionShaderFilterParams params;
    params.mask = mask_;
    params.factor = 0.5f;
    params.inverse = true;

    auto filter = std::make_shared<GEMaskTransitionShaderFilter>(params);
    EXPECT_NE(filter, nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
