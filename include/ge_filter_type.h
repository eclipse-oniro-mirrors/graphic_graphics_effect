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
#ifndef GRAPHICS_EFFECT_GE_FILTER_TYPE_H
#define GRAPHICS_EFFECT_GE_FILTER_TYPE_H
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class GEFilterType : int32_t {
    NONE,
    KAWASE_BLUR,
    MESA_BLUR,
    GREY,
    AIBAR,
    LINEAR_GRADIENT_BLUR,
    MAGNIFIER,
    WATER_RIPPLE,
    DISPLACEMENT_DISTORT_FILTER,
    COLOR_GRADIENT,
    MASK_TRANSITION,
    SDF,
    // Mask
    RIPPLE_MASK,
    DOUBLE_RIPPLE_MASK,
    PIXEL_MAP_MASK,
    RADIAL_GRADIENT_MASK,
    WAVE_GRADIENT_MASK,
    FRAME_GRADIENT_MASK,
    LINEAR_GRADIENT_MASK,
    SDF_UNION_OP,
    SDF_RRECT_MASK,
    // Shader
    DOT_MATRIX,
    FLOW_LIGHT_SWEEP,
    COMPLEX_SHADER,
    SOUND_WAVE,
    EDGE_LIGHT,
    BEZIER_WARP,
    DISPERSION,
    CONTENT_LIGHT,
    BORDER_LIGHT,
    CONTOUR_DIAGONAL_FLOW_LIGHT,
    WAVY_RIPPLE_LIGHT,
    AURORA_NOISE,
    PARTICLE_CIRCULAR_HALO,
    DIRECTION_LIGHT,
    VARIABLE_RADIUS_BLUR,
    COLOR_GRADIENT_EFFECT,
    LIGHT_CAVE,
    AIBAR_GLOW,
    ROUNDED_RECT_FLOWLIGHT,
    GRADIENT_FLOW_COLORS,
    MAX
};

template<typename T>
struct GEFilterTypeInfo {
    static constexpr GEFilterType ID = GEFilterType::NONE;
    static constexpr std::string_view Name = "";
    using ParamType = void;
};

template<typename T>
struct GEFilterParamsTypeInfo {
    static constexpr GEFilterType ID = GEFilterType::NONE;
    using FilterType = void;
};

// Register type info for certain GE Filter type (Non-instructive)
#define REGISTER_GEFILTER_TYPEINFO(ENUM_VALUE, FILTER_TYPE, PARAM_TYPE, TYPE_NAME)           \
    class FILTER_TYPE;                                                                       \
    template<>                                                                               \
    struct ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FILTER_TYPE> {                           \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        static constexpr std::string_view Name = TYPE_NAME;                                  \
        using ParamType = PARAM_TYPE;                                                        \
    };                                                                                       \
    template<>                                                                               \
    struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<PARAM_TYPE> {                      \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        using FilterType = FILTER_TYPE;                                                      \
    }

// Register type info for certain GE Filter Param type without Filter type (Non-instructive)
// Used on extension filter only when its param type is known and possibly used in GEVisualEffectImpl
#define REGISTER_GEFILTERPARAM_TYPEINFO(ENUM_VALUE, PARAM_TYPE)                              \
    template<>                                                                               \
    struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<PARAM_TYPE> {                      \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        using FilterType = void;                                                             \
    }

// Virtual interface for runtime type identification of registered GEFilter types and type-erasured class GEFilterParams
// Currently, you SHOULD NOT add member varaibles or any stateful data members to this class.
struct IGEFilterType {
    // Safeguard for type-safety, but generally you should not cast derived into IGEFilterType
    // since this interface acts only as a contract/trait for type queries
    virtual ~IGEFilterType() = default;

    // For runtime-type identification
    virtual GEFilterType Type() const
    {
        return Drawing::GEFilterType::NONE;
    }

    // Readonly type string for tracing/logging only, NOT INTENTED for serialization
    virtual const std::string_view TypeName() const
    {
        return "IGEFilterType";
    }
};

// Compile time static check to ensure DECLARE_GEFILTER_TYPEFUNC is used correctly
template<typename T>
struct GEFilterTypeInfoStaticCheck {
    using ParamType = typename GEFilterTypeInfo<T>::ParamType;
    static constexpr bool IsGEFilterType = std::is_base_of_v<IGEFilterType, T>;
    static_assert(IsGEFilterType, "T must implement IGEFilterType");

    static_assert(!std::is_void_v<typename GEFilterTypeInfo<T>::ParamType>, "Unregistered GEFilter type");
    static_assert(
        !std::is_void_v<typename GEFilterParamsTypeInfo<ParamType>::FilterType>, "Unregistered GEFilterParams type");
    static_assert(GEFilterTypeInfo<T>::ID == GEFilterParamsTypeInfo<ParamType>::ID, "Inconsistent type id");
};

// Declare the Type and TypeName functions for a given GEFilter class
// Macro hint: Self parameter is techincally not required since Self can be deduced from *this. Current implementation
// requires Self to be manually specified to prevent hard-to-read compile errors when the macro is wrongly used.
#define DECLARE_GEFILTER_TYPEFUNC(Self)                                                                 \
    ::OHOS::Rosen::Drawing::GEFilterType Type() const override                                          \
    {                                                                                                   \
        static_assert(std::is_same_v<Self, std::remove_cv_t<std::remove_reference_t<decltype(*this)>>>, \
            "DECLARE_GEFILTER_TYPEFUNC: Macro is used outside class scope / Incorrect Self type");      \
        static_assert(::OHOS::Rosen::Drawing::GEFilterTypeInfoStaticCheck<Self>::IsGEFilterType);       \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<Self>::ID;                                      \
    }                                                                                                   \
    const std::string_view TypeName() const override                                                    \
    {                                                                                                   \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<Self>::Name;                                    \
    }

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FILTER_TYPE_H