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
#include "ge_dispersion_shader_filter.h"
#include "ge_ripple_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace GraphicsEffectEngine {

class GEDispersionShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Rosen::Drawing::Image> MakeImage(Rosen::Drawing::Canvas &canvas);

    static inline Rosen::Drawing::Canvas canvas_;
    std::shared_ptr<Rosen::Drawing::Image> image_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> imageEmpty_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Rosen::Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Rosen::Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEDispersionShaderFilterTest::SetUpTestCase(void) {}
void GEDispersionShaderFilterTest::TearDownTestCase(void) {}

void GEDispersionShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Rosen::Drawing::Bitmap bmp;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::COLORTYPE_RGBA_8888, Rosen::Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Rosen::Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();

    bmp.Build(0, 0, format); // 0, 0  bitmap size
    imageEmpty_ = bmp.MakeImage();
}

void GEDispersionShaderFilterTest::TearDown() {}

std::shared_ptr<GEDispersionShaderFilter> CreateDispersionShaderFilter(Drawing::GEDispersionShaderFilterParams params)
{
    return std::make_shared<GEDispersionShaderFilter>(params);
}

std::shared_ptr<Drawing::GERippleShaderMask> CreateDispersionRippleShaderMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.f, 0.f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

std::shared_ptr<GEDispersionShaderFilter> CreateDispersionShaderFilter()
{
    Drawing::GEDispersionShaderFilterParams params = {
        CreateDispersionRippleShaderMask(), 0.5f, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}};
    return CreateDispersionShaderFilter(params);
}

/**
 * @tc.name: GEDispersionShaderFilterCreateTest
 * @tc.desc: GEDispersionShaderFilterTest.GEDispersionShaderFilterCreateTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDispersionShaderFilterTest, GEDispersionShaderFilterCreateTest, TestSize.Level1)
{
    auto dispersionShaderFilter = CreateDispersionShaderFilter();
    EXPECT_NE(dispersionShaderFilter, nullptr);
}

/**
 * @tc.name: GetDispersionEffectTest
 * @tc.desc: GEDispersionShaderFilterTest.GetDispersionEffectTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDispersionShaderFilterTest, GetDispersionEffectTest, TestSize.Level1)
{
    auto dispersionShaderFilter = CreateDispersionShaderFilter();
    ASSERT_NE(dispersionShaderFilter, nullptr);

    EXPECT_NE(dispersionShaderFilter->GetDispersionEffect(), nullptr);
    EXPECT_NE(dispersionShaderFilter->GetDispersionEffect(), nullptr);
}

/**
 * @tc.name: OnProcessImageTest
 * @tc.desc: GEDispersionShaderFilterTest.OnProcessImageTest
 * @tc.type: FUNC
 */
HWTEST_F(GEDispersionShaderFilterTest, OnProcessImageTest, TestSize.Level1)
{
    auto dispersionShaderFilter = CreateDispersionShaderFilter();
    ASSERT_NE(dispersionShaderFilter, nullptr);

    EXPECT_EQ(dispersionShaderFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
    dispersionShaderFilter->params_.mask = nullptr;
    EXPECT_EQ(dispersionShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);

    dispersionShaderFilter->params_.mask = CreateDispersionRippleShaderMask();
    EXPECT_EQ(dispersionShaderFilter->OnProcessImage(canvas_, imageEmpty_, src_, dst_), nullptr);

    EXPECT_NE(dispersionShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace Rosen
} // namespace OHOS