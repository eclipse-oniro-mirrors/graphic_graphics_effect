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

#include "ge_filter_type.h"
#include "ge_grey_shader_filter.h"
#include "ge_kawase_blur_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEFilterTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

using GEFilterParams = Drawing::GEFilterParams;

/**
 * @tc.name: GEFilterParamsBoxUnboxGreyShader
 * @tc.desc: Test GEFilterParams Box/Unbox functionality for GEGreyShaderFilterParams
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterTypeTest, BoxUnboxGreyShader, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxGreyShader start";
    
    // Create params for grey shader filter
    Drawing::GEGreyShaderFilterParams params { 0.5f, 0.7f };
    
    // Box the params into type-erased GEFilterParams
    auto boxedParams = GEFilterParams::Box(params);
    EXPECT_NE(boxedParams, nullptr);
    
    // Unbox back to original type
    auto unboxedParams = GEFilterParams::Unbox<Drawing::GEGreyShaderFilterParams>(boxedParams);
    EXPECT_TRUE(unboxedParams.has_value());
    
    // Verify values match
    EXPECT_EQ(unboxedParams->greyCoef1, params.greyCoef1);
    EXPECT_EQ(unboxedParams->greyCoef2, params.greyCoef2);
    
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxGreyShader end";
}

/**
 * @tc.name: GEFilterParamsBoxUnboxKawaseBlur
 * @tc.desc: Test GEFilterParams Box/Unbox functionality for GEKawaseBlurShaderFilterParams
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterTypeTest, BoxUnboxKawaseBlur, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxKawaseBlur start";
    
    // Create params for kawase blur filter
    Drawing::GEKawaseBlurShaderFilterParams params { 5 };
    
    // Box the params into type-erased GEFilterParams
    auto boxedParams = GEFilterParams::Box(params);
    EXPECT_NE(boxedParams, nullptr);
    
    // Unbox back to original type
    auto unboxedParams = GEFilterParams::Unbox<Drawing::GEKawaseBlurShaderFilterParams>(boxedParams);
    EXPECT_TRUE(unboxedParams.has_value());
    
    // Verify values match
    EXPECT_EQ(unboxedParams->radius, params.radius);
    
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxKawaseBlur end";
}

/**
 * @tc.name: GEFilterParamsBoxUnboxSharedPtr
 * @tc.desc: Test GEFilterParams Box/Unbox functionality with shared_ptr
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterTypeTest, BoxUnboxSharedPtr, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxSharedPtr start";
    
    // Create params for grey shader filter
    auto params = std::make_shared<Drawing::GEGreyShaderFilterParams>(Drawing::GEGreyShaderFilterParams{0.3f, 0.8f});
    
    // Box the shared_ptr params into type-erased GEFilterParams
    auto boxedParams = GEFilterParams::Box(params);
    EXPECT_NE(boxedParams, nullptr);
    
    // Unbox back to original shared_ptr type
    auto unboxedParams = GEFilterParams::Unbox<std::shared_ptr<Drawing::GEGreyShaderFilterParams>>(boxedParams);
    EXPECT_NE(unboxedParams, nullptr);
    
    // Verify values match
    EXPECT_EQ(unboxedParams->greyCoef1, params->greyCoef1);
    EXPECT_EQ(unboxedParams->greyCoef2, params->greyCoef2);
    
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsBoxUnboxSharedPtr end";
}

/**
 * @tc.name: GEFilterParamsInvalidUnbox
 * @tc.desc: Test GEFilterParams invalid unboxing behavior
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterTypeTest, InvalidUnbox, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsInvalidUnbox start";
    
    // Create params for grey shader filter
    Drawing::GEGreyShaderFilterParams params { 0.5f, 0.7f };
    
    // Box the params
    auto boxedParams = GEFilterParams::Box(params);
    EXPECT_NE(boxedParams, nullptr);
    
    // Try to unbox with wrong type - should return empty optional
    auto unboxedParams = GEFilterParams::Unbox<Drawing::GEKawaseBlurShaderFilterParams>(boxedParams);
    EXPECT_FALSE(unboxedParams.has_value());
    
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsInvalidUnbox end";
}

/**
 * @tc.name: GEFilterParamsTypeRegistration
 * @tc.desc: Test that filter types are properly registered
 * @tc.type: FUNC
 */
HWTEST_F(GEFilterTypeTest, TypeRegistration, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsTypeRegistration start";

    // Test static checks for registered types
    static_assert(GEFilterParams::IsRegisteredFilterTypeInfo<GEGreyShaderFilter>,
                  "GEGreyShaderFilter should be registered");
    static_assert(GEFilterParams::IsRegisteredFilterTypeInfo<GEKawaseBlurShaderFilter>,
                  "GEKawaseBlurShaderFilter should be registered");
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<Drawing::GEGreyShaderFilterParams>,
                  "GEGreyShaderFilterParams should be registered");
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<Drawing::GEKawaseBlurShaderFilterParams>,
                  "GEKawaseBlurShaderFilterParams should be registered");
    
    // Test type info access
    EXPECT_EQ(Drawing::GEFilterTypeInfo<GEGreyShaderFilter>::ID, Drawing::GEFilterType::GREY);
    EXPECT_EQ(Drawing::GEFilterParamsTypeInfo<Drawing::GEGreyShaderFilterParams>::ID, Drawing::GEFilterType::GREY);    
    EXPECT_EQ(Drawing::GEFilterTypeInfo<GEKawaseBlurShaderFilter>::ID, Drawing::GEFilterType::KAWASE_BLUR);
    EXPECT_EQ(Drawing::GEFilterParamsTypeInfo<Drawing::GEKawaseBlurShaderFilterParams>::ID, 
              Drawing::GEFilterType::KAWASE_BLUR);    

    GTEST_LOG_(INFO) << "GEFilterTypeTest GEFilterParamsTypeRegistration end";
}

} // namespace Rosen
} // namespace OHOS