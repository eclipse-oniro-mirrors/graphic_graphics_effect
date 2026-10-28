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

#include <algorithm>
#include <cmath>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {

GEBinarizationShaderFilter::GEBinarizationShaderFilter(const Drawing::GEBinarizationShaderFilterParams& params)
    : low_(params.low), high_(params.high), threshold_(params.threshold), thresholdRange_(params.thresholdRange)
{
    if (!InitBinarizationEffect()) {
        LOGE("GEBinarizationShaderFilter failed to construct when initializing BinarizationEffect.");
        return;
    }
}

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_binarizationEffect;

std::shared_ptr<Drawing::Image> GEBinarizationShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEBinarizationShaderFilter::OnProcessImage image is null");
        return image;
    }
    if (!std::isfinite(low_) || !std::isfinite(high_) ||
        !std::isfinite(threshold_) || !std::isfinite(thresholdRange_)) {
        LOGE("GEBinarizationShaderFilter::OnProcessImage param is NaN or Inf");
        return image;
    }
    low_ = std::clamp(low_, 0.0f, 1.0f);
    high_ = std::clamp(high_, 0.0f, 1.0f);
    threshold_ = std::clamp(threshold_, 0.0f, 1.0f);
    thresholdRange_ = std::clamp(thresholdRange_, 0.0f, 1.0f);
    if (!g_binarizationEffect) {
        LOGE("GEBinarizationShaderFilter::binarizationEffect is null");
        return image;
    }
    Drawing::RuntimeShaderBuilder builder(g_binarizationEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder.SetChild("imageShader", imageShader);
    builder.SetUniform("low", low_);
    builder.SetUniform("high", high_);
    builder.SetUniform("threshold", threshold_);
    builder.SetUniform("thresholdRange", thresholdRange_);
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
        uniform shader imageShader;
        uniform mediump float low;
        uniform mediump float high;
        uniform mediump float threshold;
        uniform mediump float thresholdRange;

        mediump vec4 main(vec2 drawing_coord) {
            mediump vec3 c = vec3(
                imageShader.eval(drawing_coord).r,
                imageShader.eval(drawing_coord).g,
                imageShader.eval(drawing_coord).b);
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float thresholdLow = threshold - thresholdRange;
            float thresholdHigh = threshold + thresholdRange;
            thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
            float lowRes = mix(high, -1.0, step(thresholdLow, gray));
            float highRes = mix(-1.0, low, step(thresholdHigh, gray));
            float midRes = (thresholdHigh - gray) * (high - low) / (thresholdHigh - thresholdLow) + low;
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
