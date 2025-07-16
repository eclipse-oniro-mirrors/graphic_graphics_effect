/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_contour_diagonal_flow_light_shader.h"
#include "ge_log.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "utils/rect.h"
#include "draw/color.h"
#include "draw/path.h"
#include "ge_shader_filter_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {

class GEContourDiagonalFlowLightShaderTest : public Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {}
    void TearDown() override
    {}

    static inline Canvas canvas_;
};

HWTEST_F(GEContourDiagonalFlowLightShaderTest, CreateContourDiagonalFlowLightShader_001, TestSize.Level1)
{
    // Test case for static CreateContourDiagonalFlowLightShader method
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.0f;
    params.line1Length_ = 0.3f;
    params.line1Color_ = Vector4f(0.1f, 0.2f, 0.3f, 0.4f);
    params.line2Start_ = 0.5f;
    params.line2Length_ = 0.3f;
    params.line2Color_ = Vector4f(0.5f, 0.6f, 0.7f, 0.8f);
    params.thickness_ = 0.02f;

    auto shader = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(params);
    EXPECT_NE(shader, nullptr);
}

HWTEST_F(GEContourDiagonalFlowLightShaderTest, GEContourDiagonalFlowLightShaderConstructor_001, TestSize.Level1)
{
    // Test case for constructor with parameters
    GEContentDiagonalFlowLightShaderParams testParams;
    testParams.line1Start_ = 0.2f;
    testParams.line1Length_ = 0.3f;
    testParams.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    testParams.line2Start_ = 0.7f;
    testParams.line2Length_ = 0.5f;
    testParams.line2Color_ = Vector4f(0.5f, 0.6f, 0.7f, 0.8f);
    testParams.thickness_ = 0.2f;

    GEContourDiagonalFlowLightShader shader(testParams);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Start_, testParams.line1Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Length_, testParams.line1Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[0], testParams.line1Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[1], testParams.line1Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[2], testParams.line1Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line1Color_[3], testParams.line1Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Start_, testParams.line2Start_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Length_, testParams.line2Length_);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[0], testParams.line2Color_[0]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[1], testParams.line2Color_[1]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[2], testParams.line2Color_[2]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.line2Color_[3], testParams.line2Color_[3]);
    EXPECT_FLOAT_EQ(shader.contourDiagonalFlowLightParams_.thickness_, testParams.thickness_);
}
HWTEST_F(GEContourDiagonalFlowLightShaderTest, MakeDrawingShader_001, TestSize.Level1)
{
    // Test case for MakeDrawingShader method
    Drawing::Rect rect(0, 0, 100, 100);
    float progress = 0.5f;
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.5f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.0f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.35f, 0.8f);
    params.thickness_ = 0.25f;
    auto shader = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(params);
    shader->MakeDrawingShader(rect, progress);
    EXPECT_NE(shader->GetDrawingShader(), nullptr);
}

HWTEST_F(GEContourDiagonalFlowLightShaderTest, Preprocess_001, TestSize.Level1)
{
    // Test case for Preprocess method
    Drawing::Rect rect(0, 0, 100, 100);
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.1f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.6f, 0.3f, 0.9f, 1.0f);
    params.line2Start_ = 0.6f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.67f, 0.24f, 0.9f, 0.8f);
    params.thickness_ = 0.25f;
    auto shader = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(params);
    shader->Preprocess(canvas_, rect);
    auto cache = shader->GetCache();
    EXPECT_NE(cache, nullptr);
}

HWTEST_F(GEContourDiagonalFlowLightShaderTest, GetContourDiagonalFlowLightBuilder_001, TestSize.Level1)
{
    // Test case for GetContourDiagonalFlowLightBuilder method
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.2f;
    params.line1Length_ = 0.8f;
    params.line1Color_ = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
    params.line2Start_ = 0.7f;
    params.line2Length_ = 0.3f;
    params.line2Color_ = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
    params.thickness_ = 0.4f;
    auto shader = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(params);
    auto builder = shader->GetContourDiagonalFlowLightBuilder();
    EXPECT_NE(builder, nullptr);
}

HWTEST_F(GEContourDiagonalFlowLightShaderTest, DrawRuntimeShader_001, TestSize.Level1)
{
    // Test case for MakeContourDiagonalFlowLightShader method
    Drawing::Rect rect(0, 0, 100, 100);
    GEContentDiagonalFlowLightShaderParams params;
    params.line1Start_ = 0.05f;
    params.line1Length_ = 0.6f;
    params.line1Color_ = Vector4f(0.8f, 0.5f, 1.0f, 1.0f);
    params.line2Start_ = 0.55f;
    params.line2Length_ = 0.2f;
    params.line2Color_ = Vector4f(0.2f, 0.6f, 1.0f, 0.8f);
    params.thickness_ = 0.5f;
    auto shader = GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(params);
    auto img = shader->DrawRuntimeShader(canvas_, rect);
    EXPECT_NE(img, nullptr);
}
}
}