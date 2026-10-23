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

#include "ge_filter_params.h"
#include "ge_params_reflection.h"
#include "ge_binarization_shader_filter.h"
#include "ge_light_up_effect_shader_filter.h"
#include "ge_dynamic_dim_shader_filter.h"

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

/**
 * @tc.name: BinarizationParamsBoxed
 * @tc.desc: Verify Binarization params can be boxed and unboxed
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, BinarizationParamsBoxed, TestSize.Level0)
{
    auto params = std::make_shared<GEBinarizationShaderFilterParams>();
    params->low = 0.1f;
    params->high = 0.9f;
    params->threshold = 0.5f;
    params->thresholdRange = 0.1f;
    auto boxed = GEFilterParams::Box(params);
    ASSERT_NE(boxed.get(), nullptr);
    auto unboxed = GEFilterParams::Unbox<GEBinarizationShaderFilterParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_FLOAT_EQ(unboxed->low, 0.1f);
    EXPECT_FLOAT_EQ(unboxed->high, 0.9f);
    EXPECT_FLOAT_EQ(unboxed->threshold, 0.5f);
    EXPECT_FLOAT_EQ(unboxed->thresholdRange, 0.1f);
}

/**
 * @tc.name: LightUpEffectParamsBoxed
 * @tc.desc: Verify LightUpEffect params can be boxed and unboxed
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, LightUpEffectParamsBoxed, TestSize.Level0)
{
    auto params = std::make_shared<GELightUpEffectShaderFilterParams>();
    params->value = 0.7f;
    auto boxed = GEFilterParams::Box(params);
    ASSERT_NE(boxed.get(), nullptr);
    auto unboxed = GEFilterParams::Unbox<GELightUpEffectShaderFilterParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_FLOAT_EQ(unboxed->value, 0.7f);
}

/**
 * @tc.name: DynamicDimParamsBoxed
 * @tc.desc: Verify DynamicDim params can be boxed and unboxed
 * @tc.type: FUNC
 */
HWTEST_F(GEParamsReflectionTest, DynamicDimParamsBoxed, TestSize.Level0)
{
    auto params = std::make_shared<GEDynamicDimShaderFilterParams>();
    params->value = 0.4f;
    auto boxed = GEFilterParams::Box(params);
    ASSERT_NE(boxed.get(), nullptr);
    auto unboxed = GEFilterParams::Unbox<GEDynamicDimShaderFilterParams>(*boxed);
    ASSERT_NE(unboxed.get(), nullptr);
    EXPECT_FLOAT_EQ(unboxed->value, 0.4f);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
