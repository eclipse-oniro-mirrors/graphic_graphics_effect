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

#include "sdf/ge_sdf_pixelmap_shader_shape.h"
#include "ge_log.h"
#include "utils/ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFPixelmapShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    if (params_.image == nullptr) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShader has empty image");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFPixelmapShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    auto pixelmapShader = GeneratePixelmapShader();
    if (!pixelmapShader) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShader has empty shader");
        return nullptr;
    }
    auto sdfPixelmapShapeShader = GenerateShaderEffect(pixelmapShader, builder);
    return sdfPixelmapShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    if (params_.image == nullptr) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal has empty image");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFPixelmapNormalShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    auto pixelmapShader = GeneratePixelmapShader();
    if (!pixelmapShader) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal has empty shader");
        return nullptr;
    }
    auto sdfPixelmapShapeShader = GenerateShaderEffect(pixelmapShader, builder);
    return sdfPixelmapShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GeneratePixelmapShader() const
{
    Drawing::Matrix matrix;
    return Drawing::ShaderEffect::CreateImageShader(*params_.image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFPixelmapShaderShape::GetSDFPixelmapShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfPixelmapShaderShapeBuilder = nullptr;
    if (sdfPixelmapShaderShapeBuilder) {
        return sdfPixelmapShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader pixelmapShader;
        half4 main(vec2 fragCoord)
        {
            // SDF will be mapped from [0, 1] to [-63.5, 64]
            vec4 pixelmapShape = pixelmapShader.eval(fragCoord);
            return half4(pixelmapShape.xyz, pixelmapShape.a * 127.5 - 63.5);
        }
    )";

    auto sdfPixelmapShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfPixelmapShaderBuilderEffect) {
        LOGE("GESDFPixelmapShaderShape::GetSDFPixelmapShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfPixelmapShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfPixelmapShaderBuilderEffect);
    return sdfPixelmapShaderShapeBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFPixelmapShaderShape::GetSDFPixelmapNormalShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfPixelmapShaderShapeBuilder = nullptr;
    if (sdfPixelmapShaderShapeBuilder) {
        return sdfPixelmapShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader pixelmapShader;
        half4 main(vec2 fragCoord)
        {
            // Gradient(rg channel) will be mapped from [0, 1] to [-1, 1]
            // SDF(a channel) will be mapped from [0, 1] to [-63.5, 64]
            vec4 pixelmapShape = pixelmapShader.eval(fragCoord);
            return half4(pixelmapShape.xy * 2 - 1, pixelmapShape.z, pixelmapShape.a * 127.5 - 63.5);
        }
    )";

    auto sdfPixelmapShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfPixelmapShaderBuilderEffect) {
        LOGE("GESDFPixelmapShaderShape::GetSDFPixelmapNormalShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfPixelmapShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfPixelmapShaderBuilderEffect);
    return sdfPixelmapShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GenerateShaderEffect(
    std::shared_ptr<ShaderEffect> pixelmapShader, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFPixelmapShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    builder->SetChild("pixelmapShader", pixelmapShader);

    auto sdfPixelmapShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfPixelmapShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfPixelmapShapeShader;
}

} // Drawing
} // namespace Rosen
} // namespace OHOS