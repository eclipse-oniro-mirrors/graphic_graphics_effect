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
#include "ge_border_sdf_shader.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBorderSDFShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    static inline Drawing::Canvas canvas_;

    std::shared_ptr<Drawing::GESDFShaderShape> CreateTestShape() const
    {
        Drawing::GESDFRRectShapeParams param;
        param.rrect = {10.0f, 10.0f, 200.0f, 200.0f};
        param.rrect.SetCornerRadius(20.0f, 20.0f);
        return std::make_shared<Drawing::GESDFRRectShaderShape>(param);
    }

    Drawing::GEBorderSDFShaderParams GetTestParams() const
    {
        Drawing::GEBorderSDFShaderParams params;
        params.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
        params.width = 10.0f;
        params.isOutline = false;
        params.style = 0;
        params.dashWidth = 0.0f;
        params.dashGap = 0.0f;
        params.shape = CreateTestShape();
        return params;
    }
};

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest001, TestSize.Level1)
{
    auto shader = std::make_unique<GEBorderSDFShader>();
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::BORDER_SDF_SHADER);
    EXPECT_EQ(shader->TypeName(), "BorderSDFShader");
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest002, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = std::make_unique<GEBorderSDFShader>(params);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::BORDER_SDF_SHADER);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest003, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest004, TestSize.Level1)
{
    auto params = GetTestParams();
    params.isOutline = true;
    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest005, TestSize.Level1)
{
    Drawing::GEBorderSDFShaderParams params;
    params.color = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    params.width = 0.0f;
    params.isOutline = false;
    params.style = 0;
    params.shape = CreateTestShape();

    auto shaderZero = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shaderZero.MakeDrawingShader(canvas_, rect, 0.5f);

    Drawing::GEBorderSDFShaderParams paramsLarge;
    paramsLarge.color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    paramsLarge.width = 4096.0f;
    paramsLarge.isOutline = false;
    paramsLarge.style = 0;
    paramsLarge.shape = CreateTestShape();

    auto shaderLarge = GEBorderSDFShader(paramsLarge);
    shaderLarge.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shaderLarge.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest006, TestSize.Level1)
{
    Drawing::GEBorderSDFShaderParams params;
    params.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params.width = 10.0f;
    params.isOutline = false;
    params.style = 1;
    params.shape = CreateTestShape();

    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest007, TestSize.Level1)
{
    auto params = GetTestParams();

    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 10, 10),
        Drawing::Rect(0, 0, 50, 50),
        Drawing::Rect(0, 0, 100, 200),
        Drawing::Rect(0, 0, 500, 500)
    };
    for (const auto& rect : rects) {
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest008, TestSize.Level1)
{
    Vector4f colors[] = {
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f)
    };
    Drawing::Rect rect(0, 0, 100, 100);
    for (const auto& color : colors) {
        auto params = GetTestParams();
        params.color = color;
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest009, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest010, TestSize.Level1)
{
    auto params = GetTestParams();
    params.isOutline = false;

    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest011, TestSize.Level1)
{
    constexpr size_t ARRAY_SIZE = 6;
    float widthValues[ARRAY_SIZE] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
    Drawing::Rect rect(0, 0, 100, 100);

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        auto params = GetTestParams();
        params.width = widthValues[i];
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest012, TestSize.Level1)
{
    Vector4f colors[] = {
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.5f, 0.5f, 0.5f, 0.5f),
        Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(1.0f, 0.5f, 0.25f, 0.75f)
    };
    Drawing::Rect rect(0, 0, 100, 100);

    for (const auto& color : colors) {
        auto params = GetTestParams();
        params.color = color;
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest013, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect1(0, 0, 50, 50);
    Drawing::Rect rect2(0, 0, 100, 100);
    Drawing::Rect rect3(0, 0, 200, 200);

    shader.MakeDrawingShader(canvas_, rect1, 0.5f);
    shader.MakeDrawingShader(canvas_, rect2, 0.75f);
    shader.MakeDrawingShader(canvas_, rect3, 1.0f);

    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest014, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 100, 100);

    constexpr size_t CALL_COUNT = 6;
    for (size_t i = 0; i < CALL_COUNT; i++) {
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest015, TestSize.Level1)
{
    auto params = GetTestParams();

    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 10, 100),
        Drawing::Rect(0, 0, 100, 10),
        Drawing::Rect(0, 0, 200, 100),
        Drawing::Rect(0, 0, 100, 200),
        Drawing::Rect(0, 0, 1, 1000),
        Drawing::Rect(0, 0, 1000, 1)
    };

    for (const auto& rect : rects) {
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest016, TestSize.Level1)
{
    auto params1 = GetTestParams();
    params1.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params1.width = 5.0f;
    auto shader1 = GEBorderSDFShader(params1);
    Drawing::Rect rect(0, 0, 100, 100);
    shader1.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader1.GetDrawingShader(), nullptr);

    auto params2 = GetTestParams();
    params2.color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    params2.width = 15.0f;
    params2.isOutline = true;
    auto shader2 = GEBorderSDFShader(params2);
    shader2.MakeDrawingShader(canvas_, rect, 0.75f);
    EXPECT_NE(shader2.GetDrawingShader(), nullptr);

    auto params3 = GetTestParams();
    params3.color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    params3.width = 20.0f;
    auto shader3 = GEBorderSDFShader(params3);
    shader3.MakeDrawingShader(canvas_, rect, 1.0f);
    EXPECT_NE(shader3.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest017, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFShader(params);

    shader.MakeDrawingShader(Drawing::Rect(0, 0, 50, 50), 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);

    shader.MakeDrawingShader(Drawing::Rect(0, 0, 100, 100), 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);

    shader.MakeDrawingShader(Drawing::Rect(0, 0, 200, 200), 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest018, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);

    bool isOutlineValues[] = {false, true};
    for (bool isOutline : isOutlineValues) {
        auto params = GetTestParams();
        params.isOutline = isOutline;
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest019, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);

    float dashConfigs[][2] = {
        {0.0f, 0.0f},
        {5.0f, 3.0f},
        {10.0f, 5.0f},
        {20.0f, 10.0f},
        {1.0f, 100.0f},
    };
    for (const auto& config : dashConfigs) {
        auto params = GetTestParams();
        params.dashWidth = config[0];
        params.dashGap = config[1];
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest020, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);

    float widthValues[] = {0.0f, 0.0001f, 0.001f, 0.01f, 0.1f, 1.0f};
    for (float w : widthValues) {
        auto params = GetTestParams();
        params.width = w;
        auto shader = GEBorderSDFShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        if (w > 0.0001f) { // 0.0001 width size smallest
            EXPECT_NE(shader.GetDrawingShader(), nullptr);
        } else {
            EXPECT_EQ(shader.GetDrawingShader(), nullptr);
        }
    }
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest021, TestSize.Level1)
{
    Drawing::GEBorderSDFShaderParams params;
    params.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params.width = 10.0f;
    params.isOutline = false;
    params.style = 0;

    auto shader = GEBorderSDFShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFShaderTest, GEBorderSDFShaderTest022, TestSize.Level1)
{
    auto shader = GEBorderSDFShader();
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

}  // namespace Rosen
}  // namespace OHOS
