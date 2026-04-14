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

#ifndef GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_TRAITS_H
#define GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_TRAITS_H

#include <memory>
#include <type_traits>

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace ValueTransformerTypeTraits {
// Helper trait: detect std::shared_ptr
template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

// Helper trait: detect if Transform(const FromType&, ToType&) method exists
template<typename, typename, typename, typename = void>
struct has_transform_method_const_ref : std::false_type {};

template<typename Transformer, typename FromType, typename ToType>
struct has_transform_method_const_ref<Transformer, FromType, ToType,
    std::void_t<decltype(std::declval<Transformer>().Transform(
        std::declval<const FromType&>(), std::declval<ToType&>()))>> : std::true_type {};

// Helper trait: detect if Transform(FromType, ToType&) method exists
template<typename, typename, typename, typename = void>
struct has_transform_method_value : std::false_type {};

template<typename Transformer, typename FromType, typename ToType>
struct has_transform_method_value<Transformer, FromType, ToType,
    std::void_t<decltype(std::declval<Transformer>().Transform(std::declval<FromType>(), std::declval<ToType&>()))>>
    : std::true_type {};

// Helper trait: detect if Transform method exists with either signature
template<typename Transformer, typename FromType, typename ToType>
struct has_transform_method : std::bool_constant<has_transform_method_const_ref<Transformer, FromType, ToType>::value ||
                                                 has_transform_method_value<Transformer, FromType, ToType>::value> {};

template<typename Transformer, typename FromType, typename ToType>
inline constexpr bool has_transform_method_v = has_transform_method<Transformer, FromType, ToType>::value;
} // namespace ValueTransformerTypeTraits

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_TRAITS_H