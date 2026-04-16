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
#include "ge_spatial_point_light.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESpatialPointLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {}
    void TearDown() override
    {}
};

/**
 * @tc.name: GESpatialPointLightShaderTest001
 * @tc.type: FUNC
 * @tc.desc: Test default construction and Type()
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest001, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::SPATIAL_POINT_LIGHT);
    EXPECT_EQ(shader->TypeName(), Drawing::GE_SHADER_SPATIAL_POINT_LIGHT);
    EXPECT_EQ(shader->GetDescription(), "GESpatialPointLightShader");
}

/**
 * @tc.name: GESpatialPointLightShaderTest002
 * @tc.type: FUNC
 * @tc.desc: Test construction with params and GetSpatialPointLightBuilder
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest002, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.5f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.3f;
    params.lightColor = Vector4f(1.0f, 0.8f, 0.6f, 1.0f);

    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    auto builder1 = shader->GetSpatialPointLightBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader->GetSpatialPointLightBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest003
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with valid rect
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest003, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 2.0f;
    params.lightPosition = Vector3f(0.3f, 0.7f, 0.5f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(0.8f, 0.9f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest004
 * @tc.type: FUNC
 * @tc.desc: Test MakeDrawingShader sets drShader_
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest004, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 50, 50);
    shader.MakeDrawingShader(rect, 0.5f);
}

/**
 * @tc.name: GESpatialPointLightShaderTest005
 * @tc.type: FUNC
 * @tc.desc: Test SetSpatialPointLightParams updates internal state
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest005, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    Drawing::GESpatialPointLightShaderParams newParams;
    newParams.lightIntensity = 3.0f;
    newParams.lightPosition = Vector3f(0.8f, 0.2f, 2.0f);
    newParams.attenuation = 0.8f;
    newParams.lightColor = Vector4f(0.5f, 0.5f, 1.0f, 0.8f);

    shader->SetSpatialPointLightParams(newParams);
    Drawing::Rect rect(0, 0, 200, 200);
    shader->MakeDrawingShader(rect, 1.0f);
}

/**
 * @tc.name: GESpatialPointLightShaderTest006
 * @tc.type: FUNC
 * @tc.desc: Test with boundary values (zero, negative, large)
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest006, TestSize.Level1)
{
    // Test zero values
    Drawing::GESpatialPointLightShaderParams paramsZero;
    paramsZero.lightIntensity = 0.0f;
    paramsZero.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    paramsZero.attenuation = 0.0f;
    paramsZero.lightColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    auto shaderZero = Drawing::GESpatialPointLightShader(paramsZero);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shaderZero.MakeSpatialPointLightShader(rect), nullptr);

    // Test large values
    Drawing::GESpatialPointLightShaderParams paramsLarge;
    paramsLarge.lightIntensity = 100.0f;
    paramsLarge.lightPosition = Vector3f(100.0f, 100.0f, 100.0f);
    paramsLarge.attenuation = 64.0f;
    paramsLarge.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shaderLarge = Drawing::GESpatialPointLightShader(paramsLarge);
    EXPECT_NE(shaderLarge.MakeSpatialPointLightShader(rect), nullptr);

    // Test negative values
    Drawing::GESpatialPointLightShaderParams paramsNeg;
    paramsNeg.lightIntensity = -1.0f;
    paramsNeg.lightPosition = Vector3f(-10.0f, -10.0f, -10.0f);
    paramsNeg.attenuation = -0.5f;
    paramsNeg.lightColor = Vector4f(-1.0f, -1.0f, -1.0f, -1.0f);
    auto shaderNeg = Drawing::GESpatialPointLightShader(paramsNeg);
    EXPECT_NE(shaderNeg.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest007
 * @tc.type: FUNC
 * @tc.desc: Test various rect sizes
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest007, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 10, 10),
        Drawing::Rect(0, 0, 50, 50),
        Drawing::Rect(0, 0, 100, 200),
        Drawing::Rect(0, 0, 500, 500)
    };
    for (const auto& rect : rects) {
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest008
 * @tc.type: FUNC
 * @tc.desc: Test different light colors
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest008, TestSize.Level1)
{
    Vector4f colors[] = {
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),  // white
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),  // red
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),  // green
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f)   // blue
    };
    Drawing::Rect rect(0, 0, 100, 100);
    for (const auto& color : colors) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
        params.attenuation = 0.5f;
        params.lightColor = color;
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest009
 * @tc.type: FUNC
 * @tc.desc: Test GetDrawingShader returns valid shader after MakeDrawingShader
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest009, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 16.0f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest010
 * @tc.type: FUNC
 * @tc.desc: Test mask nullptr (default behavior)
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest010, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    params.mask = nullptr;

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest011
 * @tc.type: FUNC
 * @tc.desc: Test various intensity values with different attenuation
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest011, TestSize.Level1)
{
    float intensityValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
    float attenuationValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f};
    Drawing::Rect rect(0, 0, 100, 100);

    for (size_t i = 0; i < sizeof(intensityValues) / sizeof(intensityValues[0]); i++) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = intensityValues[i];
        params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
        params.attenuation = attenuationValues[i];
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest012
 * @tc.type: FUNC
 * @tc.desc: Test extreme position values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest012, TestSize.Level1)
{
    Vector3f positions[] = {
        Vector3f(0.0f, 0.0f, 0.0f),       // Origin
        Vector3f(1000.0f, 1000.0f, 1000.0f), // Large positive
        Vector3f(-500.0f, -500.0f, -500.0f), // Large negative
        Vector3f(0.5f, 0.5f, 0.001f),     // Small Z
        Vector3f(50.0f, 50.0f, 500.0f)    // Large Z
    };
    Drawing::Rect rect(0, 0, 100, 100);

    for (const auto& pos : positions) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = pos;
        params.attenuation = 0.5f;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest013
 * @tc.type: FUNC
 * @tc.desc: Test various light colors including alpha variations
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest013, TestSize.Level1)
{
    Vector4f colors[] = {
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),   // White
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),   // Red
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),   // Green
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f),   // Blue
        Vector4f(1.0f, 1.0f, 0.0f, 1.0f),   // Yellow
        Vector4f(0.5f, 0.5f, 0.5f, 0.5f),   // Semi-transparent gray
        Vector4f(0.0f, 0.0f, 0.0f, 1.0f),   // Black
        Vector4f(1.0f, 0.5f, 0.25f, 0.75f)  // Orange with alpha
    };
    Drawing::Rect rect(0, 0, 100, 100);

    for (const auto& color : colors) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
        params.attenuation = 0.5f;
        params.lightColor = color;
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest014
 * @tc.type: FUNC
 * @tc.desc: Test multiple MakeDrawingShader calls
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest014, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect1(0, 0, 50, 50);
    Drawing::Rect rect2(0, 0, 100, 100);
    Drawing::Rect rect3(0, 0, 200, 200);

    shader.MakeDrawingShader(rect1, 0.5f);
    shader.MakeDrawingShader(rect2, 0.75f);
    shader.MakeDrawingShader(rect3, 1.0f);

    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest015
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with various alpha values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest015, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);

    float alphas[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.5f};
    for (float alpha : alphas) {
        auto effect = shader.MakeSpatialPointLightShader(rect);
        EXPECT_NE(effect, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest016
 * @tc.type: FUNC
 * @tc.desc: Test SetSpatialPointLightParams multiple times
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest016, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    Drawing::Rect rect(0, 0, 100, 100);

    Drawing::GESpatialPointLightShaderParams params1;
    params1.lightIntensity = 1.0f;
    params1.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params1.attenuation = 0.5f;
    params1.lightColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    shader->SetSpatialPointLightParams(params1);
    EXPECT_NE(shader->MakeSpatialPointLightShader(rect), nullptr);

    Drawing::GESpatialPointLightShaderParams params2;
    params2.lightIntensity = 2.0f;
    params2.lightPosition = Vector3f(75.0f, 75.0f, 50.0f);
    params2.attenuation = 1.0f;
    params2.lightColor = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    shader->SetSpatialPointLightParams(params2);
    EXPECT_NE(shader->MakeSpatialPointLightShader(rect), nullptr);

    Drawing::GESpatialPointLightShaderParams params3;
    params3.lightIntensity = 0.5f;
    params3.lightPosition = Vector3f(25.0f, 25.0f, 200.0f);
    params3.attenuation = 0.1f;
    params3.lightColor = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    shader->SetSpatialPointLightParams(params3);
    EXPECT_NE(shader->MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest017
 * @tc.type: FUNC
 * @tc.desc: Test different rect aspect ratios
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest017, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 10, 100),    // Tall narrow
        Drawing::Rect(0, 0, 100, 10),    // Wide short
        Drawing::Rect(0, 0, 200, 100),   // Wide medium
        Drawing::Rect(0, 0, 100, 200),   // Tall medium
        Drawing::Rect(0, 0, 1, 1000),    // Very tall
        Drawing::Rect(0, 0, 1000, 1)     // Very wide
    };

    for (const auto& rect : rects) {
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest018
 * @tc.type: FUNC
 * @tc.desc: Test builder caching behavior
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest018, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    auto builder1 = shader->GetSpatialPointLightBuilder();
    EXPECT_NE(builder1, nullptr);

    auto builder2 = shader->GetSpatialPointLightBuilder();
    EXPECT_NE(builder2, nullptr);

    auto builder3 = shader->GetSpatialPointLightBuilder();
    EXPECT_NE(builder3, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest019
 * @tc.type: FUNC
 * @tc.desc: Test shader after params update with GetDrawingShader
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest019, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    Drawing::Rect rect(0, 0, 100, 100);

    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.5f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    shader->SetSpatialPointLightParams(params);

    shader->MakeDrawingShader(rect, 1.0f);
    auto drawingShader = shader->GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);

    params.lightIntensity = 3.0f;
    shader->SetSpatialPointLightParams(params);
    shader->MakeDrawingShader(rect, 0.5f);
    drawingShader = shader->GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest020
 * @tc.type: FUNC
 * @tc.desc: Test edge cases for light position coordinates
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest020, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);

    // Edge positions
    Vector3f edgePositions[] = {
        Vector3f(0.0f, 0.0f, 100.0f),     // Top-left corner
        Vector3f(100.0f, 0.0f, 100.0f),   // Top-right corner
        Vector3f(0.0f, 100.0f, 100.0f),   // Bottom-left corner
        Vector3f(100.0f, 100.0f, 100.0f), // Bottom-right corner
        Vector3f(50.0f, 0.0f, 100.0f),    // Top center
        Vector3f(50.0f, 100.0f, 100.0f),  // Bottom center
        Vector3f(0.0f, 50.0f, 100.0f),    // Left center
        Vector3f(100.0f, 50.0f, 100.0f)   // Right center
    };

    for (const auto& pos : edgePositions) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = pos;
        params.attenuation = 0.5f;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        auto shader = Drawing::GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

}  // namespace Rosen
}  // namespace OHOS
