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
#ifndef GRAPHICS_EFFECT_POINT_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_POINT_LIGHT_SHADER_H

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "draw/canvas.h"
#include "effect/runtime_shader_builder.h"
#include "effect/shader_effect.h"
#include "ge_common.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "ge_tone_mapping_helper.h"
#include "utils/rect.h"
#include "utils/round_rect.h"

namespace OHOS {
namespace Rosen {

constexpr int MAX_LIGHT_SOURCES = 12;

enum class IlluminatedType : uint32_t {
    INVALID = static_cast<uint32_t>(-1),
    NONE = 0,
    BORDER,
    CONTENT,
    BORDER_CONTENT,
    BLOOM_BORDER,
    BLOOM_BORDER_CONTENT,
    BLEND_BORDER,
    BLEND_CONTENT,
    BLEND_BORDER_CONTENT,
    FEATHERING_BORDER,
    NORMAL_BORDER_CONTENT
};

struct GEPointLightSourceData {
    Vector4f lightPosition;
    float lightIntensity = 0.0f;
    Vector4f lightColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
};

struct GEPointLightShaderParamsUser {
    IlluminatedType illuminatedType = IlluminatedType::INVALID;
    float borderWidth = 0.0f;
    Drawing::RoundRect contentRRect = Drawing::RoundRect(Drawing::Rect(), 0.0f, 0.0f);
    Drawing::RoundRect borderRRect = Drawing::RoundRect(Drawing::Rect(), 0.0f, 0.0f);
    Drawing::Rect sdfDrawRect;
    std::shared_ptr<Drawing::ShaderEffect> sdfShaderEffect = nullptr;
    std::vector<GEPointLightSourceData> lightSources;
    bool enableEDREffect = false;
    float displayHeadroom = 0.0f;
    float bloom = 0.0f; // not used yet
};

class GE_EXPORT GEPointLightShader : public GEShader {
public:
    explicit GEPointLightShader(const Drawing::GEPointLightShaderParams& params);
    ~GEPointLightShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEPointLightShader, Drawing::GEPointLightShaderParams);

    void OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override {}
    void MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress) override {}

    const std::string GetDescription() const { return "GEPointLightShader"; }

private:
    GEPointLightShader(const GEPointLightShader&) = delete;
    GEPointLightShader(GEPointLightShader&&) = delete;
    GEPointLightShader& operator=(const GEPointLightShader&) = delete;
    GEPointLightShader& operator=(GEPointLightShader&&) = delete;

    void DrawLight(Drawing::Canvas& canvas) const;
    void DrawLightByIlluminatedType(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
        const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawSingleLightByIlluminatedType(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
        const std::array<float, 1>& lightIntensityArray) const;
    void ProcessLightSourcesData(std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    void ProcessSingleLightSourcesData(std::array<float, 1>& lightIntensityArray,
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> CreateShaderBuilder(size_t lightLength) const;

    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetPhongShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFeatheringBorderLightShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetNormalLightShaderBuilder(size_t lightLength);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFContentLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFBorderLightShaderBuilder();
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSDFContentAndBorderLightShaderBuilder();

    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeFeatheringBoardLightShaderBuilder(size_t lightLength) const;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeNormalLightShaderBuilder(size_t lightLength) const;

    void DrawContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawSingleContentLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Brush& brush, const std::array<float, 1>& lightIntensityArray) const;
    bool DrawSDFContentLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const;
    void DrawBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawSingleBorderLight(Drawing::Canvas& canvas, std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        Drawing::Pen& pen, const std::array<float, 1>& lightIntensityArray) const;
    bool DrawSDFBorderLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect) const;
    void DrawContentAndBorderLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
        const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const;
    void DrawSingleContentAndBorderLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
        const std::array<float, 1>& lightIntensityArray) const;
    void DrawSDFContentAndBorderLight(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffectContent,
        std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const;

    GEPointLightShaderParamsUser params_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_POINT_LIGHT_SHADER_H