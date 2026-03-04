/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_FILTER_PARAM_H
#define GRAPHICS_EFFECT_GE_FILTER_PARAM_H
#include <memory>

#include "ge_filter_type.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
// Type-erasured params class
class GEFilterParams {
public:
    template<typename T>
    struct OptionalTypeTrait {
        using Type = std::optional<T>;
    };
    template<typename T>
    struct OptionalTypeTrait<std::shared_ptr<T>> {
        using Type = std::shared_ptr<T>;
    };
    template<typename T>
    using OptionalType = typename OptionalTypeTrait<T>::Type;

    template<typename T>
    static constexpr bool IsRegisteredFilterTypeInfo = GEFilterTypeInfo<T>::ID != GEFilterType::NONE;
    template<typename T>
    static constexpr bool IsRegisteredParamTypeInfo = GEFilterParamsTypeInfo<T>::ID != GEFilterType::NONE;

    template<typename T>
    static OptionalType<T> Unbox(const std::shared_ptr<GEFilterParams>& params);
    template<typename T>
    static std::shared_ptr<GEFilterParams> Box(T&& params);

    virtual ~GEFilterParams() = default;

    GEFilterType GetType() { return id; }

protected:
    GEFilterParams(GEFilterType type) : id(type) {}
    GEFilterType id;
};

// Type-specific proxy wrapper of params
template<typename T>
class GEFilterParamsWrapper : public GEFilterParams {
public:
    using ParamType = T;
    using FilterType = typename GEFilterParamsTypeInfo<T>::FilterType;
    static constexpr std::nullopt_t Null = std::nullopt;
    static_assert(std::is_void_v<FilterType> || GEFilterParams::IsRegisteredFilterTypeInfo<FilterType>,
        "FilterType wrongly registered");
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<ParamType>, "Unregistered GEFilterParams type");
    template<typename U>
    GEFilterParamsWrapper(U&& params)
        : GEFilterParams(GEFilterParamsTypeInfo<ParamType>::ID), data(std::forward<U>(params))
    {}

    T data;
};

// Type-specific proxy wrapper of params, specialization of std::shared_ptr<T> for handy use
template<typename T>
class GEFilterParamsWrapper<std::shared_ptr<T>> : public GEFilterParams {
public:
    using ParamType = T;
    using FilterType = typename GEFilterParamsTypeInfo<T>::FilterType;
    static constexpr std::nullptr_t Null = nullptr;
    static_assert(std::is_void_v<FilterType> || GEFilterParams::IsRegisteredFilterTypeInfo<FilterType>,
        "FilterType wrongly registered");
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<ParamType>, "Unregistered GEFilterParams type");
    GEFilterParamsWrapper(const std::shared_ptr<T>& params)
        : GEFilterParams(GEFilterParamsTypeInfo<ParamType>::ID), data(params)
    {}

    std::shared_ptr<T> data;
};

// Checked downcast from GEFilterParams to specific params type
template<typename T>
GEFilterParams::OptionalType<T> GEFilterParams::Unbox(const std::shared_ptr<GEFilterParams>& params)
{
    static_assert(!std::is_reference_v<T>, "Can't unbox as a reference");
    using ParamType = typename GEFilterParamsWrapper<T>::ParamType;
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<ParamType>, "Unbox an unregistered GEFilterParam type");
    if (GEFilterParamsTypeInfo<ParamType>::ID == params->id) {
        return std::static_pointer_cast<GEFilterParamsWrapper<T>>(params)->data;
    }
    return GEFilterParamsWrapper<T>::Null;
}

// Upcast from specific params type to type-erasured GEFilterParams
template<typename T>
std::shared_ptr<GEFilterParams> GEFilterParams::Box(T&& params)
{
    // As the type parameter of an universal reference, T may be U&/const U&, so removing qualifiers is necessary
    using TValue = std::remove_cv_t<std::remove_reference_t<T>>;
    using ParamType = typename GEFilterParamsWrapper<TValue>::ParamType;
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<ParamType>, "Unbox an unregistered GEFilterParam type");
    auto p = std::make_shared<GEFilterParamsWrapper<TValue>>(std::forward<T>(params));
    return std::static_pointer_cast<GEFilterParams>(p);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif