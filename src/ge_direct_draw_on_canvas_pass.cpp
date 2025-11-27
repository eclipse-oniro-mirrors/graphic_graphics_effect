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

#include "ge_direct_draw_on_canvas_pass.h"

namespace OHOS {
namespace Rosen {

static bool IsDirectDrawOnCanvasSupported(Drawing::GEFilterType type)
{
    // NOTICE: DO NOT use std::map/std::unordered_set to replace switch/case, it's worse in performance, not better.
    switch (type) {
        // Currently only GEFrostedGlassShaderFilter is supported.
        // If you want to add new support, add a case line below to support it
        // When static reflection infrastructure refactor is done, use that to query info.
        case Drawing::GEFilterType::FROSTED_GLASS:
            return true;
        default:
            return false;
    }
    return false; // should be unreachable
}

bool DirectDrawOnCanvasFlag::IsDirectDrawOnCanvasEnabled(const GEFilterComposable& composable)
{
    auto flag = composable.GetFlags<DirectDrawOnCanvasFlag>();
    return flag != nullptr && flag->isDirectDrawOnCanvasEnabled;
}

std::string_view GEDirectDrawOnCanvasPass::GetLogName() const
{
    return "GEDirectDrawOnCanvasPass";
}

GEFilterComposerPassResult GEDirectDrawOnCanvasPass::Run(std::vector<GEFilterComposable>& composables)
{
    if (composables.empty()) {
        return GEFilterComposerPassResult { false };
    }
    // now it won't be empty, it's safe to access .back()
    auto& lastOne = composables.back();
    auto effect = lastOne.GetEffect();
    if (effect == nullptr) {
        return GEFilterComposerPassResult { false };
    }
    const auto& impl = effect->GetImpl();
    if (impl == nullptr) {
        return GEFilterComposerPassResult { false };
    }

    if (IsDirectDrawOnCanvasSupported(impl->GetFilterType())) {
        LOGD("GEDirectDrawOnCanvasPass: direct draw on canvas is enabled for %d", impl->GetFilterType());
        lastOne.SetFlags<DirectDrawOnCanvasFlag>(true);
        return GEFilterComposerPassResult { true };
    }

    return GEFilterComposerPassResult { false };
}

} // namespace Rosen
} // namespace OHOS