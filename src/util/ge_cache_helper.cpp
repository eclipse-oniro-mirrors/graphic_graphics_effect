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

#include "ge_cache_helper.h"
#include "ge_system_properties.h"

namespace OHOS::Rosen {
bool GECacheHelper::IsSDFCacheEnabled(bool defaultValue)
{
#ifdef GE_OHOS
    // if -1, return the defaultValue; else if 0 force false, else if 1 force true
    static CachedHandle g_Handle = CachedParameterCreate("persist.sys.graphic.effect.enablesdfcache", "-1");
    int changed = 0;
    const char* enable = CachedParameterGetChanged(g_Handle, &changed);
    auto readValue = GESystemProperties::ConvertToInt(enable, -1);
    if (readValue == -1) {
        return defaultValue;
    }
    return readValue > 0;
#else
    return defaultValue;
#endif
}
} // OHOS::Rosen