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

#include "ge_filter_composer.h"
#include "ge_filter_composer_pass.h"
#include "ge_filter_type.h"
#include "ge_filter_type_info.h"
#include "ge_direct_draw_on_canvas_pass.h"
#include "ge_hps_build_pass.h"
#include "ge_mesa_fusion_pass.h"
#include "ge_render.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

using Drawing::GEFilterType;

class GEFilterComposerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        std::vector<const char*> extensionProperties { "hps_gaussian_blur_effect", "hps_mesa_blur_effect",
            "hps_gray_effect" };
        HpsEffectFilter::UnitTestSetExtensionProperties(extensionProperties); // valid because -Dprivate=public
    }
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

    // Helper methods
    std::shared_ptr<Drawing::GEVisualEffect> CreateVisualEffect(std::string type);
    std::shared_ptr<Drawing::GEVisualEffect> CreateGreyEffect();
    std::shared_ptr<Drawing::GEVisualEffect> CreateKawaseBlurEffect();
    std::shared_ptr<Drawing::GEVisualEffect> CreateMesaBlurEffect();

    static inline Drawing::Canvas canvas_;
};

std::shared_ptr<Drawing::GEVisualEffect> GEFilterComposerTest::CreateVisualEffect(std::string type)
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
    auto effect = CreateVisualEffect(Drawing::GE_FILTER_MESA_BLUR);
    Drawing::GEMESABlurShaderFilterParams params { 2, 0.5f, 0.5f };
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, params.radius);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, params.greyCoef1);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, params.greyCoef2);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.f);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.f);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.f);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.f);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.f);
    effect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.f);
    return effect;
}

struct DummyFilter : Drawing::IGEFilterType {};

/**
 * @tc.name: IGEFilterType
 * @tc.desc: Test IGEFilterType interface validness
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, IGEFilterType, TestSize.Level1)
{
    DummyFilter dummy;
    EXPECT_EQ(dummy.Type(), GEFilterType::NONE);
    EXPECT_EQ(dummy.TypeName(), "IGEFilterType");
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
    auto hpsEffect = std::make_shared<HpsEffectFilter>(canvas_);
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
    GEHpsBuildPass pass(canvas_, context);

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
    GEHpsBuildPass pass(canvas_, context);

    std::vector<GEFilterComposable> composables;
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);

    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassRunNoEffects end";
}

/**
 * @tc.name: HpsBuildPassRunUnsupportedEffect
 * @tc.desc: Test GEHpsBuildPass Run function with no effects
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsBuildPassRunUnsupportedEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassRunUnsupportedEffect start";

    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = nullptr; // Not used in this test
    GEHpsBuildPass pass(canvas_, context);

    std::vector<GEFilterComposable> composables;
    composables.push_back(CreateVisualEffect(Drawing::GE_FILTER_WATER_RIPPLE));
    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);

    GTEST_LOG_(INFO) << "GEFilterComposerTest HpsBuildPassRunUnsupportedEffect end";
}

/**
 * @tc.name: HpsBuildPassRunWithComposableEffect
 * @tc.desc: Test GEHpsBuildPass Run function with composable effects
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsBuildPassRunWithComposableEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEHpsBuildPassTest HpsBuildPassRunWithComposableEffect start";

    // Create a bitmap to use as image
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    auto image = bmp.MakeImage();

    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = image;
    context.src = Drawing::Rect { 0.0f, 0.0f, 50.0f, 50.0f };
    context.dst = Drawing::Rect { 0.0f, 0.0f, 50.0f, 50.0f };
    context.saturationForHPS = 1.0f;
    context.brightnessForHPS = 1.0f;

    GEHpsBuildPass pass(canvas_, context);

    std::vector<GEFilterComposable> composables;
    auto mesaEffect = CreateMesaBlurEffect();
    auto greyEffect = CreateGreyEffect();
    auto kawaseBlurEffect = CreateKawaseBlurEffect();
    composables.push_back(mesaEffect);
    composables.push_back(greyEffect);
    composables.push_back(kawaseBlurEffect);

    auto result = pass.Run(composables);
    // Should compose the effect into a HpsEffectFilter
    EXPECT_TRUE(result.changed);
    ASSERT_EQ(composables.size(), 1);

    // Check that the result is a HpsEffectFilter (not a regular GEVisualEffect)
    auto hpsEffect = composables[0].GetHpsEffect();
    ASSERT_NE(hpsEffect, nullptr);
    EXPECT_EQ(hpsEffect->hpsEffect_.size(), 3); // 3: added 3 effects above

    GTEST_LOG_(INFO) << "GEHpsBuildPassTest HpsBuildPassRunWithComposableEffect end";
}

/**
 * @tc.name: HpsBuildPassRunWithMixedEffect
 * @tc.desc: Test GEHpsBuildPass Run function with composable and unsupported effects
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, HpsBuildPassRunWithMixedEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEHpsBuildPassTest HpsBuildPassRunWithMixedEffect start";

    // Create a bitmap to use as image
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    auto image = bmp.MakeImage();

    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = image;
    context.src = Drawing::Rect { 0.0f, 0.0f, 50.0f, 50.0f };
    context.dst = Drawing::Rect { 0.0f, 0.0f, 50.0f, 50.0f };
    context.saturationForHPS = 1.0f;
    context.brightnessForHPS = 1.0f;

    GEHpsBuildPass pass(canvas_, context);

    std::vector<GEFilterComposable> composables {
        CreateGreyEffect(), CreateGreyEffect(),
        CreateVisualEffect(Drawing::GE_FILTER_WATER_RIPPLE),
        CreateGreyEffect()
    };

    auto result = pass.Run(composables);
    // Should not compose at all. HPS currently requires all filters to be applied.
    EXPECT_FALSE(result.changed);
    EXPECT_EQ(composables.size(), 4); // 4: original size

    GTEST_LOG_(INFO) << "GEHpsBuildPassTest HpsBuildPassRunWithMixedEffect end";
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
    auto greyEffect = CreateGreyEffect();
    auto mesaBlurEffect = CreateMesaBlurEffect();
    composables.push_back(greyEffect);
    composables.push_back(mesaBlurEffect);

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
 * @tc.name: MesaFusionPassRunOddSize
 * @tc.desc: Test GEMesaFusionPass Run function with odd number of composables
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, MesaFusionPassRunOddSize, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunOddSize start";

    GEMesaFusionPass pass;

    std::vector<GEFilterComposable> composables;
    // Add 3 effects: grey -> kawase blur -> grey (no fusion)
    auto greyEffect1 = CreateGreyEffect();
    auto kawaseEffect = CreateKawaseBlurEffect();
    auto greyEffect2 = CreateGreyEffect();
    composables.push_back(greyEffect1);
    composables.push_back(kawaseEffect);
    composables.push_back(greyEffect2);

    auto result = pass.Run(composables);
    // Should have fused first two (grey + kawase) but leave last one alone
    EXPECT_TRUE(result.changed);
    ASSERT_EQ(composables.size(), 2); // Should be reduced to two effects
    auto effect1 = composables[0].GetEffect();
    ASSERT_NE(effect1, nullptr);
    EXPECT_EQ(effect1->GetImpl()->GetFilterType(), Drawing::GEFilterType::MESA_BLUR);
    auto effect2 = composables[1].GetEffect();
    ASSERT_NE(effect2, nullptr);
    EXPECT_EQ(effect2->GetImpl()->GetFilterType(), Drawing::GEFilterType::GREY); // Should be left as grey

    GTEST_LOG_(INFO) << "GEFilterComposerTest MesaFusionPassRunOddSize end";
}

/**
 * @tc.name: GEFilterComposerRunNoPasses
 * @tc.desc: Test GEFilterComposer Run function with no passes
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEFilterComposerRunNoPasses, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerRunNoPasses start";

    GEFilterComposer composer;
    std::vector<GEFilterComposable> composables;

    auto result = composer.Run(composables);
    EXPECT_FALSE(result.anyPassChanged);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerRunNoPasses end";
}

/**
 * @tc.name: GEFilterComposerBuildComposables
 * @tc.desc: Test GEFilterComposer BuildComposables function
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEFilterComposerBuildComposables, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerBuildComposables start";

    // Create some visual effects
    std::vector<std::shared_ptr<Drawing::GEVisualEffect>> effects;
    auto greyEffect = CreateGreyEffect();
    effects.push_back(greyEffect);
    auto kawaseBlurEffect = CreateKawaseBlurEffect();
    effects.push_back(kawaseBlurEffect);

    auto composables = GEFilterComposer::BuildComposables(effects);

    EXPECT_EQ(composables.size(), 2);

    // Check that the composables contain the expected effects
    EXPECT_EQ(composables[0].GetEffect(), greyEffect);
    EXPECT_EQ(composables[1].GetEffect(), kawaseBlurEffect);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerBuildComposables end";
}

/**
 * @tc.name: GEFilterComposerAddNullPass
 * @tc.desc: Test GEFilterComposer Add function with null pass
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEFilterComposerAddNullPass, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerAddNullPass start";

    GEFilterComposer composer;

    // This should not crash and should just ignore the null pass
    std::unique_ptr<GEFilterComposerPass> nullPass = nullptr;
    composer.Add(std::move(nullPass));
    EXPECT_TRUE(composer.passes_.empty());

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerAddNullPass end";
}

/**
 * @tc.name: GEFilterComposerTemplateAdd
 * @tc.desc: Test GEFilterComposer template Add function with pass constructor args
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEFilterComposerTemplateAdd, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerTemplateAdd start";

    GEFilterComposer composer;

    // Test template Add with constructor arguments
    GraphicsEffectEngine::GERender::HpsGEImageEffectContext context;
    context.image = nullptr; // Not used in this test
    composer.Add<GEHpsBuildPass>(canvas_, context);
    EXPECT_EQ(composer.passes_.size(), 1);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerTemplateAdd end";
}

/**
 * @tc.name: GEFilterComposerRunWithChanges
 * @tc.desc: Test GEFilterComposer Run function when passes make changes
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEFilterComposerRunWithChanges, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerRunWithChanges start";

    GEFilterComposer composer;
    composer.Add<GEMesaFusionPass>();

    std::vector<GEFilterComposable> composables;
    composables.push_back(CreateGreyEffect());
    composables.push_back(CreateKawaseBlurEffect());

    auto result = composer.Run(composables);
    EXPECT_TRUE(result.anyPassChanged);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEFilterComposerRunWithChanges end";
}

/**
 * @tc.name: GEDirectDrawOnCanvasPassRun
 * @tc.desc: Test GEDirectDrawOnCanvasPass Run function with valid composables
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEDirectDrawOnCanvasPassRun, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRun start";

    GEDirectDrawOnCanvasPass pass;
    std::vector<GEFilterComposable> composables;

    // Other effects should not be affected
    auto effect1 = CreateGreyEffect();
    composables.push_back(effect1);
    auto effect2 = CreateGreyEffect();
    composables.push_back(effect2);

    // Add the final visual effect
    auto effect = CreateGreyEffect();
    composables.push_back(effect);

    auto result = pass.Run(composables);
    EXPECT_TRUE(result.changed);
    EXPECT_FALSE(effect1->GetAllowDirectDrawOnCanvas());
    EXPECT_FALSE(effect2->GetAllowDirectDrawOnCanvas());
    EXPECT_TRUE(effect->GetAllowDirectDrawOnCanvas());

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRun end";
}

/**
 * @tc.name: GEDirectDrawOnCanvasPassRunEmptyComposables
 * @tc.desc: Test GEDirectDrawOnCanvasPass Run function with empty composables
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEDirectDrawOnCanvasPassRunEmptyComposables, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRunEmptyComposables start";

    GEDirectDrawOnCanvasPass pass;
    std::vector<GEFilterComposable> composables;

    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRunEmptyComposables end";
}

/**
 * @tc.name: GEDirectDrawOnCanvasPassRunNullEffect
 * @tc.desc: Test GEDirectDrawOnCanvasPass Run function with null effect
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterComposerTest, GEDirectDrawOnCanvasPassRunNullEffect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRunNullEffect start";

    GEDirectDrawOnCanvasPass pass;
    std::vector<GEFilterComposable> composables;
    auto nullEffect = std::shared_ptr<Drawing::GEVisualEffect>(nullptr);
    composables.push_back(GEFilterComposable(nullEffect));

    auto result = pass.Run(composables);
    EXPECT_FALSE(result.changed);

    GTEST_LOG_(INFO) << "GEFilterComposerTest GEDirectDrawOnCanvasPassRunNullEffect end";
}

} // namespace Rosen
} // namespace OHOS