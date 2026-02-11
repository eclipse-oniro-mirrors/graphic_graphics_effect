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
    auto shader = clipShader.MakeSDFClipShader(rect);
    EXPECT_EQ(shader, nullptr);

    Drawing::GESDFRRectShapeParams rectShapeParams {{1.0f, 1.0f, 200.0f, 200.0f, 10.0f, 10.0f}};
    auto sdfShape = std::make_shared<Drawing::GESDFRRectShaderShape>(rectShapeParams);
    params.shape = sdfShape;
    clipShader.SetSDFClipParams(params);
    shader = clipShader.MakeSDFClipShader(rect);
    EXPECT_EQ(shader, nullptr);
}
} // namespace Rosen
} // namespace OHOS