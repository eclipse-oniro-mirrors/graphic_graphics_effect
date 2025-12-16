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

#include "ge_color_gradient_effect.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_shader_filter_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEColorGradientEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    Drawing::Rect rect_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::GERadialGradientShaderMaskParams maskParam_{};
};

void GEColorGradientEffectTest::SetUpTestCase(void) {}

void GEColorGradientEffectTest::TearDownTestCase(void) {}

void GEColorGradientEffectTest::SetUp()
{
    maskParam_.radiusX_ = 1.0f;
    maskParam_.radiusY_ = 2.0f;
    size_t size = 1;
    maskParam_.colors_ = std::vector<float>(size, 1.0f);
    maskParam_.positions_ = std::vector<float>(size, 1.0f);
}

void GEColorGradientEffectTest::TearDown() {}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: test MakeDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GEColorGradientEffectTest, MakeDrawingShader_001, TestSize.Level1)
{
    Drawing::GEXColorGradientEffectParams param{};
    auto shader = std::make_shared<GEColorGradientEffect>(param);
    shader->MakeDrawingShader(Drawing::Rect{0.0f, 0.0f, 0.0f, 0.0f}, 0.0f);
    EXPECT_EQ(shader->drShader_, nullptr);
    shader->MakeDrawingShader(rect_, 0.0f);
    EXPECT_NE(shader->drShader_, nullptr);
    param.mask_ = std::make_shared<Drawing::GERadialGradientShaderMask>(maskParam_);
    auto shaderWithMask = std::make_shared<GEColorGradientEffect>(param);
    shaderWithMask->MakeDrawingShader(rect_, 0.0f);
    EXPECT_NE(shaderWithMask->drShader_, nullptr);
}

/**
 * @tc.name: CreateShaderEffect_001
 * @tc.desc: test CreateShaderEffect function
 * @tc.type: FUNC
 */
HWTEST_F(GEColorGradientEffectTest, CreateShaderEffect_001, TestSize.Level1)
{
    Drawing::GEXColorGradientEffectParams param{};
    auto shaderNoMask = std::make_shared<GEColorGradientEffect>(param);
    EXPECT_FALSE(shaderNoMask->CreateShaderEffect(0.0f, 1.0f));
    EXPECT_FALSE(shaderNoMask->CreateShaderEffect(1.0f, 0.0f));
    shaderNoMask->positions_.clear();
    shaderNoMask->strengths_.clear();
    shaderNoMask->colors_.clear();
    EXPECT_FALSE(shaderNoMask->CreateShaderEffect(1.0f, 1.0f));
    const size_t sizeColors = 12;
    auto colors = std::vector<Drawing::Color4f>(sizeColors, Drawing::Color4f{0.f, 0.0f, 0.0f, 0.0f});
    shaderNoMask->colors_ = colors;
    EXPECT_FALSE(shaderNoMask->CreateShaderEffect(1.0f, 1.0f));
    auto positions = std::vector<Drawing::Point>(sizeColors, Drawing::Point{0.5f, 0.3f});
    shaderNoMask->positions_ = positions;
    EXPECT_FALSE(shaderNoMask->CreateShaderEffect(1.0f, 1.0f));
    auto strengths = std::vector<float>(sizeColors, 2.0f);
    shaderNoMask->strengths_ = strengths;
    EXPECT_TRUE(shaderNoMask->CreateShaderEffect(1.0f, 1.0f));
    EXPECT_TRUE(shaderNoMask->CreateShaderEffect(1.0f, 1.0f));
    param.mask_ = std::make_shared<Drawing::GERadialGradientShaderMask>(maskParam_);
    auto shaderWithMask = std::make_shared<GEColorGradientEffect>(param);
    shaderWithMask->colors_ = colors;
    shaderWithMask->positions_ = positions;
    shaderWithMask->strengths_ = strengths;
    EXPECT_TRUE(shaderWithMask->CreateShaderEffect(1.0f, 1.0f));
    EXPECT_TRUE(shaderWithMask->CreateShaderEffect(1.0f, 1.0f));
}
} // namespace Rosen
} // namespace OHOS
