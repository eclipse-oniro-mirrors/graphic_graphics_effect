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
#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
#include "effect/shader_effect.h"
#include "ge_fractal_glass_shader_mask.h"
#include "ge_log.h"
#include "ge_shader_filter_params.h"
#include "platform/common/rs_log.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEFractalGlassShaderMask::GEFractalGlassShaderMask(const GEFractalGlassMaskParams& param) : param_(param)
{
    constexpr float minGlassNum = 0.0f;
    constexpr float maxGlassNum = 100.0f;
    constexpr float minGlassStrength = 0.0f;
    constexpr float maxGlassStrength = 10.0f;
    constexpr float minGlassSoftness = 0.0f;
    constexpr float maxGlassSoftness = 0.01f;
    param_.num_ = std::clamp(std::round(param_.num_), minGlassNum, maxGlassNum);
    param_.strength_ = std::clamp(param_.strength_, minGlassStrength, maxGlassStrength);
    param_.softness_ = std::clamp(param_.softness_, minGlassSoftness, maxGlassSoftness);
}

bool GEFractalGlassShaderMask::IsValid() const
{
    // pixelMap is optional: when null, shader uses refractMask=1.0 (no texture)
    if (param_.image != nullptr) {
        if (!param_.src.IsValid()) {
            LOGE("GEFractalGlassShaderMask::IsValid src is invalid");
            return false;
        }
        if (ROSEN_LE(param_.src.GetWidth() * param_.image->GetWidth(), 0.f)) {
            LOGE("GEFractalGlassShaderMask::IsValid src width is zero");
            return false;
        }
        if (ROSEN_LE(param_.src.GetHeight() * param_.image->GetHeight(), 0.f)) {
            LOGE("GEFractalGlassShaderMask::IsValid src height is zero");
            return false;
        }
    }
    return true;
}

void GEFractalGlassShaderMask::SetCommonUniforms(
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder, float width, float height) const
{
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("glassNum", param_.num_);
    builder->SetUniform("glassStrength", param_.strength_);
    builder->SetUniform("glassSoftness", param_.softness_);
    builder->SetUniform("glassSymmetric", param_.symmetric_ ? 1.0f : 0.0f);
}

std::shared_ptr<ShaderEffect> GEFractalGlassShaderMask::GenerateDrawingShader(float width, float height) const
{
    if (!IsValid()) {
        LOGE("GEFractalGlassShaderMask::GenerateDrawingShader param is invalid, hasImage=%{public}d",
            param_.image != nullptr);
        return nullptr;
    }

    if (param_.image != nullptr) {
        auto builder = GetFractalGlassShaderMaskBuilder();
        if (builder == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShader get builder error");
            return nullptr;
        }
        static const Drawing::SamplingOptions option(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
        Drawing::Matrix matrix;
        auto sx = param_.dst.GetWidth() * width / (param_.src.GetWidth() * param_.image->GetWidth());
        auto sy = param_.dst.GetHeight() * height / (param_.src.GetHeight() * param_.image->GetHeight());
        auto tx = param_.dst.left_ * width - param_.src.left_ * param_.image->GetWidth() * sx;
        auto ty = param_.dst.top_ * height - param_.src.top_ * param_.image->GetHeight() * sy;
        matrix.SetScaleTranslate(sx, sy, tx, ty);
        builder->SetChild("maskTexture", Drawing::ShaderEffect::CreateImageShader(*param_.image,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, option, matrix));
        SetCommonUniforms(builder, width, height);
        auto shaderEffect = builder->MakeShader(nullptr, false);
        if (shaderEffect == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShader make shader failed");
            return nullptr;
        }
        return shaderEffect;
    } else {
        auto builder = GetFractalGlassShaderMaskBuilderNoTexture();
        if (builder == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShader get noTexture builder error");
            return nullptr;
        }
        SetCommonUniforms(builder, width, height);
        auto shaderEffect = builder->MakeShader(nullptr, false);
        if (shaderEffect == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShader noTexture make shader failed");
            return nullptr;
        }
        return shaderEffect;
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEFractalGlassShaderMask::GetFractalGlassShaderMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (builder != nullptr) {
        return builder;
    }

    static constexpr char prog[] = R"(
        uniform shader maskTexture;
        uniform half2 iResolution;
        uniform half glassNum;
        uniform half glassStrength;
        uniform half glassSoftness;
        uniform half glassSymmetric;

        float displacement(float x, float num, float strength)
        {
            if (num <= 0.0) { return 0.0; }
            float modulus = 1.0 / num;
            return mod(x, modulus) * strength;
        }

        float fractal_glass(float x)
        {
            float d = 0.0;
            for (int i = -5; i <= 5; i++) {
                d += displacement(x + float(i) * glassSoftness, glassNum, glassStrength);
            }
            return x + d / 11.0;
        }

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord / iResolution;
            half refractMask = maskTexture.eval(fragCoord).r;
            float offsetL = fractal_glass(uv.x) - uv.x;
            float offsetR = fractal_glass(1.0 - uv.x) - (1.0 - uv.x);
            float blend = smoothstep(0.48, 0.52, uv.x);
            float offsetSym = mix(offsetL, -offsetR, blend);
            float offsetAsym = offsetL;
            float offset = mix(offsetAsym, offsetSym, glassSymmetric) * refractMask;
            return half4(offset, offset, offset, refractMask);
        }
    )";

    auto effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (effect == nullptr) {
        LOGE("GEFractalGlassShaderMask::GetFractalGlassShaderMaskBuilder effect error");
        return nullptr;
    }

    builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEFractalGlassShaderMask::GetFractalGlassShaderMaskBuilderNoTexture()
    const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (builder != nullptr) {
        return builder;
    }

    // No texture variant: refractMask = 1.0 (full effect everywhere)
    static constexpr char prog[] = R"(
        uniform half2 iResolution;
        uniform half glassNum;
        uniform half glassStrength;
        uniform half glassSoftness;
        uniform half glassSymmetric;

        float displacement(float x, float num, float strength)
        {
            if (num <= 0.0) { return 0.0; }
            float modulus = 1.0 / num;
            return mod(x, modulus) * strength;
        }

        float fractal_glass(float x)
        {
            float d = 0.0;
            for (int i = -5; i <= 5; i++) {
                d += displacement(x + float(i) * glassSoftness, glassNum, glassStrength);
            }
            return x + d / 11.0;
        }

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord / iResolution;
            float offsetL = fractal_glass(uv.x) - uv.x;
            float offsetR = fractal_glass(1.0 - uv.x) - (1.0 - uv.x);
            float blend = smoothstep(0.48, 0.52, uv.x);
            float offsetSym = mix(offsetL, -offsetR, blend);
            float offsetAsym = offsetL;
            float offset = mix(offsetAsym, offsetSym, glassSymmetric);
            return half4(offset, offset, offset, 1.0);
        }
    )";

    auto effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (effect == nullptr) {
        LOGE("GEFractalGlassShaderMask::GetFractalGlassShaderMaskBuilderNoTexture effect error");
        return nullptr;
    }

    builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builder;
}

std::shared_ptr<ShaderEffect> GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    if (!IsValid()) {
        LOGE("GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal param is invalid, hasImage=%{public}d",
            param_.image != nullptr);
        return nullptr;
    }

    if (param_.image != nullptr) {
        auto builder = GetFractalGlassShaderNormalMaskBuilder();
        if (builder == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal get builder error");
            return nullptr;
        }
        static const Drawing::SamplingOptions option(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
        Drawing::Matrix matrix;
        auto sx = param_.dst.GetWidth() * width / (param_.src.GetWidth() * param_.image->GetWidth());
        auto sy = param_.dst.GetHeight() * height / (param_.src.GetHeight() * param_.image->GetHeight());
        auto tx = param_.dst.left_ * width - param_.src.left_ * param_.image->GetWidth() * sx;
        auto ty = param_.dst.top_ * height - param_.src.top_ * param_.image->GetHeight() * sy;
        matrix.SetScaleTranslate(sx, sy, tx, ty);
        builder->SetChild("maskTexture", Drawing::ShaderEffect::CreateImageShader(*param_.image,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, option, matrix));
        SetCommonUniforms(builder, width, height);
        auto shaderEffect = builder->MakeShader(nullptr, false);
        if (shaderEffect == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal make shader failed");
            return nullptr;
        }
        return shaderEffect;
    } else {
        auto builder = GetFractalGlassShaderNormalMaskBuilderNoTexture();
        if (builder == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal get noTexture builder error");
            return nullptr;
        }
        SetCommonUniforms(builder, width, height);
        auto shaderEffect = builder->MakeShader(nullptr, false);
        if (shaderEffect == nullptr) {
            LOGE("GEFractalGlassShaderMask::GenerateDrawingShaderHasNormal noTexture make shader failed");
            return nullptr;
        }
        return shaderEffect;
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEFractalGlassShaderMask::GetFractalGlassShaderNormalMaskBuilder()
    const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (builder != nullptr) {
        return builder;
    }

    static constexpr char prog[] = R"(
        uniform shader maskTexture;
        uniform half2 iResolution;
        uniform half glassNum;
        uniform half glassStrength;
        uniform half glassSoftness;
        uniform half glassSymmetric;

        float displacement(float x, float num, float strength) {
            if (num <= 0.0) { return 0.0; }
            float modulus = 1.0 / num;
            return mod(x, modulus) * strength;
        }

        float fractal_glass(float x) {
            float d = 0.0;
            for (int i = -5; i <= 5; i++) {
                d += displacement(x + float(i) * glassSoftness, glassNum, glassStrength);
            }
            return x + d / 11.0;
        }

        half4 main(float2 fragCoord) {
            half2 uv = fragCoord / iResolution;
            half refractMask = maskTexture.eval(fragCoord).r;
            half refractWeight = (refractMask - 0.5) * 2.0;

            float offsetL = fractal_glass(uv.x) - uv.x;
            float offsetR = fractal_glass(1.0 - uv.x) - (1.0 - uv.x);
            float blend = smoothstep(0.48, 0.52, uv.x);
            float offsetSym = mix(offsetL, -offsetR, blend);
            float offsetAsym = offsetL;
            float totalOffset = mix(offsetAsym, offsetSym, glassSymmetric) * refractWeight;

            half r = clamp(0.5 - totalOffset * 5.0, 0.0, 1.0);
            half g = 0.5;
            half a = refractMask > 0.0 ? 1.0 : 0.0;
            return half4(r, g, a, 1.0);
        }
    )";

    auto effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (effect == nullptr) {
        LOGE("GEFractalGlassShaderMask::GetFractalGlassShaderNormalMaskBuilder effect error");
        return nullptr;
    }

    builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder>
    GEFractalGlassShaderMask::GetFractalGlassShaderNormalMaskBuilderNoTexture() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (builder != nullptr) {
        return builder;
    }

    // No texture variant: refractWeight = 1.0 (full effect everywhere)
    static constexpr char prog[] = R"(
        uniform half2 iResolution;
        uniform half glassNum;
        uniform half glassStrength;
        uniform half glassSoftness;
        uniform half glassSymmetric;

        float displacement(float x, float num, float strength)
        {
            if (num <= 0.0) { return 0.0; }
            float modulus = 1.0 / num;
            return mod(x, modulus) * strength;
        }

        float fractal_glass(float x)
        {
            float d = 0.0;
            for (int i = -5; i <= 5; i++) {
                d += displacement(x + float(i) * glassSoftness, glassNum, glassStrength);
            }
            return x + d / 11.0;
        }

        half4 main(float2 fragCoord)
        {
            half2 uv = fragCoord / iResolution;

            float offsetL = fractal_glass(uv.x) - uv.x;
            float offsetR = fractal_glass(1.0 - uv.x) - (1.0 - uv.x);
            float blend = smoothstep(0.48, 0.52, uv.x);
            float offsetSym = mix(offsetL, -offsetR, blend);
            float offsetAsym = offsetL;
            float totalOffset = mix(offsetAsym, offsetSym, glassSymmetric);

            half r = clamp(0.5 - totalOffset * 5.0, 0.0, 1.0);
            half g = 0.5;
            return half4(r, g, 1.0, 1.0);
        }
    )";

    auto effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (effect == nullptr) {
        LOGE("GEFractalGlassShaderMask::GetFractalGlassShaderNormalMaskBuilderNoTexture effect error");
        return nullptr;
    }

    builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builder;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS