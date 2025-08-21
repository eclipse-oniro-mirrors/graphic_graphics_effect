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
namespace{
thread_local static std::shared_ptr<Drawing::RuntimeEffect> frameGradientMaskShaderEffect_ = nullptr;
}
GEFrameGradientShaderMask::GEFrameGradientShaderMask(const GEFrameGradientMaskParams& param) :
    gradientBezierControlPoints_(param.gradientBezierControlPoints), cornerRadius_(param.cornerRadius), frameWidth_(param.frameWidth)
    {}

std::shared_ptr<ShaderEffect> GEFrameGradientShaderMask::GenerateDrawingShader(float width, float height) const
{
    MakeFrameGradientMaskShaderEffect();
    if (!frameGradientMaskShaderEffect_  || cornerRadius_ < 0.0 || frameWidth_ < 0.0 || width <= 0.0 || height <= 0.0) {
        LOGE("GEFrameGradientShaderMask::GenerateDrawingShader create builder error");
        return nullptr;
    }
    auto maskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(frameGradientMaskShaderEffect_);
    maskBuilder->SetUniform("iResolution", width, height);
    std::array<float, 4> gradientBezier = {gradientBezierControlPoints_[0], gradientBezierControlPoints_[1],
        gradientBezierControlPoints_[2], gradientBezierControlPoints_[3]}
    maskBuilder->SetUniform("gradientBezierControlPoints", gradientBezier.data(), 4);
    maskBuilder->SetUniform("cornerRadius", cornerRadius_);
    maskBuilder->SetUniform("frameWidth", frameWidth_);
    auto maskShader = maskBuilder->MakeShader(nullptr, false);
    if (!maskShader) {
        LOGE("GEFrameGradientShaderMask::GenerateDrawingShader effect error");
    }
    return maskShader;
}

std::shared_ptr<ShaderEffect> GEFrameGradientShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    MakeFrameGradientMaskShaderEffect();
    if (!frameGradientMaskShaderEffect_  || cornerRadius_ < 0.0 || frameWidth_ < 0.0 || width <= 0.0 || height <= 0.0) {
        LOGE("GEFrameGradientShaderMask::GenerateDrawingShader create builder error");
        return nullptr;
    }
    auto maskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(frameGradientMaskShaderEffect_);
    maskBuilder->SetUniform("iResolution", width, height);
    std::array<float, 4> gradientBezier = {gradientBezierControlPoints_[0], gradientBezierControlPoints_[1],
        gradientBezierControlPoints_[2], gradientBezierControlPoints_[3]}
    maskBuilder->SetUniform("gradientBezierControlPoints", gradientBezier.data(), 4);
    maskBuilder->SetUniform("cornerRadius", cornerRadius_);
    maskBuilder->SetUniform("frameWidth", frameWidth_);
    auto maskShader = maskBuilder->MakeShader(nullptr, false);
    if (!maskShader) {
        LOGE("GEFrameGradientShaderMask::GenerateDrawingShaderHasNormal effect error");
    }
    return maskShader;
}

void GEFrameGradientShaderMask::MakeFrameGradientMaskShaderEffect() const
{
    if (frameGradientMaskShaderEffect_) {
        return ;
    }

    static const std::string maskString(R"(
        uniform half2 iResolution;
        uniform half4 gradientBezierControlPoints;
        uniform half frameWidth;
        uniform half cornerRadius;

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
 
        half4 main(float2 fragCoord) {
            half2 uv = fragCoord.xy / iResolution.xy;
            half screenRatio = iResolution.x / iResolution.y;
            uv = uv * 2.0 - 1.0;
            uv.x *= screenRatio;
            half sdfRRect =
                abs(sdRoundedBox(uv.xy, vec2(screenRatio, 1.0),
                clamp(cornerRadius, 0.0, min(screenRatio, 1.0))));
            sdfRRect = (sdfRRect - frameWidth) / (-frameWidth - 1e-6);
            sdfRRect = cubicBezierInterpolation(gradientBezierControlPoints, clamp(sdfRRect, 0.0, 1.0));
            return half4(sdfRRect);
        }
    )");

    frameGradientMaskShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(maskString);
    if (frameGradientMaskShaderEffect_ == nullptr) {
        LOGE("GEFrameGradientShaderMask::MakeFrameGradientMaskShaderEffect error");
    }

}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS