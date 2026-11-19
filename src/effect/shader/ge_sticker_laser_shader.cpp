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

#include "ge_sticker_laser_shader.h"

#include <algorithm>
#include <cstdint>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_cache_helper.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"
#include "ge_sticker_outline_shader.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr char STICKER_LASER_SHADER[] = R"(
    uniform vec2 direction;
    uniform float frequency;
    uniform float progress;

    half4 main(vec2 fragCoord)
    {
        vec2 dir = length(direction) > 0.0 ? normalize(direction) : vec2(1.0, 0.0);
        float t = dot(dir, fragCoord) * frequency + progress;
        t = fract(t);
        vec3 spectrum = vec3(0.84) + vec3(0.196) * cos(6.28318 * (t + vec3(0.0, 0.33, 0.67)));
        return half4(half3(spectrum), 1.0);
    }
)";

constexpr float LASER_INFLUENCE = 0.6f;
// The decodable SDF range must stay strictly beyond the outline threshold: the card
// shader's step(distance, outlineWidth) treats the saturated far-field value as
// inside the outline when the range equals the threshold.
constexpr float OUTLINE_RANGE_MARGIN_PX = 1.0f;

struct StickerLaserCache {
    uint64_t hash = 0;
    std::shared_ptr<Drawing::Image> decodedSdfImage = nullptr;
};
auto PackStickerLaserCacheFunc = GECacheHelper::PackCacheAny<StickerLaserCache>;
auto UnpackStickerLaserCacheOrDefaultFunc = GECacheHelper::UnpackCacheAnyOrDefault<StickerLaserCache>;

uint64_t CalStickerHash(const std::shared_ptr<Drawing::Image>& sticker, int spreadFactor)
{
    if (sticker == nullptr) {
        return 0;
    }
    const uintptr_t ptr = reinterpret_cast<uintptr_t>(sticker.get());
    uint64_t hash = static_cast<uint64_t>(ptr);
    hash = hash * 31ULL + static_cast<uint64_t>(sticker->GetWidth());
    hash = hash * 31ULL + static_cast<uint64_t>(sticker->GetHeight());
    hash = hash * 31ULL + static_cast<uint64_t>(spreadFactor);
    return hash;
}

thread_local std::shared_ptr<Drawing::RuntimeEffect> g_stickerLaserColorEffect = nullptr;

std::shared_ptr<Drawing::ShaderEffect> CreateLaserColorShader(
    const Drawing::GEStickerLaserShaderParams& params, const Drawing::Rect& rect)
{
    if (!g_stickerLaserColorEffect) {
        g_stickerLaserColorEffect = GECreateRuntimeEffectForShader(STICKER_LASER_SHADER);
    }
    if (g_stickerLaserColorEffect == nullptr) {
        GE_LOGE("GEStickerLaserShader::CreateLaserColorShader effect is nullptr.");
        return nullptr;
    }

    Drawing::RuntimeShaderBuilder builder(g_stickerLaserColorEffect);
    builder.SetUniform("direction", params.direction.x_, params.direction.y_);
    builder.SetUniform("frequency", params.frequency);
    builder.SetUniform("progress", params.progress);

    GE_LOGD("GEStickerLaserShader::CreateLaserColorShader frequency %{public}f progress %{public}f "
            "direction %{public}f %{public}f",
        params.frequency, params.progress, params.direction.x_, params.direction.y_);

    auto shader = builder.MakeShader(nullptr, false);
    if (shader == nullptr) {
        GE_LOGE("GEStickerLaserShader::CreateLaserColorShader shader is nullptr.");
        return nullptr;
    }
    return shader;
}
} // namespace

GEStickerLaserShader::GEStickerLaserShader(const Drawing::GEStickerLaserShaderParams& params) : params_(params) {}

void GEStickerLaserShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (params_.sticker == nullptr) {
        return;
    }

    const float requiredRange = std::max(params_.outlineWidth + OUTLINE_RANGE_MARGIN_PX,
        params_.elevationShadowRadius);
    const uint64_t inputHash =
        CalStickerHash(params_.sticker, GEStickerOutlineShader::CalSdfSpreadFactor(requiredRange));
    const StickerLaserCache emptyCache {};
    auto cache = UnpackStickerLaserCacheOrDefaultFunc(cacheAnyPtr_, emptyCache);
    if (cache.hash == inputHash && cache.decodedSdfImage != nullptr) {
        return;
    }

    auto sdfImage = GEStickerOutlineShader::MakeStickerSDFImage(canvas, params_.sticker, requiredRange);
    if (sdfImage == nullptr) {
        GE_LOGE("GEStickerLaserShader::Preprocess make SDF image failed");
        cacheAnyPtr_ = nullptr;
        return;
    }
    auto decodedSdfImage = GEStickerOutlineShader::MakeDecodedStickerSDFImage(
        canvas, sdfImage, params_.sticker->GetImageInfo(), requiredRange);
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerLaserShader::Preprocess make decoded SDF image failed");
        cacheAnyPtr_ = nullptr;
        return;
    }
    StickerLaserCache cacheData { inputHash, decodedSdfImage };
    cacheAnyPtr_ = PackStickerLaserCacheFunc(std::move(cacheData));
}

void GEStickerLaserShader::MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress)
{
    drShader_ = nullptr;
    if (params_.sticker == nullptr) {
        GE_LOGE("GEStickerLaserShader::MakeDrawingShader sticker is nullptr.");
        return;
    }

    const StickerLaserCache emptyCache {};
    auto decodedSdfImage = UnpackStickerLaserCacheOrDefaultFunc(cacheAnyPtr_, emptyCache).decodedSdfImage;
    if (decodedSdfImage == nullptr) {
        GE_LOGE("GEStickerLaserShader::MakeDrawingShader decodedSdfImage is nullptr.");
        return;
    }

    auto laserColorShader = CreateLaserColorShader(params_, rect);
    if (laserColorShader == nullptr) {
        GE_LOGE("GEStickerLaserShader::MakeDrawingShader laserColorShader is nullptr.");
        return;
    }

    drShader_ = GEStickerOutlineShader::MakeOutlineCardShaderImpl(params_.sticker, decodedSdfImage, rect,
        params_.outlineWidth, params_.lightDir, params_.elevationShadowColor, params_.elevationShadowRadius,
        params_.shadowDistance, laserColorShader, LASER_INFLUENCE);
}

} // namespace Rosen
} // namespace OHOS
