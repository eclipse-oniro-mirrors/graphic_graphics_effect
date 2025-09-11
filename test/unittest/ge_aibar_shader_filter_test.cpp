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

#include "ge_aibar_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEAIBarShaderFilterTest : public testing::Test {
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

void GEAIBarShaderFilterTest::SetUpTestCase(void) {}

void GEAIBarShaderFilterTest::TearDownTestCase(void) {}

void GEAIBarShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEAIBarShaderFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify the OnProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_001 start";

    // 0.0, 0.0, 0.0, 0.0, 0.0 valid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_001 end";
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_002 start";

    // -0.5, -0.5, -0.5, -0.5, -0.5 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { -0.5, -0.5, -0.5, -0.5, -0.5 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_002 end";
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_003 start";

    // 2.0, 2.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_003 end";
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_004 start";

    // 1.0, 2.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 1.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_004 end";
}

/**
 * @tc.name: OnProcessImage_005
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_005 start";

    // 2.0, 1.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 1.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_005 end";
}

/**
 * @tc.name: OnProcessImage_006
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_006 start";

    // 2.0, 2.0, 1.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 1.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_006 end";
}

/**
 * @tc.name: OnProcessImage_007
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_007 start";

    // 2.0, 2.0, 2.0, 1.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 1.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_007 end";
}

/**
 * @tc.name: OnProcessImage_008
 * @tc.desc: Verify the OnProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, OnProcessImage_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_008 start";

    // 2.0, 2.0, 2.0, 2.0, 1.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 1.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest OnProcessImage_008 end";
}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify the GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, GetDescription_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription_001 start";

    // 0.0, 0.0, 0.0, 0.0, 0.0 valid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->GetDescription(), "GEAIBarShaderFilter");

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription_001 end";
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify the Type
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, Type_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest Type_001 start";

    // 0.0, 0.0, 0.0, 0.0, 0.0 valid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->Type(), Drawing::GEFilterType::AIBAR);
    EXPECT_EQ(geAIBarShaderFilter->TypeName(), Drawing::GE_FILTER_AI_BAR);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest Type_001 end";
}

} // namespace Rosen
} // namespace OHOS
