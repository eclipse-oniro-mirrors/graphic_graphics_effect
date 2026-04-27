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
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;
using namespace Rosen::Drawing;

class GEEffectFactoryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEEffectFactoryTest::SetUpTestCase(void) {}
void GEEffectFactoryTest::TearDownTestCase(void) {}

void GEEffectFactoryTest::SetUp() {}

void GEEffectFactoryTest::TearDown() {}

/**
 * @tc.name: Create_001
 * @tc.desc: Verify Create function with valid filter type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_GREY);
    auto result = GEEffectFactory::Create(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: Create_002
 * @tc.desc: Verify Create function with valid shader type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_002, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_SHADER_BORDER_LIGHT);
    auto result = GEEffectFactory::Create(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: Create_003
 * @tc.desc: Verify Create function with valid mask type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_003, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_MASK_RIPPLE);
    auto result = GEEffectFactory::Create(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: Create_004
 * @tc.desc: Verify Create function with valid shape type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_004, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_SHAPE_SDF_RRECT_SHAPE);
    auto result = GEEffectFactory::Create(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: Create_005
 * @tc.desc: Verify Create function with nullptr input
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_005, TestSize.Level1)
{
    std::shared_ptr<GEVisualEffectImpl> nullImpl = nullptr;
    auto result = GEEffectFactory::Create(nullImpl);

    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Create_006
 * @tc.desc: Verify Create function with out of range type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_006, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_GREY);
    impl->filterType_ = GEVisualEffectImpl::FilterType::MAX;

    auto result = GEEffectFactory::Create(impl);

    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Create_007
 * @tc.desc: Verify Create function with unregistered type
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_007, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>("");
    impl->filterType_ = GEVisualEffectImpl::FilterType::NONE;

    auto result = GEEffectFactory::Create(impl);

    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateGreyFilter_001
 * @tc.desc: Verify creating Grey filter with parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateGreyFilter_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_GREY);
    impl->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 0.5f);
    impl->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 0.5f);

    auto result = GEEffectFactory::CreateFilter(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateKawaseBlurFilter_001
 * @tc.desc: Verify creating Kawase Blur filter with parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateKawaseBlurFilter_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_KAWASE_BLUR);
    impl->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 5);

    auto result = GEEffectFactory::CreateFilter(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateRippleMask_001
 * @tc.desc: Verify creating Ripple mask
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateRippleMask_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_MASK_RIPPLE);

    auto result = GEEffectFactory::CreateMask(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateBorderLightShader_001
 * @tc.desc: Verify creating Border Light shader
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateBorderLightShader_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_SHADER_BORDER_LIGHT);

    auto result = GEEffectFactory::CreateShader(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateSDFRRectShape_001
 * @tc.desc: Verify creating SDF RRect shape
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateSDFRRectShape_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_SHAPE_SDF_RRECT_SHAPE);

    auto result = GEEffectFactory::CreateShape(impl);

    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateMultipleEffects_001
 * @tc.desc: Verify creating multiple different effect types
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateMultipleEffects_001, TestSize.Level1)
{
    auto greyImpl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_GREY);
    auto greyResult = GEEffectFactory::CreateFilter(greyImpl);
    EXPECT_NE(greyResult, nullptr);

    auto rippleImpl = std::make_shared<GEVisualEffectImpl>(GE_MASK_RIPPLE);
    auto rippleResult = GEEffectFactory::CreateMask(rippleImpl);
    EXPECT_NE(rippleResult, nullptr);

    auto borderLightImpl = std::make_shared<GEVisualEffectImpl>(GE_SHADER_BORDER_LIGHT);
    auto borderLightResult = GEEffectFactory::CreateShader(borderLightImpl);
    EXPECT_NE(borderLightResult, nullptr);

    auto rrectShapeImpl = std::make_shared<GEVisualEffectImpl>(GE_SHAPE_SDF_RRECT_SHAPE);
    auto rrectShapeResult = GEEffectFactory::CreateShape(rrectShapeImpl);
    EXPECT_NE(rrectShapeResult, nullptr);
}

/**
 * @tc.name: CreateWithParams_001
 * @tc.desc: Verify creating effects with various parameter types
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, CreateWithParams_001, TestSize.Level1)
{
    auto impl1 = std::make_shared<GEVisualEffectImpl>(GE_FILTER_GREY);
    impl1->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 0.3f);
    impl1->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 0.7f);
    auto result1 = GEEffectFactory::CreateFilter(impl1);
    EXPECT_NE(result1, nullptr);

    auto impl2 = std::make_shared<GEVisualEffectImpl>(GE_FILTER_KAWASE_BLUR);
    impl2->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 10);
    auto result2 = GEEffectFactory::CreateFilter(impl2);
    EXPECT_NE(result2, nullptr);
}

/**
 * @tc.name: Create_FallbackSuccess_001
 * @tc.desc: Verify EXTERNAL_FALLBACK fallback success when external loader fails
 * @tc.type:FUNC
 */
HWTEST_F(GEEffectFactoryTest, Create_FallbackSuccess_001, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>(GE_FILTER_MESA_BLUR);
    impl->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 10);

    auto result = GEEffectFactory::CreateFilter(impl);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->Type(), GEFilterType::MESA_BLUR);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS