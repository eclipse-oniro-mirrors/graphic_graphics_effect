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

#include "ge_frame_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEFrameCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEFrameCacheTest::SetUpTestCase() {}
void GEFrameCacheTest::TearDownTestCase() {}
void GEFrameCacheTest::SetUp() {}
void GEFrameCacheTest::TearDown() {}

/**
 * @tc.name: GEIsCacheHit_InvalidCache
 * @tc.desc: Verify GEIsCacheHit returns false when cache is not valid
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, GEIsCacheHit_InvalidCache, TestSize.Level1)
{
    GEFrameCacheData cache;
    cache.valid = false;
    GEFrameSkipPolicy policy { GEFrameSkipMode::NONE, 0 };
    EXPECT_FALSE(GEIsCacheHit(cache, 123u, 456u, policy));
}

/**
 * @tc.name: GEIsCacheHit_HashMatch
 * @tc.desc: Verify GEIsCacheHit returns true when paramHash and canvasHash match
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, GEIsCacheHit_HashMatch, TestSize.Level1)
{
    GEFrameCacheData cache;
    cache.valid = true;
    cache.paramHash = 100u;
    cache.canvasHash = 200u;
    GEFrameSkipPolicy policy { GEFrameSkipMode::NONE, 0 };
    EXPECT_TRUE(GEIsCacheHit(cache, 100u, 200u, policy));
}

/**
 * @tc.name: GEIsCacheHit_HashMismatch_NoSkip
 * @tc.desc: Verify GEIsCacheHit returns false on hash mismatch with NONE skip mode
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, GEIsCacheHit_HashMismatch_NoSkip, TestSize.Level1)
{
    GEFrameCacheData cache;
    cache.valid = true;
    cache.paramHash = 100u;
    cache.canvasHash = 200u;
    GEFrameSkipPolicy policy { GEFrameSkipMode::NONE, 0 };
    EXPECT_FALSE(GEIsCacheHit(cache, 999u, 200u, policy));
}

/**
 * @tc.name: GEIsCacheHit_FixedInterval_Decrement
 * @tc.desc: Verify GEIsCacheHit decrements remainingSkipFrames and returns true during skip period
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, GEIsCacheHit_FixedInterval_Decrement, TestSize.Level1)
{
    GEFrameCacheData cache;
    cache.valid = true;
    cache.paramHash = 100u;
    cache.canvasHash = 200u;
    cache.remainingSkipFrames = 3;
    GEFrameSkipPolicy policy { GEFrameSkipMode::FIXED_INTERVAL, 3 };

    EXPECT_TRUE(GEIsCacheHit(cache, 999u, 888u, policy));
    EXPECT_EQ(cache.remainingSkipFrames, 2u);
    EXPECT_TRUE(GEIsCacheHit(cache, 999u, 888u, policy));
    EXPECT_EQ(cache.remainingSkipFrames, 1u);
    EXPECT_TRUE(GEIsCacheHit(cache, 999u, 888u, policy));
    EXPECT_EQ(cache.remainingSkipFrames, 0u);
    EXPECT_FALSE(GEIsCacheHit(cache, 999u, 888u, policy));
}

/**
 * @tc.name: GEIsCacheHit_OnParamSame_NoSkip
 * @tc.desc: Verify GEIsCacheHit returns false on hash mismatch with ON_PARAM_SAME mode
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, GEIsCacheHit_OnParamSame_NoSkip, TestSize.Level1)
{
    GEFrameCacheData cache;
    cache.valid = true;
    cache.paramHash = 100u;
    cache.canvasHash = 200u;
    GEFrameSkipPolicy policy { GEFrameSkipMode::ON_PARAM_SAME, 5 };
    EXPECT_FALSE(GEIsCacheHit(cache, 999u, 888u, policy));
}

/**
 * @tc.name: FrameCacheStore_DefaultState
 * @tc.desc: Verify default GEFrameCacheStore state
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_DefaultState, TestSize.Level1)
{
    GEFrameCacheStore store;
    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::NONE);
    EXPECT_EQ(store.GetSkipPolicy().mode, GEFrameSkipMode::NONE);
    EXPECT_EQ(store.GetSkipPolicy().skipInterval, 0u);
    EXPECT_FALSE(store.IsContainerCacheEnabled());
    EXPECT_FALSE(store.IsCacheIncompatible());
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: FrameCacheStore_SetCachePolicy_Container
 * @tc.desc: Verify SetCachePolicy with CONTAINER level enables container cache
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_SetCachePolicy_Container, TestSize.Level1)
{
    GEFrameCacheStore store;
    GEFrameSkipPolicy policy { GEFrameSkipMode::ON_PARAM_SAME, 2 };
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, policy);
    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::CONTAINER);
    EXPECT_TRUE(store.IsContainerCacheEnabled());
    EXPECT_EQ(store.GetSkipPolicy().mode, GEFrameSkipMode::ON_PARAM_SAME);
    EXPECT_EQ(store.GetSkipPolicy().skipInterval, 2u);
}

/**
 * @tc.name: FrameCacheStore_SetCachePolicy_PerEffect
 * @tc.desc: Verify SetCachePolicy with PER_EFFECT level does not enable container cache
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_SetCachePolicy_PerEffect, TestSize.Level1)
{
    GEFrameCacheStore store;
    GEFrameSkipPolicy policy { GEFrameSkipMode::FIXED_INTERVAL, 3 };
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, policy);
    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::PER_EFFECT);
    EXPECT_FALSE(store.IsContainerCacheEnabled());
}

/**
 * @tc.name: FrameCacheStore_SetCachePolicy_LevelChange_Invalidates
 * @tc.desc: Verify that changing cache level or skip mode invalidates container cache and effect caches
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_SetCachePolicy_LevelChange_Invalidates, TestSize.Level1)
{
    GEFrameCacheStore store;
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::ON_PARAM_SAME, 2 });
    auto image = std::make_shared<Drawing::Image>();
    store.UpdateContainerCache(100u, 200u, image, 2);
    EXPECT_TRUE(store.IsContainerCacheValid());

    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::ON_PARAM_SAME, 2 });
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: FrameCacheStore_MarkCacheIncompatible
 * @tc.desc: Verify MarkCacheIncompatible only sets incompatible flag when level is CONTAINER
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_MarkCacheIncompatible, TestSize.Level1)
{
    GEFrameCacheStore store;
    store.MarkCacheIncompatible(true);
    EXPECT_FALSE(store.IsCacheIncompatible());

    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::NONE, 0 });
    store.MarkCacheIncompatible(true);
    EXPECT_TRUE(store.IsCacheIncompatible());

    store.MarkCacheIncompatible(false);
    EXPECT_FALSE(store.IsCacheIncompatible());
}

/**
 * @tc.name: FrameCacheStore_ContainerCache_UpdateAndInvalidate
 * @tc.desc: Verify UpdateContainerCache and InvalidateContainerCache
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_ContainerCache_UpdateAndInvalidate, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto image = std::make_shared<Drawing::Image>();
    store.UpdateContainerCache(42u, 99u, image, 3);
    EXPECT_TRUE(store.IsContainerCacheValid());
    EXPECT_EQ(store.GetContainerCache().paramHash, 42u);
    EXPECT_EQ(store.GetContainerCache().canvasHash, 99u);
    EXPECT_EQ(store.GetContainerCache().remainingSkipFrames, 3u);

    store.InvalidateContainerCache();
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: FrameCacheStore_EffectCache_GetUpdateInvalidate
 * @tc.desc: Verify GetEffectCache, UpdateEffectCache, and InvalidateEffectCaches
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_EffectCache_GetUpdateInvalidate, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto key = GEFrameCacheStore::MakeEffectKey(5, 0);

    auto cache = store.GetEffectCache(key);
    ASSERT_NE(cache, nullptr);
    EXPECT_FALSE(cache->valid);

    auto image = std::make_shared<Drawing::Image>();
    store.UpdateEffectCache(key, 111u, 222u, image, 2);
    cache = store.GetEffectCache(key);
    ASSERT_NE(cache, nullptr);
    EXPECT_TRUE(cache->valid);
    EXPECT_EQ(cache->paramHash, 111u);
    EXPECT_EQ(cache->canvasHash, 222u);
    EXPECT_EQ(cache->remainingSkipFrames, 2u);

    store.InvalidateEffectCaches();
    cache = store.GetEffectCache(key);
    ASSERT_NE(cache, nullptr);
    EXPECT_FALSE(cache->valid);
}

/**
 * @tc.name: FrameCacheStore_UpdateEffectCache_NullImage_NoUpdate
 * @tc.desc: Verify UpdateEffectCache does not update when image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_UpdateEffectCache_NullImage_NoUpdate, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto key = GEFrameCacheStore::MakeEffectKey(5, 0);
    store.UpdateEffectCache(key, 111u, 222u, nullptr, 2);
    auto cache = store.GetEffectCache(key);
    ASSERT_NE(cache, nullptr);
    EXPECT_FALSE(cache->valid);
}

/**
 * @tc.name: FrameCacheStore_EffectSkipPolicy_SetGet
 * @tc.desc: Verify SetEffectSkipPolicy and GetEffectSkipPolicy
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_EffectSkipPolicy_SetGet, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto key = GEFrameCacheStore::MakeEffectKey(7, 1);
    GEFrameSkipPolicy policy { GEFrameSkipMode::FIXED_INTERVAL, 4 };
    store.SetEffectSkipPolicy(key, policy);

    auto& retrieved = store.GetEffectSkipPolicy(key);
    EXPECT_EQ(retrieved.mode, GEFrameSkipMode::FIXED_INTERVAL);
    EXPECT_EQ(retrieved.skipInterval, 4u);
}

/**
 * @tc.name: FrameCacheStore_EffectSkipPolicy_DefaultForMissingKey
 * @tc.desc: Verify GetEffectSkipPolicy returns default policy for missing key
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_EffectSkipPolicy_DefaultForMissingKey, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto& retrieved = store.GetEffectSkipPolicy("nonexistent_key");
    EXPECT_EQ(retrieved.mode, GEFrameSkipMode::NONE);
    EXPECT_EQ(retrieved.skipInterval, 0u);
}

/**
 * @tc.name: FrameCacheStore_TrimEffectCaches_RemovesStale
 * @tc.desc: Verify TrimEffectCaches removes entries not in the valid set
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_TrimEffectCaches_RemovesStale, TestSize.Level1)
{
    GEFrameCacheStore store;
    auto key1 = GEFrameCacheStore::MakeEffectKey(1, 0);
    auto key2 = GEFrameCacheStore::MakeEffectKey(2, 1);
    auto key3 = GEFrameCacheStore::MakeEffectKey(3, 2);

    store.SetEffectSkipPolicy(key1, { GEFrameSkipMode::ON_PARAM_SAME, 1 });
    store.SetEffectSkipPolicy(key2, { GEFrameSkipMode::FIXED_INTERVAL, 2 });
    store.SetEffectSkipPolicy(key3, { GEFrameSkipMode::NONE, 0 });

    std::unordered_set<std::string> validKeys = { key1, key2 };
    store.TrimEffectCaches(validKeys);

    auto& p1 = store.GetEffectSkipPolicy(key1);
    EXPECT_EQ(p1.mode, GEFrameSkipMode::ON_PARAM_SAME);
    auto& p2 = store.GetEffectSkipPolicy(key2);
    EXPECT_EQ(p2.mode, GEFrameSkipMode::FIXED_INTERVAL);
    auto& p3 = store.GetEffectSkipPolicy(key3);
    EXPECT_EQ(p3.mode, GEFrameSkipMode::NONE);
    EXPECT_EQ(p3.skipInterval, 0u);
}

/**
 * @tc.name: FrameCacheStore_ApplyCachePolicy_PerEffect_SetsEffectPolicies
 * @tc.desc: Verify ApplyCachePolicy with PER_EFFECT sets per-effect skip policies
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_ApplyCachePolicy_PerEffect_SetsEffectPolicies, TestSize.Level1)
{
    GEFrameCacheStore store;
    GEFrameSkipPolicy policy { GEFrameSkipMode::ON_PARAM_SAME, 2 };
    std::vector<int32_t> filterTypes = { 10, 20, 30 };
    store.ApplyCachePolicy(GEFrameCacheLevel::PER_EFFECT, policy, filterTypes);

    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::PER_EFFECT);
    for (size_t i = 0; i < filterTypes.size(); i++) {
        auto key = GEFrameCacheStore::MakeEffectKey(filterTypes[i], i);
        auto& sp = store.GetEffectSkipPolicy(key);
        EXPECT_EQ(sp.mode, GEFrameSkipMode::ON_PARAM_SAME);
        EXPECT_EQ(sp.skipInterval, 2u);
    }
}

/**
 * @tc.name: FrameCacheStore_MakeEffectKey_Format
 * @tc.desc: Verify MakeEffectKey produces expected key format
 * @tc.type: FUNC
 */
HWTEST_F(GEFrameCacheTest, FrameCacheStore_MakeEffectKey_Format, TestSize.Level1)
{
    EXPECT_EQ(GEFrameCacheStore::MakeEffectKey(5, 0), "5_0");
    EXPECT_EQ(GEFrameCacheStore::MakeEffectKey(100, 3), "100_3");
}

} // namespace Rosen
} // namespace OHOS
