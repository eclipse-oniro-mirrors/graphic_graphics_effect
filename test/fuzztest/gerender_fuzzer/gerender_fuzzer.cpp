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
 
#include "draw/canvas.h"
#include "gerender_fuzzer.h"
#include "ge_render.h"
#include "ge_hps_effect_filter.h"
#include <fuzzer/FuzzedDataProvider.h>
 
namespace OHOS {
namespace GraphicsEffectEngine {
 
using namespace Rosen;
 
namespace {
constexpr size_t VEC_SIZE_MAX = 4;
constexpr size_t FILTER_COUNT_MAX = 5;
constexpr uint8_t TEST_CASE_COUNT = 10;

enum TestCaseIndex {
    TEST_CASE_KAWASE_BLUR_DRAW_IMAGE = 0,
    TEST_CASE_KAWASE_BLUR_APPLY_IMAGE = 1,
    TEST_CASE_NULL_IMAGE_APPLY = 2,
    TEST_CASE_MULTI_EFFECT_FILTERS = 3,
    TEST_CASE_MAGNIFIER_EFFECT = 4,
    TEST_CASE_WATER_RIPPLE_MESA_BLUR = 5,
    TEST_CASE_BORDER_LIGHT_SHADER = 6,
    TEST_CASE_HPS_IMAGE_EFFECT = 7,
    TEST_CASE_HPS_APPLY_EFFECT = 8,
    TEST_CASE_RANDOM_FILTERS = 9
};
}
 
Drawing::Matrix CreateFuzzedMatrix(FuzzedDataProvider& fdp)
{
    Drawing::Matrix matrix;
    matrix.SetMatrix(fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>(),
                     fdp.ConsumeFloatingPoint<float>());
    return matrix;
}
 
Drawing::Rect CreateFuzzedRect(FuzzedDataProvider& fdp)
{
    float left = fdp.ConsumeFloatingPoint<float>();
    float top = fdp.ConsumeFloatingPoint<float>();
    float right = fdp.ConsumeFloatingPoint<float>();
    float bottom = fdp.ConsumeFloatingPoint<float>();
    return Drawing::Rect(left, top, right, bottom);
}
 
void GERenderFuzzTest001(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    int32_t radius = fdp.ConsumeIntegral<int32_t>();
    visualEffect->SetParam("KAWASE_BLUR_RADIUS", radius);
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
 
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
 
    geRender->DrawImageEffect(canvas, *veContainer, image, src, dst, sampling);
}
 
void GERenderFuzzTest002(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    int32_t radius = fdp.ConsumeIntegral<int32_t>();
    visualEffect->SetParam("KAWASE_BLUR_RADIUS", radius);
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    veContainer->AddToChainedFilter(visualEffect);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
 
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
 
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
void GERenderFuzzTest003(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::shared_ptr<Drawing::Image> image = nullptr;
    if (fdp.ConsumeBool()) {
        image = std::make_shared<Drawing::Image>();
    }
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
    Drawing::SamplingOptions sampling;
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
void GERenderFuzzTest004(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectGrey = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_GREY);
    auto visualEffectAIBar = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_AI_BAR);
    auto visualEffectLinear = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
 
    float greyCoef1 = fdp.ConsumeFloatingPoint<float>();
    float greyCoef2 = fdp.ConsumeFloatingPoint<float>();
    visualEffectGrey->SetParam("GREY_COEF_1", greyCoef1);
    visualEffectGrey->SetParam("GREY_COEF_2", greyCoef2);
 
    Drawing::Matrix matrix = CreateFuzzedMatrix(fdp);
    std::vector<std::pair<float, float>> fractionStops;
    size_t stopCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    for (size_t i = 0; i <= stopCount; i++) {
        fractionStops.push_back({fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()});
    }
    float blurRadius = fdp.ConsumeFloatingPoint<float>();
    float geoWidth = fdp.ConsumeFloatingPoint<float>();
    float geoHeight = fdp.ConsumeFloatingPoint<float>();
    float tranX = fdp.ConsumeFloatingPoint<float>();
    float tranY = fdp.ConsumeFloatingPoint<float>();
    int32_t direction = fdp.ConsumeIntegral<int32_t>();
    bool isOffscreen = fdp.ConsumeBool();
 
    visualEffectLinear->SetParam("BLURRADIUS", blurRadius);
    visualEffectLinear->SetParam("GEOWIDTH", geoWidth);
    visualEffectLinear->SetParam("GEOHEIGHT", geoHeight);
    visualEffectLinear->SetParam("TRANX", tranX);
    visualEffectLinear->SetParam("TRANY", tranY);
    visualEffectLinear->SetParam("CANVASMAT", matrix);
    visualEffectLinear->SetParam("FRACTIONSTOPS", fractionStops);
    visualEffectLinear->SetParam("DIRECTION", direction);
    visualEffectLinear->SetParam("ISOFFSCREEN", isOffscreen);
 
    veContainer->AddToChainedFilter(visualEffectGrey);
    veContainer->AddToChainedFilter(visualEffectAIBar);
    veContainer->AddToChainedFilter(visualEffectLinear);
 
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
    Drawing::SamplingOptions sampling;
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
void GERenderFuzzTest005(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectMagnifier = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MAGNIFIER);
 
    visualEffectMagnifier->SetParam("FACTOR", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("WIDTH", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("HEIGHT", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("CORNERRADIUS", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("BORDERWIDTH", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("SHADOWOFFSETX", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("SHADOWOFFSETY", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("SHADOWSIZE", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("SHADOWSTRENGTH", fdp.ConsumeFloatingPoint<float>());
    visualEffectMagnifier->SetParam("GRADIENTMASKCOLOR1", fdp.ConsumeIntegral<uint32_t>());
    visualEffectMagnifier->SetParam("GRADIENTMASKCOLOR2", fdp.ConsumeIntegral<uint32_t>());
    visualEffectMagnifier->SetParam("OUTERCONTOURCOLOR1", fdp.ConsumeIntegral<uint32_t>());
    visualEffectMagnifier->SetParam("OUTERCONTOURCOLOR2", fdp.ConsumeIntegral<uint32_t>());
    visualEffectMagnifier->SetParam("ROTATEDEGREE", fdp.ConsumeIntegral<int32_t>());
 
    veContainer->AddToChainedFilter(visualEffectMagnifier);
 
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
    Drawing::SamplingOptions sampling;
 
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
void GERenderFuzzTest006(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto visualEffectWaterRipple = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_WATER_RIPPLE);
    auto visualEffectMesa = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR);
 
    visualEffectWaterRipple->SetParam("PROGRESS", fdp.ConsumeFloatingPoint<float>());
    visualEffectWaterRipple->SetParam("WAVE_NUM", fdp.ConsumeIntegral<int32_t>());
    visualEffectWaterRipple->SetParam("RIPPLE_CENTER_X", fdp.ConsumeFloatingPoint<float>());
    visualEffectWaterRipple->SetParam("RIPPLE_CENTER_Y", fdp.ConsumeFloatingPoint<float>());
    visualEffectWaterRipple->SetParam("RIPPLE_MODE", fdp.ConsumeIntegral<int32_t>());
 
    visualEffectMesa->SetParam("MESA_BLUR_RADIUS", fdp.ConsumeIntegral<int32_t>());
    visualEffectMesa->SetParam("MESA_BLUR_GREY_COEF_1", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("MESA_BLUR_GREY_COEF_2", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("OFFSET_X", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("OFFSET_Y", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("OFFSET_Z", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("OFFSET_W", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("TILE_MODE", fdp.ConsumeIntegral<int32_t>());
    visualEffectMesa->SetParam("WIDTH", fdp.ConsumeFloatingPoint<float>());
    visualEffectMesa->SetParam("HEIGHT", fdp.ConsumeFloatingPoint<float>());
 
    veContainer->AddToChainedFilter(visualEffectWaterRipple);
    veContainer->AddToChainedFilter(visualEffectMesa);
 
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
    Drawing::SamplingOptions sampling;
 
    auto resImg = geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
void GERenderFuzzTest007(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
 
    auto visualEffectBorderLight = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_SHADER_BORDER_LIGHT);
    visualEffectBorderLight->SetParam("BORDER_LIGHT_WIDTH", fdp.ConsumeFloatingPoint<float>());
    visualEffectBorderLight->SetParam("BORDER_LIGHT_RADIUS", fdp.ConsumeFloatingPoint<float>());
 
    veContainer->AddToChainedFilter(visualEffectBorderLight);
 
    Drawing::Rect bounds = CreateFuzzedRect(fdp);
    geRender->DrawShaderEffect(canvas, *veContainer, bounds);
}
 
void GERenderFuzzTest008(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    Drawing::Brush brush;
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
 
    auto visualEffectKawase = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    int32_t radius = fdp.ConsumeIntegral<int32_t>();
    visualEffectKawase->SetParam("KAWASE_BLUR_RADIUS", radius);
    veContainer->AddToChainedFilter(visualEffectKawase);
 
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> outImage = nullptr;
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
 
    GERender::HpsGEImageEffectContext context;
    context.image = image;
    context.src = src;
    context.dst = dst;
    context.alpha = fdp.ConsumeFloatingPoint<float>();
    context.saturationForHPS = fdp.ConsumeFloatingPoint<float>();
    context.brightnessForHPS = fdp.ConsumeFloatingPoint<float>();
 
    geRender->ApplyHpsGEImageEffect(canvas, *veContainer, context, outImage, brush);
}
 
void GERenderFuzzTest009(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    Drawing::Canvas canvas;
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
 
    auto visualEffectKawase = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    int32_t radius = fdp.ConsumeIntegral<int32_t>();
    visualEffectKawase->SetParam("KAWASE_BLUR_RADIUS", radius);
    veContainer->AddToChainedFilter(visualEffectKawase);
 
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> outImage = nullptr;
 
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
 
    GERender::HpsGEImageEffectContext context;
    context.image = image;
    context.src = src;
    context.dst = dst;
    context.alpha = fdp.ConsumeFloatingPoint<float>();
    context.saturationForHPS = fdp.ConsumeFloatingPoint<float>();
    context.brightnessForHPS = fdp.ConsumeFloatingPoint<float>();
 
    geRender->ApplyHpsImageEffect(canvas, *veContainer, context, outImage);
}
 
void GERenderFuzzTest010(FuzzedDataProvider& fdp)
{
    auto geRender = std::make_shared<GERender>();
    auto veContainer = std::make_shared<Drawing::GEVisualEffectContainer>();

    std::vector<std::string> predefinedNames = {
        Drawing::GE_FILTER_KAWASE_BLUR,
        Drawing::GE_FILTER_MESA_BLUR,
        Drawing::GE_FILTER_GREY,
        Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR,
        Drawing::GE_FILTER_MAGNIFIER,
        Drawing::GE_FILTER_WATER_RIPPLE,
        Drawing::GE_FILTER_AI_BAR,
        Drawing::GE_FILTER_FROSTED_GLASS,
        Drawing::GE_FILTER_SDF_EDGE_LIGHT,
    };
    size_t filterCount = fdp.ConsumeIntegral<uint8_t>() % FILTER_COUNT_MAX;
    for (size_t i = 0; i <= filterCount; i++) {
        size_t nameIndex = fdp.ConsumeIntegral<uint8_t>() % predefinedNames.size();
        Drawing::DrawingPaintType type = static_cast<Drawing::DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
        auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(predefinedNames[nameIndex], type);
        visualEffect->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, fdp.ConsumeIntegral<int32_t>());
        visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_1, fdp.ConsumeFloatingPoint<float>());
        visualEffect->SetParam(Drawing::GE_FILTER_GREY_COEF_2, fdp.ConsumeFloatingPoint<float>());
        veContainer->AddToChainedFilter(visualEffect);
    }

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src = CreateFuzzedRect(fdp);
    Drawing::Rect dst = CreateFuzzedRect(fdp);
    Drawing::SamplingOptions sampling;
    geRender->DrawImageEffect(canvas, *veContainer, image, src, dst, sampling);
    geRender->ApplyImageEffect(canvas, *veContainer, {image, src, dst}, sampling);
}
 
}  // namespace GraphicsEffectEngine
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);

    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % OHOS::GraphicsEffectEngine::TEST_CASE_COUNT;
    switch (choice) {
        case OHOS::GraphicsEffectEngine::TEST_CASE_KAWASE_BLUR_DRAW_IMAGE:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest001(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_KAWASE_BLUR_APPLY_IMAGE:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest002(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_NULL_IMAGE_APPLY:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest003(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_MULTI_EFFECT_FILTERS:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest004(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_MAGNIFIER_EFFECT:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest005(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_WATER_RIPPLE_MESA_BLUR:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest006(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_BORDER_LIGHT_SHADER:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest007(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_HPS_IMAGE_EFFECT:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest008(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_HPS_APPLY_EFFECT:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest009(fdp);
            break;
        case OHOS::GraphicsEffectEngine::TEST_CASE_RANDOM_FILTERS:
            OHOS::GraphicsEffectEngine::GERenderFuzzTest010(fdp);
            break;
        default:
            break;
    }
    return 0;
}