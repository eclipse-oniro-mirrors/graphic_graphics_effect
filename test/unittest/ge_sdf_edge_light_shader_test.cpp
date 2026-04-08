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

#include "draw/canvas.h"
#include "draw/color.h"
#include "ge_double_ripple_shader_mask.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "ge_sdf_edge_light_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr size_t NUM_2 = 2;
constexpr size_t NUM_50 = 50;

class GESDFEdgeLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> sdfImage_ { nullptr };
    std::shared_ptr<Drawing::GEShaderMask> lightMask_ { nullptr };

    Drawing::Rect rect_ { 20.0f, 20.0f, 200.0f, 200.0f };

private:
    Drawing::GESDFEdgeLightEffectParams MakeParams();
};

void GESDFEdgeLightShaderTest::SetUpTestCase() {}

void GESDFEdgeLightShaderTest::TearDownTestCase() {}

void GESDFEdgeLightShaderTest::SetUp()
{
    // Build a 50x50 SDF image (use RED just as placeholder content)
    Drawing::Bitmap sdfBmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    sdfBmp.Build(NUM_50, NUM_50, fmt);
    sdfBmp.ClearWithColor(Drawing::Color::COLOR_RED);
    sdfImage_ = sdfBmp.MakeImage();

    // Create a light mask
    Drawing::GEDoubleRippleShaderMaskParams mp;
    lightMask_ = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
}

void GESDFEdgeLightShaderTest::TearDown()
{
    sdfImage_ = nullptr;
    lightMask_ = nullptr;
}

Drawing::GESDFEdgeLightEffectParams GESDFEdgeLightShaderTest::MakeParams()
{
    Drawing::GESDFEdgeLightEffectParams p;
    p.sdfSpreadFactor = 64.0f;
    p.bloomIntensityCutoff = 0.1f;
    p.maxIntensity = 1.0f;
    p.maxBloomIntensity = 1.0f;
    p.bloomFalloffPow = 2.0f;
    p.minBorderWidth = 2.0f;
    p.maxBorderWidth = 5.0f;
    p.innerBorderBloomWidth = 30.0f;
    p.outerBorderBloomWidth = 30.0f;
    // sdfImage and lightMask can be set in params before constructing shader
    return p;
}

/**
 * @tc.name: MakeDrawingShader_InvalidInputs
 * @tc.desc: Verify MakeDrawingShader handles invalid inputs gracefully (null sdfImage or lightMask).
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, MakeDrawingShader_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);

    // Case 1: sdfImage_ not set → MakeDrawingShader should complete but shader may be invalid
    shader->MakeDrawingShader(rect_, 0.0f);
    // On CPU canvas, shader generation may fail silently

    // Case 2: lightMask not set → MakeDrawingShader should complete but shader may be invalid
    auto params2 = MakeParams();
    params2.sdfImage = sdfImage_;
    // lightMask is not set (nullptr)
    auto shader2 = std::make_unique<GESDFEdgeLightShader>(params2);
    shader2->MakeDrawingShader(rect_, 0.0f);
}

/**
 * @tc.name: MakeDrawingShader_CPUFallback
 * @tc.desc: On CPU Canvas (no GPU context), MakeDrawingShader should complete gracefully.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, MakeDrawingShader_CPUFallback, TestSize.Level0)
{
    ASSERT_NE(sdfImage_, nullptr);
    ASSERT_NE(lightMask_, nullptr);

    // Provide required resources via constructor params
    auto params = MakeParams();
    params.sdfImage = sdfImage_;
    params.lightMask = lightMask_;
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);

    // On CPU canvas, MakeDrawingShader should complete (shader may be null due to no GPU context)
    shader->MakeDrawingShader(rect_, 0.0f);
}

/**
 * @tc.name: ParameterVariations_Smoke
 * @tc.desc: Verify different parameter combinations work correctly on CPU.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, ParameterVariations_Smoke, TestSize.Level0)
{
    ASSERT_NE(sdfImage_, nullptr);
    ASSERT_NE(lightMask_, nullptr);

    // Test with initial SDF image
    auto params1 = MakeParams();
    params1.sdfImage = sdfImage_;
    params1.lightMask = lightMask_;
    auto shader1 = std::make_unique<GESDFEdgeLightShader>(params1);
    shader1->MakeDrawingShader(rect_, 0.0f);

    // Test with different SDF image
    Drawing::Bitmap newSdfBmp;
    Drawing::BitmapFormat fmt { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    newSdfBmp.Build(NUM_2, NUM_2, fmt);
    newSdfBmp.ClearWithColor(Drawing::Color::COLOR_GREEN);
    auto newSdfImage = newSdfBmp.MakeImage();
    ASSERT_NE(newSdfImage, nullptr);

    auto params2 = MakeParams();
    params2.sdfImage = newSdfImage;
    params2.lightMask = lightMask_;
    auto shader2 = std::make_unique<GESDFEdgeLightShader>(params2);
    shader2->MakeDrawingShader(rect_, 0.0f);

    // Test with different parameters
    auto params3 = MakeParams();
    params3.sdfImage = sdfImage_;
    params3.lightMask = lightMask_;
    params3.bloomIntensityCutoff = 0.5f;
    params3.maxIntensity = 2.0f;
    params3.maxBloomIntensity = 1.5f;
    params3.bloomFalloffPow = 3.0f;
    params3.minBorderWidth = 1.0f;
    params3.maxBorderWidth = 10.0f;
    params3.innerBorderBloomWidth = 20.0f;
    params3.outerBorderBloomWidth = 40.0f;
    params3.sdfSpreadFactor = 128.0f;
    auto shader3 = std::make_unique<GESDFEdgeLightShader>(params3);
    shader3->MakeDrawingShader(rect_, 0.0f);
}

/**
 * @tc.name: TypeCheck
 * @tc.desc: Verify shader type and type name are correct.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, TypeCheck, TestSize.Level0)
{
    auto params = MakeParams();
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);

    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::SDF_EDGE_LIGHT_EFFECT);
    EXPECT_EQ(shader->TypeName(), std::string_view("SDFEdgeLightEffect"));
}

} // namespace Rosen
} // namespace OHOS
