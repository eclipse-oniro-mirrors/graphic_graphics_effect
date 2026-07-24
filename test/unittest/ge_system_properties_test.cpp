/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ge_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GESystemPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESystemPropertiesTest::SetUpTestCase(void) {}
void GESystemPropertiesTest::TearDownTestCase(void) {}

void GESystemPropertiesTest::SetUp() {}

void GESystemPropertiesTest::TearDown() {}

/**
 * @tc.name: GetEventProperty_001
 * @tc.desc: Verify function GetEventProperty
 * @tc.type:FUNC
 */
HWTEST_F(GESystemPropertiesTest, GetEventProperty_001, TestSize.Level2)
{
    EXPECT_EQ(GESystemProperties::GetEventProperty(""), "0");
}

/**
 * @tc.name: GetBoolSystemProperty_001
 * @tc.desc: Verify function GetBoolSystemProperty
 * @tc.type:FUNC
 */
HWTEST_F(GESystemPropertiesTest, GetBoolSystemProperty_001, TestSize.Level2)
{
    EXPECT_FALSE(GESystemProperties::GetBoolSystemProperty("", false));
}

/**
 * @tc.name: ConvertToInt
 * @tc.desc: Verify function ConvertToInt
 * @tc.type:FUNC
 */
HWTEST_F(GESystemPropertiesTest, ConvertToInt, TestSize.Level2)
{
    constexpr int defaultNullptr = 7;
    EXPECT_EQ(GESystemProperties::ConvertToInt(nullptr, defaultNullptr), defaultNullptr);
    constexpr int defaultNonNumeric = 9;
    EXPECT_EQ(GESystemProperties::ConvertToInt("abc", defaultNonNumeric), defaultNonNumeric);
    constexpr int expectedValid = 42;
    EXPECT_EQ(GESystemProperties::ConvertToInt("42", 0), expectedValid);
    constexpr int defaultEmpty = 5;
    EXPECT_EQ(GESystemProperties::ConvertToInt("", defaultEmpty), defaultEmpty);
    constexpr int defaultOverflow = 3;
    EXPECT_EQ(GESystemProperties::ConvertToInt("9999999999999999999", defaultOverflow), defaultOverflow);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
