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
 
#include "ge_edge_light_shader_filter.h"

#include "common/rs_vector4.h"
#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {
namespace GraphicsEffectEngine {
 
class GEEdgeLightShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Rosen::Drawing::Image> MakeImage(Rosen::Drawing::Canvas& canvas);
 
    static inline Rosen::Drawing::Canvas canvas_;
    std::shared_ptr<Rosen::Drawing::Image> image_ { nullptr };
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

    bmp.Build(0, 0, format); // 0, 0  bitmap size
    imageEmpty_ = bmp.MakeImage();
}
 
void GEEdgeLightShaderFilterTest::TearDown()
{
    image_ = nullptr;
    imageEmpty_ = nullptr;
}
 
/**
 * @tc.name: OnProcessImage_001
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEEdgeLightShaderFilterTest, OnProcessImage_001, TestSize.Level0)
{
    Vector4f color = Vector4f{0.2f, 0.7f, 0.1f, 0.0f};
    Rosen::Drawing::GEEdgeLightShaderFilterParams
        geEdgeLightShaderParams{ 1.0f, true, color, nullptr, false };
    auto geEdgeLightShaderFilter = std::make_shared<Rosen::GEEdgeLightShaderFilter>(geEdgeLightShaderParams);
    EXPECT_EQ(geEdgeLightShaderFilter->OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_002
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEEdgeLightShaderFilterTest, OnProcessImage_002, TestSize.Level0)
{
    Vector4f color = Vector4f{0.2f, 0.7f, 0.1f, 0.0f};
    Rosen::Drawing::GEEdgeLightShaderFilterParams
        geEdgeLightShaderParams{ 1.0f, true, color, nullptr, false };
    auto geEdgeLightShaderFilter = std::make_shared<Rosen::GEEdgeLightShaderFilter>(geEdgeLightShaderParams);
    EXPECT_EQ(geEdgeLightShaderFilter->OnProcessImage(canvas_, image_, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImage_003
 * @tc.desc: Verify function OnProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEEdgeLightShaderFilterTest, OnProcessImage_003, TestSize.Level0)
{
    Vector4f color = Vector4f{0.2f, 0.7f, 0.1f, 0.0f};
    Rosen::Drawing::GEEdgeLightShaderFilterParams
        geEdgeLightShaderParams{ 1.0f, true, color, nullptr, false };
    auto geEdgeLightShaderFilter = std::make_shared<Rosen::GEEdgeLightShaderFilter>(geEdgeLightShaderParams);
    EXPECT_EQ(geEdgeLightShaderFilter->OnProcessImage(canvas_, imageEmpty_, src_, dst_), nullptr);
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
    EXPECT_EQ(geEdgeLightShaderFilter->TypeName(), Drawing::GE_FILTER_EDGE_LIGHT);
}

} // namespace GraphicsEffectEngine
} // namespace Rosen
} // namespace OHOS