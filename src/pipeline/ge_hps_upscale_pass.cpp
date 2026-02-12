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
#include "ge_hps_upscale_pass.h"

#include <memory>

#include "ge_filter_type.h"

namespace OHOS {
namespace Rosen {

std::string_view GEHpsUpscalePass::GetLogName() const
{
    return "GEHpsUpscalePass";
}

GEFilterComposerPassResult GEHpsUpscalePass::Run(std::vector<GEFilterComposable>& composables)
{
    using Drawing::GEFilterType;
    bool changed = false;
    for (size_t i = 0; i < composables.size(); ++i) {
        // Upscale only for hps effect which needed
        auto hpsEffect = composables[i].GetHpsEffect();
        if (hpsEffect == nullptr) {
            continue;
        }
        if (i == composables.size() - 1) {  // Dont need upscale for last hps effect
            if (hpsEffect->IsNeedUpscale()) {
                hpsEffect->SetNeedUpscale(false);
                changed = true;
            }
            continue;
        }
        if (composables[i + 1].GetEffect()) { // Need upscale is ge effect after hps effect
            if (!hpsEffect->IsNeedUpscale()) {
                hpsEffect->SetNeedUpscale(true);
                changed = true;
            }
            continue;
        }
    }
    return GEFilterComposerPassResult { changed };
}
} // namespace Rosen
} // namespace OHOS
