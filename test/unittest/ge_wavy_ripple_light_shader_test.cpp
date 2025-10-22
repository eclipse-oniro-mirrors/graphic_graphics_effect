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
#include "ge_wavy_ripple_light_shader.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEWavyRippleLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {}
    void TearDown() override
    {}
};

/**
 * @tc.name: GEWavyRippleLightShaderConstructor_001
 * @tc.desc: Verify the constructor of GEWavyRippleLightShader
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderConstructor_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.0f, 0.2f}, 0.5f, 0.2f};
    auto shader = GEWavyRippleLightShader(params);

    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.center_.first, params.center_.first);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.center_.second, params.center_.second);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.radius_, params.radius_);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.thickness_, params.thickness_);
}

/**
 * @tc.name: MakeDrawingShader_001
 * @tc.desc: Verify function MakeDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, MakeDrawingShader_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{1.0f, 2.0f}, 0.8f, 0.0f};
    auto shader = GEWavyRippleLightShader(params);

    Drawing::Rect rect{0, 0, 50, 50};
    shader.MakeDrawingShader(rect, 0.5f);
    ASSERT_NE(shader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GetDescription_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.9f, 0.2f}, 0.33f, 0.25f};
    auto shader = GEWavyRippleLightShader(params);

    std::string expectStr = "GEWavyRippleLightShader";
    EXPECT_EQ(shader.GetDescription(), expectStr);
}

/**
 * @tc.name: SetWavyRippleLightParams_001
 * @tc.desc: Verify function SetWavyRippleLightParams
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, SetWavyRippleLightParams_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.8f, 0.3f}, 0.1f, 2.0f};
    auto shader = GEWavyRippleLightShader(params);

    Drawing::Rect rect{0, 0, 100, 100};
    shader.MakeDrawingShader(rect, 0.75f);

    Drawing::GEWavyRippleLightShaderParams params1{{0.5f, 0.2f}, 0.6f, 0.5f};
    shader.SetWavyRippleLightParams(params1);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.center_.first, params1.center_.first);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.center_.second, params1.center_.second);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.radius_, params1.radius_);
    EXPECT_FLOAT_EQ(shader.wavyRippleLightParams_.thickness_, params1.thickness_);
}

/**
 * @tc.name: MakeWavyRippleLightShader_001
 * @tc.desc: Verify function MakeWavyRippleLightShader
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, MakeWavyRippleLightShader_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.5f, 0.3f}, 0.1f, 0.1f};
    auto shader = GEWavyRippleLightShader(params);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader.MakeWavyRippleLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GetWavyRippleLightBuilder_001
 * @tc.desc: Verify function GetWavyRippleLightBuilder
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GetWavyRippleLightBuilder_001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.5f, 0.3f}, 0.1f, 0.5f};
    auto shader = GEWavyRippleLightShader(params);

    auto builder1 = shader.GetWavyRippleLightBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader.GetWavyRippleLightBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEWavyRippleLightShaderTest Type_001 start";

    auto filter = std::make_unique<GEWavyRippleLightShader>();
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::WAVY_RIPPLE_LIGHT);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_SHADER_WAVY_RIPPLE_LIGHT);

    GTEST_LOG_(INFO) << "GEWavyRippleLightShaderTest Type_001 end";
}
}  // namespace Rosen
}  // namespace OHOS
