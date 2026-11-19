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
#include "ge_binarization_shader_filter.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_common.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {

GEBinarizationShaderFilter::GEBinarizationShaderFilter(const Drawing::GEBinarizationShaderFilterParams& params)
    : low_(params.low), high_(params.high), threshold_(params.threshold), thresholdRange_(params.thresholdRange)
{
    InitBinarizationEffect();
}

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_binarizationEffect;

std::shared_ptr<Drawing::Image> GEBinarizationShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEBinarizationShaderFilter::OnProcessImage image is null");
        return image;
    }
    if (!GE_GE(low_, 0.0f) || !GE_LE(low_, 1.0f) ||
        !GE_GE(high_, 0.0f) || !GE_LE(high_, 1.0f) ||
        !GE_GE(threshold_, 0.0f) || !GE_LE(threshold_, 1.0f) ||
        !GE_GE(thresholdRange_, 0.0f) || !GE_LE(thresholdRange_, 1.0f)) {
        LOGE("GEBinarizationShaderFilter::OnProcessImage invalid param");
        return image;
    }
    if (!g_binarizationEffect) {
        LOGE("GEBinarizationShaderFilter::binarizationEffect is null");
        return image;
    }
    Drawing::RuntimeShaderBuilder builder(g_binarizationEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float thresholdLow = threshold_ - thresholdRange_;
    float thresholdHigh = threshold_ + thresholdRange_;
    thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
    builder.SetChild("imageShader", imageShader);
    builder.SetUniform("ubo_low", low_);
    builder.SetUniform("ubo_high", high_);
    builder.SetUniform("ubo_thresholdLow", thresholdLow);
    builder.SetUniform("ubo_thresholdHigh", thresholdHigh);
#ifdef RS_ENABLE_GPU
    auto outImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
#else
    auto outImage = builder.MakeImage(nullptr, nullptr, image->GetImageInfo(), false);
#endif
    if (outImage == nullptr) {
        LOGE("GEBinarizationShaderFilter::OnProcessImage MakeImage failed");
        return image;
    }
    return outImage;
}

bool GEBinarizationShaderFilter::InitBinarizationEffect()
{
    if (g_binarizationEffect != nullptr) {
        return true;
    }
    static std::string binarizationString(R"(
        uniform mediump float ubo_low;
        uniform mediump float ubo_high;
        uniform mediump float ubo_thresholdLow;
        uniform mediump float ubo_thresholdHigh;
        uniform shader imageShader;
        mediump vec4 main(vec2 drawing_coord) {
            mediump vec3 c = imageShader(drawing_coord).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(ubo_high, -1.0, step(ubo_thresholdLow, gray));
            float highRes = mix(-1.0, ubo_low, step(ubo_thresholdHigh, gray));
            float midRes = (ubo_thresholdHigh - gray) * (ubo_high - ubo_low) /
            (ubo_thresholdHigh - ubo_thresholdLow) + ubo_low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            mediump vec3 invert = vec3(invertedGray);
            mediump vec4 res = vec4(invert, 1.0);
            return res;
        }
    )");
    g_binarizationEffect = GECreateRuntimeEffectForShader(binarizationString);
    if (g_binarizationEffect == nullptr) {
        LOGE("GEBinarizationShaderFilter::InitBinarizationEffect effect create failed");
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
