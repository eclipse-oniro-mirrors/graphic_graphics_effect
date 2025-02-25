/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "get_object.h"
#include "ge_external_dynamic_loader.h"
#include "ge_external_dynamic_loader_fuzz.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

bool GEExternalDynamicLoaderFuzzTest001(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    auto& dynamicLoader = GEExternalDynamicLoader::GetInstance();
    auto type = (uint32_t)Drawing::GEVisualEffectImpl::FilterType::NONE;
    uint32_t len = GETest::GetPlainData<uint32_t>();
    uint8_t* param = (uint8_t*)(data + GETest::g_pos);
    auto object = dynamicLoader.CreateGEXObjectByType(type, len, param);
    if (object) {
        delete (GEShaderFilter*)object;
    }
    return true;
}

}  // namespace GraphicsEffectEngine
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GraphicsEffectEngine::GEExternalDynamicLoaderFuzzTest001(data, size);
    return 0;
}
