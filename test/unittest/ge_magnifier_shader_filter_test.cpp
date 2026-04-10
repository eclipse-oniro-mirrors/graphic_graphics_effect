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

#include "ge_magnifier_shader_filter.h"
#include "ge_sdf_rrect_shader_shape.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEMagnifierShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEMagnifierShaderFilterTest::SetUpTestCase() {}
void GEMagnifierShaderFilterTest::TearDownTestCase() {}

void GEMagnifierShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEMagnifierShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription_001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, GetDescription001, TestSize.Level3)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GEMagnifierShaderFilter";
    EXPECT_EQ(filter->GetDescription(), expectStr);
}

/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src, dst), image_);
}

/**
 * @tc.name: OnProcessImage_004
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, OnProcessImage_004, TestSize.Level0)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->OnProcessImage(canvas_, image_, src_, dst_), image_);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(filter->OnProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ConvertToRgba_001
 * @tc.desc: Verify function ConvertToRgba
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ConvertToRgba_001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    uint32_t color1 = uint32_t(0x80808000);
    float maskColor1[4] = { 0.0f }; // 4 len of tuple
    filter->ConvertToRgba(color1, maskColor1, 4);
}

/**
 * @tc.name: ConvertToRgba_002
 * @tc.desc: Verify function ConvertToRgba
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ConvertToRgba_002, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    uint32_t color1 = uint32_t(0x80808000);
    float maskColor1[4] = { 0.0f }; // 4 len of tuple
    filter->ConvertToRgba(color1, maskColor1, 3);
    filter->ConvertToRgba(color1, nullptr, 3);
}

/**
 * @tc.name: Type_001
 * @tc.desc: Verify function Type
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, Type_001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    EXPECT_EQ(filter->Type(), Drawing::GEFilterType::MAGNIFIER);
    EXPECT_EQ(filter->TypeName(), Drawing::GE_FILTER_MAGNIFIER);
}

/**
 * @tc.name: ValidateMagnifierParams_001
 * @tc.desc: Verify function ValidateMagnifierParams with valid params
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ValidateMagnifierParams_001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    
    Drawing::GERRect rrect1 = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
    Drawing::GESDFRRectShapeParams sdfParams1 = {rrect1};
    auto sdfShape1 = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParams1);
    filter->sdfShape_ = sdfShape1;
    
    EXPECT_TRUE(filter->ValidateMagnifierParams(100.0f, 100.0f));
}

/**
 * @tc.name: ValidateMagnifierParams_002
 * @tc.desc: Verify function ValidateMagnifierParams with invalid image size
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ValidateMagnifierParams_002, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    
    Drawing::GERRect rrect2 = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
    Drawing::GESDFRRectShapeParams sdfParams2 = {rrect2};
    auto sdfShape2 = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParams2);
    filter->sdfShape_ = sdfShape2;
    
    EXPECT_FALSE(filter->ValidateMagnifierParams(0.0f, 100.0f));
    EXPECT_FALSE(filter->ValidateMagnifierParams(100.0f, 0.0f));
    EXPECT_FALSE(filter->ValidateMagnifierParams(-1.0f, 100.0f));
    EXPECT_FALSE(filter->ValidateMagnifierParams(100.0f, -1.0f));
}

/**
 * @tc.name: ValidateMagnifierParams_003
 * @tc.desc: Verify function ValidateMagnifierParams with null magnifierPara
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ValidateMagnifierParams_003, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    
    Drawing::GERRect rrect3 = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
    Drawing::GESDFRRectShapeParams sdfParams3 = {rrect3};
    auto sdfShape3 = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParams3);
    filter->sdfShape_ = sdfShape3;
    filter->magnifierPara_ = nullptr;
    
    EXPECT_FALSE(filter->ValidateMagnifierParams(100.0f, 100.0f));
}

/**
 * @tc.name: ValidateMagnifierParams_004
 * @tc.desc: Verify function ValidateMagnifierParams with invalid factor
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ValidateMagnifierParams_004, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        0.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    
    Drawing::GERRect rrect4 = {0.0f, 0.0f, 100.0f, 100.0f, 10.0f, 10.0f};
    Drawing::GESDFRRectShapeParams sdfParams4 = {rrect4};
    auto sdfShape4 = std::make_shared<Drawing::GESDFRRectShaderShape>(sdfParams4);
    filter->sdfShape_ = sdfShape4;
    
    EXPECT_FALSE(filter->ValidateMagnifierParams(100.0f, 100.0f));
}

/**
 * @tc.name: ValidateMagnifierParams_005
 * @tc.desc: Verify function ValidateMagnifierParams with null sdfShape
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ValidateMagnifierParams_005, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{
        1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    
    filter->sdfShape_ = nullptr;
    
    EXPECT_FALSE(filter->ValidateMagnifierParams(100.0f, 100.0f));
}

} // namespace GraphicsEffectEngine
} // namespace OHOS