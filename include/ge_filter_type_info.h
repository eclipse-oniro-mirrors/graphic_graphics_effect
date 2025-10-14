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
#ifndef GRAPHICS_EFFECT_GE_FILTER_TYPE_INFO_H
#define GRAPHICS_EFFECT_GE_FILTER_TYPE_INFO_H
#include "ge_filter_type.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
template<typename T, typename = void>
struct GEFilterTypeInfo {
    using ParamType = void;
    static constexpr GEFilterType ID = GEFilterType::NONE;
    static constexpr std::string_view Name = "";
};

template<typename T>
struct GEFilterTypeInfo<T, std::void_t<typename T::ParamType>> {
    using ParamType = typename T::ParamType;
    static constexpr GEFilterType ID = GEFilterParamsTypeInfo<ParamType>::ID;
    static constexpr std::string_view Name = GEFilterParamsTypeInfo<ParamType>::FilterName;
};

// Compile time static check to ensure DECLARE_GEFILTER_TYPEFUNC is used correctly
template<typename T>
struct GEFilterTypeInfoStaticCheck {
    using ParamType = typename GEFilterTypeInfo<T>::ParamType;
    static constexpr bool IsGEFilterType = std::is_base_of_v<IGEFilterType, T>;
    static_assert(IsGEFilterType, "T must implement IGEFilterType");

    static_assert(!std::is_void_v<typename GEFilterTypeInfo<T>::ParamType>, "Unregistered GEFilter type");
    static_assert(GEFilterTypeInfo<T>::ID == GEFilterParamsTypeInfo<ParamType>::ID, "Inconsistent type id");
};

// Declare the ParamType alias, Type and TypeName functions for a given GEFilter class
#define DECLARE_GEFILTER_TYPEFUNC(SELF, PARAM_TYPE)                                                     \
    using ParamType = PARAM_TYPE;                                                                       \
    ::OHOS::Rosen::Drawing::GEFilterType Type() const override                                          \
    {                                                                                                   \
        static_assert(std::is_same_v<SELF, std::remove_cv_t<std::remove_reference_t<decltype(*this)>>>, \
            "DECLARE_GEFILTER_TYPEFUNC: Macro is used outside class scope / Incorrect Self type");      \
        static_assert(::OHOS::Rosen::Drawing::GEFilterTypeInfoStaticCheck<SELF>::IsGEFilterType);       \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<SELF>::ID;                                      \
    }                                                                                                   \
    const std::string_view TypeName() const override                                                    \
    {                                                                                                   \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<SELF>::Name;                                    \
    }

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_FILTER_TYPE_INFO_H