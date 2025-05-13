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

#include "ge_visual_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectTest::SetUpTestCase(void) {}
void GEVisualEffectTest::TearDownTestCase(void) {}

void GEVisualEffectTest::SetUp() {}
void GEVisualEffectTest::TearDown() {}

/**
 * @tc.name: SetParam_001
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_001 end";
}

/**
 * @tc.name: SetParam_002
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_002 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<double>(1));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_002 end";
}

/**
 * @tc.name: SetParam_003
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_003 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast< const char* const>("1"));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_003 end";
}

/**
 * @tc.name: SetParam_004
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_004 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, static_cast<int64_t>(1));

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam_004 end";
}


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
