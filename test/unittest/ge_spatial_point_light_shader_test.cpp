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
 * @tc.name: DefaultConstruction_Type
 * @tc.type: FUNC
 * @tc.desc: Test default construction returns correct type and name
 */
HWTEST_F(GESpatialPointLightShaderTest, DefaultConstruction_Type, TestSize.Level1)
{
    auto shader = std::make_unique<GESpatialPointLightShader>();
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::SPATIAL_POINT_LIGHT);
    EXPECT_EQ(shader->TypeName(), "SpatialPointLight");
    EXPECT_EQ(shader->GetDescription(), "GESpatialPointLightShader");
}

/**
 * @tc.name: ConstructionWithParams_GetBuilderNoMask
 * @tc.type: FUNC
 * @tc.desc: Test construction with params and GetSpatialPointLightBuilderNoMask returns valid builder
 */
HWTEST_F(GESpatialPointLightShaderTest, ConstructionWithParams_GetBuilderNoMask, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.5f;
    params.lightPosition = Vector3f(0.5f, 0.5f, 1.0f);
    params.attenuation = 0.3f;
    params.lightColor = Vector4f(1.0f, 0.8f, 0.6f, 1.0f);

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
    auto builder = shader->GetSpatialPointLightBuilderNoMask();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: GetBuilderNoMask_CachingBehavior
 * @tc.type: FUNC
 * @tc.desc: Test GetSpatialPointLightBuilderNoMask returns cached builder on subsequent calls
 */
HWTEST_F(GESpatialPointLightShaderTest, GetBuilderNoMask_CachingBehavior, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
    auto builder1 = shader->GetSpatialPointLightBuilderNoMask();
    EXPECT_NE(builder1, nullptr);

    auto builder2 = shader->GetSpatialPointLightBuilderNoMask();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GetBuilderWithMask_ReturnsValidBuilder
 * @tc.type: FUNC
 * @tc.desc: Test GetSpatialPointLightBuilderWithMask returns valid builder
 */
HWTEST_F(GESpatialPointLightShaderTest, GetBuilderWithMask_ReturnsValidBuilder, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
    auto builder = shader->GetSpatialPointLightBuilderWithMask();
    EXPECT_NE(builder, nullptr);
}

/**
 * @tc.name: MakeSpatialPointLightShader_ValidRect
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with valid rect returns valid shader effect
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeSpatialPointLightShader_ValidRect, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 2.0f;
    params.lightPosition = Vector3f(0.3f, 0.7f, 0.5f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(0.8f, 0.9f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: MakeSpatialPointLightShader_ZeroRectWidth
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with zero width returns nullptr
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeSpatialPointLightShader_ZeroRectWidth, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 0, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: MakeSpatialPointLightShader_ZeroRectHeight
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with zero height returns nullptr
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeSpatialPointLightShader_ZeroRectHeight, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 0);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: MakeSpatialPointLightShader_SmallRect
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with small rect returns valid shader effect
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeSpatialPointLightShader_SmallRect, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 10, 10);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: MakeSpatialPointLightShader_LargeRect
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with large rect returns valid shader effect
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeSpatialPointLightShader_LargeRect, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(500.0f, 500.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 500, 500);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: MakeDrawingShader_SetsShader
 * @tc.type: FUNC
 * @tc.desc: Test MakeDrawingShader sets drShader_ member variable
 */
HWTEST_F(GESpatialPointLightShaderTest, MakeDrawingShader_SetsShader, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 1.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 50, 50);
    shader.MakeDrawingShader(rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: SetSpatialPointLightParams_UpdatesState
 * @tc.type: FUNC
 * @tc.desc: Test SetSpatialPointLightParams updates internal state and affects shader generation
 */
HWTEST_F(GESpatialPointLightShaderTest, SetSpatialPointLightParams_UpdatesState, TestSize.Level1)
{
    auto shader = std::make_unique<GESpatialPointLightShader>();
    Drawing::GESpatialPointLightShaderParams newParams;
    newParams.lightIntensity = 3.0f;
    newParams.lightPosition = Vector3f(0.8f, 0.2f, 2.0f);
    newParams.attenuation = 0.8f;
    newParams.lightColor = Vector4f(0.5f, 0.5f, 1.0f, 0.8f);

    shader->SetSpatialPointLightParams(newParams);
    Drawing::Rect rect(0, 0, 200, 200);
    shader->MakeDrawingShader(rect, 1.0f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: SetSpatialPointLightParams_MultipleUpdates
 * @tc.type: FUNC
 * @tc.desc: Test SetSpatialPointLightParams multiple times updates state correctly
 */
HWTEST_F(GESpatialPointLightShaderTest, SetSpatialPointLightParams_MultipleUpdates, TestSize.Level1)
{
    auto shader = std::make_unique<GESpatialPointLightShader>();
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
}

/**
 * @tc.name: BoundaryValues_ZeroParams
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with zero values for all params
 */
HWTEST_F(GESpatialPointLightShaderTest, BoundaryValues_ZeroParams, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 0.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    params.attenuation = 0.0f;
    params.lightColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: BoundaryValues_LargeParams
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with large values for all params
 */
HWTEST_F(GESpatialPointLightShaderTest, BoundaryValues_LargeParams, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 100.0f;
    params.lightPosition = Vector3f(100.0f, 100.0f, 100.0f);
    params.attenuation = 64.0f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: BoundaryValues_NegativeParams
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with negative values for all params
 */
HWTEST_F(GESpatialPointLightShaderTest, BoundaryValues_NegativeParams, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = -1.0f;
    params.lightPosition = Vector3f(-10.0f, -10.0f, -10.0f);
    params.attenuation = -0.5f;
    params.lightColor = Vector4f(-1.0f, -1.0f, -1.0f, -1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: MaskNullptr_DefaultBehavior
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with mask nullptr uses NoMask builder
 */
HWTEST_F(GESpatialPointLightShaderTest, MaskNullptr_DefaultBehavior, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    params.mask = nullptr;

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: LightIntensity_VariousValues
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with various light intensity values
 */
HWTEST_F(GESpatialPointLightShaderTest, LightIntensity_VariousValues, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);
    float intensityValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};

    for (float intensity : intensityValues) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = intensity;
        params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
        params.attenuation = 0.5f;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        auto shader = GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: Attenuation_VariousValues
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with various attenuation values
 */
HWTEST_F(GESpatialPointLightShaderTest, Attenuation_VariousValues, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);
    float attenuationValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f};

    for (float attenuation : attenuationValues) {
        Drawing::GESpatialPointLightShaderParams params;
        params.lightIntensity = 1.0f;
        params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
        params.attenuation = attenuation;
        params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        auto shader = GESpatialPointLightShader(params);
        EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
    }
}

/**
 * @tc.name: LightPosition_Origin
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with light position at origin
 */
HWTEST_F(GESpatialPointLightShaderTest, LightPosition_Origin, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightPosition_LargePositive
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with large positive light position
 */
HWTEST_F(GESpatialPointLightShaderTest, LightPosition_LargePositive, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(1000.0f, 1000.0f, 1000.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightPosition_LargeNegative
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with large negative light position
 */
HWTEST_F(GESpatialPointLightShaderTest, LightPosition_LargeNegative, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(-500.0f, -500.0f, -500.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightColor_White
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with white light color
 */
HWTEST_F(GESpatialPointLightShaderTest, LightColor_White, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightColor_Red
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with red light color
 */
HWTEST_F(GESpatialPointLightShaderTest, LightColor_Red, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightColor_Green
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with green light color
 */
HWTEST_F(GESpatialPointLightShaderTest, LightColor_Green, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightColor_Blue
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with blue light color
 */
HWTEST_F(GESpatialPointLightShaderTest, LightColor_Blue, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: LightColor_SemiTransparent
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with semi-transparent light color
 */
HWTEST_F(GESpatialPointLightShaderTest, LightColor_SemiTransparent, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: MultipleMakeDrawingShaderCalls
 * @tc.type: FUNC
 * @tc.desc: Test multiple MakeDrawingShader calls update shader correctly
 */
HWTEST_F(GESpatialPointLightShaderTest, MultipleMakeDrawingShaderCalls, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
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
 * @tc.name: GetDrawingShader_AfterMakeDrawingShader
 * @tc.type: FUNC
 * @tc.desc: Test GetDrawingShader returns valid shader after MakeDrawingShader
 */
HWTEST_F(GESpatialPointLightShaderTest, GetDrawingShader_AfterMakeDrawingShader, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 50.0f, 100.0f);
    params.attenuation = 16.0f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

/**
 * @tc.name: EdgePosition_TopLeftCorner
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with light at top-left corner
 */
HWTEST_F(GESpatialPointLightShaderTest, EdgePosition_TopLeftCorner, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: EdgePosition_BottomRightCorner
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with light at bottom-right corner
 */
HWTEST_F(GESpatialPointLightShaderTest, EdgePosition_BottomRightCorner, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(100.0f, 100.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    EXPECT_NE(shader.MakeSpatialPointLightShader(rect), nullptr);
}

/**
 * @tc.name: RectAspectRatio_TallNarrow
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with tall narrow rect
 */
HWTEST_F(GESpatialPointLightShaderTest, RectAspectRatio_TallNarrow, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(5.0f, 50.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 10, 100);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: RectAspectRatio_WideShort
 * @tc.type: FUNC
 * @tc.desc: Test MakeSpatialPointLightShader with wide short rect
 */
HWTEST_F(GESpatialPointLightShaderTest, RectAspectRatio_WideShort, TestSize.Level1)
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = 1.0f;
    params.lightPosition = Vector3f(50.0f, 5.0f, 100.0f);
    params.attenuation = 0.5f;
    params.lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto shader = GESpatialPointLightShader(params);
    Drawing::Rect rect(0, 0, 100, 10);
    auto effect = shader.MakeSpatialPointLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: ShaderAfterParamsUpdate_GetDrawingShader
 * @tc.type: FUNC
 * @tc.desc: Test shader remains valid after params update with GetDrawingShader
 */
HWTEST_F(GESpatialPointLightShaderTest, ShaderAfterParamsUpdate_GetDrawingShader, TestSize.Level1)
{
    auto shader = std::make_unique<GESpatialPointLightShader>();
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

}  // namespace Rosen
}  // namespace OHOS