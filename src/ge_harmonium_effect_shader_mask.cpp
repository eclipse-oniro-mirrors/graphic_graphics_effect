/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
#include "effect/shader_effect.h"
#include "ge_log.h"
#include "ge_harmonium_effect_shader_mask.h"
#include "ge_shader_filter_params.h"
#include "platform/common/rs_log.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool GEHarmoniumEffectShaderMask::IsValid() const
{
    if (param_.image == nullptr) {
        ROSEN_LOGE("GEHarmoniumEffectShaderMask::IsValid image is nullptr");
        return false;
    }

    return true;
}

std::shared_ptr<ShaderEffect> GEHarmoniumEffectShaderMask::GenerateDrawingShader(float width, float height) const
{
    if (!IsValid()) {
        ROSEN_LOGE("GEHarmoniumEffectShaderMask::GenerateDrawingShader param is invalid");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetHarmoniumEffectShaderMaskBuilder();
    if (builder == nullptr) {
        LOGE("GEHarmoniumEffectShaderMask::GenerateDrawingShaderHas get builder error");
        return nullptr;
    }
    static const Drawing::SamplingOptions option(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    builder->SetChild("image", Drawing::ShaderEffect::CreateImageShader(*param_.image,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, option, Drawing::Matrix()));

    auto harmoniumEffectMaskEffectShader = builder->MakeShader(nullptr, false);
    if (harmoniumEffectMaskEffectShader == nullptr) {
        LOGE("GEHarmoniumEffectShaderMask::GenerateDrawingShader make shader failed");
        return nullptr;
    }
    return harmoniumEffectMaskEffectShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEHarmoniumEffectShaderMask::GetHarmoniumEffectShaderMaskBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> harmoniumEffectShaderMaskBuilder = nullptr;
    if (harmoniumEffectShaderMaskBuilder != nullptr) {
        return harmoniumEffectShaderMaskBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader image;

        vec4 main(float2 fragCoord)
        {
            return image.eval(fragCoord);
        }
    )";

    auto harmoniumEffectShaderMaskEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (harmoniumEffectShaderMaskEffect == nullptr) {
        LOGE("GEPixelMapShaderMask::GetPixelMapShaderMaskBuilder effect error");
        return nullptr;
    }

    harmoniumEffectShaderMaskBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(harmoniumEffectShaderMaskEffect);
    return harmoniumEffectShaderMaskBuilder;
}

std::shared_ptr<ShaderEffect> GEHarmoniumEffectShaderMask::GenerateDrawingShaderHasNormal(float width, float height) const
{
    return GenerateDrawingShader(width, height);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS