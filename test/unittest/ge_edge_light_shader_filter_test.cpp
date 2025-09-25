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
#include "ge_ripple_shader_mask.h"
#include "ge_edge_light_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace GraphicsEffectEngine {

std::shared_ptr<Drawing::GERippleShaderMask> CreateEdgeLightRippleShaderMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = {0.f, 0.f};
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

class GEEdgeLightShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Rosen::Drawing::Image> MakeImage(Rosen::Drawing::Canvas& canvas);

    static inline Rosen::Drawing::Canvas canvas_;
    std::shared_ptr<Rosen::Drawing::Image> image_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> imageComposite_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> imageEmpty_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Rosen::Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Rosen::Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEEdgeLightShaderFilterTest::SetUpTestCase(void) {}
void GEEdgeLightShaderFilterTest::TearDownTestCase(void) {}

void GEEdgeLightShaderFilterTest::SetUp()
{
    Rosen::Drawing::Bitmap bmp;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::COLORTYPE_RGBA_8888, Rosen::Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Rosen::Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    imageComposite_ = bmp.MakeImage();
    bmp.Build(0, 0, format); // 0, 0  bitmap size
    imageEmpty_ = bmp.MakeImage();
}

void GEEdgeLightShaderFilterTest::TearDown()
{
    image_ = nullptr;
    imageEmpty_ = nullptr;
}

std::shared_ptr<GEEdgeLightShaderFilter> CreateEdgeLightShaderFilter(Drawing::GEEdgeLightShaderFilterParams params)
{
    return std::make_shared<GEEdgeLightShaderFilter>(params);
}

std::shared_ptr<GEEdgeLightShaderFilter> CreateEdgeLightShaderFilter()
{
    Drawing::GEEdgeLightShaderFilterParams params = {1.0f, true, Vector4f(0.2f, 0.7f, 0.1f, 0.0f), nullptr, false};
    return CreateEdgeLightShaderFilter(params);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GEEdgeLightShaderFilterTest, Type_001, TestSize.Level0)
{
    Vector4f color = Vector4f{0.2f, 0.7f, 0.1f, 0.0f};
    Rosen::Drawing::GEEdgeLightShaderFilterParams
        geEdgeLightShaderParams{ 1.0f, true, color, nullptr, false };
    auto geEdgeLightShaderFilter = std::make_shared<Rosen::GEEdgeLightShaderFilter>(geEdgeLightShaderParams);
    EXPECT_EQ(geEdgeLightShaderFilter->Type(), Drawing::GEFilterType::EDGE_LIGHT);
    EXPECT_EQ(geEdgeLightShaderFilter->TypeName(), Drawing::GE_FILTER_EDGE_LIGHT);
}

/**
 * @tc.name: GEEdgeLightShaderFilterCreateTest
 * @tc.desc: GEEdgeLightShaderFilterTest.GEEdgeLightShaderFilterCreateTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEEdgeLightShaderFilterTest, GEEdgeLightShaderFilterCreateTest, TestSize.Level1)
{
    auto edgeLightShaderFilter = CreateEdgeLightShaderFilter();
    ASSERT_NE(edgeLightShaderFilter, nullptr);

    EXPECT_TRUE(edgeLightShaderFilter->InitConvertFragShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->InitDetectFragShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->InitGaussShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->InitCompositeShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->InitMaskShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->InitMergeImageShaderEffect());
    EXPECT_TRUE(edgeLightShaderFilter->IsShaderEffectInitValid());
}

/**
 * @tc.name: IsShaderEffectInitValidTest
 * @tc.desc: GEEdgeLightShaderFilterTest.IsShaderEffectInitValidTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEEdgeLightShaderFilterTest, IsShaderEffectInitValidTest, TestSize.Level1)
{
    auto edgeLightShaderFilter = CreateEdgeLightShaderFilter();
    ASSERT_NE(edgeLightShaderFilter, nullptr);

    edgeLightShaderFilter->InitConvertFragShaderEffect();
    edgeLightShaderFilter->InitDetectFragShaderEffect();
    edgeLightShaderFilter->InitGaussShaderEffect();
    edgeLightShaderFilter->InitCompositeShaderEffect();
    edgeLightShaderFilter->InitMaskShaderEffect();
    edgeLightShaderFilter->InitMergeImageShaderEffect();
    edgeLightShaderFilter->IsShaderEffectInitValid();

    ASSERT_TRUE(edgeLightShaderFilter->IsShaderEffectInitValid());
}

/**
 * @tc.name: SubFunctionTest
 * @tc.desc: GEEdgeLightShaderFilterTest.SubFunctionTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEEdgeLightShaderFilterTest, SubFunctionTest, TestSize.Level1)
{
    auto edgeLightShaderFilter = CreateEdgeLightShaderFilter();
    ASSERT_NE(edgeLightShaderFilter, nullptr);

    EXPECT_NE(edgeLightShaderFilter->ConvertColorSpace(canvas_, image_, Drawing::ColorSpace::CreateSRGBLinear()), image_);
    EXPECT_NE(edgeLightShaderFilter->DetectEdge(canvas_, image_), image_);
    EXPECT_NE(edgeLightShaderFilter->GaussianBlur(canvas_, image_), image_);
    edgeLightShaderFilter->mask_ = CreateEdgeLightRippleShaderMask();
    EXPECT_NE(edgeLightShaderFilter->MergeImage(canvas_, image_, imageComposite_), image_);
}

/**
 * @tc.name: IsInputImageValidTest
 * @tc.desc: GEEdgeLightShaderFilterTest.IsInputImageValidTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEEdgeLightShaderFilterTest, IsInputImageValidTest, TestSize.Level1)
{
    auto edgeLightShaderFilter = CreateEdgeLightShaderFilter();
    ASSERT_NE(edgeLightShaderFilter, nullptr);

    EXPECT_FALSE(edgeLightShaderFilter->IsInputImageValid(nullptr));
    EXPECT_TRUE(edgeLightShaderFilter->IsInputImageValid(image_));
    EXPECT_FALSE(edgeLightShaderFilter->IsInputImageValid(imageEmpty_));
}

/**
 * @tc.name: OnProcessImageTest
 * @tc.desc: GEEdgeLightShaderFilterTest.OnProcessImageTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(GEEdgeLightShaderFilterTest, OnProcessImageTest, TestSize.Level1)
{
    auto edgeLightShaderFilter = CreateEdgeLightShaderFilter();
    edgeLightShaderFilter->mask_ = nullptr;
    ASSERT_NE(edgeLightShaderFilter, nullptr);

    EXPECT_EQ(edgeLightShaderFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    EXPECT_EQ(edgeLightShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    edgeLightShaderFilter->color_ = Vector4f(0.5f, 0.5f, 0.5f, 0.0f);
    EXPECT_EQ(edgeLightShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

} // namespace GraphicsEffectEngine
} // namespace Rosen
} // namespace OHOS