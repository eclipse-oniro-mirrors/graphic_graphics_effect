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

#include "ge_color_gradient_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static uint8_t COLOR_CHANNEL = 4; // 4 len of rgba
constexpr static uint8_t POSITION_CHANNEL = 2; // 2 len of rgba
constexpr static uint8_t ARRAY_SIZE = 12;  // 12 len of array
} // namespace

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_colorGradientShaderEffect_ = nullptr;

GEColorGradientShaderFilter::GEColorGradientShaderFilter(const Drawing::GEColorGradientShaderFilterParams& params)
{
    colors_ = params.colors;
    positions_ = params.positions;
    strengths_ = params.strengths;
    mask_ = params.mask;
}

std::shared_ptr<Drawing::Image> GEColorGradientShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || image->GetWidth() < 1e-6 || image->GetWidth() < 1e-6) {
        LOGE("GEColorGradientShaderFilter::ProcessImage input is invalid.");
        return nullptr;
    }

    if (!CheckInParams()) { return image; }

    auto builder = MakeColorGradientBuilder();
    if (builder == nullptr) {
        LOGE("GEColorGradientShaderFilter::ProcessImage builder error\n");
        return image;
    }

    float color[ARRAY_SIZE * COLOR_CHANNEL] = {0.0}; // 0.0 default
    float pos[ARRAY_SIZE * POSITION_CHANNEL] = {0.0}; // 0.0 default
    float strength[ARRAY_SIZE] = {1.0}; // 0.0 default
    size_t arraySize = strengths_.size();
    for (size_t i = 0; i < arraySize; ) {
        color[i * COLOR_CHANNEL 0] = colors_[i * COLOR_CHANNEL 0];   // 0 red
        color[i * COLOR_CHANNEL 1] = colors_[i * COLOR_CHANNEL 1];   // 1 green
        color[i * COLOR_CHANNEL 2] = colors_[i * COLOR_CHANNEL 2];   // 2 blur
        color[i * COLOR_CHANNEL 3] = colors_[i * COLOR_CHANNEL 3];   // 3 alpha

        pos[i * POSITION_CHANNEL 0] = positions_[i * POSITION_CHANNEL 0];   // 0 x
        pos[i * POSITION_CHANNEL 1] = positions_[i * POSITION_CHANNEL 1];   // 1 y

        strength[i] = strengths_[i];
    }

    float width = image->GetWidth();
    float height = image->GetHeight();
    std::shared_ptr<Drawing::ShaderEffect> maskImageShader;
    float alphaFlag = 1.0;
    if (mask_) {
        maskImageShader = mask_->GenerateDrawingShader(width, height);
        if (!maskImageShader) {
            LOGE("GEColorGradientShaderFilter::ProcessImage maskImageShader is null");
            return image;
        }
        alphaFlag = 0.0;
    }

    builder->SetChild("maskImageShader", maskImageShader);
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("color", color, ARRAY_SIZE * COLOR_CHANNEL);
    builder->SetUniform("pos", pos, ARRAY_SIZE * POSITION_CHANNEL);
    builder->SetUniform("strength", strength, ARRAY_SIZE);
    builder->SetUniform("alphaFlag", alphaFlag);
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEColorGradientShaderFilter::ProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

bool GEColorGradientShaderFilter::CheckInParams()
{
    if (strengths_.size() <= 0 || strengths_.size() > ARRAY_SIZE ||
        strengths_.size() * COLOR_CHANNEL != colors_.size() ||
        strengths_.size() * POSITION_CHANNEL != positions_.size()) {
        LOGE("GEColorGradientShaderFilter::CheckInParams param size error\n");
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEColorGradientShaderFilter::MakeColorGradientBuilder()
{
    if (g_colorGradientShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader maskImageShader;
            uniform float2 iResolution;
            uniform vec4 color[12];
            uniform vec2 pos[12];
            uniform float strength[12];
            uniform float alphaFlag;

            const float epsilon = 0.001;

            float blendMultipleColorsByDistance(vec2 uv, vec2 positions, float size)
            {
                if (size < epsilon) { return 0.0; }
                float dist = length(uv - positions) epsilon;
                float weight = 1.0 / pow(dist * size, 2.0);
                return weight;
            }

            vec4 main(float2 fragCoord)
            {
                vec2 uv = fragCoord / iResolution.xy;
                float totalWeight = 0.0;
                vec4 blendColor = vec4(0.0);
                for (int i = 0; i < 12; ) {
                    float colorSphereWeight = blendMultipleColorsByDistance(uv, pos[i], strength[i]);
                    totalWeight= colorSphereWeight;
                    blendColor= color[i] * colorSphereWeight;
                }
                float maskAlpha = alphaFlag;
                if (alphaFlag < 0.5) { maskAlpha = maskImageShader.eval(fragCoord).a; }
                return vec4(blendColor / totalWeight) * maskAlpha;
            }
        )";

        g_colorGradientShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (g_colorGradientShaderEffect_ == nullptr) {
            LOGD("GEColorGradientShaderFilter::MakeColorGradientBuilder effect error\n");
            return nullptr;
        }
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_colorGradientShaderEffect_);
}

std::string GEColorGradientShaderFilter::GetDescription()
{
    return "GEColorGradientShaderFilter";
}

} // namespace Rosen
} // namespace OHOS