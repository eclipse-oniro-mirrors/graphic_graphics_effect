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

#include "sdf/ge_sdf_rrect_shader_shape.h"
#include "ge_log.h"
#include "utils/ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr float HALF = 0.5;
std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetSDFRRectShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    auto sdfRRectShapeShader = GenerateShaderEffect(width, height, builder);
    if (auto matrixTransformShapeShader = GenerateMatrixTransformShader(width, height, sdfRRectShapeShader)) {
        return matrixTransformShapeShader;
    }
    return sdfRRectShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetSDFRRectNormalShapeBuilder();
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    auto sdfRRectShapeShader = GenerateShaderEffect(width, height, builder);
    if (auto matrixTransformShapeShader = GenerateMatrixTransformShader(width, height, sdfRRectShapeShader)) {
        return matrixTransformShapeShader;
    }
    return sdfRRectShapeShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFRRectShaderShape::GetSDFRRectShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfRRectShaderShapeBuilder = nullptr;
    if (sdfRRectShaderShapeBuilder) {
        return sdfRRectShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform float2 iResolution;
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
            vec2 center = fragCoord - iResolution.xy * 0.5;
            float sdf = sdfRRect(center, centerPos, halfSize, radius);
            return half4(sdf, 0, 0, 1);
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

    static constexpr char prog[] = R"(
        uniform float2 iResolution;
        uniform vec2 centerPos;
        uniform vec2 halfSize;
        uniform float radius;

        const float N_EPS = 1e-6;

        vec2 safeNorm(vec2 v) 
        { 
            return v / max(length(v), N_EPS);
        }

        vec3 sdgRRect(vec2 p, vec2 b, float r)
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

        half4 main(float2 fragCoord)
        {
            vec2 uv = fragCoord - 0.5 * iResolution.xy;
            vec3 sdg = sdgRRect(uv - centerPos, halfSize, radius);
            return half4(sdg.x, sdg.yz, 1.0);
        }
    )";

    auto sdfRRectNormalShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfRRectNormalShaderBuilderEffect) {
        LOGE("GESDFRRectShaderShape::GettSDFRRectNormalShapeBuilder effect error");
        return nullptr;
    }

    sdfRRectNormalShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfRRectNormalShaderBuilderEffect);
    return sdfRRectNormalShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateShaderEffect(float width, float height,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    // todo check boundary
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("centerPos", params_.rrect.left_ + HALF * params_.rrect.width_,
        params_.rrect.top_ - HALF * params_.rrect.height_);
    builder->SetUniform("halfSize", params_.rrect.width_ * HALF, params_.rrect.height_ * HALF);
    builder->SetUniform("radius", params_.rrect.radiusX_);

    auto sdfRRectShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfRRectShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfRRectShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateMatrixTransformShader(float width, float height,
    std::shared_ptr<ShaderEffect> shapeShader) const
{
    GE_TRACE_NAME_FMT("GESDFRRectShaderShape::GenerateMatrixTransformShader, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    builder = GetMatrixTransformBuilder();
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateMatrixTransformShader has builder error");
        return nullptr;
    }
    auto matrixTransformShapeShader = GenerateMatrixTransformShaderEffect(width, height, shapeShader, builder);
    return matrixTransformShapeShader;
}

// todo turn to utils class. Should know anchor of transform
std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFRRectShaderShape::GetMatrixTransformBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> matrixTransformBuilder = nullptr;
    if (matrixTransformBuilder) {
        return matrixTransformBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader shapeShader;
        uniform float2 iResolution;
        uniform vec2 centerPos;
        uniform float3x3 transformMatrix

        half4 main(vec2 fragCoord) {
            vec2 centered = fragCoord - 0.5 * iResolution.xy - centerPos;
            vec3 transformedCoord = transformMatrix * vec3(centered, 1.0);
            transformedCoord.xy += 0.5 * iResolution.xy + centerPos;
            return shapeShader.eval(transformedCoord.xy);
        }
    )";

    auto matrixTransformBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!matrixTransformBuilderEffect) {
        LOGE("GESDFRRectShaderShape::GetMatrixTransformBuilder effect error");
        return nullptr;
    }

    matrixTransformBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(matrixTransformBuilderEffect);
    return matrixTransformBuilder;
}

std::shared_ptr<ShaderEffect> GESDFRRectShaderShape::GenerateMatrixTransformShaderEffect(float width, float height,
    std::shared_ptr<ShaderEffect> shapeShader, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFRRectShaderShape::GenerateMatrixTransformShaderEffect builder error");
        return nullptr;
    }

    if (params_.matrix == Drawing::Matrix()) {
        return nullptr;
    }

    // todo check boundary
    builder->SetChild("shapeShader", shapeShader);
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("centerPos", params_.rrect.left_ + HALF * params_.rrect.width_,
        params_.rrect.top_ - HALF * params_.rrect.height_);
    builder->SetUniform("transformMatrix", params_.matrix);
    auto matrixTransformShapeShader = builder->MakeShader(nullptr, false);
    if (!matrixTransformShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateMatrixTransformShaderEffect shaderEffect error");
    }
    return matrixTransformShapeShader;
}

} // Drawing
} // namespace Rosen
} // namespace OHOS
