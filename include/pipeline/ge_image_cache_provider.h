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
#ifndef GRAPHICS_EFFECT_GE_IMAGE_CACHE_PROVIDER_H
#define GRAPHICS_EFFECT_GE_IMAGE_CACHE_PROVIDER_H
#include <memory>

#include "ge_cache_provider.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

struct GEImageCache : GECache<GEImageCache> {
    std::shared_ptr<Drawing::Image> data;
};

class GEImageCacheProvider : public GECacheProvider<GEImageCacheProvider> {
public:
    const IGECache* GetFirst() const override
    {
        return static_cast<const IGECache*>(&cache_);
    }

    virtual bool Store(const IGECache& cache) override
    {
        auto imageCache = cache.As<GEImageCache>();
        if (imageCache != nullptr) {
            cache_.data = imageCache->data;
            return true;
        }
        return false;
    }

private:
    GEImageCache cache_ {};
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_IMAGE_CACHE_PROVIDER_H