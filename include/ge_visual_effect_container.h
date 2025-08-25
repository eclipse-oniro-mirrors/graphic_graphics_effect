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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_CONTAINER_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_CONTAINER_H

#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GEVisualEffectContainer {
public:
    GEVisualEffectContainer();
    ~GEVisualEffectContainer() = default;

    void AddToChainedFilter(std::shared_ptr<Drawing::GEVisualEffect> visualEffect);

    const std::vector<std::shared_ptr<GEVisualEffect>>& GetFilters() const
    {
        return filterVec_;
    }

    void UpdateCacheDataFrom(const std::shared_ptr<GEVisualEffectContainer>& ge);

    void SetGeometry(const Drawing::Canvas& canvas, float geoWidth, float geoHeight);

    void SetDisplayHeadroom(float headroom);

    void RemoveFilterWithType(int32_t typeToRemove);

private:
    std::shared_ptr<GEVisualEffect> GetGEVisualEffect(const std::string& name);

    std::vector<std::shared_ptr<GEVisualEffect>> filterVec_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_CONTAINER_H
