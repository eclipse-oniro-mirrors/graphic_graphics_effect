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

#include "ge_cache_provider.h"
#include "ge_image_cache_provider.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GECacheProviderTest : public testing::Test {
public:
    static void SetupTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

    std::shared_ptr<Drawing::Image> MakeImage();
};

std::shared_ptr<Drawing::Image> GECacheProviderTest::MakeImage()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    return bmp.MakeImage();
}

HWTEST_F(GECacheProviderTest, GEImageCacheCast, TestSize.Level0)
{
    GEImageCache imageCache;
    const IGECache& cache = imageCache;
    EXPECT_TRUE(cache.Is<GEImageCache>());
}

HWTEST_F(GECacheProviderTest, GEImageCacheProviderCast, TestSize.Level0)
{
    GEImageCacheProvider imageCacheProvider;
    const IGECacheProvider& cacheProvider = imageCacheProvider;
    EXPECT_TRUE(cacheProvider.Is<GEImageCacheProvider>());
}

HWTEST_F(GECacheProviderTest, GEImageCacheProviderStore, TestSize.Level0)
{
    GEImageCacheProvider imageCacheProvider;
    IGECacheProvider& cacheProvider = imageCacheProvider;

    GEImageCache cache;
    cache.data = MakeImage();
    ASSERT_NE(cache.data, nullptr);
    cacheProvider.Store(cache);
    auto pCache = cacheProvider.GetFirst();
    ASSERT_NE(pCache, nullptr);
    auto pImageCache = pCache->As<GEImageCache>();
    ASSERT_NE(pImageCache, nullptr);
    ASSERT_EQ(pImageCache->data, cache.data);
}

} // namespace Rosen
} // namespace OHOS