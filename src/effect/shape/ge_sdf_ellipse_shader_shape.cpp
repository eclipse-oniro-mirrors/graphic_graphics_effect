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

#include "ge_sdf_ellipse_shader_shape.h"

#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr float MIN_SIZE = 0.0001f;
} // namespace

static constexpr char SDF_GRAD_PROG[] = R"(
    uniform vec2 centerPos;
    uniform vec2 radius;

    const float EPS = 1e-6;
    const float N_SCALE = 2048.0;

    float sdEllipse(vec2 p, vec2 r)
    {
        vec2 safeR = max(r, vec2(EPS));
        vec2 invR = p / safeR;
        vec2 invR2 = p / max(safeR * safeR, vec2(EPS));
        float k0 = length(invR);
        if (k0 < EPS) {
            return -min(safeR.x, safeR.y);
        }
        float k1 = max(length(invR2), EPS);
        return k0 * (k0 - 1.0) / k1;
    }

    vec2 safeNorm(vec2 v)
    {
        return v / max(length(v), EPS);
    }

    vec2 ellipseGrad(vec2 p, vec2 r)
    {
        vec2 safeR = max(r, vec2(EPS));
        vec2 scaled = p / safeR;
        float scaledLen = length(scaled);
        if (scaledLen < EPS) {
            return vec2(0.0, -1.0);
        }
        return safeNorm(p / max(safeR * safeR * scaledLen, vec2(EPS)));
    }

    float EncodeDir(vec2 dir)
    {
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    vec4 main(float2 fragCoord)
    {
        vec2 posFromCenter = fragCoord - centerPos;
        float sdf = sdEllipse(posFromCenter, radius);
        vec2 grad = ellipseGrad(posFromCenter, radius);
        float packedDir = EncodeDir(posFromCenter);
        return vec4(grad, packedDir, sdf);
    }
)";

std::shared_ptr<ShaderEffect> GESDFEllipseShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFEllipseShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFEllipseShaderShapeBuilder();
    if (!builder) {
        LOGE("GESDFEllipseShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<ShaderEffect> GESDFEllipseShaderShape::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFEllipseShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFEllipseNormalShapeBuilder();
    if (!builder) {
        LOGE("GESDFEllipseShaderShape::GenerateDrawingShaderHasNormal has builder error");
        return nullptr;
    }
    return GenerateShaderEffect(builder);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFEllipseShaderShape::GetSDFEllipseShaderShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfEllipseShaderShapeBuilder = nullptr;
    if (sdfEllipseShaderShapeBuilder) {
        return sdfEllipseShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform vec2 centerPos;
        uniform vec2 radius;

        const float EPS = 1e-6;

        float sdEllipse(vec2 p, vec2 r)
        {
            vec2 safeR = max(r, vec2(EPS));
            vec2 invR = p / safeR;
            vec2 invR2 = p / max(safeR * safeR, vec2(EPS));
            float k0 = length(invR);
            if (k0 < EPS) {
                return -min(safeR.x, safeR.y);
            }
            float k1 = max(length(invR2), EPS);
            return k0 * (k0 - 1.0) / k1;
        }

        half4 main(vec2 fragCoord)
        {
            float sdf = sdEllipse(fragCoord - centerPos, radius);
            return half4(0, 0, 0, sdf);
        }
    )";

    auto sdfEllipseShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfEllipseShaderBuilderEffect) {
        LOGE("GESDFEllipseShaderShape::GetSDFEllipseShaderShapeBuilder effect error");
        return nullptr;
    }

    sdfEllipseShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfEllipseShaderBuilderEffect);
    return sdfEllipseShaderShapeBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFEllipseShaderShape::GetSDFEllipseNormalShapeBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfEllipseNormalShaderShapeBuilder = nullptr;
    if (sdfEllipseNormalShaderShapeBuilder) {
        return sdfEllipseNormalShaderShapeBuilder;
    }

    auto sdfEllipseNormalShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(SDF_GRAD_PROG);
    if (!sdfEllipseNormalShaderBuilderEffect) {
        LOGE("GESDFEllipseShaderShape::GetSDFEllipseNormalShapeBuilder effect error");
        return nullptr;
    }

    sdfEllipseNormalShaderShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfEllipseNormalShaderBuilderEffect);
    return sdfEllipseNormalShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFEllipseShaderShape::GenerateShaderEffect(
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFEllipseShaderShape::GenerateShaderEffect builder error");
        return nullptr;
    }
    if (params_.radius.x_ < MIN_SIZE || params_.radius.y_ < MIN_SIZE) {
        LOGE("GESDFEllipseShaderShape::GenerateShaderEffect invalid radius");
        return nullptr;
    }

    builder->SetUniform("centerPos", params_.center.x_, params_.center.y_);
    builder->SetUniform("radius", params_.radius.x_, params_.radius.y_);

    auto sdfEllipseShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfEllipseShapeShader) {
        LOGE("GESDFEllipseShaderShape::GenerateShaderEffect shaderEffect error");
    }
    return sdfEllipseShapeShader;
}

bool GESDFEllipseShaderShape::TryGetCenter(float& outX, float& outY) const
{
    outX = params_.center.x_;
    outY = params_.center.y_;
    return true;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
