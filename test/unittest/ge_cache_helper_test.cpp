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
#include "ge_cache_helper.h"
#ifdef GE_OHOS
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "utils/system_properties.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GECacheHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    const std::string enableSDFTag_ = "persist.sys.graphic.effect.enablesdfcache";
    const std::string keepDefaultValue_ = "-1";
    const std::string forceFalseValue_ = "0";
    const std::string forceTrueValue_ = "1";
};

void GECacheHelperTest::SetUpTestCase(void) {}
void GECacheHelperTest::TearDownTestCase(void) {}
void GECacheHelperTest::SetUp()
{
    system::SetParameter(enableSDFTag_.c_str(), keepDefaultValue_.c_str());
}

void GECacheHelperTest::TearDown()
{
    system::SetParameter(enableSDFTag_.c_str(), keepDefaultValue_.c_str());
}

/**
 * @tc.name: IsSDFCacheEnabledDefault
 * @tc.desc: Verify IsSDFCacheEnabled returns defaultValue true when system property is not set
 * @tc.type: FUNC
 */
HWTEST_F(GECacheHelperTest, IsSDFCacheEnabledDefault, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledDefault start";
    system::SetParameter(enableSDFTag_.c_str(), keepDefaultValue_.c_str());
    bool result = GECacheHelper::IsSDFCacheEnabled(true);
    EXPECT_TRUE(result);
    result = GECacheHelper::IsSDFCacheEnabled(false);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledDefault end";
}

/**
 * @tc.name: IsSDFCacheEnabledForceFalse
 * @tc.desc: Verify IsSDFCacheEnabled returns defaultValue false when system property is not set
 * @tc.type: FUNC
 */
HWTEST_F(GECacheHelperTest, IsSDFCacheEnabledForceFalse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledForceFalse start";
    system::SetParameter(enableSDFTag_.c_str(), forceFalseValue_.c_str());
    bool result = GECacheHelper::IsSDFCacheEnabled(false);
    EXPECT_FALSE(result);
    result = GECacheHelper::IsSDFCacheEnabled(true);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledForceFalse end";
}

/**
 * @tc.name: IsSDFCacheEnabledForceTrue
 * @tc.desc: Verify IsSDFCacheEnabled returns defaultValue false when system property is not set
 * @tc.type: FUNC
 */
HWTEST_F(GECacheHelperTest, IsSDFCacheEnabledForceTrue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledForceTrue start";
    system::SetParameter(enableSDFTag_.c_str(), forceTrueValue_.c_str());
    bool result = GECacheHelper::IsSDFCacheEnabled(false);
    EXPECT_TRUE(result);
    result = GECacheHelper::IsSDFCacheEnabled(true);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GECacheHelperTest IsSDFCacheEnabledForceTrue end";
}
} // namespace Rosen
} // namespace OHOS
