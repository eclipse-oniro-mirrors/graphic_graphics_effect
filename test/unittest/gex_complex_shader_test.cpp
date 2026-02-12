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

#include "gex_complex_shader.h"
#include "ge_visual_effect_impl.h"
#include "ge_external_dynamic_loader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEXComplexShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEXComplexShaderTest::SetUpTestCase() {}

void GEXComplexShaderTest::TearDownTestCase() {}

void GEXComplexShaderTest::SetUp() {}

void GEXComplexShaderTest::TearDown() {}

/**
 * @tc.name: CreateDynamicImpl001
 * @tc.desc: Verify function CreateDynamicImpl
 * @tc.type:FUNC
 */
HWTEST_F(GEXComplexShaderTest, CreateDynamicImpl001, TestSize.Level1)
{
    GEXComplexShaderParams temp;
    temp.params_ = {0.5f, 0.5f, 0.5f};
    auto shader = GEXComplexShader::CreateDynamicImpl(temp);

    EXPECT_EQ(shader, nullptr);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
