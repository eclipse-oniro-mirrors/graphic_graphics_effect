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

#include "ge_effects_params.h"
#include "ge_params_reflection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
template <typename ParamsT>
std::unique_ptr<GEFilterParams> MakeBoxedParams()
{
    return GEFilterParams::Box(std::make_shared<ParamsT>());
}

void SetBlendMode(GEFilterParams& params, GEParamsMemberTag tag, MaterialColorBlendMode mode)
{
    GEParamsMemberHelper::SetParamsMemberByTag(params, tag, mode);
}
} // namespace

class GEParamsReflectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEParamsReflectionTest::SetUpTestCase() {}
void GEParamsReflectionTest::TearDownTestCase() {}
void GEParamsReflectionTest::SetUp() {}
void GEParamsReflectionTest::TearDown() {}

/**
 * @tc.name: MaterialColorBlendMode_FrostedEffectTag_SetsValue
 * @tc.desc: Verify FROSTED_GLASS_EFFECT_COLOR_BLEND_MODE writes TINTED_GLASS into params
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, MaterialColorBlendMode_FrostedEffectTag_SetsValue, TestSize.Level0)
{
    auto boxed = MakeBoxedParams<GEFrostedGlassEffectParams>();
    ASSERT_NE(boxed.get(), nullptr);
    SetBlendMode(*boxed, GEParamsMemberTag::FROSTED_GLASS_EFFECT_COLOR_BLEND_MODE,
        MaterialColorBlendMode::TINTED_GLASS);
    auto unboxed = GEFilterParams::Unbox<GEFrostedGlassEffectParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_EQ(unboxed->colorBlendMode, MaterialColorBlendMode::TINTED_GLASS);
}

/**
 * @tc.name: MaterialColorBlendMode_FrostedShaderTag_OverwritesValue
 * @tc.desc: Verify FROSTED_GLASS_COLOR_BLEND_MODE overwrites existing value to LINEAR_MIX
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, MaterialColorBlendMode_FrostedShaderTag_OverwritesValue, TestSize.Level0)
{
    auto boxed = MakeBoxedParams<GEFrostedGlassShaderFilterParams>();
    ASSERT_NE(boxed.get(), nullptr);
    SetBlendMode(*boxed, GEParamsMemberTag::FROSTED_GLASS_COLOR_BLEND_MODE,
        MaterialColorBlendMode::TINTED_GLASS);
    SetBlendMode(*boxed, GEParamsMemberTag::FROSTED_GLASS_COLOR_BLEND_MODE,
        MaterialColorBlendMode::LINEAR_MIX);
    auto unboxed = GEFilterParams::Unbox<GEFrostedGlassShaderFilterParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_EQ(unboxed->colorBlendMode, MaterialColorBlendMode::LINEAR_MIX);
}

/**
 * @tc.name: MaterialColorBlendMode_SpatialGlassTag_SetsValue
 * @tc.desc: Verify SPATIAL_GLASS_EFFECT_COLOR_BLEND_MODE writes TINTED_GLASS into params
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, MaterialColorBlendMode_SpatialGlassTag_SetsValue, TestSize.Level0)
{
    auto boxed = MakeBoxedParams<GESpatialGlassEffectParams>();
    ASSERT_NE(boxed.get(), nullptr);
    SetBlendMode(*boxed, GEParamsMemberTag::SPATIAL_GLASS_EFFECT_COLOR_BLEND_MODE,
        MaterialColorBlendMode::TINTED_GLASS);
    auto unboxed = GEFilterParams::Unbox<GESpatialGlassEffectParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_EQ(unboxed->colorBlendMode, MaterialColorBlendMode::TINTED_GLASS);
}

/**
 * @tc.name: MaterialColorBlendMode_FilterTypeMismatch_NoChange
 * @tc.desc: Verify mismatched filter type returns early leaving colorBlendMode at default
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, MaterialColorBlendMode_FilterTypeMismatch_NoChange, TestSize.Level0)
{
    auto boxed = MakeBoxedParams<GEFrostedGlassEffectParams>();
    ASSERT_NE(boxed.get(), nullptr);
    SetBlendMode(*boxed, GEParamsMemberTag::SPATIAL_GLASS_EFFECT_COLOR_BLEND_MODE,
        MaterialColorBlendMode::TINTED_GLASS);
    auto unboxed = GEFilterParams::Unbox<GEFrostedGlassEffectParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_EQ(unboxed->colorBlendMode, MaterialColorBlendMode::LINEAR_MIX);
}

/**
 * @tc.name: MaterialColorBlendMode_UnmatchedTag_DefaultBranchNoChange
 * @tc.desc: Verify same-type non-matching tag hits default branch without changing colorBlendMode
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, MaterialColorBlendMode_UnmatchedTag_DefaultBranchNoChange, TestSize.Level0)
{
    auto boxed = MakeBoxedParams<GEFrostedGlassEffectParams>();
    ASSERT_NE(boxed.get(), nullptr);
    SetBlendMode(*boxed, GEParamsMemberTag::FROSTED_GLASS_EFFECT_COLOR_BLEND_MODE,
        MaterialColorBlendMode::TINTED_GLASS);
    SetBlendMode(*boxed, GEParamsMemberTag::FROSTED_GLASS_EFFECT_MATERIAL_COLOR,
        MaterialColorBlendMode::LINEAR_MIX);
    auto unboxed = GEFilterParams::Unbox<GEFrostedGlassEffectParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_EQ(unboxed->colorBlendMode, MaterialColorBlendMode::TINTED_GLASS);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
