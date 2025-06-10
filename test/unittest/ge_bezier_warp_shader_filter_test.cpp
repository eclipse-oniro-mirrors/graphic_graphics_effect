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

#include "ge_bezier_warp_shader_filter.h"
#include "ge_visual_effect_impl.h"
#include "ge_external_dynamic_loader.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEBezierWarpShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };

    const int imageWidth_  = 50;
    const int imageHeight_ = 50;
};

void GEBezierWarpShaderFilterTest::SetUpTestCase(void) {}

void GEBezierWarpShaderFilterTest::TearDownTestCase(void) {}

void GEBezierWarpShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEBezierWarpShaderFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: ProcessImage_001
 * @tc.desc: Verify the ProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEBezierWarpShaderFilterTest, ProcessImage_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEBezierWarpShaderFilterTest ProcessImage_001 start";

    Drawing::GEBezierWarpShaderFilterParams geBezierWarpShaderFilterParams;
    auto geBezierWarpShaderFilter = std::make_unique<GEBezierWarpShaderFilter>(geBezierWarpShaderFilterParams);
    EXPECT_EQ(geBezierWarpShaderFilter->ProcessImage(canvas_, image_, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEBezierWarpShaderFilterTest ProcessImage_001 end";
}

/**
 * @tc.name: InitCtrlPoints_001
 * @tc.desc: Verify the InitCtrlPoints function
 * @tc.type: FUNC
 */
HWTEST_F(GEBezierWarpShaderFilterTest, InitCtrlPoints_001, TestSize.Level1)
{
    constexpr size_t BOTTOM_RIGHT_INDEX = 6;
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points;
    Drawing::GEBezierWarpShaderFilterParams geBezierWarpShaderFilterParams;
    auto geBezierWarpShaderFilter = std::make_unique<GEBezierWarpShaderFilter>(geBezierWarpShaderFilterParams);
    geBezierWarpShaderFilter->InitCtrlPoints(points);
    EXPECT_EQ(points[BOTTOM_RIGHT_INDEX].GetX(), 1.0f);
    EXPECT_EQ(points[BOTTOM_RIGHT_INDEX].GetY(), 1.0f);
}

/**
 * @tc.name: GetBrush_001
 * @tc.desc: Verify the GetBrush function
 * @tc.type: FUNC
 */
HWTEST_F(GEBezierWarpShaderFilterTest, GetBrush_001, TestSize.Level1)
{
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points;
    Drawing::GEBezierWarpShaderFilterParams geBezierWarpShaderFilterParams;
    auto geBezierWarpShaderFilter = std::make_unique<GEBezierWarpShaderFilter>(geBezierWarpShaderFilterParams);
    auto brush1 = geBezierWarpShaderFilter->GetBrush(nullptr);
    EXPECT_EQ(brush1.shaderEffect_, nullptr);
    auto brush2 = geBezierWarpShaderFilter->GetBrush(image_);
    EXPECT_NE(brush2.shaderEffect_, nullptr);
}

/**
 * @tc.name: SetPathTo_001
 * @tc.desc: Verify the SetPathTo function
 * @tc.type: FUNC
 */
HWTEST_F(GEBezierWarpShaderFilterTest, SetPathTo_001, TestSize.Level1)
{
    Drawing::Path path;
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> bezierPatch = {{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {11.0f, 12.0f},
        {13.0f, 14.0f},
        {15.0f, 16.0f},
        {17.0f, 18.0f},
        {19.0f, 20.0f},
        {21.0f, 22.0f},
        {23.0f, 24.0f}
    }};
    Drawing::GEBezierWarpShaderFilterParams geBezierWarpShaderFilterParams;
    auto geBezierWarpShaderFilter = std::make_unique<GEBezierWarpShaderFilter>(geBezierWarpShaderFilterParams);
    geBezierWarpShaderFilter->SetPathTo(path, bezierPatch);
    EXPECT_NE(path.GetLength(true), 0.0f);
}
} // namespace Rosen
} // namespace OHOS
