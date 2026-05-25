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

#include "ge_shader_diagnostics.h"

#include <atomic>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <string>

#include "ge_log.h"

#ifdef GE_DIAGNOSTICS_DUMP_SHADER_CREATOR
#include <openssl/sha.h>
#include "securec.h"
#endif

namespace OHOS {
namespace Rosen {

#ifndef GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc)
{
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
}

#else // GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

namespace {

constexpr const char* CSV_PATH = "/data/local/tmp/ge_shader_diagnostics.csv";
constexpr const char* SKSL_DIR = "/data/local/tmp/";

std::mutex g_fileMutex;
std::atomic<bool> g_headerWritten{false};

std::string ComputeSHA256(const std::string& src)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, src.data(), src.size());
    SHA256_Final(digest, &ctx);
    static const char hexTable[] = "0123456789abcdef";
    std::string result;
    result.reserve(SHA256_DIGEST_LENGTH * 2);
    for (unsigned char b : digest) {
        result.push_back(hexTable[b >> 4]);
        result.push_back(hexTable[b & 0x0f]);
    }
    return result;
}

std::string FormatCsvField(const std::string& field)
{
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {
        std::string escaped;
        escaped.reserve(field.size() + 2);
        escaped.push_back('"');
        for (char c : field) {
            if (c == '"') {
                escaped.push_back('"');
            }
            escaped.push_back(c);
        }
        escaped.push_back('"');
        return escaped;
    }
    return field;
}

void WriteCsvRecord(const std::string& record)
{
    std::lock_guard<std::mutex> lock(g_fileMutex);
    std::ofstream file(CSV_PATH, std::ios::app);
    if (!file.is_open()) {
        LOGE("GEShaderDiagnostics: Failed to open %{public}s", CSV_PATH);
        return;
    }
    if (!g_headerWritten.exchange(true)) {
        file << "hash,file,function,line,srcLen\n";
    }
    file << record << "\n";
    file.flush();
}

}

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc)
{
    std::string hash = ComputeSHA256(shaderSrc);
    char lineBuf[512];
    int len = snprintf_s(lineBuf, sizeof(lineBuf), sizeof(lineBuf) - 1,
        "%s,%s,%s,%u,%zu",
        hash.c_str(),
        FormatCsvField(srcLoc.FileName()).c_str(),
        FormatCsvField(srcLoc.FunctionName()).c_str(),
        srcLoc.Line(),
        shaderSrc.length());
    if (len > 0) {
        WriteCsvRecord(std::string(lineBuf, len));
    }
    char skslPath[256];
    len = snprintf_s(skslPath, sizeof(skslPath), sizeof(skslPath) - 1,
        "%sge_shader_diagnostics.%s.sksl", SKSL_DIR, hash.c_str());
    if (len > 0) {
        std::ofstream skslFile(skslPath, std::ios::out);
        if (skslFile.is_open()) {
            skslFile << shaderSrc;
            skslFile.flush();
        }
    }
    LOGD("GEShaderDiagnostics: %{public}s:%{public}u hash=%{public}s",
         srcLoc.FileName(), srcLoc.Line(), hash.c_str());
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
}

#endif // GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

} // namespace Rosen
} // namespace OHOS