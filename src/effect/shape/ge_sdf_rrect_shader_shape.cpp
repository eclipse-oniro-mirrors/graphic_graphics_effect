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

#include "ge_sdf_cascade_manager.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr float HALF = 0.5;
constexpr float EXTEND = 0.5; // Fixing edge difference between SDF and Skia RRect
static const std::string shaderStringFunc = R"(
    vec2 safeNorm(vec2 v)
    {
        float N_EPS = 1e-6;
        return v / max(length(v), N_EPS);
    }

    vec3 ComputeSdg(vec2 p, vec2 b, float r)
    {
        float rr = clamp(r, 0.0, min(b.x, b.y));

        vec2 s = sign(p);
        vec2 w = abs(p) - b + rr;

        float g = max(w.x, w.y);
        vec2  q = max(w, 0.0);
        float l = length(q);

        float outside = step(0.0, g);
        float sd = mix(g, l, outside) - rr;

        float pickX = step(w.y, w.x);
        vec2  gradIn  = vec2(pickX, 1.0 - pickX);
        vec2  gradOut = safeNorm(q);
        vec2  grad    = s * mix(gradIn, gradOut, outside);

        return vec3(sd, grad);
    }

    float EncodeDir_RRect(vec2 dir)
    {
        float N_SCALE = 2048.0;
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    // Calling from main function
    vec4 sdgRRect(vec2 fragCoord, float3x3 transformMatrix, vec2 centerPos, vec2 halfSize, float radius)
    {
        // only apply transform to rgba's a pipeline
        float2x2 invtransformMatrix =
            float2x2(transformMatrix[0][0], transformMatrix[1][0], transformMatrix[0][1], transformMatrix[1][1]);
        vec3 transformedCoord = transformMatrix * vec3(fragCoord, 1.0);
        fragCoord = abs(transformedCoord.z) > 0.00001 ? transformedCoord.xy / transformedCoord.z : transformedCoord.xy;
        vec2 posFromCenter = fragCoord - centerPos;
        vec3 sdg = ComputeSdg(posFromCenter, halfSize, radius);
        sdg.yz = invtransformMatrix * sdg.yz;
        float packedDir = EncodeDir_RRect(invtransformMatrix * posFromCenter);
        return vec4(sdg.yz, packedDir, sdg.x);
    }
)";

static const std::string shaderStringMain = R"(
    uniform vec2 centerPos;
    uniform vec2 halfSize;
    uniform float radius;

    vec4 main(float2 fragCoord)
    {
       float3x3 identity = float3x3(1.0); // To use the public sdgRRect function
       return sdgRRect(fragCoord, identity, centerPos, halfSize, radius);
    }
)";

bool GESDFRRectShaderShape::GenerateCascadeShaderHasNormal(
    GESDFCascadeManager& manager, float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateCascadeShaderHasNormal, Width: %g, Height: %g", width, height);
    bool hasType = manager.AddSDFType(GESDFShapeType::RRECT);
    if (!hasType) {
        manager.PrependShaderFunction(shaderStringFunc);
    }
    auto thisUniformIndex = manager.GenerateUniformIndex();
    this->SetUniformIndex(thisUniformIndex);
    const Drawing::Matrix& thisMatrix = this->GetTransMatrix();
    Vector2f centerPos = Vector2f(params_.rrect.left_ + params_.rrect.width_ * HALF,
        params_.rrect.top_ + params_.rrect.height_ * HALF);
    Vector2f halfSize = Vector2f(params_.rrect.width_ * HALF, params_.rrect.height_ * HALF);
    UniformData transformMatrixUniform = {SDFUniformType::MATRIX, "transformMatrix", thisMatrix};
    UniformData centerPosUniform = {SDFUniformType::VECTOR2F, "centerPos", centerPos};
    UniformData halfSizeUniform  = {SDFUniformType::VECTOR2F, "halfSize", halfSize};
    UniformData radiusUniform = {SDFUniformType::FLOAT, "radius", std::min(
        std::max(0.0f, params_.rrect.radiusX_ + EXTEND), std::min(halfSize.x_ + EXTEND, halfSize.y_ + EXTEND))};
    manager.AddUniformData(thisUniformIndex,
        {transformMatrixUniform, centerPosUniform, halfSizeUniform, radiusUniform});
    
    std::ostringstream sdfCallOss;
    sdfCallOss << "sdgRRect(fragCoord, transformMatrix" << thisUniformIndex << ", centerPos" << thisUniformIndex
        << ", halfSize" << thisUniformIndex << ", radius" << thisUniformIndex << ")";
    manager.AppendSDFCall(thisUniformIndex, sdfCallOss.str());
    return true;
}

    std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFRRectShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFRRectNormalShapeBuilder();
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFRRectShaderShape::GetSDFRRectShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfRRectShaderShapeBuilder = nullptr;
    if (sdfRRectShaderShapeBuilder) {
        return sdfRRectShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform vec2 centerPos;
        uniform vec2 halfSize;
        uniform float radius;

        float sdfRRect(in vec2 coord, in vec2 p, in vec2 b, in float r)
        {
            vec2 d = abs(coord - p) - (b - r);
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
        }

        half4 main(vec2 fragCoord)
        {
            float sdf = sdfRRect(fragCoord, centerPos, halfSize, radius);
            return half4(0, 0, 0, sdf);
        }
    )";

    auto sdfRRectShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfRRectShaderBuilderEffect) {
        LOGE("GESDFRRectShaderShape::GetSDFRRectShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfRRectShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfRRectShaderBuilderEffect);
    return sdfRRectShaderShapeBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFRRectShaderShape::GetSDFRRectNormalShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfRRectNormalShaderShapeBuilder = nullptr;
    if (sdfRRectNormalShaderShapeBuilder) {
        return sdfRRectNormalShaderShapeBuilder;
    }

    auto sdfRRectNormalShaderBuilderEffect =
        Drawing::RuntimeEffect::CreateForShader(shaderStringFunc + shaderStringMain);
    if (!sdfRRectNormalShaderBuilderEffect) {
        LOGE("GESDFRRectShaderShape::GettSDFRRectNormalShapeBuilder effect error");
        return nullptr;
    }

    sdfRRectNormalShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(
        sdfRRectNormalShaderBuilderEffect);
    return sdfRRectNormalShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateShaderEffect(
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    if (params_.rrect.width_ < 0.0001f || params_.rrect.height_ < 0.0001f) {
        return nullptr;
    }

    builder->SetUniform("centerPos", params_.rrect.left_ + params_.rrect.width_ * HALF,
        params_.rrect.top_ + params_.rrect.height_ * HALF);
    auto halfWidth = params_.rrect.width_ * HALF + EXTEND;
    auto halfHeight = params_.rrect.height_ * HALF + EXTEND;
    builder->SetUniform("halfSize", halfWidth, halfHeight);
    builder->SetUniform("radius", std::min(std::max(0.0f, params_.rrect.radiusX_ + EXTEND),
        std::min(halfWidth, halfHeight)));

    auto sdfRRectShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfRRectShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfRRectShapeShader;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
