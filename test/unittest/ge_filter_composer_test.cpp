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
#include <memory>
#include <vector>

#include "ge_filter_type.h"
#include "ge_filter_composer_pass.h"
#include "ge_hps_build_pass.h"
#include "ge_mesa_fusion_pass.h"
#include "ge_hps_compatible_pass.h"
#include "ge_visual_effect_impl.h"
#include "ge_render.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

using Drawing::GEFilterType;

class GEFilterComposerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper methods
    std::shared_ptr<Drawing::GEVisualEffect> CreateVisualEffect(std::string type);
    std::shared_ptr<Drawing::GEVisualEffect> CreateGreyEffect();
    std::shared_ptr<Drawing::GEVisualEffect> CreateKawaseBlurEffect();
    std::shared_ptr<Drawing::GEVisualEffect> CreateMesaBlurEffect();
    
    static inline Drawing::Canvas drawingCanvas_;
};

std::shared_ptr<Drawing::GEVisualEffect> GEFilterComposerTest::CreateVisualEffect(
    std::string type)
{
    auto effect = std::make_shared<Drawing::GEVisualEffect>(type, Drawing::DrawingPaintType::BRUSH);
    return effect;
}

std::shared_ptr<Drawing::GEVisualEffect> GEFilterComposerTest::CreateGreyEffect()
{
    auto effect = CreateVisualEffect(Drawing::GE_FILTER_GREY);
    Drawing::GEGreyShaderFilterParams params { 0.5f, 0.5f };
    effect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, params.greyCoef1);
    effect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, params.greyCoef2);
    return effect;
}

std::shared_ptr<Drawing::GEVisualEffect> GEFilterComposerTest::CreateKawaseBlurEffect()
{
    auto effect = CreateVisualEffect(Drawing::GE_FILTER_KAWASE_BLUR);
    Drawing::GEKawaseBlurShaderFilterParams params { 2 };
    effect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, params.radius);
    return effect;
}

std::shared_ptr<Drawing::GEVisualEffect> GEFilterComposerTest::CreateMesaBlurEffect()
{
    auto effect = CreateVisualEffect(Drawing::GE_FILTER_GREY);
    Drawing::GEMESABlurShaderFilterParams params { 2, 0.5f, 0.5f };
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, params.radius);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, params.greyCoef1);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, params.greyCoef2);
    return effect;
}

/**
 * @tc.name: GetEffect
 * @tc.desc: Test GEFilterComposable GetEffect function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GetEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GetEffect start";
    
    auto effect = CreateGreyEffect();
    GEFilterComposable composable(effect);
    
    auto retrievedEffect = composable.GetEffect();
    EXPECT_NE(retrievedEffect, nullptr);
    EXPECT_EQ(retrievedEffect, effect);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest GetEffect end";
}

/**
 * @tc.name: GetHpsEffect
 * @tc.desc: Test GEFilterComposable GetHpsEffect function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GetHpsEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GetHpsEffect start";
    
    // Create a HpsEffectFilter (this would normally be created by GEHpsBuildPass)
    auto hpsEffect = std::make_shared<HpsEffectFilter>(drawingCanvas_);
    GEFilterComposable composable(hpsEffect);
    
    auto retrievedHpsEffect = composable.GetHpsEffect();
    EXPECT_NE(retrievedHpsEffect, nullptr);
    EXPECT_EQ(retrievedHpsEffect, hpsEffect);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest GetHpsEffect end";
}

/**
 * @tc.name: HpsBuildPassGetLogName
 * @tc.desc: Test GEHpsBuildPass GetLogName function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsBuildPassGetLogName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassGetLogName start";
    
    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = nullptr; // Not used in this test
    GEHpsBuildPass pass(drawingCanvas_, context);
    
    auto name = pass.GetLogName();
    EXPECT_EQ(name, "GEHpsBuildPass");
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassGetLogName end";
}

/**
 * @tc.name: HpsBuildPassRunNoEffects
 * @tc.desc: Test GEHpsBuildPass Run function with no effects
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsBuildPassRunNoEffects, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassRunNoEffects start";
    
    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = nullptr; // Not used in this test
    GEHpsBuildPass pass(drawingCanvas_, context);
    
    std::vector<GEFilterComposable> composables;
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassRunNoEffects end";
}

/**
 * @tc.name: MesaFusionPassGetLogName
 * @tc.desc: Test GEMesaFusionPass GetLogName function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, MesaFusionPassGetLogName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassGetLogName start";
    
    GEMesaFusionPass pass;
    
    auto name = pass.GetLogName();
    EXPECT_EQ(name, "GEMesaFusionPass");
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassGetLogName end";
}

/**
 * @tc.name: MesaFusionPassRunNoFusion
 * @tc.desc: Test GEMesaFusionPass Run function with no fusion opportunity
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, MesaFusionPassRunNoFusion, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunNoFusion start";
    
    GEMesaFusionPass pass;
    
    std::vector<GEFilterComposable> composables;
    auto effect = CreateGreyEffect();
    composables.push_back(effect);
    
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunNoFusion end";
}

/**
 * @tc.name: MesaFusionPassRunWithFusion
 * @tc.desc: Test GEMesaFusionPass Run function with fusion opportunity
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, MesaFusionPassRunWithFusion, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunWithFusion start";
    
    GEMesaFusionPass pass;
    
    std::vector<GEFilterComposable> composables;
    // Add a grey effect followed by a kawase blur (fusion opportunity)
    auto greyEffect = CreateGreyEffect();
    auto kawaseEffect = CreateKawaseBlurEffect();
    composables.push_back(greyEffect);
    composables.push_back(kawaseEffect);
    
    auto result = pass.Run(composables);
    // Should have fused them into a single mesa blur effect
    EXPECT_TRUE(result.changed);
    ASSERT_EQ(composables.size(), 1); // Should be reduced to one effect
    auto effect = composables.front().GetEffect();
    ASSERT_NE(effect, nullptr);
    EXPECT_EQ(effect->GetImpl()->GetFilterType(), Drawing::GEFilterType::MESA_BLUR);
    auto params = effect->GetImpl()->GetMESAParams();
    ASSERT_NE(params, nullptr);
    
    auto mesaEffect = CreateMesaBlurEffect();
    auto paramsRef = mesaEffect->GetImpl()->GetMESAParams();
    ASSERT_NE(paramsRef, nullptr);
    EXPECT_EQ(params->radius, paramsRef->radius);
    EXPECT_EQ(params->greyCoef1, paramsRef->greyCoef1);
    EXPECT_EQ(params->greyCoef2, paramsRef->greyCoef2);
    EXPECT_EQ(params->offsetX, 0.0);
    EXPECT_EQ(params->offsetY, 0.0);
    EXPECT_EQ(params->offsetZ, 0.0);
    EXPECT_EQ(params->offsetW, 0.0);
    EXPECT_EQ(params->width, 0);
    EXPECT_EQ(params->height, 0.0);
    EXPECT_EQ(params->tileMode, 0);

    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunWithFusion end";
}

/**
 * @tc.name: HpsCompatiblePassGetLogName
 * @tc.desc: Test GEHpsCompatiblePass GetLogName function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsCompatiblePassGetLogName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassGetLogName start";
    
    GEHpsCompatiblePass pass;
    
    auto name = pass.GetLogName();
    EXPECT_EQ(name, "GEHpsCompatiblePass");
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassGetLogName end";
}

/**
 * @tc.name: HpsCompatiblePassRunWithBlur
 * @tc.desc: Test GEHpsCompatiblePass Run function with blur filters
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsCompatiblePassRunWithBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassRunWithBlur start";
    
    GEHpsCompatiblePass pass;
    
    std::vector<GEFilterComposable> composables;
    // Add a kawase blur effect
    auto kawaseEffect = CreateKawaseBlurEffect();
    composables.push_back(kawaseEffect);
    
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed); // This pass doesn't modify composables
    
    // Check that blur filter detection works correctly
    bool exists = pass.IsBlurFilterExists();
    EXPECT_TRUE(exists);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassRunWithBlur end";
}

/**
 * @tc.name: HpsCompatiblePassRunWithoutBlur
 * @tc.desc: Test GEHpsCompatiblePass Run function without blur filters
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsCompatiblePassRunWithoutBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassRunWithoutBlur start";
    
    GEHpsCompatiblePass pass;
    
    std::vector<GEFilterComposable> composables;
    // Add an AI bar effect (not a blur filter)
    auto greyEffect = CreateGreyEffect();
    composables.push_back(greyEffect);
    
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed); // This pass doesn't modify composables
    
    // Check that blur filter detection works correctly
    bool exists = pass.IsBlurFilterExists();
    EXPECT_FALSE(exists);
    
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsCompatiblePassRunWithoutBlur end";
}

} // namespace Rosen
} // namespace OHOS