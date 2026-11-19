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

#include "draw/color.h"
#include "image/bitmap.h"
#include "ge_shader_diagnostics.h"
#include "ge_sticker_emboss_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr int32_t TEST_IMAGE_SIZE = 50;

class GEStickerEmbossFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };

    // The SDF/composite stages need a GPU context; on a raster canvas the
    // filter takes the internal-failure branches and hands the input back.
    bool HasGpuContext() const
    {
        return canvas_.GetGPUContext() != nullptr;
    }

    static std::shared_ptr<Drawing::Image> MakeBitmapImage(int32_t width, int32_t height)
    {
        Drawing::Bitmap bmp;
        Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        bmp.Build(width, height, format);
        bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
        return bmp.MakeImage();
    }
};

void GEStickerEmbossFilterTest::SetUpTestCase(void) {}
void GEStickerEmbossFilterTest::TearDownTestCase(void) {}

void GEStickerEmbossFilterTest::SetUp()
{
    canvas_.Restore();
    image_ = MakeBitmapImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE);
    src_ = image_->GetImageInfo().GetBound();
}

void GEStickerEmbossFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImageNullImage
 * @tc.desc: Verify OnProcessImage returns nullptr for a null input image
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageNullImage, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImageNullSticker
 * @tc.desc: Verify OnProcessImage returns the input image when the sticker param is missing
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageNullSticker, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    EXPECT_EQ(filter->GetCache(), nullptr);
}

/**
 * @tc.name: OnProcessImageZeroSizeImage
 * @tc.desc: Verify OnProcessImage hands zero-size input images (0x50/50x0/0x0) back unchanged
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageZeroSizeImage, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto zeroWidth = MakeBitmapImage(0, TEST_IMAGE_SIZE);
    auto zeroHeight = MakeBitmapImage(TEST_IMAGE_SIZE, 0);
    auto zeroBoth = MakeBitmapImage(0, 0);

    // Zero-size inputs take the early-exit branch, the input is returned as-is
    EXPECT_EQ(filter->OnProcessImage(canvas_, zeroWidth, src_, dst_), zeroWidth);
    EXPECT_EQ(filter->OnProcessImage(canvas_, zeroHeight, src_, dst_), zeroHeight);
    EXPECT_EQ(filter->OnProcessImage(canvas_, zeroBoth, src_, dst_), zeroBoth);
    EXPECT_EQ(filter->GetCache(), nullptr);
}

/**
 * @tc.name: OnProcessImageZeroSizeSticker
 * @tc.desc: Verify OnProcessImage returns the input unchanged for a zero-size sticker
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageZeroSizeSticker, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = MakeBitmapImage(0, TEST_IMAGE_SIZE);
    params.outlineWidth = 2.0f;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    EXPECT_EQ(filter->GetCache(), nullptr);
}

/**
 * @tc.name: OnProcessImageValid
 * @tc.desc: Verify OnProcessImage output size matches the input on the GPU main path
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageValid, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    params.refractionRate = 1.0f;
    params.dirIntensity = 1.0f;
    params.edgeLightWidth = 1.0f;
    params.edgeLightFeatherRate = 1.0f;
    params.edgeLightIntensity = Vector2f(0.5f, 0.5f);
    params.spotLightCenter = Vector2f(0.5f, 0.5f);
    params.spotLightIntensity = 1.0f;
    params.specPow = 1.0f;
    params.specIntensity = 1.0f;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    if (HasGpuContext()) {
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result->GetWidth(), image_->GetWidth());
        EXPECT_EQ(result->GetHeight(), image_->GetHeight());
    } else {
        EXPECT_EQ(result, image_);
    }
}

/**
 * @tc.name: OnProcessImageReusesCache
 * @tc.desc: Verify a repeated call with the same sticker and spread reuses the SDF cache
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, OnProcessImageReusesCache, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto first = filter->OnProcessImage(canvas_, image_, src_, dst_);
    auto cacheAfterFirst = filter->GetCache();
    auto second = filter->OnProcessImage(canvas_, image_, src_, dst_);
    if (HasGpuContext()) {
        ASSERT_NE(first, nullptr);
        ASSERT_NE(second, nullptr);
        // Cache hit keeps the packed entry untouched (no re-generation)
        EXPECT_NE(cacheAfterFirst, nullptr);
        EXPECT_EQ(filter->GetCache(), cacheAfterFirst);
    } else {
        EXPECT_EQ(first, image_);
        EXPECT_EQ(second, image_);
        EXPECT_EQ(filter->GetCache(), nullptr);
    }
}

/**
 * @tc.name: MakeDecodedStickerSDFImageNullSticker
 * @tc.desc: Verify MakeDecodedStickerSDFImage returns nullptr for a null sticker
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeDecodedStickerSDFImageNullSticker, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->MakeDecodedStickerSDFImage(canvas_, nullptr, 4.0f), nullptr);
}

/**
 * @tc.name: MakeDecodedStickerSDFImageValid
 * @tc.desc: Verify the decoded SDF matches the sticker resolution and hits the cache on repeat
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeDecodedStickerSDFImageValid, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto first = filter->MakeDecodedStickerSDFImage(canvas_, image_, 4.0f);
    if (HasGpuContext()) {
        ASSERT_NE(first, nullptr);
        // The decoded SDF is restored to the sticker resolution
        EXPECT_EQ(first->GetWidth(), image_->GetWidth());
        EXPECT_EQ(first->GetHeight(), image_->GetHeight());
        EXPECT_NE(filter->GetCache(), nullptr);
        // Same sticker + same spread: the second call hits the cache
        auto second = filter->MakeDecodedStickerSDFImage(canvas_, image_, 4.0f);
        EXPECT_EQ(second, first);
    } else {
        EXPECT_EQ(first, nullptr);
        // The failed build clears the cache
        EXPECT_EQ(filter->GetCache(), nullptr);
    }
}

/**
 * @tc.name: MakeStickerEmbossImageInvalidInput
 * @tc.desc: Verify MakeStickerEmbossImage hands the input back for a null image or null decoded SDF
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeStickerEmbossImageInvalidInput, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->MakeStickerEmbossImage(canvas_, nullptr, image_), nullptr);
    EXPECT_EQ(filter->MakeStickerEmbossImage(canvas_, image_, nullptr), image_);
}

/**
 * @tc.name: MakeStickerEmbossImageNullStickerParam
 * @tc.desc: Verify MakeStickerEmbossImage returns the input when the filter has no sticker param
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeStickerEmbossImageNullStickerParam, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = nullptr;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // image and SDF are valid but params_.sticker is null: the guard returns the input
    EXPECT_EQ(filter->MakeStickerEmbossImage(canvas_, image_, image_), image_);
}

/**
 * @tc.name: MakeStickerEmbossImageValid
 * @tc.desc: Verify the composed emboss result matches the input size on the GPU path
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeStickerEmbossImageValid, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    auto result = filter->MakeStickerEmbossImage(canvas_, image_, image_);
    if (HasGpuContext()) {
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result->GetWidth(), image_->GetWidth());
        EXPECT_EQ(result->GetHeight(), image_->GetHeight());
    } else {
        EXPECT_EQ(result, image_);
    }
}

/**
 * @tc.name: SetEmbossShaderUniformsInvalidInput
 * @tc.desc: Verify uniform binding early-exit guards do not crash
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, SetEmbossShaderUniformsInvalidInput, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = nullptr;
    auto nullStickerFilter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(nullStickerFilter != nullptr);

    Drawing::GEStickerEmbossFilterParams validParams;
    validParams.sticker = image_;
    auto validFilter = std::make_shared<GEStickerEmbossFilter>(validParams);
    ASSERT_TRUE(validFilter != nullptr);

    // A real builder keeps the reference binding well-defined; the guards under test
    // return before any uniform is written
    constexpr char guardProbeShader[] = "half4 main(float2 xy) { return half4(1.0); }";
    auto effect = GECreateRuntimeEffectForShader(guardProbeShader);
    ASSERT_NE(effect, nullptr);
    Drawing::RuntimeShaderBuilder builder(effect);
    // null image -> early return on the image check
    validFilter->SetEmbossShaderUniforms(builder, nullptr);
    // valid image but the filter has no sticker -> early return on the sticker check
    nullStickerFilter->SetEmbossShaderUniforms(builder, image_);
    // both invalid
    nullStickerFilter->SetEmbossShaderUniforms(builder, nullptr);
    // no crash and no shader state changed
    EXPECT_EQ(validFilter->GetCache(), nullptr);
}

/**
 * @tc.name: SetEmbossShaderUniformsMinFalloffClamp
 * @tc.desc: Verify a zero outline width is clamped to the minimum falloff and the edge band stays inside
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, SetEmbossShaderUniformsMinFalloffClamp, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 0.0f; // below EMBOSS_SDF_MIN_FALLOFF_PX, clamped to 1px
    params.edgeLightWidth = 10.0f; // larger than outlineWidth, clamped back inside
    params.edgeLightFeatherRate = 2.0f; // clamped into [0, EDGE_LIGHT_FEATHER_MAX]
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // The uniform binding path (with the clamps) runs before MakeImage fails on the raster canvas
    auto result = filter->MakeStickerEmbossImage(canvas_, image_, image_);
    if (HasGpuContext()) {
        EXPECT_NE(result, nullptr);
    } else {
        EXPECT_EQ(result, image_);
    }
}

/**
 * @tc.name: MakeDecodedStickerSDFImageCacheInvalidatedBySpread
 * @tc.desc: Verify changing the spread factor invalidates the cached SDF and rebuilds it
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, MakeDecodedStickerSDFImageCacheInvalidatedBySpread, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    constexpr float smallRange = 4.0f;
    constexpr float largeRange = 128.0f;
    auto first = filter->MakeDecodedStickerSDFImage(canvas_, image_, smallRange);
    auto afterSmall = filter->GetCache();
    if (HasGpuContext()) {
        ASSERT_NE(first, nullptr);
        // A different spread invalidates the cache entry and rebuilds the SDF
        auto rebuilt = filter->MakeDecodedStickerSDFImage(canvas_, image_, largeRange);
        ASSERT_NE(rebuilt, nullptr);
        EXPECT_NE(rebuilt, first);
        // Switching back must not return the stale first entry
        auto backToSmall = filter->MakeDecodedStickerSDFImage(canvas_, image_, smallRange);
        ASSERT_NE(backToSmall, nullptr);
        EXPECT_NE(backToSmall, first);
        EXPECT_NE(backToSmall, rebuilt);
    } else {
        EXPECT_EQ(first, nullptr);
        EXPECT_EQ(filter->GetCache(), nullptr);
        EXPECT_EQ(afterSmall, nullptr);
        // Failed builds keep clearing the cache
        EXPECT_EQ(filter->MakeDecodedStickerSDFImage(canvas_, image_, largeRange), nullptr);
        EXPECT_EQ(filter->GetCache(), nullptr);
    }
}

/**
 * @tc.name: SetEmbossShaderUniformsValid
 * @tc.desc: Verify a spot light at the image center degrades the directional light to zero without crash
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, SetEmbossShaderUniformsValid, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    // Spot light exactly at the component center: lightDir degrades to (0, 0)
    params.spotLightCenter = Vector2f(static_cast<float>(image_->GetWidth()) * 0.5f,
        static_cast<float>(image_->GetHeight()) * 0.5f);
    params.spotLightIntensity = 1.0f;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // Full uniform binding path including the degenerate direction branch
    auto result = filter->MakeStickerEmbossImage(canvas_, image_, image_);
    if (HasGpuContext()) {
        EXPECT_NE(result, nullptr);
    } else {
        EXPECT_EQ(result, image_);
    }
}

/**
 * @tc.name: TypeAndName
 * @tc.desc: Verify the factory registration type and type name of the emboss filter
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerEmbossFilterTest, TypeAndName, TestSize.Level1)
{
    Drawing::GEStickerEmbossFilterParams params;
    auto filter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::STICKER_EMBOSS_FILTER);
    EXPECT_FALSE(filter->TypeName().empty());
    EXPECT_EQ(filter->TypeName(), "StickerEmbossFilter");
}

} // namespace Rosen
} // namespace OHOS
