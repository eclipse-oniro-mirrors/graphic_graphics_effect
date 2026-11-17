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

#include "ge_sticker_halftone_shader.h"
#include "ge_sticker_outline_shader.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEStickerHalftoneShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override {}

    std::shared_ptr<Drawing::Image> CreateImage(int width, int height);
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    static constexpr int32_t testImageWidth = 50;
    static constexpr int32_t testImageHeight = 50;
};

void GEStickerHalftoneShaderTest::SetUp()
{
    image_ = CreateImage(testImageWidth, testImageHeight);
    EXPECT_NE(image_, nullptr);
}

std::shared_ptr<Drawing::Image> GEStickerHalftoneShaderTest::CreateImage(int width, int height)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor stores params correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerHalftoneShaderTest, Constructor_001, TestSize.Level1)
{
    Drawing::GEStickerHalftoneShaderParams params;
    params.gridSize_ = 10.0f;
    params.maxRadiusRatio_ = 0.8f;
    params.lumCoeff_ = 0.5f;
    params.colorWidthRatio_ = 0.3f;
    params.saturation_ = 0.6f;
    params.smoothness_ = 0.2f;
    params.angle_ = 1.5f;
    params.outlinePx_ = 5.0f;
    params.shadowDistance_ = 8.0f;
    params.sticker_ = image_;

    GEStickerHalftoneShader shader(params);
    EXPECT_FLOAT_EQ(shader.params_.gridSize_, 10.0f);
    EXPECT_FLOAT_EQ(shader.params_.maxRadiusRatio_, 0.8f);
    EXPECT_FLOAT_EQ(shader.params_.lumCoeff_, 0.5f);
    EXPECT_FLOAT_EQ(shader.params_.colorWidthRatio_, 0.3f);
    EXPECT_FLOAT_EQ(shader.params_.saturation_, 0.6f);
    EXPECT_FLOAT_EQ(shader.params_.smoothness_, 0.2f);
    EXPECT_FLOAT_EQ(shader.params_.angle_, 1.5f);
    EXPECT_FLOAT_EQ(shader.params_.outlinePx_, 5.0f);
    EXPECT_FLOAT_EQ(shader.params_.shadowDistance_, 8.0f);
    EXPECT_EQ(shader.params_.sticker_, image_);
}

/**
 * @tc.name: Constructor_002
 * @tc.desc: Verify constructor uses default params when not set
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerHalftoneShaderTest, Constructor_002, TestSize.Level1)
{
    Drawing::GEStickerHalftoneShaderParams params;
    GEStickerHalftoneShader shader(params);
    EXPECT_FLOAT_EQ(shader.params_.gridSize_, 0.0f);
    EXPECT_FLOAT_EQ(shader.params_.maxRadiusRatio_, 1.0f);
    EXPECT_FLOAT_EQ(shader.params_.lumCoeff_, 0.0f);
    EXPECT_FLOAT_EQ(shader.params_.colorWidthRatio_, 0.5f);
    EXPECT_FLOAT_EQ(shader.params_.saturation_, 1.0f);
    EXPECT_FLOAT_EQ(shader.params_.smoothness_, 0.0f);
    EXPECT_FLOAT_EQ(shader.params_.angle_, 0.0f);
    EXPECT_FLOAT_EQ(shader.params_.outlinePx_, 0.0f);
    EXPECT_FLOAT_EQ(shader.params_.shadowDistance_, 0.0f);
    EXPECT_EQ(shader.params_.sticker_, nullptr);
}

/**
 * @tc.name: Preprocess_001
 * @tc.desc: Verify Preprocess returns early when sticker is null
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerHalftoneShaderTest, Preprocess_001, TestSize.Level1)
{
    Drawing::GEStickerHalftoneShaderParams params;
    params.sticker_ = nullptr;
    GEStickerHalftoneShader shader(params);

    Drawing::Canvas canvas;
    Drawing::Rect rect(0, 0, 100, 100);
    // Should not crash when sticker is null
    shader.Preprocess(canvas, rect);
    EXPECT_EQ(shader.cacheAnyPtr_, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: Verify MakeDrawingShader returns early when sticker is null
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerHalftoneShaderTest, MakeDrawingShader_001, TestSize.Level1)
{
    Drawing::GEStickerHalftoneShaderParams params;
    params.sticker_ = nullptr;
    GEStickerHalftoneShader shader(params);

    Drawing::Canvas canvas;
    Drawing::Rect rect(0, 0, 100, 100);
    // Should not crash when sticker is null
    shader.MakeDrawingShader(canvas, rect, -1.0f);
    EXPECT_EQ(shader.drShader_, nullptr);
}

/**
 * @tc.name: CalStickerHash_001
 * @tc.desc: Verify shared CalStickerHash works via GEStickerOutlineShader
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerHalftoneShaderTest, CalStickerHash_001, TestSize.Level1)
{
    // Halftone uses GEStickerOutlineShader::CalStickerHash (shared static method)
    auto hash = GEStickerOutlineShader::CalStickerHash(image_, 64);
    EXPECT_NE(hash, 0u);

    auto hashNull = GEStickerOutlineShader::CalStickerHash(nullptr, 64);
    EXPECT_EQ(hashNull, 0u);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
