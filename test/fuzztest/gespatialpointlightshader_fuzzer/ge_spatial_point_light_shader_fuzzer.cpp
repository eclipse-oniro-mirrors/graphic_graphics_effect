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

#include "ge_spatial_point_light_shader_fuzzer.h"
#include "fuzzed_data_provider.h"
#include "ge_spatial_point_light.h"

namespace OHOS {
namespace Rosen {

void DoGetDescription(FuzzedDataProvider& fdp)
{
    float lightIntensity = fdp.ConsumeFloatingPoint<float>();
    float lightPosX = fdp.ConsumeFloatingPoint<float>();
    float lightPosY = fdp.ConsumeFloatingPoint<float>();
    float lightPosZ = fdp.ConsumeFloatingPoint<float>();
    float attenuation = fdp.ConsumeFloatingPoint<float>();
    float colorR = fdp.ConsumeFloatingPoint<float>();
    float colorG = fdp.ConsumeFloatingPoint<float>();
    float colorB = fdp.ConsumeFloatingPoint<float>();
    float colorA = fdp.ConsumeFloatingPoint<float>();

    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = lightIntensity;
    params.lightPosition = Vector3f(lightPosX, lightPosY, lightPosZ);
    params.attenuation = attenuation;
    params.lightColor = Vector4f(colorR, colorG, colorB, colorA);

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
    shader->GetDescription();
}

void DoMakeSpatialPointLightShader(FuzzedDataProvider& fdp)
{
    float rectLeft = fdp.ConsumeFloatingPoint<float>();
    float rectTop = fdp.ConsumeFloatingPoint<float>();
    float rectRight = fdp.ConsumeFloatingPoint<float>();
    float rectBottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect rect{rectLeft, rectTop, rectRight, rectBottom};

    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = fdp.ConsumeFloatingPoint<float>();
    params.lightPosition = Vector3f(
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>()
    );
    params.attenuation = fdp.ConsumeFloatingPoint<float>();
    params.lightColor = Vector4f(
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>()
    );

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
    shader->MakeSpatialPointLightShader(rect);
}

void DoMakeDrawingShader(FuzzedDataProvider& fdp)
{
    float rectLeft = fdp.ConsumeFloatingPoint<float>();
    float rectTop = fdp.ConsumeFloatingPoint<float>();
    float rectRight = fdp.ConsumeFloatingPoint<float>();
    float rectBottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect rect{rectLeft, rectTop, rectRight, rectBottom};
    float progress = fdp.ConsumeFloatingPoint<float>();

    Drawing::GESpatialPointLightShaderParams params;
    params.lightIntensity = fdp.ConsumeFloatingPoint<float>();
    params.lightPosition = Vector3f(
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>()
    );
    params.attenuation = fdp.ConsumeFloatingPoint<float>();
    params.lightColor = Vector4f(
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>()
    );

    auto shader = std::make_unique<GESpatialPointLightShader>(params);
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
    
    FuzzedDataProvider fdp(data, size);
    
    const uint8_t DO_GET_DESCRIPTION = 0;
    const uint8_t DO_MAKE_SPATIAL_POINT_LIGHT_SHADER = 1;
    const uint8_t DO_MAKE_DRAWING_SHADER = 2;
    const uint8_t TARGET_SIZE = 3;
    
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    
    switch (choice) {
        case DO_GET_DESCRIPTION:
            OHOS::Rosen::DoGetDescription(fdp);
            break;
        case DO_MAKE_SPATIAL_POINT_LIGHT_SHADER:
            OHOS::Rosen::DoMakeSpatialPointLightShader(fdp);
            break;
        case DO_MAKE_DRAWING_SHADER:
            OHOS::Rosen::DoMakeDrawingShader(fdp);
            break;
        default:
            break;
    }
    
    return 0;
}