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

#include "ge_effects_params.h"
#include "ge_value_transformer_traits.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

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

struct Vector4fToRectFTransformer {
    static bool Transform(const Vector4f& value, RectF& out)
    {
        out = RectF(value.x_, value.y_, value.z_, value.w_);
        return true;
    }
};

struct PairToVector2fTransformer {
    static bool Transform(const std::pair<float, float>& value, Vector2f& out)
    {
        out = Vector2f(value.first, value.second);
        return true;
    }
};

struct PairToPointTransformer {
    static bool Transform(const std::pair<float, float>& value, Drawing::Point& out)
    {
        out = Drawing::Point(value.first, value.second);
        return true;
    }
};

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

struct SDFUnionOpTransformer {
    static bool Transform(uint32_t value, GESDFUnionOp& out)
    {
        if (value >= static_cast<uint32_t>(GESDFUnionOp::MAX)) {
            return false;
        }
        out = static_cast<GESDFUnionOp>(value);
        return true;
    }
};

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
