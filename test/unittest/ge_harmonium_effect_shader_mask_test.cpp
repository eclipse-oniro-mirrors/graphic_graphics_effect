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
#include "ge_harmonium_effect_shader_mask.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEHarmoniumEffectShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEHarmoniumEffectShaderMaskTest::SetUpTestCase(void) {}

void GEHarmoniumEffectShaderMaskTest::TearDownTestCase(void) {}

void GEHarmoniumEffectShaderMaskTest::SetUp() {}

void GEHarmoniumEffectShaderMaskTest::TearDown() {}

static std::shared_ptr<Drawing::Image> MakeImage()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify the constructor function
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, Constructor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_001 start";

    GEHarmoniumEffectMaskParams param{
        .image=MakeImage(),
    };
    auto geHarmoniumEffectShaderMask = std::make_shared<GEHarmoniumEffectShaderMask>(param);
    EXPECT_EQ(geHarmoniumEffectShaderMask->param_.image, param.image);
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_001 end";
}

/**
 * @tc.name: GenerateBuilder_001
 * @tc.desc: Verify the Get Builder function
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, GenerateBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateBuilder_001 start";

    GEHarmoniumEffectMaskParams param;
    auto geHarmoniumEffectShaderMask = std::make_shared<GEHarmoniumEffectShaderMask>(param);
    auto builder = geHarmoniumEffectShaderMask->GetHarmoniumEffectShaderMaskBuilder();
    EXPECT_NE(builder, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateBuilder_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 start";

    GEHarmoniumEffectMaskParams param;
    auto geHarmoniumEffectShaderMask = std::make_shared<GEHarmoniumEffectShaderMask>(param);
    auto shaderEffect = geHarmoniumEffectShaderMask->GenerateDrawingShader(0.f, 0.f);
    EXPECT_EQ(shaderEffect, nullptr);
    shaderEffect = geHarmoniumEffectShaderMask->GenerateDrawingShaderHasNormal(0.f, 0.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 start";

    GEHarmoniumEffectMaskParams param {
        .image=MakeImage(),
    };
    ASSERT_NE(param.image, nullptr);
    auto geHarmoniumEffectShaderMask = std::make_shared<GEHarmoniumEffectShaderMask>(param);
    auto shaderEffect = geHarmoniumEffectShaderMask->GenerateDrawingShader(0.f, 0.f);
    EXPECT_NE(shaderEffect, nullptr);
    shaderEffect = geHarmoniumEffectShaderMask->GenerateDrawingShaderHasNormal(0.f, 0.f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 end";
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEHarmoniumEffectShaderMaskTest Type_001 start";
    GEHarmoniumEffectMaskParams param {};
    auto filter = std::make_unique<GEHarmoniumEffectShaderMask>(param);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::HARMONIUM_EFFECT_MASK);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_MASK_HARMONIUM_EFFECT);

    GTEST_LOG_(INFO) << "GEHarmoniumEffectShaderMaskTest Type_001 end";
}

/**
 * @tc.name: GetImage_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEHarmoniumEffectShaderMaskTest, GetImage_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEHarmoniumEffectShaderMaskTest GetImage_001 start";
    GEHarmoniumEffectMaskParams param {
        .image=MakeImage(),
    };
    ASSERT_NE(param.image, nullptr);
    auto geHarmoniumEffectShaderMask = std::make_shared<GEHarmoniumEffectShaderMask>(param);
    EXPECT_NE(geHarmoniumEffectShaderMask->GetImage(), nullptr);

    GTEST_LOG_(INFO) << "GEHarmoniumEffectShaderMaskTest GetImage_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
