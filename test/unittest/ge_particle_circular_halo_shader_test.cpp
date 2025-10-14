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
#include "draw/canvas.h"
#include "render_context/render_context.h"

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
private:
    Drawing::GEParticleCircularHaloShaderParams InitialParams(
        float radius, float centerFirst, float centerSecond, float noise);
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Canvas canvasNoGpu_;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GEParticleCircularHaloShaderTest::SetUpTestCase(void) {}
void GEParticleCircularHaloShaderTest::TearDownTestCase(void) {}

void GEParticleCircularHaloShaderTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 10.0f, 10.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    canvas_ = surface_->GetCanvas();
}

void GEParticleCircularHaloShaderTest::TearDown() {}

std::shared_ptr<Drawing::Surface> GEParticleCircularHaloShaderTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEParticleCircularHaloShaderTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

Drawing::GEParticleCircularHaloShaderParams GEParticleCircularHaloShaderTest::InitialParams(
    float radius, float centerFirst, float centerSecond, float noise)
{
    Drawing::GEParticleCircularHaloShaderParams params;
    params.radius_ = radius;
    params.center_ = std::make_pair(centerFirst, centerSecond);
    params.noise_ = noise;
    return params;
}

/**
* @tc.name: CreateSurfaceCanvasTest
* @tc.desc: Verify if surface_ and canvas_ were successfully created
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, CreateSurfaceCanvasTest, TestSize.Level1)
{
    EXPECT_NE(surface_, nullptr);
    EXPECT_NE(canvas_, nullptr);
}

/**
* @tc.name: GetDescpritionTest
* @tc.desc: Verify GetDescprition
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, GetDescpritionTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    EXPECT_EQ(shader->GetDescription(), "GEParticleCircularHaloShader");
}

/**
* @tc.name: CreateParamsTest
* @tc.desc: Verify CreateParticleCircularHaloShader
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, CreateParamsTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    EXPECT_EQ(shader->particleCircularHaloParams_.radius_, 0.5);
    EXPECT_EQ(shader->particleCircularHaloParams_.center_, std::make_pair(0.5f, 0.5f));
    EXPECT_EQ(shader->particleCircularHaloParams_.noise_, 1.0);
}

/**
* @tc.name: NoPreprocessTest
* @tc.desc: Verify initial cache Imgs
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, NoPreprocessTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    EXPECT_EQ(shader->cacheAnyPtr_, nullptr);
}

/**
* @tc.name: GetGlowHaloBuilderTest
* @tc.desc: Verify GetGlowHaloBuilder function
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, GetGlowHaloBuilderTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto shderEffect = shader->GetGlowHaloBuilder();
    EXPECT_NE(shderEffect, nullptr);
}

/**
* @tc.name: MakeDrawingShaderTest
* @tc.desc: Verify full drawing pipeline with MakeDrawingShader
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, MakeDrawingShaderTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shader->MakeDrawingShader(rect_, 0.5f);
    EXPECT_NE(shader->drShader_, nullptr);
}

/**
* @tc.name: MakeShaderWithDifferentParamsTest
* @tc.desc: Test full pipe line with different inputs
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, MakeShaderWithDifferentParamsTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 4.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);

    params = InitialParams(-0.1, 0.5, 0.5, 4.0);
    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);

    params = InitialParams(2.0, 0.5, 0.5, 4.0);
    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);

    params = InitialParams(0.5, -0.5, 0.5, 4.0);
    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);

    params = InitialParams(0.5, 0.5, -0.5, 4.0);
    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);

    params = InitialParams(0.5, 20.0, 0.5, 4.0);
    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(*canvas_, rect_);
    shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_NE(shaderEffect, nullptr);
}

/**
* @tc.name: MakeNullShaderTest
* @tc.desc: Test shader with nullptr outputs
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, MakeNullShaderTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    shader->Preprocess(canvasNoGpu_, rect_);
    auto shaderEffect = shader->MakeParticleCircularHaloShader(rect_);
    EXPECT_EQ(shaderEffect, nullptr);

    shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    auto glowEffect = shader->MakeGlowHaloShader(canvasNoGpu_, imageInfo_);
    EXPECT_EQ(glowEffect, nullptr);
    
    auto particleEffect = shader->MakeParticleHaloShader(canvasNoGpu_, imageInfo_);
    EXPECT_EQ(particleEffect, nullptr);
}

/**
* @tc.name: PreprocessBranchCoverageTest
* @tc.desc: Verify Preprocess() cache behavior:
            1. First frame builds both particle/glow and cache.
            2. Noise unchanged reuses cache.
            3. Noise changed rebuilds particle only
* @tc.type: FUNC
*/
HWTEST_F(GEParticleCircularHaloShaderTest, PreprocessBranchCoverageTest, TestSize.Level1)
{
    Drawing::GEParticleCircularHaloShaderParams params = InitialParams(0.5, 0.5, 0.5, 1.0);
    auto shader = GEParticleCircularHaloShader::CreateParticleCircularHaloShader(params);
    
    shader->Preprocess(*canvas_, rect_);
    EXPECT_NE(shader->cacheAnyPtr_, nullptr);
    auto particle1 = shader->particleHaloImg_.get();
    auto glow1 = shader->glowHaloImg_.get();
    EXPECT_NE(particle1, nullptr);
    EXPECT_NE(glow1, nullptr);

    shader->Preprocess(*canvas_, rect_);
    auto particle2 = shader->particleHaloImg_.get();
    auto glow2 = shader->glowHaloImg_.get();
    EXPECT_EQ(particle1, particle2);
    EXPECT_EQ(glow1, glow2);

    shader->particleCircularHaloParams_.noise_ += 0.1f;
    shader->Preprocess(*canvas_, rect_);
    auto particle3 = shader->particleHaloImg_.get();
    auto glow3 = shader->glowHaloImg_.get();
    EXPECT_NE(particle2, particle3);
    EXPECT_EQ(glow2, glow3);
}
}  // namespace Rosen
}  // namespace OHOS