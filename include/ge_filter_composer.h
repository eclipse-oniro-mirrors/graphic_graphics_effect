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
/**
 * @class GEFilterComposer
 * @brief Orchestrates a sequence of filter composition passes over graphical effects.
 *
 * This class manages a pipeline of @ref GEFilterComposerPass objects, applying them
 * sequentially to a list of @ref GEFilterComposable objects. It is designed to separate
 * specific optimization logics from GERender and reduce cross-pass dependencies or side effects.
 *
 * Usage typically involves:
 * 1. Creating a composer instance.
 * 2. Adding one or more passes (e.g., `Add<GEHpsBuildPass>(canvas, context)`).
 * 3. Converting visual effects into composables via @ref BuildComposables.
 * 4. Running the pipeline with @ref Run.
 *
 * @see GEFilterComposerPass
 * @see GEFilterComposable
 * @see GEFilterComposerPassResult
 * @see GEFilterComposer::ComposerRunResult
 */
class GEFilterComposer {
public:
    GEFilterComposer() = default;
    ~GEFilterComposer() = default;

    // Add a GEFilterComposerPass with its constructor
    template <typename Pass, typename... Args> void Add(Args&&... args)
    {
        static_assert(std::is_base_of_v<GEFilterComposerPass, Pass>, "Pass should be subtype of GEFilterComposerPass");
        Add(std::make_unique<Pass>(std::forward<Args>(args)...));
    }

    // Add a GEFilterComposerPass
    void Add(std::unique_ptr<GEFilterComposerPass> pass);

    struct ComposerRunResult {
        bool anyPassChanged; // True if at least one pass made changes; false otherwise.
    };

    // Executes all registered passes in sequence on the given composables.
    // Applies each pass in the order they were added.
    ComposerRunResult Run(std::vector<GEFilterComposable>& composables) const;
    
    // Converts a list of GEVisualEffect objects into GEFilterComposable wrappers.
    static std::vector<GEFilterComposable> BuildComposables(
        const std::vector<std::shared_ptr<Drawing::GEVisualEffect>>&);

private:
    std::vector<std::unique_ptr<GEFilterComposerPass>> passes_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_EFFECT_GE_FILTER_COMPOSER_H
