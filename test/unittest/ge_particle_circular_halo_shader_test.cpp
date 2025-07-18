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
#include "ge_particle_circular_halo_shader.h"
#include "ge_external_dynamic_loader.h"
#include "ge_visual_effect_impl.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEParticleCircularHaloShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEParticleCircularHaloShaderTest::SetUpTestCase(void) {}
void GEParticleCircularHaloShaderTest::TearDownTestCase(void) {}

void GEParticleCircularHaloShaderTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEParticleCircularHaloShaderTest::TearDown() {}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_001 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(0.5f, 0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (0.5, 0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_001 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_002 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = -0.1f;
    params.center_ = std::make_pair(0.5f, 0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = -0.5, center = (0.5, 0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_002 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_003 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 2.0f;
    params.center_ = std::make_pair(0.5f, 0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 2.0, center = (0.5, 0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);



    
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_003 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_004 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(-0.5f, 0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (-0.5, 0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_004 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_005 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(0.5f, -0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (0.5, -0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_005 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_006 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(1.5f, 0.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (1.5, 0.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_006 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_007 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(0.5f, 1.5f);
    params.noise_ = 4.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (0.5, 1.5), noise = 4.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_007 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_008 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 0.5f;
    params.center_ = std::make_pair(1.5f, 0.5f);
    params.noise_ = -2.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (1.5, 0.5), noise = -2.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_008 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_009 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = -0.5f;
    params.center_ = std::make_pair(-0.5f, -0.5f);
    params.noise_ = -2.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = -0.5, center = (-0.5, -0.5), noise = -2.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_009 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_010 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.center_ = std::make_pair(0.5f, 0.5f);
    params.noise_ = 2.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (0.5, 0.5), noise = 2.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_010 end";
}

HWTEST_F(GEParticleCircularHaloShaderTest, GEParticleCircularHaloShaderTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_011 start";
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = 1;
    params.center_ = std::make_pair(0.5f, 0.5f);
    params.noise_ = 2.0f;

    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);

    if (shaderEffect) {
        GTEST_LOG_(INFO) << "Shader created successfully, radius = 0.5, center = (0.5, 0.5), noise = 2.0";
    } else {
        GTEST_LOG_(ERROR) << "Failed to create shader.";
    }
    EXPECT_NE(shaderEffect, nullptr);

    GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest MakeParticleCircularHaloShader_011 end";
}

}  // namespace Rosen
}  // namespace OHOS