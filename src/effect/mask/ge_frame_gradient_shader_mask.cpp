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

#include <chrono>
#include "ge_log.h"
#include "ge_frame_gradient_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
thread_local static std::shared_ptr<Drawing::RuntimeEffect> frameGradientMaskShaderEffect_ = nullptr;

static constexpr float EPSILON = 1e-6;
static constexpr float SQRT_2 = 1.41421356f;
static constexpr float NORMALIZATION_FACTOR = 2.0f;

inline static const std::string maskString = R"(
    uniform half2 iResolution;
    uniform half4 innerBezier;
    uniform half4 outerBezier;
    uniform half innerFrameWidth;
    uniform half outerFrameWidth;
    uniform half cornerRadius;
    uniform half2 rectWH;
    uniform half2 rectPos;
    uniform half axialFeatherStrength;
    uniform half axialCenter;
    uniform half axialCoreWidth;
    uniform half2 axialDirection;
    uniform half boxAngleDeg;

    const half PI = 3.1415926;

    half sdRoundedBox(half2 p, half2 b, half r)
    {
        half2 q = abs(p) - b + r;
        return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
    }

    half cubicBezierInterpolation(half4 controlPoints, half t)
    {
        // [(1-t)^3 * {0,0} + 3(1-t)^2 * t * CP.xy + 3t^2 * (1-t) * CP.zw + t^3 * {1,1}].y
        half2 XY = controlPoints.yw + controlPoints.yw + controlPoints.yw;
        return (((1.0 + XY.x - XY.y) * t - XY.x - XY.x + XY.y) * t + XY.x) * t;
    }

    half AxialEnvelope(half t, half axialCenter, half axialCoreWidth)
    {
        half halfWidth = 0.5 * axialCoreWidth;
        half riseEnd = axialCenter - halfWidth;
        half fallStart = axialCenter + halfWidth;

        half rise = smoothstep(0.0, riseEnd, t);
        half fall = 1.0 - smoothstep(fallStart, 1.0, t);
        return rise * fall;
    }

    half2 Rotate2D(half2 p, half a)
    {
        half s = sin(a);
        half c = cos(a);
        return half2(c * p.x - s * p.y,
            s * p.x + c * p.y);
    }

    half4 main(float2 fragCoord) {
        half2 uv = fragCoord / iResolution.xy;
        half2 centeredUvs = (uv - 0.5) * 2.0 - rectPos;
        half screenRatio = iResolution.x / iResolution.y;
        centeredUvs.x *= screenRatio;

        half angle = boxAngleDeg * PI / 180.0;
        half2 localPos = Rotate2D(centeredUvs, -angle);

        half sdfrect = sdRoundedBox(localPos.xy, half2(screenRatio * rectWH.x, rectWH.y),
                        clamp(cornerRadius, 0.0, min(screenRatio * rectWH.x, rectWH.y)));

        half2 sdfInOut = half2(sdfrect, -sdfrect) + half2(innerFrameWidth, outerFrameWidth);

        sdfInOut /= half2(innerFrameWidth, outerFrameWidth) + 1e-5;

        half gradient = min(sdfInOut.x, sdfInOut.y);

        if (gradient <= 0.0) {
            return half4(0.0);
        }
        gradient = cubicBezierInterpolation(sdfrect > 0.0 ? outerBezier : innerBezier, gradient);

        // new optional axial modulation
        half dirLen2 = dot(axialDirection, axialDirection);
        half enable = step(1e-5, dirLen2) * step(1e-5, axialFeatherStrength);
        half2 dir = (dirLen2 > 1e-5) ? axialDirection * inversesqrt(dirLen2) : half2(0.0, 0.0);
        half halfExtent = abs(dir.x) * rectWH.x + abs(dir.y) * rectWH.y;
        half axisCoord = dot(half2(localPos.x / screenRatio, localPos.y), dir);
        half t = clamp(axisCoord / (2.0 * max(halfExtent, 1e-5)) + 0.5, 0.0, 1.0);
        half envelope = AxialEnvelope(t, axialCenter, axialCoreWidth);
        half axialMask = mix(1.0, mix(1.0, envelope, axialFeatherStrength), enable);
        return half4(gradient * axialMask);
    }
)";

} // namespace

GEFrameGradientShaderMask::GEFrameGradientShaderMask(const GEFrameGradientMaskParams& param)
    : innerBezier_(param.innerBezier),
      outerBezier_(param.outerBezier),
      cornerRadius_(std::max(param.cornerRadius, 0.0f)),
      innerFrameWidth_(std::max(param.innerFrameWidth, 0.0f)),
      outerFrameWidth_(std::max(param.outerFrameWidth, 0.0f)),
      rectWH_(std::make_pair(std::max(param.rectWH.first, 0.0f), std::max(param.rectWH.second, 0.0f))),
      rectPos_(param.rectPos), axialFeatherStrength_(param.axialFeatherStrength), axialCenter_(param.axialCenter),
      axialCoreWidth_(param.axialCoreWidth), axialDirection_(param.axialDirection), boxAngleDeg_(param.boxAngleDeg)
{}

bool GEFrameGradientShaderMask::ValidateParams(float width, float height) const
{
    if (width < EPSILON || height < EPSILON) {
        GE_LOGE("GEFrameGradientShaderMask::ValidateParams size zero");
        return false;
    }
    if (innerFrameWidth_ < EPSILON && outerFrameWidth_ < EPSILON) {
        GE_LOGE("GEFrameGradientShaderMask::ValidateParams FrameWidth zero");
        return false;
    }
    if (rectWH_.first < EPSILON && rectWH_.second < EPSILON) {
        GE_LOGE("GEFrameGradientShaderMask::ValidateParams RectWH zero");
        return false;
    }

    GE_LOGD("GEFrameGradientShaderMask::CreateFrameGradientMaskShader -> "
        "innerFrameWidth: %{public}f, outerFrameWidth: %{public}f, cornerRadius: %{public}f, "
        "rectPos: (%{public}f, %{public}f), "
        "rectWH: (%{public}f, %{public}f), "
        "innerBezier: (%{public}f, %{public}f, %{public}f, %{public}f), "
        "outerBezier: (%{public}f, %{public}f, %{public}f, %{public}f)",
        innerFrameWidth_, outerFrameWidth_, cornerRadius_, rectPos_.first, rectPos_.second, rectWH_.first,
        rectWH_.second, innerBezier_[0], innerBezier_[1], innerBezier_[2], innerBezier_[3], outerBezier_[0],
        outerBezier_[1], outerBezier_[2], outerBezier_[3]);
    return true;
}

std::shared_ptr<ShaderEffect> GEFrameGradientShaderMask::GenerateDrawingShader(float width, float height) const
{
    return CreateFrameGradientMaskShader(width, height);
}

std::shared_ptr<ShaderEffect> GEFrameGradientShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    return CreateFrameGradientMaskShader(width, height);
}

std::shared_ptr<ShaderEffect> GEFrameGradientShaderMask::CreateFrameGradientMaskShader(float width, float height) const
{
    MakeFrameGradientMaskShaderEffect();
    if (!frameGradientMaskShaderEffect_ || !ValidateParams(width, height)) {
        GE_LOGE("GEFrameGradientShaderMask::CreateFrameGradientMaskShader create builder error");
        return nullptr;
    }

    auto maskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(frameGradientMaskShaderEffect_);
    maskBuilder->SetUniform("iResolution", width, height);
    std::array<float, ARRAY_SIZE_FOUR> innerBezier = {innerBezier_[0],
        innerBezier_[1], innerBezier_[2], innerBezier_[3]};
    std::array<float, ARRAY_SIZE_FOUR> outerBezier = {outerBezier_[0],
        outerBezier_[1], outerBezier_[2], outerBezier_[3]};
    maskBuilder->SetUniform("innerBezier", innerBezier.data(), ARRAY_SIZE_FOUR);
    maskBuilder->SetUniform("outerBezier", outerBezier.data(), ARRAY_SIZE_FOUR);
    maskBuilder->SetUniform("cornerRadius", cornerRadius_ * NORMALIZATION_FACTOR / height);
    maskBuilder->SetUniform("innerFrameWidth", innerFrameWidth_ * NORMALIZATION_FACTOR / height);
    maskBuilder->SetUniform("outerFrameWidth", outerFrameWidth_ * NORMALIZATION_FACTOR / height);
    maskBuilder->SetUniform("rectWH", rectWH_.first, rectWH_.second);
    maskBuilder->SetUniform("rectPos", rectPos_.first, rectPos_.second);
    maskBuilder->SetUniform("axialFeatherStrength", axialFeatherStrength_);
    maskBuilder->SetUniform("axialCenter", axialCenter_);
    maskBuilder->SetUniform("axialCoreWidth", axialCoreWidth_);
    maskBuilder->SetUniform("axialDirection", axialDirection_.first, axialDirection_.second);
    maskBuilder->SetUniform("boxAngleDeg", boxAngleDeg_);
    auto maskShader = maskBuilder->MakeShader(nullptr, false);
    if (!maskShader) {
        GE_LOGE("GEFrameGradientShaderMask::CreateFrameGradientMaskShader fail");
    }
    return maskShader;
}

void GEFrameGradientShaderMask::MakeFrameGradientMaskShaderEffect() const
{
    if (frameGradientMaskShaderEffect_) {
        return;
    }

    frameGradientMaskShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(maskString);
    if (frameGradientMaskShaderEffect_ == nullptr) {
        GE_LOGE("GEFrameGradientShaderMask::MakeFrameGradientMaskShaderEffect error");
    }
}

Drawing::Rect GEFrameGradientShaderMask::GetSubtractedRect(float width, float height) const
{
    if (!ValidateParams(width, height)) {
        GE_LOGE("GEFrameGradientShaderMask::GetSubtractedRect invalid input");
        return Drawing::Rect();
    }

    float screenRatio = height / width;

    // rectPos_ [-1, 1] Normalized to [0, 1]
    float centerXNormalized = (rectPos_.first + 1.0f) / NORMALIZATION_FACTOR;
    float centerYNormalized = (rectPos_.second + 1.0f) / NORMALIZATION_FACTOR;

    // Calculate top-left coordinates of the rounded rectangle
    float rectLeft = centerXNormalized - rectWH_.first / NORMALIZATION_FACTOR;
    float rectTop = centerYNormalized - rectWH_.second / NORMALIZATION_FACTOR;
    float rectRight = rectLeft + rectWH_.first;
    float rectBottom = rectTop + rectWH_.second;

    // Calculate half of the inner border width
    float innerBorderWidth = innerFrameWidth_ / height;

    // Calculate minimum edge to determine real corner radius
    float minEdge = std::min(height * rectWH_.second, width * rectWH_.first) / NORMALIZATION_FACTOR;
    float realCornerRadius = std::min(cornerRadius_, minEdge) / height;

    // EPSILON:Small offset for precision
    // Calculate initial adjusted boundary values based on border width
    float adjustedLeft = rectLeft + innerBorderWidth * screenRatio + EPSILON;
    float adjustedTop = rectTop + innerBorderWidth + EPSILON;
    float adjustedRight = rectRight - innerBorderWidth * screenRatio - EPSILON;
    float adjustedBottom = rectBottom - innerBorderWidth - EPSILON;

    // If border width is greater than or equal to corner radius, calculate inscribed rectangle directly by border width
    if (innerBorderWidth < realCornerRadius) {
        // Calculate inscribed rectangle based on aspect ratio and corner radius relationship
        if (std::abs(screenRatio * rectWH_.second - rectWH_.first) > realCornerRadius) {
            if (screenRatio * rectWH_.second > rectWH_.first) {
                // Vertical rectangle case
                adjustedTop = rectTop + realCornerRadius;
                adjustedBottom = rectBottom - realCornerRadius;
            } else {
                // Horizontal rectangle case
                adjustedLeft = rectLeft + realCornerRadius * screenRatio;
                adjustedRight = rectRight - realCornerRadius * screenRatio;
            }
        } else {
            // Calculate inscribed rectangle for corner area (diagonal case)
            float innerRadius = realCornerRadius - innerBorderWidth;
            float diagonalOffset = realCornerRadius - innerRadius / SQRT_2;
            adjustedTop = rectTop + diagonalOffset + EPSILON;
            adjustedBottom = rectBottom - diagonalOffset - EPSILON;
            adjustedLeft = rectLeft + diagonalOffset * screenRatio + EPSILON;
            adjustedRight = rectRight - diagonalOffset * screenRatio - EPSILON;
        }
    }
    GE_LOGD("GEFrameGradientShaderMask::GetSubtractedRect "
        "l: %{public}f, t: %{public}f, r: %{public}f, b: %{public}f, radius: %{public}f;",
        adjustedLeft, adjustedTop, adjustedRight, adjustedBottom, realCornerRadius);
    // Check rectangle validity
    if (adjustedLeft > adjustedRight || adjustedTop > adjustedBottom) {
        return Drawing::Rect();
    }
    return Drawing::Rect(adjustedLeft, adjustedTop, adjustedRight, adjustedBottom);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS