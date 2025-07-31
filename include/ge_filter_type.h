/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class GEFilterType {
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
    static constexpr GEFilterType id = GEFilterType::NONE;
    using param_type = void;
};

template<typename T>
struct GEFilterParamsTypeInfo {
    using type = void;
};

#define REGISTER_GEFILTER_TYPEINFO(EnumValue, ActualType, ParamType) \
    class ActualType; \
    template<> struct ::OHOS::Rosen::Drawing::GEFilterTypeInfo<ActualType> { \
        static constexpr GEFilterType id = GEFilterType::EnumValue; \
        using param_type = ParamType; \
    }; \
    template<> struct ::OHOS::Rosen::Drawing::GEFilterParamsTypeInfo<ParamType> { \
        using type = ActualType; \
    }; 

#define DECLARE_GEFILTER_TYPEFUNC(Self) \
    ::OHOS::Rosen::Drawing::GEFilterType Type() const override { \
        return ::OHOS::Rosen::Drawing::GEFilterTypeInfo<Self>::id; \
    }

class GEFilterParams {
public:
    template<typename T>
    static std::optional<T> Unbox(const std::shared_ptr<GEFilterParams>& params);

    template<typename T>
    static std::shared_ptr<GEFilterParams> Box(T&& params);

    GEFilterParams(GEFilterType type) : id(type) {}
protected:
    GEFilterType id;
};

template <typename T>
class GEFilterParamsWrapper: public GEFilterParams {
public:
    static_assert(!std::is_void_v<typename GEFilterParamsTypeInfo<T>::type>, "Unregistered GEFilter type");
    GEFilterParamsWrapper(T&& params) :
        GEFilterParams(GEFilterTypeInfo<typename GEFilterParamsTypeInfo<T>::type>::id), data(std::move(params)) {}

    T data;
};

template<typename T>
std::optional<T> GEFilterParams::Unbox(const std::shared_ptr<GEFilterParams>& params)
{
    static_assert(!std::is_void_v<typename GEFilterTypeInfo<T>::param_type>, "Unbox an unregistered GEFilter type");
    return GEFilterTypeInfo<T>::id == params->id ? 
        std::static_pointer_cast<GEFilterParamsWrapper<typename GEFilterTypeInfo<T>::param_type>>(params)->data 
        : std::nullopt;
}

template<typename T>
std::shared_ptr<GEFilterParams> GEFilterParams::Box(T&& params)
{
    static_assert(!std::is_void_v<typename GEFilterParamsTypeInfo<T>::type>, "Box a unregistered GEFilter type");
    auto p = std::make_shared<GEFilterParamsWrapper<T>>(std::move(params));
    return std::static_pointer_cast<GEFilterParams>(p);
}

}
}
}

#endif // GRAPHICS_EFFECT_GE_FILTER_TYPE_H