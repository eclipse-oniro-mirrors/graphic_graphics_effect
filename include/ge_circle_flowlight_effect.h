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
#ifndef GRAPHICS_EFFECT_GE_CIRCLE_FLOWLIGHT_EFFECT_H
#define GRAPHICS_EFFECT_GE_CIRCLE_FLOWLIGHT_EFFECT_H

#include "ge_common.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_visual_effect.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr size_t COLOR_SIZE = 16;
}
class GE_EXPORT GECircleFlowlightEffect : public GEShader {
public:
    GECircleFlowlightEffect(Drawing::GECircleFlowlightEffectParams& CircleFlowlightEffectParams);
    ~GECircleFlowlightEffect() override = default;
    GECircleFlowlightEffect(const GECircleFlowlightEffect&) = delete;
    GECircleFlowlightEffect(const GECircleFlowlightEffect&&) = delete;
    GECircleFlowlightEffect& operator=(const GECircleFlowlightEffect&) = delete;
    GECircleFlowlightEffect& operator=(const GECircleFlowlightEffect&&) = delete;

    DECLARE_GEFILTER_TYPEFUNC(GECircleFlowlightEffect, Drawing::GECircleFlowlightEffectParams);

    const std::string GetDescription() const { return "GECircleFlowlightEffect"; }

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
private:
    constexpr uint8_t COLOR_CHANNEL = 4;
    constexpr uint8_t ARRAY_NUM = 4;

    static void MakeCircleFlowlightEffect();
    static void MakeCircleFlowlightEffectWithMask();
    bool IsValidParam(float width, float height);
    void SetUniform(float width, float height);

    std::array<float, COLOR_CHANNEL * ARRAY_NUM> colors_;
    Vector4f rotationFrequency_;
    Vector4f rotationAmplitude_;
    Vector4f rotationSeed_;
    Vector4f gradientX_;
    Vector4f gradientY_;
    float progress_;
    std::shared_ptr<Drawing::GEShaderMask> mask_ = nullptr;

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_CIRCLE_FLOWLIGHT_EFFECT_H