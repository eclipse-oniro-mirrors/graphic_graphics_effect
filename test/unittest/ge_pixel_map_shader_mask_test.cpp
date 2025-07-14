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
#include "ge_pixel_map_shader_mask.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEPixelMapShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEPixelMapShaderMaskTest::SetUpTestCase(void) {}

void GEPixelMapShaderMaskTest::TearDownTestCase(void) {}

void GEPixelMapShaderMaskTest::SetUp() {}

void GEPixelMapShaderMaskTest::TearDown() {}

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
HWTEST_F(GEPixelMapShaderMaskTest, Constructor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_001 start";

    GEPixelMapMaskParams param{
        .image=MakeImage(),
        .src=RectF(-0.5, -1.0, -0.1, -0.2),
        .dst=RectF(1.3, 1.2, 1.6, 2.0),
        .fillColor=Vector4f(0.2, 0.8, 0.1, 0.9),
    };
    auto gePixelMapShaderMask = std::make_shared<GEPixelMapShaderMask>(param);
    EXPECT_EQ(gePixelMapShaderMask->param_.image, param.image);
    EXPECT_EQ(gePixelMapShaderMask->param_.src, param.src);
    EXPECT_EQ(gePixelMapShaderMask->param_.dst, param.dst);
    EXPECT_EQ(gePixelMapShaderMask->param_.fillColor, param.fillColor);
    GTEST_LOG_(INFO) << "GEVisualEffectTest Constructor_001 end";
}

/**
 * @tc.name: GenerateBuilder_001
 * @tc.desc: Verify the Get Builder function
 * @tc.type: FUNC
 */
HWTEST_F(GEPixelMapShaderMaskTest, GenerateBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateBuilder_001 start";

    GEPixelMapMaskParams param;
    auto gePixelMapShaderMask = std::make_shared<GEPixelMapShaderMask>(param);
    auto builder = gePixelMapShaderMask->GetPixelMapShaderMaskBuilder();
    EXPECT_NE(builder, nullptr);
    builder = gePixelMapShaderMask->GetPixelMapShaderNormalMaskBuilder();
    EXPECT_NE(builder, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateBuilder_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEPixelMapShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 start";

    GEPixelMapMaskParams param;
    auto gePixelMapShaderMask = std::make_shared<GEPixelMapShaderMask>(param);
    auto shaderEffect = gePixelMapShaderMask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
    shaderEffect = gePixelMapShaderMask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEPixelMapShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 start";

    GEPixelMapMaskParams param {
        .image=MakeImage(),
        .src=RectF(0.1, 0.2, 0.8, 0.7),
        .dst=RectF(0.3, 0.1, 0.6, 0.5),
        .fillColor=Vector4f(0.2, 0.8, 0.1, 0.9),
    };
    ASSERT_NE(param.image, nullptr);
    auto gePixelMapShaderMask = std::make_shared<GEPixelMapShaderMask>(param);
    auto shaderEffect = gePixelMapShaderMask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
    shaderEffect = gePixelMapShaderMask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_002 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
