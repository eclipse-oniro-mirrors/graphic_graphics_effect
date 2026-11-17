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

#include "ge_sticker_outline_shader.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "effect/filter/ge_mesa_blur_shader_filter.h"
#include "effect/filter/ge_sdf_from_image_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_cache_helper.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {

namespace {
// The sticker is downsampled to 1/4 resolution before SDF generation
constexpr int STICKER_SDF_DOWNSAMPLE = 4;
// Upper bound for JFA spread factor
constexpr int STICKER_SDF_MAX_SPREAD_FACTOR = 1024;
// JFA spread factor doubles each iteration
constexpr int STICKER_SDF_SPREAD_DOUBLE = 2;

// Decodes the 1/4-resolution encoded SDF back to the sticker's native resolution,
// in native pixels.
constexpr char STICKER_SDF_DECODE_SHADER[] = R"(
    uniform shader sdfImage;
    uniform float sdfRange;

    half4 main(vec2 fragCoord)
    {
        vec4 sdfColor = sdfImage.eval(fragCoord);
        // Encoded SDF: alpha in [0, 1] maps to distance in [-sdfRange, sdfRange]
        float distance = (sdfColor.a * 2.0 - 1.0) * sdfRange;
        return half4(distance);
    }
)";

constexpr char STICKER_OUTLINE_CARD_SHADER[] = R"(
    uniform shader imageTex;
    uniform shader maskTex;
    uniform shader bandColor;

    uniform float outlineWidth;
    uniform float bandInfluence;
    uniform vec2 lightDir;
    uniform float shadowDistance;
    uniform float shadowRadius;
    uniform vec2 shadowScale;
    uniform vec4 shadowColor;

    half4 main(vec2 fragCoord)
    {
        vec4 imageColor = imageTex.eval(fragCoord);
        float distance = maskTex.eval(fragCoord).a;
        vec4 bandColor4 = bandColor.eval(fragCoord);
        vec4 tint = mix(vec4(1.0), vec4(bandColor4.rgb, 1.0), bandInfluence);

        vec4 maskHoloColor = vec4(vec3(1.0), step(distance, outlineWidth));
        float isOutline = step(imageColor.a, 0.99);
        vec4 result = mix(imageColor * tint, maskHoloColor, isOutline);
        result = mix(result, vec4(bandColor4.rgb, maskHoloColor.a), isOutline);

        if (shadowRadius > 0.0) {
            vec2 shadowDir = length(lightDir) > 0.0 ? normalize(lightDir) : vec2(0.0);
            float shadowSdf = maskTex.eval(fragCoord - shadowDir * shadowDistance * shadowScale).a;
            float shadowSample = 1.0 - smoothstep(0.0, shadowRadius, shadowSdf);
            vec4 shadowRes = vec4(shadowColor.rgb, shadowColor.a * shadowSample);
            result = mix(shadowRes, result, result.a);
        }
        return half4(result.rgb * result.a, result.a);
    }
)";

struct StickerOutlineCache {
    uint32_t hash = 0;
    std::shared_ptr<Drawing::Image> decodedSdfImage = nullptr;
};
auto PackStickerOutlineCacheFunc = GECacheHelper::PackCacheAny<StickerOutlineCache>;
auto UnpackStickerOutlineCacheOrDefaultFunc = GECacheHelper::UnpackCacheAnyOrDefault<StickerOutlineCache>;

thread_local std::shared_ptr<Drawing::RuntimeEffect> g_stickerSdfDecodeEffect = nullptr;
thread_local std::shared_ptr<Drawing::RuntimeEffect> g_stickerOutlineCardEffect = nullptr;
thread_local std::shared_ptr<Drawing::RuntimeEffect> g_compositeEffect = nullptr;

std::shared_ptr<Drawing::RuntimeShaderBuilder> GetStickerSdfDecodeBuilder()
{
    if (!g_stickerSdfDecodeEffect) {
        g_stickerSdfDecodeEffect = GECreateRuntimeEffectForShader(STICKER_SDF_DECODE_SHADER);
    }
    if (!g_stickerSdfDecodeEffect) {
        GE_LOGE("GEStickerOutlineShader::GetStickerSdfDecodeBuilder effect is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_stickerSdfDecodeEffect);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GetStickerOutlineCardBuilder()
{
    if (!g_stickerOutlineCardEffect) {
        g_stickerOutlineCardEffect = GECreateRuntimeEffectForShader(STICKER_OUTLINE_CARD_SHADER);
    }
    if (!g_stickerOutlineCardEffect) {
        GE_LOGE("GEStickerOutlineShader::GetStickerOutlineCardBuilder effect is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_stickerOutlineCardEffect);
}

constexpr char COMPOSITE_SHADER[] = R"(
uniform shader cardTex;
uniform shader contentTex;
half4 main(float2 fragCoord) {
    float4 card = cardTex.eval(fragCoord);
    float4 content = contentTex.eval(fragCoord);
    float4 result;
    result.rgb = card.rgb + content.rgb * (1.0 - card.a);
    result.a = card.a + content.a * (1.0 - card.a);
    return half4(result);
}
)";

std::shared_ptr<Drawing::RuntimeShaderBuilder> GetCompositeBuilder()
{
    if (!g_compositeEffect) {
        g_compositeEffect = GECreateRuntimeEffectForShader(COMPOSITE_SHADER);
    }
    if (!g_compositeEffect) {
        GE_LOGE("GEStickerOutlineShader::GetCompositeBuilder effect is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_compositeEffect);
}
} // namespace

uint32_t GEStickerOutlineShader::CalStickerHash(const std::shared_ptr<Drawing::Image>& sticker, int spreadFactor)
{
    if (sticker == nullptr) {
        return 0;
    }
    const uint64_t ptr = reinterpret_cast<uintptr_t>(sticker.get());
    uint32_t hash = static_cast<uint32_t>(ptr) ^ static_cast<uint32_t>(ptr >> 32);
    const auto& info = sticker->GetImageInfo();
    hash = hash * 31U + static_cast<uint32_t>(info.GetWidth());
    hash = hash * 31U + static_cast<uint32_t>(info.GetHeight());
    hash = hash * 31U + static_cast<uint32_t>(info.GetColorType());
    hash = hash * 31U + static_cast<uint32_t>(info.GetAlphaType());
    hash = hash * 31U + static_cast<uint32_t>(spreadFactor);
    auto colorSpace = sticker->GetColorSpace();
    if (colorSpace != nullptr) {
        hash = hash * 31U + static_cast<uint32_t>(colorSpace->GetType());
    }
    return hash;
}

GEStickerOutlineShader::GEStickerOutlineShader(const Drawing::GEStickerOutlineShaderParams& params)
    : params_(params) {}

int GEStickerOutlineShader::CalSdfSpreadFactor(float requiredRangePx)
{
    const float quarterResRange = std::max(requiredRangePx, 0.0f) / static_cast<float>(STICKER_SDF_DOWNSAMPLE);
    const float clampedRange = std::min(quarterResRange, static_cast<float>(STICKER_SDF_MAX_SPREAD_FACTOR));
    int spread = 1;
    while (spread < static_cast<int>(std::ceil(clampedRange)) && spread < STICKER_SDF_MAX_SPREAD_FACTOR) {
        spread *= STICKER_SDF_SPREAD_DOUBLE;
    }
    return spread;
}

std::shared_ptr<Drawing::Image> GEStickerOutlineShader::MakeStickerSDFImage(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& sticker, float requiredRangePx)
{
    if (sticker == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeStickerSDFImage sticker is nullptr.");
        return nullptr;
    }

    // Downsample the sticker to 1/4 resolution to reduce SDF generation cost
    Drawing::GEMESABlurShaderFilterParams downsampleParams {};
    downsampleParams.radius = 1;
    GEMESABlurShaderFilter downsampleFilter(downsampleParams);
    const Drawing::Rect stickerRect(
        0.0f, 0.0f, static_cast<float>(sticker->GetWidth()), static_cast<float>(sticker->GetHeight()));
    const Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR);
    auto downsampledSticker = downsampleFilter.DownSamplingForEdge(
        canvas, sticker, stickerRect, linear, 1.0f / static_cast<float>(STICKER_SDF_DOWNSAMPLE));
    if (downsampledSticker == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeStickerSDFImage downsampledSticker is nullptr.");
        return nullptr;
    }

    // Generate the encoded SDF from the downsampled sticker alpha mask
    Drawing::GESDFFromImageFilterParams sdfParams {};
    sdfParams.spreadFactor = CalSdfSpreadFactor(requiredRangePx);
    sdfParams.generateDerivs = false;
    GESDFFromImageFilter sdfFilter(sdfParams);
    const Drawing::Rect sdfRect(0.0f, 0.0f, static_cast<float>(downsampledSticker->GetWidth()),
        static_cast<float>(downsampledSticker->GetHeight()));
    auto sdfImage = sdfFilter.OnProcessImage(canvas, downsampledSticker, sdfRect, sdfRect);
    if (sdfImage == nullptr || sdfImage == downsampledSticker) {
        GE_LOGE("GEStickerOutlineShader::MakeStickerSDFImage sdf generation failed.");
        return nullptr;
    }
    return sdfImage;
}

std::shared_ptr<Drawing::Image> GEStickerOutlineShader::MakeDecodedStickerSDFImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& sdfImage, const Drawing::ImageInfo& stickerInfo, float requiredRangePx)
{
    if (sdfImage == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDecodedStickerSDFImage sdfImage is nullptr.");
        return nullptr;
    }
    auto builder = GetStickerSdfDecodeBuilder();
    if (builder == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDecodedStickerSDFImage builder is nullptr.");
        return nullptr;
    }

    // Scale the 1/4-resolution encoded SDF back to the sticker's native resolution
    Drawing::Matrix sdfMatrix;
    sdfMatrix.SetScale(static_cast<float>(stickerInfo.GetWidth()) / static_cast<float>(sdfImage->GetWidth()),
        static_cast<float>(stickerInfo.GetHeight()) / static_cast<float>(sdfImage->GetHeight()));
    auto sdfShader = Drawing::ShaderEffect::CreateImageShader(*sdfImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), sdfMatrix);
    if (sdfShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDecodedStickerSDFImage sdfShader is nullptr.");
        return nullptr;
    }

    builder->SetChild("sdfImage", sdfShader);
    builder->SetUniform("sdfRange", static_cast<float>(CalSdfSpreadFactor(requiredRangePx) * STICKER_SDF_DOWNSAMPLE));

    // Use f16 format to keep sub-pixel SDF precision
    Drawing::ImageInfo decodedInfo = stickerInfo;
    decodedInfo.SetColorType(Drawing::ColorType::COLORTYPE_RGBA_F16);
    auto decodedSdfImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, decodedInfo, false);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDecodedStickerSDFImage decodedSdfImage is nullptr.");
        return nullptr;
    }
    return decodedSdfImage;
}

std::shared_ptr<Drawing::ShaderEffect> GEStickerOutlineShader::MakeOutlineCardShader(
    const std::shared_ptr<Drawing::Image>& srcImage, const std::shared_ptr<Drawing::Image>& decodedSdfImage,
    const Drawing::Rect& rect, float outlineWidth, const Vector2f& lightDir, const Vector4f& shadowColor,
    float shadowRadius, float shadowDistance)
{
    auto bandShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_WHITE);
    if (bandShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader bandShader is nullptr.");
        return nullptr;
    }
    return MakeOutlineCardShaderImpl(srcImage, decodedSdfImage, rect, outlineWidth, lightDir,
        shadowColor, shadowRadius, shadowDistance, bandShader, 0.0f);
}

std::shared_ptr<Drawing::ShaderEffect> GEStickerOutlineShader::MakeOutlineCardShaderImpl(
    const std::shared_ptr<Drawing::Image>& srcImage, const std::shared_ptr<Drawing::Image>& decodedSdfImage,
    const Drawing::Rect& rect, float outlineWidth, const Vector2f& lightDir, const Vector4f& shadowColor,
    float shadowRadius, float shadowDistance, const std::shared_ptr<Drawing::ShaderEffect>& bandColorShader,
    float bandInfluence)
{
    if (srcImage == nullptr || decodedSdfImage == nullptr || !rect.IsValid()) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader input is invalid.");
        return nullptr;
    }
    if (bandColorShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader bandColorShader is nullptr.");
        return nullptr;
    }
    auto builder = GetStickerOutlineCardBuilder();
    if (builder == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader builder is nullptr.");
        return nullptr;
    }

    const float width = rect.GetWidth();
    const float height = rect.GetHeight();
    const float scaleX = width / static_cast<float>(srcImage->GetWidth());
    const float scaleY = height / static_cast<float>(srcImage->GetHeight());

    Drawing::Matrix srcMatrix;
    srcMatrix.SetScale(scaleX, scaleY);
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*srcImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), srcMatrix);
    if (srcImageShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader srcImageShader is nullptr.");
        return nullptr;
    }

    Drawing::Matrix maskMatrix;
    maskMatrix.SetScale(width / static_cast<float>(decodedSdfImage->GetWidth()),
        height / static_cast<float>(decodedSdfImage->GetHeight()));
    auto maskShader = Drawing::ShaderEffect::CreateImageShader(*decodedSdfImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), maskMatrix);
    if (maskShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader maskShader is nullptr.");
        return nullptr;
    }

    builder->SetChild("imageTex", srcImageShader);
    builder->SetChild("maskTex", maskShader);
    builder->SetChild("bandColor", bandColorShader);
    builder->SetUniform("outlineWidth", outlineWidth);
    builder->SetUniform("bandInfluence", bandInfluence);
    builder->SetUniform("lightDir", lightDir.x_, lightDir.y_);
    builder->SetUniform("shadowDistance", shadowDistance);
    builder->SetUniform("shadowRadius", shadowRadius);
    builder->SetUniform("shadowScale", scaleX, scaleY);
    builder->SetUniformVec4("shadowColor", shadowColor.x_, shadowColor.y_, shadowColor.z_, shadowColor.w_);

    GE_LOGD("GEStickerOutlineShader::MakeOutlineCardShader outlineWidth %{public}f "
            "bandInfluence %{public}f shadowRadius %{public}f shadowDistance %{public}f",
        outlineWidth, bandInfluence, shadowRadius, shadowDistance);

    auto shader = builder->MakeShader(nullptr, false);
    if (shader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeOutlineCardShader shader is nullptr.");
        return nullptr;
    }
    return shader;
}

void GEStickerOutlineShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (params_.sticker_ == nullptr) {
        return;
    }

    const float requiredRange = std::max(params_.outlinePx_, params_.elevationShadowRadius_);
    const int spreadFactor = CalSdfSpreadFactor(requiredRange);
    const bool hasExternalCache = cacheAnyPtr_ != nullptr;

    if (hasExternalCache) {
        const uint32_t inputHash = CalStickerHash(params_.sticker_, spreadFactor);
        const StickerOutlineCache emptyCache {};
        auto cache = UnpackStickerOutlineCacheOrDefaultFunc(cacheAnyPtr_, emptyCache);
        if (cache.hash == inputHash && cache.decodedSdfImage != nullptr) {
            return;
        }
    }

    auto sdfImage = MakeStickerSDFImage(canvas, params_.sticker_, requiredRange);
    if (sdfImage == nullptr) {
        GE_LOGE("GEStickerOutlineShader::Preprocess make SDF image failed");
        cacheAnyPtr_ = nullptr;
        return;
    }
    auto decodedSdfImage = MakeDecodedStickerSDFImage(canvas, sdfImage,
        params_.sticker_->GetImageInfo(), requiredRange);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerOutlineShader::Preprocess make decoded SDF image failed");
        cacheAnyPtr_ = nullptr;
        return;
    }

    uint32_t inputHash = 0;
    if (hasExternalCache) {
        inputHash = CalStickerHash(params_.sticker_, spreadFactor);
    }
    StickerOutlineCache cacheData { inputHash, decodedSdfImage };
    cacheAnyPtr_ = PackStickerOutlineCacheFunc(std::move(cacheData));
}

void GEStickerOutlineShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = nullptr;
    if (params_.sticker_ == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDrawingShader sticker is nullptr.");
        return;
    }

    const StickerOutlineCache emptyCache {};
    auto decodedSdfImage = UnpackStickerOutlineCacheOrDefaultFunc(cacheAnyPtr_, emptyCache).decodedSdfImage;
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeDrawingShader decodedSdfImage is nullptr.");
        return;
    }

    drShader_ = MakeOutlineCardShader(params_.sticker_, decodedSdfImage, rect,
        params_.outlinePx_, params_.lightDir_, params_.elevationShadowColor_,
        params_.elevationShadowRadius_, params_.shadowDistance_);
}

std::shared_ptr<Drawing::Image> GEStickerOutlineShader::MakeCompositeImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::ShaderEffect>& cardShader,
    const std::shared_ptr<Drawing::Image>& contentImage, const Drawing::Rect& rect)
{
    if (cardShader == nullptr || contentImage == nullptr || !rect.IsValid()) {
        GE_LOGE("GEStickerOutlineShader::MakeCompositeImage input is invalid.");
        return nullptr;
    }

    auto builder = GetCompositeBuilder();
    if (builder == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeCompositeImage builder is nullptr.");
        return nullptr;
    }

    Drawing::Matrix matrix;
    auto contentShader = Drawing::ShaderEffect::CreateImageShader(*contentImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (contentShader == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeCompositeImage contentShader is nullptr.");
        return nullptr;
    }

    builder->SetChild("cardTex", cardShader);
    builder->SetChild("contentTex", contentShader);
    auto result = builder->MakeImage(canvas.GetGPUContext().get(), &matrix, contentImage->GetImageInfo(), false);
    if (result == nullptr) {
        GE_LOGE("GEStickerOutlineShader::MakeCompositeImage result is nullptr.");
        return nullptr;
    }
    return result;
}

} // namespace Rosen
} // namespace OHOS
