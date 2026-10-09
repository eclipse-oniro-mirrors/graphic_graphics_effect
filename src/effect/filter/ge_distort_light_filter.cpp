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

#include "ge_distort_light_filter.h"

#include <algorithm>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr char G_SHADER_STRING_DISTORT_LIGHT[] = R"(
    uniform shader imageShader;
    uniform shader maskShader;
    uniform float lightFactor;
    uniform float highLightFactor;
    uniform float highLightThreshold;
    uniform vec4 highLightColor;
    uniform float highLightSoft;

    half4 main(float2 fragCoord) {
        half maskAlpha = maskShader.eval(fragCoord).a;
        half4 finalColor = imageShader.eval(fragCoord).rgba;
        half3 highlightRGB = highLightColor.rgb * highLightColor.a;

        // base Light
        finalColor.rgb += highlightRGB * maskAlpha * lightFactor;
        // soft highLight
        half centAlpha = smoothstep(highLightThreshold, highLightThreshold + highLightSoft, maskAlpha);
        finalColor.rgb += highlightRGB * centAlpha * highLightFactor;

        return finalColor;
    }
)";
} // namespace

GEDistortLightFilter::GEDistortLightFilter(const Drawing::GEDistortLightFilterParams& params)
    : distortMask_(params.distortMask), lightFactor_(std::clamp(params.lightFactor, 0.0f, 1.0f)),
      highLightFactor_(std::clamp(params.highLightFactor, 0.0f, 1.0f)),
      highLightWidth_(std::clamp(params.highLightWidth, 0.0f, 1.0f)),
      highLightSoft_(std::clamp(params.highLightSoft, 0.0f, 1.0f)), highLightColor_(params.highLightColor)
{}

std::shared_ptr<Drawing::Image> GEDistortLightFilter::MakeDistortLightImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::ImageInfo& imageInfo,
    const std::shared_ptr<Drawing::RuntimeEffect> shader)
{
    Drawing::Matrix matrix = canvasInfo_.mat;
    matrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    Drawing::Matrix invertMatrix;
    if (!matrix.Invert(invertMatrix)) {
        LOGE("GEDistortLightFilter::MakeDistortLightImage Invert matrix failed");
        return nullptr;
    }

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertMatrix);
    if (!imageShader) {
        LOGE("GEDistortLightFilter::MakeDistortLightImage create imageShader failed");
        return nullptr;
    }

    auto maskShader = distortMask_->GenerateDrawingShader(canvasInfo_.geoWidth, canvasInfo_.geoHeight);
    if (!maskShader) {
        LOGE("GEDistortLightFilter::MakeDistortLightImage mask generate failed");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder builder(shader);
    builder.SetChild("imageShader", imageShader);
    builder.SetChild("maskShader", maskShader);
    builder.SetUniform("lightFactor", lightFactor_);
    builder.SetUniform("highLightFactor", highLightFactor_);
    builder.SetUniform("highLightThreshold", 1.0f - highLightWidth_);
    builder.SetUniform("highLightSoft", highLightSoft_);
    builder.SetUniformVec4("highLightColor", highLightColor_.redF_, highLightColor_.greenF_, highLightColor_.blueF_,
        highLightColor_.alphaF_);

#ifdef RS_ENABLE_GPU
    return builder.MakeImage(canvas.GetGPUContext().get(), &matrix, imageInfo, false);
#else
    return builder.MakeImage(nullptr, &matrix, imageInfo, false);
#endif
}

std::shared_ptr<Drawing::Image> GEDistortLightFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || distortMask_ == nullptr) {
        LOGE("GEDistortLightFilter::OnProcessImage invalid input");
        return image;
    }

    auto imageInfo = image->GetImageInfo();
    if (imageInfo.GetHeight() < 1e-6 || imageInfo.GetWidth() < 1e-6) {
        LOGE("GEDistortLightFilter::OnProcessImage invalid image size");
        return nullptr;
    }

    static const std::shared_ptr<Drawing::RuntimeEffect> shader =
        GECreateRuntimeEffectForShader(G_SHADER_STRING_DISTORT_LIGHT);
    if (shader == nullptr) {
        LOGE("GEDistortLightFilter::OnProcessImage shader init failed");
        return image;
    }

    auto resultImage = MakeDistortLightImage(canvas, image, imageInfo, shader);
    if (!resultImage) {
        LOGE("GEDistortLightFilter::OnProcessImage make image failed");
        return image;
    }

    return resultImage;
}

} // namespace Rosen
} // namespace OHOS
