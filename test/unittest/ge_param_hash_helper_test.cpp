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

#include <gtest/gtest.h>
#include <string>

#include "ge_param_hash_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEParamHashHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEParamHashHelperTest::SetUpTestCase() {}
void GEParamHashHelperTest::TearDownTestCase() {}
void GEParamHashHelperTest::SetUp() {}
void GEParamHashHelperTest::TearDown() {}

/**
 * @tc.name: Hash_DeterministicSameInput
 * @tc.desc: Verify Hash produces the same result for the same input
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, Hash_DeterministicSameInput, TestSize.Level1)
{
    uint32_t data = 0x12345678;
    uint32_t h1 = GEParamHashHelper::Hash(&data, sizeof(data), 0);
    uint32_t h2 = GEParamHashHelper::Hash(&data, sizeof(data), 0);
    EXPECT_EQ(h1, h2);
}

/**
 * @tc.name: Hash_DifferentSeed_DifferentResult
 * @tc.desc: Verify Hash with different seeds produces different results
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, Hash_DifferentSeed_DifferentResult, TestSize.Level1)
{
    uint32_t data = 0x12345678;
    uint32_t h1 = GEParamHashHelper::Hash(&data, sizeof(data), 0);
    uint32_t h2 = GEParamHashHelper::Hash(&data, sizeof(data), 42);
    EXPECT_NE(h1, h2);
}

/**
 * @tc.name: Hash_DifferentData_DifferentResult
 * @tc.desc: Verify Hash with different data produces different results
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, Hash_DifferentData_DifferentResult, TestSize.Level1)
{
    uint32_t data1 = 0x12345678;
    uint32_t data2 = 0x87654321;
    uint32_t h1 = GEParamHashHelper::Hash(&data1, sizeof(data1), 0);
    uint32_t h2 = GEParamHashHelper::Hash(&data2, sizeof(data2), 0);
    EXPECT_NE(h1, h2);
}

/**
 * @tc.name: Hash_EmptyInput
 * @tc.desc: Verify Hash handles zero-size input
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, Hash_EmptyInput, TestSize.Level1)
{
    uint32_t h = GEParamHashHelper::Hash(nullptr, 0, 0);
    EXPECT_NE(h, 0u);
}

/**
 * @tc.name: HashValue_IntegerTypes
 * @tc.desc: Verify HashValue works with different integer types
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, HashValue_IntegerTypes, TestSize.Level1)
{
    int32_t val1 = 42;
    float val2 = 3.14f;
    double val3 = 2.71828;

    uint32_t h1 = GEParamHashHelper::HashValue(val1);
    uint32_t h2 = GEParamHashHelper::HashValue(val2);
    uint32_t h3 = GEParamHashHelper::HashValue(val3);

    EXPECT_NE(h1, 0u);
    EXPECT_NE(h2, 0u);
    EXPECT_NE(h3, 0u);
}

/**
 * @tc.name: CombineHash_Deterministic
 * @tc.desc: Verify CombineHash is deterministic for the same inputs
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, CombineHash_Deterministic, TestSize.Level1)
{
    uint32_t base = 100u;
    uint32_t value = 200u;
    uint32_t h1 = GEParamHashHelper::CombineHash(base, value);
    uint32_t h2 = GEParamHashHelper::CombineHash(base, value);
    EXPECT_EQ(h1, h2);
}

/**
 * @tc.name: CombineHash_DifferentValues_DifferentResults
 * @tc.desc: Verify CombineHash with different values produces different results
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, CombineHash_DifferentValues_DifferentResults, TestSize.Level1)
{
    uint32_t base = 100u;
    uint32_t h1 = GEParamHashHelper::CombineHash(base, 200u);
    uint32_t h2 = GEParamHashHelper::CombineHash(base, 300u);
    EXPECT_NE(h1, h2);
}

/**
 * @tc.name: HashFields_MultipleFields
 * @tc.desc: Verify HashFields combines multiple field hashes
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, HashFields_MultipleFields, TestSize.Level1)
{
    int a = 1;
    float b = 2.0f;
    uint32_t h = GEParamHashHelper::HashFields(0, a, b);
    EXPECT_NE(h, 0u);

    int c = 3;
    uint32_t h2 = GEParamHashHelper::HashFields(0, a, b, c);
    EXPECT_NE(h, h2);
}

/**
 * @tc.name: HashString_Deterministic
 * @tc.desc: Verify HashString is deterministic for the same input
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, HashString_Deterministic, TestSize.Level1)
{
    std::string str = "test_shader";
    uint32_t h1 = GEParamHashHelper::HashString(str);
    uint32_t h2 = GEParamHashHelper::HashString(str);
    EXPECT_EQ(h1, h2);
}

/**
 * @tc.name: HashString_DifferentStrings_DifferentResults
 * @tc.desc: Verify HashString produces different results for different strings
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, HashString_DifferentStrings_DifferentResults, TestSize.Level1)
{
    std::string str1 = "shader_a";
    std::string str2 = "shader_b";
    uint32_t h1 = GEParamHashHelper::HashString(str1);
    uint32_t h2 = GEParamHashHelper::HashString(str2);
    EXPECT_NE(h1, h2);
}

/**
 * @tc.name: HashString_WithSeed
 * @tc.desc: Verify HashString with a seed differs from without
 * @tc.type: FUNC
 */
HWTEST_F(GEParamHashHelperTest, HashString_WithSeed, TestSize.Level1)
{
    std::string str = "test_shader";
    uint32_t h1 = GEParamHashHelper::HashString(str, 0);
    uint32_t h2 = GEParamHashHelper::HashString(str, 99);
    EXPECT_NE(h1, h2);
}

} // namespace Rosen
} // namespace OHOS
