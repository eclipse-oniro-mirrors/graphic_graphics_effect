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
#include "ge_sticker_laser_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEStickerLaserShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };

    static std::shared_ptr<Drawing::ShaderEffect> MakeProbeShader();

    // SDF generation needs a GPU context; a raster canvas keeps the cache empty
    bool HasGpuContext() const
    {
        return canvas_.GetGPUContext() != nullptr;
    }
};

void GEStickerLaserShaderTest::SetUpTestCase(void) {}
void GEStickerLaserShaderTest::TearDownTestCase(void) {}

void GEStickerLaserShaderTest::SetUp()
{
    canvas_.Restore();

    constexpr int32_t bitmapSize = 50;
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(bitmapSize, bitmapSize, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEStickerLaserShaderTest::TearDown() {}

std::shared_ptr<Drawing::ShaderEffect> GEStickerLaserShaderTest::MakeProbeShader()
{
    constexpr char probeShader[] = "half4 main(float2 xy) { return half4(1.0); }";
    auto effect = GECreateRuntimeEffectForShader(probeShader);
    if (effect == nullptr) {
        return nullptr;
    }
    Drawing::RuntimeShaderBuilder builder(effect);
    return builder.MakeShader(nullptr, false);
}

/**
 * @tc.name: PreprocessNullSticker
 * @tc.desc: Verify Preprocess leaves the cache empty for a null sticker
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, PreprocessNullSticker, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->Preprocess(canvas_, src_);
    EXPECT_EQ(shader->GetCache(), nullptr);
}

/**
 * @tc.name: PreprocessBuildsCache
 * @tc.desc: Verify Preprocess builds the SDF cache and the second call hits it
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, PreprocessBuildsCache, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->Preprocess(canvas_, src_);
    auto cacheAfterFirst = shader->GetCache();
    shader->Preprocess(canvas_, src_);
    if (HasGpuContext()) {
        ASSERT_NE(cacheAfterFirst, nullptr);
        // Same input: the hit path keeps the packed entry untouched
        EXPECT_EQ(shader->GetCache(), cacheAfterFirst);
    } else {
        // The raster build fails and clears the cache
        EXPECT_EQ(cacheAfterFirst, nullptr);
        EXPECT_EQ(shader->GetCache(), nullptr);
    }
}

/**
 * @tc.name: MakeDrawingShaderRectOverload
 * @tc.desc: Verify the rect-only MakeDrawingShader overload is a no-op for this shader
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderRectOverload, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->MakeDrawingShader(src_, 1.0f);
    EXPECT_EQ(shader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderNullSticker
 * @tc.desc: Verify MakeDrawingShader generates nothing for a null sticker
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderNullSticker, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->MakeDrawingShader(canvas_, src_, 1.0f);
    EXPECT_EQ(shader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithoutCache
 * @tc.desc: Verify MakeDrawingShader generates nothing when the SDF cache is empty
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderWithoutCache, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    // A sticker is set but Preprocess was never called
    shader->MakeDrawingShader(canvas_, src_, 1.0f);
    EXPECT_EQ(shader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithCache
 * @tc.desc: Verify the composed laser shader is generated from a prepared cache
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderWithCache, TestSize.Level1)
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
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->OnDrawShader(canvas_, src_);
    if (HasGpuContext()) {
        EXPECT_NE(shader->GetDrawingShader(), nullptr);
    } else {
        // The raster canvas stops at the decoded-SDF guard
        EXPECT_EQ(shader->GetDrawingShader(), nullptr);
    }
}

/**
 * @tc.name: DrawShaderWithoutShader
 * @tc.desc: Verify DrawShader is a safe no-op when no shader was generated
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, DrawShaderWithoutShader, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->DrawShader(canvas_, src_);
    EXPECT_EQ(shader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderDegenerateDirection
 * @tc.desc: Verify zero direction vectors still reach the compose path without crash
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderDegenerateDirection, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    params.direction = Vector2f(0.0f, 0.0f); // shader falls back to vec2(1.0, 0.0)
    params.lightDir = Vector2f(0.0f, 0.0f);
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->OnDrawShader(canvas_, src_);
    if (HasGpuContext()) {
        EXPECT_NE(shader->GetDrawingShader(), nullptr);
    } else {
        EXPECT_EQ(shader->GetDrawingShader(), nullptr);
    }
}

/**
 * @tc.name: TypeAndName
 * @tc.desc: Verify the factory registration type and type name of the laser shader
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, TypeAndName, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::STICKER_LASER);
    EXPECT_FALSE(shader->TypeName().empty());
    EXPECT_EQ(shader->TypeName(), "StickerLaser");
}

/**
 * @tc.name: ConstructorStoresParams
 * @tc.desc: Verify the constructor copies every laser parameter into params_
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, ConstructorStoresParams, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 3.0f;
    params.frequency = 0.01f;
    params.progress = 0.4f;
    params.elevationShadowRadius = 8.0f;
    params.shadowDistance = 12.0f;
    params.direction = Vector2f(1.0f, 0.5f);
    params.lightDir = Vector2f(-0.5f, 0.5f);
    params.elevationShadowColor = Vector4f(0.1f, 0.2f, 0.3f, 0.4f);

    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    EXPECT_EQ(shader->params_.sticker, image_);
    EXPECT_FLOAT_EQ(shader->params_.outlineWidth, 3.0f);
    EXPECT_FLOAT_EQ(shader->params_.frequency, 0.01f);
    EXPECT_FLOAT_EQ(shader->params_.progress, 0.4f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowRadius, 8.0f);
    EXPECT_FLOAT_EQ(shader->params_.shadowDistance, 12.0f);
    EXPECT_FLOAT_EQ(shader->params_.direction.x_, 1.0f);
    EXPECT_FLOAT_EQ(shader->params_.direction.y_, 0.5f);
    EXPECT_FLOAT_EQ(shader->params_.lightDir.x_, -0.5f);
    EXPECT_FLOAT_EQ(shader->params_.lightDir.y_, 0.5f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.x_, 0.1f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.y_, 0.2f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.z_, 0.3f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.w_, 0.4f);
}

/**
 * @tc.name: ConstructorDefaultParams
 * @tc.desc: Verify the constructor keeps the declared laser parameter defaults
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, ConstructorDefaultParams, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    EXPECT_EQ(shader->params_.sticker, nullptr);
    EXPECT_FLOAT_EQ(shader->params_.outlineWidth, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.frequency, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.progress, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowRadius, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.shadowDistance, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.direction.x_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.direction.y_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.lightDir.x_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.lightDir.y_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.x_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.y_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.z_, 0.0f);
    EXPECT_FLOAT_EQ(shader->params_.elevationShadowColor.w_, 26.0f / 255.0f);
}

/**
 * @tc.name: PreprocessRebuildsAfterCacheCleared
 * @tc.desc: Verify Preprocess rebuilds the decoded SDF after the cache is cleared externally
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, PreprocessRebuildsAfterCacheCleared, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    params.outlineWidth = 2.0f;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    shader->Preprocess(canvas_, src_);
    auto firstCache = shader->GetCache();
    shader->cacheAnyPtr_ = nullptr;
    shader->Preprocess(canvas_, src_);
    if (HasGpuContext()) {
        ASSERT_NE(firstCache, nullptr);
        ASSERT_NE(shader->GetCache(), nullptr);
        EXPECT_NE(shader->GetCache(), firstCache);
    } else {
        EXPECT_EQ(firstCache, nullptr);
        EXPECT_EQ(shader->GetCache(), nullptr);
    }
}

/**
 * @tc.name: MakeDrawingShaderClearsPreviousShader
 * @tc.desc: Verify the canvas overload clears drShader_ before its null-sticker guard
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderClearsPreviousShader, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    auto probe = MakeProbeShader();
    ASSERT_NE(probe, nullptr);
    shader->drShader_ = probe;
    ASSERT_EQ(shader->GetDrawingShader(), probe);

    shader->MakeDrawingShader(canvas_, src_, 1.0f);
    EXPECT_EQ(shader->drShader_, nullptr);
    EXPECT_EQ(shader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderRectOverloadKeepsState
 * @tc.desc: Verify the rect-only overload leaves the existing shader and cache untouched
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerLaserShaderTest, MakeDrawingShaderRectOverloadKeepsState, TestSize.Level1)
{
    Drawing::GEStickerLaserShaderParams params;
    params.sticker = image_;
    auto shader = std::make_shared<GEStickerLaserShader>(params);
    ASSERT_TRUE(shader != nullptr);

    auto probe = MakeProbeShader();
    ASSERT_NE(probe, nullptr);
    shader->drShader_ = probe;
    auto cacheBefore = shader->GetCache();

    shader->MakeDrawingShader(src_, 1.0f);
    EXPECT_EQ(shader->drShader_, probe);
    EXPECT_EQ(shader->GetCache(), cacheBefore);
}

} // namespace Rosen
} // namespace OHOS
