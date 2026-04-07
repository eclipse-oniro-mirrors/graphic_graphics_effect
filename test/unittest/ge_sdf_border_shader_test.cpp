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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFBorderShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

    std::shared_ptr<GESDFShaderShape> CreateTestShape() const
    {
        GESDFRRectShapeParams param;
        param.rrect = {10.0f, 10.0f, 200.0f, 200.0f};
        param.rrect.SetCornerRadius(20.0f, 20.0f);
        return std::make_shared<GESDFRRectShaderShape>(param);
    }

    GESDFBorderParams GetTestBorderParams() const
    {
        GESDFBorderParams params;
        params.color = Color::COLOR_RED;
        params.width = 10.0f;
        params.isOutline = false;
        return params;
    }

    GESDFBorderParams GetTestOutlineParams() const
    {
        GESDFBorderParams params;
        params.color = Color::COLOR_BLUE;
        params.width = 10.0f;
        params.isOutline = true;
        return params;
    }
};

/**
 * @tc.name: GetSDFBorderEffect_Inner_001
 * @tc.desc: isOutline=false Test
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Inner_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Inner_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Outline_001
 * @tc.desc: isOutline=true Test
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Outline_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestOutlineParams();
    params.border.isOutline = true;

    GESDFBorderShader borderShader(params);
    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Outline_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Cache_Inner_001
 * @tc.desc: Validate repeated calls return the same inner border cache instance
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Cache_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Inner_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    auto effect1 = borderShader.GetSDFBorderEffect();
    auto effect2 = borderShader.GetSDFBorderEffect();
    EXPECT_EQ(effect1, effect2);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Inner_001 end";
}

/**
 * @tc.name: GetSDFBorderEffect_Cache_Outline_001
 * @tc.desc: Validate repeated calls return the same outer border cache instance
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, GetSDFBorderEffect_Cache_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Outline_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestOutlineParams();
    params.border.isOutline = true;

    GESDFBorderShader borderShader(params);
    auto effect1 = borderShader.GetSDFBorderEffect();
    auto effect2 = borderShader.GetSDFBorderEffect();
    EXPECT_EQ(effect1, effect2);
    GTEST_LOG_(INFO) << "GetSDFBorderEffect_Cache_Outline_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_Inner_001
 * @tc.desc: Validate inner border shader generation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_Inner_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Inner_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Inner_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_Outline_001
 * @tc.desc: Validate outer border shader generation
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_Outline_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Outline_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestOutlineParams();
    params.border.isOutline = true;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_Outline_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_NullShape_001
 * @tc.desc: Validate null shape handling
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_NullShape_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_NullShape_001 start";
    GESDFBorderShaderParams params;
    params.shape = nullptr;
    params.border = GetTestBorderParams();

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_NullShape_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_ZeroWidth_001
 * @tc.desc: Validate zero width rect handling
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_ZeroWidth_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroWidth_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 0.0f, 300.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroWidth_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_ZeroHeight_001
 * @tc.desc: Validate zero height rect handling
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_ZeroHeight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroHeight_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 0.0f};
    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_ZeroHeight_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_InnerBorder_001
 * @tc.desc: Validate inner border full process
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_InnerBorder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_InnerBorder_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestBorderParams();
    params.border.isOutline = false;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 300.0f};

    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);

    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_InnerBorder_001 end";
}

/**
 * @tc.name: MakeSDFBorderShader_OutlineBorder_001
 * @tc.desc: Validate outer border full process
 * @tc.type: FUNC
 */
HWTEST_F(GESDFBorderShaderTest, MakeSDFBorderShader_OutlineBorder_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_OutlineBorder_001 start";
    GESDFBorderShaderParams params;
    params.shape = CreateTestShape();
    params.border = GetTestOutlineParams();
    params.border.isOutline = true;

    GESDFBorderShader borderShader(params);
    Rect rect {0, 0, 300.0f, 300.0f};

    auto effect = borderShader.GetSDFBorderEffect();
    EXPECT_NE(effect, nullptr);

    auto shader = borderShader.MakeSDFBorderShader(rect);
    EXPECT_NE(shader, nullptr);
    GTEST_LOG_(INFO) << "MakeSDFBorderShader_OutlineBorder_001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
