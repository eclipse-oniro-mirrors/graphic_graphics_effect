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
#include "ge_sound_wave_filter.h"
 
namespace OHOS {
namespace Rosen {

namespace {
constexpr static uint8_t COLOR_CHANNEL = 3; // 3 len of rgb
} // namespace

GESoundWaveFilter::GESoundWaveFilter(const Drawing::GESoundWaveFilterParams& params)
    :colorProgress_(params.colorProgress), centerBrightness_(params.centerBrightness),
    soundIntensity_(params.soundIntensity), shockWaveAlphaA_(params.shockWaveAlphaA),
    shockWaveAlphaB_(params.shockWaveAlphaB), shockWaveProgressA_(params.shockWaveProgressA),
    shockWaveProgressB_(params.shockWaveProgressB)
{
    colorA_ = Drawing::Color(params.colorA);
    colorB_ = Drawing::Color(params.colorB);
    colorC_ = Drawing::Color(params.colorC);
}

 
std::shared_ptr<Drawing::Image> GESoundWaveFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GESoundWaveFilter::ProcessImage input is invalid");
        return nullptr;
    }
 
    Drawing::Matrix matrix;
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }
    auto soundWaveShader = GetSoundWaveEffect();
    if (soundWaveShader == nullptr) {
        LOGE("GESoundWaveFilter::ProcessImage g_SoundWaveEffect init failed");
        return nullptr;
    }

    float colorA[COLOR_CHANNEL] = {colorA_.GetRedF(), colorA_.GetGreenF(), colorA_.GetBlueF()};
    float colorB[COLOR_CHANNEL] = {colorB_.GetRedF(), colorB_.GetGreenF(), colorB_.GetBlueF()};
    float colorC[COLOR_CHANNEL] = {colorC_.GetRedF(), colorC_.GetGreenF(), colorC_.GetBlueF()};

    Drawing::RuntimeShaderBuilder builder(soundWaveShader);
    builder.SetChild("image", shader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("colorA", colorA, COLOR_CHANNEL);
    builder.SetUniform("colorB", colorB, COLOR_CHANNEL);
    builder.SetUniform("colorC", colorC, COLOR_CHANNEL);
    builder.SetUniform("colorProgress", colorProgress_);
    builder.SetUniform("centerBrightness", centerBrightness_);
    builder.SetUniform("soundIntensity", soundIntensity_);
    builder.SetUniform("shockWaveAlphaA", shockWaveAlphaA_);
    builder.SetUniform("shockWaveAlphaB", shockWaveAlphaB_);
    builder.SetUniform("shockWaveProgressA", shockWaveProgressA_);
    builder.SetUniform("shockWaveProgressB", shockWaveProgressB_);
 
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (invertedImage == nullptr) {
        LOGE("GESoundWaveFilter::ProcessImage make image failed");
        return nullptr;
    }
    return invertedImage;
}

std::shared_ptr<Drawing::RuntimeEffect> GESoundWaveFilter::GetSoundWaveEffect()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_soundWaveShader = nullptr;
    if (g_soundWaveShader == nullptr) {
        g_soundWaveShader = Drawing::RuntimeEffect::CreateForShader(shaderStringSoundWave);
    }
    return g_soundWaveShader;
}


} // namespace Rosen
} // namespace OHOS