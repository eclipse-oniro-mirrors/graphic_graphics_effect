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
 * @interface IGEFilterComposableFlags
 * @brief Base interface for settable flags used in filter composition passes.
 * For convinent use, please derive from GEFilterComposableFlags<YourFlagType>
 *
 * @see GEFilterComposableFlags
 */
class IGEFilterComposableFlags : public ExactDowncastUtils::ExactDowncastable {};

/**
 * @class GEFilterComposableFlags
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

/**
 * @class GEFilterComposable
 * @brief Represents a composable graphical effect with optional flags.
 *
 * This class wraps either a `GEVisualEffect` or `HpsEffectFilter` and provides
 * type-safe access to both through the `Get<T>()` method. It also supports attaching
 * a set of composition flags via `SetFlags()` and retrieving them via `GetFlags<T>()`.
 *
 * The class uses `std::variant` to hold one of the two effect types, ensuring
 * memory safety and avoiding raw pointers. The `flags_` member allows pass-specific
 * metadata to be associated with the effect during composition.
 *
 * @note The `GetEffect()` and `GetHpsEffect()` methods return shared_ptr to the
 *       underlying effect type, or `nullptr` if the variant does not contain that type.
 *
 * @see GEFilterComposerPass
 * @see GEFilterComposer
 * @see IGEFilterComposableFlags
 */
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

    /**
     * @brief Returns the underlying GEVisualEffect, if present.
     * @return Shared pointer to GEVisualEffect, or nullptr if not present.
     */
    std::shared_ptr<Drawing::GEVisualEffect> GetEffect()
    {
        return Get<Drawing::GEVisualEffect>();
    }

    /**
     * @brief Returns the underlying HpsEffectFilter, if present.
     * @return Shared pointer to HpsEffectFilter, or nullptr if not present.
     */
    std::shared_ptr<HpsEffectFilter> GetHpsEffect()
    {
        return Get<HpsEffectFilter>();
    }

    /**
     * @brief Sets flags of type T using provided arguments.
     *
     * This method creates a new instance of `T` using the provided arguments and
     * sets it as the flags for this composable.
     * @tparam T The flag type to set.
     * @tparam Args Variadic template parameters for constructing T.
     * @param args Arguments to pass to the constructor of T.
     * @note T must be derived from `IGEFilterComposableFlags`.
     */
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

    /**
     * @brief Retrieves the flags as type T, if present.
     * @tparam T The desired flag type.
     * @return Pointer to T if the flags are of that type; otherwise, nullptr.
     * @note The returned pointer is non-owning. Do not modify or delete it.
     */
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

/**
 * @class GEFilterComposerPass
 * @brief Base class for filter composition passes in the graphics effect pipeline.
 *
 * This abstract base class defines the interface for individual passes in the filter
 * composition pipeline. Each pass is responsible for transforming a collection of
 * @ref GEFilterComposable objects, potentially modifying them or combining them
 * into more efficient representations.
 * @see GEFilterComposer
 * @see GEFilterComposable
 * @see GEFilterComposerPassResult
 */
class GEFilterComposerPass {
public:
    virtual ~GEFilterComposerPass() = default;
    // Log only, DO NOT use it for serialization
    virtual std::string_view GetLogName() const = 0;

    /**
     * @brief Executes the composition pass on the given list of composables.
     *
     * This method performs the actual transformation logic of the pass. It may
     * modify the composables in place, merge them, or replace them with new
     * representations (e.g., converting multiple visual effects into a single HPS filter).
     * @param[in,out] composables A reference to a vector of @ref GEFilterComposable
     *                            objects to be processed.
     * @return A @ref GEFilterComposerPassResult indicating whether any changes were
     *         made to the composables.
     * @note The pass is allowed to modify the vector in-place (e.g., by erasing,
     *       replacing, or reordering elements).
     */
    virtual GEFilterComposerPassResult Run(std::vector<GEFilterComposable>& composables) = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FILTER_COMPOSER_PASS_H