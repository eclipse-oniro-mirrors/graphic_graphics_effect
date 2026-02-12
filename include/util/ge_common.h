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

#ifndef GRAPHICS_EFFECT_COMMON_H
#define GRAPHICS_EFFECT_COMMON_H
#include <cmath>
#include <memory>

namespace OHOS {
namespace Rosen {

#ifdef _WIN32
#define GE_EXPORT __attribute__((dllexport))
#define GE_IMPORT __attribute__((dllimport))
#else
#define GE_EXPORT __attribute__((visibility("default")))
#define GE_IMPORT __attribute__((visibility("default")))
#endif

template<typename T>
inline constexpr bool GE_EQ(const T& x, const T& y)
{
    if constexpr (std::is_floating_point<T>::value) {
        return (std::abs((x) - (y)) <= (std::numeric_limits<T>::epsilon()));
    } else {
        return x == y;
    }
}

template<typename T>
inline bool GE_EQ(T x, T y, T epsilon)
{
    return (std::abs((x) - (y)) <= (epsilon));
}

template<typename T>
inline bool GE_EQ(const std::weak_ptr<T>& x, const std::weak_ptr<T>& y)
{
    return !(x.owner_before(y) || y.owner_before(x));
}

template<typename T>
inline constexpr bool GE_NE(const T& x, const T& y)
{
    return !GE_EQ(x, y);
}

inline bool GE_LNE(float left, float right) // less not equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) < epsilon;
}

inline bool GE_GNE(float left, float right) // great not equal
{
    constexpr float epsilon = 0.001f;
    return (left - right) > epsilon;
}

inline bool GE_GE(float left, float right) // great or equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) > epsilon;
}

inline bool GE_LE(float left, float right) // less or equal
{
    constexpr float epsilon = 0.001f;
    return (left - right) < epsilon;
}

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_COMMON_H
