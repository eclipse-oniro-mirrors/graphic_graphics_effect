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

#include <cmath>
#include <gtest/gtest.h>

#include "image/bitmap.h"
#include "ge_sticker_outline_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEStickerOutlineShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override {}

    std::shared_ptr<Drawing::Image> CreateImage(int width, int height);
    std::shared_ptr<Drawing::Image> image50x50_ { nullptr };
    std::shared_ptr<Drawing::Image> image100x100_ { nullptr };
    static constexpr int32_t smallImageSize = 50;
    static constexpr int32_t largeImageSize = 100;
};

void GEStickerOutlineShaderTest::SetUp()
{
    image50x50_ = CreateImage(smallImageSize, smallImageSize);
    image100x100_ = CreateImage(largeImageSize, largeImageSize);
    EXPECT_NE(image50x50_, nullptr);
    EXPECT_NE(image100x100_, nullptr);
}

std::shared_ptr<Drawing::Image> GEStickerOutlineShaderTest::CreateImage(int width, int height)
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

/**
 * @tc.name: CalStickerHash_001
 * @tc.desc: Verify CalStickerHash for null, consistency, different spreadFactor and different images
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalStickerHash_001, TestSize.Level1)
{
    // null returns 0
    EXPECT_EQ(GEStickerOutlineShader::CalStickerHash(nullptr, 64), 0u);

    // same input produces same non-zero hash
    auto hash1 = GEStickerOutlineShader::CalStickerHash(image50x50_, 64);
    auto hash2 = GEStickerOutlineShader::CalStickerHash(image50x50_, 64);
    EXPECT_EQ(hash1, hash2);
    EXPECT_NE(hash1, 0u);

    // different spreadFactor produces different hash
    auto hash64 = GEStickerOutlineShader::CalStickerHash(image50x50_, 64);
    auto hash128 = GEStickerOutlineShader::CalStickerHash(image50x50_, 128);
    EXPECT_NE(hash64, hash128);

    // different images produce different hash
    auto hash50 = GEStickerOutlineShader::CalStickerHash(image50x50_, 64);
    auto hash100 = GEStickerOutlineShader::CalStickerHash(image100x100_, 64);
    EXPECT_NE(hash50, hash100);
}

/**
 * @tc.name: CalSdfSpreadFactor_001
 * @tc.desc: Verify CalSdfSpreadFactor returns minimum 1 for zero input
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalSdfSpreadFactor_001, TestSize.Level1)
{
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(0.0f), 1);
}

/**
 * @tc.name: CalSdfSpreadFactor_002
 * @tc.desc: Verify CalSdfSpreadFactor returns 1 for negative input
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalSdfSpreadFactor_002, TestSize.Level1)
{
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(-10.0f), 1);
}

/**
 * @tc.name: CalSdfSpreadFactor_003
 * @tc.desc: Verify CalSdfSpreadFactor returns correct power of 2 for normal input
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalSdfSpreadFactor_003, TestSize.Level1)
{
    // requiredRangePx=40: quarterResRange=10, ceil(10)=10, spread: 1->2->4->8->16
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(40.0f), 16);
    // requiredRangePx=8: quarterResRange=2, ceil(2)=2, spread: 1->2
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(8.0f), 2);
    // requiredRangePx=9: quarterResRange=2.25, ceil(2.25)=3, spread: 1->2->4
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(9.0f), 4);
}

/**
 * @tc.name: CalSdfSpreadFactor_004
 * @tc.desc: Verify CalSdfSpreadFactor clamps to MAX_SPREAD_FACTOR for large input
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalSdfSpreadFactor_004, TestSize.Level1)
{
    // 1e20 would overflow int without clamp; should return 1024
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(1e20f), 1024);
    // 4096: quarterResRange=1024, exactly at boundary
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(4096.0f), 1024);
    // 4097: quarterResRange=1024.25, clamped to 1024
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(4097.0f), 1024);
}

/**
 * @tc.name: CalSdfSpreadFactor_005
 * @tc.desc: Verify CalSdfSpreadFactor handles infinity and NaN safely
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerOutlineShaderTest, CalSdfSpreadFactor_005, TestSize.Level1)
{
    // Infinity: max(inf, 0)=inf, inf/4=inf, min(inf, 1024)=1024
    EXPECT_EQ(GEStickerOutlineShader::CalSdfSpreadFactor(
        std::numeric_limits<float>::infinity()), 1024);
    // NaN: max(NaN, 0) is implementation-defined, should not crash
    auto result = GEStickerOutlineShader::CalSdfSpreadFactor(
        std::numeric_limits<float>::quiet_NaN());
    EXPECT_GE(result, 1);
    EXPECT_LE(result, 1024);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
