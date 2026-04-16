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
#include <any>
#include "ge_spatial_point_light.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "ge_shader_mask.h"

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
 * @tc.desc: Test with boundary values for lightIntensity
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest006, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 0.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    params.attenuation = 0.0f;
    params.lightColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest007
 * @tc.type: FUNC
 * @tc.desc: Test with large values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest007, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 100.0f;
    params.lightPosition = Vector3f(10.0f, 10.0f, 10.0f);
    params.attenuation = 10.0f;
    params.lightColor = Vector4f(10.0f, 10.0f, 10.0f, 10.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 500, 500);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest008
 * @tc.type: FUNC
 * @tc.desc: Test with negative values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest008, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = -1.0f;
    params.lightPosition = Vector3f(-0.5f, -0.5f, -1.0f);
    params.attenuation = -0.5f;
    params.lightColor = Vector4f(-1.0f, -1.0f, -1.0f, -1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
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
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
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
 * @tc.desc: Test Hash() method returns valid hash value
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest010, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    uint32_t hash = shader->Hash();
    EXPECT_EQ(hash, 0u);
}

/**
 * @tc.name: GESpatialPointLightShaderTest011
 * @tc.type: FUNC
 * @tc.desc: Test SetCache and GetCache methods
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest011, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    auto cacheData = std::make_shared<std::any>(std::string("test_cache"));
    shader->SetCache(cacheData);
    auto retrievedCache = shader->GetCache();
    EXPECT_NE(retrievedCache, nullptr);
    EXPECT_NE(*retrievedCache, std::any());
}

/**
 * @tc.name: GESpatialPointLightShaderTest012
 * @tc.type: FUNC
 * @tc.desc: Test GetCache returns nullptr when no cache is set
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest012, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    auto retrievedCache = shader->GetCache();
    EXPECT_EQ(retrievedCache, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest013
 * @tc.type: FUNC
 * @tc.desc: Test multiple calls to GetSpatialPointLightBuilder returns valid builders (thread_local cache)
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest013, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    for (int i = 0; i < 5; i++) {
        auto builder = shader->GetSpatialPointLightBuilder();
        EXPECT_NE(builder, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest014
 * @tc.type: FUNC
 * @tc.desc: Test progress parameter variation (0, 0.5, 1.0)
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest014, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    float progressList[] = { 0.0f, 0.5f, 1.0f };
    for (float progress : progressList) {
        auto shader = Drawing::GESpatialPointLightShader(params);
        Drawing::Rect rect(0, 0, 100, 100);
        shader.MakeDrawingShader(rect, progress);
        auto drawingShader = shader.GetDrawingShader();
        EXPECT_NE(drawingShader, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest015
 * @tc.type: FUNC
 * @tc.desc: Test empty rect handling
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest015, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = Drawing::GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 0, 0);
    auto effect = shader.MakeSpatialPointLightShader(rect);
}

/**
 * @tc.name: GESpatialPointLightShaderTest016
 * @tc.type: FUNC
 * @tc.desc: Test SetSupportHeadroom method
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest016, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    shader->SetSupportHeadroom(0.1f);
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    shader->SetSpatialPointLightParams(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader->MakeDrawingShader(rect, 0.5f);
}

/**
 * @tc.name: GESpatialPointLightShaderTest017
 * @tc.type: FUNC
 * @tc.desc: Test with different attenuation values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest017, TestSize.Level1)
{
    float attenuationList[] = { 0.0f, 0.1f, 0.5f, 1.0f, 32.0f, 64.0f };
    for (float attenuation : attenuationList) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
        params.attenuation = attenuation;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

        auto shader = Drawing::GESpatialPointLightShader(params);
        Drawing::Rect rect(0, 0, 100, 100);
        auto effect = shader.MakeSpatialPointLightShader(rect);
        EXPECT_NE(effect, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest018
 * @tc.type: FUNC
 * @tc.desc: Test with different light color values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest018, TestSize.Level1)
{
    Vector4f colorList[] = {
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
        Vector4f(0.5f, 0.5f, 0.5f, 0.5f),
        Vector4f(0.0f, 0.0f, 0.0f, 0.0f)
    };
    for (const auto& color : colorList) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
        params.attenuation = 0.5f;
        params.lightColor = color;

        auto shader = Drawing::GESpatialPointLightShader(params);
        Drawing::Rect rect(0, 0, 100, 100);
        auto effect = shader.MakeSpatialPointLightShader(rect);
        EXPECT_NE(effect, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest019
 * @tc.type: FUNC
 * @tc.desc: Test with different light position values
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest019, TestSize.Level1)
{
    Vector3f positionList[] = {
        Vector3f(0.0f, 0.0f, 0.0f),
        Vector3f(0.5f, 0.5f, 0.5f),
        Vector3f(1.0f, 1.0f, 1.0f),
        Vector3f(100.0f, 100.0f, 50.0f),
        Vector3f(-100.0f, -100.0f, -50.0f)
    };
    for (const auto& position : positionList) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = position;
        params.attenuation = 0.5f;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

        auto shader = Drawing::GESpatialPointLightShader(params);
        Drawing::Rect rect(0, 0, 100, 100);
        auto effect = shader.MakeSpatialPointLightShader(rect);
        EXPECT_NE(effect, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest020
 * @tc.type: FUNC
 * @tc.desc: Test various rect sizes
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest020, TestSize.Level1)
{
    std::pair<float, float> sizeList[] = {
        { 10.0f, 10.0f },
        { 50.0f, 50.0f },
        { 100.0f, 100.0f },
        { 200.0f, 200.0f },
        { 500.0f, 500.0f },
        { 100.0f, 200.0f },
        { 200.0f, 100.0f }
    };
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    for (const auto& size : sizeList) {
        auto shader = Drawing::GESpatialPointLightShader(params);
        Drawing::Rect rect(0, 0, size.first, size.second);
        auto effect = shader.MakeSpatialPointLightShader(rect);
        EXPECT_NE(effect, nullptr);
    }
}

/**
 * @tc.name: GESpatialPointLightShaderTest021
 * @tc.type: FUNC
 * @tc.desc: Test destructor is properly called
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest021, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    shader->SetSpatialPointLightParams(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader->MakeDrawingShader(rect, 0.5f);
    shader.reset();
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: GESpatialPointLightShaderTest022
 * @tc.type: FUNC
 * @tc.desc: Test with mask nullptr (default behavior)
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest022, TestSize.Level1)
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
 * @tc.name: GESpatialPointLightShaderTest023
 * @tc.type: FUNC
 * @tc.desc: Test chained SetSpatialPointLightParams calls
 */
HWTEST_F(GESpatialPointLightShaderTest, GESpatialPointLightShaderTest023, TestSize.Level1)
{
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>();

    Drawing::GESpatialPointLightShaderParams params1;
    params1.lightIntensity = 1.0f;
    params1.lightPosition = Vector3f(0.3f, 0.3f, 0.5f);
    params1.attenuation = 0.3f;
    params1.lightColor = Vector4f(1.0f, 0.5f, 0.5f, 1.0f);
    shader->SetSpatialPointLightParams(params1);

    Drawing::GESpatialPointLightShaderParams params2;
    params2.lightIntensity = 2.0f;
    params2.lightPosition = Vector3f(0.7f, 0.7f, 1.0f);
    params2.attenuation = 0.7f;
    params2.lightColor = Vector4f(0.5f, 1.0f, 0.5f, 1.0f);
    shader->SetSpatialPointLightParams(params2);

    Drawing::Rect rect(0, 0, 100, 100);
    shader->MakeDrawingShader(rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

}  // namespace Rosen
}  // namespace OHOS
