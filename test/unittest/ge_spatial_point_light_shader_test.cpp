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
#include "ge_spatial_point_light.h"

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

}  // namespace Rosen
}  // namespace OHOS
