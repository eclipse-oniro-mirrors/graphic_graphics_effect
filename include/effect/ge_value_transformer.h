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

#ifndef GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_H
#define GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "ge_effects_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Forward declarations
enum class GEParamsMemberTag : uint32_t;

// Base constraint templates for Min, Max, and Convert components
// Base template for Min constraint
template<GEParamsMemberTag, typename = void>
struct GEParamsConstraintMinInfo {
    static constexpr bool HAS_MIN = false;
    static constexpr bool COMPONENT_WISE = false;
    static constexpr size_t COMPONENT_COUNT = 0;
};

// Base template for Max constraint
template<GEParamsMemberTag, typename = void>
struct GEParamsConstraintMaxInfo {
    static constexpr bool HAS_MAX = false;
    static constexpr bool COMPONENT_WISE = false;
    static constexpr size_t COMPONENT_COUNT = 0;
};

// Base template for Convert constraint
template<GEParamsMemberTag, typename = void>
struct GEParamsConstraintConvertInfo {
    static constexpr bool HAS_CONVERT = false;
    static constexpr bool HAS_CAST_FROM = false;
    using CastFromType = void;
    static constexpr bool HAS_CUSTOM = false;
    using CustomTransformer = void;
};

// Main constraint info template that combines Min, Max, and Convert info
// Members (MIN, MAX, MIN_COMPONENTS, MAX_COMPONENTS) are inherited from specialized templates
// Access to these members should be guarded by if constexpr checks on HAS_* flags
template<GEParamsMemberTag Tag>
struct GEParamsConstraintInfo : GEParamsConstraintMinInfo<Tag>,
                                GEParamsConstraintMaxInfo<Tag>,
                                GEParamsConstraintConvertInfo<Tag> {
    using CastFromType = typename GEParamsConstraintConvertInfo<Tag>::CastFromType;
    using CustomTransformer = typename GEParamsConstraintConvertInfo<Tag>::CustomTransformer;

    // Range flag derived from Min and Max
    static constexpr bool HAS_RANGE =
        GEParamsConstraintMinInfo<Tag>::HAS_MIN || GEParamsConstraintMaxInfo<Tag>::HAS_MAX;
    static constexpr bool COMPONENT_WISE =
        GEParamsConstraintMinInfo<Tag>::COMPONENT_WISE || GEParamsConstraintMaxInfo<Tag>::COMPONENT_WISE;

    // Other members are inherited from specialized templates and should be accessed
    // only after checking the corresponding HAS_* flag with if constexpr
};

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

// Base ValueTransformer template with new prototype
template<GEParamsMemberTag Tag, typename FromType, typename ToType>
struct GEParamsValueTransformer {
    static bool TransformCustomConvert(const FromType& value, ToType& out)
    {
        using namespace ValueTransformerTypeTraits;
        using Constraint = GEParamsConstraintInfo<Tag>;
        using Transformer = typename Constraint::CustomTransformer;
        // When FromType == ToType, check if Transform is implemented
        // If not implemented or types match, fallback to direct assignment
        // Required for mapping multiple SetParams entries to single member
        if constexpr (std::is_same_v<FromType, ToType>) {
            if constexpr (has_transform_method_v<Transformer, FromType, ToType>) {
                if (!Transformer::Transform(value, out)) {
                    return false;
                }
            } else {
                // Fallback: direct assignment when Transform not implemented
                out = value;
            }
        } else {
            // FromType != ToType: always call Transform
            if (!Transformer::Transform(value, out)) {
                return false;
            }
        }
        return true;
    }

    static bool Transform(const FromType& value, ToType& out)
    {
        using namespace ValueTransformerTypeTraits;
        using Constraint = GEParamsConstraintInfo<Tag>;

        // Apply conversion (custom or default)
        if constexpr (Constraint::HAS_CONVERT) {
            if constexpr (Constraint::HAS_CUSTOM) {
                if (!TransformCustomConvert(value, out)) {
                    return false;
                }
            } else if constexpr (std::is_same_v<FromType, ToType>) {
                // No custom transformer and types match: direct assignment
                out = value;
            } else if constexpr (is_shared_ptr_v<FromType> && is_shared_ptr_v<ToType>) {
                // Pointer type: use std::static_pointer_cast
                out = std::static_pointer_cast<typename ToType::element_type>(value);
            } else {
                // Value type: use static_cast
                out = static_cast<ToType>(value);
            }
        } else if constexpr (std::is_same_v<FromType, ToType>) {
            out = value;
        } else {
            static_assert(sizeof(FromType) == 0, "Unsupported conversion from FromType to ToType");
            return false;
        }

        // Apply range constraints
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    out = ApplyComponentWiseClamp(out, Constraint::MIN_COMPONENTS, Constraint::MAX_COMPONENTS);
                } else if constexpr (Constraint::HAS_MIN) {
                    out = ApplyComponentWiseMin(out, Constraint::MIN_COMPONENTS);
                } else if constexpr (Constraint::HAS_MAX) {
                    out = ApplyComponentWiseMax(out, Constraint::MAX_COMPONENTS);
                }
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    out = std::clamp(out, Constraint::MIN, Constraint::MAX);
                } else if constexpr (Constraint::HAS_MIN) {
                    out = std::max(out, Constraint::MIN);
                } else if constexpr (Constraint::HAS_MAX) {
                    out = std::min(out, Constraint::MAX);
                }
            }
        }
        return true;
    }

private:
    // Helper function: component-wise clamp for std::pair<float, float>
    static std::pair<float, float> ApplyComponentWiseClamp(
        const std::pair<float, float>& value, const float (&min)[2], const float (&max)[2])
    {
        return std::pair<float, float>(
            std::clamp(value.first, min[0], max[0]), std::clamp(value.second, min[1], max[1]));
    }

    // Helper function: component-wise clamp for Vector2f
    static Vector2f ApplyComponentWiseClamp(const Vector2f& value, const float (&min)[2], const float (&max)[2])
    {
        return Vector2f(std::clamp(value.x_, min[0], max[0]), std::clamp(value.y_, min[1], max[1]));
    }

    // Helper function: component-wise clamp for Vector3f
    static Vector3f ApplyComponentWiseClamp(const Vector3f& value, const float (&min)[3], const float (&max)[3])
    {
        return Vector3f(std::clamp(value.x_, min[0], max[0]), std::clamp(value.y_, min[1], max[1]),
            std::clamp(value.z_, min[2], max[2]));
    }

    // Helper function: component-wise clamp for Vector4f
    static Vector4f ApplyComponentWiseClamp(const Vector4f& value, const float (&min)[4], const float (&max)[4])
    {
        return Vector4f(std::clamp(value.x_, min[0], max[0]), std::clamp(value.y_, min[1], max[1]),
            std::clamp(value.z_, min[2], max[2]), std::clamp(value.w_, min[3], max[3]));
    }

    // Helper function: component-wise min
    template<typename T>
    static T ApplyComponentWiseMin(const T& value, const float (&min)[4])
    {
        if constexpr (std::is_same_v<T, Vector2f>) {
            return Vector2f(std::max(value.x_, min[0]), std::max(value.y_, min[1]));
        } else if constexpr (std::is_same_v<T, Vector3f>) {
            return Vector3f(std::max(value.x_, min[0]), std::max(value.y_, min[1]), std::max(value.z_, min[2]));
        } else if constexpr (std::is_same_v<T, Vector4f>) {
            return Vector4f(std::max(value.x_, min[0]), std::max(value.y_, min[1]), std::max(value.z_, min[2]),
                std::max(value.w_, min[3]));
        } else {
            return value;
        }
    }

    // Helper function: component-wise min for std::pair<float, float>
    static std::pair<float, float> ApplyComponentWiseMin(const std::pair<float, float>& value, const float (&min)[2])
    {
        return std::make_pair(std::max(value.first, min[0]), std::max(value.second, min[1]));
    }

    // Helper function: component-wise max
    template<typename T>
    static T ApplyComponentWiseMax(const T& value, const float (&max)[4])
    {
        if constexpr (std::is_same_v<T, Vector2f>) {
            return Vector2f(std::min(value.x_, max[0]), std::min(value.y_, max[1]));
        } else if constexpr (std::is_same_v<T, Vector3f>) {
            return Vector3f(std::min(value.x_, max[0]), std::min(value.y_, max[1]), std::min(value.z_, max[2]));
        } else if constexpr (std::is_same_v<T, Vector4f>) {
            return Vector4f(std::min(value.x_, max[0]), std::min(value.y_, max[1]), std::min(value.z_, max[2]),
                std::min(value.w_, max[3]));
        } else {
            return value;
        }
    }

    // Helper function: component-wise max for std::pair<float, float>
    static std::pair<float, float> ApplyComponentWiseMax(const std::pair<float, float>& value, const float (&max)[2])
    {
        return std::make_pair(std::min(value.first, max[0]), std::min(value.second, max[1]));
    }
};

// Custom Transformer: Vector4f to Color4f
struct Vector4fToColor4fTransformer {
    static bool Transform(const Vector4f& value, Color4f& out)
    {
        out.redF_ = value.x_;
        out.greenF_ = value.y_;
        out.blueF_ = value.z_;
        out.alphaF_ = value.w_;
        return true;
    }
};

// Custom Transformer: Vector4f to RectF
struct Vector4fToRectFTransformer {
    static bool Transform(const Vector4f& value, RectF& out)
    {
        out = RectF(value.x_, value.y_, value.z_, value.w_);
        return true;
    }
};

// Custom Transformer: Pair to Vector2f
struct PairToVector2fTransformer {
    static bool Transform(const std::pair<float, float>& value, Vector2f& out)
    {
        out = Vector2f(value.first, value.second);
        return true;
    }
};

// Custom Transformer: std::pair<float, float> to Drawing::Point
struct PairToPointTransformer {
    static bool Transform(const std::pair<float, float>& value, Drawing::Point& out)
    {
        out = Drawing::Point(value.first, value.second);
        return true;
    }
};

// Custom Transformer: Clamp with fallback for FrostedGlass edLightDir
struct ClampFallbackTransformer {
    static bool Transform(const std::pair<float, float>& value, Vector2f& out)
    {
        constexpr float MIN_V = -1.0f;
        constexpr float MAX_V = 1.0f;
        constexpr float EPS = 1e-6f;

        float dx = std::clamp(value.first, MIN_V, MAX_V);
        float dy = std::clamp(value.second, MIN_V, MAX_V);
        if (std::abs(dx) < EPS && std::abs(dy) < EPS) {
            dx = 1.0f;
            dy = 0.0f;
        }
        out = Vector2f(dx, dy);
        return true;
    }
};

// Custom Transformer: DotMatrixEffectType with switch-case fallback
struct DotMatrixEffectTypeTransformer {
    static bool Transform(int32_t value, DotMatrixEffectType& out)
    {
        enum class EffectTypeParam { NONE = 0, ROTATE, RIPPLE };
        EffectTypeParam effectTypeParam = static_cast<EffectTypeParam>(value);
        switch (effectTypeParam) {
            case EffectTypeParam::NONE:
                out = DotMatrixEffectType::NONE;
                return true;
            case EffectTypeParam::ROTATE:
                out = DotMatrixEffectType::ROTATE;
                return true;
            case EffectTypeParam::RIPPLE:
                out = DotMatrixEffectType::RIPPLE;
                return true;
            default:
                out = DotMatrixEffectType::NONE;
                return true;
        }
    }
};

// Custom Transformer: SDFUnionOp with boundary check
struct SDFUnionOpTransformer {
    static bool Transform(uint32_t value, GESDFUnionOp& out)
    {
        if (value >= static_cast<uint32_t>(GESDFUnionOp::MAX)) {
            // Invalid value, return false (don't set)
            return false;
        }
        out = static_cast<GESDFUnionOp>(value);
        return true;
    }
};

// Custom Transformer: DistortionCollapse has a special pair -> Vector4f convert
struct DistortionCollapsePairToVector4fTransformer {
    static bool Transform(const std::pair<float, float>& value, Vector4f& out)
    {
        out = Vector4f(value.first, value.first, value.second, value.second);
        return true;
    }
};

// Custom Transformer: std::shared_ptr<Drawing::Image> to std::weak_ptr<Drawing::Image>
struct SharedPtrImageToWeakTransformer {
    static bool Transform(const std::shared_ptr<Drawing::Image>& value, std::weak_ptr<Drawing::Image>& out)
    {
        out = value;
        return true;
    }
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_H
