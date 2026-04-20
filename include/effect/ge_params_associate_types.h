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

#ifndef GRAPHICS_EFFECT_GE_PARAMS_ASSOCIATE_TYPES_H
#define GRAPHICS_EFFECT_GE_PARAMS_ASSOCIATE_TYPES_H
#include <cstdint>

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

#include "utils/matrix.h"
#include "utils/rect.h"
/*
 * Common associate types for params definition
 * DO NOT PUT THESE TYPES IN THIS FILE:
 * 1. ACTUAL params definition
 * 2. Types that only work for exactly ONE param definition
 * 3. Any complex helpers with implementations
 */

namespace OHOS {
namespace Rosen {
namespace Drawing {
// Canvas Info for GEVisualEffectImpl, originated from 2d canvas SetGeometry-related info
struct CanvasInfo {
    float geoWidth = 0.0f;
    float geoHeight = 0.0f;
    float tranX = 0.0f;
    float tranY = 0.0f;
    Drawing::Matrix mat;
    RectF materialDst;
};

// Rounded rect used by SDF, FrostedGlass, etc
struct GERRect {
    float left_ = 0.f;
    float top_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float radiusX_ = 0.f;
    float radiusY_ = 0.f;
    enum Corner : uint32_t {
        TOP_LEFT = 0,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        CORNER_COUNT,
    };

    Vector2f radius_[CORNER_COUNT] = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } };

    void SetCornerRadius(float radiusX, float radiusY)
    {
        for (uint32_t index = 0; index < CORNER_COUNT; index++) {
            radius_[index] = Vector2f(radiusX, radiusY);
        }
    }

    void SetCornerRadius(const Vector4f& radius)
    {
        radius_[TOP_LEFT] = Vector2f(radius[TOP_LEFT], radius[TOP_LEFT]);
        radius_[TOP_RIGHT] = Vector2f(radius[TOP_RIGHT], radius[TOP_RIGHT]);
        radius_[BOTTOM_RIGHT] = Vector2f(radius[BOTTOM_RIGHT], radius[BOTTOM_RIGHT]);
        radius_[BOTTOM_LEFT] = Vector2f(radius[BOTTOM_LEFT], radius[BOTTOM_LEFT]);
    }

    bool HasCircularCornerRadii() const
    {
        for (uint32_t index = 0; index < CORNER_COUNT; index++) {
            if (!ROSEN_EQ(radius_[index].x_, radius_[index].y_)) {
                return false;
            }
        }
        return true;
    }

    bool HasUniformCornerRadii() const
    {
        for (uint32_t index = 1; index < CORNER_COUNT; index++) {
            if (!radius_[index].IsNearEqual(radius_[TOP_LEFT])) {
                return false;
            }
        }
        return true;
    }

    float GetCommonRadiusX() const
    {
        return radius_[TOP_LEFT].x_;
    }
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_PARAMS_ASSOCIATE_TYPES_H
