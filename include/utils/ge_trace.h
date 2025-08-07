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

#ifndef GE_TRACE_H
#define GE_TRACE_H

#ifdef NOT_BUILD_FOR_OHOS_SDK
#include "ge_system_properties.h"
#include "hitrace_meter.h"

namespace OHOS::Rosen {

#define LIKELY(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY(exp) (__builtin_expect((exp) != 0, false))
#define GE_TRACE(name)                                                                                                  \
    static bool debugTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);       \
    auto geNameTrace = (UNLIEKLY(debugTraceEnable)) ?                                                                   \
                        std::make_unique<GEOptionalTrace>(name) :                                                       \
                        nullptr

#define GE_TRACE_FUNC()                                                                                                 \
    static bool debugTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);       \
    auto geNameTrace = (UNLIEKLY(debugTraceEnable)) ?                                                                   \
                        std::make_unique<GEOptionalTrace>(__func__) :                                                   \
                        nullptr

#define GE_TRACE_NAME_FMT(fmt, ...)                                                                                     \
    do {                                                                                                                \
        static bool debugTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);   \
        if (UNLIEKLY(debugTraceEnable)) {                                                                               \
            std::string name { "GE#" };                                                                                 \
            name.append(fmt);                                                                                           \
            HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, name.c_str(), ##__VA_ARGS__);                                    \
        }                                                                                                               \
    } while (0)

class GEOptionalTrace {
public:
    GEOptionalTrace(std::string traceStr)
    {
        std::string name { "GE#" };
        name.append(traceStr);
        StartTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL, name);
    }

    ~GEOptionalTrace()
    {
        FinishTrace(HITRACE_TAG_GRAPHIC_AGP | HITRACE_TAG_COMMERCIAL);
    }
};
} // namespace OHOS::Rosen

#else
#define GE_TRACE(name)
#define GE_TRACE_FUNC()
#define GE_TRACE_NAME_FMT(fmt, ...)
#endif // NOT_BUILD_FOR_OHOS_SDK

#endif // GE_TRACE_H
