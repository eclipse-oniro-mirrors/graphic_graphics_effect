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

#include "core/ge_effect_factory.h"
#include "ge_binarization_shader_filter.h"
#include "ge_direct_draw_on_canvas_pass.h"
#include "ge_filter_composer.h"
#include "ge_filter_params.h"
#include "ge_filter_type.h"
#include "ge_params_reflection.h"
#include "ge_shader_filter_params_namesonly.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace Drawing;

class GEParticleAggregationShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<GEVisualEffect> CreateVisualEffect(const std::string& type);
};

void GEParticleAggregationShaderFilterTest::SetUpTestCase() {}
void GEParticleAggregationShaderFilterTest::TearDownTestCase() {}
void GEParticleAggregationShaderFilterTest::SetUp() {}
void GEParticleAggregationShaderFilterTest::TearDown() {}

std::shared_ptr<GEVisualEffect> GEParticleAggregationShaderFilterTest::CreateVisualEffect(const std::string& type)
{
    return std::make_shared<GEVisualEffect>(type, DrawingPaintType::BRUSH);
}

/**
 * @tc.name: Build_001
 * @tc.desc: Verify GEParamsBuilder::Build creates PARTICLE_AGGREGATION params with defaults
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, Build_001, TestSize.Level1)
{
    auto params = GEParamsBuilder::Build(GEFilterType::PARTICLE_AGGREGATION);
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(params->GetType(), GEFilterType::PARTICLE_AGGREGATION);

    auto unboxed = GEFilterParams::Unbox<GEParticleAggregationShaderFilterParams>(*params);
    ASSERT_NE(unboxed, nullptr);
    EXPECT_FLOAT_EQ(unboxed->progress_, 0.f);
    EXPECT_FLOAT_EQ(unboxed->direction_, 0.f);
    EXPECT_FLOAT_EQ(unboxed->maxParticleCount_, 0.f);
    EXPECT_EQ(unboxed->mode_, 0);
}

/**
 * @tc.name: GetFilterTypeFromString_001
 * @tc.desc: Verify GetFilterTypeFromString maps "ParticleAggregation" to PARTICLE_AGGREGATION
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, GetFilterTypeFromString_001, TestSize.Level1)
{
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString(GE_FILTER_PARTICLE_AGGREGATION),
        GEFilterType::PARTICLE_AGGREGATION);
    // unknown name returns NONE
    EXPECT_EQ(GEParamsBuilder::GetFilterTypeFromString("NotAFilter"), GEFilterType::NONE);
}

/**
 * @tc.name: GetFilterTypeFromTag_001
 * @tc.desc: Verify GetFilterTypeFromTag maps each PARTICLE_AGGREGATION member tag
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, GetFilterTypeFromTag_001, TestSize.Level1)
{
    EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS),
        GEFilterType::PARTICLE_AGGREGATION);
    EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(GEParamsMemberTag::PARTICLE_AGGREGATION_DIRECTION),
        GEFilterType::PARTICLE_AGGREGATION);
    EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(GEParamsMemberTag::PARTICLE_AGGREGATION_MAX_PARTICLE_COUNT),
        GEFilterType::PARTICLE_AGGREGATION);
    EXPECT_EQ(GEParamsMemberHelper::GetFilterTypeFromTag(GEParamsMemberTag::PARTICLE_AGGREGATION_MODE),
        GEFilterType::PARTICLE_AGGREGATION);
}

/**
 * @tc.name: GEParamsMemberTagFromString_001
 * @tc.desc: Verify GEParamsMemberTagFromString maps each ParticleAggregation_* string to its tag
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, GEParamsMemberTagFromString_001, TestSize.Level1)
{
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(GE_FILTER_PARTICLE_AGGREGATION_PROGRESS),
        GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS);
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(GE_FILTER_PARTICLE_AGGREGATION_DIRECTION),
        GEParamsMemberTag::PARTICLE_AGGREGATION_DIRECTION);
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(GE_FILTER_PARTICLE_AGGREGATION_MAX_PARTICLE_COUNT),
        GEParamsMemberTag::PARTICLE_AGGREGATION_MAX_PARTICLE_COUNT);
    EXPECT_EQ(GEParamsMemberHelper::GEParamsMemberTagFromString(GE_FILTER_PARTICLE_AGGREGATION_MODE),
        GEParamsMemberTag::PARTICLE_AGGREGATION_MODE);
}

/**
 * @tc.name: SetParamsMemberByTag_001
 * @tc.desc: Verify SetParamsMemberByTag writes float/int32 values for all member tags
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, SetParamsMemberByTag_001, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEParticleAggregationShaderFilterParams>());
    ASSERT_NE(boxed, nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS, 0.5f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_DIRECTION, 1.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(
        *boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_MAX_PARTICLE_COUNT, 5000.0f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_MODE, 1);

    auto unboxed = GEFilterParams::Unbox<GEParticleAggregationShaderFilterParams>(*boxed);
    ASSERT_NE(unboxed, nullptr);
    EXPECT_FLOAT_EQ(unboxed->progress_, 0.5f);
    EXPECT_FLOAT_EQ(unboxed->direction_, 1.0f);
    EXPECT_FLOAT_EQ(unboxed->maxParticleCount_, 5000.0f);
    EXPECT_EQ(unboxed->mode_, 1);
}

/**
 * @tc.name: SetParamsMemberByTag_ProgressClamp_001
 * @tc.desc: Verify progress is clamped to [0.0, 1.0] by min/max constraints
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, SetParamsMemberByTag_ProgressClamp_001, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEParticleAggregationShaderFilterParams>());
    ASSERT_NE(boxed, nullptr);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS, 2.0f);
    EXPECT_FLOAT_EQ(GEFilterParams::Unbox<GEParticleAggregationShaderFilterParams>(*boxed)->progress_, 1.0f);

    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS, -1.0f);
    EXPECT_FLOAT_EQ(GEFilterParams::Unbox<GEParticleAggregationShaderFilterParams>(*boxed)->progress_, 0.0f);
}

/**
 * @tc.name: SetParamsMemberByTag_TypeMismatch_001
 * @tc.desc: Verify SetParamsMemberByTag is a no-op when tag type does not match params type
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, SetParamsMemberByTag_TypeMismatch_001, TestSize.Level1)
{
    auto boxed = GEFilterParams::Box(std::make_shared<GEBinarizationShaderFilterParams>());
    ASSERT_NE(boxed, nullptr);
    // should not crash and should not change anything (returns early on type mismatch)
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS, 0.5f);
    GEParamsMemberHelper::SetParamsMemberByTag(*boxed, GEParamsMemberTag::PARTICLE_AGGREGATION_MODE, 1);
    EXPECT_EQ(boxed->GetType(), GEFilterType::BINARIZATION);
}

/**
 * @tc.name: FactoryRegister_001
 * @tc.desc: Verify PARTICLE_AGGREGATION is registered in GEEffectFactory via GE_FACTORY_REGISTER_EXTERNAL
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, FactoryRegister_001, TestSize.Level1)
{
    using GraphicsEffectEngine::GEEffectFactory;
    size_t idx = static_cast<size_t>(GEFilterType::PARTICLE_AGGREGATION);
    ASSERT_LT(idx, GEEffectFactory::creators_.size());
    EXPECT_TRUE(GEEffectFactory::creators_[idx].has_value())
        << "PARTICLE_AGGREGATION creator should be registered";
}

/**
 * @tc.name: DirectDrawOnCanvasPass_Last_001
 * @tc.desc: Verify PARTICLE_AGGREGATION enables direct draw when it is the last composable
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, DirectDrawOnCanvasPass_Last_001, TestSize.Level1)
{
    GEDirectDrawOnCanvasPass pass;
    std::vector<GEFilterComposable> composables {
        CreateVisualEffect(GE_FILTER_GREY),
        CreateVisualEffect(GE_FILTER_PARTICLE_AGGREGATION)
    };
    auto result = pass.Run(composables);
    EXPECT_TRUE(result.changed);
    ASSERT_EQ(composables.size(), 2); // 2: original size
    EXPECT_FALSE(DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(composables[0]));
    EXPECT_TRUE(DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(composables[1]));
}

/**
 * @tc.name: DirectDrawOnCanvasPass_NotLast_001
 * @tc.desc: Verify PARTICLE_AGGREGATION does not enable direct draw when not the last composable
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, DirectDrawOnCanvasPass_NotLast_001, TestSize.Level1)
{
    GEDirectDrawOnCanvasPass pass;
    std::vector<GEFilterComposable> composables {
        CreateVisualEffect(GE_FILTER_PARTICLE_AGGREGATION),
        CreateVisualEffect(GE_FILTER_GREY)
    };
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);
    EXPECT_FALSE(DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(composables[0]));
    EXPECT_FALSE(DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(composables[1]));
}

/**
 * @tc.name: VisualEffectImpl_Full_001
 * @tc.desc: Verify GEVisualEffectImpl full flow: construct, GetFilterType, SetParam, Make/Get params
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, VisualEffectImpl_Full_001, TestSize.Level1)
{
    // construction triggers GetFilterTypeFromString + Build
    GEVisualEffectImpl impl(GE_FILTER_PARTICLE_AGGREGATION);
    EXPECT_EQ(impl.GetFilterType(), GEFilterType::PARTICLE_AGGREGATION);
    ASSERT_NE(impl.GetParams<GEParticleAggregationShaderFilterParams>(), nullptr);

    // SetParam by string tag triggers GEParamsMemberTagFromString + SetParamsMemberByTag
    impl.SetParam(GE_FILTER_PARTICLE_AGGREGATION_PROGRESS, 0.5f);
    impl.SetParam(GE_FILTER_PARTICLE_AGGREGATION_DIRECTION, 1.0f);
    impl.SetParam(GE_FILTER_PARTICLE_AGGREGATION_MAX_PARTICLE_COUNT, 5000.0f);
    impl.SetParam(GE_FILTER_PARTICLE_AGGREGATION_MODE, 1);
    auto params = impl.GetParticleAggregationShaderFilterParams();
    ASSERT_NE(params, nullptr);
    EXPECT_FLOAT_EQ(params->progress_, 0.5f);
    EXPECT_FLOAT_EQ(params->direction_, 1.0f);
    EXPECT_FLOAT_EQ(params->maxParticleCount_, 5000.0f);
    EXPECT_EQ(params->mode_, 1);

    // SetParam by enum tag
    impl.SetParam(GEParamsMemberTag::PARTICLE_AGGREGATION_PROGRESS, 0.3f);
    EXPECT_FLOAT_EQ(impl.GetParticleAggregationShaderFilterParams()->progress_, 0.3f);

    // Make method generated by GE_DECLARE_MAKE_GET_PARAMS
    impl.MakeParticleAggregationShaderFilterParams();
    EXPECT_NE(impl.GetParticleAggregationShaderFilterParams(), nullptr);
}

/**
 * @tc.name: VisualEffectImpl_Defaults_001
 * @tc.desc: Verify default param values after construction
 * @tc.type: FUNC
 */
HWTEST_F(GEParticleAggregationShaderFilterTest, VisualEffectImpl_Defaults_001, TestSize.Level1)
{
    GEVisualEffectImpl impl(GE_FILTER_PARTICLE_AGGREGATION);
    auto params = impl.GetParticleAggregationShaderFilterParams();
    ASSERT_NE(params, nullptr);
    EXPECT_FLOAT_EQ(params->progress_, 0.f);
    EXPECT_FLOAT_EQ(params->direction_, 0.f);
    EXPECT_FLOAT_EQ(params->maxParticleCount_, 0.f);
    EXPECT_EQ(params->mode_, 0);
}
} // namespace Rosen
} // namespace OHOS
