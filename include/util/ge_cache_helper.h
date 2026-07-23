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

#ifndef GRAPHICS_EFFECT_SHADER_CACHE_HELPER_H
#define GRAPHICS_EFFECT_SHADER_CACHE_HELPER_H
#include <any>
#include "ge_common.h"
#include "ge_log.h"
namespace OHOS::Rosen {
class GE_EXPORT GECacheHelper {
public:
    GECacheHelper() = default;
    virtual ~GECacheHelper() = default;

    template<class T>
    static std::shared_ptr<std::any> PackCacheAny(T&& datas)
    {
        return std::make_shared<std::any>(std::forward<T>(datas));
    }

    template<class T>
    static T UnpackCacheAnyOrDefault(std::shared_ptr<std::any> cachePtr, const T& defaultValue)
    {
        if (cachePtr == nullptr) {
            GE_LOGD("GECacheHelper::UnpackCacheAnyOrDefault Input nullptr");
            return defaultValue;
        }
        auto dataPtr = std::any_cast<std::decay_t<T>>(cachePtr.get());
        if (dataPtr == nullptr) {
            GE_LOGE("GECacheHelper::UnpackCacheAnyOrDefault mismatch type");
            return defaultValue;
        }
        return *dataPtr;
    }

    static bool IsSDFCacheEnabled(bool defaultValue);
};
} // OHOS::Rosen
#endif // GRAPHICS_EFFECT_SHADER_CACHE_HELPER_H