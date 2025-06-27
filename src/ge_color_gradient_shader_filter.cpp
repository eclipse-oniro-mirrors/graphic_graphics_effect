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
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_maskColorGradientShaderEffect_ = nullptr;

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

    float color[ARRAY_SIZE * COLOR_CHANNEL] = {0.0}; // 0.0 default
    float position[ARRAY_SIZE * POSITION_CHANNEL] = {0.0}; // 0.0 default
    float strength[ARRAY_SIZE] = {0.0}; // 0.0 default
    if (!CheckInParams(color, position, strength, ARRAY_SIZE)) { return image; }

    Drawing::Matrix matrix = canvasInfo_.mat_;
    matrix.PostTranslate(-canvasInfo_.tranX_, -canvasInfo_.tranY_);
    Drawing::Matrix invertMatrix;
    matrix.Invert(invertMatrix);
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertMatrix);
    if (srcImageShader == nullptr) {
        LOGE("GEColorGradientShaderFilter::ProcessImage srcImageShader is null");
        return image;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        PreProcessColorGradientBuilder(canvasInfo_.geoWidth_, canvasInfo_.geoHeight_);
    if (!builder) {
        LOGE("GEColorGradientShaderFilter::ProcessImage mask builder error\n");
        return image;
    }
    
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetUniform("iResolution", canvasInfo_.geoWidth_, canvasInfo_.geoHeight_);
    builder->SetUniform("color", color, ARRAY_SIZE * COLOR_CHANNEL);
    builder->SetUniform("position", position, ARRAY_SIZE * POSITION_CHANNEL);
    builder->SetUniform("strength", strength, ARRAY_SIZE);
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), &(matrix), image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEColorGradientShaderFilter::ProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

bool GEColorGradientShaderFilter::CheckInParams(float* color, float* position, float* strength, int tupleSize)
{
    if (strengths_.size() <= 0 || strengths_.size() > ARRAY_SIZE ||
        strengths_.size() * COLOR_CHANNEL != colors_.size() ||
        strengths_.size() * POSITION_CHANNEL != positions_.size()) {
        LOGE("GEColorGradientShaderFilter::CheckInParams param size error\n");
        return false;
    }

    int arraySize = static_cast<int>(strengths_.size());
    if (!color || !position || !strength || tupleSize < arraySize) {
        LOGE("GEColorGradientShaderFilter::CheckInParams array size error\n");
        return false;
    }

    for (int i = 0; i < arraySize; i++) {
        color[i * COLOR_CHANNEL + 0] = colors_[i * COLOR_CHANNEL + 0];   // 0 red
        color[i * COLOR_CHANNEL + 1] = colors_[i * COLOR_CHANNEL + 1];   // 1 green
        color[i * COLOR_CHANNEL + 2] = colors_[i * COLOR_CHANNEL + 2];   // 2 blur
        color[i * COLOR_CHANNEL + 3] = colors_[i * COLOR_CHANNEL + 3];   // 3 alpha

        position[i * POSITION_CHANNEL + 0] = positions_[i * POSITION_CHANNEL + 0];   // 0 x
        position[i * POSITION_CHANNEL + 1] = positions_[i * POSITION_CHANNEL + 1];   // 1 y

        strength[i] = strengths_[i];
    }

    return true;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEColorGradientShaderFilter::MakeColorGradientBuilder()
{
    if (g_colorGradientShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader srcImageShader;
            uniform float2 iResolution;
            uniform vec4 color[12];
            uniform vec2 position[12];
            uniform float strength[12];

            const float epsilon = 0.001;

            float blendMultipleColorsByDistance(vec2 uv, vec2 positions, float strength)
            {
                positions.x *= iResolution.x / iResolution.y;
                float dist = length(uv - positions) + epsilon;
                float weight = strength / pow(dist, 2.0);
                return weight;
            }

            vec4 main(float2 fragCoord)
            {
                vec2 uv = fragCoord / iResolution.xy;
                float screenRatio = iResolution.x / iResolution.y;
                uv.x *= screenRatio;
                float totalWeight = 0.0;
                vec4 blendColor = vec4(0.0);
                for (int i = 0; i < 12; i++) {
                    float colorSphereWeight = blendMultipleColorsByDistance(uv, position[i], strength[i]);
                    totalWeight += colorSphereWeight;
                    blendColor += color[i] * colorSphereWeight;
                }

                vec4 finalColor = vec4(blendColor / totalWeight);
                finalColor.rgb = mix(srcImageShader.eval(fragCoord).rgb, finalColor.rgb, finalColor.a);
                return vec4(finalColor.rgb, 1.0);
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

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEColorGradientShaderFilter::MakeMaskColorGradientBuilder()
{
    if (g_maskColorGradientShaderEffect_ == nullptr) {
        static constexpr char withMaskProg[] = R"(
            uniform shader srcImageShader;
            uniform shader maskImageShader;
            uniform float2 iResolution;
            uniform vec4 color[12];
            uniform vec2 position[12];
            uniform float strength[12];

            const float epsilon = 0.001;

            float blendMultipleColorsByDistance(vec2 uv, vec2 positions, float strength)
            {
                positions.x *= iResolution.x / iResolution.y;
                float dist = length(uv - positions) + epsilon;
                float weight = strength / pow(dist, 2.0);
                return weight;
            }

            vec4 main(float2 fragCoord)
            {
                vec2 uv = fragCoord / iResolution.xy;
                float screenRatio = iResolution.x / iResolution.y;
                uv.x *= screenRatio;
                float totalWeight = 0.0;
                vec4 blendColor = vec4(0.0);
                for (int i = 0; i < 12; i++) {
                    float colorSphereWeight = blendMultipleColorsByDistance(uv, position[i], strength[i]);
                    totalWeight += colorSphereWeight;
                    blendColor += color[i] * colorSphereWeight;
                }

                vec4 finalColor = vec4(blendColor / totalWeight) * maskImageShader.eval(fragCoord).a;
                finalColor.rgb = mix(srcImageShader.eval(fragCoord).rgb, finalColor.rgb, finalColor.a);
                return vec4(finalColor.rgb, 1.0);
            }
        )";

        g_maskColorGradientShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(withMaskProg);
        if (g_maskColorGradientShaderEffect_ == nullptr) {
            LOGD("GEColorGradientShaderFilter::MakeMaskColorGradientBuilder effect error\n");
            return nullptr;
        }
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_maskColorGradientShaderEffect_);
}

std::string GEColorGradientShaderFilter::GetDescription()
{
    return "GEColorGradientShaderFilter";
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEColorGradientShaderFilter::PreProcessColorGradientBuilder(
    float geoWidth, float geoHeight)
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (mask_) {
        builder = MakeMaskColorGradientBuilder();
        if (!builder) {
            LOGE("GEColorGradientShaderFilter::PreProcessColorGradientBuilder mask builder error\n");
            return nullptr;
        }
        auto maskImageShader = mask_->GenerateDrawingShader(geoWidth, geoHeight);
        if (!maskImageShader) {
            LOGE("GEColorGradientShaderFilter::PreProcessColorGradientBuilder maskImageShader is null");
            return nullptr;
        }
        builder->SetChild("maskImageShader", maskImageShader);
    } else {
        builder = MakeColorGradientBuilder();
        if (!builder) {
            LOGE("GEColorGradientShaderFilter::PreProcessColorGradientBuilder builder error\n");
            return nullptr;
        }
    }
    return builder;
}

} // namespace Rosen
} // namespace OHOS