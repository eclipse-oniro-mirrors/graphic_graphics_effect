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

#include <string>

#include "ge_log.h"

#ifdef GE_DIAGNOSTICS_DUMP_SHADER_CREATOR
#include <cerrno>
#include <fcntl.h>
#include <openssl/sha.h>
#include <unistd.h>

#include "securec.h"
#endif

namespace OHOS {
namespace Rosen {

#ifndef GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, [[maybe_unused]] const GESourceLocation& srcLoc)
{
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
}

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(const std::string& shaderSrc,
    const Drawing::RuntimeEffectOptions& options, [[maybe_unused]] const GESourceLocation& srcLoc)
{
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc, options);
}

#else // GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

namespace {

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
    if (field.find(',') != std::string::npos || field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos || field.find('\r') != std::string::npos) {
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

/**
 * Atomically create a file and write content using O_CREAT|O_EXCL.
 * Returns true if the file was newly created and written.
 * Returns false if the file already existed (EEXIST) — this is the expected
 * "skip" case for duplicate shaders across processes.
 * Logs and returns false on other I/O errors.
 *
 * Note: fsync() is intentionally omitted — diagnostics data is non-critical
 * and does not need crash durability guarantees.
 */
bool AtomicWriteFile(const char* path, const void* data, size_t len)
{
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd < 0) {
        if (errno == EEXIST) {
            return false;
        }
        LOGE("GEShaderDiagnostics: open(%{public}s) failed: errno=%{public}d", path, errno);
        return false;
    }
    const char* buf = static_cast<const char*>(data);
    size_t written = 0;
    while (written < len) {
        ssize_t n = write(fd, buf + written, len - written);
        if (n <= 0) {
            int savedErrno = errno;
            close(fd);
            unlink(path);
            LOGE("GEShaderDiagnostics: write(%{public}s) failed: errno=%{public}d", path, savedErrno);
            return false;
        }
        written += static_cast<size_t>(n);
    }
    close(fd);
    return true;
}

bool DumpDiagnostics(const std::string& hash, const GESourceLocation& srcLoc, size_t srcLen)
{
    char csvBuf[512];
    int len = snprintf_s(csvBuf, sizeof(csvBuf), sizeof(csvBuf) - 1, "%s,%s,%u,%zu",
        FormatCsvField(srcLoc.FileName()).c_str(), FormatCsvField(srcLoc.FunctionName()).c_str(), srcLoc.Line(),
        srcLen);
    if (len <= 0) {
        return false;
    }
    char csvPath[256];
    int pathLen = snprintf_s(
        csvPath, sizeof(csvPath), sizeof(csvPath) - 1, "%sge_shader_diagnostics.%s.csv", GE_SHADER_DIAGNOSTICS_OUT_DIR, hash.c_str());
    if (pathLen <= 0) {
        return false;
    }
    return AtomicWriteFile(csvPath, csvBuf, static_cast<size_t>(len));
}

void DumpSkslSource(const std::string& hash, const std::string& shaderSrc)
{
    char skslPath[256];
    int pathLen = snprintf_s(
        skslPath, sizeof(skslPath), sizeof(skslPath) - 1, "%sge_shader_diagnostics.%s.sksl", GE_SHADER_DIAGNOSTICS_OUT_DIR, hash.c_str());
    if (pathLen <= 0) {
        return;
    }
    AtomicWriteFile(skslPath, shaderSrc.data(), shaderSrc.size());
}

} // anonymous namespace

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc)
{
    std::string hash = ComputeSHA256(shaderSrc);
    if (DumpDiagnostics(hash, srcLoc, shaderSrc.length())) {
        DumpSkslSource(hash, shaderSrc);
    }
    LOGD("GEShaderDiagnostics: %{public}s:%{public}u hash=%{public}s", srcLoc.FileName(), srcLoc.Line(), hash.c_str());
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
}

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const Drawing::RuntimeEffectOptions& options, const GESourceLocation& srcLoc)
{
    std::string hash = ComputeSHA256(shaderSrc);
    if (DumpDiagnostics(hash, srcLoc, shaderSrc.length())) {
        DumpSkslSource(hash, shaderSrc);
    }
    LOGD("GEShaderDiagnostics: %{public}s:%{public}u hash=%{public}s", srcLoc.FileName(), srcLoc.Line(), hash.c_str());
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc, options);
}

#endif // GE_DIAGNOSTICS_DUMP_SHADER_CREATOR

} // namespace Rosen
} // namespace OHOS