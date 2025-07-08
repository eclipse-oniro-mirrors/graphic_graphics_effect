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
#include "ge_variable_radius_blur_shader_filter.h"
#include "ge_ripple_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEVariableRadiusBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Rosen::Drawing::Image> MakeImage(Rosen::Drawing::Canvas &canvas);

    static inline Rosen::Drawing::Canvas canvas_;
    std::shared_ptr<Rosen::Drawing::Image> image_{nullptr};
    std::shared_ptr<Rosen::Drawing::Image> imageEmpty_ { nullptr };
 
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Rosen::Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Rosen::Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEVariableRadiusBlurShaderFilterTest::SetUpTestCase(void) {}
void GEVariableRadiusBlurShaderFilterTest::TearDownTestCase(void) {}

void GEVariableRadiusBlurShaderFilterTest::SetUp()
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

void GEVariableRadiusBlurShaderFilterTest::TearDown() {}

std::shared_ptr<GEVariableRadiusBlurShaderFilter> CreateVariableRadiusBlurShaderFilter(
    Drawing::GEVariableRadiusBlurShaderFilterParams params)
{
    return std::make_shared<GEVariableRadiusBlurShaderFilter>(params);
}

std::shared_ptr<Drawing::GERippleShaderMask> CreateRippleShaderMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.f, 0.f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

std::shared_ptr<GEVariableRadiusBlurShaderFilter> CreateVariableRadiusBlurShaderFilter()
{

    Drawing::GEVariableRadiusBlurShaderFilterParams params = {
        CreateRippleShaderMask(), 100.0f};
    return CreateVariableRadiusBlurShaderFilter(params);
}

/**
 * @tc.name: CreateVariableRadiusBlurShaderFilterTest
 * @tc.desc: GEVariableRadiusBlurShaderFilterTest.CreateVariableRadiusBlurShaderFilterTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEVariableRadiusBlurShaderFilterTest, CreateVariableRadiusBlurShaderFilterTest, TestSize.Level1)
{
    auto variableRadiusBlurShaderFilter = CreateVariableRadiusBlurShaderFilter();
    ASSERT_NE(variableRadiusBlurShaderFilter, nullptr);
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: GEVariableRadiusBlurShaderFilterTest.GetDescriptionTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEVariableRadiusBlurShaderFilterTest, GetDescriptionTest, TestSize.Level1)
{
    auto variableRadiusBlurShaderFilter = CreateVariableRadiusBlurShaderFilter();
    ASSERT_NE(variableRadiusBlurShaderFilter, nullptr);

    std::string emptyString = "";
    EXPECT_NE(variableRadiusBlurShaderFilter->GetDescription(), emptyString);
    EXPECT_NE(variableRadiusBlurShaderFilter->GetDetailedDescription(), emptyString);
}

/**
 * @tc.name: ProcessImageTest001
 * @tc.desc: GEVariableRadiusBlurShaderFilterTest.ProcessImageTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEVariableRadiusBlurShaderFilterTest, ProcessImageTest001, TestSize.Level1)
{
    auto variableRadiusBlurShaderFilter = CreateVariableRadiusBlurShaderFilter();
    ASSERT_NE(variableRadiusBlurShaderFilter, nullptr);

    EXPECT_EQ(variableRadiusBlurShaderFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    variableRadiusBlurShaderFilter->params_.mask = nullptr;
    EXPECT_EQ(variableRadiusBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    variableRadiusBlurShaderFilter->params_.mask = CreateRippleShaderMask();
    EXPECT_EQ(variableRadiusBlurShaderFilter->ProcessImage(canvas_, imageEmpty_, src_, dst_), imageEmpty_);

    EXPECT_NE(variableRadiusBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), nullptr);
}

}  // namespace Rosen
}  // namespace OHOS