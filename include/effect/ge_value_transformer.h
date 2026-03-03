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

#include "effect/ge_params_reflection_v2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// ============================================================================
// Default Value Transformer (pass-through)
// ============================================================================

template<GEParamsMemberTag Tag, typename T>
struct GEParamsValueTransformer {
    static T Transform(T value) { return value; }
};

// ============================================================================
// Float Clamp Specializations
// ============================================================================

// Progress values [0.0, 1.0]
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::GASIFY_SCALE_TWIST_PROGRESS, float> {
    static float Transform(float value) { return std::clamp(value, 0.0f, 1.0f); }
};

template<>
struct GEParamsValueTransformer<GEParamsMemberTag::GASIFY_BLUR_PROGRESS, float> {
    static float Transform(float value) { return std::clamp(value, 0.0f, 1.0f); }
};

template<>
struct GEParamsValueTransformer<GEParamsMemberTag::GASIFY_PROGRESS, float> {
    static float Transform(float value) { return std::clamp(value, 0.0f, 1.0f); }
};

// Particle circular halo radius [0.001, 10.0]
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::PARTICLE_CIRCULAR_HALO_RADIUS, float> {
    static float Transform(float value) { return std::clamp(value, 0.001f, 10.0f); }
};

// SDF edge light spread factor
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::SDF_EDGE_LIGHT_SDF_SPREAD_FACTOR, float> {
    static float Transform(float value)
    {
        constexpr float MIN_SDF_SPREAD_FACTOR = 0.0f;
        constexpr float MAX_SDF_SPREAD_FACTOR = 1.0f;
        return std::clamp(value, MIN_SDF_SPREAD_FACTOR, MAX_SDF_SPREAD_FACTOR);
    }
};

// ============================================================================
// Float Max (non-negative) Specializations
// ============================================================================

template<>
struct GEParamsValueTransformer<GEParamsMemberTag::PARTICLE_CIRCULAR_HALO_NOISE, float> {
    static float Transform(float value) { return std::max(value, 0.0f); }
};

// ============================================================================
// Pair<float, float> Component-wise Transformations
// ============================================================================

// Particle circular halo center - each component clamped to [0.0, 1.0]
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::PARTICLE_CIRCULAR_HALO_CENTER, std::pair<float, float>> {
    static std::pair<float, float> Transform(const std::pair<float, float>& value)
    {
        return {
            std::clamp(value.first, 0.0f, 1.0f),
            std::clamp(value.second, 0.0f, 1.0f)
        };
    }
};

// Gasify scale twist scale - each component must be non-negative
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::GASIFY_SCALE_TWIST_SCALE, std::pair<float, float>> {
    static std::pair<float, float> Transform(const std::pair<float, float>& value)
    {
        return {
            std::max(value.first, 0.0f),
            std::max(value.second, 0.0f)
        };
    }
};

// ============================================================================
// Vector2f Component-wise Transformations
// ============================================================================

// Frosted glass corner radius - non-negative
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::FROSTED_GLASS_CORNER_RADIUS, Vector2f> {
    static Vector2f Transform(const Vector2f& value)
    {
        constexpr float V_MIN = 0.0f;
        return Vector2f(std::max(value.x, V_MIN), std::max(value.y, V_MIN));
    }
};

// Frosted glass dark scale [0.0, 1.0]
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::FROSTED_GLASS_DARK_SCALE, Vector2f> {
    static Vector2f Transform(const Vector2f& value)
    {
        constexpr float MIN_S = 0.0f;
        constexpr float MAX_S = 1.0f;
        return Vector2f(std::clamp(value.x, MIN_S, MAX_S), std::clamp(value.y, MIN_S, MAX_S));
    }
};

// ============================================================================
// Vector3f Component-wise Transformations
// ============================================================================

// Frosted glass KBS (kernel, blur, strength) parameters
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::FROSTED_GLASS_BG_KBS, Vector3f> {
    static Vector3f Transform(const Vector3f& value)
    {
        constexpr float KB_MIN = 0.0f;
        constexpr float KB_MAX = 10.0f;
        constexpr float S_MIN = -1.0f;
        constexpr float S_MAX = 1.0f;
        return Vector3f(
            std::clamp(value.x, KB_MIN, KB_MAX),
            std::clamp(value.y, KB_MIN, KB_MAX),
            std::clamp(value.z, S_MIN, S_MAX)
        );
    }
};

// Frosted glass position parameters [0.0, 1.0]
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::FROSTED_GLASS_BG_POS, Vector3f> {
    static Vector3f Transform(const Vector3f& value)
    {
        constexpr float V_MIN = 0.0f;
        constexpr float V_MAX = 1.0f;
        return Vector3f(
            std::clamp(value.x, V_MIN, V_MAX),
            std::clamp(value.y, V_MIN, V_MAX),
            std::clamp(value.z, V_MIN, V_MAX)
        );
    }
};

// ============================================================================
// Vector4f Component-wise Transformations
// ============================================================================

// Material color [0.0, 1.0] for RGBA
template<>
struct GEParamsValueTransformer<GEParamsMemberTag::FROSTED_GLASS_MATERIAL_COLOR, Vector4f> {
    static Vector4f Transform(const Vector4f& value)
    {
        constexpr float MIN_C = 0.0f;
        constexpr float MAX_C = 1.0f;
        return Vector4f(
            std::clamp(value.x, MIN_C, MAX_C),
            std::clamp(value.y, MIN_C, MAX_C),
            std::clamp(value.z, MIN_C, MAX_C),
            std::clamp(value.w, MIN_C, MAX_C)
        );
    }
};

// ============================================================================
// How to add new value transformers:
// ============================================================================
//
// To add a new transformation, specialize GEParamsValueTransformer for your
// specific tag and type:
//
// template<>
// struct GEParamsValueTransformer<GEParamsMemberTag::YOUR_TAG, float> {
//     static float Transform(float value) {
//         // Your transformation logic here
//         return std::clamp(value, min_val, max_val);
//     }
// };
//
// The transformer is automatically applied during SetParam via the
// generated dispatch code in ge_type_dispatch_gen.h
//
// ============================================================================

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_H
