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
#include "ge_aurora_noise_shader.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {

class GEAuroraNoiseShaderTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {}
    void TearDown() override
    {}

    static inline Canvas canvas_;
};

/**
 * @tc.name: GEAuroraNoiseShaderTest001
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest001, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.0};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    EXPECT_NE(shader, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest002
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest002, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{1.0};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    auto builder1 = shader->GetAuroraNoiseBuilder();
    EXPECT_NE(builder1, nullptr);
    auto builder2 = shader->GetAuroraNoiseBuilder();
    EXPECT_NE(builder2, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest003
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest003, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.2};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader->MakeAuroraNoiseShader(rect);
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest004
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest004, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 50, 50};
    shader->MakeDrawingShader(rect, 0.5f);
    SUCCEED();
}

/**
 * @tc.name: GEAuroraNoiseShaderTest005
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest005, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{5.0};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    shader->MakeDrawingShader(rect, 0.75f);
    shader->SetAuroraNoiseParams(params);
    ASSERT_NE(shader, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest006
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest006, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::ImageInfo imageInfo(50, 50, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    auto image = shader->MakeAuroraNoiseVerticalBlurShader(canvas_, imageInfo);
    EXPECT_EQ(image, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader->MakeAuroraNoiseShader(rect);
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest007
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest007, TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 100, 100);
    GEAuroraNoiseShaderParams params{0.5};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    shader->Preprocess(canvas_, rect);
    auto cache = shader->GetCache();
    EXPECT_EQ(cache, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest008
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest008, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.7};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::ImageInfo imageInfo(50, 50, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_OPAQUE);
    auto image = shader->MakeAuroraNoiseGeneratorShader(canvas_, imageInfo);
    EXPECT_EQ(image, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader->MakeAuroraNoiseShader(rect);
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: GEAuroraNoiseShaderTest009
 * @tc.type: FUNC
 */
HWTEST_F(GEAuroraNoiseShaderTest, GEAuroraNoiseShaderTest009, TestSize.Level1)
{
    Drawing::GEAuroraNoiseShaderParams params{0.7};
    auto shader = GEAuroraNoiseShader::CreateAuroraNoiseShader(params);
    ASSERT_NE(shader, nullptr);

    Drawing::ImageInfo image1Info(100, 100, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_OPAQUE);
    auto image1 = shader->MakeAuroraNoiseGeneratorShader(canvas_, image1Info);
    EXPECT_EQ(image1, nullptr);

    Drawing::ImageInfo image2Info(100, 100, ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    auto image2 = shader->MakeAuroraNoiseVerticalBlurShader(canvas_, image2Info);
    EXPECT_EQ(image2, nullptr);

    Drawing::Rect rect{0, 0, 100, 100};
    auto effect = shader->MakeAuroraNoiseShader(rect);
    EXPECT_EQ(effect, nullptr);
}
}  // namespace Rosen
}  // namespace OHOS