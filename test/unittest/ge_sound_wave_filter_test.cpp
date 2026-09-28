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

#include "draw/color.h"
#include "ge_sound_wave_filter.h"
#include "image/bitmap.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESoundWaveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 51.0f, 51.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 51.0f, 51.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 51.0f, 51.0f };
};

void GESoundWaveFilterTest::SetUpTestCase(void) {}
void GESoundWaveFilterTest::TearDownTestCase(void) {}

void GESoundWaveFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GESoundWaveFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImage001
 * @tc.desc: Verify the OnProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, OnProcessImage001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage001 start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage001 end";
}

/**
 * @tc.name: OnProcessImage002
 * @tc.desc: Verify the OnProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, OnProcessImage002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage002 start";

    // init data
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1, 1, format); // 1, 1  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->OnProcessImage(canvas_, image, src_, dst_), image);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage002 end";
}

/**
 * @tc.name: OnProcessImage003
 * @tc.desc: Verify the OnProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, OnProcessImage003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage003 start";

    // init data
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(10, 10, format); // 10, 10  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->OnProcessImage(canvas_, image, src_, dst_), image);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage003 end";
}

/**
 * @tc.name: OnProcessImage004
 * @tc.desc: Verify the OnProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, OnProcessImage004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage004 start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImage004 end";
}

/**
 * @tc.name: CheckSoundWaveParams
 * @tc.desc: Verify the CheckSoundWaveParams
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, CheckSoundWaveParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveParams start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    geSoundWaveFilterParams.colorA = {20.0f, -10.0f, 20.0f, -10.0f};
    geSoundWaveFilterParams.soundIntensity = 10.0f;
    geSoundWaveFilterParams.shockWaveAlphaA = 10.0f;
    geSoundWaveFilterParams.shockWaveAlphaB = 10.0f;
    geSoundWaveFilterParams.shockWaveTotalAlpha = 20.0f;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    geSoundWaveFilter->CheckSoundWaveParams();
    EXPECT_NE(geSoundWaveFilter->colorA_.greenF_, -10.0f);
    EXPECT_NE(geSoundWaveFilter->soundIntensity_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveProgressA_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveProgressB_, 10.0f);
    EXPECT_NE(geSoundWaveFilter->shockWaveTotalAlpha_, 20.0f);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveParams end";
}

/**
 * @tc.name: CheckSoundWaveColor4f
 * @tc.desc: Verify the CheckSoundWaveColor4f
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, CheckSoundWaveColor4f, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveColor4f start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    Drawing::Color4f colorA = { 20.0f, 20.0f, 20.0f, 20.0f };
    geSoundWaveFilter->CheckSoundWaveColor4f(colorA);
    EXPECT_EQ(colorA.redF_, 10.0f);
    EXPECT_EQ(colorA.greenF_, 10.0f);
    EXPECT_EQ(colorA.blueF_, 10.0f);
    EXPECT_EQ(colorA.alphaF_, 1.0f);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest CheckSoundWaveColor4f end";
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type function
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, Type_001, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest Type_001 start";

    Drawing::GESoundWaveFilterParams geSoundWaveFilterParams;
    std::unique_ptr<GESoundWaveFilter> geSoundWaveFilter =
        std::make_unique<GESoundWaveFilter>(geSoundWaveFilterParams);
    EXPECT_EQ(geSoundWaveFilter->Type(), Drawing::GEFilterType::SOUND_WAVE);
    EXPECT_EQ(geSoundWaveFilter->TypeName(), Drawing::GE_FILTER_SOUND_WAVE);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest Type_001 end";
}

/**
 * @tc.name: SentinelPath
 * @tc.desc: Verify sentinel path: circleRadius == 0 keeps members intact across repeated uniform binding
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, SentinelPath, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest SentinelPath start";

    Drawing::GESoundWaveFilterParams params;
    params.soundIntensity = 0.5f;
    params.circleRadius = 0.0f; // sentinel: fall back to the soundIntensity-driven layout
    auto filter = std::make_unique<GESoundWaveFilter>(params);

    // The sentinel is resolved into local uniforms inside ComputeAndSetUniforms.
    auto effect = filter->GetSoundWaveEffect();
    ASSERT_NE(effect, nullptr);
    Drawing::RuntimeShaderBuilder builder(effect);
    filter->ComputeAndSetUniforms(builder);

    // Derived uniforms live in locals; members keep the constructor state.
    EXPECT_FLOAT_EQ(filter->circleRadius_, 0.0f);
    EXPECT_FLOAT_EQ(filter->circleCenter_.x_, 0.0f);
    EXPECT_FLOAT_EQ(filter->circleCenter_.y_, 0.0f);
    EXPECT_FLOAT_EQ(filter->lightIntensity_, 1.0f);

    // Binding is idempotent: repeated calls leave the sentinel member intact.
    filter->ComputeAndSetUniforms(builder);
    EXPECT_FLOAT_EQ(filter->circleRadius_, 0.0f);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest SentinelPath end";
}

/**
 * @tc.name: NonSentinelPath
 * @tc.desc: Verify non-sentinel path: uniform binding leaves circle members read-only
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, NonSentinelPath, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest NonSentinelPath start";

    Drawing::GESoundWaveFilterParams params;
    params.soundIntensity = 0.4f;
    params.circleRadius = 0.3f;
    params.circleCenter = Vector2f(0.2f, -0.1f);
    params.lightIntensity = 3.0f;
    auto filter = std::make_unique<GESoundWaveFilter>(params);

    auto effect = filter->GetSoundWaveEffect();
    ASSERT_NE(effect, nullptr);
    Drawing::RuntimeShaderBuilder builder(effect);
    filter->ComputeAndSetUniforms(builder);

    EXPECT_FLOAT_EQ(filter->circleRadius_, 0.3f);
    EXPECT_FLOAT_EQ(filter->circleCenter_.x_, 0.2f);
    EXPECT_FLOAT_EQ(filter->circleCenter_.y_, -0.1f);
    EXPECT_FLOAT_EQ(filter->lightIntensity_, 3.0f);

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest NonSentinelPath end";
}

/**
 * @tc.name: OnProcessImageSuccess
 * @tc.desc: Verify OnProcessImage success path on a GPU canvas (MakeImage returns a new image)
 * @tc.type: FUNC
 */
HWTEST_F(GESoundWaveFilterTest, OnProcessImageSuccess, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImageSuccess start";

    // GPU-backed canvas so builder.MakeImage succeeds, covering the success return path.
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        return; // GPU context unavailable in this environment
    }
    Drawing::ImageInfo imageInfo { 50, 50, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    auto surface = Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo);
    if (surface == nullptr) {
        return; // GPU surface unavailable in this environment
    }
    auto gpuCanvas = surface->GetCanvas();
    ASSERT_NE(gpuCanvas, nullptr);

    Drawing::GESoundWaveFilterParams params;
    auto filter = std::make_unique<GESoundWaveFilter>(params);
    Drawing::CanvasInfo ci;
    ci.geoWidth = 50.0f;
    ci.geoHeight = 50.0f;
    filter->SetShaderFilterCanvasinfo(ci);

    auto result = filter->OnProcessImage(*gpuCanvas, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result.get(), image_.get());

    GTEST_LOG_(INFO) << "GESoundWaveFilterTest OnProcessImageSuccess end";
}

} // namespace Rosen
} // namespace OHOS
