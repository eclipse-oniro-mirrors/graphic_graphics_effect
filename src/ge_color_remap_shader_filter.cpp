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

#include <algorithm>
#include <cmath>

#include "ge_color_remap_shader_filter.h"
#include "ge_log.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static uint8_t REMAP_COLOR_CHANNEL = 3; // 3 len of rgb
constexpr static uint8_t COLOR_ARRAY_SIZE = 5;  // 5 len of array
} // namespace

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_colorRemapShaderEffect_ = nullptr;

GEColorRemapShaderFilter::GEColorRemapShaderFilter(
    const Drawing::GEColorRemapFilterParams &params) : params_(params) {}

static const std::string colorRemapEffect(R"(
    uniform shader ImageShader;
    uniform vec3 colors[5];
    uniform float positions[5];
    uniform vec3 tailColor;
    uniform float tailPosition;

    float Grayscale(vec4 color) {
        return dot(color.rgb, vec3(0.299, 0.587, 0.114));
    }

    vec4 ColorRemapFilter(float grayColor, vec3 cs[5], float ps[5], vec3 tailC, float tailP) {
        float gray = clamp(grayColor, 0.0, 1.0);
        vec3 color = cs[0];

        if (gray >= ps[0] && gray < ps[1]) {
            float t = clamp((gray-ps[0]) / (ps[1]-ps[0]), 0.0, 1.0);
            color = mix(cs[0], cs[1], t);
            return vec4(color, 1.0);
        }
        if (gray >= ps[1] && gray < ps[2]) {
            float t = clamp((gray-ps[1]) / (ps[2]-ps[1]), 0.0, 1.0);
            color = mix(cs[1], cs[2], t);
            return vec4(color, 1.0);
        }
        if (gray >= ps[2] && gray < ps[3]) {
            float t = clamp((gray-ps[2]) / (ps[3]-ps[2]), 0.0, 1.0);
            color = mix(cs[2], cs[3], t);
            return vec4(color, 1.0);
        }
        if (gray >= ps[3] && gray < ps[4]) {
            float t = clamp((gray-ps[3]) / (ps[4]-ps[3]), 0.0, 1.0);
            color = mix(cs[3], cs[4], t);
            return vec4(color, 1.0);
        }

        if (gray >= tailP) {
            float d = ps[0] + 1.0 - tailP;
            float t = d <= 0.0 ? 0.0 : clamp((gray - tailP) / d, 0.0, 1.0);
            color = mix(tailC, cs[0], t);
        }
        if (gray < ps[0]) {
            float t = clamp((1.0 - tailP + gray) / (1.0 - tailP + ps[0]), 0.0, 1.0);
            color = mix(tailC, vec3(cs[0]), t);
        }
        return vec4(color, 1.0);
    }

    half4 main(vec2 fragcoord) {
        vec4 blurredColor = ImageShader.eval(fragcoord).rgba;
        float pass1Result = Grayscale(blurredColor);

        vec4 finalColor = ColorRemapFilter(pass1Result, colors, positions, tailColor, tailPosition);
        return half4(finalColor);
    }
)");

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEColorRemapShaderFilter::MakeColorRemapBuilder()
{
    if (g_colorRemapShaderEffect_ == nullptr) {
        g_colorRemapShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(colorRemapEffect);
        if (g_colorRemapShaderEffect_ == nullptr) {
            LOGE("MakeColorRemapBuilder effect error\n");
            return nullptr;
        }
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_colorRemapShaderEffect_);
}

bool GEColorRemapShaderFilter::CheckInColorRemapParams(float* color, float* position)
{
    if (params_.colors.empty() || params_.positions.empty()) {
        LOGE("updateColorParams colors or positions is invalid");
        return false;
    }

    size_t n = std::min(params_.positions.size(), params_.colors.size());
    n = n > COLOR_ARRAY_SIZE ? COLOR_ARRAY_SIZE : n;
    for (size_t i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (params_.positions[j] > params_.positions[j + 1]) {
                std::swap(params_.positions[j], params_.positions[j + 1]);
                std::swap(params_.colors[j], params_.colors[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }

    for (size_t i = 0; i < n; i++) {
        color[i * REMAP_COLOR_CHANNEL + 0] = params_.colors[i][0];   // 0 red
        color[i * REMAP_COLOR_CHANNEL + 1] = params_.colors[i][1];   // 1 green
        color[i * REMAP_COLOR_CHANNEL + 2] = params_.colors[i][2];   // 2 blur
        position[i] = params_.positions[i];   // 0 x
    }
    tailColor_ = params_.colors[n - 1];
    tailPosition_ = params_.positions[n - 1];
    return true;
}

std::shared_ptr<Drawing::Image> GEColorRemapShaderFilter::OnProcessImage(
    Drawing::Canvas &canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect &src, const Drawing::Rect &dst)
{
    if (image == nullptr || image->GetWidth() < 1e-6 || image->GetHeight() < 1e-6) {
        LOGE("GEColorRemapShaderFilter input is invalid.");
        return nullptr;
    }

    float color[COLOR_ARRAY_SIZE * REMAP_COLOR_CHANNEL] = {0.0}; // 0.0 default
    float position[COLOR_ARRAY_SIZE] = {0.0}; // 0.0 default
    if (!CheckInColorRemapParams(color, position)) {
        LOGE("GEColorRemapShaderFilter CheckInColorRemapParams failed");
        return image;
    };

    Drawing::Matrix matrix;
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (srcImageShader == nullptr) {
        LOGE("GEColorRemapShaderFilter srcImageShader is null");
        return image;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = MakeColorRemapBuilder();
    if (!builder) {
        LOGE("GEColorRemapShaderFilter MakeColorRemapBuilder builder error\n");
        return image;
    }

    builder->SetChild("ImageShader", srcImageShader);
    builder->SetUniform("colors", color, COLOR_ARRAY_SIZE * REMAP_COLOR_CHANNEL);
    builder->SetUniform("positions", position, COLOR_ARRAY_SIZE);
    // index of tailColor_ is 0 red,0 red, 1 green, 2 blue
    builder->SetUniform("tailColor", tailColor_[0], tailColor_[1], tailColor_[2]);
    builder->SetUniform("tailPosition", tailPosition_);

    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEColorRemapShaderFilter resultImage is null");
        return image;
    }
    return resultImage;
}

} // namespace Rosen
} // namespace OHOS
