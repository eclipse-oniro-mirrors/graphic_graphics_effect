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

#include <cerrno>
#include <fcntl.h>
#include <openssl/sha.h>
#include <string>
#include <unistd.h>

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

namespace {

enum class DiagnosticsOverride {
    NONE,      // governed by runtime property
    FORCE_ON,  // test forces diagnostics on
    FORCE_OFF, // test forces diagnostics off
};

DiagnosticsOverride g_shaderDiagnosticsOverride = DiagnosticsOverride::NONE;
constexpr int DIAGNOSTICS_FILE_MODE = 0644;     // -rw-r--r--
constexpr unsigned int BITS_PER_HEX_NIBBLE = 4; // high nibble shift for hex encoding
constexpr unsigned char HEX_NIBBLE_MASK = 0x0f; // low nibble mask for hex encoding
constexpr int HEX_CHARS_PER_BYTE = 2;           // two hex chars per digest byte
constexpr size_t CSV_QUOTE_COUNT = 2;           // wrapping quotes in CSV escaping
constexpr int OPENSSL_SUCCESS = 1;              // OpenSSL SHA funcs return 1 on success, 0 on failure
// Largest shader source in the repo is ~15KB; 128KB is a generous safety cap that
// rejects pathological inputs without truncating any real shader.
constexpr size_t MAX_DIAGNOSTICS_WRITE_SIZE = 128 * 1024;

static bool IsShaderDiagnosticsEnabled()
{
    switch (g_shaderDiagnosticsOverride) {
        case DiagnosticsOverride::FORCE_ON:
            return true;
        case DiagnosticsOverride::FORCE_OFF:
            return false;
        case DiagnosticsOverride::NONE:
            break;
    }
#ifdef GE_OHOS
    constexpr const char* PROPERTY_SHADER_DIAGNOSTICS_ENABLED = "persist.sys.graphic.geShaderDiagnosticsEnabled";
    static bool enabled =
        (std::atoi(GESystemProperties::GetEventProperty(PROPERTY_SHADER_DIAGNOSTICS_ENABLED).c_str()) != 0);
    return enabled;
#else
    return false;
#endif
}

std::string ComputeSHA256(const std::string& src)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    if (SHA256_Init(&ctx) != OPENSSL_SUCCESS || SHA256_Update(&ctx, src.data(), src.size()) != OPENSSL_SUCCESS ||
        SHA256_Final(digest, &ctx) != OPENSSL_SUCCESS) {
        LOGE("GEShaderDiagnostics: SHA256 computation failed");
        return "";
    }
    static const char hexTable[] = "0123456789abcdef";
    std::string result;
    result.reserve(SHA256_DIGEST_LENGTH * HEX_CHARS_PER_BYTE);
    for (unsigned char b : digest) {
        result.push_back(hexTable[b >> BITS_PER_HEX_NIBBLE]);
        result.push_back(hexTable[b & HEX_NIBBLE_MASK]);
    }
    return result;
}

std::string FormatCsvField(const std::string& field)
{
    if (field.find(',') != std::string::npos || field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos || field.find('\r') != std::string::npos) {
        std::string escaped;
        escaped.reserve(field.size() + CSV_QUOTE_COUNT);
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
 * Logs and returns false on other I/O errors, or if len exceeds
 * MAX_DIAGNOSTICS_WRITE_SIZE (guards against pathological inputs).
 *
 * Durability: fsync() is intentionally omitted — diagnostics files are
 * regenerable (O_EXCL recreates on next shader compile if lost).
 *
 * Safety: all unlink() calls occur while the fd is still open. O_EXCL guarantees
 * we created the file, so unlink() only removes our own file. The close()-failure
 * path does NOT unlink: the fd is already closed (TOCTOU window), and collection
 * tools are expected to handle occasionally-broken diagnostics files.
 */
bool AtomicWriteFile(const char* path, const void* data, size_t len)
{
    if (len > MAX_DIAGNOSTICS_WRITE_SIZE) {
        LOGE("GEShaderDiagnostics: write rejected, size=%{public}zu exceeds cap=%{public}zu, path=%{public}s", len,
            MAX_DIAGNOSTICS_WRITE_SIZE, path);
        return false;
    }
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, DIAGNOSTICS_FILE_MODE);
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
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            int savedErrno = errno;
            unlink(path);
            close(fd);
            LOGE("GEShaderDiagnostics: write(%{public}s) failed: errno=%{public}d", path, savedErrno);
            return false;
        }
        if (n == 0) {
            break;
        }
        written += static_cast<size_t>(n);
    }
    if (written < len) {
        unlink(path);
        close(fd);
        return false;
    }
    if (close(fd) < 0) {
        LOGE("GEShaderDiagnostics: close(%{public}s) failed: errno=%{public}d", path, errno);
        return false;
    }
    return true;
}

bool DumpDiagnostics(const std::string& hash, const GESourceLocation& srcLoc, size_t srcLen)
{
    std::string csv = FormatCsvField(srcLoc.FileName()) + "," + FormatCsvField(srcLoc.FunctionName()) + "," +
                      std::to_string(srcLoc.Line()) + "," + std::to_string(srcLen);
    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    return AtomicWriteFile(csvPath.c_str(), csv.data(), csv.size());
}

bool DumpSkslSource(const std::string& hash, const std::string& shaderSrc)
{
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";
    return AtomicWriteFile(skslPath.c_str(), shaderSrc.data(), shaderSrc.size());
}

} // anonymous namespace

void GESetShaderDiagnosticsEnabledForTest(bool enabled)
{
    g_shaderDiagnosticsOverride = enabled ? DiagnosticsOverride::FORCE_ON : DiagnosticsOverride::FORCE_OFF;
}

void GEClearShaderDiagnosticsOverrideForTest()
{
    g_shaderDiagnosticsOverride = DiagnosticsOverride::NONE;
}

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const GESourceLocation& srcLoc)
{
    if (!IsShaderDiagnosticsEnabled()) {
        return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
    }
    std::string hash = ComputeSHA256(shaderSrc);
    if (hash.empty()) {
        return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
    }
    if (DumpDiagnostics(hash, srcLoc, shaderSrc.length()) && !DumpSkslSource(hash, shaderSrc)) {
        LOGE("GEShaderDiagnostics: sksl write failed, orphan csv may exist: hash=%{public}s", hash.c_str());
    }
    LOGD("GEShaderDiagnostics: %{public}s:%{public}u hash=%{public}s", srcLoc.FileName(), srcLoc.Line(), hash.c_str());
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc);
}

std::shared_ptr<Drawing::RuntimeEffect> GECreateRuntimeEffectForShader(
    const std::string& shaderSrc, const Drawing::RuntimeEffectOptions& options, const GESourceLocation& srcLoc)
{
    if (!IsShaderDiagnosticsEnabled()) {
        return Drawing::RuntimeEffect::CreateForShader(shaderSrc, options);
    }
    std::string hash = ComputeSHA256(shaderSrc);
    if (hash.empty()) {
        return Drawing::RuntimeEffect::CreateForShader(shaderSrc, options);
    }
    if (DumpDiagnostics(hash, srcLoc, shaderSrc.length()) && !DumpSkslSource(hash, shaderSrc)) {
        LOGE("GEShaderDiagnostics: sksl write failed, orphan csv may exist: hash=%{public}s", hash.c_str());
    }
    LOGD("GEShaderDiagnostics: %{public}s:%{public}u hash=%{public}s", srcLoc.FileName(), srcLoc.Line(), hash.c_str());
    return Drawing::RuntimeEffect::CreateForShader(shaderSrc, options);
}
} // namespace Rosen
} // namespace OHOS