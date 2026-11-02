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

#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "ge_filter_params.h"
#include "ge_params_reflection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {

using CosmosFieldValue = std::variant<float, int32_t, bool, Vector2f, Vector3f, Vector4f>;

struct CosmosField {
    GEParamsMemberTag tag;
    const char* name;
    CosmosFieldValue value;
};

const std::vector<CosmosField>& AllCosmosFields()
{
    static const std::vector<CosmosField> fields = {
        { GEParamsMemberTag::COSMOS_RING_PROGRESS, "CosmosRing_Progress", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_RADIUS, "CosmosRing_Radius", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_WIDTH, "CosmosRing_Width", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_FEATHER, "CosmosRing_Feather", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_NOISE_DENSITY, "CosmosRing_NoiseDensity", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_NOISE_PERIOD, "CosmosRing_NoisePeriod", 2 },
        { GEParamsMemberTag::COSMOS_RING_NOISE_CONTRAST, "CosmosRing_NoiseContrast", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_NOISE2_DENSITY, "CosmosRing_Noise2Density", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_NOISE2_PERIOD, "CosmosRing_Noise2Period", 2 },
        { GEParamsMemberTag::COSMOS_RING_NOISE2_CONTRAST, "CosmosRing_Noise2Contrast", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_NOISE_SEED, "CosmosRing_NoiseSeed", 2 },
        { GEParamsMemberTag::COSMOS_RING_CHROMA_STRENGTH, "CosmosRing_ChromaStrength", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_DARK_COLOR, "CosmosRing_DarkColor", Vector4f(0.1f, 0.2f, 0.3f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_MID_COLOR, "CosmosRing_MidColor", Vector4f(0.1f, 0.2f, 0.3f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_BRIGHT_COLOR, "CosmosRing_BrightColor", Vector4f(0.1f, 0.2f, 0.3f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_DARK_POSITION, "CosmosRing_DarkPosition", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_MID_POSITION, "CosmosRing_MidPosition", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_BRIGHT_POSITION, "CosmosRing_BrightPosition", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_CHROMA_ROTATION, "CosmosRing_ChromaRotation", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_CHROMA_BRIGHTNESS_START, "CosmosRing_ChromaBrightnessStart", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_ENABLE, "CosmosRing_HlEnable", true },
        { GEParamsMemberTag::COSMOS_RING_HL_OUTER_RADIUS, "CosmosRing_HlOuterRadius", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_INNER_RADIUS, "CosmosRing_HlInnerRadius", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_OUTER_WIDTH, "CosmosRing_HlOuterWidth", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_INNER_WIDTH, "CosmosRing_HlInnerWidth", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_DISPERSION_STRENGTH, "CosmosRing_HlDispersionStrength", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_OUTER_CENTER_ANGLE, "CosmosRing_HlP1OuterCenterAngle", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_OUTER_ARC_SPAN, "CosmosRing_HlP1OuterArcSpan", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_OUTER_BRIGHTNESS, "CosmosRing_HlP1OuterBrightness", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_OUTER_DISP_SCALES, "CosmosRing_HlP1OuterDispScales",
            Vector3f(0.1f, 0.2f, 0.3f) },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_CENTER_ANGLE, "CosmosRing_HlP1InnerCenterAngle", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_ARC_SPAN, "CosmosRing_HlP1InnerArcSpan", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_BRIGHTNESS, "CosmosRing_HlP1InnerBrightness", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_DISP_SCALES, "CosmosRing_HlP1InnerDispScales",
            Vector3f(0.1f, 0.2f, 0.3f) },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_OUTER_CENTER_ANGLE, "CosmosRing_HlP2OuterCenterAngle", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_OUTER_ARC_SPAN, "CosmosRing_HlP2OuterArcSpan", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_OUTER_BRIGHTNESS, "CosmosRing_HlP2OuterBrightness", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_OUTER_DISP_SCALES, "CosmosRing_HlP2OuterDispScales",
            Vector3f(0.1f, 0.2f, 0.3f) },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_INNER_CENTER_ANGLE, "CosmosRing_HlP2InnerCenterAngle", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_INNER_ARC_SPAN, "CosmosRing_HlP2InnerArcSpan", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_INNER_BRIGHTNESS, "CosmosRing_HlP2InnerBrightness", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_P2_INNER_DISP_SCALES, "CosmosRing_HlP2InnerDispScales",
            Vector3f(0.1f, 0.2f, 0.3f) },
        { GEParamsMemberTag::COSMOS_RING_GLOW_STRENGTH, "CosmosRing_GlowStrength", Vector2f(0.2f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_GLOW_RADIUS, "CosmosRing_GlowRadius", Vector2f(0.2f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_GLOW_THRESHOLD, "CosmosRing_GlowThreshold", Vector2f(0.2f, 0.4f) },
        { GEParamsMemberTag::COSMOS_RING_GLOW_INTENSITY, "CosmosRing_GlowIntensity", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_GLOW_SATURATION, "CosmosRing_GlowSaturation", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_GLOW_EXPOSURE, "CosmosRing_GlowExposure", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_RADIAL_FEATHER_PX, "CosmosRing_HlRadialFeatherPx", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_TANGENT_FEATHER_DEG, "CosmosRing_HlTangentFeatherDeg", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_SPINDLE_STRENGTH, "CosmosRing_HlSpindleStrength", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_SPINDLE_POWER, "CosmosRing_HlSpindlePower", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_ANGULAR_FADE, "CosmosRing_HlAngularFade", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_WHOLE_BRIGHTNESS, "CosmosRing_HlWholeBrightness", 0.5f },
        { GEParamsMemberTag::COSMOS_RING_HL_ROTATION_SPEED, "CosmosRing_HlRotationSpeed", 0.5f },
    };
    return fields;
}

std::unique_ptr<GEFilterParams> BuildCosmosRingParams()
{
    return GEParamsBuilder::Build(GEFilterType::COSMOS_RING);
}

std::shared_ptr<GEXCosmosRingEffectParams> UnboxCosmosRingParams(const GEFilterParams& params)
{
    return GEFilterParams::Unbox<GEXCosmosRingEffectParams>(params);
}

void SetFieldValue(GEFilterParams& params, GEParamsMemberTag tag, const CosmosFieldValue& value)
{
    std::visit(
        [&params, tag](const auto& v) {
            GEParamsMemberHelper::SetParamsMemberByTag(params, tag, v);
        },
        value);
}

} // namespace

class GECosmosRingParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GECosmosRingParamsTest::SetUpTestCase() {}
void GECosmosRingParamsTest::TearDownTestCase() {}
void GECosmosRingParamsTest::SetUp() {}
void GECosmosRingParamsTest::TearDown() {}

/**
 * @tc.name: ParamsTypeInfoAndBuilder
 * @tc.desc: Verify CosmosRing type info, builder and string lookup are wired
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, ParamsTypeInfoAndBuilder, TestSize.Level1)
{
    EXPECT_EQ(GEFilterParamsTypeInfo<GEXCosmosRingEffectParams>::ID, GEFilterType::COSMOS_RING);
    EXPECT_EQ(GEFilterParamsTypeInfo<GEXCosmosRingEffectParams>::FilterName, std::string_view("CosmosRing"));

    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);
    EXPECT_EQ(boxed->GetType(), GEFilterType::COSMOS_RING);

    auto params = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(GEFilterParams::Unbox<GEXGlassEffectParams>(*boxed), nullptr);

    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("CosmosRing"), GEFilterType::COSMOS_RING);
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("NotAnEffect"), GEFilterType::NONE);
}

/**
 * @tc.name: TagAndStringMappingCoversAllFields
 * @tc.desc: Verify every CosmosRing tag maps to COSMOS_RING and to its property name
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, TagAndStringMappingCoversAllFields, TestSize.Level1)
{
    for (const auto& field : AllCosmosFields()) {
        EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(field.tag), GEFilterType::COSMOS_RING);
        EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(field.name), field.tag);
    }
    EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(GEParamsMemberTag::INVALID), GEFilterType::NONE);
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString("NotAProperty"), GEParamsMemberTag::INVALID);
}

/**
 * @tc.name: SetAllFieldsCoversEveryTag
 * @tc.desc: Verify all CosmosRing tags can be set through the reflection setters
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, SetAllFieldsCoversEveryTag, TestSize.Level1)
{
    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);

    for (const auto& field : AllCosmosFields()) {
        SetFieldValue(*boxed, field.tag, field.value);
    }

    EXPECT_EQ(boxed->GetType(), GEFilterType::COSMOS_RING);
    auto params = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(params, nullptr);
}

/**
 * @tc.name: ScalarConstraintsClampOutOfRange
 * @tc.desc: Verify float and int range constraints clamp instead of rejecting values
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, ScalarConstraintsClampOutOfRange, TestSize.Level1)
{
    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);
    auto& params = *boxed;

    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_PROGRESS, -1.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_RADIUS, 5000.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_WIDTH, 2000.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_FEATHER, 1000.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_NOISE2_DENSITY, 0.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_NOISE2_PERIOD, 0);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_NOISE_SEED, 999999);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_GLOW_EXPOSURE, -2.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(
        params, GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_CENTER_ANGLE, -720.0f);

    auto cosmos = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(cosmos, nullptr);
    EXPECT_FLOAT_EQ(cosmos->progress, 0.0f);
    EXPECT_FLOAT_EQ(cosmos->radius, 4096.0f);
    EXPECT_FLOAT_EQ(cosmos->width, 1024.0f);
    EXPECT_FLOAT_EQ(cosmos->feather, 256.0f);
    EXPECT_FLOAT_EQ(cosmos->noise2Density, 1.0f);
    EXPECT_EQ(cosmos->noise2Period, 1);
    EXPECT_EQ(cosmos->noiseSeed, 100000);
    EXPECT_FLOAT_EQ(cosmos->glowExposure, 0.0f);
    EXPECT_FLOAT_EQ(cosmos->hlP1InnerCenterAngle, -360.0f);

    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_PROGRESS, 2.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_NOISE_PERIOD, 100);
    GEParamsMemberHelper::SetParamsMemberByTag(
        params, GEParamsMemberTag::COSMOS_RING_HL_P1_INNER_CENTER_ANGLE, 720.0f);
    EXPECT_FLOAT_EQ(cosmos->progress, 1.0f);
    EXPECT_EQ(cosmos->noisePeriod, 16);
    EXPECT_FLOAT_EQ(cosmos->hlP1InnerCenterAngle, 360.0f);
}

/**
 * @tc.name: VectorConstraintsAndPairConversion
 * @tc.desc: Verify Vector2f component clamping and std::pair to Vector2f conversion
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, VectorConstraintsAndPairConversion, TestSize.Level1)
{
    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);
    auto& params = *boxed;

    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_GLOW_STRENGTH,
        std::pair<float, float>(-1.0f, 2.0f));
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_GLOW_RADIUS,
        Vector2f(-5.0f, 600.0f));
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_GLOW_THRESHOLD,
        std::pair<float, float>(-2.0f, 2.0f));

    auto cosmos = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(cosmos, nullptr);
    EXPECT_FLOAT_EQ(cosmos->glowStrength.x_, 0.0f);
    EXPECT_FLOAT_EQ(cosmos->glowStrength.y_, 1.0f);
    EXPECT_FLOAT_EQ(cosmos->glowRadius.x_, 0.0f);
    EXPECT_FLOAT_EQ(cosmos->glowRadius.y_, 512.0f);
    EXPECT_FLOAT_EQ(cosmos->glowThreshold.x_, -1.0f);
    EXPECT_FLOAT_EQ(cosmos->glowThreshold.y_, 1.0f);
}

/**
 * @tc.name: VectorAndBoolRoundTrip
 * @tc.desc: Verify Vector3f, Vector4f and bool members store the given values
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, VectorAndBoolRoundTrip, TestSize.Level1)
{
    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);
    auto& params = *boxed;

    const Vector3f scales(0.8f, 0.0f, 1.4f);
    const Vector4f color(0.1f, 0.2f, 0.3f, 0.4f);
    GEParamsMemberHelper::SetParamsMemberByTag(
        params, GEParamsMemberTag::COSMOS_RING_HL_P2_OUTER_DISP_SCALES, scales);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_MID_COLOR, color);
    GEParamsMemberHelper::SetParamsMemberByTag(params, GEParamsMemberTag::COSMOS_RING_HL_ENABLE, false);

    auto cosmos = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(cosmos, nullptr);
    EXPECT_FLOAT_EQ(cosmos->hlP2OuterDispScales.x_, scales.x_);
    EXPECT_FLOAT_EQ(cosmos->hlP2OuterDispScales.y_, scales.y_);
    EXPECT_FLOAT_EQ(cosmos->hlP2OuterDispScales.z_, scales.z_);
    EXPECT_EQ(cosmos->midColor, color);
    EXPECT_FALSE(cosmos->hlEnable);
}

/**
 * @tc.name: WrongFilterTypeIsIgnored
 * @tc.desc: Verify setters reject tags that do not belong to the boxed params type
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, WrongFilterTypeIsIgnored, TestSize.Level1)
{
    auto glass = GEParamsBuilder::Build(GEFilterType::GLASS_EFFECT);
    ASSERT_NE(glass, nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*glass, GEParamsMemberTag::COSMOS_RING_PROGRESS, 0.5f);
    EXPECT_EQ(glass->GetType(), GEFilterType::GLASS_EFFECT);
    EXPECT_EQ(GEFilterParams::Unbox<GEXCosmosRingEffectParams>(*glass), nullptr);

    auto cosmosBoxed = BuildCosmosRingParams();
    ASSERT_NE(cosmosBoxed, nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*cosmosBoxed, GEParamsMemberTag::GLASS_EFFECT_OPACITY, 0.5f);
    auto cosmos = UnboxCosmosRingParams(*cosmosBoxed);
    ASSERT_NE(cosmos, nullptr);
    EXPECT_FLOAT_EQ(cosmos->progress, 0.0f);
}

/**
 * @tc.name: DefaultValues
 * @tc.desc: Verify representative CosmosRing defaults
 * @tc.type: FUNC
 */
HWTEST_F(GECosmosRingParamsTest, DefaultValues, TestSize.Level1)
{
    auto boxed = BuildCosmosRingParams();
    ASSERT_NE(boxed, nullptr);
    auto cosmos = UnboxCosmosRingParams(*boxed);
    ASSERT_NE(cosmos, nullptr);
    EXPECT_FLOAT_EQ(cosmos->progress, 0.0f);
    EXPECT_FLOAT_EQ(cosmos->radius, 150.0f);
    EXPECT_FLOAT_EQ(cosmos->width, 40.0f);
    EXPECT_FLOAT_EQ(cosmos->feather, 5.0f);
    EXPECT_EQ(cosmos->noisePeriod, 16);
    EXPECT_EQ(cosmos->noiseSeed, 0);
    EXPECT_TRUE(cosmos->hlEnable);
    EXPECT_FLOAT_EQ(cosmos->glowStrength.x_, 0.45f);
    EXPECT_FLOAT_EQ(cosmos->glowStrength.y_, 0.25f);
    EXPECT_EQ(cosmos->midColor, Vector4f(0.1922f, 0.3255f, 0.5961f, 1.0f));
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
