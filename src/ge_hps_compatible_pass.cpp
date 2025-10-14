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
#include "ge_hps_compatible_pass.h"

namespace OHOS {
namespace Rosen {
std::string_view GEHpsCompatiblePass::GetLogName() const
{
    return "GEHpsCompatiblePass";
}

bool GEHpsCompatiblePass::IsBlurFilterExists() const
{
    return blurFilterExists_;
}

GEFilterComposerPassResult GEHpsCompatiblePass::Run(std::vector<GEFilterComposable>& composables)
{
    using Drawing::GEFilterType;
    blurFilterExists_ = false;
    for (auto& composable : composables) {
        if (auto effect = composable.GetEffect(); effect != nullptr) {
            const auto& ve = effect->GetImpl();
            if (ve == nullptr) {
                continue;
            }
            auto type = ve->GetFilterType();
            if (type == GEFilterType::KAWASE_BLUR || type == GEFilterType::MESA_BLUR) {
                blurFilterExists_ = true;
                break;
            }
        }
    }
    return GEFilterComposerPassResult { false }; // This pass does not modify composables
}

} // namespace Rosen
} // namespace OHOS