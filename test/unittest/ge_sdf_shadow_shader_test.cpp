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

#include "sdf/ge_sdf_shadow_shader.h"
#include "sdf/ge_sdf_rrect_shader_shape.h"

#include "draw/color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESDFShadowShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    static inline Drawing::Canvas canvas_;
};

/**
 * @tc.name: MakeSDFShadowShaderTest
 * @tc.desc: test MakeSDFShadowShader by sdfShape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, MakeSDFShadowShaderTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    GESDFShadowShader shadowShader(shadowParams);

    Drawing::Rect rect0;
    auto shader = shadowShader.MakeSDFShadowShader(rect0);
    EXPECT_EQ(shader, nullptr);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect1 { 1.0f, 1.0f, 2.0f, 2.0f };
    shader = shadowShader.MakeSDFShadowShader(rect1);
    EXPECT_EQ(shader, nullptr);

    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f, 10.0f, 10.0f}};
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    shadowParams.shape = sdfShape;
    shadowShader.SetSDFShadowParams(shadowParams);
    shader = shadowShader.MakeSDFShadowShader(rect1);
    EXPECT_EQ(shader, nullptr);

    shadowParams.shadow.color = Drawing::Color::COLOR_GRAY;
    shadowShader.SetSDFShadowParams(shadowParams);
    shader = shadowShader.MakeSDFShadowShader(rect1);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: UpdateRectForShadowTest
 * @tc.desc: test UpdateRectForShadow by shadow Params
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShadowShaderTest, UpdateRectForShadowTest, TestSize.Level1)
{
    Drawing::GESDFShadowShaderParams shadowParams;
    shadowParams.shadow.radius = 10.0f; // 10.0f: set radius > 0
    GESDFShadowShader shadowShader(shadowParams);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_NE(rect, newRect);
    EXPECT_EQ(newRect.GetLeft(), -14.0f);
    EXPECT_EQ(newRect.GetRight(), 17.0f);
    EXPECT_EQ(newRect.GetTop(), -14.0f);
    EXPECT_EQ(newRect.GetBottom(), 17.0f);

    shadowParams.shadow.offsetX = 5.0f; // 5.0f: set offsetX > 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_EQ(newRect.GetLeft(), -9.0f);
    EXPECT_EQ(newRect.GetRight(), 22.0f);

    shadowParams.shadow.offsetY = -5.0f; // -5.0f: set offsetY < 0
    shadowShader.SetSDFShadowParams(shadowParams);
    newRect = rect;
    shadowShader.UpdateRectForShadow(newRect);
    EXPECT_EQ(newRect.GetTop(), -19.0f);
    EXPECT_EQ(newRect.GetBottom(), 12.0f);
}
} // namespace Rosen
} // namespace OHOS