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

#include "$HEADER_FILE"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"

// Additional includes for complex filters:
// #include <chrono>
// #include "draw/surface.h"
// #include "effect/color_matrix.h"
// #include "ge_system_properties.h"
// #include "ge_tone_mapping_helper.h"

namespace OHOS {
namespace Rosen {

$CLASS_NAME::$CLASS_NAME(const Drawing::$PARAMS_CLASS& params)
{
    // Optional: Initialize shader effects in constructor
    // if (!GetEffect()) {
    //     LOGE("$CLASS_NAME::$CLASS_NAME failed to initialize effect.");
    //     return;
    // }
}

std::shared_ptr<Drawing::RuntimeEffect> $CLASS_NAME::GetEffect()
{
    static constexpr char s_prog$EFFECT_NAME[] = R"(
        uniform shader image;
        uniform vec2 iResolution;

        half4 main(vec2 fragCoord)
        {
            vec2 uv = fragCoord.xy / iResolution.xy;
            vec4 color = image.eval(fragCoord);

            // TODO: Implement your effect logic here
            return color;
        }
    )";

    static const std::shared_ptr<Drawing::RuntimeEffect> s_effect =
        Drawing::RuntimeEffect::CreateForShader(s_prog$EFFECT_NAME);

    if (s_effect == nullptr) {
        LOGE("$CLASS_NAME::GetEffect create failed");
    }

    return s_effect;
}

std::shared_ptr<Drawing::Image> $CLASS_NAME::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    // 1. Input validation
    if (!image) {
        LOGE("$CLASS_NAME::OnProcessImage input image is null");
        return nullptr;
    }

    // 2. Get image info
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();

    // 3. Size validation
    if (height < 1e-6 || width < 1e-6) {
        LOGE("$CLASS_NAME::OnProcessImage invalid image size");
        return nullptr;
    }

    // 4. Get shader effect
    auto shader = GetEffect();
    if (!shader) {
        LOGE("$CLASS_NAME::OnProcessImage shader init failed");
        return image;
    }

    // 5. Create image shader with matrix
    Drawing::Matrix matrix = canvasInfo_.mat;
    matrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    Drawing::Matrix invertMatrix;

    if (!matrix.Invert(invertMatrix)) {
        LOGE("$CLASS_NAME::OnProcessImage Invert matrix failed");
        return image;
    }

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image,
        Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR),
        invertMatrix
    );

    // 6. Set up RuntimeShaderBuilder
    Drawing::RuntimeShaderBuilder builder(shader);
    builder.SetChild("image", imageShader);
    builder.SetUniform("iResolution", width, height);

    // TODO: Set additional uniforms based on your parameters
    // builder.SetUniform("parameterName", value);

    // 7. Create output image
#ifdef RS_ENABLE_GPU
    auto resultImage = builder.MakeImage(canvas.GetGPUContext().get(), &matrix, imageInfo, false);
#else
    auto resultImage = builder.MakeImage(nullptr, &matrix, imageInfo, false);
#endif

    if (!resultImage) {
        LOGE("$CLASS_NAME::OnProcessImage make image failed");
        return image;
    }

    return resultImage;
}

} // namespace Rosen
} // namespace OHOS
