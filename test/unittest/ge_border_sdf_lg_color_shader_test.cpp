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
#include "ge_border_sdf_lg_color_shader.h"
#include "ge_sdf_empty_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBorderSDFLGColorShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

    std::shared_ptr<Drawing::GESDFShaderShape> CreateTestShape() const
    {
        Drawing::GESDFRRectShapeParams param;
        param.rrect = {10.0f, 10.0f, 200.0f, 200.0f};
        param.rrect.SetCornerRadius(20.0f, 20.0f);
        return std::make_shared<Drawing::GESDFRRectShaderShape>(param);
    }

    Drawing::GEBorderSDFLGColorShaderParams GetTestParams() const
    {
        Drawing::GEBorderSDFLGColorShaderParams params;
        params.angle = 45.0f;
        params.width = 10.0f;
        params.isOutline = false;
        params.colorNumber = 2;
        params.color0 = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
        params.color1 = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
        params.position0 = 0.0f;
        params.position1 = 1.0f;
        params.shape = CreateTestShape();
        return params;
    }

    static inline Drawing::Canvas canvas_;
};

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest001, TestSize.Level1)
{
    auto shader = std::make_unique<GEBorderSDFLGColorShader>();
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::BORDER_SDF_LG_COLOR);
    EXPECT_EQ(shader->TypeName(), "BorderSDFLGColor");
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest002, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = std::make_unique<GEBorderSDFLGColorShader>(params);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->Type(), Drawing::GEFilterType::BORDER_SDF_LG_COLOR);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest003, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest004, TestSize.Level1)
{
    auto params = GetTestParams();
    params.isOutline = true;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.Preprocess(canvas_, rect);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest005, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest006, TestSize.Level1)
{
    Drawing::GEBorderSDFLGColorShaderParams params;
    params.colorNumber = 2;
    params.position0 = 0.0f;
    params.position1 = 1.0f;
    params.width = 0.0f;
    params.shape = CreateTestShape();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest007, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 3;
    params.color0 = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params.position0 = 0.0f;
    params.color1 = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    params.position1 = 0.5f;
    params.color2 = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    params.position2 = 1.0f;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest008, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 5;
    params.position0 = 0.0f;
    params.position1 = 0.25f;
    params.position2 = 0.5f;
    params.position3 = 0.75f;
    params.position4 = 1.0f;
    params.color0 = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params.color1 = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    params.color2 = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    params.color3 = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
    params.color4 = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest009, TestSize.Level1)
{
    auto params = GetTestParams();
    params.position0 = 0.5f;
    params.position1 = 0.3f;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest010, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 3;
    params.position0 = 0.0f;
    params.position1 = 0.5f;
    params.position2 = 0.3f;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest011, TestSize.Level1)
{
    auto params = GetTestParams();
    params.position0 = 0.3f;
    params.position1 = 0.3f;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest012, TestSize.Level1)
{
    auto params = GetTestParams();

    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 10, 10),
        Drawing::Rect(0, 0, 50, 50),
        Drawing::Rect(0, 0, 100, 200),
        Drawing::Rect(0, 0, 500, 500)
    };
    for (const auto& rect : rects) {
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest013, TestSize.Level1)
{
    float angles[] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 270.0f, 360.0f};
    Drawing::Rect rect(0, 0, 100, 100);
    for (float angle : angles) {
        auto params = GetTestParams();
        params.angle = angle;
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest014, TestSize.Level1)
{
    float widthValues[] = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f};
    Drawing::Rect rect(0, 0, 100, 100);
    for (float w : widthValues) {
        auto params = GetTestParams();
        params.width = w;
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest015, TestSize.Level1)
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
        params.color0 = color;
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest016, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect1(0, 0, 50, 50);
    Drawing::Rect rect2(0, 0, 100, 100);
    Drawing::Rect rect3(0, 0, 200, 200);

    shader.MakeDrawingShader(canvas_, rect1, 0.5f);
    shader.MakeDrawingShader(canvas_, rect2, 0.75f);
    shader.MakeDrawingShader(canvas_, rect3, 1.0f);

    auto drawingShader = shader.GetDrawingShader();
    EXPECT_NE(drawingShader, nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest017, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);

    constexpr size_t CALL_COUNT = 6;
    for (size_t i = 0; i < CALL_COUNT; i++) {
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest018, TestSize.Level1)
{
    auto params = GetTestParams();
    Drawing::Rect rect(0, 0, 100, 100);

    bool isOutlineValues[] = {false, true};
    for (bool isOutline : isOutlineValues) {
        auto p = params;
        p.isOutline = isOutline;
        auto shader = GEBorderSDFLGColorShader(p);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest019, TestSize.Level1)
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
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest020, TestSize.Level1)
{
    float widthValues[] = {0.0f, 0.0001f, 0.001f, 0.01f, 0.1f, 1.0f};
    Drawing::Rect rect(0, 0, 100, 100);
    for (float w : widthValues) {
        auto params = GetTestParams();
        params.width = w;
        auto shader = GEBorderSDFLGColorShader(params);
        shader.MakeDrawingShader(canvas_, rect, 0.5f);
        EXPECT_NE(shader.GetDrawingShader(), nullptr);
    }
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest021, TestSize.Level1)
{
    auto params1 = GetTestParams();
    params1.color0 = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params1.color1 = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
    params1.width = 5.0f;
    auto shader1 = GEBorderSDFLGColorShader(params1);
    Drawing::Rect rect(0, 0, 100, 100);
    shader1.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader1.GetDrawingShader(), nullptr);

    auto params2 = GetTestParams();
    params2.color0 = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
    params2.color1 = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    params2.width = 15.0f;
    params2.isOutline = true;
    auto shader2 = GEBorderSDFLGColorShader(params2);
    shader2.MakeDrawingShader(canvas_, rect, 0.75f);
    EXPECT_NE(shader2.GetDrawingShader(), nullptr);

    auto params3 = GetTestParams();
    params3.colorNumber = 4;
    params3.position0 = 0.0f;
    params3.position1 = 0.33f;
    params3.position2 = 0.66f;
    params3.position3 = 1.0f;
    params3.width = 20.0f;
    auto shader3 = GEBorderSDFLGColorShader(params3);
    shader3.MakeDrawingShader(canvas_, rect, 1.0f);
    EXPECT_NE(shader3.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest022, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);

    shader.MakeDrawingShader(canvas_, Drawing::Rect(0, 0, 50, 50), 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);

    shader.MakeDrawingShader(canvas_, Drawing::Rect(0, 0, 100, 100), 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);

    shader.MakeDrawingShader(canvas_, Drawing::Rect(0, 0, 200, 200), 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest023, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 2;
    params.color2 = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    params.position2 = 0.5f;
    params.color3 = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
    params.position3 = 0.7f;
    params.color4 = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
    params.position4 = 0.9f;

    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.Preprocess(canvas_, rect);

    EXPECT_EQ(shader.params_.color2.x_, 0.0f);
    EXPECT_EQ(shader.params_.position2, 0.0f);
    EXPECT_EQ(shader.params_.color3.x_, 0.0f);
    EXPECT_EQ(shader.params_.position3, 0.0f);
    EXPECT_EQ(shader.params_.color4.x_, 0.0f);
    EXPECT_EQ(shader.params_.position4, 0.0f);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest024, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 3;
    params.position0 = 0.0f;
    params.position1 = 0.5f;
    params.position2 = 1.0f;
    params.color3 = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    params.position3 = 0.8f;
    params.color4 = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
    params.position4 = 0.9f;

    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.Preprocess(canvas_, rect);

    EXPECT_EQ(shader.params_.color3.x_, 0.0f);
    EXPECT_EQ(shader.params_.position3, 0.0f);
    EXPECT_EQ(shader.params_.color4.x_, 0.0f);
    EXPECT_EQ(shader.params_.position4, 0.0f);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest025, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 4;
    params.position0 = 0.0f;
    params.position1 = 0.33f;
    params.position2 = 0.66f;
    params.position3 = 1.0f;
    params.color4 = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    params.position4 = 0.9f;

    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.Preprocess(canvas_, rect);

    EXPECT_EQ(shader.params_.color4.x_, 0.0f);
    EXPECT_EQ(shader.params_.position4, 0.0f);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest026, TestSize.Level1)
{
    auto params = GetTestParams();
    params.colorNumber = 5;
    params.position0 = 0.0f;
    params.position1 = 0.25f;
    params.position2 = 0.5f;
    params.position3 = 0.75f;
    params.position4 = 1.0f;

    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 300, 300);
    shader.Preprocess(canvas_, rect);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}


HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest_OnDrawShader001, TestSize.Level1)
{
    auto params = GetTestParams();
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.OnDrawShader(canvas_, rect);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest_OnDrawShader002, TestSize.Level1)
{
    auto params = GetTestParams();
    params.isOutline = true;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.OnDrawShader(canvas_, rect);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest_OnDrawShader003, TestSize.Level1)
{
    auto params = GetTestParams();
    params.width = 0.0f;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.OnDrawShader(canvas_, rect);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

HWTEST_F(GEBorderSDFLGColorShaderTest, GEBorderSDFLGColorShaderTest027, TestSize.Level1)
{
    auto params = GetTestParams();
    auto emptyShape = std::make_shared<Drawing::GESDFEmptyShaderShape>();
    params.shape = emptyShape;
    auto shader = GEBorderSDFLGColorShader(params);
    Drawing::Rect rect(0, 0, 100, 100);
    shader.MakeDrawingShader(canvas_, rect, 0.5f);
    EXPECT_EQ(shader.GetDrawingShader(), nullptr);
}

}  // namespace Rosen
}  // namespace OHOS