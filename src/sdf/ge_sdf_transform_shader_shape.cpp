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

#include "sdf/ge_sdf_transform_shader_shape.h"
#include "ge_log.h"
#include "utils/ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTransformShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    if (!params_.shape) {
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetSDFTransformShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    auto shapeShader = params_.shape->GenerateDrawingShader(width, height);
    auto sdfTransformShapeShader = GenerateShaderEffect(width, height, shapeShader, builder);
    return sdfTransformShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTransformShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    if (!params_.shape) {
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetSDFTransformShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    auto shapeShader = params_.shape->GenerateDrawingShaderHasNormal(width, height);
    auto sdfTransformShapeShader = GenerateShaderEffect(width, height, shapeShader, builder);
    return sdfTransformShapeShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTransformShaderShape::GetSDFTransformShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfTransformShaderShapeBuilder = nullptr;
    if (sdfTransformShaderShapeBuilder) {
        return sdfTransformShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader shapeShader;
        uniform float3x3 transformMatrix;

        half4 main(vec2 fragCoord) {
            vec3 transformedCoord = transformMatrix * vec3(fragCoord, 1.0);
            vec2 perspectiveCoord = transformedCoord.xy / transformedCoord.z;
            return shapeShader.eval(perspectiveCoord);
        }
    )";

    auto sdfTransformShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfTransformShaderBuilderEffect) {
        LOGE("GESDFTransformShaderShape::GetSDFTransformShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfTransformShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfTransformShaderBuilderEffect);
    return sdfTransformShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateShaderEffect(float width, float height,
    std::shared_ptr<ShaderEffect> shapeShader, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    if (params_.matrix == Drawing::Matrix()) {
        return shapeShader;
    }

    builder->SetChild("shapeShader", shapeShader);
    builder->SetUniform("transformMatrix", params_.matrix);

    auto sdfTransformShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfTransformShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfTransformShapeShader;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS