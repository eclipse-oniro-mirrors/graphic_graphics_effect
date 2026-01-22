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
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFTransformShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    auto shapeShader = params_.shape->GenerateDrawingShader(width, height);
    if (!shapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShader has empty shader");
        return nullptr;
    }
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
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFTransformShaderHasNormalShapeBuilder();
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    auto shapeShader = params_.shape->GenerateDrawingShaderHasNormal(width, height);
    if (!shapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateDrawingShaderHasNormal has empty shader");
        return nullptr;
    }
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
            vec2 perspectiveCoord = transformedCoord.xy;
            if (abs(transformedCoord.z) > 0.00001) {
                perspectiveCoord = transformedCoord.xy / transformedCoord.z;
            }
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

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTransformShaderShape::GetSDFTransformShaderHasNormalShapeBuilder()
    const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfTransformShaderHasNormalShapeBuilder = nullptr;
    if (sdfTransformShaderHasNormalShapeBuilder) {
        return sdfTransformShaderHasNormalShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader shapeShader;
        uniform float3x3 transformMatrix;
        const float nScale = 2048.0;
        float EncodeDir(vec2 dir)
        {
            float xPos = floor(dir.x + nScale);
            float yPos = floor(dir.y + nScale);
            return xPos + (yPos / nScale) / 2.0;
        }
        vec2 DecodeDir(float z) {
            float xPos = floor(z);
            float yPos = (z - xPos) * nScale * 2.0 - nScale;
            xPos -= nScale;
            return vec2(xPos, yPos);
        }
        half4 main(vec2 fragCoord) {
            // only apply transform to rgba's a pipeline
            float2x2 invtransformMatrix =
                float2x2(transformMatrix[0][0], transformMatrix[1][0], transformMatrix[0][1], transformMatrix[1][1]);
            vec3 transformedCoord = transformMatrix * vec3(fragCoord, 1.0);
            vec2 perspectiveCoord = transformedCoord.xy;

            // but scaling may be applied to all ?
            if (abs(transformedCoord.z) > 0.00001) {
                perspectiveCoord = transformedCoord.xy / transformedCoord.z;
            }
            // calculate rgb
            half4 res = shapeShader.eval(perspectiveCoord);
            res.rg = invtransformMatrix * res.rg;
            vec2 centerVec = invtransformMatrix * DecodeDir(res.b);
            res.b = EncodeDir(centerVec);
            return res;
        }
    )";

    auto sdfTransformShaderHasNormalBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfTransformShaderHasNormalBuilderEffect) {
        LOGE("GESDFTransformShaderShape::sdfTransformShaderHasNormalShapeBuilder effect error");
        return nullptr;
    }

    sdfTransformShaderHasNormalShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfTransformShaderHasNormalBuilderEffect);
    return sdfTransformShaderHasNormalShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateShaderEffect(float width, float height,
    std::shared_ptr<ShaderEffect> shapeShader, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFTransformShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    if (params_.matrix.IsIdentity()) {
        return shapeShader;
    }

    // turn transform  from shape to fragcoord
    Drawing::Matrix invertMatrix;
    if (params_.matrix.Invert(invertMatrix)) {
        builder->SetUniform("transformMatrix", invertMatrix);
    } else {
        LOGE("GESDFTransformShaderShape::GenerateShaderEffect, invert matrix failed");
        builder->SetUniform("transformMatrix", params_.matrix);
    }
    builder->SetChild("shapeShader", shapeShader);

    auto sdfTransformShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfTransformShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfTransformShapeShader;
}

bool GESDFTransformShaderShape::HasType(const GESDFShapeType type) const
{
    if (type == GetSDFShapeType()) {
        return true;
    }
    if (params_.shape ? params_.shape->HasType(type) : false) {
        return true;
    }
    return false;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS