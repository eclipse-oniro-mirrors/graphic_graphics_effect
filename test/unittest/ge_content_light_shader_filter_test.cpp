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

#include "ge_content_light_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class GEContentLightShaderFilterTest : public testing::Test {
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

void GEContentLightShaderFilterTest::SetUpTestCase(void) {}

void GEContentLightShaderFilterTest::TearDownTestCase(void) {}

void GEContentLightShaderFilterTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEContentLightShaderFilterTest::TearDown() { image_ = nullptr; }

/**
 * @tc.name: ProcessImage_001
 * @tc.desc: Verify the ProcessImage: image is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEContentLightShaderFilterTest, ProcessImage_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest ProcessImage_001 start";
    Drawing::GEContentLightFilterParams params;
    auto filter = std::make_unique<GEContentLightFilter>(params);
    EXPECT_EQ(filter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);
 
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(1, 0, format); // 0, 1  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image1 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image1, src_, dst_), nullptr);

    bmp.Build(0, 1, format); // 1, 0  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLACK);
    auto image2 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image2, src_, dst_), nullptr);

    bmp.Build(0, 0, format); // 1, 0  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    auto image3 = bmp.MakeImage();
    EXPECT_EQ(filter->ProcessImage(canvas_, image3, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest ProcessImage_001 end";
}

/**
 * @tc.name: ProcessImage_002
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GEContentLightShaderFilterTest, ProcessImage_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest ProcessImage_002 start";

    Drawing::GEContentLightFilterParams params;
    auto filter = std::make_unique<GEContentLightFilter>(params);
    EXPECT_EQ(filter->ProcessImage(canvas_, image_, src_, dst_), image_);

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    Vector4f lightColor = Vector4f(0.2f, 0.4f, 0.6f, 0.5f);
    float lightIntentsity = 0.6f;
    Vector3f rotateAngle = Vector3f(0.1f, 0.2f, 0.3f);

    Drawing::GEContentLightFilterParams params1 { lightPosition, lightColor, lightIntentsity, rotateAngle };
    auto filter1 = std::make_unique<GEContentLightFilter>(params1);
    EXPECT_EQ(filter1->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest ProcessImage_002 end";
}

/**
 * @tc.name: GetContentLightEffect_001
 * @tc.desc: Verify the GetContentLightEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEContentLightShaderFilterTest, GetContentLightEffect_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest GetContentLightEffect_001 start";

    Drawing::GEContentLightFilterParams params;
    auto filter = std::make_unique<GEContentLightFilter>(params);
    filter->GetContentLightEffect();
    EXPECT_NE(filter->contentLightShaderEffect_, nullptr);

    GTEST_LOG_(INFO) << "GEContentLightShaderFilterTest GetContentLightEffect_001 end";
}
} // namespace Rosen
} // namespace OHOS