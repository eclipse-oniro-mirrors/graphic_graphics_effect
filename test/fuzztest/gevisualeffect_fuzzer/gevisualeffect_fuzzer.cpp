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
 
#include <cstring>
#include <memory>
#include "gevisualeffect_fuzzer.h"
#include "ge_render.h"
#include <fuzzer/FuzzedDataProvider.h>
 
namespace OHOS {
namespace Rosen {
namespace Drawing {
 
namespace {
constexpr size_t STR_LEN = 32;
constexpr size_t VEC_SIZE_MAX = 4;
constexpr size_t FUZZER_POINT_NUM = 12;
constexpr size_t CORNER_COUNT = 4;
constexpr uint8_t FUZZER_TEST_CASE_COUNT = 9;
 
enum TestCaseIndex {
    TEST_CASE_INT_PARAMS = 0,
    TEST_CASE_FLOAT_PARAMS = 1,
    TEST_CASE_STRING_BOOL_PARAMS = 2,
    TEST_CASE_MATRIX_PARAM = 3,
    TEST_CASE_VECTOR_PARAMS = 4,
    TEST_CASE_SHARED_PTR_PARAMS = 5,
    TEST_CASE_RECT_PARAMS = 6,
    TEST_CASE_VECTOR_COLOR_PARAMS = 7,
    TEST_CASE_POINT_ARRAY_PARAM = 8
};
}
 
void GEVisualEffectFuzzTestIntParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t param32 = fdp.ConsumeIntegral<int32_t>();
    geVisualEffect->SetParam(tag, param32);
 
    int64_t param64 = fdp.ConsumeIntegral<int64_t>();
    geVisualEffect->SetParam(tag, param64);
 
    uint32_t paramU32 = fdp.ConsumeIntegral<uint32_t>();
    geVisualEffect->SetParam(tag, paramU32);
 
    geVisualEffect->GetName();
    geVisualEffect->GetImpl();
}
 
void GEVisualEffectFuzzTestFloatParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
    float paramF = fdp.ConsumeFloatingPoint<float>();
    geVisualEffect->SetParam(tag, paramF);
 
    double paramD = fdp.ConsumeFloatingPoint<double>();
    geVisualEffect->SetParam(tag, paramD);
}
 
void GEVisualEffectFuzzTestStringBoolParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
    std::string paramStr = fdp.ConsumeRandomLengthString(STR_LEN);
    geVisualEffect->SetParam(tag, paramStr.c_str());
 
    bool paramBool = fdp.ConsumeBool();
    geVisualEffect->SetParam(tag, paramBool);
}
 
void GEVisualEffectFuzzTestMatrixParam(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
    Drawing::Matrix param;
    param.SetMatrix(fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>(),
                    fdp.ConsumeFloatingPoint<float>());
    geVisualEffect->SetParam(tag, param);
}
 
void GEVisualEffectFuzzTestVectorParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
 
    size_t pairCount = fdp.ConsumeIntegral<uint8_t>() % VEC_SIZE_MAX;
    std::vector<std::pair<float, float>> pairVec;
    for (size_t i = 0; i <= pairCount; i++) {
        pairVec.push_back({fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()});
    }
    geVisualEffect->SetParam(tag, pairVec);
 
    std::vector<Vector2f> vec2f;
    for (size_t i = 0; i <= pairCount; i++) {
        vec2f.push_back(Vector2f(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()));
    }
    geVisualEffect->SetParam(tag, vec2f);
 
    std::vector<Vector4f> vec4f;
    for (size_t i = 0; i <= pairCount; i++) {
        vec4f.push_back(Vector4f(fdp.ConsumeFloatingPoint<float>(),
                                 fdp.ConsumeFloatingPoint<float>(),
                                 fdp.ConsumeFloatingPoint<float>(),
                                 fdp.ConsumeFloatingPoint<float>()));
    }
    geVisualEffect->SetParam(tag, vec4f);
 
    std::vector<float> floatVec;
    for (size_t i = 0; i <= pairCount; i++) {
        floatVec.push_back(fdp.ConsumeFloatingPoint<float>());
    }
    geVisualEffect->SetParam(tag, floatVec);
}
 
void GEVisualEffectFuzzTestSharedPtrParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
 
    std::shared_ptr<Drawing::Image> imageParam = std::make_shared<Drawing::Image>();
    geVisualEffect->SetParam(tag, imageParam);
}
 
void GEVisualEffectFuzzTestRectParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
 
    GERRect geRect;
    geRect.left_ = fdp.ConsumeFloatingPoint<float>();
    geRect.top_ = fdp.ConsumeFloatingPoint<float>();
    geRect.width_ = fdp.ConsumeFloatingPoint<float>();
    geRect.height_ = fdp.ConsumeFloatingPoint<float>();
    for (size_t i = 0; i < CORNER_COUNT; i++) {
        geRect.radius_[i] = Vector2f(fdp.ConsumeFloatingPoint<float>(),
                                     fdp.ConsumeFloatingPoint<float>());
    }
    geVisualEffect->SetParam(tag, geRect);
 
    Drawing::RectF rectF;
    rectF.SetLeft(fdp.ConsumeFloatingPoint<float>());
    rectF.SetTop(fdp.ConsumeFloatingPoint<float>());
    float width = fdp.ConsumeFloatingPoint<float>();
    float height = fdp.ConsumeFloatingPoint<float>();
    rectF.SetRight(rectF.GetLeft() + width);
    rectF.SetBottom(rectF.GetTop() + height);
    geVisualEffect->SetParam(tag, rectF);
}
 
void GEVisualEffectFuzzTestVectorColorParams(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
 
    Vector3f vec3f(fdp.ConsumeFloatingPoint<float>(),
                   fdp.ConsumeFloatingPoint<float>(),
                   fdp.ConsumeFloatingPoint<float>());
    geVisualEffect->SetParam(tag, vec3f);
 
    Vector4f vec4f(fdp.ConsumeFloatingPoint<float>(),
                   fdp.ConsumeFloatingPoint<float>(),
                   fdp.ConsumeFloatingPoint<float>(),
                   fdp.ConsumeFloatingPoint<float>());
    geVisualEffect->SetParam(tag, vec4f);
 
    Drawing::Color4f color4f;
    color4f.redF_ = fdp.ConsumeFloatingPoint<float>();
    color4f.greenF_ = fdp.ConsumeFloatingPoint<float>();
    color4f.blueF_ = fdp.ConsumeFloatingPoint<float>();
    color4f.alphaF_ = fdp.ConsumeFloatingPoint<float>();
    geVisualEffect->SetParam(tag, color4f);
}
 
void GEVisualEffectFuzzTestPointArrayParam(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
 
    std::array<Drawing::Point, FUZZER_POINT_NUM> pointArray;
    for (size_t i = 0; i < FUZZER_POINT_NUM; i++) {
        pointArray[i] = Drawing::Point(fdp.ConsumeFloatingPoint<float>(),
                                       fdp.ConsumeFloatingPoint<float>());
    }
    geVisualEffect->SetParam(tag, pointArray);
}
 
void GEVisualEffectFuzzTestGetInterfaces(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    DrawingPaintType type = static_cast<DrawingPaintType>(fdp.ConsumeIntegral<uint8_t>());
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
 
    std::string res = geVisualEffect->GetName();
    auto impl = geVisualEffect->GetImpl();
}
 
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
 
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
 
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::Drawing::FUZZER_TEST_CASE_COUNT;
    switch (choice) {
        case OHOS::Rosen::Drawing::TEST_CASE_INT_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestIntParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_FLOAT_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestFloatParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_STRING_BOOL_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestStringBoolParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_MATRIX_PARAM:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestMatrixParam(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_VECTOR_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestVectorParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_SHARED_PTR_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestSharedPtrParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_RECT_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestRectParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_VECTOR_COLOR_PARAMS:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestVectorColorParams(fdp);
            break;
        case OHOS::Rosen::Drawing::TEST_CASE_POINT_ARRAY_PARAM:
            OHOS::Rosen::Drawing::GEVisualEffectFuzzTestPointArrayParam(fdp);
            break;
        default:
            break;
    }
    return 0;
}