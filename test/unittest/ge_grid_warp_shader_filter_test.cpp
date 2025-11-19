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

#include "ge_grid_warp_shader_filter.h"
#include "ge_visual_effect_impl.h"
#include "ge_external_dynamic_loader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEGridWarpShaderFilterTest : public testing::Test {
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

void GEGridWarpShaderFilterTest::SetUpTestCase(void) {}
void GEGridWarpShaderFilterTest::TearDownTestCase(void) {}

void GEGridWarpShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(imageWidth_, imageHeight_, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEGridWarpShaderFilterTest::TearDown()
{
    image_ = nullptr;
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify the OnProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEGridWarpShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEGridWarpShaderFilterTest OnProcessImage_001 start";

    Drawing::GEGridWarpShaderFilterParams geGridWarpShaderFilterParams;
    auto geGridWarpShaderFilter = std::make_unique<GEGridWarpShaderFilter>(geGridWarpShaderFilterParams);
    EXPECT_EQ(geGridWarpShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEGridWarpShaderFilterTest OnProcessImage_001 end";
}

/**
 * @tc.name: GetBrush_001
 * @tc.desc: Verify the GetBrush function
 * @tc.type: FUNC
 */
HWTEST_F(GEGridWarpShaderFilterTest, GetBrush_001, TestSize.Level1)
{
    Drawing::GEGridWarpShaderFilterParams geGridWarpShaderFilterParams;
    auto geGridWarpShaderFilter = std::make_unique<GEGridWarpShaderFilter>(geGridWarpShaderFilterParams);
    auto brush1 = geGridWarpShaderFilter->GetBrush(nullptr);
    EXPECT_EQ(brush1.shaderEffect_, nullptr);
    auto brush2 = geGridWarpShaderFilter->GetBrush(image_);
    EXPECT_NE(brush2.shaderEffect_, nullptr);
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify the constructor with specific ControlPoints values
 * @tc.type: FUNC
 */
HWTEST_F(GEGridWarpShaderFilterTest, Constructor_001, TestSize.Level1)
{
    size_t pointsNum = GEGridWarpShaderFilter::GRID_WARP_MAIN_POINT_NUM;
    Drawing::GEGridWarpShaderFilterParams params;
    for (size_t i = 0; i < pointsNum; ++i) {
        params.gridPoints[i].first = (i % 3) * 0.5f;
        params.gridPoints[i].second = (i / 3) * 0.5f;
    }
    for (size_t i = 0; i < pointsNum; ++i) {
        params.rotationAngles[i].first = 0.0f;
        params.rotationAngles[i].second = 0.0f;
    }

    GEGridWarpShaderFilter filter(params);
    const float pointsDistance = 1.0f / 6.0f;

    std::array<Drawing::Point, GEGridWarpShaderFilter::BEZIER_WARP_POINT_NUM> bezierPatch0 = {
        Drawing::Point{0.0f, 0.0f}, Drawing::Point{pointsDistance, 0.0f}, Drawing::Point{0.5f - pointsDistance, 0.0f},
        Drawing::Point{0.5f, 0.0f}, Drawing::Point{0.5f, pointsDistance}, Drawing::Point{0.5f, 0.5f - pointsDistance},
        Drawing::Point{0.5f, 0.5f}, Drawing::Point{0.5f - pointsDistance, 0.5f}, Drawing::Point{pointsDistance, 0.5f},
        Drawing::Point{0.0f, 0.5f}, Drawing::Point{0.0f, 0.5f - pointsDistance}, Drawing::Point{0.0f, pointsDistance}
    };

    for (size_t i = 0; i < GEGridWarpShaderFilter::BEZIER_WARP_POINT_NUM; i++) {
        EXPECT_NEAR(filter.bezierPatch_[0][i].GetX(), bezierPatch0[i].GetX(), 1e-6);
        EXPECT_NEAR(filter.bezierPatch_[0][i].GetY(), bezierPatch0[i].GetY(), 1e-6);
    }
}

/**
 * @tc.name: CalcTexCoords_001
 * @tc.desc: Verify the CalcTexCoords function with given image dimensions
 * @tc.type: FUNC
 */
HWTEST_F(GEGridWarpShaderFilterTest, CalcTexCoords_001, TestSize.Level1)
{
    const int testImageWidth = 100;
    const int testImageHeight = 100;

    Drawing::GEGridWarpShaderFilterParams params;
    GEGridWarpShaderFilter filter(params);
    auto texCoords = filter.CalcTexCoords(testImageWidth, testImageHeight);

    EXPECT_EQ(texCoords[0][0].GetX(), 0.0f);
    EXPECT_EQ(texCoords[0][0].GetY(), 0.0f);
    EXPECT_EQ(texCoords[0][1].GetX(), 50.0f);
    EXPECT_EQ(texCoords[0][1].GetY(), 0.0f);
    EXPECT_EQ(texCoords[0][2].GetX(), 50.0f);
    EXPECT_EQ(texCoords[0][2].GetY(), 50.0f);
    EXPECT_EQ(texCoords[0][3].GetX(), 0.0f);
    EXPECT_EQ(texCoords[0][3].GetY(), 50.0f);

    EXPECT_EQ(texCoords[1][0].GetX(), 50.0f);
    EXPECT_EQ(texCoords[1][0].GetY(), 0.0f);
    EXPECT_EQ(texCoords[1][1].GetX(), 100.0f);
    EXPECT_EQ(texCoords[1][1].GetY(), 0.0f);
    EXPECT_EQ(texCoords[1][2].GetX(), 100.0f);
    EXPECT_EQ(texCoords[1][2].GetY(), 50.0f);
    EXPECT_EQ(texCoords[1][3].GetX(), 50.0f);
    EXPECT_EQ(texCoords[1][3].GetY(), 50.0f);

    EXPECT_EQ(texCoords[2][0].GetX(), 0.0f);
    EXPECT_EQ(texCoords[2][0].GetY(), 50.0f);
    EXPECT_EQ(texCoords[2][1].GetX(), 50.0f);
    EXPECT_EQ(texCoords[2][1].GetY(), 50.0f);
    EXPECT_EQ(texCoords[2][2].GetX(), 50.0f);
    EXPECT_EQ(texCoords[2][2].GetY(), 100.0f);
    EXPECT_EQ(texCoords[2][3].GetX(), 0.0f);
    EXPECT_EQ(texCoords[2][3].GetY(), 100.0f);

    EXPECT_EQ(texCoords[3][0].GetX(), 50.0f);
    EXPECT_EQ(texCoords[3][0].GetY(), 50.0f);
    EXPECT_EQ(texCoords[3][1].GetX(), 100.0f);
    EXPECT_EQ(texCoords[3][1].GetY(), 50.0f);
    EXPECT_EQ(texCoords[3][2].GetX(), 100.0f);
    EXPECT_EQ(texCoords[3][2].GetY(), 100.0f);
    EXPECT_EQ(texCoords[3][3].GetX(), 50.0f);
    EXPECT_EQ(texCoords[3][3].GetY(), 100.0f);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type function
 * @tc.type: FUNC
 */
HWTEST_F(GEGridWarpShaderFilterTest, Type_001, TestSize.Level2)
{
    Drawing::GEGridWarpShaderFilterParams geGridWarpShaderFilterParams;
    auto geGridWarpShaderFilter = std::make_unique<GEGridWarpShaderFilter>(geGridWarpShaderFilterParams);
    EXPECT_EQ(geGridWarpShaderFilter->Type(), Drawing::GEFilterType::GRID_WARP);
    EXPECT_EQ(geGridWarpShaderFilter->TypeName(), Drawing::GE_FILTER_GRID_WARP);
}

} // namespace Rosen
} // namespace OHOS