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

#include "ge_log.h"
#include "ge_sticker_halftone_shader.h"
#include "ge_sticker_outline_shader.h"
#include "ge_shader_diagnostics.h"
#include "ge_cache_helper.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr char STICKER_HALFTONE_SHADER[] = R"__SKSL__(
    uniform shader inputTex;
    uniform float gridSize;
    uniform float maxRadiusRatio;
    uniform float lumCoeff;
    uniform float colorWidthRatio;
    uniform float saturation;
    uniform float smoothness;
    uniform float angle;

    float2x2 rotateMat(float theta)
    {
        float cTheta = cos(theta);
        float sTheta = sin(theta);
        return float2x2(cTheta, -sTheta, sTheta, cTheta);
    }

    half4 main(float2 fragCoord)
    {
        // Guard against gridSize == 0 (direct GE construction with default params)
        if (gridSize <= 0.0) {
            return half4(inputTex.eval(fragCoord));
        }

        float2 rotCoord = fragCoord * rotateMat(angle);
        float2 rotUv = rotCoord / gridSize;
        float2 ip = floor(rotUv);
        float2 fp = fract(rotUv) - 0.5;

        float4 tex = inputTex.eval(fragCoord);
        float4 colTex = inputTex.eval((ip + 0.5) * gridSize * rotateMat(-angle));

        float lum = dot(colTex.rgb, float3(0.299, 0.587, 0.114));
        colTex.rgb = mix(float3(lum), colTex.rgb, saturation);

        float outerRadius = clamp(1.0 - lum * lumCoeff, 0.0, maxRadiusRatio) * gridSize * 0.5;
        float innerRadius = max(outerRadius - colorWidthRatio * gridSize * 0.5, 0.0);
        // Cap halfBand to gridSize to prevent overflow when smoothness * gridSize exceeds float range
        float halfBand = min(max(smoothness * gridSize, 1e-4), gridSize);
        float d = length(fp) * gridSize;

        // 1.0 - smoothstep(edge0, edge1, d) produces a dot mask: 1.0 inside radius, 0.0 outside.
        // When float precision collapses edge0 == edge1 (large radius + tiny halfBand),
        // fall back to hard step to avoid undefined smoothstep behavior.
        float innerDot = (innerRadius <= 0.0) ? 0.0 :
            (innerRadius + halfBand > innerRadius - halfBand)
            ? 1.0 - smoothstep(innerRadius - halfBand, innerRadius + halfBand, d)
            : step(d, innerRadius);
        float outerDot = (outerRadius <= 0.0) ? 0.0 :
            (outerRadius + halfBand > outerRadius - halfBand)
            ? 1.0 - smoothstep(outerRadius - halfBand, outerRadius + halfBand, d)
            : step(d, outerRadius);
        float3 col = mix(float3(1.0), mix(colTex.rgb, float3(0.0), innerDot), outerDot);
        return half4(float4(col * tex.a, tex.a));
    }
)__SKSL__";

thread_local std::shared_ptr<Drawing::RuntimeEffect> g_stickerHalftoneEffect = nullptr;

struct StickerHalftoneCache {
    uint32_t hash = 0;
    std::shared_ptr<Drawing::Image> decodedSdfImage = nullptr;
};

auto PackStickerHalftoneCacheFunc = GECacheHelper::PackCacheAny<StickerHalftoneCache>;
auto UnpackStickerHalftoneCacheOrDefaultFunc = GECacheHelper::UnpackCacheAnyOrDefault<StickerHalftoneCache>;
std::shared_ptr<Drawing::Image> MakeHalftoneImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& sticker, const Drawing::GEStickerHalftoneShaderParams& params)
{
    if (!g_stickerHalftoneEffect) {
        g_stickerHalftoneEffect = GECreateRuntimeEffectForShader(STICKER_HALFTONE_SHADER);
    }
    if (!g_stickerHalftoneEffect) {
        GE_LOGE("GEStickerHalftoneShader::MakeHalftoneImage g_stickerHalftoneEffect is nullptr.");
        return nullptr;
    }

    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*sticker, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (!imageShader) {
        GE_LOGE("GEStickerHalftoneShader::MakeHalftoneImage imageShader is nullptr.");
        return nullptr;
    }
    Drawing::RuntimeShaderBuilder builder(g_stickerHalftoneEffect);
    builder.SetChild("inputTex", imageShader);
    builder.SetUniform("gridSize", params.gridSize_);
    builder.SetUniform("maxRadiusRatio", params.maxRadiusRatio_);
    builder.SetUniform("lumCoeff", params.lumCoeff_);
    builder.SetUniform("colorWidthRatio", params.colorWidthRatio_);
    builder.SetUniform("saturation", params.saturation_);
    builder.SetUniform("smoothness", params.smoothness_);
    builder.SetUniform("angle", params.angle_);

    auto halftoneImage = builder.MakeImage(canvas.GetGPUContext().get(), &matrix, sticker->GetImageInfo(), false);
    if (!halftoneImage) {
        GE_LOGE("GEStickerHalftoneShader::MakeHalftoneImage halftoneImage is nullptr.");
        return nullptr;
    }
    return halftoneImage;
}
} // namespace

void GEStickerHalftoneShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (params_.sticker_ == nullptr) {
        return;
    }

    const float requiredRange = std::max(params_.outlinePx_, params_.elevationShadowRadius_);
    const int spreadFactor = GEStickerOutlineShader::CalSdfSpreadFactor(requiredRange);
    const bool hasExternalCache = cacheAnyPtr_ != nullptr;

    if (hasExternalCache) {
        const uint32_t inputHash = GEStickerOutlineShader::CalStickerHash(params_.sticker_, spreadFactor);
        const StickerHalftoneCache emptyCache{};
        const auto& cache = UnpackStickerHalftoneCacheOrDefaultFunc(cacheAnyPtr_, emptyCache);
        if (cache.hash == inputHash && cache.decodedSdfImage != nullptr) {
            return;
        }
    }

    auto sdfImage = GEStickerOutlineShader::MakeStickerSDFImage(canvas, params_.sticker_, requiredRange);
    if (!sdfImage) {
        GE_LOGE("GEStickerHalftoneShader::Preprocess sdfImage is nullptr.");
        cacheAnyPtr_ = nullptr;
        return;
    }

    auto decodedSdfImage = GEStickerOutlineShader::MakeDecodedStickerSDFImage(canvas, sdfImage,
        params_.sticker_->GetImageInfo(), requiredRange);
    if (!decodedSdfImage) {
        GE_LOGE("GEStickerHalftoneShader::Preprocess decodedSdfImage is nullptr.");
        cacheAnyPtr_ = nullptr;
        return;
    }

    uint32_t inputHash = 0;
    if (hasExternalCache) {
        inputHash = GEStickerOutlineShader::CalStickerHash(params_.sticker_, spreadFactor);
    }
    StickerHalftoneCache cacheData{inputHash, decodedSdfImage};
    cacheAnyPtr_ = PackStickerHalftoneCacheFunc(std::move(cacheData));
}

void GEStickerHalftoneShader::MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress)
{
    drShader_ = nullptr;
    if (params_.sticker_ == nullptr) {
        GE_LOGE("GEStickerHalftoneShader::MakeDrawingShader sticker is nullptr.");
        return;
    }

    const StickerHalftoneCache emptyCache{};
    auto decodedSdfImage = UnpackStickerHalftoneCacheOrDefaultFunc(cacheAnyPtr_, emptyCache).decodedSdfImage;
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerHalftoneShader::MakeDrawingShader decodedSdfImage is nullptr.");
        return;
    }

    auto halftoneImage = MakeHalftoneImage(canvas, params_.sticker_, params_);
    if (!halftoneImage) {
        GE_LOGE("GEStickerHalftoneShader::MakeDrawingShader halftoneImage is nullptr.");
        return;
    }

    drShader_ = GEStickerOutlineShader::MakeOutlineCardShader(halftoneImage, decodedSdfImage, rect,
        params_.outlinePx_, params_.lightDir_, params_.elevationShadowColor_,
        params_.elevationShadowRadius_, params_.shadowDistance_);
}

} // namespace Rosen
} // namespace OHOS
