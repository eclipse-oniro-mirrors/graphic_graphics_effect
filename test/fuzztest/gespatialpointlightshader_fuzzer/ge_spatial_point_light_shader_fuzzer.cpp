/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "ge_spatial_point_light_shader_fuzzer.h"
#include "ge_spatial_point_light.h"
#include "get_object.h"

namespace OHOS {
namespace Rosen {

void GESpatialPointLightShaderFuzzTest()
{
    // Get fuzz data for parameters
    float lightIntensity = GETest::GetPlainData<float>();
    float lightPosX = GETest::GetPlainData<float>();
    float lightPosY = GETest::GetPlainData<float>();
    float lightPosZ = GETest::GetPlainData<float>();
    float attenuation = GETest::GetPlainData<float>();
    float colorR = GETest::GetPlainData<float>();
    float colorG = GETest::GetPlainData<float>();
    float colorB = GETest::GetPlainData<float>();
    float colorA = GETest::GetPlainData<float>();
    
    // Get fuzz data for rect
    float rectLeft = GETest::GetPlainData<float>();
    float rectTop = GETest::GetPlainData<float>();
    float rectWidth = GETest::GetPlainData<float>();
    float rectHeight = GETest::GetPlainData<float>();
    
    // Clamp rect dimensions to reasonable range
    if (rectWidth < 1.0f) rectWidth = 1.0f;
    if (rectWidth > 1000.0f) rectWidth = 1000.0f;
    if (rectHeight < 1.0f) rectHeight = 1.0f;
    if (rectHeight > 1000.0f) rectHeight = 1000.0f;
    
    Drawing::Rect rect(rectLeft, rectTop, rectLeft + rectWidth, rectTop + rectHeight);
    
    // Create params with fuzz data
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = lightIntensity;
    params.lightPosition = Vector3f(lightPosX, lightPosY, lightPosZ);
    params.attenuation = attenuation;
    params.lightColor = Vector4f(colorR, colorG, colorB, colorA);
    params.mask = nullptr;
    
    // Test with params constructor
    auto shaderWithParams = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    shaderWithParams->MakeSpatialPointLightShader(rect);
    shaderWithParams->MakeDrawingShader(rect, colorA);
    auto builder = shaderWithParams->GetSpatialPointLightBuilder();
    
    // Test default constructor and SetSpatialPointLightParams
    auto shaderDefault = std::make_unique<Drawing::GESpatialPointLightShader>();
    shaderDefault->SetSpatialPointLightParams(params);
    shaderDefault->MakeSpatialPointLightShader(rect);
    shaderDefault->MakeDrawingShader(rect, 0.5f);
    
    // Test GetDrawingShader
    auto drawingShader = shaderDefault->GetDrawingShader();
    
    // Test type information
    auto type = shaderDefault->Type();
    auto typeName = shaderDefault->TypeName();
    auto desc = shaderDefault->GetDescription();
}

void GESpatialPointLightShaderBoundaryFuzzTest()
{
    // Test with extreme values
    Drawing::GESpatialPointLightShaderParams params;
    
    // Zero values
    params.lightIntensity = 0.0f;
    params.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    params.attenuation = 0.0f;
    params.lightColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    
    auto shaderZero = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    Drawing::Rect rect1(0, 0, 100, 100);
    shaderZero->MakeSpatialPointLightShader(rect1);
    
    // Large positive values
    params.lightIntensity = 1000.0f;
    params.lightPosition = Vector3f(1000.0f, 1000.0f, 1000.0f);
    params.attenuation = 100.0f;
    params.lightColor = Vector4f(10.0f, 10.0f, 10.0f, 10.0f);
    
    auto shaderLarge = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    shaderLarge->MakeSpatialPointLightShader(rect1);
    
    // Negative values
    params.lightIntensity = -100.0f;
    params.lightPosition = Vector3f(-1000.0f, -1000.0f, -100.0f);
    params.attenuation = -10.0f;
    params.lightColor = Vector4f(-1.0f, -1.0f, -1.0f, -1.0f);
    
    auto shaderNeg = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    shaderNeg->MakeSpatialPointLightShader(rect1);
    
    // Various rect sizes
    Drawing::Rect rects[] = {
        Drawing::Rect(0, 0, 1, 1),
        Drawing::Rect(0, 0, 10, 10),
        Drawing::Rect(0, 0, 500, 500),
        Drawing::Rect(0, 0, 1000, 1000)
    };
    
    for (const auto& rect : rects) {
        auto shader = std::make_unique<Drawing::GESpatialPointLightShader>(params);
        shader->MakeSpatialPointLightShader(rect);
        shader->MakeDrawingShader(rect, 1.0f);
    }
}

void GESpatialPointLightShaderMultipleCallsFuzzTest()
{
    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = GETest::GetPlainData<float>();
    params.lightPosition = Vector3f(
        GETest::GetPlainData<float>(),
        GETest::GetPlainData<float>(),
        GETest::GetPlainData<float>()
    );
    params.attenuation = GETest::GetPlainData<float>();
    params.lightColor = Vector4f(
        GETest::GetPlainData<float>(),
        GETest::GetPlainData<float>(),
        GETest::GetPlainData<float>(),
        GETest::GetPlainData<float>()
    );
    
    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    Drawing::Rect rect(0, 0, 100, 100);
    
    // Multiple MakeSpatialPointLightShader calls
    for (int i = 0; i < 5; i++) {
        shader->MakeSpatialPointLightShader(rect);
    }
    
    // Multiple MakeDrawingShader calls with different alpha
    float alphas[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
    for (float alpha : alphas) {
        shader->MakeDrawingShader(rect, alpha);
    }
    
    // Multiple GetSpatialPointLightBuilder calls
    for (int i = 0; i < 3; i++) {
        auto builder = shader->GetSpatialPointLightBuilder();
    }
    
    // Multiple SetSpatialPointLightParams calls
    for (int i = 0; i < 3; i++) {
        params.lightIntensity = GETest::GetPlainData<float>();
        shader->SetSpatialPointLightParams(params);
        shader->MakeSpatialPointLightShader(rect);
    }
    
    // GetDrawingShader after operations
    auto drawingShader = shader->GetDrawingShader();
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    // initialize
    OHOS::Rosen::GETest::g_data = data;
    OHOS::Rosen::GETest::g_size = size;
    OHOS::Rosen::GETest::g_pos = 0;
    
    /* Run fuzz tests */
    OHOS::Rosen::GESpatialPointLightShaderFuzzTest();
    OHOS::Rosen::GESpatialPointLightShaderBoundaryFuzzTest();
    OHOS::Rosen::GESpatialPointLightShaderMultipleCallsFuzzTest();
    
    return 0;
}