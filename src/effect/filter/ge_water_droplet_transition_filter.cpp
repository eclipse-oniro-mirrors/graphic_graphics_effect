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
 
#include "ge_water_droplet_transition_filter.h"
#include "ge_log.h"
 
namespace OHOS {
namespace Rosen {
namespace {
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_waterDropletShaderEffect = nullptr;
 
constexpr static char WATER_DROPLET_TRANSITION_SHADER[] = R"(
    uniform shader bottomLayer;
    uniform shader topLayer;
    uniform vec2 iResolution;
    uniform float inverseFlag;
    uniform float radius;
    uniform float progress;
    uniform float transitionFadeWidth;
    uniform float distortionIntensity;
    uniform float distortionThickness;
    uniform float lightStrength;
    uniform float lightSoftness;
    uniform float noiseScaleX;
    uniform float noiseScaleY;
    uniform float noiseStrengthX;
    uniform float noiseStrengthY;
 
    float Rand(float x)
    {
        return fract(sin(x) * 43758.5453123);
    }
 
    float Noise(float x)
    {
        float i = floor(x);
        float f = fract(x);
        float u = f * f * (3.0 - 2.0 * f);
        return mix(Rand(i), Rand(i + 1.0), u);
    }

    float AlienShapeSDF(vec2 st)
    {
        float noiseValueX = Noise(st.x * noiseScaleX - sin(st.y) * 2.0 +
            sin(progress * 0.25) * 2.0 * radius) * noiseStrengthX;
        float noiseValueY = Noise(st.y * noiseScaleY - sin(st.x) * 2.0 +
            cos(progress * 0.3) * 2.0 * radius) * noiseStrengthY;
 
        vec2 perturbedSt = st - vec2(noiseValueX, noiseValueY);
 
        float perturbedDistance = length(perturbedSt) - radius;
 
        return perturbedDistance;
    }
 
    float EdgeSmoothing(vec2 st, float sdf, float aaThreshold)
    {
        float innerEdge = smoothstep(0.0, -aaThreshold, sdf);
        float outerEdge = smoothstep(aaThreshold, 0.0, sdf);
        return (1.0 - innerEdge) * outerEdge;
    }
 
    vec2 SDFGradient(vec2 st, float sdf, float epsilon)
    {
        float sdfX = AlienShapeSDF(st + vec2(epsilon, 0.0));
        float sdfY = AlienShapeSDF(st + vec2(0.0, epsilon));
        return vec2(sdfX - sdf, sdfY - sdf) / epsilon;
    }
 
    vec4 MaskTransiton(vec2 st, float sdf)
    {
        float alpha = smoothstep(-transitionFadeWidth, transitionFadeWidth,
            sdf + 0.3 * radius - 0.5 * radius * Noise((st.x) - sin(st.y) + sin(progress * 0.25)));
        alpha = mix(alpha, 1.0 - alpha, inverseFlag);
        vec4 topColor = topLayer.eval(st * iResolution.xy);
        vec4 bottomColor = bottomLayer.eval(st * iResolution.xy);
        vec4 finalColor = mix(bottomColor, topColor, alpha);
        return finalColor;
    }
 
    vec4 main(vec2 fragCoord)
    {
        vec2 st = (fragCoord.xy / iResolution.xy) * 2.0 - 1.0;
 
        float sdf = AlienShapeSDF(st);
        vec2 gradient = SDFGradient(st, sdf, 0.01);
        float gradientMagnitude = length(gradient);
 
        float distortStrength = EdgeSmoothing(st, sdf, distortionThickness) * 0.2 * radius +
            EdgeSmoothing(st, sdf - 1.3 * radius * Noise((st.x) - sin(st.y) * 2.0 + sin(progress * 0.25)),
            distortionThickness) * 0.15 * radius;
        vec2 backgroundCoord = st - gradient * vec2(distortStrength * distortionIntensity) *
            (smoothstep(1.5, 0.8, gradientMagnitude));
 
        vec4 backgroundColor = MaskTransiton(backgroundCoord * 0.5 + 0.5, sdf);
 
        float highlightsStrength = EdgeSmoothing(st, sdf, lightSoftness) * (radius + 0.005) * lightStrength;
        float highlight = highlightsStrength * (smoothstep(1.0, 0.6, gradientMagnitude));
 
        return backgroundColor + highlight;
    }
)";
}
 
GEWaterDropletTransitionFilter::GEWaterDropletTransitionFilter(
    const Drawing::GEWaterDropletTransitionFilterParams& params)
    : params_(params)
{}
 
std::shared_ptr<Drawing::RuntimeShaderBuilder> GEWaterDropletTransitionFilter::MakeWaterDropletTransitionEffect()
{
    if (g_waterDropletShaderEffect == nullptr) {
        g_waterDropletShaderEffect = Drawing::RuntimeEffect::CreateForShader(WATER_DROPLET_TRANSITION_SHADER);
        if (g_waterDropletShaderEffect == nullptr) {
            LOGE("GEXWaterDropletTransitionFilter::MakeWaterDropletTransitionEffect effect error.");
            return nullptr;
        }
    }
 
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_waterDropletShaderEffect);
}

bool GEWaterDropletTransitionFilter::AlignTransitionImages(const std::shared_ptr<Drawing::Image> image,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder)
{
    auto imageInfo = image->GetImageInfo();
    float imageHeight = imageInfo.GetHeight();
    float imageWidth = imageInfo.GetWidth();
    if (imageHeight < 1e-6 || imageWidth < 1e-6 || !params_.topLayer) {
        return false;
    }
    
    auto topLayerImageInfo = params_.topLayer->GetImageInfo();
    float topLayerImageWidth = topLayerImageInfo.GetWidth();
    float topLayerImageHeight = topLayerImageInfo.GetHeight();
    auto scaleWRatio = topLayerImageWidth / imageWidth;
    auto scaleHRatio = topLayerImageHeight / imageHeight;
    Drawing::Matrix topLayerMatrix = canvasInfo_.mat;
    topLayerMatrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    topLayerMatrix.PostScale(scaleWRatio, scaleHRatio);
    Drawing::Matrix invertTopLayerMatrix;
    if (!topLayerMatrix.Invert(invertTopLayerMatrix)) {
        LOGE("GEWaterDropletTransitionFilter::AlignTransitionImages invert topLayerMatrix failed.");
        return false;
    }

    auto topLayer = Drawing::ShaderEffect::CreateImageShader(*params_.topLayer, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertTopLayerMatrix);

    Drawing::Matrix imageMatrix = canvasInfo_.mat;
    imageMatrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    Drawing::Matrix imageInvertMatrix;
    if (!imageMatrix.Invert(imageInvertMatrix)) {
        return false;
    }
    auto bottomLayer = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), imageInvertMatrix);
    
    builder->SetChild("bottomLayer", bottomLayer);
    builder->SetChild("topLayer", topLayer);
    builder->SetUniform("iResolution", imageWidth, imageHeight);

    return true;
}

std::shared_ptr<Drawing::Image> GEWaterDropletTransitionFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || params_.topLayer == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = MakeWaterDropletTransitionEffect();
    if (!builder) {
        LOGE("GEWaterDropletTransitionFilter MakeWaterDropletTransitionEffect builder error.");
        return image;
    }

    if (!AlignTransitionImages(image, builder)) {
        return image;
    }

    builder->SetUniform("inverseFlag", params_.inverse);
    builder->SetUniform("progress", params_.progress);
    builder->SetUniform("radius", params_.radius);
    builder->SetUniform("transitionFadeWidth", params_.transitionFadeWidth);
    builder->SetUniform("distortionIntensity", params_.distortionIntensity);
    builder->SetUniform("distortionThickness", params_.distortionThickness);
    builder->SetUniform("lightStrength", params_.lightStrength);
    builder->SetUniform("lightSoftness", params_.lightSoftness);
    builder->SetUniform("noiseScaleX", params_.noiseScaleX);
    builder->SetUniform("noiseScaleY", params_.noiseScaleY);
    builder->SetUniform("noiseStrengthX", params_.noiseStrengthX);
    builder->SetUniform("noiseStrengthY", params_.noiseStrengthY);

    auto imageInfo = image->GetImageInfo();
    Drawing::Matrix imageMatrix = canvasInfo_.mat;
    imageMatrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
#ifdef RS_ENABLE_GPU
    auto effectImage = builder->MakeImage(canvas.GetGPUContext().get(), &imageMatrix, imageInfo, false);
#else
    auto effectImage = builder->MakeImage(nullptr, &imageMatrix, imageInfo, false);
#endif
    if (effectImage == nullptr) {
        LOGE("GEWaterDropletTransitionFilter::OnProcessImage make lightingImage failed");
        return image;
    }
    return effectImage;
}
 
} // namespace Rosen
} // namespace OHOS
