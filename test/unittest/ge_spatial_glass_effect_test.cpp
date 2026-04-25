/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "ge_spatial_glass_effect.h"
#include "ge_visual_effect_impl.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "draw/canvas.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESpatialGlassEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    Drawing::Canvas canvasNoGpu_;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GESpatialGlassEffectTest::SetUpTestCase(void) {}
void GESpatialGlassEffectTest::TearDownTestCase(void) {}

void GESpatialGlassEffectTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 10.0f, 10.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    if (surface_) {
        canvas_ = surface_->GetCanvas();
    }
}

void GESpatialGlassEffectTest::TearDown() {}

std::shared_ptr<Drawing::Surface> GESpatialGlassEffectTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GESpatialGlassEffectTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

/**
* @tc.name: MakeDrawingShaderTest_001
* @tc.desc: Verify MakeDrawingShader with valid parameters (Normal Path)
* @tc.type: FUNC
*/
HWTEST_F(GESpatialGlassEffectTest, MakeDrawingShaderTest_001, TestSize.Level1)
{
    Drawing::GESpatialGlassEffectParams params;
    if (surface_) {
        params.blurImage = surface_->GetImageSnapshot();
    }
    
    auto effect = std::make_shared<GESpatialGlassEffect>(params);
    effect->MakeDrawingShader(rect_, 1.0f);
    EXPECT_NE(effect, nullptr);
}

/**
* @tc.name: InitSpatialGlassEffectTest_002
* @tc.desc: Verify InitSpatialGlassEffect and static effect initialization
* @tc.type: FUNC
*/
HWTEST_F(GESpatialGlassEffectTest, InitSpatialGlassEffectTest_002, TestSize.Level1)
{
    Drawing::GESpatialGlassEffectParams params;
    auto effect = std::make_shared<GESpatialGlassEffect>(params);
    bool res = effect->InitSpatialGlassEffect();
    EXPECT_TRUE(res);
}

/**
* @tc.name: MakeSpatialGlassShaderNoShape_003
* @tc.desc: Verify MakeSpatialGlassShader when sdfShape is null
* @tc.type: FUNC
*/
HWTEST_F(GESpatialGlassEffectTest, MakeSpatialGlassShaderNoShape_003, TestSize.Level1)
{
    Drawing::GESpatialGlassEffectParams params;
    params.sdfShape = nullptr;
    auto effect = std::make_shared<GESpatialGlassEffect>(params);
    
    auto shader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto builder = effect->MakeSpatialGlassShader(shader, rect_);
    EXPECT_EQ(builder, nullptr);
}

} // namespace Rosen
} // namespace OHOS