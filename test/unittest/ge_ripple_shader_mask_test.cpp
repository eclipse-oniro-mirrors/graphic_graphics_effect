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

#include "ge_ripple_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GERippleShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GERippleShaderMaskTest::SetUpTestCase(void) {}

void GERippleShaderMaskTest::TearDownTestCase(void) {}

void GERippleShaderMaskTest::SetUp() {}

void GERippleShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify the GenerateDrawingShader function
 * @tc.type: FUNC
 */
HWTEST_F(GERippleShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 start";

    GERippleShaderMaskParams param;
    auto geRippleShaderMask = std::make_shared<GERippleShaderMask>(param);
    auto shaderEffect = geRippleShaderMask->GenerateDrawingShader(0.5f, 0.5f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GetRippleShaderMaskBuilder_001
 * @tc.desc: Verify the GetRippleShaderMaskBuilder function
 * @tc.type: FUNC
 */
HWTEST_F(GERippleShaderMaskTest, GetRippleShaderMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetRippleShaderMaskBuilder_001 start";

    GERippleShaderMaskParams param;
    auto geRippleShaderMask = std::make_shared<GERippleShaderMask>(param);
    auto shaderBuilder = geRippleShaderMask->GetRippleShaderMaskBuilder();
    EXPECT_NE(shaderBuilder, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetRippleShaderMaskBuilder_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify the GenerateDrawingShaderHasNormal function
 * @tc.type: FUNC
 */
HWTEST_F(GERippleShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_001 start";

    GERippleShaderMaskParams param;
    auto geRippleShaderMask = std::make_shared<GERippleShaderMask>(param);
    auto shaderEffect = geRippleShaderMask->GenerateDrawingShaderHasNormal(0.5f, 0.5f);
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GetRippleShaderNormalMaskBuilder_001
 * @tc.desc: Verify the GetRippleShaderNormalMaskBuilder function
 * @tc.type: FUNC
 */
HWTEST_F(GERippleShaderMaskTest, GetRippleShaderNormalMaskBuilder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest GetRippleShaderNormalMaskBuilder_001 start";

    GERippleShaderMaskParams param;
    auto geRippleShaderMask = std::make_shared<GERippleShaderMask>(param);
    auto shaderBuilder = geRippleShaderMask->GetRippleShaderNormalMaskBuilder();
    EXPECT_NE(shaderBuilder, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectTest GetRippleShaderNormalMaskBuilder_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
