
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

#include "ge_magnifier_shader_filter_fuzzer.h"
#include "ge_magnifier_shader_filter.h"
#include "get_object.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<Drawing::Image> ProcessImageFuzzTest()
{
    float fLeft = GETest::GetPlainData<float>();
    float fTop = GETest::GetPlainData<float>();
    float fWidth = GETest::GetPlainData<float>();
    float fHeight = GETest::GetPlainData<float>();
    Drawing::Rect src{fLeft, fTop, fWidth, fHeight};
    Drawing::Rect dst = GETest::GetPlainData<Drawing::Rect>();

    Drawing::GEMagnifierShaderFilterParams params = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    std::unique_ptr<GEMagnifierShaderFilter> shaderFilter =
        std::make_unique<GEMagnifierShaderFilter>(params);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    auto res = shaderFilter->ProcessImage(canvas, image, src, dst);
    return res;
}

std::string GetDescriptionFuzzTest()
{
    float factor = GETest::GetPlainData<float>();
    float width = GETest::GetPlainData<float>();
    float height = GETest::GetPlainData<float>();
    float cornerRadius = GETest::GetPlainData<float>();
    float borderWidth = GETest::GetPlainData<float>();
    float shadowOffsetX = GETest::GetPlainData<float>();
    float shadowOffsetY = GETest::GetPlainData<float>();
    float shadowSize = GETest::GetPlainData<float>();
    float shadowStrength = GETest::GetPlainData<float>();
    Drawing::GEMagnifierShaderFilterParams params = {factor, width, height, cornerRadius, borderWidth,
        shadowOffsetX, shadowOffsetY, shadowSize, shadowStrength};
    std::unique_ptr<GEMagnifierShaderFilter> shaderFilter =
        std::make_unique<GEMagnifierShaderFilter>(params);
    std::string res = shaderFilter->GetDescription();
    return res;
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
    OHOS::Rosen::ProcessImageFuzzTest();
    OHOS::Rosen::GetDescriptionFuzzTest();
    return 0;
}
