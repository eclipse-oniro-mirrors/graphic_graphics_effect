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
    if (!hpsEffectFilter->IsHpsEffectEnabled()) {
        return GEFilterComposerPassResult {false};
    }
    std::vector<GEFilterComposable> resultComposables;
    std::shared_ptr<HpsEffectFilter> composedHpsFilter = nullptr;
    bool composed = false;
    for (auto& composable: composables) {
        auto effect = composable.GetEffect();
        if (effect == nullptr || !hpsEffectFilter->IsEffectSupported(effect)) {
            if (composedHpsFilter != nullptr) { // save the composed hps filter and leave non-VisualEffects as is
                resultComposables.push_back(composedHpsFilter);
                composedHpsFilter = nullptr;
            }
            resultComposables.push_back(composable);
            continue;
        }
        if (composedHpsFilter == nullptr) {
            composedHpsFilter = std::make_shared<HpsEffectFilter>(canvas_);
        }
        composedHpsFilter->GenerateVisualEffectFromGE(effect->GetImpl(),
            context_.src, context_.dst, context_.saturationForHPS, context_.brightnessForHPS, context_.image);
        composed = true;
    }
    if (composedHpsFilter != nullptr) {
        resultComposables.push_back(composedHpsFilter);
        composedHpsFilter = nullptr;
    }
    if (composed) {
        composables.swap(resultComposables);
        LOGD("GEHpsBuildPass::Run HPS filter composed");
    }
    return GEFilterComposerPassResult {composed};
}

}
}