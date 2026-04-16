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

std::string GetDescriptionFuzzTest()
{
    float lightIntensity = GETest::GetPlainData<float>();
    float lightPosX = GETest::GetPlainData<float>();
    float lightPosY = GETest::GetPlainData<float>();
    float lightPosZ = GETest::GetPlainData<float>();
    float attenuation = GETest::GetPlainData<float>();
    float colorR = GETest::GetPlainData<float>();
    float colorG = GETest::GetPlainData<float>();
    float colorB = GETest::GetPlainData<float>();
    float colorA = GETest::GetPlainData<float>();

    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = lightIntensity;
    params.lightPosition = Vector3f(lightPosX, lightPosY, lightPosZ);
    params.attenuation = attenuation;
    params.lightColor = Vector4f(colorR, colorG, colorB, colorA);

    auto shader = std::make_unique<Drawing::GESpatialPointLightShader>(params);
    std::string res = shader->GetDescription();
    return res;
}

std::shared_ptr<Drawing::ShaderEffect> MakeSpatialPointLightShaderFuzzTest()
{
    float rectLeft = GETest::GetPlainData<float>();
    float rectTop = GETest::GetPlainData<float>();
    float rectRight = GETest::GetPlainData<float>();
    float rectBottom = GETest::GetPlainData<float>();
    Drawing::Rect rect{rectLeft, rectTop, rectRight, rectBottom};

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
    auto res = shader->MakeSpatialPointLightShader(rect);
    return res;
}

void MakeDrawingShaderFuzzTest()
{
    float rectLeft = GETest::GetPlainData<float>();
    float rectTop = GETest::GetPlainData<float>();
    float rectRight = GETest::GetPlainData<float>();
    float rectBottom = GETest::GetPlainData<float>();
    Drawing::Rect rect{rectLeft, rectTop, rectRight, rectBottom};
    float progress = GETest::GetPlainData<float>();

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
    shader->MakeDrawingShader(rect, progress);
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
    /* Run your code on data */
    OHOS::Rosen::GetDescriptionFuzzTest();
    OHOS::Rosen::MakeSpatialPointLightShaderFuzzTest();
    OHOS::Rosen::MakeDrawingShaderFuzzTest();
    return 0;
}