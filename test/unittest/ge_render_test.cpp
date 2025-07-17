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

#include "ge_render.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GERenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;

private:
    std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();

    std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
};

void GERenderTest::SetUpTestCase(void) {}

void GERenderTest::TearDownTestCase(void) {}

void GERenderTest::SetUp() {}

void GERenderTest::TearDown() {}

std::shared_ptr<Drawing::RuntimeEffect> GERenderTest::MakeGreyAdjustmentEffect()
{
    static const std::string GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        vec4 main(vec2 drawing_coord) {
            vec3 color = vec3(imageShader(drawing_coord).r, imageShader(drawing_coord).g, imageShader(drawing_coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");
    if (!greyAdjustEffect_) {
        std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect =
            Drawing::RuntimeEffect::CreateForShader(GreyGradationString);
        if (!greyAdjustEffect) {
            return nullptr;
        }
        greyAdjustEffect_ = std::move(greyAdjustEffect);
    }

    return greyAdjustEffect_;
}

std::shared_ptr<Drawing::Image> GERenderTest::MakeImage(Drawing::Canvas& canvas)
{
    auto image = std::make_shared<Drawing::Image>();
    if (image == nullptr) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage image is null";
        return nullptr;
    }
    float greyX = 0.0f;
    float greyY = 1.0f;

    auto greyAdjustEffect = MakeGreyAdjustmentEffect();
    if (!greyAdjustEffect) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage greyAdjustEffect is null";
        return nullptr;
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(greyAdjustEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("coefficient1", greyX);
    builder->SetUniform("coefficient2", greyY);
    return builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
}

/**
 * @tc.name: DrawImageEffect_001
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    const std::shared_ptr<Drawing::Image> image = nullptr;
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    geRender->DrawImageEffect(canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_001 end";
}

/**
 * @tc.name: DrawImageEffect_002
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_002 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 0);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = std::make_shared<Drawing::Image>();
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    geRender->DrawImageEffect(canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_002 end";
}

/**
 * @tc.name: DrawImageEffect_003
 * @tc.desc: Verify the DrawImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, DrawImageEffect_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_003 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    geRender->DrawImageEffect(canvas_, *veContainer, image, src, dst, sampling);

    GTEST_LOG_(INFO) << "GERenderTest DrawImageEffect_003 end";
}

/**
 * @tc.name: ApplyImageEffect_001
 * @tc.desc: Verify the ApplyImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyImageEffect001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);

    const std::shared_ptr<Drawing::Image> image = nullptr;
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    auto outImage = geRender->ApplyImageEffect(canvas_, *veContainer, image, src, dst, sampling);
    EXPECT_TRUE(outImage == image);

    GTEST_LOG_(INFO) << "GERenderTest ApplyImageEffect_001 end";
}

/**
 * @tc.name: ApplyGEEffects001
 * @tc.desc: Verify the ApplyGEEffects
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyGEEffects001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects001 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
 
    std::vector<std::shared_ptr<Drawing::GEVisualEffect>> visualEffects;
    visualEffects.push_back(visualEffect);
 
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    std::shared_ptr<Drawing::Image> outImage;
    GERender::HpsGEImageEffectContext context { image, src, src, Drawing::SamplingOptions(), false };
    geRender->ApplyGEEffects(canvas_, visualEffects, context, outImage);
    EXPECT_NE(outImage, image);
 
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects001 end";
}
 
/**
 * @tc.name: ApplyGEEffects002
 * @tc.desc: Verify the ApplyGEEffects
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyGEEffects002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects002 start";
    std::vector<std::shared_ptr<Drawing::GEVisualEffect>> visualEffects;
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    std::shared_ptr<Drawing::Image> outImage;
    GERender::HpsGEImageEffectContext context { image, src, src, Drawing::SamplingOptions(), false };
    geRender->ApplyGEEffects(canvas_, visualEffects, context, outImage);
    EXPECT_EQ(outImage, image);
 
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects002 end";
}
 
/**
 * @tc.name: ApplyGEEffects003
 * @tc.desc: Verify the ApplyGEEffects
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyGEEffects003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects003 start";
 
    std::shared_ptr<Drawing::GEVisualEffect> visualEffect = nullptr;
    std::vector<std::shared_ptr<Drawing::GEVisualEffect>> visualEffects;
    visualEffects.push_back(visualEffect);
 
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    std::shared_ptr<Drawing::Image> outImage;
    GERender::HpsGEImageEffectContext context { image, src, src, Drawing::SamplingOptions(), false };
    geRender->ApplyGEEffects(canvas_, visualEffects, context, outImage);
    EXPECT_EQ(outImage, image);
 
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects003 end";
}

/**
 * @tc.name: ApplyGEEffects004
 * @tc.desc: Verify the ApplyGEEffects
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyGEEffects004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects004 start";

    auto visualEffect1 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect1->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 1.0f);
    visualEffect1->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 1.0f);

    auto visualEffect2 = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    visualEffect2->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
 
    std::vector<std::shared_ptr<Drawing::GEVisualEffect>> visualEffects;
    visualEffects.push_back(visualEffect1);
    visualEffects.push_back(visualEffect2);

    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GERenderTest image is null";
        return;
    }
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::SamplingOptions sampling;
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    std::shared_ptr<Drawing::Image> outImage;
    GERender::HpsGEImageEffectContext context { image, src, src, Drawing::SamplingOptions(), false };
    geRender->ApplyGEEffects(canvas_, visualEffects, context, outImage);
    EXPECT_NE(outImage, image);

    GTEST_LOG_(INFO) << "GERenderTest ApplyGEEffects004 end";
}

using IndexRange = GERender::IndexRange;

/**
 * @tc.name: CategorizeRanges001
 * @tc.desc: Verify the CategorizeRanges
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, CategorizeRanges001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges001 start";
 
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    vector<IndexRange> hpsRanges = { { 3, 5 } };
    int32_t veSize = 10;
    auto result = geRender->CategorizeRanges(hpsRanges, veSize);
    EXPECT_EQ(result.size(), 3);
 
    EXPECT_EQ(result[0].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[0].range[0], 0);
    EXPECT_EQ(result[0].range[1], 2);
 
    EXPECT_EQ(result[1].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[1].range[0], 3);
    EXPECT_EQ(result[1].range[1], 5);
 
    EXPECT_EQ(result[2].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[2].range[0], 6);
    EXPECT_EQ(result[2].range[1], 9);
 
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges001 end";
}
 
/**
 * @tc.name: CategorizeRanges002
 * @tc.desc: Verify the CategorizeRanges
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, CategorizeRanges002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges002 start";
 
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    vector<IndexRange> hpsRanges = { { 2, 4 }, { 7, 9 } };
    int32_t veSize = 10;
    auto result = geRender->CategorizeRanges(hpsRanges, veSize);
    EXPECT_EQ(result.size(), 4);
 
    EXPECT_EQ(result[0].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[0].range[0], 0);
    EXPECT_EQ(result[0].range[1], 1);
 
    EXPECT_EQ(result[1].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[1].range[0], 2);
    EXPECT_EQ(result[1].range[1], 4);
 
    EXPECT_EQ(result[2].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[2].range[0], 5);
    EXPECT_EQ(result[2].range[1], 6);
 
    EXPECT_EQ(result[3].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[3].range[0], 7);
    EXPECT_EQ(result[3].range[1], 9);
 
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges002 end";
}
 
/**
 * @tc.name: CategorizeRanges003
 * @tc.desc: Verify the CategorizeRanges
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, CategorizeRanges003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges003 start";
 
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    vector<IndexRange> hpsRanges = { { 2, 5 }, { 6, 8 } };
    int32_t veSize = 10;
    auto result = geRender->CategorizeRanges(hpsRanges, veSize);
    EXPECT_EQ(result.size(), 4);
 
    EXPECT_EQ(result[0].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[0].range[0], 0);
    EXPECT_EQ(result[0].range[1], 1);
 
    EXPECT_EQ(result[1].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[1].range[0], 2);
    EXPECT_EQ(result[1].range[1], 5);
 
    EXPECT_EQ(result[2].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[2].range[0], 6);
    EXPECT_EQ(result[2].range[1], 8);
 
    EXPECT_EQ(result[3].mode, GERender::EffectMode::GE);
    EXPECT_EQ(result[3].range[0], 9);
    EXPECT_EQ(result[3].range[1], 9);
 
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges003 end";
}
 
/**
 * @tc.name: CategorizeRanges004
 * @tc.desc: Verify the CategorizeRanges
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, CategorizeRanges004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges004 start";
 
    auto geRender = std::make_shared<GERender>();
    if (!geRender) {
        GTEST_LOG_(INFO) << "GERenderTest geRender is null";
        return;
    }
    vector<IndexRange> hpsRanges = { { 0, 0 } };
    int32_t veSize = 1;
    auto result = geRender->CategorizeRanges(hpsRanges, veSize);
    EXPECT_EQ(result.size(), 1);
 
    EXPECT_EQ(result[0].mode, GERender::EffectMode::HPS);
    EXPECT_EQ(result[0].range[0], 0);
    EXPECT_EQ(result[0].range[1], 0);
 
    GTEST_LOG_(INFO) << "GERenderTest CategorizeRanges004 end";
}

/**
 * @tc.name: GenerateShaderFilters_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_AI_BAR);
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_LOW, 1.0f); // 1.0 AI bar blur low
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_HIGH, 1.0f); // 1.0 AI bar blur high
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_THRESHOLD, 1.0f); // 1.0 AI bar blur threshold
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_OPACITY, 1.0f); // 1.0 AI bar blur opacity
    visualEffect->SetParam(Drawing::GE_FILTER_AI_BAR_SATURATION, 1.0f); // 1.0 AI bar blur saturation
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_001 end";
}

/**
 * @tc.name: GenerateShaderFilters_002
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_002 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, 1.0f); // 1.0 grey blur coff
    visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, 1.0f); // 1.0 grey blur coff
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_002 end";
}

/**
 * @tc.name: GenerateShaderFilters_003
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_003 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 1); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_003 end";
}

/**
 * @tc.name: GenerateShaderFilters_004
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_004 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("");
    visualEffect->SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 1); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_004 end";
}

/**
 * @tc.name: GenerateShaderFilters_008
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_008 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>("");
    std::pair<float, float> factor = {1.0f, 1.0f};
    visualEffect->SetParam(Drawing::GE_FILTER_DISPLACEMENT_DISTORT_FACTOR, factor); // 1 blur directon
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_008 end";
}

/**
 * @tc.name: GenerateShaderFiltersMESA_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersMESA_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersMESA_001 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 1); // 1 blur directon
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, 1.0f); // 1.0 grey blur coff
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, 1.0f); // 1.0 grey blur coff
    // 0, 0.0: tileMode and stretch params
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.0f);
    visualEffect->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.0f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);

    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersMESA_001 end";
}

/**
 * @tc.name: GenerateShaderFilters_005
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_005 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_WATER_RIPPLE);
    visualEffect->SetParam("PROGRESS", 0.5f);
    visualEffect->SetParam("WAVE_NUM", 1.0f);
    visualEffect->SetParam("RIPPLE_CENTER_X", 0.5f);
    visualEffect->SetParam("RIPPLE_CENTER_Y", 0.5f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_005 end";
}

/**
 * @tc.name: GenerateShaderFilters_006
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_006 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MAGNIFIER);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_006 end";
}

/**
 * @tc.name: GenerateShaderFilters_007
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_007 start";

    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_COLOR_GRADIENT);
    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f };
    std::vector<float> positions = { 1.0f, 1.0f }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5f }; // 0.5 is strength params
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_COLOR, colors);
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_POSITION, positions);
    visualEffect->SetParam(Drawing::GE_FILTER_COLOR_GRADIENT_STRENGTH, strengths);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_007 end";
}

/**
 * @tc.name: GenerateShaderFilters_009
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_009 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_BEZIER_WARP);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_009 end";
}

/**
 * @tc.name: GenerateShaderFilters_010
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFilters_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_010 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_CONTENT_LIGHT);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFilters_010 end";
}

/**
 * @tc.name: GenerateShaderFiltersEdgelight_001
 * @tc.desc: Verify the GenerateShaderFilters
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersEdgelight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersEdgelight_001 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);
    visualEffect->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 1.0);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_NE(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersEdgelight_001 end";
}

/**
 * @tc.name: GenerateShaderFiltersDispersion_001
 * @tc.desc: Verify the GenerateShaderFiltersDispersion
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, GenerateShaderFiltersDispersion_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersDispersion_001 start";
 
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_DISPERSION);
    visualEffect->SetParam(Drawing::GE_FILTER_DISPERSION_OPACITY, 0.5f);
    Drawing::GEVisualEffectContainer veContainer;
    veContainer.AddToChainedFilter(visualEffect);
    auto geRender = std::make_shared<GERender>();
    auto shaderFilters = geRender->GenerateShaderFilters(veContainer);
    EXPECT_EQ(shaderFilters[0], nullptr);
 
    GTEST_LOG_(INFO) << "GERenderTest GenerateShaderFiltersDispersion_001 end";
}

/**
 * @tc.name: ApplyHpsImageEffect_001
 * @tc.desc: Verify the ApplyHpsImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsImageEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsImageEffect_001 start";

    Drawing::GEVisualEffectContainer veContainer;
    const std::shared_ptr<Drawing::Image> image = nullptr;
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    float alpha = 1.0f;
    std::shared_ptr<Drawing::ColorFilter> colorFilter;
    uint32_t maskColor = 255;
    float saturationForHPS = 1.0f;
    float brightnessForHPS = 1.0f;
    GERender::HpsGEImageEffectContext context = {image, src, dst, Drawing::SamplingOptions(), true, alpha,
        colorFilter, maskColor, saturationForHPS, brightnessForHPS};

    auto geRender = std::make_shared<GERender>();

    /* image is nullptr*/
    geRender->ApplyHpsImageEffect(canvas_, veContainer, context, outImage);

    /* no filter*/
    auto image2 = MakeImage(canvas_);
    context.image = image2;
    geRender->ApplyHpsImageEffect(canvas_, veContainer, context, outImage);

    /* normal case */
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);
    visualEffect->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 1.0);
    veContainer.AddToChainedFilter(visualEffect);
    EXPECT_EQ(geRender->ApplyHpsImageEffect(canvas_, veContainer, context, outImage), false);

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsImageEffect_001 end";
}

/**
 * @tc.name: ApplyHpsGEImageEffect_001
 * @tc.desc: Verify the ApplyHpsGEImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(GERenderTest, ApplyHpsGEImageEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_001 start";

    Drawing::GEVisualEffectContainer veContainer;
    const std::shared_ptr<Drawing::Image> image = nullptr;
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    const Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Brush brush;

    auto geRender = std::make_shared<GERender>();

    /* image is nullptr */
    GERender::HpsGEImageEffectContext context1 { image, src, dst, Drawing::SamplingOptions(), false };
    geRender->ApplyHpsGEImageEffect(canvas_, veContainer, context1, outImage, brush);
 

    /* no filter */
    auto image2 = MakeImage(canvas_);
    GERender::HpsGEImageEffectContext context2 { image2, src, dst, Drawing::SamplingOptions(), false };
    geRender->ApplyHpsGEImageEffect(canvas_, veContainer, context2, outImage, brush);
 
    /* normal case */
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_EDGE_LIGHT);
    visualEffect->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, 1.0);
    veContainer.AddToChainedFilter(visualEffect);
    GERender::HpsGEImageEffectContext context3 { image2, src, dst, Drawing::SamplingOptions(), false };
    EXPECT_EQ(geRender->ApplyHpsGEImageEffect(canvas_, veContainer, context3, outImage, brush), false);

    /* compatibility */
    GERender::HpsGEImageEffectContext context4 { image2, src, dst, Drawing::SamplingOptions(), true };
    outImage = nullptr;
    EXPECT_EQ(geRender->ApplyHpsGEImageEffect(canvas_, veContainer, context3, outImage, brush), false);
    EXPECT_EQ(outImage, nullptr);

    GTEST_LOG_(INFO) << "GERenderTest ApplyHpsGEImageEffect_001 end";
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
