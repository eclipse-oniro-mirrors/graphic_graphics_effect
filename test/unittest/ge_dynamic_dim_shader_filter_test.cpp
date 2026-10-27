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

#include "ge_dynamic_dim_shader_filter.h"

#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEDynamicDimShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    Drawing::Rect src_ { 0.0f, 0.0f, 50.0f, 50.0f };
    Drawing::Rect dst_ { 0.0f, 0.0f, 50.0f, 50.0f };
};

void GEDynamicDimShaderFilterTest::SetUpTestCase() {}
void GEDynamicDimShaderFilterTest::TearDownTestCase() {}

void GEDynamicDimShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_WHITE);
    image_ = bmp.MakeImage();
}

void GEDynamicDimShaderFilterTest::TearDown() {}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Verify constructor with normal params
 * @tc.type: FUNC
 */
HWTEST_F(GEDynamicDimShaderFilterTest, Constructor_001, TestSize.Level1)
{
    Drawing::GEDynamicDimShaderFilterParams params;
    params.value = 0.3f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: FilterType_001
 * @tc.desc: Verify Type() returns DYNAMIC_DIM
 * @tc.type: FUNC
 */
HWTEST_F(GEDynamicDimShaderFilterTest, FilterType_001, TestSize.Level1)
{
    Drawing::GEDynamicDimShaderFilterParams params;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::DYNAMIC_DIM);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify OnProcessImage with valid image
 * @tc.type: FUNC
 */
HWTEST_F(GEDynamicDimShaderFilterTest, OnProcessImage_001, TestSize.Level1)
{
    Drawing::GEDynamicDimShaderFilterParams params;
    params.value = 0.3f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify OnProcessImage with null image returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEDynamicDimShaderFilterTest, OnProcessImage_002, TestSize.Level1)
{
    Drawing::GEDynamicDimShaderFilterParams params;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, nullptr, src_, dst_);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify OnProcessImage with boundary value 0.0
 * @tc.type: FUNC
 */
HWTEST_F(GEDynamicDimShaderFilterTest, OnProcessImage_003, TestSize.Level1)
{
    Drawing::GEDynamicDimShaderFilterParams params;
    params.value = 0.0f;
    auto filter = std::make_shared<GEDynamicDimShaderFilter>(params);
    ASSERT_NE(filter, nullptr);
    auto result = filter->OnProcessImage(canvas_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
}

} // namespace Rosen
} // namespace OHOS
