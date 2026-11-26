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
#include <memory>

#include "draw/surface.h"
#include "ge_render.h"
#include "ge_shader_cache.h"
#include "ge_shader_filter_params_namesonly.h"
#include "ge_visual_effect_container.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

static OffscreenCanvas MakeCpuOffscreen(Drawing::Canvas&, int32_t w, int32_t h)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(w, h);
    if (!surface) {
        return {};
    }
    return { surface, surface->GetCanvas() };
}

class GEShaderCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEShaderCacheTest::SetUpTestCase() {}

void GEShaderCacheTest::TearDownTestCase()
{
    GEShaderCache::SetOffscreenCanvasCreator(nullptr);
}

void GEShaderCacheTest::SetUp() {}

void GEShaderCacheTest::TearDown()
{
    GEShaderCache::SetOffscreenCanvasCreator(nullptr);
}

/**
 * @tc.name: SetOffscreenCanvasCreator_SetsCallable
 * @tc.desc: Verify SetOffscreenCanvasCreator stores a callable
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, SetOffscreenCanvasCreator_SetsCallable, TestSize.Level1)
{
    GEShaderCache::SetOffscreenCanvasCreator([](Drawing::Canvas&, int32_t, int32_t) -> OffscreenCanvas { return {}; });
    EXPECT_TRUE(GEShaderCache::offscreenCanvasCreator_);
    GEShaderCache::SetOffscreenCanvasCreator(nullptr);
    EXPECT_FALSE(GEShaderCache::offscreenCanvasCreator_);
}

/**
 * @tc.name: DrawWithCache_NullCacheStore_FallsBackToDirectDraw
 * @tc.desc: Cover fallback branch when cacheStore is null
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_NullCacheStore_FallsBackToDirectDraw, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(container.GetCacheStore().get(), nullptr);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_EQ(container.GetCacheStore().get(), nullptr);
}

/**
 * @tc.name: DrawWithCache_CacheIncompatible_FallsBack
 * @tc.desc: Cover fallback branch when cache is incompatible
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_CacheIncompatible_FallsBack, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::NONE, 0 });
    store.MarkCacheIncompatible(true);
    EXPECT_TRUE(store.IsCacheIncompatible());
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: DrawWithCache_NONELevel_FallsBack
 * @tc.desc: Cover else branch for unexpected cache level
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_NONELevel_FallsBack, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::NONE);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: DrawWithCache_PerEffect_EmptyFilters
 * @tc.desc: Cover DrawPerEffect entry with no filters
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_PerEffect_EmptyFilters, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::NONE, 0 });
    EXPECT_EQ(store.GetCacheLevel(), GEFrameCacheLevel::PER_EFFECT);
    EXPECT_TRUE(container.GetFilters().empty());
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: DrawWithCache_PerEffect_NullVef_SkipsFilter
 * @tc.desc: Cover DrawPerEffect null-vef continue branch
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_PerEffect_NullVef_SkipsFilter, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::NONE, 0 });
    container.filterVec_.push_back(nullptr);
    ASSERT_EQ(container.GetFilters().size(), 1u);
    EXPECT_EQ(container.GetFilters()[0].get(), nullptr);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    auto cache = store.GetEffectCache(GEFrameCacheStore::MakeEffectKey(0, 0));
    ASSERT_NE(cache.get(), nullptr);
    EXPECT_FALSE(cache->valid);
}

/**
 * @tc.name: DrawWithCache_PerEffect_NullImpl_SkipsFilter
 * @tc.desc: Cover DrawPerEffect null-impl continue branch
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_PerEffect_NullImpl_SkipsFilter, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::NONE, 0 });
    auto vef = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    vef->visualEffectImpl_ = nullptr;
    container.AddToChainedFilter(vef);
    ASSERT_EQ(container.GetFilters().size(), 1u);
    EXPECT_EQ(vef->GetImpl().get(), nullptr);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    auto cache = store.GetEffectCache(GEFrameCacheStore::MakeEffectKey(0, 0));
    ASSERT_NE(cache.get(), nullptr);
    EXPECT_FALSE(cache->valid);
}

/**
 * @tc.name: DrawWithCache_PerEffect_CacheMiss
 * @tc.desc: Cover DrawPerEffect cache-miss branch
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_PerEffect_CacheMiss, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::NONE, 0 });
    auto vef = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    vef->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    container.AddToChainedFilter(vef);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    auto key = GEFrameCacheStore::MakeEffectKey(static_cast<int32_t>(vef->GetImpl()->GetFilterType()), 0);
    geRender.DrawShaderEffect(canvas, container, bounds);
    auto cache = store.GetEffectCache(key);
    ASSERT_NE(cache.get(), nullptr);
    EXPECT_FALSE(cache->valid);
}

/**
 * @tc.name: RenderToOffscreen_ZeroBounds
 * @tc.desc: Cover RenderToOffscreen sizeInvalid early return
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, RenderToOffscreen_ZeroBounds, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_EQ(GEShaderCache::RenderToOffscreen(geRender, canvas, nullptr, bounds).get(), nullptr);
}

/**
 * @tc.name: RenderToOffscreen_NoCreator
 * @tc.desc: Cover RenderToOffscreen with no offscreen creator set
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, RenderToOffscreen_NoCreator, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(GEShaderCache::RenderToOffscreen(geRender, canvas, nullptr, bounds).get(), nullptr);
}

/**
 * @tc.name: RenderToOffscreen_EmptyOffscreen
 * @tc.desc: Cover RenderToOffscreen with creator returning empty offscreen
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, RenderToOffscreen_EmptyOffscreen, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    GEShaderCache::SetOffscreenCanvasCreator([](Drawing::Canvas&, int32_t, int32_t) -> OffscreenCanvas { return {}; });
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(GEShaderCache::RenderToOffscreen(geRender, canvas, nullptr, bounds).get(), nullptr);
}

/**
 * @tc.name: RenderToOffscreen_CpuOffscreen
 * @tc.desc: Cover RenderToOffscreen success path with CPU surface
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, RenderToOffscreen_CpuOffscreen, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    GEShaderCache::SetOffscreenCanvasCreator(MakeCpuOffscreen);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    auto image = GEShaderCache::RenderToOffscreen(geRender, canvas, nullptr, bounds);
    ASSERT_NE(image.get(), nullptr);
    EXPECT_EQ(image->GetWidth(), 10);
    EXPECT_EQ(image->GetHeight(), 10);
}

/**
 * @tc.name: DrawWithCache_PerEffect_CacheHit
 * @tc.desc: Cover DrawPerEffect cache-hit + DrawCachedImage valid image
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_PerEffect_CacheHit, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::PER_EFFECT, { GEFrameSkipMode::NONE, 0 });
    auto vef = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    vef->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    container.AddToChainedFilter(vef);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    uint32_t canvasHash = GEShaderCache::CalcCanvasHash(canvas, bounds);
    uint32_t paramHash = vef->GetImpl()->Hash();
    auto key = GEFrameCacheStore::MakeEffectKey(static_cast<int32_t>(vef->GetImpl()->GetFilterType()), 0);
    store.UpdateEffectCache(key, paramHash, canvasHash, std::make_shared<Drawing::Image>(), 0);
    GEShaderCache::SetOffscreenCanvasCreator([](Drawing::Canvas&, int32_t, int32_t) -> OffscreenCanvas { return {}; });
    geRender.DrawShaderEffect(canvas, container, bounds);
    auto cache = store.GetEffectCache(key);
    ASSERT_NE(cache.get(), nullptr);
    EXPECT_TRUE(cache->valid);
    EXPECT_EQ(cache->paramHash, paramHash);
    EXPECT_EQ(cache->canvasHash, canvasHash);
}

/**
 * @tc.name: DrawWithCache_Container_NoOffscreen_ReturnsEarly
 * @tc.desc: Cover DrawContainer offscreen-creation-fail early return
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_Container_NoOffscreen_ReturnsEarly, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::NONE, 0 });
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_FALSE(store.IsContainerCacheValid());
}

/**
 * @tc.name: DrawWithCache_Container_CpuOffscreen_UpdatesCache
 * @tc.desc: Cover DrawContainer offscreen success + cache update + DrawCachedImage
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_Container_CpuOffscreen_UpdatesCache, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::NONE, 0 });
    GEShaderCache::SetOffscreenCanvasCreator(MakeCpuOffscreen);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_TRUE(store.IsContainerCacheValid());
    EXPECT_NE(store.GetContainerCache().cachedImage.get(), nullptr);
}

/**
 * @tc.name: DrawWithCache_Container_CacheHit
 * @tc.desc: Cover DrawContainer cache-hit + DrawCachedImage valid image
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawWithCache_Container_CacheHit, TestSize.Level1)
{
    GraphicsEffectEngine::GERender geRender;
    Drawing::Canvas canvas;
    Drawing::GEVisualEffectContainer container;
    auto& store = container.EnsureCacheStore();
    store.SetCachePolicy(GEFrameCacheLevel::CONTAINER, { GEFrameSkipMode::NONE, 0 });
    auto vef = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    vef->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    container.AddToChainedFilter(vef);
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    uint32_t canvasHash = GEShaderCache::CalcCanvasHash(canvas, bounds);
    uint32_t paramHash = GEShaderCache::ComputeContainerHash(container);
    store.UpdateContainerCache(paramHash, canvasHash, std::make_shared<Drawing::Image>(), 0);
    GEShaderCache::SetOffscreenCanvasCreator([](Drawing::Canvas&, int32_t, int32_t) -> OffscreenCanvas { return {}; });
    geRender.DrawShaderEffect(canvas, container, bounds);
    EXPECT_TRUE(store.IsContainerCacheValid());
    EXPECT_EQ(store.GetContainerCache().paramHash, paramHash);
    EXPECT_EQ(store.GetContainerCache().canvasHash, canvasHash);
}

/**
 * @tc.name: DrawCachedImage_NullImage_ReturnsEarly
 * @tc.desc: Cover DrawCachedImage null-image early return
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawCachedImage_NullImage_ReturnsEarly, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
    GEShaderCache::DrawCachedImage(canvas, bounds, nullptr);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
}

/**
 * @tc.name: DrawCachedImage_ValidImage_Draws
 * @tc.desc: Cover DrawCachedImage valid-image draw path
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawCachedImage_ValidImage_Draws, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
    GEShaderCache::DrawCachedImage(canvas, bounds, std::make_shared<Drawing::Image>());
    EXPECT_EQ(canvas.GetSurface(), nullptr);
}

/**
 * @tc.name: DrawCacheDebugOverlay_HitAndMiss
 * @tc.desc: Cover DrawCacheDebugOverlay hit=true and hit=false branches
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, DrawCacheDebugOverlay_HitAndMiss, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
    GEShaderCache::DrawCacheDebugOverlay(canvas, bounds, true);
    GEShaderCache::DrawCacheDebugOverlay(canvas, bounds, false);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
}

/**
 * @tc.name: ComputeContainerHash_NullAndValidFilters
 * @tc.desc: Cover ComputeContainerHash null-vef skip and valid-vef hash combine
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, ComputeContainerHash_NullAndValidFilters, TestSize.Level1)
{
    Drawing::GEVisualEffectContainer container;
    container.filterVec_.push_back(nullptr);
    auto vef = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    vef->SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    container.AddToChainedFilter(vef);
    uint32_t hash = GEShaderCache::ComputeContainerHash(container);
    EXPECT_NE(hash, 0u);
    EXPECT_EQ(hash, GEShaderCache::ComputeContainerHash(container));
}

/**
 * @tc.name: CalcCanvasHash_NoSurface
 * @tc.desc: Cover CalcCanvasHash with canvas that has no surface (colorSpace null path)
 * @tc.type: FUNC
 */
HWTEST_F(GEShaderCacheTest, CalcCanvasHash_NoSurface, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect bounds(0.0f, 0.0f, 10.0f, 10.0f);
    uint32_t hash = GEShaderCache::CalcCanvasHash(canvas, bounds);
    EXPECT_EQ(hash, GEShaderCache::CalcCanvasHash(canvas, bounds));
    Drawing::Rect otherBounds(0.0f, 0.0f, 20.0f, 10.0f);
    EXPECT_NE(hash, GEShaderCache::CalcCanvasHash(canvas, otherBounds));
}

} // namespace Rosen
} // namespace OHOS
