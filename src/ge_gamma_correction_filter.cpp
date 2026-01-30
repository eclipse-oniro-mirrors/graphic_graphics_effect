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

#include "ge_gamma_correction_filter.h"
#include "ge_log.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr static float MIN_GAMMA_VALUE = 1e-6;
} // namespace

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_gammaCorrectionShaderEffect_ = nullptr;

GEGammaCorrectionFilter::GEGammaCorrectionFilter(const float gamma) : gamma_(gamma) {}

static const std::string gammaCorrectionEffect(R"(
    uniform shader ImageShader;
    uniform float gamma;

    half4 main(vec2 fragcoord) {
        vec4 bk = ImageShader.eval(fragcoord).rgba;
        bk.rgb = pow(bk.rgb, vec3(1.0 / gamma));
        return half4(bk);
    }
)");

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEGammaCorrectionFilter::MakeGammaCorrectionBuilder()
{
    if (g_gammaCorrectionShaderEffect_ == nullptr) {
        g_gammaCorrectionShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(gammaCorrectionEffect);
        if (g_gammaCorrectionShaderEffect_ == nullptr) {
            LOGE("MakeGammaCorrectionBuilder effect error\n");
            return nullptr;
        }
    }

    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_gammaCorrectionShaderEffect_);
}

std::shared_ptr<Drawing::Image> GEGammaCorrectionFilter::OnProcessImage(
    Drawing::Canvas &canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect &src, const Drawing::Rect &dst)
{
    if (image == nullptr || image->GetWidth() < 1e-6 || image->GetHeight() < 1e-6) {
        LOGE("GEXGammaCorrectionFilter input is invalid.");
        return nullptr;
    }

    if (gamma_ < MIN_GAMMA_VALUE) {
        LOGE("GEXGammaCorrectionFilter gamma value is invalid");
        return image;
    };

    Drawing::Matrix matrix;
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (srcImageShader == nullptr) {
        LOGE("GEXGammaCorrectionFilter srcImageShader is null");
        return image;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = MakeGammaCorrectionBuilder();
    if (!builder) {
        LOGE("GEXGammaCorrectionFilter MakeGammaCorrectionBuilder builder error\n");
        return image;
    }

    builder->SetChild("ImageShader", srcImageShader);
    builder->SetUniform("gamma", gamma_);

    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEXGammaCorrectionFilter resultImage is null");
        return image;
    }
    return resultImage;
}

} // namespace Rosen
} // namespace OHOS
