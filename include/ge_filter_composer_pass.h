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

#include "utils/ge_downcast.h"

namespace OHOS {
namespace Rosen {
/**
 * @brief Base interface for settable flags used in filter composition passes.
 * For convinent use, please derive from GEFilterComposableFlags<YourFlagType>
 * 
 * @see GEFilterComposableFlags
 */
class IGEFilterComposableFlags : public ExactDowncastUtils::ExactDowncastable {};

/**
 * @brief Template base class for settable flags used in filter composition passes.
 *
 * This template class provides a convenient base for defining specific flag types
 * used by `GEFilterComposerPass` instances. It inherits from `IGEFilterComposableFlags`
 * and implements `GetExactTypeID` using the `TypeID::Get<Derived>()` mechanism,
 * enabling exact type identification.
 *
 * @tparam Derived The concrete derived class type (CRTP pattern).
 *
 * @see IGEFilterComposableFlags
 * @see ExactDowncastUtils
 */
template<typename Derived>
class GEFilterComposableFlags : public IGEFilterComposableFlags {
public:
    /**
     * @brief Returns the unique type ID for the derived class.
     *
     * This method overrides `GetExactTypeID` from `ExactDowncastable` and returns
     * the compile-time unique ID generated via the `TypeID::Get<Derived>()` mechanism.
     *
     * @return The unique type ID for the derived class.
     */
    ExactDowncastUtils::TypeID GetExactTypeID() const override
    {
        return ExactDowncastUtils::TypeID::Get<Derived>();
    }
};

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

    // Set the related flags by some GEFilterComposerPass
    template<typename T, typename... Args>
    void SetFlags(Args&&... args)
    {
        static_assert(std::is_base_of_v<IGEFilterComposableFlags, T>, "T must be subtype of IGEFilterComposableFlags");
        SetFlags(std::make_shared<T>(std::forward<T>(args)...));
    }

    // Set the related flags by some GEFilterComposerPass
    void SetFlags(const std::shared_ptr<IGEFilterComposableFlags>& flags)
    {
        flags_ = flags;
    }

    // Readonly getter returns a non-owning pointer
    // Modify the return value is not allowed. Call SetFlags() if you want to.
    template<typename T>
    const T* GetFlags() const
    {
        return flags_ != nullptr ? flags_->As<T>() : nullptr;
    }

private:
    std::variant<std::shared_ptr<Drawing::GEVisualEffect>, std::shared_ptr<HpsEffectFilter>> effect_;
    std::shared_ptr<IGEFilterComposableFlags> flags_ = nullptr;
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