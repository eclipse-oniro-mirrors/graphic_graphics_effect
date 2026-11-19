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

#include "ge_sticker_emboss_filter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "effect/runtime_effect.h"
#include "effect/shader/ge_sticker_outline_shader.h"
#include "ge_cache_helper.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {

namespace {
struct StickerSDFCache {
    uint32_t stickerUniqueID = 0;
    int stickerWidth = 0;
    int stickerHeight = 0;
    int spreadFactor = 0;
    std::shared_ptr<Drawing::Image> decodedSdfImage = nullptr;
};
auto PackStickerSDFCacheFunc = GECacheHelper::PackCacheAny<StickerSDFCache>;
auto UnpackStickerSDFCacheOrDefaultFunc = GECacheHelper::UnpackCacheAnyOrDefault<StickerSDFCache>;

constexpr float EMBOSS_SDF_MIN_RANGE_PX = 8.0f;
constexpr float EMBOSS_SDF_MIN_FALLOFF_PX = 1.0f;
constexpr float EDGE_LIGHT_FEATHER_MAX = 0.999f;

constexpr float EMBOSS_DEPTH = 25.0f;
constexpr float EMBOSS_SPOT_THETA = 0.021f;
constexpr float EMBOSS_SPOT_LIGHT_HEIGHT = -10.0f;
constexpr float EMBOSS_ALPHA_THRESHOLD = 0.05f;
constexpr float EMBOSS_ALPHA_AMPLIFY = 10.0f;
constexpr float EMBOSS_MASK_AA = 0.5f;
constexpr float LIGHT_DIR_EPS = 1e-6f;
constexpr float MIN_OUTPUT_PX_PER_STICKER_PX = 1e-6f;
const Vector3f EMBOSS_LIGHT_COLOR = Vector3f(1.0f, 1.0f, 1.0f);

constexpr char STICKER_EMBOSS_SHADER_PROG[] = R"(
uniform shader sdfTex;
uniform shader underlyingTex;
uniform shader insideTex;
uniform float resWidth;
uniform float resHeight;
uniform float sdfFalloff;

uniform float refractionStrength;
uniform float spotIntensity;
uniform float dirIntensity;
uniform float specPow;
uniform float specIntensity;
uniform float edgeLightWidth;
uniform float edgeLightFeatherRate;
uniform float2 edgeLightIntensity;

uniform float depth;
uniform float spotTheta;
uniform float alphaThreshold;
uniform float alphaAmplify;
uniform float maskAA;
uniform float3 lightColor;
uniform float2 lightDir;
uniform float3 spotPos;

const float N_EPS = 1e-6;
const float REFRACT_LIGHT_DIR_SCALE = 1.0 / 3.5;
const float DIR_LIGHT_START = 0.086;
const float DIR_LIGHT_END = 0.655;

const float EDGE_LIGHT_AA_PX = 1.0;

float sampleSdfDist(float2 coord)
{
    return sdfTex.eval(coord).a;
}

float sdfField(float dist)
{
    return 1.0 - clamp(dist / sdfFalloff, 0.0, 1.0);
}

half4 main(float2 fragCoord)
{
    float2 resolution = float2(resWidth, resHeight);
    float2 uv = fragCoord / resolution;

    float centerDist = sampleSdfDist(fragCoord);
    float centerSample = sdfField(centerDist);
    float rightSample = sdfField(sampleSdfDist(fragCoord + float2(1.0, 0.0)));
    float downSample = sdfField(sampleSdfDist(fragCoord + float2(0.0, 1.0)));

    float dx = (rightSample - centerSample) * depth;
    float dy = (downSample - centerSample) * depth;
    float3 nSpot = normalize(float3(-dx, -dy, 1.0));
    float2 gradient = float2(-dx, -dy);
    gradient = (dot(gradient, gradient) > 0.0) ? normalize(gradient) : gradient;
    float3 nDir = normalize(float3(gradient, 1.0));

    float amplified = (centerSample > alphaThreshold) ? min(centerSample * alphaAmplify, 1.0) : 0.0;
    float alpha = smoothstep(0.0, maskAA, amplified);
    if (alpha < N_EPS) {
        return half4(underlyingTex.eval(fragCoord));
    }

    float2 refractUV = uv + nDir.xy * lightDir * refractionStrength * REFRACT_LIGHT_DIR_SCALE;
    float4 underlayColor = underlyingTex.eval(refractUV * resolution);

    float4 insideColor = insideTex.eval(fragCoord);
    underlayColor.rgb = mix(underlayColor.rgb, insideColor.rgb, insideColor.a);

    float3 viewDir = float3(0.0, 0.0, 1.0);
    float3 finalColor = underlayColor.rgb;

    float3 spotLightDir = normalize(float3(uv - 0.5, 0.0) - spotPos);
    float attenuation = spotIntensity * smoothstep(cos(spotTheta), 1.0, spotLightDir.z);
    float3 halfDir = normalize(spotLightDir + viewDir);
    float spec = pow(max(dot(nSpot, halfDir), 0.0), specPow) * attenuation;
    float diffSpot = clamp(dot(nSpot, spotLightDir), 0.0, 1.0);
    finalColor += (specIntensity * spec * lightColor + lightColor * underlayColor.rgb * diffSpot * attenuation)
        * insideColor.a;

    float3 dirLightDir = normalize(float3(-lightDir / abs(spotPos.z), 0.01));
    float diff = clamp(dot(nDir, dirLightDir), 0.0, 1.0);
    float dirEnvelope = smoothstep(DIR_LIGHT_START * sdfFalloff, DIR_LIGHT_END * sdfFalloff, centerDist);
    finalColor += lightColor * underlayColor.rgb * diff * dirIntensity * dirEnvelope;

    float bandWidth = max(edgeLightWidth, N_EPS);
    float bandPos = clamp((sdfFalloff - centerDist) / bandWidth, 0.0, 1.0);
    float bandMask = 1.0 - smoothstep(sdfFalloff, sdfFalloff + EDGE_LIGHT_AA_PX, centerDist);
    float bandFade = 1.0 - smoothstep(edgeLightFeatherRate, 1.0, bandPos);
    float edgeMask = bandMask * bandFade;

    float shaded = clamp(-dot(nDir, dirLightDir), 0.0, 1.0);
    finalColor += lightColor * edgeLightIntensity.x * diff * edgeMask;
    finalColor *= 1.0 - clamp(edgeLightIntensity.y * shaded * edgeMask, 0.0, 1.0);

    float4 bgColor = underlyingTex.eval(fragCoord);
    float3 composite = finalColor * alpha + bgColor.rgb * (1.0 - alpha);
    float outAlpha = alpha + bgColor.a * (1.0 - alpha);
    return half4(float4(composite, outAlpha));
}
)";

float CalOutputPxPerStickerPx(
    const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& sticker)
{
    const float scaleX = static_cast<float>(image->GetWidth()) / static_cast<float>(sticker->GetWidth());
    const float scaleY = static_cast<float>(image->GetHeight()) / static_cast<float>(sticker->GetHeight());
    return (scaleX + scaleY) * 0.5f;
}

float CalSdfFalloffPx(float outlineWidthPx)
{
    return std::max(outlineWidthPx, EMBOSS_SDF_MIN_FALLOFF_PX);
}

// Normalized direction from the component center to the spot light center in pixel space.
void CalcEmbossLightDirection(const Drawing::GEStickerEmbossFilterParams& params, float imageWidth,
    float imageHeight, float& lightDirX, float& lightDirY)
{
    lightDirX = params.spotLightCenter.x_ - imageWidth * 0.5f;
    lightDirY = params.spotLightCenter.y_ - imageHeight * 0.5f;
    const float lightDirLen = std::sqrt(lightDirX * lightDirX + lightDirY * lightDirY);
    if (lightDirLen > LIGHT_DIR_EPS) {
        // std::max() keeps the divisor non-zero; lightDirLen is validated above
        lightDirX /= std::max(lightDirLen, LIGHT_DIR_EPS);
        lightDirY /= std::max(lightDirLen, LIGHT_DIR_EPS);
    } else {
        lightDirX = 0.0f;
        lightDirY = 0.0f;
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GetStickerEmbossBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> effect = nullptr;
    if (effect == nullptr) {
        effect = GECreateRuntimeEffectForShader(STICKER_EMBOSS_SHADER_PROG);
    }
    if (effect == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::GetStickerEmbossBuilder effect is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
}
} // namespace

GEStickerEmbossFilter::GEStickerEmbossFilter(const Drawing::GEStickerEmbossFilterParams& params) : params_(params) {}

std::shared_ptr<Drawing::Image> GEStickerEmbossFilter::MakeDecodedStickerSDFImage(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> sticker, float requiredRangePx)
{
    if (sticker == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeDecodedStickerSDFImage sticker is nullptr.");
        return nullptr;
    }

    const int spreadFactor = GEStickerOutlineShader::CalSdfSpreadFactor(requiredRangePx);
    const StickerSDFCache emptyCache {};
    auto cache = UnpackStickerSDFCacheOrDefaultFunc(cacheAnyPtr_, emptyCache);
    if (cache.stickerUniqueID == sticker->GetUniqueID() && cache.stickerWidth == sticker->GetWidth() &&
        cache.stickerHeight == sticker->GetHeight() && cache.spreadFactor == spreadFactor &&
        cache.decodedSdfImage != nullptr) {
        return cache.decodedSdfImage;
    }

    // Reuse the sticker SDF pipeline from the outline shader
    auto sdfImage = GEStickerOutlineShader::MakeStickerSDFImage(canvas, sticker, requiredRangePx);
    if (sdfImage == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeDecodedStickerSDFImage make SDF image failed.");
        cacheAnyPtr_ = nullptr;
        return nullptr;
    }
    auto decodedSdfImage =
        GEStickerOutlineShader::MakeDecodedStickerSDFImage(canvas, sdfImage, sticker->GetImageInfo(), requiredRangePx);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeDecodedStickerSDFImage make decoded SDF image failed.");
        cacheAnyPtr_ = nullptr;
        return nullptr;
    }

    StickerSDFCache cacheData { sticker->GetUniqueID(), sticker->GetWidth(), sticker->GetHeight(), spreadFactor,
        decodedSdfImage };
    cacheAnyPtr_ = PackStickerSDFCacheFunc(std::move(cacheData));
    return decodedSdfImage;
}

std::shared_ptr<Drawing::Image> GEStickerEmbossFilter::MakeStickerEmbossImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const std::shared_ptr<Drawing::Image> decodedSdfImage)
{
    if (image == nullptr || decodedSdfImage == nullptr || params_.sticker == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeStickerEmbossImage input is invalid.");
        return image;
    }

    // The sticker and the decoded SDF are at sticker resolution, scale them to the image extent
    const float scaleX = static_cast<float>(image->GetWidth()) / static_cast<float>(params_.sticker->GetWidth());
    const float scaleY = static_cast<float>(image->GetHeight()) / static_cast<float>(params_.sticker->GetHeight());
    const Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR);

    Drawing::Matrix matrix;
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, matrix);
    Drawing::Matrix stickerMatrix;
    stickerMatrix.SetScale(scaleX, scaleY);
    auto stickerShader = Drawing::ShaderEffect::CreateImageShader(
        *params_.sticker, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, stickerMatrix);
    Drawing::Matrix sdfMatrix;
    sdfMatrix.SetScale(scaleX, scaleY);
    auto sdfShader = Drawing::ShaderEffect::CreateImageShader(
        *decodedSdfImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, sdfMatrix);
    if (srcImageShader == nullptr || stickerShader == nullptr || sdfShader == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeStickerEmbossImage create image shader failed.");
        return image;
    }

    auto builder = GetStickerEmbossBuilder();
    if (builder == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeStickerEmbossImage builder is nullptr.");
        return image;
    }
    builder->SetChild("sdfTex", sdfShader);
    builder->SetChild("underlyingTex", srcImageShader);
    builder->SetChild("insideTex", stickerShader);
    SetEmbossShaderUniforms(*builder, image);

    auto result = builder->MakeImage(canvas.GetGPUContext().get(), &matrix, image->GetImageInfo(), false);
    if (result == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::MakeStickerEmbossImage make image failed.");
        return image;
    }
    return result;
}

void GEStickerEmbossFilter::SetEmbossShaderUniforms(
    Drawing::RuntimeShaderBuilder& builder, const std::shared_ptr<Drawing::Image> image)
{
    if (image == nullptr || params_.sticker == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::SetEmbossShaderUniforms input is invalid.");
        return;
    }

    // Convert output-pixel params into sticker-pixel params
    const float outputPxPerStickerPx = CalOutputPxPerStickerPx(image, params_.sticker);
    if (outputPxPerStickerPx <= 0.0f) {
        GE_LOGE("GEStickerEmbossFilter::SetEmbossShaderUniforms invalid output scale.");
        return;
    }
    // std::max() keeps the divisors non-zero; outputPxPerStickerPx is validated above
    const float outlineWidthPx = std::max(
        params_.outlineWidth / std::max(outputPxPerStickerPx, MIN_OUTPUT_PX_PER_STICKER_PX), 0.0f);
    // The edge light band is clamped inside the outline area before the unit conversion
    const float edgeLightWidthClampedPx = std::min(params_.edgeLightWidth, params_.outlineWidth);
    const float edgeLightWidthPx = std::max(
        edgeLightWidthClampedPx / std::max(outputPxPerStickerPx, MIN_OUTPUT_PX_PER_STICKER_PX), 0.0f);
    const float sdfFalloff = CalSdfFalloffPx(outlineWidthPx);
    const float featherRate = std::clamp(params_.edgeLightFeatherRate, 0.0f, EDGE_LIGHT_FEATHER_MAX);

    // Spot light center in pixels, converted to centered-UV space (uv - 0.5)
    const float imageWidth = static_cast<float>(image->GetWidth());
    const float imageHeight = static_cast<float>(image->GetHeight());
    const float spotU = params_.spotLightCenter.x_ / imageWidth - 0.5f;
    const float spotV = params_.spotLightCenter.y_ / imageHeight - 0.5f;

    // Directional light: from the component center to the spot light center (pixel space, normalized)
    float lightDirX = 0.0f;
    float lightDirY = 0.0f;
    CalcEmbossLightDirection(params_, imageWidth, imageHeight, lightDirX, lightDirY);

    builder.SetUniform("resWidth", static_cast<float>(image->GetWidth()));
    builder.SetUniform("resHeight", static_cast<float>(image->GetHeight()));
    builder.SetUniform("sdfFalloff", sdfFalloff);
    builder.SetUniform("refractionStrength", params_.refractionRate);
    builder.SetUniform("spotIntensity", params_.spotLightIntensity);
    builder.SetUniform("dirIntensity", params_.dirIntensity);
    builder.SetUniform("specPow", params_.specPow);
    builder.SetUniform("specIntensity", params_.specIntensity);
    builder.SetUniform("edgeLightWidth", edgeLightWidthPx);
    builder.SetUniform("edgeLightFeatherRate", featherRate);
    builder.SetUniform("edgeLightIntensity", params_.edgeLightIntensity.x_, params_.edgeLightIntensity.y_);
    builder.SetUniform("depth", EMBOSS_DEPTH);
    builder.SetUniform("spotTheta", EMBOSS_SPOT_THETA);
    builder.SetUniform("alphaThreshold", EMBOSS_ALPHA_THRESHOLD);
    builder.SetUniform("alphaAmplify", EMBOSS_ALPHA_AMPLIFY);
    builder.SetUniform("maskAA", EMBOSS_MASK_AA);
    builder.SetUniform("lightColor", EMBOSS_LIGHT_COLOR.x_, EMBOSS_LIGHT_COLOR.y_, EMBOSS_LIGHT_COLOR.z_);
    builder.SetUniform("lightDir", lightDirX, lightDirY);
    builder.SetUniform("spotPos", spotU, spotV, EMBOSS_SPOT_LIGHT_HEIGHT);

    GE_LOGD("GEStickerEmbossFilter::SetEmbossShaderUniforms falloff %{public}f spotUV %{public}f %{public}f "
            "lightDir %{public}f %{public}f",
        sdfFalloff, spotU, spotV, lightDirX, lightDirY);
}

std::shared_ptr<Drawing::Image> GEStickerEmbossFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::OnProcessImage image is nullptr.");
        return nullptr;
    }
    if (params_.sticker == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::OnProcessImage sticker is nullptr.");
        return image;
    }
    if (image->GetWidth() <= 0 || image->GetHeight() <= 0 || params_.sticker->GetWidth() <= 0 ||
        params_.sticker->GetHeight() <= 0) {
        GE_LOGE("GEStickerEmbossFilter::OnProcessImage invalid image or sticker size.");
        return image;
    }

    GE_TRACE_NAME_FMT(
        "GEStickerEmbossFilter::OnProcessImage Width: %d, Height: %d", image->GetWidth(), image->GetHeight());

    const float outputPxPerStickerPx = CalOutputPxPerStickerPx(image, params_.sticker);
    if (outputPxPerStickerPx <= 0.0f) {
        GE_LOGE("GEStickerEmbossFilter::OnProcessImage invalid output scale.");
        return image;
    }
    // std::max() keeps the divisor non-zero; outputPxPerStickerPx is validated above
    const float outlineWidthPx = std::max(
        params_.outlineWidth / std::max(outputPxPerStickerPx, MIN_OUTPUT_PX_PER_STICKER_PX), 0.0f);
    const float requiredRangePx = std::max(CalSdfFalloffPx(outlineWidthPx), EMBOSS_SDF_MIN_RANGE_PX);

    auto decodedSdfImage = MakeDecodedStickerSDFImage(canvas, params_.sticker, requiredRangePx);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerEmbossFilter::OnProcessImage decodedSdfImage is nullptr.");
        return image;
    }
    return MakeStickerEmbossImage(canvas, image, decodedSdfImage);
}

} // namespace Rosen
} // namespace OHOS
