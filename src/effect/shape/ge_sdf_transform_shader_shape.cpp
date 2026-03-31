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

#include "ge_sdf_transform_shader_shape.h"
#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTransformShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    if (!params_.shape) {
        return nullptr;
    }
    
    if (params_.unionMode == 1) {
        return GenerateGravityPullDrawingShader(width, height);
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
    
    if (params_.unionMode == 1) {
        return GenerateGravityPullDrawingShaderHasNormal(width, height);
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
        return Drawing::ShaderEffect::CreateColorShader(0);
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

bool GESDFTransformShaderShape::TryGetCenter(float& outX, float& outY) const
{
    outX = 0.f;
    outY = 0.f;

    if (!params_.shape) {
        return false;
    }

    float childX = 0.f;
    float childY = 0.f;
    if (!params_.shape->TryGetCenter(childX, childY)) {
        return false;
    }

    std::vector<Point> src = {Point(childX, childY)};
    std::vector<Point> dst(1);
    params_.matrix.MapPoints(dst, src, 1);
    outX = dst[0].GetX();
    outY = dst[0].GetY();
    return true;
}

static constexpr char GRAVITY_PULL_PROG[] = R"(
    uniform float spacing;
    uniform float warpStrength;
    uniform vec2 shapeCenterPos;
    uniform vec2 centerPos;
    uniform shader shapeShader;

    vec2 normalizeWithLenSq(vec2 v, float lenSq)
    {
        return (lenSq > 1e-8) ? v * inversesqrt(lenSq) : vec2(0.0, 0.0);
    }

    vec2 warpedSamplePos(vec2 p)
    {
        vec2 delta = centerPos - shapeCenterPos;
        float centerDistSq = dot(delta, delta);
        if (centerDistSq <= 1e-8) {
            return p;
        }

        float centerDist = sqrt(centerDistSq);
        float centerGate = smoothstep(
            0.25 * spacing,
            0.25 * spacing + 0.35 * spacing,
            centerDist
        );
        if (centerGate <= 1e-5) {
            return p;
        }

        vec2 fromSmallCenter = p - centerPos;
        float localDistSq = dot(fromSmallCenter, fromSmallCenter);

        float warpRadius = max(spacing, 1e-6);
        float warpRadiusSq = warpRadius * warpRadius;

        if (localDistSq >= warpRadiusSq) {
            return p;
        }

        float falloff = 1.0 - smoothstep(0.0, warpRadiusSq, localDistSq);

        float hotValue = shapeShader.eval(centerPos).a;
        float hotGate = 1.0 - smoothstep(
            5.0,
            5.0 + 0.3 * spacing,
            hotValue
        );

        float warpAmount = warpStrength * falloff * hotGate * centerGate;
        if (warpAmount <= 1e-3) {
            return p;
        }

        vec2 dir = normalizeWithLenSq(delta, centerDistSq);
        return p - dir * warpAmount;
    }

    float mergedSdfAt(vec2 p)
    {
        return shapeShader.eval(warpedSamplePos(p)).a;
    }

    half4 main(vec2 p)
    {
        return half4(0.0, 0.0, 0.0, mergedSdfAt(p));
    }
)";

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTransformShaderShape::GetGravityPullDrawingShaderBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> gravityPullShaderBuilder = nullptr;
    if (gravityPullShaderBuilder) {
        return gravityPullShaderBuilder;
    }

    auto gravityPullShaderEffect = Drawing::RuntimeEffect::CreateForShader(GRAVITY_PULL_PROG);
    if (!gravityPullShaderEffect) {
        LOGE("GESDFTransformShaderShape::GetGravityPullDrawingShaderBuilder effect error");
        return nullptr;
    }

    gravityPullShaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(gravityPullShaderEffect);
    return gravityPullShaderBuilder;
}

std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateGravityPullDrawingShader(
    float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTransformShaderShape::GenerateGravityPullDrawingShader, Width: %g, Height: %g",
        width, height);
    
    auto baseShapeShader = params_.shape->GenerateDrawingShader(width, height);
    if (!baseShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader has empty base shader");
        return nullptr;
    }

    auto transformBuilder = GetSDFTransformShaderShapeBuilder();
    if (!transformBuilder) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader has transform builder error");
        return nullptr;
    }
    auto transformedShapeShader = GenerateShaderEffect(width, height, baseShapeShader, transformBuilder);
    if (!transformedShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader has transformed shader error");
        return nullptr;
    }
    
    auto gravityPullShaderBuilder = GetGravityPullDrawingShaderBuilder();
    if (!gravityPullShaderBuilder) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader has gravity pull shader builder error");
        return nullptr;
    }

    float shapeCenterX = 0.f;
    float shapeCenterY = 0.f;
    if (!TryGetCenter(shapeCenterX, shapeCenterY)) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader get transformed shape center error");
    }

    gravityPullShaderBuilder->SetUniform("spacing", std::max(params_.spacing, 0.0001f));
    gravityPullShaderBuilder->SetUniform("warpStrength", params_.warpStrength);
    gravityPullShaderBuilder->SetUniform("shapeCenterPos", shapeCenterX, shapeCenterY);
    gravityPullShaderBuilder->SetUniform("centerPos", params_.centerPosition[0], params_.centerPosition[1]);
    gravityPullShaderBuilder->SetChild("shapeShader", transformedShapeShader);

    auto gravityPullShader = gravityPullShaderBuilder->MakeShader(nullptr, false);
    if (!gravityPullShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShader gravity pull shader effect error");
        return nullptr;
    }
    
    return gravityPullShader;
}

static constexpr char GRAVITY_PULL_NORMAL_PROG[] = R"(
    uniform float spacing;
    uniform float warpStrength;
    uniform vec2 shapeCenterPos;
    uniform vec2 centerPos;
    uniform shader shapeShader;

    vec2 normalizeWithLenSq(vec2 v, float lenSq)
    {
        return (lenSq > 1e-8) ? v * inversesqrt(lenSq) : vec2(0.0, 0.0);
    }

    vec2 warpedSamplePos(vec2 p)
    {
        vec2 delta = centerPos - shapeCenterPos;
        float centerDistSq = dot(delta, delta);
        if (centerDistSq <= 1e-8) {
            return p;
        }

        float centerDist = sqrt(centerDistSq);
        float centerGate = smoothstep(
            0.25 * spacing,
            0.25 * spacing + 0.35 * spacing,
            centerDist
        );
        if (centerGate <= 1e-5) {
            return p;
        }

        vec2 fromSmallCenter = p - centerPos;
        float localDistSq = dot(fromSmallCenter, fromSmallCenter);

        float warpRadius = max(spacing, 1e-6);
        float warpRadiusSq = warpRadius * warpRadius;

        if (localDistSq >= warpRadiusSq) {
            return p;
        }

        float falloff = 1.0 - smoothstep(0.0, warpRadiusSq, localDistSq);

        float hotValue = shapeShader.eval(centerPos).a;
        float hotGate = 1.0 - smoothstep(
            5.0,
            5.0 + 0.3 * spacing,
            hotValue
        );

        float warpAmount = warpStrength * falloff * hotGate * centerGate;
        if (warpAmount <= 1e-3) {
            return p;
        }

        vec2 dir = normalizeWithLenSq(delta, centerDistSq);
        return p - dir * warpAmount;
    }

    half4 main(vec2 p)
    {
        vec4 shape = shapeShader.eval(warpedSamplePos(p));
        return half4(shape.x, shape.y, 0.0, shape.a);
    }
)";

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFTransformShaderShape::GetGravPullDrawingShaderHasNormBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> gravPullShaderHasNormalBuilder = nullptr;
    if (gravPullShaderHasNormalBuilder) {
        return gravPullShaderHasNormalBuilder;
    }

    auto gravityPullShaderHasNormalEffect = Drawing::RuntimeEffect::CreateForShader(GRAVITY_PULL_NORMAL_PROG);
    if (!gravityPullShaderHasNormalEffect) {
        LOGE("GESDFTransformShaderShape::GetGravPullDrawingShaderHasNormBuilder effect error");
        return nullptr;
    }

    gravPullShaderHasNormalBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(gravityPullShaderHasNormalEffect);
    return gravPullShaderHasNormalBuilder;
}

std::shared_ptr<ShaderEffect> GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal(
    float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    
    auto baseShapeShader = params_.shape->GenerateDrawingShaderHasNormal(width, height);
    if (!baseShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal has empty base shader");
        return nullptr;
    }

    auto transformBuilder = GetSDFTransformShaderHasNormalShapeBuilder();
    if (!transformBuilder) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal has transform builder error");
        return nullptr;
    }
    auto transformedShapeShader = GenerateShaderEffect(width, height, baseShapeShader, transformBuilder);
    if (!transformedShapeShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal has transformed shader error");
        return nullptr;
    }
    
    auto gravityPullShaderBuilder = GetGravPullDrawingShaderHasNormBuilder();
    if (!gravityPullShaderBuilder) {
        LOGE("GESDFTransformShaderShape::GenerateGravPullDrawingShaderHasNormal has gravity pull shader builder error");
        return nullptr;
    }

    float shapeCenterX = 0.f;
    float shapeCenterY = 0.f;
    if (!TryGetCenter(shapeCenterX, shapeCenterY)) {
        LOGE("GESDFTransformShaderShape::GenerateGravPullDrawingShader get transformed shape center error");
    }

    gravityPullShaderBuilder->SetUniform("spacing", std::max(params_.spacing, 0.0001f));
    gravityPullShaderBuilder->SetUniform("warpStrength", params_.warpStrength);
    gravityPullShaderBuilder->SetUniform("shapeCenterPos", shapeCenterX, shapeCenterY);
    gravityPullShaderBuilder->SetUniform("centerPos", params_.centerPosition[0], params_.centerPosition[1]);
    gravityPullShaderBuilder->SetChild("shapeShader", transformedShapeShader);

    auto gravityPullShader = gravityPullShaderBuilder->MakeShader(nullptr, false);
    if (!gravityPullShader) {
        LOGE("GESDFTransformShaderShape::GenerateGravityPullDrawingShaderHasNormal gravity pull shader effect error");
        return nullptr;
    }
    
    return gravityPullShader;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS