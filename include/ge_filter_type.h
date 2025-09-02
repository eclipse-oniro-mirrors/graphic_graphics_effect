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
#include <type_traits>
#include <string_view>

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
    // Mask
    RIPPLE_MASK,
    DOUBLE_RIPPLE_MASK,
    PIXEL_MAP_MASK,
    RADIAL_GRADIENT_MASK,
    WAVE_GRADIENT_MASK,
    FRAME_GRADIENT_MASK,
    LINEAR_GRADIENT_MASK,
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
#define REGISTER_GEFILTER_TYPEINFO(ENUM_VALUE, FILTER_TYPE, PARAM_TYPE, TYPE_NAME) \
    class FILTER_TYPE; \
    template<> struct ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FILTER_TYPE> { \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        static constexpr std::string_view Name = TYPE_NAME; \
        using ParamType = PARAM_TYPE; \
    }; \
    template<> struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<PARAM_TYPE> { \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        using FilterType = FILTER_TYPE; \
    }

// Register type info for certain GE Filter Param type without Filter type (Non-instructive)
// Used on extension filter only when its param type is known and possibly used in GEVisualEffectImpl
#define REGISTER_GEFILTERPARAM_TYPEINFO(ENUM_VALUE, PARAM_TYPE) \
    template<> struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<PARAM_TYPE> { \
        static constexpr GEFilterType ID = ::OHOS::Rosen::Drawing::GEFilterType::ENUM_VALUE; \
        using FilterType = void; \
    }

// Type-erasured params class
class GEFilterParams {
public:
    template<typename T>
    struct OptionalTypeTrait { using Type = std::optional<T>; };
    template<typename T>
    struct OptionalTypeTrait<std::shared_ptr<T>> { using Type = std::shared_ptr<T>; };
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
    GEFilterParamsWrapper(U&& params) :
        GEFilterParams(GEFilterParamsTypeInfo<ParamType>::ID),
        data(std::forward<U>(params)) 
    {
    }

    T data;
};

// Type-specific proxy wrapper of params, specialization of std::shared_ptr<T> for handy use
template<typename T>
class GEFilterParamsWrapper<std::shared_ptr<T>>: public GEFilterParams {
public:
    using ParamType = T;
    using FilterType = typename GEFilterParamsTypeInfo<T>::FilterType;
    static constexpr std::nullptr_t Null = nullptr;
    static_assert(std::is_void_v<FilterType> || GEFilterParams::IsRegisteredFilterTypeInfo<FilterType>,
                  "FilterType wrongly registered");
    static_assert(GEFilterParams::IsRegisteredParamTypeInfo<ParamType>, "Unregistered GEFilterParams type");
    GEFilterParamsWrapper(const std::shared_ptr<T>& params) :
        GEFilterParams(GEFilterParamsTypeInfo<ParamType>::ID),
        data(params)
    {
    }

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
    
    // Convert unpacked params back into original params type, used in GEFilterComposer
    virtual std::shared_ptr<GEFilterParams> Params() const
    {
        return nullptr;
    }
};

// Compile time static check to ensure DECLARE_GEFILTER_TYPEFUNC is used correctly
template<typename T>
struct GEFilterTypeInfoStaticCheck {
    using ParamType = typename GEFilterTypeInfo<T>::ParamType;
    static constexpr bool IsGEFilterType = std::is_base_of_v<IGEFilterType, T>;
    static_assert(IsGEFilterType, "T must implement IGEFilterType");

    static_assert(!std::is_void_v<typename GEFilterTypeInfo<T>::ParamType>, "Unregistered GEFilter type");
    static_assert(std::is_same_v<typename GEFilterParamsWrapper<ParamType>::ParamType, ParamType>);
    static_assert(std::is_same_v<typename GEFilterParamsWrapper<ParamType>::FilterType, T>);
    static_assert(std::is_same_v<typename GEFilterParamsWrapper<std::shared_ptr<ParamType>>::ParamType, ParamType>);
    static_assert(std::is_same_v<typename GEFilterParamsWrapper<std::shared_ptr<ParamType>>::FilterType, T>);
    using UnboxValueType = decltype(GEFilterParams::Unbox<ParamType>(std::shared_ptr<GEFilterParams>()));
    using UnboxPtrType = decltype(GEFilterParams::Unbox<std::shared_ptr<ParamType>>(std::shared_ptr<GEFilterParams>()));
    static_assert(std::is_same_v<UnboxValueType, std::optional<ParamType>>);
    static_assert(std::is_same_v<UnboxPtrType, std::shared_ptr<ParamType>>);
};

// Declare the Type and TypeName functions for a given GEFilter class
// Notice: Self parameter is techincally not required since Self can be deduced from *this. Current implementation
// requires Self to be manually specified to prevent hard-to-read compile errors when the macro is wrongly used.
#define DECLARE_GEFILTER_TYPEFUNC(Self) \
    ::OHOS::Rosen::Drawing::GEFilterType Type() const override \
    { \
        static_assert(std::is_same_v<Self, std::remove_cv_t<std::remove_reference_t<decltype(*this)>>>, \
            "DECLARE_GEFILTER_TYPEFUNC: Macro is used outside class scope / Incorrect Self type"); \
        static_assert(::OHOS::Rosen::Drawing::GEFilterTypeInfoStaticCheck<Self>::IsGEFilterType); \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<Self>::ID; \
    } \
    const std::string_view TypeName() const override \
    { \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<Self>::Name; \
    }

}
}
}

#endif // GRAPHICS_EFFECT_GE_FILTER_TYPE_H