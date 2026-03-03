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

#include "ge_sdf_triangle_shader_shape.h"
#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

static constexpr char SDF_GRAD_PROG[] = R"(
    uniform vec2 vertex0;
    uniform vec2 vertex1;
    uniform vec2 vertex2;

    const float N_EPS = 1e-6;
    const float N_SCALE = 2048.0;

    vec2 safeNorm(vec2 v)
    {
        return v / max(length(v), N_EPS);
    }

    float sign2(vec2 p, vec2 v0, vec2 v1)
    {
        return (p.x - v1.x) * (v0.y - v1.y) - (v0.x - v1.x) * (p.y - v1.y);
    }

    vec3 sdgTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2)
    {
        vec2 e0 = v1 - v0;
        vec2 e1 = v2 - v1;
        vec2 e2 = v0 - v2;

        vec2 v0p = p - v0;
        vec2 v1p = p - v1;
        vec2 v2p = p - v2;

        vec2 pq0 = v0p - e0 * clamp(dot(v0p, e0) / dot(e0, e0), 0.0, 1.0);
        vec2 pq1 = v1p - e1 * clamp(dot(v1p, e1) / dot(e1, e1), 0.0, 1.0);
        vec2 pq2 = v2p - e2 * clamp(dot(v2p, e2) / dot(e2, e2), 0.0, 1.0);

        float d0 = dot(pq0, pq0);
        float d1 = dot(pq1, pq1);
        float d2 = dot(pq2, pq2);

        float d = min(min(d0, d1), d2);

        vec2 pq;
        if (d == d0) {
            pq = pq0;
        } else if (d == d1) {
            pq = pq1;
        } else {
            pq = pq2;
        }

        float s0 = sign2(p, v0, v1);
        float s1 = sign2(p, v1, v2);
        float s2 = sign2(p, v2, v0);

        bool inside = (s0 >= 0.0) && (s1 >= 0.0) && (s2 >= 0.0);

        float sd = sqrt(d) * (inside ? -1.0 : 1.0);
        vec2 grad = safeNorm(pq) * (inside ? -1.0 : 1.0);

        return vec3(sd, grad);
    }

    float EncodeDir(vec2 dir)
    {
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    vec4 main(float2 fragCoord)
    {
        vec3 sdg = sdgTriangle(fragCoord, vertex0, vertex1, vertex2);
        float packedDir = EncodeDir(sdg.yz);
        return vec4(sdg.yz, packedDir, sdg.x);
    }
)";

std::shared_ptr<ShaderEffect> GESDFTriangleShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTriangleShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFTriangleShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFTriangleShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<ShaderEffect> GESDFTriangleShaderShape::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTriangleShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFTriangleNormalShapeBuilder();
    if (!builder) {
        LOGE("GESDFTriangleShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTriangleShaderShape::GetSDFTriangleShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfTriangleShaderShapeBuilder = nullptr;
    if (sdfTriangleShaderShapeBuilder) {
        return sdfTriangleShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform vec2 vertex0;
        uniform vec2 vertex1;
        uniform vec2 vertex2;

        float sign2(vec2 p, vec2 v0, vec2 v1)
        {
            return (p.x - v1.x) * (v0.y - v1.y) - (v0.x - v1.x) * (p.y - v1.y);
        }

        float sdfTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2)
        {
            vec2 e0 = v1 - v0;
            vec2 e1 = v2 - v1;
            vec2 e2 = v0 - v2;

            vec2 v0p = p - v0;
            vec2 v1p = p - v1;
            vec2 v2p = p - v2;

            vec2 pq0 = v0p - e0 * clamp(dot(v0p, e0) / dot(e0, e0), 0.0, 1.0);
            vec2 pq1 = v1p - e1 * clamp(dot(v1p, e1) / dot(e1, e1), 0.0, 1.0);
            vec2 pq2 = v2p - e2 * clamp(dot(v2p, e2) / dot(e2, e2), 0.0, 1.0);

            float d = min(min(dot(pq0, pq0), dot(pq1, pq1)), dot(pq2, pq2));

            float s0 = sign2(p, v0, v1);
            float s1 = sign2(p, v1, v2);
            float s2 = sign2(p, v2, v0);

            bool inside = (s0 >= 0.0) && (s1 >= 0.0) && (s2 >= 0.0);

            return sqrt(d) * (inside ? -1.0 : 1.0);
        }

        half4 main(vec2 fragCoord)
        {
            float sdf = sdfTriangle(fragCoord, vertex0, vertex1, vertex2);
            return half4(0, 0, 0, sdf);
        }
    )";

    auto sdfTriangleShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfTriangleShaderBuilderEffect) {
        LOGE("GESDFTriangleShaderShape::GetSDFTriangleShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfTriangleShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfTriangleShaderBuilderEffect);
    return sdfTriangleShaderShapeBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTriangleShaderShape::GetSDFTriangleNormalShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfTriangleNormalShaderShapeBuilder = nullptr;
    if (sdfTriangleNormalShaderShapeBuilder) {
        return sdfTriangleNormalShaderShapeBuilder;
    }

    auto sdfTriangleNormalShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(SDF_GRAD_PROG);
    if (!sdfTriangleNormalShaderBuilderEffect) {
        LOGE("GESDFTriangleShaderShape::GetSDFTriangleNormalShapeBuilder effect error");
        return nullptr;
    }

    sdfTriangleNormalShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(
        sdfTriangleNormalShaderBuilderEffect);
    return sdfTriangleNormalShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFTriangleShaderShape::GenerateShaderEffect(
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFTriangleShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }

    builder->SetUniform("vertex0", params_.vertex0.x_, params_.vertex0.y_);
    builder->SetUniform("vertex1", params_.vertex1.x_, params_.vertex1.y_);
    builder->SetUniform("vertex2", params_.vertex2.x_, params_.vertex2.y_);

    auto sdfTriangleShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfTriangleShapeShader) {
        LOGE("GESDFTriangleShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfTriangleShapeShader;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
