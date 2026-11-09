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

#include <array>
#include <memory>
#include <vector>

#include "ge_point_light_shader.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_shader_filter_params.h"
#include "ge_tone_mapping_helper.h"
#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "utils/rect.h"
#include "utils/round_rect.h"
#include "common/rs_vector4.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
constexpr float TEST_LIGHT_INTENSITY = 1.0f;
constexpr float TEST_BORDER_WIDTH = 5.0f;
constexpr float TEST_HEADROOM = 1.0f;
constexpr float TEST_RADIUS_SMALL = 10.0f;
constexpr float TEST_RADIUS_LARGE = 40.0f;
constexpr float TEST_RECT_SIZE = 100.0f;
} // namespace

class GEPointLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

    static Drawing::GEPointLightShaderParams MakeParams(int illuminatedType, size_t lightCount,
        bool withShape = false, bool enableEDR = false, float radius = TEST_RADIUS_SMALL);
    static std::shared_ptr<Drawing::GESDFShaderShape> MakeShape();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPhongBuilder(size_t lightLength);

    static inline Drawing::Canvas canvas_;
};

std::shared_ptr<Drawing::GESDFShaderShape> GEPointLightShaderTest::MakeShape()
{
    Drawing::GESDFRRectShapeParams shapeParams;
    shapeParams.rrect.left_ = 0.f;
    shapeParams.rrect.top_ = 0.f;
    shapeParams.rrect.width_ = TEST_RECT_SIZE;
    shapeParams.rrect.height_ = TEST_RECT_SIZE;
    shapeParams.rrect.SetCornerRadius(TEST_RADIUS_SMALL, TEST_RADIUS_SMALL);
    return std::make_shared<Drawing::GESDFRRectShaderShape>(shapeParams);
}

Drawing::GEPointLightShaderParams GEPointLightShaderTest::MakeParams(int illuminatedType, size_t lightCount,
    bool withShape, bool enableEDR, float radius)
{
    Drawing::GEPointLightShaderParams params;
    params.illuminatedType = illuminatedType;
    params.illuminatedBorderWidth = TEST_BORDER_WIDTH;
    params.bloom = 0.5f;
    params.displayHeadroom = TEST_HEADROOM;
    params.enableEDREffect = enableEDR;
    if (withShape) {
        params.shape = MakeShape();
    }
    params.contentRRect.left_ = 0.f;
    params.contentRRect.top_ = 0.f;
    params.contentRRect.width_ = TEST_RECT_SIZE;
    params.contentRRect.height_ = TEST_RECT_SIZE;
    params.contentRRect.SetCornerRadius(radius, radius);
    params.borderRRect.left_ = 0.f;
    params.borderRRect.top_ = 0.f;
    params.borderRRect.width_ = TEST_RECT_SIZE;
    params.borderRRect.height_ = TEST_RECT_SIZE;
    params.borderRRect.SetCornerRadius(radius, radius);
    params.drawRect = Vector4f(0.f, 0.f, TEST_RECT_SIZE, TEST_RECT_SIZE);
    for (size_t i = 0; i < lightCount; i++) {
        params.lightPositions.push_back(Vector4f(50.f, 50.f, 100.f, 200.f));
        params.lightColors.push_back(Vector4f(255.f, 255.f, 255.f, 255.f));
        params.lightIntensities.push_back(TEST_LIGHT_INTENSITY);
    }
    return params;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShaderTest::GetPhongBuilder(size_t lightLength)
{
    return GEPointLightShader::GetPhongShaderBuilder(lightLength);
}

// ===================== Basic interface =====================

/**
 * @tc.name: Constructor_ValidParams
 * @tc.type: FUNC
 * @tc.desc: Test constructor maps valid params into params_ correctly
 */
HWTEST_F(GEPointLightShaderTest, Constructor_ValidParams, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    auto shader = std::make_unique<GEPointLightShader>(params);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->params_.illuminatedType, IlluminatedType::CONTENT);
    EXPECT_FLOAT_EQ(shader->params_.borderWidth, TEST_BORDER_WIDTH);
    EXPECT_FLOAT_EQ(shader->params_.displayHeadroom, TEST_HEADROOM);
    EXPECT_EQ(shader->params_.lightSources.size(), 1u);
    EXPECT_EQ(shader->params_.sdfShaderEffect, nullptr);
}

/**
 * @tc.name: Constructor_InvalidIlluminatedType
 * @tc.type: FUNC
 * @tc.desc: Test constructor maps out-of-range illuminatedType to INVALID
 */
HWTEST_F(GEPointLightShaderTest, Constructor_InvalidIlluminatedType, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT) + 1, 1);
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_EQ(shader->params_.illuminatedType, IlluminatedType::INVALID);
}

/**
 * @tc.name: Constructor_SizeMismatch
 * @tc.type: FUNC
 * @tc.desc: Test constructor returns empty lightSources when vector sizes mismatch
 */
HWTEST_F(GEPointLightShaderTest, Constructor_SizeMismatch, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    params.lightColors.clear(); // mismatch: 1 position/intensity but 0 colors
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_TRUE(shader->params_.lightSources.empty());
}

/**
 * @tc.name: Constructor_WithShape
 * @tc.type: FUNC
 * @tc.desc: Test constructor generates sdfShaderEffect when shape provided
 */
HWTEST_F(GEPointLightShaderTest, Constructor_WithShape, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1, true);
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_NE(shader->params_.sdfShaderEffect, nullptr);
}

/**
 * @tc.name: TypeAndTypeName
 * @tc.type: FUNC
 * @tc.desc: Test Type() and TypeName() return POINT_LIGHT / "PointLight"
 */
HWTEST_F(GEPointLightShaderTest, TypeAndTypeName, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::POINT_LIGHT);
    EXPECT_EQ(shader->TypeName(), "PointLight");
}

/**
 * @tc.name: GetDescription
 * @tc.type: FUNC
 * @tc.desc: Test GetDescription returns "GEPointLightShader"
 */
HWTEST_F(GEPointLightShaderTest, GetDescription, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_EQ(shader->GetDescription(), "GEPointLightShader");
}

// ===================== OnDrawShader / DrawLight =====================

/**
 * @tc.name: OnDrawShader_EmptyLightSources
 * @tc.type: FUNC
 * @tc.desc: Test OnDrawShader with empty lightSources returns early
 */
HWTEST_F(GEPointLightShaderTest, OnDrawShader_EmptyLightSources, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    params.lightColors.clear(); // cause mismatch -> empty lightSources
    auto shader = std::make_unique<GEPointLightShader>(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader->OnDrawShader(canvas_, rect);
    EXPECT_TRUE(shader->params_.lightSources.empty());
}

/**
 * @tc.name: OnDrawShader_SingleLight
 * @tc.type: FUNC
 * @tc.desc: Test OnDrawShader with single light source draws without crash
 */
HWTEST_F(GEPointLightShaderTest, OnDrawShader_SingleLight, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1));
    Drawing::Rect rect(0, 0, 100, 100);
    shader->OnDrawShader(canvas_, rect);
    EXPECT_TRUE(shader->params_.lightSources.size() == 1);
}

/**
 * @tc.name: OnDrawShader_MultiLight
 * @tc.type: FUNC
 * @tc.desc: Test OnDrawShader with multiple light sources draws without crash
 */
HWTEST_F(GEPointLightShaderTest, OnDrawShader_MultiLight, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 2));
    Drawing::Rect rect(0, 0, 100, 100);
    shader->OnDrawShader(canvas_, rect);
    EXPECT_TRUE(shader->params_.lightSources.size() == 2);
}

/**
 * @tc.name: DrawLight_EmptyLightSources
 * @tc.type: FUNC
 * @tc.desc: Test DrawLight with empty lightSources returns early
 */
HWTEST_F(GEPointLightShaderTest, DrawLight_EmptyLightSources, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1);
    params.lightColors.clear();
    auto shader = std::make_unique<GEPointLightShader>(params);
    shader->DrawLight(canvas_);
    EXPECT_TRUE(shader->params_.lightSources.empty());
}

/**
 * @tc.name: DrawLight_NullBuilder
 * @tc.type: FUNC
 * @tc.desc: Test DrawLight returns early when CreateShaderBuilder returns null (FEATHERING_BORDER + sdf)
 */
HWTEST_F(GEPointLightShaderTest, DrawLight_NullBuilder, TestSize.Level1)
{
    auto params = MakeParams(static_cast<int>(IlluminatedType::FEATHERING_BORDER), 1, true);
    auto shader = std::make_unique<GEPointLightShader>(params);
    EXPECT_NE(shader->params_.sdfShaderEffect, nullptr);
    shader->DrawLight(canvas_);
    EXPECT_TRUE(shader->params_.lightSources.size() == 1);
}

/**
 * @tc.name: DrawLight_SingleLight
 * @tc.type: FUNC
 * @tc.desc: Test DrawLight single-light path
 */
HWTEST_F(GEPointLightShaderTest, DrawLight_SingleLight, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 1));
    shader->DrawLight(canvas_);
    EXPECT_TRUE(shader->params_.lightSources.size() == 1);
}

/**
 * @tc.name: DrawLight_MultiLight
 * @tc.type: FUNC
 * @tc.desc: Test DrawLight multi-light path
 */
HWTEST_F(GEPointLightShaderTest, DrawLight_MultiLight, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 2));
    shader->DrawLight(canvas_);
    EXPECT_TRUE(shader->params_.lightSources.size() == 2);
}

// ===================== DrawLightByIlluminatedType (multi-light dispatch) =====================

/**
 * @tc.name: DrawLightByIlluminatedType_AllBranches
 * @tc.type: FUNC
 * @tc.desc: Test DrawLightByIlluminatedType dispatches for each IlluminatedType branch
 */
HWTEST_F(GEPointLightShaderTest, DrawLightByIlluminatedType_AllBranches, TestSize.Level1)
{
    std::vector<int> types = {
        static_cast<int>(IlluminatedType::BORDER_CONTENT),
        static_cast<int>(IlluminatedType::BLEND_BORDER_CONTENT),
        static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT),
        static_cast<int>(IlluminatedType::CONTENT),
        static_cast<int>(IlluminatedType::BLEND_CONTENT),
        static_cast<int>(IlluminatedType::BORDER),
        static_cast<int>(IlluminatedType::BLEND_BORDER),
        static_cast<int>(IlluminatedType::FEATHERING_BORDER),
        static_cast<int>(IlluminatedType::NONE),
    };
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (size_t i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    for (int t : types) {
        auto shader = std::make_unique<GEPointLightShader>(MakeParams(t, 2));
        shader->DrawLightByIlluminatedType(canvas_, builder, intensityArray);
    }
    SUCCEED();
}

// ===================== DrawSingleLightByIlluminatedType (single-light dispatch) =====================

/**
 * @tc.name: DrawSingleLightByIlluminatedType_AllBranches
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleLightByIlluminatedType dispatches for each IlluminatedType branch
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleLightByIlluminatedType_AllBranches, TestSize.Level1)
{
    std::vector<int> types = {
        static_cast<int>(IlluminatedType::BORDER_CONTENT),
        static_cast<int>(IlluminatedType::BLEND_BORDER_CONTENT),
        static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT),
        static_cast<int>(IlluminatedType::CONTENT),
        static_cast<int>(IlluminatedType::BLEND_CONTENT),
        static_cast<int>(IlluminatedType::BORDER),
        static_cast<int>(IlluminatedType::BLEND_BORDER),
        static_cast<int>(IlluminatedType::FEATHERING_BORDER),
        static_cast<int>(IlluminatedType::NONE),
    };
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    for (int t : types) {
        auto shader = std::make_unique<GEPointLightShader>(MakeParams(t, 1));
        shader->DrawSingleLightByIlluminatedType(canvas_, builder, intensityArray);
    }
    SUCCEED();
}

// ===================== ProcessLightSourcesData / ProcessSingleLightSourcesData =====================

/**
 * @tc.name: ProcessLightSourcesData_NoEDR
 * @tc.type: FUNC
 * @tc.desc: Test ProcessLightSourcesData fills intensity array without EDR tone mapping
 */
HWTEST_F(GEPointLightShaderTest, ProcessLightSourcesData_NoEDR, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    shader->ProcessLightSourcesData(intensityArray, builder);
    EXPECT_FLOAT_EQ(intensityArray[0], TEST_LIGHT_INTENSITY);
    EXPECT_FLOAT_EQ(intensityArray[1], TEST_LIGHT_INTENSITY);
}

/**
 * @tc.name: ProcessLightSourcesData_WithEDR
 * @tc.type: FUNC
 * @tc.desc: Test ProcessLightSourcesData covers EDR tone-mapping branch
 */
HWTEST_F(GEPointLightShaderTest, ProcessLightSourcesData_WithEDR, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::CONTENT), 2, false, true));
    EXPECT_TRUE(GEToneMappingHelper::NeedToneMapping(shader->params_.displayHeadroom));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    shader->ProcessLightSourcesData(intensityArray, builder);
    EXPECT_FLOAT_EQ(intensityArray[0], TEST_LIGHT_INTENSITY);
}

/**
 * @tc.name: ProcessLightSourcesData_TruncateAtMax
 * @tc.type: FUNC
 * @tc.desc: Test ProcessLightSourcesData truncates beyond MAX_LIGHT_SOURCES
 */
HWTEST_F(GEPointLightShaderTest, ProcessLightSourcesData_TruncateAtMax, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 15));
    EXPECT_EQ(shader->params_.lightSources.size(), 15u);
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    shader->ProcessLightSourcesData(intensityArray, builder);
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        EXPECT_FLOAT_EQ(intensityArray[i], TEST_LIGHT_INTENSITY);
    }
}

/**
 * @tc.name: ProcessSingleLightSourcesData_NoEDR
 * @tc.type: FUNC
 * @tc.desc: Test ProcessSingleLightSourcesData fills intensity without EDR
 */
HWTEST_F(GEPointLightShaderTest, ProcessSingleLightSourcesData_NoEDR, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { 0 };
    shader->ProcessSingleLightSourcesData(intensityArray, builder);
    EXPECT_FLOAT_EQ(intensityArray[0], TEST_LIGHT_INTENSITY);
}

/**
 * @tc.name: ProcessSingleLightSourcesData_WithEDR
 * @tc.type: FUNC
 * @tc.desc: Test ProcessSingleLightSourcesData covers EDR tone-mapping branch
 */
HWTEST_F(GEPointLightShaderTest, ProcessSingleLightSourcesData_WithEDR, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1, false, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { 0 };
    shader->ProcessSingleLightSourcesData(intensityArray, builder);
    EXPECT_FLOAT_EQ(intensityArray[0], TEST_LIGHT_INTENSITY);
}

// ===================== CreateShaderBuilder =====================

/**
 * @tc.name: CreateShaderBuilder_NormalBorderContent
 * @tc.type: FUNC
 * @tc.desc: Test CreateShaderBuilder returns Normal builder for NORMAL_BORDER_CONTENT
 */
HWTEST_F(GEPointLightShaderTest, CreateShaderBuilder_NormalBorderContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT), 1));
    EXPECT_NE(shader->CreateShaderBuilder(1), nullptr);
}

/**
 * @tc.name: CreateShaderBuilder_FeatheringWithSdf
 * @tc.type: FUNC
 * @tc.desc: Test CreateShaderBuilder returns nullptr for FEATHERING_BORDER with sdf
 */
HWTEST_F(GEPointLightShaderTest, CreateShaderBuilder_FeatheringWithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::FEATHERING_BORDER), 1, true));
    EXPECT_EQ(shader->CreateShaderBuilder(1), nullptr);
}

/**
 * @tc.name: CreateShaderBuilder_FeatheringWithoutSdf
 * @tc.type: FUNC
 * @tc.desc: Test CreateShaderBuilder returns Feathering builder for FEATHERING_BORDER without sdf
 */
HWTEST_F(GEPointLightShaderTest, CreateShaderBuilder_FeatheringWithoutSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::FEATHERING_BORDER), 1, false));
    EXPECT_NE(shader->CreateShaderBuilder(1), nullptr);
}

/**
 * @tc.name: CreateShaderBuilder_Phong
 * @tc.type: FUNC
 * @tc.desc: Test CreateShaderBuilder returns Phong builder for other types
 */
HWTEST_F(GEPointLightShaderTest, CreateShaderBuilder_Phong, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1));
    EXPECT_NE(shader->CreateShaderBuilder(1), nullptr);
}

// ===================== Static Get*ShaderBuilder =====================

/**
 * @tc.name: GetPhongShaderBuilder_SingleAndMulti
 * @tc.type: FUNC
 * @tc.desc: Test GetPhongShaderBuilder returns valid builder for single/multi light
 */
HWTEST_F(GEPointLightShaderTest, GetPhongShaderBuilder_SingleAndMulti, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetPhongShaderBuilder(1), nullptr);
    EXPECT_NE(GEPointLightShader::GetPhongShaderBuilder(2), nullptr);
}

/**
 * @tc.name: GetFeatheringBorderLightShaderBuilder_SingleAndMulti
 * @tc.type: FUNC
 * @tc.desc: Test GetFeatheringBorderLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, GetFeatheringBorderLightShaderBuilder_SingleAndMulti, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetFeatheringBorderLightShaderBuilder(1), nullptr);
    EXPECT_NE(GEPointLightShader::GetFeatheringBorderLightShaderBuilder(2), nullptr);
}

/**
 * @tc.name: GetNormalLightShaderBuilder_SingleAndMulti
 * @tc.type: FUNC
 * @tc.desc: Test GetNormalLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, GetNormalLightShaderBuilder_SingleAndMulti, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetNormalLightShaderBuilder(1), nullptr);
    EXPECT_NE(GEPointLightShader::GetNormalLightShaderBuilder(2), nullptr);
}

/**
 * @tc.name: GetSDFContentLightShaderBuilder
 * @tc.type: FUNC
 * @tc.desc: Test GetSDFContentLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, GetSDFContentLightShaderBuilder, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetSDFContentLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: GetSDFBorderLightShaderBuilder
 * @tc.type: FUNC
 * @tc.desc: Test GetSDFBorderLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, GetSDFBorderLightShaderBuilder, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetSDFBorderLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: GetSDFContentAndBorderLightShaderBuilder
 * @tc.type: FUNC
 * @tc.desc: Test GetSDFContentAndBorderLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, GetSDFContentAndBorderLightShaderBuilder, TestSize.Level1)
{
    EXPECT_NE(GEPointLightShader::GetSDFContentAndBorderLightShaderBuilder(), nullptr);
}

// ===================== MakeFeatheringBoardLightShaderBuilder / MakeNormalLightShaderBuilder =====================

/**
 * @tc.name: MakeFeatheringBoardLightShaderBuilder_Valid
 * @tc.type: FUNC
 * @tc.desc: Test MakeFeatheringBoardLightShaderBuilder returns valid builder
 */
HWTEST_F(GEPointLightShaderTest, MakeFeatheringBoardLightShaderBuilder_Valid, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::FEATHERING_BORDER), 1));
    EXPECT_NE(shader->MakeFeatheringBoardLightShaderBuilder(1), nullptr);
}

/**
 * @tc.name: MakeNormalLightShaderBuilder_G2Curve
 * @tc.type: FUNC
 * @tc.desc: Test MakeNormalLightShaderBuilder with small corner radius uses G2 curvature branch
 */
HWTEST_F(GEPointLightShaderTest, MakeNormalLightShaderBuilder_G2Curve, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT), 1, false, false, TEST_RADIUS_SMALL));
    EXPECT_NE(shader->MakeNormalLightShaderBuilder(1), nullptr);
}

/**
 * @tc.name: MakeNormalLightShaderBuilder_G1Curve
 * @tc.type: FUNC
 * @tc.desc: Test MakeNormalLightShaderBuilder with large corner radius uses G1 curvature branch
 */
HWTEST_F(GEPointLightShaderTest, MakeNormalLightShaderBuilder_G1Curve, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT), 1, false, false, TEST_RADIUS_LARGE));
    EXPECT_NE(shader->MakeNormalLightShaderBuilder(1), nullptr);
}

// ===================== DrawContentLight (multi) =====================

/**
 * @tc.name: DrawContentLight_NormalBorderContentCoefficient
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentLight with NORMAL_BORDER_CONTENT uses 0.5 coefficient
 */
HWTEST_F(GEPointLightShaderTest, DrawContentLight_NormalBorderContentCoefficient, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    shader->DrawContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawContentLight_BlendContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentLight with BLEND_CONTENT uses overlay blend path
 */
HWTEST_F(GEPointLightShaderTest, DrawContentLight_BlendContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BLEND_CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    shader->DrawContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawContentLight_BlendBorderContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentLight with BLEND_BORDER_CONTENT uses overlay blend path
 */
HWTEST_F(GEPointLightShaderTest, DrawContentLight_BlendBorderContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER_CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    shader->DrawContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawContentLight_PlainContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentLight with plain CONTENT uses direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawContentLight_PlainContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    shader->DrawContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawContentLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentLight with sdfShaderEffect routes to DrawSDFContentLight
 */
HWTEST_F(GEPointLightShaderTest, DrawContentLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 2, true));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    shader->DrawContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

// ===================== DrawSingleContentLight (single) =====================

/**
 * @tc.name: DrawSingleContentLight_PlainContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleContentLight with plain CONTENT direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleContentLight_PlainContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Brush brush;
    shader->DrawSingleContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawSingleContentLight_BlendContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleContentLight with BLEND_CONTENT overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleContentLight_BlendContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BLEND_CONTENT), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Brush brush;
    shader->DrawSingleContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawSingleContentLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleContentLight with sdf routes to DrawSDFContentLight
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleContentLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Brush brush;
    shader->DrawSingleContentLight(canvas_, builder, brush, intensityArray);
    SUCCEED();
}

// ===================== DrawSDFContentLight =====================

/**
 * @tc.name: DrawSDFContentLight_NullShaderEffect
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentLight returns false when lightShaderEffect is null
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentLight_NullShaderEffect, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1, true));
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = nullptr;
    Drawing::Brush brush;
    EXPECT_FALSE(shader->DrawSDFContentLight(canvas_, shaderEffect, brush));
}

/**
 * @tc.name: DrawSDFContentLight_BlendContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentLight with BLEND_CONTENT overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentLight_BlendContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BLEND_CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(shaderEffect, nullptr);
    Drawing::Brush brush;
    EXPECT_TRUE(shader->DrawSDFContentLight(canvas_, shaderEffect, brush));
}

/**
 * @tc.name: DrawSDFContentLight_PlainContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentLight with plain CONTENT direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentLight_PlainContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(shaderEffect, nullptr);
    Drawing::Brush brush;
    EXPECT_TRUE(shader->DrawSDFContentLight(canvas_, shaderEffect, brush));
}

// ===================== DrawBorderLight (multi) =====================

/**
 * @tc.name: DrawBorderLight_PlainBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawBorderLight with plain BORDER direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawBorderLight_PlainBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Pen pen;
    shader->DrawBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawBorderLight_BlendBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawBorderLight with BLEND_BORDER overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawBorderLight_BlendBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Pen pen;
    shader->DrawBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawBorderLight_BlendBorderContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawBorderLight with BLEND_BORDER_CONTENT overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawBorderLight_BlendBorderContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER_CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Pen pen;
    shader->DrawBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawBorderLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawBorderLight with sdf routes to DrawSDFBorderLight
 */
HWTEST_F(GEPointLightShaderTest, DrawBorderLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 2, true));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Pen pen;
    shader->DrawBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

// ===================== DrawSingleBorderLight (single) =====================

/**
 * @tc.name: DrawSingleBorderLight_PlainBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleBorderLight with plain BORDER direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleBorderLight_PlainBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Pen pen;
    shader->DrawSingleBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawSingleBorderLight_BlendBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleBorderLight with BLEND_BORDER overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleBorderLight_BlendBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Pen pen;
    shader->DrawSingleBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawSingleBorderLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleBorderLight with sdf routes to DrawSDFBorderLight
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleBorderLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Pen pen;
    shader->DrawSingleBorderLight(canvas_, builder, pen, intensityArray);
    SUCCEED();
}

// ===================== DrawSDFBorderLight =====================

/**
 * @tc.name: DrawSDFBorderLight_NullShaderEffect
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFBorderLight returns false when lightShaderEffect is null
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFBorderLight_NullShaderEffect, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 1, true));
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = nullptr;
    EXPECT_FALSE(shader->DrawSDFBorderLight(canvas_, shaderEffect));
}

/**
 * @tc.name: DrawSDFBorderLight_BlendBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFBorderLight with BLEND_BORDER overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFBorderLight_BlendBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(shaderEffect, nullptr);
    EXPECT_TRUE(shader->DrawSDFBorderLight(canvas_, shaderEffect));
}

/**
 * @tc.name: DrawSDFBorderLight_PlainBorder
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFBorderLight with plain BORDER direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFBorderLight_PlainBorder, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(MakeParams(static_cast<int>(IlluminatedType::BORDER), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(shaderEffect, nullptr);
    EXPECT_TRUE(shader->DrawSDFBorderLight(canvas_, shaderEffect));
}

// ===================== DrawContentAndBorderLight (multi) =====================

/**
 * @tc.name: DrawContentAndBorderLight_WithoutSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentAndBorderLight without sdf routes to Content+Border
 */
HWTEST_F(GEPointLightShaderTest, DrawContentAndBorderLight_WithoutSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BORDER_CONTENT), 2));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    shader->DrawContentAndBorderLight(canvas_, builder, brush, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawContentAndBorderLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawContentAndBorderLight with sdf routes to DrawSDFContentAndBorderLight
 */
HWTEST_F(GEPointLightShaderTest, DrawContentAndBorderLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BORDER_CONTENT), 2, true));
    auto builder = GetPhongBuilder(2);
    ASSERT_NE(builder, nullptr);
    std::array<float, MAX_LIGHT_SOURCES> intensityArray = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        intensityArray[i] = TEST_LIGHT_INTENSITY;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    shader->DrawContentAndBorderLight(canvas_, builder, brush, pen, intensityArray);
    SUCCEED();
}

// ===================== DrawSingleContentAndBorderLight (single) =====================

/**
 * @tc.name: DrawSingleContentAndBorderLight_WithoutSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleContentAndBorderLight without sdf routes to Content+Border
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleContentAndBorderLight_WithoutSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BORDER_CONTENT), 1));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Brush brush;
    Drawing::Pen pen;
    shader->DrawSingleContentAndBorderLight(canvas_, builder, brush, pen, intensityArray);
    SUCCEED();
}

/**
 * @tc.name: DrawSingleContentAndBorderLight_WithSdf
 * @tc.type: FUNC
 * @tc.desc: Test DrawSingleContentAndBorderLight with sdf routes to DrawSDFContentAndBorderLight
 */
HWTEST_F(GEPointLightShaderTest, DrawSingleContentAndBorderLight_WithSdf, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BORDER_CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    std::array<float, 1> intensityArray = { TEST_LIGHT_INTENSITY };
    Drawing::Brush brush;
    Drawing::Pen pen;
    shader->DrawSingleContentAndBorderLight(canvas_, builder, brush, pen, intensityArray);
    SUCCEED();
}

// ===================== DrawSDFContentAndBorderLight =====================

/**
 * @tc.name: DrawSDFContentAndBorderLight_NullInputs
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentAndBorderLight returns early when inputs are null
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentAndBorderLight_NullInputs, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BORDER_CONTENT), 1, true));
    std::shared_ptr<Drawing::ShaderEffect> contentEffect = nullptr;
    std::shared_ptr<Drawing::ShaderEffect> borderEffect = nullptr;
    Drawing::Brush brush;
    shader->DrawSDFContentAndBorderLight(canvas_, contentEffect, borderEffect, brush);
    EXPECT_TRUE(shader->params_.lightSources.size() == 1);
}

/**
 * @tc.name: DrawSDFContentAndBorderLight_BlendBorderContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentAndBorderLight with BLEND_BORDER_CONTENT overlay path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentAndBorderLight_BlendBorderContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::BLEND_BORDER_CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    auto contentBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(*builder);
    std::shared_ptr<Drawing::ShaderEffect> contentEffect = contentBuilder->MakeShader(nullptr, false);
    std::shared_ptr<Drawing::ShaderEffect> borderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(contentEffect, nullptr);
    ASSERT_NE(borderEffect, nullptr);
    Drawing::Brush brush;
    shader->DrawSDFContentAndBorderLight(canvas_, contentEffect, borderEffect, brush);
    SUCCEED();
}

/**
 * @tc.name: DrawSDFContentAndBorderLight_NormalBorderContent
 * @tc.type: FUNC
 * @tc.desc: Test DrawSDFContentAndBorderLight with NORMAL_BORDER_CONTENT direct draw path
 */
HWTEST_F(GEPointLightShaderTest, DrawSDFContentAndBorderLight_NormalBorderContent, TestSize.Level1)
{
    auto shader = std::make_unique<GEPointLightShader>(
        MakeParams(static_cast<int>(IlluminatedType::NORMAL_BORDER_CONTENT), 1, true));
    auto builder = GetPhongBuilder(1);
    ASSERT_NE(builder, nullptr);
    auto contentBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(*builder);
    std::shared_ptr<Drawing::ShaderEffect> contentEffect = contentBuilder->MakeShader(nullptr, false);
    std::shared_ptr<Drawing::ShaderEffect> borderEffect = builder->MakeShader(nullptr, false);
    ASSERT_NE(contentEffect, nullptr);
    ASSERT_NE(borderEffect, nullptr);
    Drawing::Brush brush;
    shader->DrawSDFContentAndBorderLight(canvas_, contentEffect, borderEffect, brush);
    SUCCEED();
}

} // namespace Rosen
} // namespace OHOS