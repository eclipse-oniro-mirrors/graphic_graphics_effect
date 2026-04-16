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

}  // namespace Rosen
}  // namespace OHOS
