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

#include "ge_sdf_edge_light_shader.h"

#include <unordered_map>

#include "ge_log.h"

namespace OHOS::Rosen {
namespace {

constexpr char SHADER[] = R"(
    uniform vec2 iResolution;

    uniform shader sdfShader;
    uniform shader lightMaskShader;

    uniform float spreadFactor;

    uniform vec3 lightColor;
    uniform float bloomIntensityCutoff;
    uniform float maxIntensity;
    uniform float maxBloomIntensity;
    uniform float bloomFalloffPow;

    uniform float minBorderWidth;
    uniform float maxBorderWidth;

    uniform float innerBorderBloomWidth;
    uniform float outerBorderBloomWidth;

    float bloomMultiplierFromDist(float d) {
        float dNorm = abs(d) / (d > 0 ? outerBorderBloomWidth : innerBorderBloomWidth);
        float falloff = max((1 - dNorm) / pow(1 + dNorm, bloomFalloffPow), 0);
        return maxBloomIntensity * falloff;
    }

    float edgeLightFakeBloom(float intensity, float d, float smoothD) {
        float bloomBorder = 1 - step(outerBorderBloomWidth, d);
        bloomBorder *= step(-innerBorderBloomWidth, d);
        float edgeThickness = smoothstep(0, 1, intensity) *
                              (maxBorderWidth - minBorderWidth) + minBorderWidth;
        float thinBorder = (1 - smoothstep(0, edgeThickness, d)) * smoothstep(-edgeThickness, 0, d);
        float b = intensity * maxIntensity * (thinBorder + smoothstep(bloomIntensityCutoff, 1, intensity) *
                  bloomBorder * bloomMultiplierFromDist(smoothD));
        return b;
    }

    half4 main(vec2 fragCoord)
    {
        float lightMaskValue = lightMaskShader.eval(fragCoord).r;
        vec4 sdfSample = sdfShader.eval(fragCoord);
        half3 rgb = lightColor * edgeLightFakeBloom(lightMaskValue, sdfSample.a, sdfSample.a);
        return half4(rgb, rgb.r);
    }
)";
} // namespace

GESDFEdgeLightShader::GESDFEdgeLightShader(const Drawing::GESDFEdgeLightEffectParams& params)
    : params_(params)
{}

void GESDFEdgeLightShader::OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    canvas.Save();
    canvas.ResetClip();
    MakeDrawingShader(rect, -1.f); // not use progress
    auto shader = GetDrawingShader();
    if (!shader) {
        LOGE("GESDFEdgeLightShader::OnDrawShader: no shader generated, draw nothing");
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(rect);
    canvas.DetachBrush();
    canvas.Restore();
}

void GESDFEdgeLightShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = nullptr;

    if (!params_.sdfShape) {
        LOGE("GESDFEdgeLightShader::MakeDrawingShader sdfShape is null");
        return;
    }

    if (!params_.lightMask) {
        LOGE("GESDFEdgeLightShader::MakeDrawingShader lightMask is null");
        return;
    }

    auto builder = GetEffectShaderBuilder(rect);
    if (builder == nullptr) {
        LOGE("GESDFEdgeLightShader::MakeDrawingShader builder is nullptr");
        return;
    }

    auto sdfEdgeLightShader = builder->MakeShader(nullptr, false);
    if (sdfEdgeLightShader == nullptr) {
        LOGE("GESDFEdgeLightShader::MakeDrawingShader sdfEdgeLightShader is nullptr");
    }
    drShader_ = sdfEdgeLightShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFEdgeLightShader::GetEffectShaderBuilder(const Drawing::Rect& rect)
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> effectShader_ = nullptr;

    if (effectShader_ == nullptr) {
        effectShader_ = Drawing::RuntimeEffect::CreateForShader(SHADER);
    }

    if (effectShader_ == nullptr) {
        LOGE("GESDFEdgeLightShader::GetEffectShaderBuilder effectShader_ is nullptr");
        return nullptr;
    }

    const Drawing::Matrix matrix;
    const Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR);
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effectShader_);

    auto width = rect.GetWidth();
    auto height = rect.GetHeight();

    auto sdfShader = params_.sdfShape->GenerateDrawingShader(width, height);
    if (sdfShader == nullptr) {
        LOGE("GESDFEdgeLightShader::GetEffectShaderBuilder sdfShader is nullptr");
        return nullptr;
    }
    auto lightMaskShader = params_.lightMask->GenerateDrawingShader(width, height);
    if (lightMaskShader == nullptr) {
        LOGE("GESDFEdgeLightShader::GetEffectShaderBuilder lightMaskShader is nullptr");
        return nullptr;
    }
    builder->SetUniform("iResolution", width, height);
    builder->SetChild("sdfShader", sdfShader);
    builder->SetChild("lightMaskShader", lightMaskShader);
    builder->SetUniform("spreadFactor", params_.sdfSpreadFactor);
    builder->SetUniform("lightColor", params_.color[0], params_.color[1], params_.color[2]); // 0 is R, 1 is G, 2 is B
    builder->SetUniform("bloomIntensityCutoff", params_.bloomIntensityCutoff);
    builder->SetUniform("maxIntensity", params_.maxIntensity);
    builder->SetUniform("maxBloomIntensity", params_.maxBloomIntensity);
    builder->SetUniform("bloomFalloffPow", params_.bloomFalloffPow);

    params_.minBorderWidth = std::max(params_.minBorderWidth, std::numeric_limits<float>::epsilon());
    params_.maxBorderWidth = std::max(params_.maxBorderWidth, std::numeric_limits<float>::epsilon());
    builder->SetUniform("minBorderWidth", params_.minBorderWidth);
    builder->SetUniform("maxBorderWidth", params_.maxBorderWidth);

    builder->SetUniform("innerBorderBloomWidth", params_.innerBorderBloomWidth);
    builder->SetUniform("outerBorderBloomWidth", params_.outerBorderBloomWidth);

    return builder;
}
} // namespace OHOS::Rosen
