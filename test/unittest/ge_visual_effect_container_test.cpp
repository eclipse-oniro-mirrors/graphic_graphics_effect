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

#include "ge_visual_effect_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectContainerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectContainerTest::SetUpTestCase(void) {}

void GEVisualEffectContainerTest::TearDownTestCase(void) {}

void GEVisualEffectContainerTest::SetUp() {}

void GEVisualEffectContainerTest::TearDown() {}

/**
 * @tc.name: AddToChainedFilter_001
 * @tc.desc: Verify the AddToChainedFilter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(visualEffect);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_001 end";
}

/**
 * @tc.name: AddToChainedFilter_002
 * @tc.desc: Verify the AddToChainedFilter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_002 start";

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_002 end";
}

/**
 * @tc.name: UpdateDarkScale_001
 * @tc.desc: Verify UpdateDarkScale sets darkScale for FrostedGlass
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateDarkScale_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_001 start";

    auto frosted = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    auto other = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(frosted);
    container->AddToChainedFilter(other);

    const float darkScale = 0.7f;
    container->UpdateDarkScale(darkScale);

    auto impl = frosted->GetImpl();
    ASSERT_NE(impl, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
