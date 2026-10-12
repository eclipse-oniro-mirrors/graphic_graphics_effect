/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "core/ge_effect_factory.h"
#include "ge_distort_light_filter.h"
#include "ge_ripple_shader_mask.h"
#include "ge_shader_mask.h"
#include "ge_visual_effect_impl.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace GraphicsEffectEngine {

using Drawing::GEVisualEffectImpl;
using ::OHOS::GraphicsEffectEngine::GEEffectFactory;

namespace {
// Mask stub whose shader generation always fails; used to drive the
// maskShader == nullptr guard in GEDistortLightFilter::OnProcessImage.
class NullShaderMaskStub : public Drawing::GEShaderMask {
public:
    std::shared_ptr<Drawing::ShaderEffect> GenerateDrawingShader(float width, float height) const override
    {
        return nullptr;
    }

    std::shared_ptr<Drawing::ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override
    {
        return nullptr;
    }
};

std::shared_ptr<Drawing::GERippleShaderMask> MakeRippleMask()
{
    Drawing::GERippleShaderMaskParams maskParams;
    maskParams.center_ = { 0.5f, 0.5f };
    maskParams.radius_ = 0.5f;
    maskParams.width_ = 0.5f;
    maskParams.widthCenterOffset_ = 0.0f;
    return std::make_shared<Drawing::GERippleShaderMask>(maskParams);
}

Drawing::GEDistortLightFilterParams MakeParams(std::shared_ptr<Drawing::GEShaderMask> mask)
{
    Drawing::GEDistortLightFilterParams params;
    params.distortMask = std::move(mask);
    return params;
}
} // namespace

class GEDistortLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    Drawing::CanvasInfo MakeCanvasInfo(const Drawing::Matrix& mat);
    std::shared_ptr<Drawing::Surface> CreateSurface();

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvasGpu_ = nullptr;

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEDistortLightFilterTest::SetUpTestCase(void) {}
void GEDistortLightFilterTest::TearDownTestCase(void) {}

void GEDistortLightFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50 bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();

    surface_ = CreateSurface();
    if (surface_ != nullptr) {
        canvasGpu_ = surface_->GetCanvas();
    }
}

void GEDistortLightFilterTest::TearDown() {}

Drawing::CanvasInfo GEDistortLightFilterTest::MakeCanvasInfo(const Drawing::Matrix& mat)
{
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = 50.0f;
    canvasInfo.geoHeight = 50.0f;
    canvasInfo.mat = mat;
    return canvasInfo;
}

std::shared_ptr<Drawing::Surface> GEDistortLightFilterTest::CreateSurface()
{
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "GEDistortLightFilterTest::CreateSurface create gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, image_->GetImageInfo());
}

/**
 * @tc.name: ConstructorClampsParamsToRange
 * @tc.desc: GEDistortLightFilterTest.ConstructorClampsParamsToRange
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, ConstructorClampsParamsToRange, TestSize.Level1)
{
    Drawing::GEDistortLightFilterParams params = MakeParams(MakeRippleMask());
    params.lightFactor = 5.0f;      // 5.0f exceeds max 1.0f
    params.highLightFactor = -1.0f; // -1.0f below min 0.0f
    params.highLightWidth = 2.0f;
    params.highLightSoft = -3.0f;
    GEDistortLightFilter filter(params);

    EXPECT_FLOAT_EQ(filter.lightFactor_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightFactor_, 0.0f);
    EXPECT_FLOAT_EQ(filter.highLightWidth_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightSoft_, 0.0f);
}

/**
 * @tc.name: ConstructorKeepsInRangeParams
 * @tc.desc: GEDistortLightFilterTest.ConstructorKeepsInRangeParams
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, ConstructorKeepsInRangeParams, TestSize.Level1)
{
    Drawing::GEDistortLightFilterParams params = MakeParams(nullptr);
    GEDistortLightFilter filter(params);

    EXPECT_EQ(filter.distortMask_, nullptr);
    EXPECT_FLOAT_EQ(filter.lightFactor_, 0.5f);
    EXPECT_FLOAT_EQ(filter.highLightFactor_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightWidth_, 0.2f);
    EXPECT_FLOAT_EQ(filter.highLightSoft_, 0.1f);
    EXPECT_FLOAT_EQ(filter.highLightColor_.redF_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightColor_.greenF_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightColor_.blueF_, 1.0f);
    EXPECT_FLOAT_EQ(filter.highLightColor_.alphaF_, 1.0f);
}

/**
 * @tc.name: OnProcessImageNullImageReturnsNull
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageNullImageReturnsNull
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageNullImageReturnsNull, TestSize.Level1)
{
    GEDistortLightFilter filter(MakeParams(MakeRippleMask()));

    EXPECT_EQ(filter.OnProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImageNullDistortMaskReturnsInput
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageNullDistortMaskReturnsInput
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageNullDistortMaskReturnsInput, TestSize.Level1)
{
    GEDistortLightFilter filter(MakeParams(nullptr));

    EXPECT_EQ(filter.OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImageInvalidImageSizeReturnsNull
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageInvalidImageSizeReturnsNull
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageInvalidImageSizeReturnsNull, TestSize.Level1)
{
    // A default-constructed Image is non-null but reports empty (0x0) info — exercises the size guard.
    auto emptyInfoImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(emptyInfoImage, nullptr);
    GEDistortLightFilter filter(MakeParams(MakeRippleMask()));

    EXPECT_EQ(filter.OnProcessImage(canvas_, emptyInfoImage, src_, dst_), nullptr);
}

/**
 * @tc.name: OnProcessImageSingularMatrixReturnsInput
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageSingularMatrixReturnsInput
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageSingularMatrixReturnsInput, TestSize.Level1)
{
    GEDistortLightFilter filter(MakeParams(MakeRippleMask()));
    Drawing::Matrix singular;
    singular.SetScale(0.0f, 0.0f); // det == 0, matrix.Invert fails
    filter.SetShaderFilterCanvasinfo(MakeCanvasInfo(singular));

    EXPECT_EQ(filter.OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImageNullMaskShaderReturnsInput
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageNullMaskShaderReturnsInput
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageNullMaskShaderReturnsInput, TestSize.Level1)
{
    GEDistortLightFilter filter(MakeParams(std::make_shared<NullShaderMaskStub>()));
    filter.SetShaderFilterCanvasinfo(MakeCanvasInfo(Drawing::Matrix()));

    EXPECT_EQ(filter.OnProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: OnProcessImageAppliesDistortLight
 * @tc.desc: GEDistortLightFilterTest.OnProcessImageAppliesDistortLight
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, OnProcessImageAppliesDistortLight, TestSize.Level1)
{
    // MakeImage requires a GPU context; a plain test canvas falls back to the input image.
    ASSERT_NE(canvasGpu_, nullptr);
    GEDistortLightFilter filter(MakeParams(MakeRippleMask()));
    filter.SetShaderFilterCanvasinfo(MakeCanvasInfo(Drawing::Matrix()));

    auto result = filter.OnProcessImage(*canvasGpu_, image_, src_, dst_);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result, image_);
}

/**
 * @tc.name: FactoryCreateWithAllStringTags
 * @tc.desc: GEDistortLightFilterTest.FactoryCreateWithAllStringTags
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, FactoryCreateWithAllStringTags, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>("DistortLight");
    ASSERT_NE(impl, nullptr);

    impl->SetParam("DistortLight_DistortMask", MakeRippleMask());
    impl->SetParam("DistortLight_LightFactor", 0.8f);     // 0.8f within [0.0, 1.0]
    impl->SetParam("DistortLight_HighLightFactor", 5.0f); // 5.0f clamped to max 1.0
    impl->SetParam("DistortLight_HighLightWidth", 0.3f);
    impl->SetParam("DistortLight_HighLightSoft", 0.05f);
    impl->SetParam("DistortLight_HighLightColor", Vector4f(1.0f, 0.5f, 0.25f, 1.0f)); // cast_from path

    auto result = GEEffectFactory::CreateFilter(impl);
    ASSERT_NE(result, nullptr);
    auto filter = std::static_pointer_cast<GEDistortLightFilter>(result);
    ASSERT_NE(filter, nullptr);

    EXPECT_NE(filter->distortMask_, nullptr);
    EXPECT_FLOAT_EQ(filter->lightFactor_, 0.8f);
    EXPECT_FLOAT_EQ(filter->highLightFactor_, 1.0f);
    EXPECT_FLOAT_EQ(filter->highLightWidth_, 0.3f);
    EXPECT_FLOAT_EQ(filter->highLightSoft_, 0.05f);
    EXPECT_FLOAT_EQ(filter->highLightColor_.redF_, 1.0f);
    EXPECT_FLOAT_EQ(filter->highLightColor_.greenF_, 0.5f);
    EXPECT_FLOAT_EQ(filter->highLightColor_.blueF_, 0.25f);
    EXPECT_FLOAT_EQ(filter->highLightColor_.alphaF_, 1.0f);

    // native Color4f setter path
    impl->SetParam("DistortLight_HighLightColor", Drawing::Color4f { 0.2f, 0.4f, 0.6f, 0.8f });
    auto nativeResult = GEEffectFactory::CreateFilter(impl);
    ASSERT_NE(nativeResult, nullptr);
    auto nativeFilter = std::static_pointer_cast<GEDistortLightFilter>(nativeResult);
    ASSERT_NE(nativeFilter, nullptr);
    EXPECT_FLOAT_EQ(nativeFilter->highLightColor_.redF_, 0.2f);
    EXPECT_FLOAT_EQ(nativeFilter->highLightColor_.greenF_, 0.4f);
    EXPECT_FLOAT_EQ(nativeFilter->highLightColor_.blueF_, 0.6f);
    EXPECT_FLOAT_EQ(nativeFilter->highLightColor_.alphaF_, 0.8f);
}

/**
 * @tc.name: FactoryCreateWithoutParams
 * @tc.desc: GEDistortLightFilterTest.FactoryCreateWithoutParams
 * @tc.type: FUNC
 */
HWTEST_F(GEDistortLightFilterTest, FactoryCreateWithoutParams, TestSize.Level1)
{
    auto impl = std::make_shared<GEVisualEffectImpl>("DistortLight");
    ASSERT_NE(impl, nullptr);

    auto result = GEEffectFactory::CreateFilter(impl);
    ASSERT_NE(result, nullptr);
    auto filter = std::static_pointer_cast<GEDistortLightFilter>(result);
    ASSERT_NE(filter, nullptr);

    EXPECT_EQ(filter->distortMask_, nullptr);
    EXPECT_FLOAT_EQ(filter->lightFactor_, 0.5f);
}

} // namespace GraphicsEffectEngine
} // namespace Rosen
} // namespace OHOS
