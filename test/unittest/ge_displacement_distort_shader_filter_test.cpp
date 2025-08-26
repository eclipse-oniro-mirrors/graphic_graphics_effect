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
 
#include "ge_displacement_distort_shader_filter.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace GraphicsEffectEngine {
 
using namespace Rosen;
 
class GEDisplacementDistortShaderFilterTest : public testing::Test {
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
 
void GEDisplacementDistortShaderFilterTest::SetUpTestCase(void) {}

void GEDisplacementDistortShaderFilterTest::TearDownTestCase(void) {}

void GEDisplacementDistortShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEDisplacementDistortShaderFilterTest::TearDown() { image_ = nullptr; }
 
/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    EXPECT_NE(geDisplacementDistortFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}
 
/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    geDisplacementDistortFilterParams.factor_ = {0.5f, 0.5f};
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    EXPECT_NE(geDisplacementDistortFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnDrawImage_001
 * @tc.desc: Verify function OnDrawImage
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, OnDrawImage_001, TestSize.Level0)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    Drawing::Brush brush;
    EXPECT_FALSE(geDisplacementDistortFilter->OnDrawImage(canvas_, nullptr, src_, dst_, brush));
}

/**
 * @tc.name: GetDisplacementDistortEffect_001
 * @tc.desc: Verify function GetDisplacementDistortEffect
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, GetDisplacementDistortEffect_001, TestSize.Level2)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    geDisplacementDistortFilterParams.factor_ = {0.5f, 0.5f};
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    EXPECT_NE(geDisplacementDistortFilter->GetDisplacementDistortEffect(), nullptr);
}

/**
 * @tc.name: Cache_001
 * @tc.desc: Verify cache function
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, Cache_001, TestSize.Level2)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    EXPECT_EQ(geDisplacementDistortFilter->GetCache(), nullptr);
    auto cache = std::make_shared<std::any>(std::make_any<float>(1.0));
    geDisplacementDistortFilter->SetCache(cache);
    EXPECT_EQ(std::any_cast<float>(*geDisplacementDistortFilter->GetCache()), 1.0);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify type function
 * @tc.type:FUNC
 */
HWTEST_F(GEDisplacementDistortShaderFilterTest, Type_001, TestSize.Level2)
{
    Drawing::GEDisplacementDistortFilterParams geDisplacementDistortFilterParams;
    auto geDisplacementDistortFilter = std::make_shared<GEDisplacementDistortFilter>(geDisplacementDistortFilterParams);
    EXPECT_EQ(geDisplacementDistortFilter->Type(), Drawing::GE_FILTER_DISPLACEMENT_DISTORT);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS