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

#include "ge_filter_composer.h"
#include "ge_log.h"
#include "ge_render.h"

namespace OHOS {
namespace Rosen {

void GEFilterComposer::Add(std::shared_ptr<GEFilterComposerPass> pass)
{
    if (pass != nullptr) {
        passes_.push_back(pass);
    } else {
        LOGE("GEFilterComposer::Add nullptr pass found");
    }
}

GEFilterComposer::ComposerRunResult GEFilterComposer::Run(std::vector<GEFilterComposable>& composables) const
{
    ComposerRunResult runResult {false};
    for (auto& pass : passes_) {
        auto result = pass->Run(composables);
        auto name = pass->GetLogName();
        if (result.changed) {
            runResult.anyPassChanged = true;
            LOGD("GEFilterComposer::Transform Pass %s changed the effects", name.data());
        } else {
            LOGD("GEFilterComposer::Transform Pass %s does not change effects", name.data());
        }
    }
    return runResult;
}

std::vector<GEFilterComposable> GEFilterComposer::BuildComposables(
    const std::vector<std::shared_ptr<Drawing::GEVisualEffect>>& effects)
{
    std::vector<GEFilterComposable> composables;
    for (const auto& effect: effects) {
        composables.emplace_back(effect);
    }
    return composables;
}

} // namespace Rosen
} // namespace OHOS
