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
#include "ge_sdf_border_shader.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "draw/color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

// 测试用RRect形状参数
static Drawing::GESDFRRectShapeParams testRRectParams = {
    {10.0f, 10.0f, 200.0f, 200.0f, 20.0f, 20.0f}
};

// 测试用border参数（内描边）
static Drawing::GESDFBorderParams testBorderParams = {
    Drawing::Color::COLOR_RED,  // color
    10.0f,                       // width
    false                          // isOutline (内描边)
};

// 测试用border参数（外描边）
static Drawing::GESDFBorderParams testOutlineParams = {
    Drawing::Color::COLOR_BLUE,  // color
    10.0f,                        // width
    true                           // isOutline (外描边)
};

class GESDFBorderShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: GetSDFBorderEffect_Inner_001
 * @tc.desc: 验证 isOutline=false 时返回内描边RuntimeEffect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Inner_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Inner_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Outline_001
 * @tc.desc: 验证 isOutline=true 时返回外描边RuntimeEffect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Outline_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testOutlineParams;
    params.border.isOutline = true;

    Drawing::GESDFBorderShader borderShader(params);
    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Outline_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Cache_Inner_001
 * @tc.desc: 验证重复调用返回同一内描边缓存实例
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Cache_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Inner_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    auto effect1 = borderShader.GetSDFBorderEffect();
    auto effect2 = borderShader.GetSDFBorderEffect();
    EXPECT_EQ(effect1, effect2);  // 同一缓存实例
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Inner_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Cache_Outline_001
 * @tc.desc: 验证重复调用返回同一外描边缓存实例
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Cache_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Outline_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testOutlineParams;
    params.border.isOutline = true;

    Drawing::GESDFBorderShader borderShader(params);
    auto effect1 = borderShader.GetSDFBorderEffect();
    auto effect2 = borderShader.GetSDFBorderEffect();
    EXPECT_EQ(effect1, effect2);  // 同一缓存实例
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Outline_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_Inner_001
 * @tc.desc: 验证内描边shader正确生成
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Inner_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Inner_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_Outline_001
 * @tc.desc: 验证外描边shader正确生成
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Outline_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testOutlineParams;
    params.border.isOutline = true;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Outline_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_NullShape_001
 * @tc.desc: 验证shape为nullptr时的处理
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_NullShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_NullShape_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = nullptr;
    params.border = testBorderParams;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_NullShape_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_ZeroWidth_001
 * @tc.desc: 验证rect宽度为0时的处理
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_ZeroWidth_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroWidth_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 0.0f, 300.0f};  // 宽度为0
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroWidth_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_ZeroHeight_001
 * @tc.desc: 验证rect高度为0时的处理
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_ZeroHeight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroHeight_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 0.0f};  // 高度为0
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroHeight_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_InnerBorder_001
 * @tc.desc: 验证内描边完整流程
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_InnerBorder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_InnerBorder_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testBorderParams;
    params.border.isOutline = false;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 300.0f};

    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);

    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_InnerBorder_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_OutlineBorder_001
 * @tc.desc: 验证外描边完整流程
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_OutlineBorder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_OutlineBorder_001 start";
    Drawing::GESDFBorderShaderParams params;
    params.shape = std::make_shared<Drawing::GESDFRRectShaderShape>(testRRectParams);
    params.border = testOutlineParams;
    params.border.isOutline = true;

    Drawing::GESDFBorderShader borderShader(params);
    Drawing::Rect rect {0, 0, 300.0f, 300.0f};

    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);

    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_OutlineBorder_001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
