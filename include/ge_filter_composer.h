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

#ifndef GRAPHICS_EFFECT_GE_FILTER_COMPOSER_H
#define GRAPHICS_EFFECT_GE_FILTER_COMPOSER_H

#include <memory>
#include <vector>

#include "ge_filter_composer_pass.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
namespace OHOS {
namespace Rosen {
class GEFilterComposer {
public:
    GEFilterComposer() = default;
    ~GEFilterComposer() = default;

    // Add a GEFilterComposerPass with its constructor
    template <typename Pass, typename... Args> void Add(Args&&... args)
    {
        static_assert(std::is_base_of_v<GEFilterComposerPass, Pass>, "Pass should be subtype of GEFilterComposerPass");
        Add(std::make_shared<Pass>(std::forward<Args>(args)...));
    }

    // Add a GEFilterComposerPass
    template <typename Pass> void Add(std::shared_ptr<Pass> pass)
    {
        static_assert(std::is_base_of_v<GEFilterComposerPass, Pass>, "Pass should be subtype of GEFilterComposerPass");
        Add(std::static_pointer_cast<GEFilterComposerPass>(pass));
    }

    // Add a GEFilterComposerPass
    void Add(std::shared_ptr<GEFilterComposerPass> pass);
    
    struct ComposerRunResult {
        bool anyPassChanged;
    };

    // Transform composables with GEFilterComposerPass
    ComposerRunResult Run(std::vector<GEFilterComposable>& composables) const;
    
    // Convert GEVisualEffects into GEFilterComposables
    static std::vector<GEFilterComposable> BuildComposables(
        const std::vector<std::shared_ptr<Drawing::GEVisualEffect>>&);

private:
    std::vector<std::shared_ptr<GEFilterComposerPass>> passes_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_EFFECT_GE_FILTER_COMPOSER_H
