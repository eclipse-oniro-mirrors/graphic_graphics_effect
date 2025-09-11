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

#ifndef GRAPHICS_EFFECT_GE_FILTER_COMPOSER_PASS_H
#define GRAPHICS_EFFECT_GE_FILTER_COMPOSER_PASS_H
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "ge_hps_effect_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class GEFilterComposable {
    using GEViusalEffect = Drawing::GEVisualEffect;

public:
    GEFilterComposable(const std::shared_ptr<GEViusalEffect>& effect) : effect_(effect) {}
    GEFilterComposable(const std::shared_ptr<HpsEffectFilter>& effect) : effect_(effect) {}

    template<typename T>
    std::shared_ptr<T> Get()
    {
        auto p = std::get_if<std::shared_ptr<T>>(&effect_);
        if (p != nullptr) {
            return *p;
        }
        return nullptr;
    }

    std::shared_ptr<Drawing::GEVisualEffect> GetEffect()
    {
        return Get<Drawing::GEVisualEffect>();
    }

    std::shared_ptr<HpsEffectFilter> GetHpsEffect()
    {
        return Get<HpsEffectFilter>();
    }

private:
    std::variant<std::shared_ptr<Drawing::GEVisualEffect>, std::shared_ptr<HpsEffectFilter>> effect_;
};

struct GEFilterComposerPassResult {
    bool changed;
};

class GEFilterComposerPass {
public:
    virtual ~GEFilterComposerPass() = default;
    // Log only, DO NOT use it for serialization
    virtual std::string_view GetLogName() const = 0;
    // Execute the transform pass for effects
    virtual GEFilterComposerPassResult Run(std::vector<GEFilterComposable>& composables) = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FILTER_COMPOSER_PASS_H