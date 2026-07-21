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
#include "ge_cache_helper.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_shader_shape.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

// TestMock
class TestSDFShaderShape : public GESDFShaderShape {
public:
    explicit TestSDFShaderShape(GESDFShapeType type) : type_(type) {}

    GESDFShapeType GetSDFShapeType() const override
    {
        return type_;
    }
    bool HasType(const GESDFShapeType type) const override
    {
        return true;
    }
private:
    GESDFShapeType type_;
};


class GESDFShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    Drawing::Canvas canvas_;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GESDFShaderShapeTest::SetUpTestCase(void) {}
void GESDFShaderShapeTest::TearDownTestCase(void) {}

void GESDFShaderShapeTest::TearDown()
{
    surface_ = nullptr;
}

void GESDFShaderShapeTest::SetUp()
{
    Drawing::Rect rect {0.0f, 0.0f, 100.0f, 100.0f};
    rect_ = rect;
    imageInfo_ = Drawing::ImageInfo {rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    surface_ = CreateSurface();
    EXPECT_NE(surface_, nullptr);
    EXPECT_NE(surface_->GetCanvas(), nullptr);
    canvas_ = *(surface_->GetCanvas());
}

std::shared_ptr<Drawing::Surface> GESDFShaderShapeTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GESDFShaderShapeTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

/**
 * @tc.name: GenerateDrawingShader_001
 * @tc.desc: Verify GenerateDrawingShader always returns nullptr for base class
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShaderShapeTest, GenerateDrawingShader_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GenerateDrawingShader_001 start";
    TestSDFShaderShape shape(GESDFShapeType::RRECT);
    auto shader = shape.GenerateDrawingShader(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GenerateDrawingShader_001 end";
}

/**
 * @tc.name: GenerateDrawingShaderHasNormal_001
 * @tc.desc: Verify GenerateDrawingShaderHasNormal always returns nullptr for base class
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShaderShapeTest, GenerateDrawingShaderHasNormal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GenerateDrawingShaderHasNormal_001 start";
    TestSDFShaderShape shape(GESDFShapeType::RRECT);
    auto shader = shape.GenerateDrawingShaderHasNormal(100.0f, 100.0f);
    EXPECT_EQ(shader, nullptr);
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GenerateDrawingShaderHasNormal_001 end";
}

/**
 * @tc.name: GetInscribedRectDefaultReturnsFalse
 * @tc.desc: Verify GetInscribedRect default implementation returns false for non-overriding shapes
 * @tc.type: FUNC
 */
HWTEST_F(GESDFShaderShapeTest, GetInscribedRectDefaultReturnsFalse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GetInscribedRectDefaultReturnsFalse start";
    TestSDFShaderShape shape(GESDFShapeType::EMPTY);
    Drawing::Rect rect;
    EXPECT_FALSE(shape.GetInscribedRect(rect));
    GTEST_LOG_(INFO) << "GESDFShaderShapeTest GetInscribedRectDefaultReturnsFalse end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS