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
#include "ge_hps_build_pass.h"

namespace OHOS {
namespace Rosen {
std::string_view GEHpsBuildPass::GetLogName() const
{
    return "GEHpsBuildPass";
}

GEFilterComposerPassResult GEHpsBuildPass::Run(std::vector<GEFilterComposable>& composables)
{
    auto hpsEffectFilter = std::make_shared<HpsEffectFilter>(canvas_);
    const auto& context = context_.get();
    if (!hpsEffectFilter->IsHpsEffectEnabled() || composables.empty()) {
        return GEFilterComposerPassResult { false };
    }
    std::vector<GEFilterComposable> resultComposables;
    std::shared_ptr<HpsEffectFilter> composedHpsFilter = std::make_shared<HpsEffectFilter>(canvas_);
    for (auto& composable : composables) {
        auto effect = composable.GetEffect();
        if (effect == nullptr || !hpsEffectFilter->IsEffectSupported(effect)) {
            return GEFilterComposerPassResult { false }; // Only compose when all effects in the sequence support HPS
        }
        composedHpsFilter->GenerateVisualEffectFromGE(effect->GetImpl(), context.src, context.dst,
            context.saturationForHPS, context.brightnessForHPS, context.image);
    }
    // All effects are traversed and are hps-composable.
    resultComposables.push_back(composedHpsFilter);
    composables.swap(resultComposables);
    LOGD("GEHpsBuildPass::Run HPS filter composed");
    return GEFilterComposerPassResult { true };
}

} // namespace Rosen
} // namespace OHOS