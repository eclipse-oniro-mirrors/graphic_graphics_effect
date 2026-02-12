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
#include "ge_tone_mapping_helper.h"

#include <algorithm>
#include <mutex>

#include "ge_edge_light_shader_filter.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

namespace {
// Bezier curves describing tone mapping rule, anchors define
const std::vector<Vector2f> g_hdrCurveAnchors = {
    {0.0f, 0.0f}, {0.75f, 0.75f}, {1.0f, 0.95f}, {1.25f, 0.97f},
    {1.5f, 0.98f}, {1.75f, 0.9875f}, {1.90f, 0.995f}, {2.0f, 1.0f},
};

// Bezier curves describing tone mapping rule, control points define
const std::vector<Vector2f> g_hdrCurveControlPoint = {
    {0.375f, 0.375f}, {0.875f, 0.875f}, {1.125f, 0.9625f}, {1.375f, 0.9725f},
    {1.625f, 0.98f}, {1.825f, 0.99f}, {1.95f, 0.998f},  // 1 less than anchors
};

constexpr float SDR_LUMINANCE = 1.0f;
}  // namespace

Vector4f GEToneMappingHelper::GetBrightnessMapping(float headroom, Vector4f& input)
{
    float highValue = std::max({input.x_, input.y_, input.z_, SDR_LUMINANCE});
    float compressRatio = GetBrightnessMapping(headroom, highValue) / highValue;
    Vector4f output;
    output.x_ = input.x_ * compressRatio;
    output.y_ = input.y_ * compressRatio;
    output.z_ = input.z_ * compressRatio;
    output.w_ = input.w_;
    return output;
}

float GEToneMappingHelper::GetBrightnessMapping(float headroom, float input)
{
    if (ROSEN_GE(headroom, EFFECT_MAX_LUMINANCE)) {
        return input;
    }
    int rangeIndex = -1;
    for (auto point : g_hdrCurveAnchors) {
        if (input <= point.x_) {
            break;
        }
        rangeIndex++;
    }
    if (rangeIndex == -1) {
        return 0.0f;
    }
    if ((rangeIndex >= static_cast<int>(g_hdrCurveAnchors.size()) - 1) ||
        (rangeIndex >= static_cast<int>(g_hdrCurveControlPoint.size()))) {
        return headroom;
    }

    // calculate new hdr bightness via bezier curve
    Vector2f start = g_hdrCurveAnchors[rangeIndex];
    Vector2f end = g_hdrCurveAnchors[rangeIndex + 1];
    Vector2f control = g_hdrCurveControlPoint[rangeIndex];

    float y = 0.0f;
    if (CalcBezierResultY(start, end, control, input, y)) {
        y = ((EFFECT_MAX_LUMINANCE - headroom) * y + (headroom - SDR_LUMINANCE) * input) /
            (EFFECT_MAX_LUMINANCE - SDR_LUMINANCE);  // linear interpolation
        LOGD("GEToneMappingHelper::GetBrightnessMapping y is %{public}f", y);
        return std::clamp(y, 0.0f, headroom);
    }
    return std::clamp(input, 0.0f, headroom);
}

Drawing::Color4f GEToneMappingHelper::GetBrightnessMapping(float brightness, Drawing::Color4f& input)
{
    float highValue = std::max({input.redF_, input.greenF_, input.blueF_, SDR_LUMINANCE});
    float compressRatio = GetBrightnessMapping(brightness, highValue) / highValue;
    Drawing::Color4f output;
    output.redF_ = input.redF_ * compressRatio;
    output.greenF_ = input.greenF_ * compressRatio;
    output.blueF_ = input.blueF_ * compressRatio;
    output.alphaF_ = input.alphaF_;
    return output;
}

bool GEToneMappingHelper::CalcBezierResultY(
    const Vector2f& start, const Vector2f& end, const Vector2f& control, float x, float& y)
{
    const float a = start[0] - 2 * control[0] + end[0];
    const float b = 2 * (control[0] - start[0]);
    const float c = start[0] - x;
    const float discriminant = b * b - 4 * a * c;

    float t = 0.0f;
    if (ROSEN_LNE(discriminant, 0.0f)) {
        return false;
    } else if (ROSEN_EQ(a, 0.0f)) {
        t = -c / b;
    } else {
        const float sqrtD = std::sqrt(discriminant);
        const float t1 = (-b + sqrtD) / (2.0 * a);
        const float t2 = (-b - sqrtD) / (2.0 * a);
        if (ROSEN_GE(t1, 0.0f) && ROSEN_LE(t1, 1.0f)) {
            t = t1;
        } else if (ROSEN_GE(t2, 0.0f) && ROSEN_LE(t2, 1.0f)) {
            t = t2;
        }
    }

    y = start[1] + t * (2.0f * (control[1] - start[1]) + t * (start[1] - 2.0f * control[1] + end[1]));
    return true;
}

} // Rosen
} // OHOS
