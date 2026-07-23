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
#include "ge_cache_helper.h"
#include "ge_shader.h"
#include "ge_log.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "utils/rect.h"
#include "draw/color.h"
#include "draw/path.h"
#include "ge_shader_filter_params.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_shader_shape.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {
class GEShaderEffectTest : public Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Surface> CreateSurface();
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    Drawing::Canvas canvas_;
    Drawing::Rect rect_ = {};
    Drawing::ImageInfo imageInfo_ = {};
};

void GEShaderEffectTest::TearDown()
{
    surface_ = nullptr;
}

void GEShaderEffectTest::SetUp()
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

std::shared_ptr<Drawing::Surface> GEShaderEffectTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEShaderEffectTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

struct SDFImageCacheTest {
    uint32_t hash = 0;
    std::shared_ptr<Drawing::Image> sdfImage = nullptr;
};
using CacheDataType = std::shared_ptr<Drawing::Image>;
class GETestShader : public GEShader {
public:
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override
    {
        static constexpr char prog[] = R"(
            vec4 main(float2 fragCoord)
            {
                return vec4(0.0, 0.0, 0.0, 1.0);
            }
        )";
        auto testShaderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
        auto build = std::make_shared<Drawing::RuntimeShaderBuilder>(testShaderEffect);
        drShader_ = build->MakeShader(nullptr, false);
    }

protected:
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override
    {
        if (cacheAnyPtr_ == nullptr) {
            CacheDataType cacheData = nullptr;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
    }
};

class TestSDFShaderShape : public Drawing::GESDFShaderShape {
public:
    explicit TestSDFShaderShape(Drawing::GESDFShapeType type) : type_(type) {}

    Drawing::GESDFShapeType GetSDFShapeType() const override
    {
        return type_;
    }
    bool HasType(const Drawing::GESDFShapeType type) const override
    {
        return true;
    }
private:
    Drawing::GESDFShapeType type_;
};

class GETestSubstractedShader : public GEShader {
public:
    explicit GETestSubstractedShader(const Drawing::Rect& rect)
    {
        testSubtractedShader = rect;
    }

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override
    {
        static constexpr char prog[] = R"(
            vec4 main(float2 fragCoord)
            {
                return vec4(0.0, 0.0, 0.0, 1.0);
            }
        )";
        auto testShaderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
        auto build = std::make_shared<Drawing::RuntimeShaderBuilder>(testShaderEffect);
        drShader_ = build->MakeShader(nullptr, false);
    }

    Drawing::Rect GetSubtractedRect(float width, float height) const override { return testSubtractedShader; }

protected:
    Drawing::Rect testSubtractedShader;

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override
    {
        if (cacheAnyPtr_ == nullptr) {
            CacheDataType cacheData = nullptr;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
    }
};

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_01, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestShader>();
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_NE(testShader->GetDrawingShader(), nullptr);
    std::shared_ptr<std::any> cachPtr = nullptr;
    testShader->SetCache(cachPtr);
    EXPECT_EQ(testShader->GetCache(), nullptr);
    testShader->DrawShader(canvas_, rect_);
    EXPECT_NE(testShader->GetCache(), nullptr);
    EXPECT_FALSE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_Contain, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(0.25, 0.25, 0.75, 0.75));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_TRUE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_NotContain, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(0.25, 0.25, 1.2, 1.2));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_FALSE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_RenderRectInsideSubRect, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(-0.1, -0.1, 1.2, 1.2));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_TRUE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

/**
 * @tc.name: MakeSDFShaderWithCacheNullShape
 * @tc.desc: Verify MakeSDFShaderWithCache returns nullptr and clears cache when sdfShape is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderEffectTest, MakeSDFShaderWithCacheNullShape, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheNullShape start";
    auto testShader = std::make_shared<GETestShader>();
    std::shared_ptr<Drawing::GESDFShaderShape> nullShape = nullptr;
    auto result = testShader->MakeSDFShaderWithCache(nullShape, canvas_, rect_);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(testShader->GetCache(), nullptr);
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheNullShape end";
}

/**
 * @tc.name: MakeSDFShaderWithCacheHashMismatchNullImage
 * @tc.desc: Verify MakeSDFShaderWithCache returns nullptr when hash mismatches and MakeSDFImage returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderEffectTest, MakeSDFShaderWithCacheHashMismatchNullImage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheHashMismatchNullImage start";
    auto testShader = std::make_shared<GETestShader>();
    auto sdfShape = std::make_shared<TestSDFShaderShape>(Drawing::GESDFShapeType::RRECT);
    sdfShape->SetHash(42);
    testShader->SetCache(nullptr);
    auto result = testShader->MakeSDFShaderWithCache(sdfShape, canvas_, rect_);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(testShader->GetCache(), nullptr);
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheHashMismatchNullImage end";
}

/**
 * @tc.name: MakeSDFShaderWithCacheHashMatchNullImage
 * @tc.desc: Verify MakeSDFShaderWithCache returns nullptr when hash matches cache but cached image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderEffectTest, MakeSDFShaderWithCacheHashMatchNullImage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheHashMatchNullImage start";
    auto testShader = std::make_shared<GETestShader>();
    auto sdfShape = std::make_shared<TestSDFShaderShape>(Drawing::GESDFShapeType::RRECT);
    uint32_t hash = 42;
    sdfShape->SetHash(hash);
    SDFImageCacheTest cachData1{hash, nullptr};
    testShader->SetCache(GECacheHelper::PackCacheAny(std::move(cachData1)));
    auto result = testShader->MakeSDFShaderWithCache(sdfShape, canvas_, rect_);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(testShader->GetCache(), nullptr);

    Drawing::GESDFRRectShapeParams param{};
    param.rrect = {0.0f, 0.0f, 100.0f, 100.0f};
    param.rrect.SetCornerRadius(10.0f, 10.0f);
    auto rectShape = std::make_shared<Drawing::GESDFRRectShaderShape>(param);
    rectShape->SetHash(0);
    SDFImageCacheTest cachData2{hash, nullptr};
    testShader->SetCache(GECacheHelper::PackCacheAny(std::move(cachData2)));
    result = testShader->MakeSDFShaderWithCache(rectShape, canvas_, rect_);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(testShader->GetCache(), nullptr);
    result = testShader->MakeSDFShaderWithCache(rectShape, canvas_, rect_);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(testShader->GetCache(), nullptr);
    GTEST_LOG_(INFO) << "GEShaderEffectTest MakeSDFShaderWithCacheHashMatchNullImage end";
}
}
}