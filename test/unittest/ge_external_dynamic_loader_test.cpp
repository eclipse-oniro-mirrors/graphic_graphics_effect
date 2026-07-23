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
#include "ge_external_dynamic_loader.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_visual_effect_impl.h"

#include <fstream>
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
const std::string GRAPHICS_EFFECT_EXT_LIB_PATH = "/system/lib64/libgraphics_effect_ext.z.so";
#else
const std::string GRAPHICS_EFFECT_EXT_LIB_PATH = "/system/lib/libgraphics_effect_ext.z.so";
#endif
}
class GEExternalDynamicLoaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    GEExternalDynamicLoader* testLoader_ = nullptr;
};

void GEExternalDynamicLoaderTest::SetUpTestCase(void) {}
void GEExternalDynamicLoaderTest::TearDownTestCase(void) {}

void GEExternalDynamicLoaderTest::SetUp()
{
    testLoader_ = &GEExternalDynamicLoader::GetInstance();
}

void GEExternalDynamicLoaderTest::TearDown()
{
    testLoader_ = nullptr;
}

/**
 * @tc.name: DynamicLoader_Test_001
 * @tc.desc: Verify loader is OK or not
 * @tc.type: FUNC
 */
HWTEST_F(GEExternalDynamicLoaderTest, DynamicLoader_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEExternalDynamicLoaderTest DynamicLoader_Test_001 start";

    ASSERT_NE(testLoader_, nullptr);
    std::ifstream file(GRAPHICS_EFFECT_EXT_LIB_PATH);
    if (!file) {
        EXPECT_EQ(testLoader_->libHandle_, nullptr);
        EXPECT_EQ(testLoader_->createObjectFunc_, nullptr);
        GTEST_LOG_(INFO) << "GEExternalDynamicLoaderTest DynamicLoader_Test_001 end, target lib file not exist";
    } else {
        EXPECT_NE(testLoader_->libHandle_, nullptr);
        EXPECT_NE(testLoader_->createObjectFunc_, nullptr);

        auto object = testLoader_->CreateGEXObjectByType(
            (uint32_t)Drawing::GEVisualEffectImpl::FilterType::NONE, 0, (void*)nullptr);
        EXPECT_EQ(object, nullptr);

        auto mesaBlurParam = std::make_shared<Drawing::GEMESABlurShaderFilterParams>();
        auto mesaBlurObject =
            testLoader_->CreateGEXObjectByType((uint32_t)Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR,
                sizeof(Drawing::GEMESABlurShaderFilterParams), (void*)mesaBlurParam.get());
        EXPECT_NE(mesaBlurObject, nullptr);

        GTEST_LOG_(INFO) << "GEExternalDynamicLoaderTest DynamicLoaderTest001 end";
    }

    if (testLoader_->destroyObjectFunc_ == nullptr) {
        EXPECT_FALSE(testLoader_->DestroyGEXObject(nullptr));
        int dummy = 0;
        EXPECT_FALSE(testLoader_->DestroyGEXObject(static_cast<void*>(&dummy)));
    } else {
        EXPECT_TRUE(testLoader_->DestroyGEXObject(nullptr));
    }
}

/**
 * @tc.name: CreateGEXObjectByType_NullFunc
 * @tc.desc: Verify CreateGEXObjectByType returns nullptr when createObjectFunc_ is null
 * @tc.type: FUNC
 */
HWTEST_F(GEExternalDynamicLoaderTest, CreateGEXObjectByType_NullFunc, TestSize.Level1)
{
    if (testLoader_->createObjectFunc_ == nullptr) {
        EXPECT_EQ(testLoader_->CreateGEXObjectByType(0, 0, nullptr), nullptr);
    }
}

/**
 * @tc.name: DestroyGEXObject_NullFunc
 * @tc.desc: Verify DestroyGEXObject returns false when destroyObjectFunc_ is null
 * @tc.type: FUNC
 */
HWTEST_F(GEExternalDynamicLoaderTest, DestroyGEXObject_NullFunc, TestSize.Level1)
{
    if (testLoader_->destroyObjectFunc_ == nullptr) {
        EXPECT_FALSE(testLoader_->DestroyGEXObject(nullptr));
    }
}

/**
 * @tc.name: GetInstance_Singleton
 * @tc.desc: Verify GetInstance returns the same singleton instance
 * @tc.type: FUNC
 */
HWTEST_F(GEExternalDynamicLoaderTest, GetInstance_Singleton, TestSize.Level1)
{
    auto& instance1 = GEExternalDynamicLoader::GetInstance();
    auto& instance2 = GEExternalDynamicLoader::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

} // namespace Rosen
} // namespace OHOS