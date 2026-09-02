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

#include "ge_fractal_glass_shader_mask.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEFractalGlassShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEFractalGlassShaderMaskTest::SetUpTestCase(void) {}
void GEFractalGlassShaderMaskTest::TearDownTestCase(void) {}

void GEFractalGlassShaderMaskTest::SetUp() {}
void GEFractalGlassShaderMaskTest::TearDown() {}

static std::shared_ptr<Drawing::Image> MakeImage()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader returns valid shader with no image (NoTexture path)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader returns valid shader with image
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .num_ = 25.0f,
        .strength_ = 1.0f,
        .softness_ = 0.001f,
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with isSymmetric = true and image
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .num_ = 25.0f,
        .strength_ = 1.0f,
        .softness_ = 0.001f,
        .symmetric_ = true,
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_004
 * @tc.desc: Verify GenerateDrawingShader with isSymmetric = false and image
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_004, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .num_ = 25.0f,
        .strength_ = 1.0f,
        .softness_ = 0.001f,
        .symmetric_ = false,
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_005
 * @tc.desc: Verify GenerateDrawingShader with isSymmetric = false and no image (NoTexture path)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_005, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    param.symmetric_ = false;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns valid shader with no image (NoTexture path)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns valid shader with image
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .num_ = 25.0f,
        .strength_ = 1.0f,
        .softness_ = 0.001f,
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_003
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with isSymmetric = false (NoTexture path)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShaderHasNormal_003, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    param.symmetric_ = false;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify Type and TypeName
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, Type_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param {};
    auto filter = std::make_unique<GEFractalGlassShaderMask>(param);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::FRACTAL_GLASS_MASK);
    EXPECT_EQ(filter->TypeName(), "FractalGlassMask");
}

/**
 * @tc.name: GlassNumVariations_001
 * @tc.desc: Verify GenerateDrawingShader with different glassNum values
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GlassNumVariations_001, TestSize.Level1)
{
    constexpr std::array<float, 4> glassNums = { 5.0f, 10.0f, 25.0f, 50.0f };
    for (const auto& num : glassNums) {
        GEFractalGlassMaskParams param {
            .image = MakeImage(),
            .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .num_ = num,
        };
        auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
        auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
        EXPECT_NE(shaderEffect, nullptr);
    }
}

/**
 * @tc.name: GlassStrengthVariations_001
 * @tc.desc: Verify GenerateDrawingShader with different glassStrength values
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GlassStrengthVariations_001, TestSize.Level1)
{
    constexpr std::array<float, 4> strengths = { 0.5f, 1.0f, 1.5f, 2.0f };
    for (const auto& strength : strengths) {
        GEFractalGlassMaskParams param {
            .image = MakeImage(),
            .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .strength_ = strength,
        };
        auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
        auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
        EXPECT_NE(shaderEffect, nullptr);
    }
}

/**
 * @tc.name: GlassSoftnessVariations_001
 * @tc.desc: Verify GenerateDrawingShader with different glassSoftness values
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GlassSoftnessVariations_001, TestSize.Level1)
{
    constexpr std::array<float, 4> softnesses = { 0.001f, 0.003f, 0.005f, 0.01f };
    for (const auto& softness : softnesses) {
        GEFractalGlassMaskParams param {
            .image = MakeImage(),
            .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
            .softness_ = softness,
        };
        auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
        auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
        EXPECT_NE(shaderEffect, nullptr);
    }
}

/**
 * @tc.name: GenerateDrawingShader_006
 * @tc.desc: Verify GenerateDrawingShader returns nullptr when src rect is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_006, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(50.0f, 50.0f, 10.0f, 10.0f), // invalid: left > right
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_007
 * @tc.desc: Verify GenerateDrawingShader returns nullptr when src width is zero
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_007, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 0.0f, 50.0f), // zero width: left == right
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_008
 * @tc.desc: Verify GenerateDrawingShader returns nullptr when src height is zero
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShader_008, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 0.0f), // zero height: top == bottom
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_004
 * @tc.desc: Verify GenerateDrawingShaderHasNormal with image and isSymmetric = false
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShaderHasNormal_004, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .num_ = 25.0f,
        .strength_ = 1.0f,
        .softness_ = 0.001f,
        .symmetric_ = false,
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_005
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns nullptr when src rect is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, GenerateDrawingShaderHasNormal_005, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(50.0f, 50.0f, 10.0f, 10.0f), // invalid: left > right
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: TextureBuilderCaching_001
 * @tc.desc: Verify texture builder is cached (GetFractalGlassShaderMaskBuilder)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, TextureBuilderCaching_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto builder1 = mask->GetFractalGlassShaderMaskBuilder();
    auto builder2 = mask->GetFractalGlassShaderMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_EQ(builder1, builder2);
}

/**
 * @tc.name: NormalMaskBuilderCaching_001
 * @tc.desc: Verify normal mask builder is cached (GetFractalGlassShaderNormalMaskBuilder)
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, NormalMaskBuilderCaching_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param {
        .image = MakeImage(),
        .src = RectF(0.0f, 0.0f, 50.0f, 50.0f),
        .dst = RectF(0.0f, 0.0f, 50.0f, 50.0f),
    };
    ASSERT_NE(param.image, nullptr);
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto builder1 = mask->GetFractalGlassShaderNormalMaskBuilder();
    auto builder2 = mask->GetFractalGlassShaderNormalMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_EQ(builder1, builder2);
}

/**
 * @tc.name: NoTextureBuilderCaching_001
 * @tc.desc: Verify NoTexture builder is cached
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, NoTextureBuilderCaching_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto builder1 = mask->GetFractalGlassShaderMaskBuilderNoTexture();
    auto builder2 = mask->GetFractalGlassShaderMaskBuilderNoTexture();
    EXPECT_NE(builder1, nullptr);
    EXPECT_EQ(builder1, builder2);
}

/**
 * @tc.name: NoTextureNormalBuilderCaching_001
 * @tc.desc: Verify NoTexture normal mask builder is cached
 * @tc.type: FUNC
 */
HWTEST_F(GEFractalGlassShaderMaskTest, NoTextureNormalBuilderCaching_001, TestSize.Level1)
{
    GEFractalGlassMaskParams param;
    auto mask = std::make_shared<GEFractalGlassShaderMask>(param);
    auto builder1 = mask->GetFractalGlassShaderNormalMaskBuilderNoTexture();
    auto builder2 = mask->GetFractalGlassShaderNormalMaskBuilderNoTexture();
    EXPECT_NE(builder1, nullptr);
    EXPECT_EQ(builder1, builder2);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS