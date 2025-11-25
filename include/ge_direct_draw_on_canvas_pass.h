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
#ifndef GRAPHICS_EFFECT_GE_DIRECT_DRAW_ON_CANVAS_PASS_H
#define GRAPHICS_EFFECT_GE_DIRECT_DRAW_ON_CANVAS_PASS_H
#include "ge_filter_composer_pass.h"
#include "ge_render.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

struct DirectDrawOnCanvasFlag : GEFilterComposableFlags<DirectDrawOnCanvasFlag> {
    bool isDirectDrawOnCanvasEnabled;

    static bool IsDirectDrawOnCanvasEnabled(const GEFilterComposable& composable);
};

// Enable direct drawing on canvas for the last GEVisualEffect in composables
class GEDirectDrawOnCanvasPass : public GEFilterComposerPass {
public:
    std::string_view GetLogName() const override;
    GEFilterComposerPassResult Run(std::vector<GEFilterComposable>& composables) override;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_DIRECT_DRAW_ON_CANVAS_PASS_H