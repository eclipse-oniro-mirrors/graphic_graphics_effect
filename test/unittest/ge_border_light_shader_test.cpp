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
#include "ge_border_light_shader.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBorderLightShaderTest : public testing::Test {
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
 * @tc.name: GEBorderLightShaderTest002
 * @tc.type: FUNC
 */
HWTEST_F(GEBorderLightShaderTest, GEBorderLightShaderTest002, TestSize.Level1)
{
    Drawing::GEBorderLightShaderParams params{
        Vector3f{0.0f, 1.0f, 2.0f}, Vector4f{0.2f, 0.3f, 0.4f, 0.5f}, 1.0f, 2.0f, Vector3f{0.1f, 0.2f, 0.3f}};
    auto shader = GEBorderLightShader(params);

    auto builder1 = shader.GetBorderLightBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader.GetBorderLightBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GEBorderLightShaderTest003
 * @tc.type: FUNC
 */
HWTEST_F(GEBorderLightShaderTest, GEBorderLightShaderTest003, TestSize.Level1)
{
    Drawing::GEBorderLightShaderParams params{
        Vector3f{2.0f, 3.0f, 4.0f}, Vector4f{0.3f, 0.4f, 0.5f, 0.6f}, 1.5f, 3.0f, Vector3f{0.5f, 0.5f, 0.5f}};
    auto shader = GEBorderLightShader(params);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader.MakeBorderLightShader(rect);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: GEBorderLightShaderTest004
 * @tc.type: FUNC
 */
HWTEST_F(GEBorderLightShaderTest, GEBorderLightShaderTest004, TestSize.Level1)
{
    Drawing::GEBorderLightShaderParams params{
        Vector3f{3.0f, 4.0f, 5.0f}, Vector4f{0.4f, 0.5f, 0.6f, 0.7f}, 2.0f, 4.0f, Vector3f{1.0f, 1.0f, 1.0f}};
    auto shader = GEBorderLightShader(params);

    Drawing::Rect rect{0, 0, 50, 50};
    shader.MakeDrawingShader(rect, 0.5f);
}

/**
 * @tc.name: GEBorderLightShaderTest006
 * @tc.type: FUNC
 */
HWTEST_F(GEBorderLightShaderTest, GEBorderLightShaderTest006, TestSize.Level1)
{
    Drawing::GEBorderLightShaderParams params{
        Vector3f{5.0f, 6.0f, 7.0f}, Vector4f{0.6f, 0.7f, 0.8f, 0.9f}, 3.0f, 6.0f, Vector3f{0.0f, 0.0f, 0.0f}};
    auto shader = GEBorderLightShader(params);

    Drawing::Rect rect{0, 0, 100, 100};
    shader.MakeDrawingShader(rect, 0.75f);
    shader.SetBorderLightParams(params);
}

/**
 * @tc.name: Type
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEBorderLightShaderTest, Type_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEBorderLightShaderTest Type_001 start";

    auto filter = std::make_unique<GEBorderLightShader>();
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::BORDER_LIGHT);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_SHADER_BORDER_LIGHT);

    GTEST_LOG_(INFO) << "GEBorderLightShaderTest Type_001 end";
}
}  // namespace Rosen
}  // namespace OHOS
