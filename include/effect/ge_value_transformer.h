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
#include <cstdint>

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace GEV2 {
// Forward declarations
enum class GEParamsMemberTag : uint32_t;

template<GEParamsMemberTag Tag>
struct GEParamsConstraintInfo {
    static constexpr bool HAS_RANGE = false;
    static constexpr bool COMPONENT_WISE = false;
    static constexpr bool HAS_MIN = false;
    static constexpr bool HAS_MAX = false;
};

template<GEParamsMemberTag Tag, typename T>
struct GEParamsValueTransformer {
    static T Transform(T value) { return value; }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, float> {
    static float Transform(float value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                return std::clamp(value, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]);
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return std::clamp(value, Constraint::MIN, Constraint::MAX);
                } else if constexpr (Constraint::HAS_MIN) {
                    return std::max(value, Constraint::MIN);
                } else if constexpr (Constraint::HAS_MAX) {
                    return std::min(value, Constraint::MAX);
                }
            }
        }
        return value;
    }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, int> {
    static int Transform(int value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                return std::clamp(value, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]);
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return std::clamp(value, Constraint::MIN, Constraint::MAX);
                } else if constexpr (Constraint::HAS_MIN) {
                    return std::max(value, Constraint::MIN);
                } else if constexpr (Constraint::HAS_MAX) {
                    return std::min(value, Constraint::MAX);
                }
            }
        }
        return value;
    }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, Vector2f> {
    static Vector2f Transform(const Vector2f& value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector2f(
                        std::clamp(value.x_, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]),
                        std::clamp(value.y_, Constraint::MIN_COMPONENTS[1], Constraint::MAX_COMPONENTS[1])
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector2f(
                        std::max(value.x_, Constraint::MIN_COMPONENTS[0]),
                        std::max(value.y_, Constraint::MIN_COMPONENTS[1])
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector2f(
                        std::min(value.x_, Constraint::MAX_COMPONENTS[0]),
                        std::min(value.y_, Constraint::MAX_COMPONENTS[1])
                    );
                }
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector2f(
                        std::clamp(value.x_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.y_, Constraint::MIN, Constraint::MAX)
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector2f(
                        std::max(value.x_, Constraint::MIN),
                        std::max(value.y_, Constraint::MIN)
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector2f(
                        std::min(value.x_, Constraint::MAX),
                        std::min(value.y_, Constraint::MAX)
                    );
                }
            }
        }
        return value;
    }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, Vector3f> {
    static Vector3f Transform(const Vector3f& value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector3f(
                        std::clamp(value.x_, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]),
                        std::clamp(value.y_, Constraint::MIN_COMPONENTS[1], Constraint::MAX_COMPONENTS[1]),
                        std::clamp(value.z_, Constraint::MIN_COMPONENTS[2], Constraint::MAX_COMPONENTS[2])
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector3f(
                        std::max(value.x_, Constraint::MIN_COMPONENTS[0]),
                        std::max(value.y_, Constraint::MIN_COMPONENTS[1]),
                        std::max(value.z_, Constraint::MIN_COMPONENTS[2])
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector3f(
                        std::min(value.x_, Constraint::MAX_COMPONENTS[0]),
                        std::min(value.y_, Constraint::MAX_COMPONENTS[1]),
                        std::min(value.z_, Constraint::MAX_COMPONENTS[2])
                    );
                }
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector3f(
                        std::clamp(value.x_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.y_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.z_, Constraint::MIN, Constraint::MAX)
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector3f(
                        std::max(value.x_, Constraint::MIN),
                        std::max(value.y_, Constraint::MIN),
                        std::max(value.z_, Constraint::MIN)
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector3f(
                        std::min(value.x_, Constraint::MAX),
                        std::min(value.y_, Constraint::MAX),
                        std::min(value.z_, Constraint::MAX)
                    );
                }
            }
        }
        return value;
    }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, Vector4f> {
    static Vector4f Transform(const Vector4f& value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector4f(
                        std::clamp(value.x_, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]),
                        std::clamp(value.y_, Constraint::MIN_COMPONENTS[1], Constraint::MAX_COMPONENTS[1]),
                        std::clamp(value.z_, Constraint::MIN_COMPONENTS[2], Constraint::MAX_COMPONENTS[2]),
                        std::clamp(value.w_, Constraint::MIN_COMPONENTS[3], Constraint::MAX_COMPONENTS[3])
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector4f(
                        std::max(value.x_, Constraint::MIN_COMPONENTS[0]),
                        std::max(value.y_, Constraint::MIN_COMPONENTS[1]),
                        std::max(value.z_, Constraint::MIN_COMPONENTS[2]),
                        std::max(value.w_, Constraint::MIN_COMPONENTS[3])
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector4f(
                        std::min(value.x_, Constraint::MAX_COMPONENTS[0]),
                        std::min(value.y_, Constraint::MAX_COMPONENTS[1]),
                        std::min(value.z_, Constraint::MAX_COMPONENTS[2]),
                        std::min(value.w_, Constraint::MAX_COMPONENTS[3])
                    );
                }
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return Vector4f(
                        std::clamp(value.x_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.y_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.z_, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.w_, Constraint::MIN, Constraint::MAX)
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return Vector4f(
                        std::max(value.x_, Constraint::MIN),
                        std::max(value.y_, Constraint::MIN),
                        std::max(value.z_, Constraint::MIN),
                        std::max(value.w_, Constraint::MIN)
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return Vector4f(
                        std::min(value.x_, Constraint::MAX),
                        std::min(value.y_, Constraint::MAX),
                        std::min(value.z_, Constraint::MAX),
                        std::min(value.w_, Constraint::MAX)
                    );
                }
            }
        }
        return value;
    }
};

template<GEParamsMemberTag Tag>
struct GEParamsValueTransformer<Tag, std::pair<float, float>> {
    static std::pair<float, float> Transform(const std::pair<float, float>& value) {
        using Constraint = GEParamsConstraintInfo<Tag>;
        if constexpr (Constraint::HAS_RANGE) {
            if constexpr (Constraint::COMPONENT_WISE) {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return std::make_pair(
                        std::clamp(value.first, Constraint::MIN_COMPONENTS[0], Constraint::MAX_COMPONENTS[0]),
                        std::clamp(value.second, Constraint::MIN_COMPONENTS[1], Constraint::MAX_COMPONENTS[1])
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return std::make_pair(
                        std::max(value.first, Constraint::MIN_COMPONENTS[0]),
                        std::max(value.second, Constraint::MIN_COMPONENTS[1])
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return std::make_pair(
                        std::min(value.first, Constraint::MAX_COMPONENTS[0]),
                        std::min(value.second, Constraint::MAX_COMPONENTS[1])
                    );
                }
            } else {
                if constexpr (Constraint::HAS_MIN && Constraint::HAS_MAX) {
                    return std::make_pair(
                        std::clamp(value.first, Constraint::MIN, Constraint::MAX),
                        std::clamp(value.second, Constraint::MIN, Constraint::MAX)
                    );
                } else if constexpr (Constraint::HAS_MIN) {
                    return std::make_pair(
                        std::max(value.first, Constraint::MIN),
                        std::max(value.second, Constraint::MIN)
                    );
                } else if constexpr (Constraint::HAS_MAX) {
                    return std::make_pair(
                        std::min(value.first, Constraint::MAX),
                        std::min(value.second, Constraint::MAX)
                    );
                }
            }
        }
        return value;
    }
};

} // namespace GEV2
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VALUE_TRANSFORMER_H
