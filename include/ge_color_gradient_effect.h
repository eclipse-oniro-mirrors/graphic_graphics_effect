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

#ifndef GRAPHICS_EFFECT_GE_COLOR_GRADIENT_SHADER_EFFECT_H
#define GRAPHICS_EFFECT_GE_COLOR_GRADIENT_SHADER_EFFECT_H

#include "ge_visual_effect.h"
#include "ge_shader.h"
#include "ge_common.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
class GEColorGradientEffect : public GEShader {
public:
    GEColorGradientEffect(const Drawing::GEXColorGradientEffectParams& param);
    ~GEColorGradientEffect() override = default;
    GEColorGradientEffect(const GEColorGradientEffect&) = delete;
    GEColorGradientEffect(const GEColorGradientEffect&&) = delete;
    GEColorGradientEffect& operator=(const GEColorGradientEffect&) = delete;
    GEColorGradientEffect& operator=(const GEColorGradientEffect&&) = delete;

    const std::string GetDescription() const { return "GEColorGradientEffect"; }

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
private:
    static void MakeColorGradientEffect();
    static void MakeColorGradientEffectWithMask();
    void CalculateBlenderCol();
    bool IsValidParam(float width, float height);
    void SetUniform(float width, float height);
    float CalculateCompressRatio();

    std::vector<Drawing::Color4f> colors_;
    std::vector<Drawing::Color4f> blend_colors_;
    std::vector<Drawing::Point> positions_;
    std::vector<float> strengths_;
    std::shared_ptr<Drawing::GEShaderMask> mask_ = nullptr;
    int paramColorSize_ = 0;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_ = nullptr;
    float gradientBlend_ = 6.0f;
    float gradientBlendK_ = 20.0f;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_COLOR_GRADIENT_SHADER_EFFECT_H