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

#include "ge_distortion_collapse_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

constexpr size_t BITMAP_SIZE = 100;

class GEDistortionCollapseFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 0.0f, 0.0f, 100.0f, 100.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 100.0f, 100.0f };
};

void GEDistortionCollapseFilterTest::SetUpTestCase(void) {}
void GEDistortionCollapseFilterTest::TearDownTestCase(void) {}

void GEDistortionCollapseFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(BITMAP_SIZE, BITMAP_SIZE, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEDistortionCollapseFilterTest::TearDown() {}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage with default parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage with corner parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    params.LUCorner_ = Drawing::Point(0.0f, 0.0f);
    params.RUCorner_ = Drawing::Point(1.0f, 0.0f);
    params.RBCorner_ = Drawing::Point(1.0f, 1.0f);
    params.LBCorner_ = Drawing::Point(0.0f, 1.0f);
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage with barrel distortion
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    params.barrelDistortion_ = Vector4f(0.1f, 0.1f, 0.1f, 0.1f);
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage with null image
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_004, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    std::shared_ptr<Drawing::Image> nullImage = nullptr;
    EXPECT_EQ(filter->OnProcessImage(canvas_, nullImage, src_, dst_), nullImage);
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify function OnProcessImage with all parameters
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_005, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    params.LUCorner_ = Drawing::Point(0.1f, 0.1f);
    params.RUCorner_ = Drawing::Point(0.9f, 0.1f);
    params.RBCorner_ = Drawing::Point(0.9f, 0.9f);
    params.LBCorner_ = Drawing::Point(0.1f, 0.9f);
    params.barrelDistortion_ = Vector4f(0.2f, 0.3f, 0.2f, 0.3f);
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify type function
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, Type_001, TestSize.Level2)
{
    Drawing::GEDistortionCollapseFilterParams params;
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::DISTORTION_COLLAPSE);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_DISTORTION_COLLAPSE);
}

/**
 * @tc.name: Cache_001
 * @tc.desc: Verify cache function
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, Cache_001, TestSize.Level2)
{
    Drawing::GEDistortionCollapseFilterParams params;
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->GetCache(), nullptr);
    auto cache = std::make_shared<std::any>(std::make_any<float>(1.0));
    filter->SetCache(cache);
    EXPECT_EQ(std::any_cast<float>(*filter->GetCache()), 1.0);
}

/**
 * @tc.name: OnProcessImage_006
 * @tc.desc: Verify function OnProcessImage with different src/dst rects
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_006, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    Drawing::Rect src { 10.0f, 10.0f, 90.0f, 90.0f };
    Drawing::Rect dst { 0.0f, 0.0f, 100.0f, 100.0f };
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src, dst), image_);
}

/**
 * @tc.name: OnProcessImage_007
 * @tc.desc: Verify function OnProcessImage with asymmetric barrel distortion
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_007, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    params.barrelDistortion_ = Vector4f(0.1f, 0.2f, 0.3f, 0.4f);
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_008
 * @tc.desc: Verify function OnProcessImage with extreme corner values
 * @tc.type:FUNC
 */
HWTEST_F(GEDistortionCollapseFilterTest, OnProcessImage_008, TestSize.Level0)
{
    Drawing::GEDistortionCollapseFilterParams params;
    params.LUCorner_ = Drawing::Point(0.0f, 0.0f);
    params.RUCorner_ = Drawing::Point(1.0f, 0.0f);
    params.RBCorner_ = Drawing::Point(1.0f, 1.0f);
    params.LBCorner_ = Drawing::Point(0.0f, 1.0f);
    params.barrelDistortion_ = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    auto filter = std::make_shared<GEDistortionCollapseFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
