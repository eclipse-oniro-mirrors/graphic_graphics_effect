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

#include "ge_color_gradient_shader_filter.h"
#include "ge_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEShaderFilterTest : public testing::Test {
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
};

void GEShaderFilterTest::SetUpTestCase(void) {}
void GEShaderFilterTest::TearDownTestCase(void) {}

void GEShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEShaderFilterTest::TearDown() {}

/**
 * @tc.name: SetShaderFilterCanvasinfo_001
 * @tc.desc: Verify the SetShaderFilterCanvasinfo
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderFilterTest, SetShaderFilterCanvasinfo_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEShaderFilterTest SetShaderFilterCanvasinfo_001 start";
    Drawing::CanvasInfo canvasInfo = {100.0f, 100.0f, 0.0f, 0.0f, Drawing::Matrix()};

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<float> poitions = { 1.0f, 1.0f }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5f }; // 0.5 is strength params
    Drawing::GEColorGradientShaderFilterParams params { colors, poitions, strengths, nullptr };
    auto filter = std::make_unique<GEColorGradientShaderFilter>(params);

    EXPECT_NE(filter->canvasInfo_.geoHeight_, canvasInfo.geoHeight_);
    filter->SetShaderFilterCanvasinfo(canvasInfo);
    EXPECT_EQ(filter->canvasInfo_.geoHeight_, canvasInfo.geoHeight_);

    GTEST_LOG_(INFO) << "GEShaderFilterTest SetShaderFilterCanvasinfo_001 end";
}

} // namespace Rosen
} // namespace OHOS