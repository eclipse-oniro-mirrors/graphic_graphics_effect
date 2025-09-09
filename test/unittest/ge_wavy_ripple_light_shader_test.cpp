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
 * @tc.name: GEWavyRippleLightShaderTest001
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderTest001, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.0f, 0.2f}, 0.5f, 0.2f};
    auto shader = GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GEWavyRippleLightShaderTest002
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderTest002, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.5f, 0.3f}, 0.1f, 0.5f};
    auto shader = GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    ASSERT_NE(shader, nullptr);

    auto builder1 = shader->GetWavyRippleLightBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader->GetWavyRippleLightBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GEWavyRippleLightShaderTest003
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderTest003, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.5f, 0.3f}, 0.1f, 0.1f};
    auto shader = GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader->MakeWavyRippleLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GEWavyRippleLightShaderTest004
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderTest004, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{1.0f, 2.0f}, 0.8f, 0.0f};
    auto shader = GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 50, 50};
    shader->MakeDrawingShader(rect, 0.5f);
    SUCCEED();
}

/**
 * @tc.name: GEWavyRippleLightShaderTest005
 * @tc.type: FUNC
 */
HWTEST_F(GEWavyRippleLightShaderTest, GEWavyRippleLightShaderTest005, TestSize.Level1)
{
    Drawing::GEWavyRippleLightShaderParams params{{0.8f, 0.3f}, 0.1f, 2.0f};
    auto shader = GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    shader->MakeDrawingShader(rect, 0.75f);
    shader->GEWavyRippleLightShader::CreateWavyRippleLightShader(params);
    ASSERT_NE(shader, nullptr);
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
