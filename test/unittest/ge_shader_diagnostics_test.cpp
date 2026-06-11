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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <openssl/sha.h>
#include <string>
#include <unistd.h>

#include "ge_shader_diagnostics.h"
#include "ge_source_location.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
// Minimal valid SkSL shaders for testing
const std::string SKSL_MINIMAL = "half4 main(float2 xy) { return half4(1.0); }";
const std::string SKSL_RED = "half4 main(float2 xy) { return half4(1.0, 0.0, 0.0, 1.0); }";
const std::string SKSL_PASSTHROUGH =
    "uniform shader imageInput;\n"
    "half4 main(float2 xy) {\n"
    "    return imageInput.eval(xy);\n"
    "}";
} // namespace

class GEShaderDiagnosticsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::string ComputeTestSHA256(const std::string& src);
    void CleanupDiagnosticsFiles(const std::string& hash);
    static std::string ReadFileContent(const std::string& path);
};

void GEShaderDiagnosticsTest::SetUpTestCase(void) {}
void GEShaderDiagnosticsTest::TearDownTestCase(void) {}
void GEShaderDiagnosticsTest::SetUp()
{
    GESetShaderDiagnosticsEnabledForTest(true);
}
void GEShaderDiagnosticsTest::TearDown()
{
    GESetShaderDiagnosticsEnabledForTest(false);
}

std::string GEShaderDiagnosticsTest::ComputeTestSHA256(const std::string& src)
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

void GEShaderDiagnosticsTest::CleanupDiagnosticsFiles(const std::string& hash)
{
    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";
    unlink(csvPath.c_str());
    unlink(skslPath.c_str());
}

std::string GEShaderDiagnosticsTest::ReadFileContent(const std::string& path)
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return "";
    }
    std::string content;
    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        content.append(buf, static_cast<size_t>(n));
    }
    close(fd);
    return content;
}

// ============================================================================
// Both-mode tests
// ============================================================================

/**
 * @tc.name: CreateForShader_EmptySource
 * @tc.desc: Verify GECreateRuntimeEffectForShader returns nullptr for empty shader source
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_EmptySource, TestSize.Level1)
{
    auto result = GECreateRuntimeEffectForShader("");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateForShader_InvalidSource
 * @tc.desc: Verify GECreateRuntimeEffectForShader returns nullptr for invalid shader source
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_InvalidSource, TestSize.Level1)
{
    auto result = GECreateRuntimeEffectForShader("this is not valid SkSL");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateForShader_ValidMinimalSkSL
 * @tc.desc: Verify GECreateRuntimeEffectForShader returns non-null for minimal valid SkSL
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_ValidMinimalSkSL, TestSize.Level1)
{
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateForShader_ExplicitSourceLocation
 * @tc.desc: Verify GECreateRuntimeEffectForShader accepts explicit GESourceLocation parameter
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_ExplicitSourceLocation, TestSize.Level1)
{
    auto srcLoc = GESourceLocation::Current();
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL, srcLoc);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateForShader_DefaultSourceLocation
 * @tc.desc: Verify GECreateRuntimeEffectForShader works with default GESourceLocation::Current()
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_DefaultSourceLocation, TestSize.Level1)
{
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateForShaderWithOptions_EmptySource
 * @tc.desc: Verify RuntimeEffectOptions overload returns nullptr for empty source
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_EmptySource, TestSize.Level1)
{
    Drawing::RuntimeEffectOptions options;
    auto result = GECreateRuntimeEffectForShader("", options);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateForShaderWithOptions_ValidMinimalSkSL
 * @tc.desc: Verify RuntimeEffectOptions overload returns non-null for valid SkSL
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_ValidMinimalSkSL, TestSize.Level1)
{
    Drawing::RuntimeEffectOptions options;
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL, options);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateForShaderWithOptions_ExplicitSourceLocation
 * @tc.desc: Verify RuntimeEffectOptions overload accepts explicit GESourceLocation
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_ExplicitSourceLocation, TestSize.Level1)
{
    Drawing::RuntimeEffectOptions options;
    auto srcLoc = GESourceLocation::Current();
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL, options, srcLoc);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: CreateForShaderWithOptions_DefaultSourceLocation
 * @tc.desc: Verify RuntimeEffectOptions overload works with default GESourceLocation
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_DefaultSourceLocation, TestSize.Level1)
{
    Drawing::RuntimeEffectOptions options;
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL, options);
    EXPECT_NE(result, nullptr);
}

// ============================================================================
// Diagnostics tests
// ============================================================================

/**
 * @tc.name: Diagnostics_RuntimeDisabled_NoFileCreation
 * @tc.desc: Verify that when diagnostics are runtime-disabled, no diagnostic files are created
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_RuntimeDisabled_NoFileCreation, TestSize.Level1)
{
    // Force-disable diagnostics (overrides the test SetUp which force-enabled them)
    GESetShaderDiagnosticsEnabledForTest(false);

    std::string hash = ComputeTestSHA256(SKSL_MINIMAL);
    CleanupDiagnosticsFiles(hash);

    GECreateRuntimeEffectForShader(SKSL_MINIMAL);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";

    int csvFd = open(csvPath.c_str(), O_RDONLY);
    EXPECT_LT(csvFd, 0); // File should NOT exist
    if (csvFd >= 0) {
        close(csvFd);
    }

    int skslFd = open(skslPath.c_str(), O_RDONLY);
    EXPECT_LT(skslFd, 0); // File should NOT exist
    if (skslFd >= 0) {
        close(skslFd);
    }

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_RuntimeDisabled_OptionsOverload_NoFileCreation
 * @tc.desc: Verify RuntimeEffectOptions overload also skips diagnostics when runtime-disabled
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_RuntimeDisabled_OptionsOverload_NoFileCreation, TestSize.Level1)
{
    // Force-disable diagnostics
    GESetShaderDiagnosticsEnabledForTest(false);

    std::string hash = ComputeTestSHA256(SKSL_MINIMAL);
    CleanupDiagnosticsFiles(hash);

    Drawing::RuntimeEffectOptions options;
    GECreateRuntimeEffectForShader(SKSL_MINIMAL, options);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";

    int csvFd = open(csvPath.c_str(), O_RDONLY);
    EXPECT_LT(csvFd, 0);
    if (csvFd >= 0) {
        close(csvFd);
    }

    int skslFd = open(skslPath.c_str(), O_RDONLY);
    EXPECT_LT(skslFd, 0);
    if (skslFd >= 0) {
        close(skslFd);
    }

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_FileCreation
 * @tc.desc: Verify CSV and SKSL files are created after calling GECreateRuntimeEffectForShader
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_FileCreation, TestSize.Level1)
{
    std::string hash = ComputeTestSHA256(SKSL_MINIMAL);
    CleanupDiagnosticsFiles(hash);

    GECreateRuntimeEffectForShader(SKSL_MINIMAL);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";

    int csvFd = open(csvPath.c_str(), O_RDONLY);
    EXPECT_GE(csvFd, 0);
    if (csvFd >= 0) {
        close(csvFd);
    }

    int skslFd = open(skslPath.c_str(), O_RDONLY);
    EXPECT_GE(skslFd, 0);
    if (skslFd >= 0) {
        close(skslFd);
    }

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_CsvContent
 * @tc.desc: Verify CSV file content format: file,function,line,srcLen
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_CsvContent, TestSize.Level1)
{
    std::string hash = ComputeTestSHA256(SKSL_RED);
    CleanupDiagnosticsFiles(hash);

    auto srcLoc = GESourceLocation::Current();
    GECreateRuntimeEffectForShader(SKSL_RED, srcLoc);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string csvContent = ReadFileContent(csvPath);
    EXPECT_FALSE(csvContent.empty());

    // CSV format: file,function,line,srcLen
    // srcLen is the last field — find the last comma and verify the value
    size_t lastComma = csvContent.rfind(',');
    EXPECT_NE(lastComma, std::string::npos);
    std::string srcLenStr = csvContent.substr(lastComma + 1);
    EXPECT_EQ(std::stoul(srcLenStr), SKSL_RED.length());

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_SkslContent
 * @tc.desc: Verify SKSL file content matches input shader source
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_SkslContent, TestSize.Level1)
{
    std::string hash = ComputeTestSHA256(SKSL_PASSTHROUGH);
    CleanupDiagnosticsFiles(hash);

    GECreateRuntimeEffectForShader(SKSL_PASSTHROUGH);

    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";
    std::string skslContent = ReadFileContent(skslPath);
    EXPECT_EQ(skslContent, SKSL_PASSTHROUGH);

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_FirstWriterWins
 * @tc.desc: Verify that calling twice with same source does not overwrite files (O_EXCL)
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_FirstWriterWins, TestSize.Level1)
{
    std::string hash = ComputeTestSHA256(SKSL_MINIMAL);
    CleanupDiagnosticsFiles(hash);

    GECreateRuntimeEffectForShader(SKSL_MINIMAL);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string firstCsvContent = ReadFileContent(csvPath);

    // Second call — files already exist, should not overwrite
    GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    std::string secondCsvContent = ReadFileContent(csvPath);

    EXPECT_EQ(firstCsvContent, secondCsvContent);

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_DifferentHash
 * @tc.desc: Verify that different shader sources produce different hash files
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_DifferentHash, TestSize.Level1)
{
    std::string hash1 = ComputeTestSHA256(SKSL_MINIMAL);
    std::string hash2 = ComputeTestSHA256(SKSL_RED);
    CleanupDiagnosticsFiles(hash1);
    CleanupDiagnosticsFiles(hash2);

    EXPECT_NE(hash1, hash2);

    GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    GECreateRuntimeEffectForShader(SKSL_RED);

    std::string csvPath1 = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash1 + ".csv";
    std::string csvPath2 = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash2 + ".csv";

    int fd1 = open(csvPath1.c_str(), O_RDONLY);
    int fd2 = open(csvPath2.c_str(), O_RDONLY);
    EXPECT_GE(fd1, 0);
    EXPECT_GE(fd2, 0);
    if (fd1 >= 0) {
        close(fd1);
    }
    if (fd2 >= 0) {
        close(fd2);
    }

    CleanupDiagnosticsFiles(hash1);
    CleanupDiagnosticsFiles(hash2);
}

/**
 * @tc.name: Diagnostics_OptionsOverloadFileCreation
 * @tc.desc: Verify RuntimeEffectOptions overload also creates diagnostic files
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_OptionsOverloadFileCreation, TestSize.Level1)
{
    std::string hash = ComputeTestSHA256(SKSL_MINIMAL);
    CleanupDiagnosticsFiles(hash);

    Drawing::RuntimeEffectOptions options;
    GECreateRuntimeEffectForShader(SKSL_MINIMAL, options);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";

    int csvFd = open(csvPath.c_str(), O_RDONLY);
    EXPECT_GE(csvFd, 0);
    if (csvFd >= 0) {
        close(csvFd);
    }

    int skslFd = open(skslPath.c_str(), O_RDONLY);
    EXPECT_GE(skslFd, 0);
    if (skslFd >= 0) {
        close(skslFd);
    }

    CleanupDiagnosticsFiles(hash);
}

} // namespace Rosen
} // namespace OHOS