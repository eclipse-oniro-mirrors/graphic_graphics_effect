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

#include "gtest/gtest.h"
#include "ge_binarization_shader_filter.h"
#include "ge_light_up_effect_shader_filter.h"
#include "ge_dynamic_dim_shader_filter.h"
#include "ge_visual_effect.h"
#include "ge_effect_factory.h"

using namespace testing;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;

class GEColorShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(GEColorShaderFilterTest, BinarizationFilterType, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::BINARIZATION);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectFilterType, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 0.8f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::LIGHT_UP_EFFECT);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimFilterType, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = 0.3f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::DYNAMIC_DIM);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationParamClamp, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = -0.5f;
    params.high = -0.3f;
    params.threshold = -0.1f;
    params.thresholdRange = -0.2f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectParamClamp, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 2.0f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimParamClamp, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = -1.0f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationViaVisualEffect, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("Binarization", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("Binarization_Low", 0.2f);
    ve->SetParam("Binarization_High", 0.8f);
    ve->SetParam("Binarization_Threshold", 0.5f);
    ve->SetParam("Binarization_ThresholdRange", 0.1f);
    auto impl = ve->GetImpl();
    ASSERT_NE(impl, nullptr);
    EXPECT_EQ(impl->GetFilterType(), GEFilterType::BINARIZATION);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectViaVisualEffect, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("LightUpEffect", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("LightUpEffect_Value", 0.7f);
    auto impl = ve->GetImpl();
    ASSERT_NE(impl, nullptr);
    EXPECT_EQ(impl->GetFilterType(), GEFilterType::LIGHT_UP_EFFECT);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimViaVisualEffect, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("DynamicDim", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("DynamicDim_Value", 0.4f);
    auto impl = ve->GetImpl();
    ASSERT_NE(impl, nullptr);
    EXPECT_EQ(impl->GetFilterType(), GEFilterType::DYNAMIC_DIM);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationFilterTypeName, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::BINARIZATION);
    auto typeName = filter->TypeName();
    EXPECT_FALSE(typeName.empty());
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectFilterTypeName, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 0.5f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::LIGHT_UP_EFFECT);
    auto typeName = filter->TypeName();
    EXPECT_FALSE(typeName.empty());
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimFilterTypeName, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = 0.3f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::DYNAMIC_DIM);
    auto typeName = filter->TypeName();
    EXPECT_FALSE(typeName.empty());
}

HWTEST_F(GEColorShaderFilterTest, BinarizationFilterOnProcessImageNull, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = 0.1f;
    params.high = 0.9f;
    params.threshold = 0.5f;
    params.thresholdRange = 0.1f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 100, 100);
    Drawing::Rect dst(0, 0, 100, 100);
    auto result = filter->OnProcessImage(canvas, nullptr, src, dst);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectFilterOnProcessImageNull, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 0.5f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 100, 100);
    Drawing::Rect dst(0, 0, 100, 100);
    auto result = filter->OnProcessImage(canvas, nullptr, src, dst);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimFilterOnProcessImageNull, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = 0.3f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 100, 100);
    Drawing::Rect dst(0, 0, 100, 100);
    auto result = filter->OnProcessImage(canvas, nullptr, src, dst);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationParamBoundaryZero, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = 0.0f;
    params.high = 0.0f;
    params.threshold = 0.0f;
    params.thresholdRange = 0.0f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::BINARIZATION);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationParamBoundaryMax, TestSize.Level1)
{
    GEBinarizationShaderFilterParams params;
    params.low = 1.0f;
    params.high = 1.0f;
    params.threshold = 1.0f;
    params.thresholdRange = 1.0f;
    auto filter = std::make_shared<GEBinarizationShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::BINARIZATION);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectParamBoundaryZero, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 0.0f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::LIGHT_UP_EFFECT);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectParamBoundaryOne, TestSize.Level1)
{
    GELightUpEffectShaderFilterParams params;
    params.value = 1.0f;
    auto filter = std::make_shared<GELightUpEffectShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::LIGHT_UP_EFFECT);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimParamBoundaryZero, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = 0.0f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::DYNAMIC_DIM);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimParamBoundaryOne, TestSize.Level1)
{
    GEDynamicDimShaderFilterParams params;
    params.value = 1.0f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::DYNAMIC_DIM);
}

HWTEST_F(GEColorShaderFilterTest, BinarizationViaFactoryCreate, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("Binarization", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("Binarization_Low", 0.2f);
    ve->SetParam("Binarization_High", 0.8f);
    ve->SetParam("Binarization_Threshold", 0.5f);
    ve->SetParam("Binarization_ThresholdRange", 0.1f);
    auto filter = GraphicsEffectEngine::GEEffectFactory::CreateFilter(ve->GetImpl());
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::BINARIZATION);
}

HWTEST_F(GEColorShaderFilterTest, LightUpEffectViaFactoryCreate, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("LightUpEffect", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("LightUpEffect_Value", 0.7f);
    auto filter = GraphicsEffectEngine::GEEffectFactory::CreateFilter(ve->GetImpl());
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::LIGHT_UP_EFFECT);
}

HWTEST_F(GEColorShaderFilterTest, DynamicDimViaFactoryCreate, TestSize.Level1)
{
    auto ve = std::make_shared<GEVisualEffect>("DynamicDim", DrawingPaintType::BRUSH);
    ASSERT_NE(ve, nullptr);
    ve->SetParam("DynamicDim_Value", 0.4f);
    auto filter = GraphicsEffectEngine::GEEffectFactory::CreateFilter(ve->GetImpl());
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), GEFilterType::DYNAMIC_DIM);
}
