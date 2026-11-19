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
#include <memory>

#include "draw/color.h"
#include "image/bitmap.h"
#include "ge_filter_params.h"
#include "ge_params_reflection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
std::shared_ptr<Drawing::Image> MakeTestImage()
{
    constexpr int32_t bitmapSize = 8;
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(bitmapSize, bitmapSize, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}
} // namespace

class GEStickerParamsReflectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEStickerParamsReflectionTest::SetUpTestCase() {}
void GEStickerParamsReflectionTest::TearDownTestCase() {}
void GEStickerParamsReflectionTest::SetUp() {}
void GEStickerParamsReflectionTest::TearDown() {}

/**
 * @tc.name: BuildStickerParams
 * @tc.desc: Verify the factory builds boxed default params for the sticker emboss and laser types
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, BuildStickerParams, TestSize.Level1)
{
    auto emboss = GEParamsBuilder::Build(GEFilterType::STICKER_EMBOSS_FILTER);
    ASSERT_NE(emboss.get(), nullptr);
    EXPECT_EQ(emboss->GetType(), GEFilterType::STICKER_EMBOSS_FILTER);
    auto embossParams = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*emboss);
    ASSERT_NE(embossParams.get(), nullptr);

    auto laser = GEParamsBuilder::Build(GEFilterType::STICKER_LASER);
    ASSERT_NE(laser.get(), nullptr);
    EXPECT_EQ(laser->GetType(), GEFilterType::STICKER_LASER);
    auto laserParams = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*laser);
    ASSERT_NE(laserParams.get(), nullptr);
}

/**
 * @tc.name: GetFilterTypeFromStringSticker
 * @tc.desc: Verify sticker filter name strings map to their GEFilterType
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, GetFilterTypeFromStringSticker, TestSize.Level1)
{
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("StickerEmbossFilter"), GEFilterType::STICKER_EMBOSS_FILTER);
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("StickerLaser"), GEFilterType::STICKER_LASER);
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("NoSuchStickerFilter"), GEFilterType::NONE);
}

/**
 * @tc.name: GetFilterTypeFromTagSticker
 * @tc.desc: Verify every sticker emboss and laser member tag maps to its owning filter type
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, GetFilterTypeFromTagSticker, TestSize.Level1)
{
    const std::pair<GEParamsMemberTag, GEFilterType> embossTags[] = {
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_STICKER, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_OUTLINE_WIDTH, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_REFRACTION_RATE, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_DIR_INTENSITY, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_WIDTH, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_FEATHER_RATE, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_INTENSITY, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_CENTER, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_INTENSITY, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_POW, GEFilterType::STICKER_EMBOSS_FILTER },
        { GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_INTENSITY, GEFilterType::STICKER_EMBOSS_FILTER },
    };
    for (const auto& entry : embossTags) {
        EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(entry.first), entry.second);
    }

    const std::pair<GEParamsMemberTag, GEFilterType> laserTags[] = {
        { GEParamsMemberTag::STICKER_LASER_STICKER, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_OUTLINE_WIDTH, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_FREQUENCY, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_PROGRESS, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_RADIUS, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_SHADOW_DISTANCE, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_DIRECTION, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_LIGHT_DIR, GEFilterType::STICKER_LASER },
        { GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_COLOR, GEFilterType::STICKER_LASER },
    };
    for (const auto& entry : laserTags) {
        EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(entry.first), entry.second);
    }
}

/**
 * @tc.name: MemberTagFromStringSticker
 * @tc.desc: Verify sticker property name strings parse into their member tags
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, MemberTagFromStringSticker, TestSize.Level1)
{
    const std::pair<std::string, GEParamsMemberTag> entries[] = {
        { "StickerEmboss_Sticker", GEParamsMemberTag::STICKER_EMBOSS_FILTER_STICKER },
        { "StickerEmboss_OutlineWidth", GEParamsMemberTag::STICKER_EMBOSS_FILTER_OUTLINE_WIDTH },
        { "StickerEmboss_RefractionRate", GEParamsMemberTag::STICKER_EMBOSS_FILTER_REFRACTION_RATE },
        { "StickerEmboss_DirIntensity", GEParamsMemberTag::STICKER_EMBOSS_FILTER_DIR_INTENSITY },
        { "StickerEmboss_EdgeLightWidth", GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_WIDTH },
        { "StickerEmboss_EdgeLightFeatherRate", GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_FEATHER_RATE },
        { "StickerEmboss_EdgeLightIntensity", GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_INTENSITY },
        { "StickerEmboss_SpotLightCenter", GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_CENTER },
        { "StickerEmboss_SpotLightIntensity", GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_INTENSITY },
        { "StickerEmboss_SpecPow", GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_POW },
        { "StickerEmboss_SpecIntensity", GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_INTENSITY },
        { "StickerLaserShader_Sticker", GEParamsMemberTag::STICKER_LASER_STICKER },
        { "StickerLaserShader_OutlineWidth", GEParamsMemberTag::STICKER_LASER_OUTLINE_WIDTH },
        { "StickerLaserShader_Frequency", GEParamsMemberTag::STICKER_LASER_FREQUENCY },
        { "StickerLaserShader_Progress", GEParamsMemberTag::STICKER_LASER_PROGRESS },
        { "StickerLaserShader_ElevationShadowRadius", GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_RADIUS },
        { "StickerLaserShader_ElevationShadowDistance", GEParamsMemberTag::STICKER_LASER_SHADOW_DISTANCE },
        { "StickerLaserShader_Direction", GEParamsMemberTag::STICKER_LASER_DIRECTION },
        { "StickerLaserShader_LightDir", GEParamsMemberTag::STICKER_LASER_LIGHT_DIR },
        { "StickerLaserShader_ElevationShadowColor", GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_COLOR },
    };
    for (const auto& entry : entries) {
        EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(entry.first), entry.second);
    }
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString("StickerEmboss_Unknown"),
        GEParamsMemberTag::INVALID);
}

/**
 * @tc.name: SetStickerEmbossFloatParamsByTag
 * @tc.desc: Verify the float setter writes every emboss float member by tag
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerEmbossFloatParamsByTag, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_OUTLINE_WIDTH, 2.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_REFRACTION_RATE, 0.4f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_DIR_INTENSITY, 0.7f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_WIDTH,
        5.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed,
        GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_FEATHER_RATE, 0.6f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_INTENSITY,
        1.2f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_POW, 8.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPEC_INTENSITY,
        0.9f);

    EXPECT_FLOAT_EQ(params->outlineWidth, 2.0f);
    EXPECT_FLOAT_EQ(params->refractionRate, 0.4f);
    EXPECT_FLOAT_EQ(params->dirIntensity, 0.7f);
    EXPECT_FLOAT_EQ(params->edgeLightWidth, 5.0f);
    EXPECT_FLOAT_EQ(params->edgeLightFeatherRate, 0.6f);
    EXPECT_FLOAT_EQ(params->spotLightIntensity, 1.2f);
    EXPECT_FLOAT_EQ(params->specPow, 8.0f);
    EXPECT_FLOAT_EQ(params->specIntensity, 0.9f);
}

/**
 * @tc.name: SetStickerEmbossPairParamsByTag
 * @tc.desc: Verify the pair<float,float> converter writes the emboss Vector2f members
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerEmbossPairParamsByTag, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_INTENSITY,
        std::pair<float, float> { 0.8f, 0.3f });
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_CENTER,
        std::pair<float, float> { 0.25f, 0.75f });

    EXPECT_FLOAT_EQ(params->edgeLightIntensity.x_, 0.8f);
    EXPECT_FLOAT_EQ(params->edgeLightIntensity.y_, 0.3f);
    EXPECT_FLOAT_EQ(params->spotLightCenter.x_, 0.25f);
    EXPECT_FLOAT_EQ(params->spotLightCenter.y_, 0.75f);
}

/**
 * @tc.name: SetStickerLaserFloatParamsByTag
 * @tc.desc: Verify the float setter writes every laser float member by tag
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerLaserFloatParamsByTag, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_OUTLINE_WIDTH, 2.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_FREQUENCY, 0.005f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_PROGRESS, 1.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_RADIUS,
        10.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_SHADOW_DISTANCE, 10.0f);

    EXPECT_FLOAT_EQ(params->outlineWidth, 2.0f);
    EXPECT_FLOAT_EQ(params->frequency, 0.005f);
    EXPECT_FLOAT_EQ(params->progress, 1.0f);
    EXPECT_FLOAT_EQ(params->elevationShadowRadius, 10.0f);
    EXPECT_FLOAT_EQ(params->shadowDistance, 10.0f);
}

/**
 * @tc.name: SetStickerLaserPairParamsByTag
 * @tc.desc: Verify the pair<float,float> converter writes the laser Direction and LightDir members
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerLaserPairParamsByTag, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_DIRECTION,
        std::pair<float, float> { 1.0f, 0.0f });
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_LIGHT_DIR,
        std::pair<float, float> { 0.5f, 0.5f });

    EXPECT_FLOAT_EQ(params->direction.x_, 1.0f);
    EXPECT_FLOAT_EQ(params->direction.y_, 0.0f);
    EXPECT_FLOAT_EQ(params->lightDir.x_, 0.5f);
    EXPECT_FLOAT_EQ(params->lightDir.y_, 0.5f);
}

/**
 * @tc.name: SetStickerLaserVector4fParamsByTag
 * @tc.desc: Verify the Vector4f setter writes the laser elevation shadow color by tag
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerLaserVector4fParamsByTag, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_COLOR,
        Vector4f(0.1f, 0.2f, 0.3f, 0.4f));

    EXPECT_FLOAT_EQ(params->elevationShadowColor.x_, 0.1f);
    EXPECT_FLOAT_EQ(params->elevationShadowColor.y_, 0.2f);
    EXPECT_FLOAT_EQ(params->elevationShadowColor.z_, 0.3f);
    EXPECT_FLOAT_EQ(params->elevationShadowColor.w_, 0.4f);
}

/**
 * @tc.name: SetStickerVector2fDirectParamsByTag
 * @tc.desc: Verify the direct Vector2f setter writes the emboss and laser Vector2f members
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerVector2fDirectParamsByTag, TestSize.Level1)
{
    auto embossBoxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(embossBoxed.get(), nullptr);
    auto emboss = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*embossBoxed);
    ASSERT_NE(emboss.get(), nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*embossBoxed,
        GEParamsMemberTag::STICKER_EMBOSS_FILTER_EDGE_LIGHT_INTENSITY, Vector2f(0.8f, 0.3f));
    GEParamsMemberHelper::SetParamsMemberByTag(*embossBoxed,
        GEParamsMemberTag::STICKER_EMBOSS_FILTER_SPOT_LIGHT_CENTER, Vector2f(0.25f, 0.75f));
    EXPECT_FLOAT_EQ(emboss->edgeLightIntensity.x_, 0.8f);
    EXPECT_FLOAT_EQ(emboss->edgeLightIntensity.y_, 0.3f);
    EXPECT_FLOAT_EQ(emboss->spotLightCenter.x_, 0.25f);
    EXPECT_FLOAT_EQ(emboss->spotLightCenter.y_, 0.75f);

    auto laserBoxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(laserBoxed.get(), nullptr);
    auto laser = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*laserBoxed);
    ASSERT_NE(laser.get(), nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*laserBoxed, GEParamsMemberTag::STICKER_LASER_DIRECTION,
        Vector2f(1.0f, 0.0f));
    GEParamsMemberHelper::SetParamsMemberByTag(*laserBoxed, GEParamsMemberTag::STICKER_LASER_LIGHT_DIR,
        Vector2f(0.5f, 0.5f));
    EXPECT_FLOAT_EQ(laser->direction.x_, 1.0f);
    EXPECT_FLOAT_EQ(laser->direction.y_, 0.0f);
    EXPECT_FLOAT_EQ(laser->lightDir.x_, 0.5f);
    EXPECT_FLOAT_EQ(laser->lightDir.y_, 0.5f);
}

/**
 * @tc.name: SetStickerImageParamsByTag
 * @tc.desc: Verify the shared_ptr<Image> setter writes the sticker member of both effects
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerImageParamsByTag, TestSize.Level1)
{
    auto sticker = MakeTestImage();
    ASSERT_NE(sticker, nullptr);

    auto embossBoxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(embossBoxed.get(), nullptr);
    auto emboss = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*embossBoxed);
    ASSERT_NE(emboss.get(), nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*embossBoxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_STICKER,
        sticker);
    EXPECT_EQ(emboss->sticker, sticker);

    auto laserBoxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(laserBoxed.get(), nullptr);
    auto laser = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*laserBoxed);
    ASSERT_NE(laser.get(), nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*laserBoxed, GEParamsMemberTag::STICKER_LASER_STICKER, sticker);
    EXPECT_EQ(laser->sticker, sticker);
}

/**
 * @tc.name: SetStickerParamsWrongFilterTypeNoChange
 * @tc.desc: Verify laser tags are rejected by emboss params through every setter kind
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerParamsWrongFilterTypeNoChange, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(boxed.get(), nullptr);
    auto params = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*boxed);
    ASSERT_NE(params.get(), nullptr);

    // float setter guard
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_OUTLINE_WIDTH, 2.0f);
    // pair<float,float> setter guard
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_DIRECTION,
        std::pair<float, float> { 1.0f, 0.0f });
    // Vector4f setter guard
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::STICKER_LASER_ELEVATION_SHADOW_COLOR,
        Vector4f(0.1f, 0.2f, 0.3f, 0.4f));

    // Type mismatch keeps every field at its default
    EXPECT_FLOAT_EQ(params->outlineWidth, 0.0f);
    EXPECT_FLOAT_EQ(params->edgeLightIntensity.x_, 0.0f);
    EXPECT_FLOAT_EQ(params->edgeLightIntensity.y_, 0.0f);
    EXPECT_FLOAT_EQ(params->spotLightCenter.x_, 0.5f);
    EXPECT_FLOAT_EQ(params->spotLightCenter.y_, 0.5f);
}

/**
 * @tc.name: SetStickerParamsUnmatchedTagNoChange
 * @tc.desc: Verify a tag without a matching member of the value type keeps the field unchanged
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, SetStickerParamsUnmatchedTagNoChange, TestSize.Level1)
{
    auto laserBoxed = GEFilterParams::Box(std::make_shared<GEStickerLaserShaderParams>());
    ASSERT_NE(laserBoxed.get(), nullptr);
    auto laser = GEFilterParams::Unbox<GEStickerLaserShaderParams>(*laserBoxed);
    ASSERT_NE(laser.get(), nullptr);
    // Float tags passed through the pair/Vector4f setters hit the default branch
    GEParamsMemberHelper::SetParamsMemberByTag(*laserBoxed, GEParamsMemberTag::STICKER_LASER_FREQUENCY,
        std::pair<float, float> { 1.0f, 2.0f });
    GEParamsMemberHelper::SetParamsMemberByTag(*laserBoxed, GEParamsMemberTag::STICKER_LASER_PROGRESS,
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(laser->frequency, 0.0f);
    EXPECT_FLOAT_EQ(laser->progress, 0.0f);

    auto embossBoxed = GEFilterParams::Box(std::make_shared<GEStickerEmbossFilterParams>());
    ASSERT_NE(embossBoxed.get(), nullptr);
    auto emboss = GEFilterParams::Unbox<GEStickerEmbossFilterParams>(*embossBoxed);
    ASSERT_NE(emboss.get(), nullptr);
    GEParamsMemberHelper::SetParamsMemberByTag(*embossBoxed, GEParamsMemberTag::STICKER_EMBOSS_FILTER_OUTLINE_WIDTH,
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(emboss->outlineWidth, 0.0f);
}

/**
 * @tc.name: StickerParamsTypeNames
 * @tc.desc: Verify the sticker filter type name strings are unique and non-empty
 * @tc.type: FUNC
 */
HWTEST_F(GEStickerParamsReflectionTest, StickerParamsTypeNames, TestSize.Level1)
{
    const auto embossName = GEFilterParamsTypeInfo<GEStickerEmbossFilterParams>::FilterName;
    const auto laserName = GEFilterParamsTypeInfo<GEStickerLaserShaderParams>::FilterName;
    EXPECT_FALSE(embossName.empty());
    EXPECT_FALSE(laserName.empty());
    EXPECT_STRNE(embossName.data(), laserName.data());
    EXPECT_STREQ(embossName.data(), "StickerEmbossFilter");
    EXPECT_STREQ(laserName.data(), "StickerLaser");
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
