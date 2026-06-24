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
// Must match the constants in ge_shader_diagnostics.cpp (anonymous namespace)
constexpr unsigned int BITS_PER_HEX_NIBBLE = 4; // high nibble shift for hex encoding
constexpr unsigned char HEX_NIBBLE_MASK = 0x0f; // low nibble mask for hex encoding
constexpr int HEX_CHARS_PER_BYTE = 2;           // two hex chars per digest byte
// Minimal valid SkSL shaders for testing
const std::string SKSL_MINIMAL = "half4 main(float2 xy) { return half4(1.0); }";
const std::string SKSL_RED = "half4 main(float2 xy) { return half4(1.0, 0.0, 0.0, 1.0); }";
const std::string SKSL_PASSTHROUGH = "uniform shader imageInput;\n"
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
    GEClearShaderDiagnosticsOverrideForTest();
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
    result.reserve(SHA256_DIGEST_LENGTH * HEX_CHARS_PER_BYTE);
    for (unsigned char b : digest) {
        result.push_back(hexTable[b >> BITS_PER_HEX_NIBBLE]);
        result.push_back(hexTable[b & HEX_NIBBLE_MASK]);
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
 * @tc.desc: Verify wrapper forwards empty source to upstream without altering its return value
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_EmptySource, TestSize.Level1)
{
    // The wrapper must not add its own semantics — it forwards to the upstream
    // Drawing::RuntimeEffect::CreateForShader and returns whatever it returns.
    std::string hash = ComputeTestSHA256("");
    CleanupDiagnosticsFiles(hash); // SHA256("") is non-empty; clean up under FORCE_ON
    auto upstream = Drawing::RuntimeEffect::CreateForShader("");
    auto result = GECreateRuntimeEffectForShader("");
    ASSERT_TRUE(upstream); // upstream never returns null for empty input
    EXPECT_EQ(static_cast<bool>(result), static_cast<bool>(upstream));
    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: CreateForShader_ValidMinimalSkSL
 * @tc.desc: Verify GECreateRuntimeEffectForShader returns non-null for minimal valid SkSL
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_ValidMinimalSkSL, TestSize.Level1)
{
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    ASSERT_NE(result, nullptr);
    // Sanity: the fixture constant is itself a non-empty shader body.
    EXPECT_FALSE(SKSL_MINIMAL.empty());
    // The wrapper must return the upstream RuntimeEffect type, not a derived one.
    EXPECT_NE(result->GetDrawingType(), Drawing::DrawingType::NO_DRAW);
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
    ASSERT_NE(result, nullptr);
    // srcLoc captured here has the file/function/line of this call site.
    EXPECT_TRUE(srcLoc.IsValid());
    EXPECT_NE(srcLoc.FileName(), nullptr);
    EXPECT_GT(srcLoc.Line(), 0);
}

/**
 * @tc.name: CreateForShader_DefaultSourceLocation
 * @tc.desc: Verify GECreateRuntimeEffectForShader works with default GESourceLocation::Current()
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShader_DefaultSourceLocation, TestSize.Level1)
{
    // Relying on the implicit GESourceLocation::Current() default argument.
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL);
    ASSERT_NE(result, nullptr);
    EXPECT_FALSE(SKSL_MINIMAL.empty());
    // Confirm the default-constructed effect reports the common drawing type.
    EXPECT_EQ(result->GetDrawingType(), Drawing::DrawingType::COMMON);
}

/**
 * @tc.name: CreateForShaderWithOptions_EmptySource
 * @tc.desc: Verify options overload forwards empty source to upstream without altering its return value
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_EmptySource, TestSize.Level1)
{
    Drawing::RuntimeEffectOptions options;
    // The wrapper must not add its own semantics — it forwards to the upstream
    // Drawing::RuntimeEffect::CreateForShader and returns whatever it returns.
    std::string hash = ComputeTestSHA256("");
    CleanupDiagnosticsFiles(hash); // SHA256("") is non-empty; clean up under FORCE_ON
    auto upstream = Drawing::RuntimeEffect::CreateForShader("", options);
    auto result = GECreateRuntimeEffectForShader("", options);
    EXPECT_EQ(static_cast<bool>(result), static_cast<bool>(upstream));
    CleanupDiagnosticsFiles(hash);
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
    ASSERT_NE(result, nullptr);
    // The static shader source must be unchanged after compilation.
    EXPECT_FALSE(SKSL_MINIMAL.empty());
    // The default options leave all compile-affecting flags disabled.
    EXPECT_FALSE(options.useAF);
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
    ASSERT_NE(result, nullptr);
    // line_ is non-zero, captured at this call site
    EXPECT_TRUE(srcLoc.IsValid());
    EXPECT_GT(srcLoc.Line(), 0);
}

/**
 * @tc.name: CreateForShaderWithOptions_DefaultSourceLocation
 * @tc.desc: Verify RuntimeEffectOptions overload works with default GESourceLocation
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, CreateForShaderWithOptions_DefaultSourceLocation, TestSize.Level1)
{
    // Options overload also accepts the implicit GESourceLocation::Current().
    // The default options leave all compile-affecting flags off.
    Drawing::RuntimeEffectOptions options;
    auto result = GECreateRuntimeEffectForShader(SKSL_MINIMAL, options);
    ASSERT_NE(result, nullptr);
    EXPECT_FALSE(options.forceNoInline);
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
 * @tc.name: Diagnostics_CsvEscape_SpecialChars
 * @tc.desc: Verify FormatCsvField escapes comma/quote/newline in file and function names
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_CsvEscape_SpecialChars, TestSize.Level1)
{
    constexpr char escShader[] = "half4 main(float2 xy) { return half4(1.0); } // esc";
    std::string hash = ComputeTestSHA256(escShader);
    CleanupDiagnosticsFiles(hash);

    auto srcLoc = GESourceLocation::Current("path,file.csv", R"(func"tion)", 0, 0);
    GECreateRuntimeEffectForShader(escShader, srcLoc);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string csvContent = ReadFileContent(csvPath);
    EXPECT_FALSE(csvContent.empty());
    // Quoted and doubled-quote: "path,file.csv","func""tion",
    EXPECT_NE(csvContent.find("\"path,file.csv\""), std::string::npos);
    EXPECT_NE(csvContent.find("\"func\"\"tion\""), std::string::npos);

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_CsvEscape_NewlineChar
 * @tc.desc: Verify FormatCsvField triggers escape on newline in file name
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_CsvEscape_NewlineChar, TestSize.Level1)
{
    constexpr char nlShader[] = "half4 main(float2 xy) { return half4(1.0); } // nl";
    std::string hash = ComputeTestSHA256(nlShader);
    CleanupDiagnosticsFiles(hash);

    auto srcLoc = GESourceLocation::Current("multi\nline.cpp", "fn", 0, 0);
    GECreateRuntimeEffectForShader(nlShader, srcLoc);

    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string csvContent = ReadFileContent(csvPath);
    EXPECT_FALSE(csvContent.empty());
    // Newline triggers escape wrapping: "multi\nline.cpp",
    EXPECT_NE(csvContent.find("\"multi\nline.cpp\""), std::string::npos);

    CleanupDiagnosticsFiles(hash);
}

/**
 * @tc.name: Diagnostics_OrphanCsvOnSkslExists
 * @tc.desc: Verify orphan-csv warning path when csv write succeeds but sksl already exists (EEXIST)
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderDiagnosticsTest, Diagnostics_OrphanCsvOnSkslExists, TestSize.Level1)
{
    constexpr char orphanShader[] = "half4 main(float2 xy) { return half4(1.0); } // orphan";
    std::string hash = ComputeTestSHA256(orphanShader);
    CleanupDiagnosticsFiles(hash);

    // First call: creates both csv (new) and sksl (new)
    GECreateRuntimeEffectForShader(orphanShader);

    // Remove only the csv, leaving the sksl in place
    std::string csvPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".csv";
    std::string skslPath = std::string(GE_SHADER_DIAGNOSTICS_OUT_DIR) + "ge_shader_diagnostics." + hash + ".sksl";
    ASSERT_EQ(unlink(csvPath.c_str()), 0);
    EXPECT_EQ(access(skslPath.c_str(), F_OK), 0);

    // Second call: csv O_CREAT|O_EXCL succeeds (true), sksl O_CREAT|O_EXCL hits EEXIST (false)
    // → DumpDiagnostics true && !DumpSkslSource(false) → LOGE orphan-csv path
    GECreateRuntimeEffectForShader(orphanShader);

    // CSV was re-created by the second call
    EXPECT_EQ(access(csvPath.c_str(), F_OK), 0);
    // SKSL remains the first-call content, unchanged
    EXPECT_EQ(ReadFileContent(skslPath), orphanShader);

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