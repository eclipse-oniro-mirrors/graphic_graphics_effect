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

#ifndef GRAPHICS_EFFECT_GE_FRAME_CACHE_H
#define GRAPHICS_EFFECT_GE_FRAME_CACHE_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "effect/shader_effect.h"
#include "ge_common.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

enum class GEFrameCacheLevel : int32_t {
    NONE = 0,
    PER_EFFECT = 1,
    CONTAINER = 2,
};

enum class GEFrameSkipMode : int32_t {
    NONE = 0,
    ON_PARAM_SAME = 1,
    FIXED_INTERVAL = 2,
};

struct GEFrameSkipPolicy {
    GEFrameSkipMode mode = GEFrameSkipMode::NONE;
    uint32_t skipInterval = 0;
};

struct GEFrameCacheData {
    uint32_t paramHash = 0;
    uint32_t canvasHash = 0;
    std::shared_ptr<Drawing::Image> cachedImage = nullptr;
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect = nullptr;
    uint32_t remainingSkipFrames = 0;
    bool valid = false;
};

GE_EXPORT bool GEIsCacheHit(GEFrameCacheData& cache, uint32_t paramHash, uint32_t canvasHash,
    const GEFrameSkipPolicy& policy);

class GEFrameCacheStore {
public:
    static std::string MakeEffectKey(int32_t filterType, size_t index)
    {
        return std::to_string(filterType) + "_" + std::to_string(index);
    }

    void SetCachePolicy(GEFrameCacheLevel level, const GEFrameSkipPolicy& skipPolicy)
    {
        if (level != cacheLevel_ || skipPolicy.mode != skipPolicy_.mode) {
            InvalidateContainerCache();
            InvalidateEffectCaches();
        }
        // skipInterval-only change: no invalidation needed. Cached images are
        // still valid (params unchanged). Current skip period finishes naturally;
        // new skipInterval takes effect on next cache update via remainingSkipFrames.
        cacheLevel_ = level;
        skipPolicy_ = skipPolicy;
        containerCacheEnabled_ = (level == GEFrameCacheLevel::CONTAINER);
    }

    void ApplyCachePolicy(GEFrameCacheLevel level, const GEFrameSkipPolicy& skipPolicy,
        const std::vector<int32_t>& filterTypes = {})
    {
        SetCachePolicy(level, skipPolicy);
        if (level != GEFrameCacheLevel::PER_EFFECT) {
            return;
        }
        for (size_t i = 0; i < filterTypes.size(); i++) {
            SetEffectSkipPolicy(MakeEffectKey(filterTypes[i], i), skipPolicy);
        }
    }

    GEFrameCacheLevel GetCacheLevel() const { return cacheLevel_; }
    const GEFrameSkipPolicy& GetSkipPolicy() const { return skipPolicy_; }
    bool IsContainerCacheEnabled() const { return containerCacheEnabled_; }
    bool IsCacheIncompatible() const { return cacheIncompatible_; }
    // CONTAINER composites all effects on a transparent offscreen surface, then
    // pastes back as a single image. Non-default canvas alpha changes overlap
    // results, and non-SRC_OVER blend mode is lost during paste-back.
    // PER_EFFECT renders each effect independently and is unaffected.
    void MarkCacheIncompatible(bool incompatible)
    {
        cacheIncompatible_ = incompatible && cacheLevel_ == GEFrameCacheLevel::CONTAINER;
    }

    const GEFrameCacheData& GetContainerCache() const { return containerCacheData_; }
    bool IsContainerCacheValid() const { return containerCacheData_.valid; }
    GEFrameCacheData& GetContainerCacheData() { return containerCacheData_; }
    void InvalidateContainerCache() { containerCacheData_.valid = false; }

    void UpdateContainerCache(uint32_t paramHash, uint32_t canvasHash,
        std::shared_ptr<Drawing::Image> image, uint32_t skipFrames)
    {
        containerCacheData_.paramHash = paramHash;
        containerCacheData_.canvasHash = canvasHash;
        containerCacheData_.cachedImage = image;
        containerCacheData_.remainingSkipFrames = skipFrames;
        containerCacheData_.valid = true;
    }

    std::shared_ptr<GEFrameCacheData> GetEffectCache(const std::string& key)
    {
        auto it = effectRenderCaches_.find(key);
        if (it != effectRenderCaches_.end()) {
            return it->second;
        }
        auto cache = std::make_shared<GEFrameCacheData>();
        effectRenderCaches_[key] = cache;
        return cache;
    }

    void UpdateEffectCache(const std::string& key, uint32_t paramHash, uint32_t canvasHash,
        std::shared_ptr<Drawing::Image> image, uint32_t skipFrames)
    {
        if (!image) {
            return;
        }
        auto& cache = effectRenderCaches_[key];
        if (!cache) {
            cache = std::make_shared<GEFrameCacheData>();
        }
        cache->paramHash = paramHash;
        cache->canvasHash = canvasHash;
        cache->cachedImage = image;
        cache->remainingSkipFrames = skipFrames;
        cache->valid = true;
    }

    void SetEffectSkipPolicy(const std::string& key, const GEFrameSkipPolicy& policy)
    {
        effectSkipPolicies_[key] = policy;
    }

    const GEFrameSkipPolicy& GetEffectSkipPolicy(const std::string& key) const
    {
        static const GEFrameSkipPolicy defaultPolicy {};
        auto it = effectSkipPolicies_.find(key);
        return it != effectSkipPolicies_.end() ? it->second : defaultPolicy;
    }

    void InvalidateEffectCaches()
    {
        for (auto& [key, cache] : effectRenderCaches_) {
            if (cache) {
                cache->valid = false;
            }
        }
    }

    void TrimEffectCaches(const std::unordered_set<std::string>& validKeys)
    {
        std::unordered_set<std::string> valid(validKeys.begin(), validKeys.end());
        for (auto it = effectRenderCaches_.begin(); it != effectRenderCaches_.end();) {
            if (valid.find(it->first) == valid.end()) {
                it = effectRenderCaches_.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = effectSkipPolicies_.begin(); it != effectSkipPolicies_.end();) {
            if (valid.find(it->first) == valid.end()) {
                it = effectSkipPolicies_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    GEFrameCacheLevel cacheLevel_ = GEFrameCacheLevel::NONE;
    GEFrameSkipPolicy skipPolicy_ {};
    bool containerCacheEnabled_ = false;
    bool cacheIncompatible_ = false;
    GEFrameCacheData containerCacheData_ {};
    std::unordered_map<std::string, std::shared_ptr<GEFrameCacheData>> effectRenderCaches_;
    std::unordered_map<std::string, GEFrameSkipPolicy> effectSkipPolicies_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FRAME_CACHE_H
