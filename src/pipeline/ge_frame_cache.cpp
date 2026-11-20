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

#include "ge_frame_cache.h"

namespace OHOS {
namespace Rosen {

bool GEIsCacheHit(GEFrameCacheData& cache, uint32_t paramHash, uint32_t canvasHash,
    const GEFrameSkipPolicy& policy)
{
    if (!cache.valid) {
        return false;
    }
    bool condHash = cache.paramHash == paramHash && cache.canvasHash == canvasHash;
    if (condHash) {
        return true;
    }
    if (policy.mode == GEFrameSkipMode::FIXED_INTERVAL && cache.remainingSkipFrames > 0) {
        cache.remainingSkipFrames--;
        return true;
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS
