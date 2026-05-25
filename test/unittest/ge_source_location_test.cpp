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

#include <gtest/gtest.h>

#include "ge_source_location.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GESourceLocationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESourceLocationTest::SetUpTestCase(void) {}

void GESourceLocationTest::TearDownTestCase(void) {}

void GESourceLocationTest::SetUp() {}

void GESourceLocationTest::TearDown() {}

/**
 * @tc.name: CurrentTest
 * @tc.desc: Test that Current() captures correct source location
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, CurrentTest, TestSize.Level1)
{
    auto loc = GESourceLocation::Current();
    EXPECT_TRUE(loc.IsValid());
    EXPECT_NE(loc.FileName(), nullptr);
    EXPECT_NE(loc.FunctionName(), nullptr);
    EXPECT_GT(loc.Line(), 0u);
}

/**
 * @tc.name: DefaultConstructorTest
 * @tc.desc: Test default constructor creates invalid location
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, DefaultConstructorTest, TestSize.Level1)
{
    GESourceLocation loc;
    EXPECT_FALSE(loc.IsValid());
    EXPECT_EQ(loc.Line(), 0u);
    EXPECT_EQ(loc.Column(), 0u);
    EXPECT_NE(loc.FileName(), nullptr);
    EXPECT_NE(loc.FunctionName(), nullptr);
}

/**
 * @tc.name: AccessorMethodsTest
 * @tc.desc: Test FileName, FunctionName, Line, Column accessor methods
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, AccessorMethodsTest, TestSize.Level1)
{
    auto loc = GESourceLocation::Current();

    const char* fileName = loc.FileName();
    EXPECT_NE(fileName, nullptr);
    EXPECT_TRUE(fileName[0] != '\0' || !loc.IsValid());

    const char* funcName = loc.FunctionName();
    EXPECT_NE(funcName, nullptr);

    if (loc.IsValid()) {
        EXPECT_GT(loc.Line(), 0u);
    }

    EXPECT_GE(loc.Column(), 0u);
}

/**
 * @tc.name: FunctionCaptureTest
 * @tc.desc: Test that function name is captured correctly in nested call
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, FunctionCaptureTest, TestSize.Level1)
{
    auto CaptureHelper = []() -> GESourceLocation { return GESourceLocation::Current(); };

    auto loc = CaptureHelper();
    EXPECT_TRUE(loc.IsValid());
    EXPECT_NE(loc.FunctionName(), nullptr);
}

/**
 * @tc.name: ConstexprTest
 * @tc.desc: Test that methods work in constexpr context
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, ConstexprTest, TestSize.Level1)
{
    constexpr GESourceLocation defaultLoc;
    constexpr bool isValid = defaultLoc.IsValid();
    constexpr uint32_t line = defaultLoc.Line();
    constexpr const char* file = defaultLoc.FileName();

    EXPECT_FALSE(isValid);
    EXPECT_EQ(line, 0u);
    EXPECT_NE(file, nullptr);
}

/**
 * @tc.name: CopySemanticsTest
 * @tc.desc: Test that GESourceLocation can be copied correctly
 * @tc.type: FUNC
 */
HWTEST_F(GESourceLocationTest, CopySemanticsTest, TestSize.Level1)
{
    auto loc1 = GESourceLocation::Current();
    GESourceLocation loc2 = loc1;

    EXPECT_EQ(loc1.FileName(), loc2.FileName());
    EXPECT_EQ(loc1.FunctionName(), loc2.FunctionName());
    EXPECT_EQ(loc1.Line(), loc2.Line());
    EXPECT_EQ(loc1.Column(), loc2.Column());
    EXPECT_EQ(loc1.IsValid(), loc2.IsValid());

    GESourceLocation loc3;
    loc3 = loc1;

    EXPECT_EQ(loc3.FileName(), loc1.FileName());
    EXPECT_EQ(loc3.Line(), loc1.Line());
}

} // namespace Rosen
} // namespace OHOS