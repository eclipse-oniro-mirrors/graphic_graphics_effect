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
#include "ge_sdf_edge_light_shader.h"
#include "ge_sdf_rrect_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class GESDFEdgeLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::GEShaderMask> lightMask_ {};
    std::shared_ptr<Drawing::GESDFShaderShape> sdfShape_ {};

    Drawing::Rect rect_ { 20.0f, 20.0f, 200.0f, 200.0f };

private:
    Drawing::GESDFEdgeLightEffectParams MakeParams();
};

void GESDFEdgeLightShaderTest::SetUpTestCase() {}

void GESDFEdgeLightShaderTest::TearDownTestCase() {}

void GESDFEdgeLightShaderTest::SetUp()
{
    // Create a light mask
    Drawing::GEDoubleRippleShaderMaskParams mp;
    lightMask_ = std::make_shared<Drawing::GEDoubleRippleShaderMask>(mp);
    auto rrect = Drawing::GERRect{ 0.0f, 0.0f, 100.0f, 100.0f };
    rrect.SetCornerRadius(10.0f, 10.0f);
    sdfShape_ = std::make_shared<Drawing::GESDFRRectShaderShape>(Drawing::GESDFRRectShapeParams {
        rrect
    });
}

void GESDFEdgeLightShaderTest::TearDown()
{
    lightMask_ = nullptr;
    sdfShape_ = nullptr;
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
    // sdfShape and lightMask can be set in params before constructing shader
    return p;
}

/**
 * @tc.name: MakeDrawingShader_InvalidInputs
 * @tc.desc: Verify MakeDrawingShader handles invalid inputs gracefully (null sdfShape or lightMask).
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, MakeDrawingShader_InvalidInputs, TestSize.Level0)
{
    auto params = MakeParams();
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);
    // Case 1: sdfShape not set → MakeDrawingShader should complete but shader may be invalid
    shader->MakeDrawingShader(canvas_, rect_, 0.0f);
    EXPECT_EQ(shader->drShader_, nullptr);

    // Case 2: lightMask not set → MakeDrawingShader should complete but shader may be invalid
    auto params2 = MakeParams();
    params2.sdfShape = sdfShape_;
    // lightMask is not set (nullptr)
    auto shader2 = std::make_unique<GESDFEdgeLightShader>(params2);
    shader2->MakeDrawingShader(canvas_, rect_, 0.0f);
    EXPECT_EQ(shader2->drShader_, nullptr);
}

/**
 * @tc.name: ParameterVariations_Smoke
 * @tc.desc: Verify different parameter combinations work correctly on CPU.
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, ParameterVariations_Smoke, TestSize.Level0)
{
    ASSERT_NE(lightMask_, nullptr);

    auto params1 = MakeParams();
    params1.lightMask = lightMask_;
    params1.sdfShape = sdfShape_;
    auto shader1 = std::make_unique<GESDFEdgeLightShader>(params1);
    shader1->MakeDrawingShader(canvas_, rect_, 0.0f);
    // Test effectShader when second enter
    shader1->MakeDrawingShader(canvas_, rect_, 0.0f);
    EXPECT_NE(shader1->drShader_, nullptr);

    // Test with different parameters
    auto params3 = MakeParams();
    params3.lightMask = lightMask_;
    params3.sdfShape = sdfShape_;
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
    shader3->MakeDrawingShader(canvas_, rect_, 0.0f);
    EXPECT_NE(shader3->drShader_, nullptr);
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

/**
 * @tc.name: OnDrawTest001
 * @tc.desc: test OnDraw with valid shader
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, OnDrawTest001, TestSize.Level0)
{
    ASSERT_NE(sdfShape_, nullptr);
    ASSERT_NE(lightMask_, nullptr);
    auto params = MakeParams();
    params.sdfShape = sdfShape_;
    params.lightMask = lightMask_;
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);
    Drawing::Canvas canvas;
    shader->MakeDrawingShader(canvas, rect_, 0.0f);
    shader->OnDrawShader(canvas, rect_);
    EXPECT_NE(shader->drShader_, nullptr);
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc: test OnDraw with null shader
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, OnDrawTest002, TestSize.Level0)
{
    auto params = MakeParams();
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);
    Drawing::Canvas canvas;
    shader->MakeDrawingShader(canvas, rect_, 0.0f);
    shader->OnDrawShader(canvas, rect_);
    EXPECT_EQ(shader->drShader_, nullptr);
}

/**
 * @tc.name: GetEffectShaderBuilderTest001
 * @tc.desc: test GetEffectShaderBuilder with valid params
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEdgeLightShaderTest, GetEffectShaderBuilderTest001, TestSize.Level0)
{
    ASSERT_NE(sdfShape_, nullptr);
    ASSERT_NE(lightMask_, nullptr);
    auto params = MakeParams();
    params.sdfShape = sdfShape_;
    params.lightMask = lightMask_;
    auto shader = std::make_unique<GESDFEdgeLightShader>(params);
    auto builder = shader->GetEffectShaderBuilder(canvas_, rect_);
    EXPECT_NE(builder, nullptr);
}
} // namespace Rosen
} // namespace OHOS