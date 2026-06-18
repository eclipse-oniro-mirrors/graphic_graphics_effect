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
#include "securec.h"

namespace OHOS::Rosen {

#define LIKELY(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY(exp) (__builtin_expect((exp) != 0, false))
#define GE_TRACE(name)                                                                                                \
    static bool debugTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);     \
    auto geNameTrace = (UNLIKELY(debugTraceEnable)) ?                                                                 \
                        std::make_unique<GEOptionalTrace>(name) :                                                     \
                        nullptr

#define GE_TRACE_FUNC()                                                                                               \
    static bool debugTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);     \
    auto geNameTrace = (UNLIKELY(debugTraceEnable)) ?                                                                 \
                        std::make_unique<GEOptionalTrace>(__func__) :                                                 \
                        nullptr

#define GE_TRACE_NAME_FMT(fmt, ...)                                                                                   \
    static bool debugFmtTraceEnable = (OHOS::system::GetIntParameter("persist.sys.graphic.openDebugTrace", 0) != 0);  \
    auto optionalFmtTrace = (UNLIKELY(debugFmtTraceEnable)) ?                                                         \
        std::make_unique<RSOptionalFmtTrace>(fmt, ##__VA_ARGS__) :  nullptr

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

class RSOptionalFmtTrace {
public:
    RSOptionalFmtTrace(const char* fmt, ...)
    {
        const int maxSize = 256;
        va_list vaList;
        char buf[maxSize];
        va_start(vaList, fmt);
        int result = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, fmt, vaList);
        va_end(vaList);

        if (result < 0) {
            std::string traceStr(buf, maxSize - 1);
            const char* errorSuffix = "#length > 256, error";
            traceStr += errorSuffix;
            StartTrace(HITRACE_TAG_GRAPHIC_AGP, traceStr.c_str());
            return;
        }

        StartTrace(HITRACE_TAG_GRAPHIC_AGP, buf);
    }
    ~RSOptionalFmtTrace()
    {
        FinishTrace(HITRACE_TAG_GRAPHIC_AGP); // 256 Maximum length of a character string to be printed
    }
};
} // namespace OHOS::Rosen

#else
#define GE_TRACE(name)
#define GE_TRACE_FUNC()
#define GE_TRACE_NAME_FMT(fmt, ...)
#endif // NOT_BUILD_FOR_OHOS_SDK

#endif // GE_TRACE_H
