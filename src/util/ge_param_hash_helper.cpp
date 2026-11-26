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

#include "ge_param_hash_helper.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS::Rosen {

uint32_t GEParamHashHelper::Hash(const void* data, size_t size, uint32_t seed)
{
#ifdef USE_M133_SKIA
    return SkChecksum::Hash32(data, size, seed);
#else
    return SkOpts::hash(data, size, seed);
#endif
}

uint32_t GEParamHashHelper::CombineHash(uint32_t base, uint32_t value)
{
    return Hash(&value, sizeof(value), base);
}

} // namespace OHOS::Rosen
