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
 
#include "gevisualeffectcontainer_fuzzer.h"
#include "ge_render.h"
#include <fuzzer/FuzzedDataProvider.h>
 
namespace OHOS {
namespace Rosen {
namespace Drawing {
 
namespace {
constexpr size_t STR_LEN = 32;
constexpr size_t EFFECT_COUNT_MAX = 5;
constexpr uint8_t FUZZER_TEST_CASE_COUNT = 10;
 
enum TestCaseIndex {
    TEST_CASE_ADD_GET_FILTERS = 0,
    TEST_CASE_ADD_NULL_FILTER = 1,
    TEST_CASE_MULTI_FILTERS_GET = 2,
    TEST_CASE_PREDEFINED_FILTERS = 3,
    TEST_CASE_FIND_EFFECT_BY_NAME = 4,
    TEST_CASE_SET_GEOMETRY = 5,
    TEST_CASE_UPDATE_CACHE_DATA = 6,
    TEST_CASE_REMOVE_FILTER_TYPE = 7,
    TEST_CASE_SET_HEADROOM = 8,
    TEST_CASE_FROSTED_GLASS_DARK_SCALE = 9
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
 
Drawing::RectF CreateFuzzedRectF(FuzzedDataProvider& fdp)
{
    Drawing::RectF rectF;
    rectF.SetLeft(fdp.ConsumeFloatingPoint<float>());
    rectF.SetTop(fdp.ConsumeFloatingPoint<float>());
    float width = fdp.ConsumeFloatingPoint<float>();
    float height = fdp.ConsumeFloatingPoint<float>();
    rectF.SetRight(rectF.GetLeft() + width);
    rectF.SetBottom(rectF.GetTop() + height);
    return rectF;
}
 
void GEVisualEffectContainerFuzzTest001(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffect);
    veContainer->GetFilters();
}
 
void GEVisualEffectContainerFuzzTest002(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    if (fdp.ConsumeBool()) {
        std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
        DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
        auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
        veContainer->AddToChainedFilter(visualEffect);
    } else {
        veContainer->AddToChainedFilter(nullptr);
    }
}
 
void GEVisualEffectContainerFuzzTest003(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    size_t effectCount = fdp.ConsumeIntegral<uint8_t>() % EFFECT_COUNT_MAX;
    for (size_t i = 0; i <= effectCount; i++) {
        std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
        DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
        auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
        veContainer->AddToChainedFilter(visualEffect);
    }
    veContainer->GetFilters();
}
 
void GEVisualEffectContainerFuzzTest004(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    std::vector<std::string> predefinedNames = {
        Drawing::GE_FILTER_KAWASE_BLUR,
        Drawing::GE_FILTER_MESA_BLUR,
        Drawing::GE_FILTER_GREY,
        Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR,
        Drawing::GE_FILTER_MAGNIFIER,
        Drawing::GE_FILTER_WATER_RIPPLE,
        Drawing::GE_FILTER_AI_BAR,
    };
    size_t nameIndex = fdp.ConsumeIntegral<uint8_t>() % predefinedNames.size();
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto visualEffect = std::make_shared<GEVisualEffect>(predefinedNames[nameIndex], type);
 
    float param1 = fdp.ConsumeFloatingPoint<float>();
    visualEffect->SetParam("radius", param1);
 
    veContainer->AddToChainedFilter(visualEffect);
    veContainer->GetFilters();
}
 
void GEVisualEffectContainerFuzzTest005(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
 
    std::string name1 = fdp.ConsumeRandomLengthString(STR_LEN);
    auto visualEffect1 = std::make_shared<GEVisualEffect>(name1, DrawingPaintType::BRUSH);
    veContainer->AddToChainedFilter(visualEffect1);
 
    std::string name2 = fdp.ConsumeRandomLengthString(STR_LEN);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(name2, DrawingPaintType::PEN);
    veContainer->AddToChainedFilter(visualEffect2);
 
    std::string searchName = fdp.ConsumeRandomLengthString(STR_LEN);
    auto foundEffect = veContainer->GetGEVisualEffect(searchName);
}
 
void GEVisualEffectContainerFuzzTest006(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
 
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffect);
 
    Drawing::Matrix matrix = CreateFuzzedMatrix(fdp);
    Drawing::RectF bound = CreateFuzzedRectF(fdp);
    Drawing::RectF materialDst = CreateFuzzedRectF(fdp);
    float geoWidth = fdp.ConsumeFloatingPoint<float>();
    float geoHeight = fdp.ConsumeFloatingPoint<float>();
 
    veContainer->SetGeometry(matrix, bound, materialDst, geoWidth, geoHeight);
    veContainer->GetFilters();
}
 
void GEVisualEffectContainerFuzzTest007(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
    auto sourceContainer = std::make_shared<GEVisualEffectContainer>();
 
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
    sourceContainer->AddToChainedFilter(visualEffect);
 
    auto visualEffectTarget = std::make_shared<GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffectTarget);
 
    veContainer->UpdateCacheDataFrom(sourceContainer);
}
 
void GEVisualEffectContainerFuzzTest008(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
 
    size_t effectCount = fdp.ConsumeIntegral<uint8_t>() % EFFECT_COUNT_MAX;
    for (size_t i = 0; i <= effectCount; i++) {
        std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
        DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
        auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
        veContainer->AddToChainedFilter(visualEffect);
    }
 
    int32_t typeToRemove = fdp.ConsumeIntegral<int32_t>();
    veContainer->RemoveFilterWithType(typeToRemove);
 
    veContainer->GetFilters();
}
 
void GEVisualEffectContainerFuzzTest009(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
 
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto visualEffect = std::make_shared<GEVisualEffect>(name, type);
    veContainer->AddToChainedFilter(visualEffect);
 
    float headroom = fdp.ConsumeFloatingPoint<float>();
    veContainer->SetDisplayHeadroom(headroom);
}
 
void GEVisualEffectContainerFuzzTest010(FuzzedDataProvider& fdp)
{
    auto veContainer = std::make_shared<GEVisualEffectContainer>();
 
    // 添加frosted glass类型的filter
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_FROSTED_GLASS);
    veContainer->AddToChainedFilter(visualEffect);
 
    float darkScale = fdp.ConsumeFloatingPoint<float>();
    veContainer->UpdateDarkScale(darkScale);
}
 
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
 
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
 
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::Drawing::FUZZER_TEST_CASE_COUNT;
    switch (choice) {
        case OHOS::Rosen::Drawing::TEST_CASE_ADD_GET_FILTERS:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest001(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_ADD_NULL_FILTER:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest002(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_MULTI_FILTERS_GET:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest003(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_PREDEFINED_FILTERS:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest004(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_FIND_EFFECT_BY_NAME:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest005(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_SET_GEOMETRY:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest006(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_UPDATE_CACHE_DATA:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest007(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_REMOVE_FILTER_TYPE:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest008(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_SET_HEADROOM:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest009(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_FROSTED_GLASS_DARK_SCALE:
            OHOS::Rosen::Drawing::GEVisualEffectContainerFuzzTest010(fdp);
            break;
        default:
            break;
    }
    return 0;
}