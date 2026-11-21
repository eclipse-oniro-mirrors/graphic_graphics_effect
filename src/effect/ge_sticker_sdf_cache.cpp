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

#include "ge_sticker_sdf_cache.h"

#include <mutex>

#include "effect/shader/ge_sticker_outline_shader.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

namespace {
struct StickerSDFCacheEntry {
    uint32_t stickerUniqueID = 0;
    int stickerWidth = 0;
    int stickerHeight = 0;
    int spreadFactor = 0;
    std::shared_ptr<Drawing::Image> decodedSdfImage = nullptr;
};

std::mutex g_stickerSDFCacheMutex;
StickerSDFCacheEntry g_stickerSDFCache;
} // namespace

std::shared_ptr<Drawing::Image> GEStickerSDFCache::MakeCachedDecodedStickerSDFImage(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& sticker, float requiredRangePx)
{
    if (sticker == nullptr) {
        GE_LOGE("GEStickerSDFCache::MakeCachedDecodedStickerSDFImage sticker is nullptr.");
        return nullptr;
    }
    const int spreadFactor = GEStickerOutlineShader::CalSdfSpreadFactor(requiredRangePx);
    {
        std::lock_guard<std::mutex> lock(g_stickerSDFCacheMutex);
        if (g_stickerSDFCache.stickerUniqueID == sticker->GetUniqueID() &&
            g_stickerSDFCache.stickerWidth == sticker->GetWidth() &&
            g_stickerSDFCache.stickerHeight == sticker->GetHeight() && g_stickerSDFCache.spreadFactor == spreadFactor &&
            g_stickerSDFCache.decodedSdfImage != nullptr) {
            return g_stickerSDFCache.decodedSdfImage;
        }
    }

    auto sdfImage = GEStickerOutlineShader::MakeStickerSDFImage(canvas, sticker, requiredRangePx);
    if (sdfImage == nullptr) {
        GE_LOGE("GEStickerSDFCache::MakeCachedDecodedStickerSDFImage make SDF image failed.");
        return nullptr;
    }
    auto decodedSdfImage =
        GEStickerOutlineShader::MakeDecodedStickerSDFImage(canvas, sdfImage, sticker->GetImageInfo(), requiredRangePx);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerSDFCache::MakeCachedDecodedStickerSDFImage make decoded SDF image failed.");
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_stickerSDFCacheMutex);
    g_stickerSDFCache.stickerUniqueID = sticker->GetUniqueID();
    g_stickerSDFCache.stickerWidth = sticker->GetWidth();
    g_stickerSDFCache.stickerHeight = sticker->GetHeight();
    g_stickerSDFCache.spreadFactor = spreadFactor;
    g_stickerSDFCache.decodedSdfImage = decodedSdfImage;
    return decodedSdfImage;
}

} // namespace Rosen
} // namespace OHOS
