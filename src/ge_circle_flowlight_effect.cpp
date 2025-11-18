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
#include "ge_log.h"
#include "ge_circle_flowlight_effect.h"
#include "ge_visual_effect_impl.h"
#include <cmath>

namespace OHOS {
namespace Rosen {

namespace {
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_circleFlowlightEffect = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_circleFlowlightEffectWithMask = nullptr;
constexpr static uint8_t COLOR_CHANNEL = 4;
constexpr static uint8_t ARRAY_NUM = 4;

const std::string CIRCLE_FLOWLIGHT_SHADER(
    R"(
    uniform half2 iResolution;
    uniform half4 color[4];
    uniform half4 rotationFrequency;
    uniform half4 rotationAmplitude;
    uniform half4 rotationSeed;
    uniform half4 gradientX;
    uniform half4 gradientY;
    uniform half progress;

    float rand_wave(float frequency, float amplitude, float time, float seed)
    {
        return amplitude * sin(dot(vec2(time, seed), vec2(frequency, 78.233)));
    }
 
    vec4 color_gradient(vec2 frag_pos, float radius, float timeValue)
    {
        vec4 colorOne = color[0];
        vec4 colorTwo = color[1];
        vec4 colorThree = color[2];
        vec4 colorFour = color[3];
 
        if (radius >= 1.0) {return vec4(0.0);}
 
        float freq_time = -2.0 * timeValue;
        mat2 rotation_matrix = mat2(cos(freq_time), sin(freq_time), -sin(freq_time), cos(freq_time));
        vec2 gradient_p[4];
 
        float wave = rand_wave(rotationFrequency.x, rotationAmplitude.x, timeValue, rotationSeed.x);
        vec2 gradient_pos = rotation_matrix * (vec2(gradientX.x, gradientY.x) * 2.0 - 1.0);
        gradient_p[0] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.y, rotationAmplitude.y, timeValue, rotationSeed.y);
        gradient_pos = rotation_matrix * (vec2(gradientX.y, gradientY.y) * 2.0 - 1.0);
        gradient_p[1] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.z, rotationAmplitude.z, timeValue, rotationSeed.z);
        gradient_pos = rotation_matrix * (vec2(gradientX.z, gradientY.z) * 2.0 - 1.0);
        gradient_p[2] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.w, rotationAmplitude.w, timeValue, rotationSeed.w);
        gradient_pos = rotation_matrix * (vec2(gradientX.w, gradientY.w) * 2.0 - 1.0);
        gradient_p[3] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        float distorted_radius = pow(radius, 4.0);
        vec2 frag_pos_new = frag_pos / radius * distorted_radius;
        vec2 gradient_uv = frag_pos_new * 0.5 + 0.5;
        float gradient_total_w = 0.0;
        vec4 gradient_interp_c = vec4(0.0);
 
        float d0 = max(0.01, length(gradient_uv - gradient_p[0]));
        float w0 = 1.0 / pow(d0, 1.0);
        gradient_interp_c += w0 * colorOne;
        gradient_total_w += w0;
 
        float d1 = max(0.01, length(gradient_uv - gradient_p[1]));
        float w1 = 1.0 / pow(d1, 1.0);
        gradient_interp_c += w1 * colorTwo;
        gradient_total_w += w1;
 
        float d2 = max(0.01, length(gradient_uv - gradient_p[2]));
        float w2 = 1.0 / pow(d2, 1.0);
        gradient_interp_c += w2 * colorThree;
        gradient_total_w += w2;
 
        float d3 = max(0.01, length(gradient_uv - gradient_p[3]));
        float w3 = 1.0 / pow(d3, 1.0);
        gradient_interp_c += w3 * colorFour;
        gradient_total_w += w3;
 
        gradient_interp_c = pow(gradient_interp_c / gradient_total_w, vec4(1.0 / 2.2));
        return gradient_interp_c;
    }
 
    float sdf_circle(vec2 uv, vec2 centerPos, float radius)
    {
        return length(uv - centerPos) - radius;
    }
 
    half4 main(vec2 fragCoord) {
        vec2 frag_pos = fragCoord.xy / iResolution.xy * 2.0 - 1.0;
        float radius = length(frag_pos);
        vec4 gradient_color = color_gradient(frag_pos, radius, progress);
        return gradient_color;
    }
    )");

const std::string CIRCLE_FLOWLIGHT_SHADER_WITH_MASK(
    R"(
    uniform shader maskImageShader;
    uniform half2 iResolution;
    uniform half4 color[4];
    uniform half4 rotationFrequency;
    uniform half4 rotationAmplitude;
    uniform half4 rotationSeed;
    uniform half4 gradientX;
    uniform half4 gradientY;
    uniform half progress;
 
    float rand_wave(float frequency, float amplitude, float time, float seed)
    {
        return amplitude * sin(dot(vec2(time, seed), vec2(frequency, 78.233)));
    }
 
    vec4 color_gradient(vec2 frag_pos, float radius, float timeValue)
    {
        vec4 colorOne= color[0];
        vec4 colorTwo = color[1];
        vec4 colorThree = color[2];
        vec4 colorFour = color[3];
 
        if (radius >= 1.0) {return vec4(0.0);}
 
        float freq_time = -2.0 * timeValue;
        mat2 rotation_matrix = mat2(cos(freq_time), sin(freq_time), -sin(freq_time), cos(freq_time));
        vec2 gradient_p[4];
 
        float wave = rand_wave(rotationFrequency.x, rotationAmplitude.x, timeValue, rotationSeed.x);
        vec2 gradient_pos = rotation_matrix * (vec2(gradientX.x, gradientY.x) * 2.0 - 1.0);
        gradient_p[0] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.y, rotationAmplitude.y, timeValue, rotationSeed.y);
        gradient_pos = rotation_matrix * (vec2(gradientX.y, gradientY.y) * 2.0 - 1.0);
        gradient_p[1] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.z, rotationAmplitude.z, timeValue, rotationSeed.z);
        gradient_pos = rotation_matrix * (vec2(gradientX.z, gradientY.z) * 2.0 - 1.0);
        gradient_p[2] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        wave = rand_wave(rotationFrequency.w, rotationAmplitude.w, timeValue, rotationSeed.w);
        gradient_pos = rotation_matrix * (vec2(gradientX.w, gradientY.w) * 2.0 - 1.0);
        gradient_p[3] = (gradient_pos + wave * normalize(gradient_pos)) * 0.5 + 0.5;
 
        float distorted_radius = pow(radius, 4.0);
        vec2 frag_pos_new = frag_pos / radius * distorted_radius;
        vec2 gradient_uv = frag_pos_new * 0.5 + 0.5;
        float gradient_total_w = 0.0;
        vec4 gradient_interp_c = vec4(0.0);
 
        float d0 = max(0.01, length(gradient_uv - gradient_p[0]));
        float w0 = 1.0 / pow(d0, 1.0);
        gradient_interp_c += w0 * colorOne;
        gradient_total_w += w0;
 
        float d1 = max(0.01, length(gradient_uv - gradient_p[1]));
        float w1 = 1.0 / pow(d1, 1.0);
        gradient_interp_c += w1 * colorTwo;
        gradient_total_w += w1;
 
        float d2 = max(0.01, length(gradient_uv - gradient_p[2]));
        float w2 = 1.0 / pow(d2, 1.0);
        gradient_interp_c += w2 * colorThree;
        gradient_total_w += w2;
 
        float d3 = max(0.01, length(gradient_uv - gradient_p[3]));
        float w3 = 1.0 / pow(d3, 1.0);
        gradient_interp_c += w3 * colorFour;
        gradient_total_w += w3;
 
        gradient_interp_c = pow(gradient_interp_c / gradient_total_w, vec4(1.0 / 2.2));
        return gradient_interp_c;
    }
 
    float sdf_circle(vec2 uv, vec2 centerPos, float radius)
    {
        return length(uv - centerPos) - radius;
    }

    half4 main(vec2 fragCoord) {
        half maskValue = maskImageShader.eval(fragCoord).a;
        if (maskValue < 0.0) {
            return half4(0.0);
        }
 
        vec2 frag_pos = fragCoord.xy / iResolution.xy * 2.0 - 1.0;
        float radius = length(frag_pos);
        vec4 gradient_color = color_gradient(frag_pos, radius, progress);
        
        gradient_color *= maskValue;
        return gradient_color;
    }
    )");
} // namespace

GECircleFlowlightEffect::GECircleFlowlightEffect(Drawing::GECircleFlowlightEffectParams& param)
{
    for (int i = 0; i < ARRAY_NUM; i++) {
        colors_[i * COLOR_CHANNEL] = param.colors[i].x_;
        colors_[i * COLOR_CHANNEL + 1] = param.colors[i].y_;
        colors_[i * COLOR_CHANNEL + 2] = param.colors[i].z_; // 2: loop index
        colors_[i * COLOR_CHANNEL + 3] = param.colors[i].w_; // 3: loop index
    }
    rotationFrequency_ = param.rotationFrequency;
    rotationAmplitude_ = param.rotationAmplitude;
    rotationSeed_ = param.rotationSeed;
    gradientX_ = param.gradientX;
    gradientY_ = param.gradientY;
    progress_ = param.progress;
    mask_ = param.mask;
}

void GECircleFlowlightEffect::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    return ;
}

void GECircleFlowlightEffect::MakeCircleFlowlightEffect()
{
    if (g_circleFlowlightEffect != nullptr) {
        return;
    }
    g_circleFlowlightEffect = Drawing::RuntimeEffect::CreateForShader(CIRCLE_FLOWLIGHT_SHADER);
    if (g_circleFlowlightEffect == nullptr) {
        LOGE("GECircleFlowlightEffect::RuntimeShader circleFlowlightEffect create failed");
    }
}

void GECircleFlowlightEffect::MakeCircleFlowlightEffectWithMask()
{
    if (g_circleFlowlightEffectWithMask != nullptr) {
        return;
    }
    g_circleFlowlightEffectWithMask =
        Drawing::RuntimeEffect::CreateForShader(CIRCLE_FLOWLIGHT_SHADER_WITH_MASK);
    if (g_circleFlowlightEffectWithMask == nullptr) {
        LOGE("GECircleFlowlightEffect::RuntimeShader circleFlowlightEffectWithMask create failed");
    }
}

bool GECircleFlowlightEffect::IsValidParam(float width, float height)
{
    if (width < 1e-6 || height < 1e-6) {
        LOGE("GECircleFlowlightEffect::MakeDrawingShader width or height less than 1e-6");
        return false;
    }
   
    if (mask_) {
        MakeCircleFlowlightEffectWithMask();
        if (!g_circleFlowlightEffectWithMask) {
            LOGE("GECircleFlowlightEffect::MakeDrawingShader fail to create circleFlowlightEffectWithMask with Mask");
            return false;
        }
    } else {
        MakeCircleFlowlightEffect();
        if (!g_circleFlowlightEffect) {
            LOGE("GECircleFlowlightEffect::MakeDrawingShader fail to create circleFlowlightEffect");
            return false;
        }
    }
    return true;
}

void GECircleFlowlightEffect::SetUniform(float width, float height)
{
    if (mask_) {
        builder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(g_circleFlowlightEffectWithMask);
    } else {
        builder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(g_circleFlowlightEffect);
    }

    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("color", colors_.data(), COLOR_CHANNEL * ARRAY_NUM);
    builder_->SetUniform("rotationFrequency", rotationFrequency_.GetData(), ARRAY_NUM);
    builder_->SetUniform("rotationAmplitude", rotationAmplitude_.GetData(), ARRAY_NUM);
    builder_->SetUniform("rotationSeed", rotationSeed_.GetData(), ARRAY_NUM);
    builder_->SetUniform("gradientX", gradientX_.GetData(), ARRAY_NUM);
    builder_->SetUniform("gradientY", gradientY_.GetData(), ARRAY_NUM);
    builder_->SetUniform("progress", progress_);
}

void GECircleFlowlightEffect::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    if (!IsValidParam(width, height)) {
        drShader_ = nullptr;
        return;
    }

    SetUniform(width, height);

    if (mask_) {
        auto maskImageShader_ = mask_->GenerateDrawingShader(width, height);
        if (!maskImageShader_) {
            LOGE("GECircleFlowlightEffect::MakeDrawingShader maskImageShader_ is null");
            drShader_ = nullptr;
            return;
        }
        builder_->SetChild("maskImageShader", maskImageShader_);
    }
    auto circleFlowlightShader = builder_->MakeShader(nullptr, false);
    if (circleFlowlightShader == nullptr) {
        LOGE("GECircleFlowlightEffect::MakeDrawingShader circleFlowlightShader is nullptr.");
        drShader_ = nullptr;
        return;
    }
    drShader_ = circleFlowlightShader;
}

} // namespace Rosen
} // namespace OHOS