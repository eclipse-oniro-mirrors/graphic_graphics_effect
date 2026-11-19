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
#include "ge_sticker_emboss_filter.h"
#include "ge_sticker_laser_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEStickerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEStickerTest::SetUpTestCase(void) {}
void GEStickerTest::TearDownTestCase(void) {}

void GEStickerTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50 bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEStickerTest::TearDown(void) {}

/**
 * @tc.name: Emboss_OnProcessImage_NullImage_001
 * @tc.desc: Verify OnProcessImage returns nullptr for a null image
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Emboss_OnProcessImage_NullImage_001, TestSize.Level0)
{
    Drawing::GEStickerEmbossFilterParams params;
    auto stickerEmbossFilter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(stickerEmbossFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(stickerEmbossFilter->OnProcessImage(canvas_, image, src_, dst_), nullptr);
}

/**
 * @tc.name: Emboss_OnProcessImage_NullSticker_001
 * @tc.desc: Verify OnProcessImage returns the input image when the sticker is missing
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Emboss_OnProcessImage_NullSticker_001, TestSize.Level0)
{
    Drawing::GEStickerEmbossFilterParams params;
    auto stickerEmbossFilter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(stickerEmbossFilter != nullptr);

    EXPECT_EQ(stickerEmbossFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: Emboss_OnProcessImage_WithSticker_001
 * @tc.desc: Verify OnProcessImage falls back to the input image on a raster canvas
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Emboss_OnProcessImage_WithSticker_001, TestSize.Level0)
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
    auto stickerEmbossFilter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(stickerEmbossFilter != nullptr);

    // The SDF/composite stages need a GPU context, so a raster canvas takes the
    // internal-failure branches and hands the input image back unchanged.
    EXPECT_EQ(stickerEmbossFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: Emboss_OnProcessImage_Repeat_001
 * @tc.desc: Verify repeated OnProcessImage calls stay stable through the SDF cache path
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Emboss_OnProcessImage_Repeat_001, TestSize.Level0)
{
    Drawing::GEStickerEmbossFilterParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto stickerEmbossFilter = std::make_shared<GEStickerEmbossFilter>(params);
    ASSERT_TRUE(stickerEmbossFilter != nullptr);

    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(stickerEmbossFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: Laser_OnDrawShader_NullSticker_001
 * @tc.desc: Verify OnDrawShader generates no shader when the sticker is missing
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Laser_OnDrawShader_NullSticker_001, TestSize.Level0)
{
    Drawing::GEStickerLaserShaderParams params;
    auto stickerLaserShader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(stickerLaserShader != nullptr);

    stickerLaserShader->OnDrawShader(canvas_, src_);
    EXPECT_EQ(stickerLaserShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: Laser_OnDrawShader_RasterCanvas_001
 * @tc.desc: Verify OnDrawShader generates no shader on a raster canvas
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Laser_OnDrawShader_RasterCanvas_001, TestSize.Level0)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    params.frequency = 0.005f;
    params.progress = 1.0f;
    params.direction = Vector2f(1.0f, 0.0f);
    params.lightDir = Vector2f(0.5f, 0.5f);
    params.elevationShadowColor = Vector4f(0.0f, 0.0f, 0.0f, 0.2f);
    params.elevationShadowRadius = 10.0f;
    params.shadowDistance = 10.0f;
    auto stickerLaserShader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(stickerLaserShader != nullptr);

    // SDF generation needs a GPU context, so a raster canvas leaves the cache empty
    // and MakeDrawingShader stops at the decoded-SDF guard.
    stickerLaserShader->OnDrawShader(canvas_, src_);
    EXPECT_EQ(stickerLaserShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: Laser_OnDrawShader_Repeat_001
 * @tc.desc: Verify repeated OnDrawShader calls stay stable through the cache path
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerTest, Laser_OnDrawShader_Repeat_001, TestSize.Level0)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto stickerLaserShader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(stickerLaserShader != nullptr);

    for (int i = 0; i < 2; i++) {
        stickerLaserShader->OnDrawShader(canvas_, src_);
        EXPECT_EQ(stickerLaserShader->GetDrawingShader(), nullptr);
    }
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
