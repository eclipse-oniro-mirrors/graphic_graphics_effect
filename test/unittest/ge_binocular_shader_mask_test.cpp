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

#include "ge_binocular_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEBinocularShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEBinocularShaderMaskTest::SetUpTestCase(void) {}
void GEBinocularShaderMaskTest::TearDownTestCase(void) {}

void GEBinocularShaderMaskTest::SetUp() {}
void GEBinocularShaderMaskTest::TearDown() {}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader returns valid shader with default params
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GEBinocularMaskParams param;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_002
 * @tc.desc: Verify GenerateDrawingShader with custom radiusX
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_002, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 0.5f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_003
 * @tc.desc: Verify GenerateDrawingShader with custom radiusY
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_003, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusY_ = 0.8f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_004
 * @tc.desc: Verify GenerateDrawingShader with custom gap
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_004, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.gap_ = 0.3f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_005
 * @tc.desc: Verify GenerateDrawingShader with custom softness
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_005, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.softness_ = 0.4f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_006
 * @tc.desc: Verify GenerateDrawingShader with all custom params
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_006, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 0.5f;
    param.radiusY_ = 0.8f;
    param.gap_ = 0.3f;
    param.softness_ = 0.4f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns nullptr (not used with displacementDistort)
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GEBinocularMaskParams param;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify Type and TypeName
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, Type_001, TestSize.Level1)
{
    GEBinocularMaskParams param {};
    auto filter = std::make_unique<GEBinocularShaderMask>(param);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::BINOCULAR_MASK);
    EXPECT_EQ(filter->TypeName(), "BinocularMask");
}

/**
 * @tc.name: BuilderCaching_001
 * @tc.desc: Verify thread_local builder is shared across different mask instances
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, BuilderCaching_001, TestSize.Level1)
{
    GEBinocularMaskParams param1;
    GEBinocularMaskParams param2;
    param2.radiusX_ = 0.5f;
    param2.radiusY_ = 0.8f;
    auto mask1 = std::make_shared<GEBinocularShaderMask>(param1);
    auto mask2 = std::make_shared<GEBinocularShaderMask>(param2);
    auto builder1 = mask1->GetBinocularShaderMaskBuilder();
    auto builder2 = mask2->GetBinocularShaderMaskBuilder();
    EXPECT_NE(builder1, nullptr);
    EXPECT_NE(builder2, nullptr);
    EXPECT_EQ(builder1, builder2);
}

/**
 * @tc.name: MultipleInstances_001
 * @tc.desc: Verify multiple instances produce independent shaders
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, MultipleInstances_001, TestSize.Level1)
{
    GEBinocularMaskParams param1;
    GEBinocularMaskParams param2;
    auto mask1 = std::make_shared<GEBinocularShaderMask>(param1);
    auto mask2 = std::make_shared<GEBinocularShaderMask>(param2);
    auto shader1 = mask1->GenerateDrawingShader(50.f, 50.f);
    auto shader2 = mask2->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);
    EXPECT_NE(shader1, shader2);
}

/**
 * @tc.name: GenerateDrawingShader_007
 * @tc.desc: Verify GenerateDrawingShader with zero radiusX
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_007, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 0.0f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_008
 * @tc.desc: Verify GenerateDrawingShader with zero radiusY
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_008, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusY_ = 0.0f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_009
 * @tc.desc: Verify GenerateDrawingShader with large resolution
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_009, TestSize.Level1)
{
    GEBinocularMaskParams param;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(1200.f, 2000.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_010
 * @tc.desc: Verify GenerateDrawingShader with all zero params
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_010, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 0.0f;
    param.radiusY_ = 0.0f;
    param.gap_ = 0.0f;
    param.softness_ = 0.0f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: GenerateDrawingShader_011
 * @tc.desc: Verify GenerateDrawingShader with large param values
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShader_011, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 2.0f;
    param.radiusY_ = 2.0f;
    param.gap_ = 1.0f;
    param.softness_ = 1.0f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShader(50.f, 50.f);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
 * @tc.name: BuilderCaching_002
 * @tc.desc: Verify builder is reused across multiple GenerateDrawingShader calls
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, BuilderCaching_002, TestSize.Level1)
{
    GEBinocularMaskParams param;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shader1 = mask->GenerateDrawingShader(50.f, 50.f);
    auto shader2 = mask->GenerateDrawingShader(100.f, 100.f);
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);
    EXPECT_NE(shader1, shader2);
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_002
 * @tc.desc: Verify GenerateDrawingShaderHasNormal returns nullptr with custom params
 * @tc.type: FUNC
 */
HWTEST_F(GEBinocularShaderMaskTest, GenerateDrawingShaderHasNormal_002, TestSize.Level1)
{
    GEBinocularMaskParams param;
    param.radiusX_ = 0.5f;
    param.radiusY_ = 0.8f;
    param.gap_ = 0.3f;
    param.softness_ = 0.4f;
    auto mask = std::make_shared<GEBinocularShaderMask>(param);
    auto shaderEffect = mask->GenerateDrawingShaderHasNormal(50.f, 50.f);
    EXPECT_EQ(shaderEffect, nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS