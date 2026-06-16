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

#include "ge_sdf_clip_shader.h"
#include "ge_sdf_rrect_shader_shape.h"

#include "draw/color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESDFClipShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    static inline Drawing::Canvas canvas_;
};

/**
 * @tc.name: MakeSDFClipShaderTest
 * @tc.desc: test MakeSDFClipShader by sdfShape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFClipShaderTest, MakeSDFClipShaderTest, TestSize.Level1)
{
    Drawing::GESDFClipShaderParams params;
    GESDFClipShader clipShader(params);

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    auto shader = clipShader.MakeSDFClipShader(canvas_, rect);
    EXPECT_EQ(shader, nullptr);

    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    rectShapeParams.rrect.SetCornerRadius(10.0f, 10.0f);
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    params.shape = sdfShape;
    clipShader.SetSDFClipParams(params);
    shader = clipShader.MakeSDFClipShader(canvas_, rect);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: OnDrawShaderRingDrawRegion
 * @tc.desc: Verify OnDrawShader takes ring draw path when shape supports GetInscribedRect
 * @tc.type: FUNC
 */
HWTEST_F(GESDFClipShaderTest, OnDrawShaderRingDrawRegion, TestSize.Level1)
{
    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f}};
    rectShapeParams.rrect.SetCornerRadius(10.0f, 10.0f);
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    Drawing::GESDFClipShaderParams params;
    params.shape = sdfShape; // RRect supports GetInscribedRect → ring draw path
    GESDFClipShader clipShader(params);
    Drawing::Rect rect {1.0f, 1.0f, 200.0f, 200.0f};
    clipShader.OnDrawShader(canvas_, rect);
    EXPECT_NE(clipShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: OnDrawShaderDrawRectFallback
 * @tc.desc: Verify OnDrawShader falls back to DrawRect when shape is null or does not support
 * @tc.type: FUNC
 */
HWTEST_F(GESDFClipShaderTest, OnDrawShaderDrawRectFallback, TestSize.Level1)
{
    Drawing::GESDFClipShaderParams params; // shape null → else path (DrawRect fallback)
    GESDFClipShader clipShader(params);
    Drawing::Rect rect {1.0f, 1.0f, 200.0f, 200.0f};
    clipShader.OnDrawShader(canvas_, rect);
    EXPECT_EQ(clipShader.GetDrawingShader(), nullptr);
}
} // namespace Rosen
} // namespace OHOS