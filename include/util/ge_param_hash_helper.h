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

#ifndef GRAPHICS_EFFECT_GE_PARAM_HASH_HELPER_H
#define GRAPHICS_EFFECT_GE_PARAM_HASH_HELPER_H

#include <cstdint>
#include <cstring>
#include <string>

#include "ge_common.h"

namespace OHOS::Rosen {

class GE_EXPORT GEParamHashHelper {
public:
    static uint32_t Hash(const void* data, size_t size, uint32_t seed = 0);

    static uint32_t CombineHash(uint32_t base, uint32_t value);

    template<typename T>
    static uint32_t HashValue(const T& value, uint32_t seed = 0)
    {
        return Hash(&value, sizeof(T), seed);
    }

    template<typename... Args>
    static uint32_t HashFields(uint32_t seed, const Args&... args)
    {
        uint32_t hash = seed;
        ((hash = HashValue(args, hash)), ...);
        return hash;
    }

    static uint32_t HashString(const std::string& str, uint32_t seed = 0)
    {
        return Hash(str.data(), str.size(), seed);
    }
};

} // namespace OHOS::Rosen

#endif // GRAPHICS_EFFECT_GE_PARAM_HASH_HELPER_H
