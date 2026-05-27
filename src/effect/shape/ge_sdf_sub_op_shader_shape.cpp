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

#include "ge_sdf_sub_op_shader_shape.h"

#include <algorithm>

#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr float MIN_SMOOTH_SUB_SPACING = 0.0001f;

static constexpr char SDF_NORMAL_SMOOTH_SUB_OP_PROG[] = R"(
    uniform float spacing;
    uniform shader left;
    uniform shader right;

    vec4 sdgSmoothSub(vec4 d1, vec4 d2, float k)
    {
        vec4 inverseD1 = vec4(-d1.xyz, -d1.a);
        float h = max(k - abs(inverseD1.a - d2.a), 0.0) / (2.0 * k);
        return vec4(mix(inverseD1.xyz, d2.xyz, (inverseD1.a > d2.a) ? h : 1.0 - h),
            max(inverseD1.a, d2.a) + h * h * k);
    }

    half4 main(vec2 fragCoord)
    {
        vec4 leftShape = left.eval(fragCoord);
        vec4 rightShape = right.eval(fragCoord);
        return sdgSmoothSub(leftShape, rightShape, spacing);
    }
)";

static constexpr char SDF_SMOOTH_SUB_OP_PROG[] = R"(
    uniform float spacing;
    uniform shader left;
    uniform shader right;

    vec4 sdfSmoothSub(vec4 d1, vec4 d2, float k)
    {
        float h = max(k - abs(-d1.a - d2.a), 0.0);
        return vec4(d2.xyz, max(-d1.a, d2.a) + h * h * 0.25 / k);
    }

    half4 main(vec2 fragCoord)
    {
        vec4 leftShape = left.eval(fragCoord);
        vec4 rightShape = right.eval(fragCoord);
        return sdfSmoothSub(leftShape, rightShape, spacing);
    }
)";
}

std::shared_ptr<ShaderEffect> GESDFSubOpShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFSubOpShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    auto leftShader = params_.left ? params_.left->GenerateDrawingShader(width, height) : nullptr;
    auto rightShader = params_.right ? params_.right->GenerateDrawingShader(width, height) : nullptr;
    if (!leftShader && !rightShader) {
        return nullptr;
    }
    if (!leftShader) {
        return rightShader;
    }
    if (!rightShader) {
        return leftShader;
    }
    return GenerateSubOpDrawingShader(leftShader, rightShader, false);
}

std::shared_ptr<ShaderEffect> GESDFSubOpShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFSubOpShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g", width, height);
    auto leftShader = params_.left ? params_.left->GenerateDrawingShaderHasNormal(width, height) : nullptr;
    auto rightShader = params_.right ? params_.right->GenerateDrawingShaderHasNormal(width, height) : nullptr;
    if (!leftShader && !rightShader) {
        return nullptr;
    }
    if (!leftShader) {
        return rightShader;
    }
    if (!rightShader) {
        return leftShader;
    }
    return GenerateSubOpDrawingShader(leftShader, rightShader, true);
}

void GESDFSubOpShaderShape::Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal)
{
    if (params_.left) {
        params_.left->Preprocess(canvas, rect, hasNormal);
    }
    if (params_.right) {
        params_.right->Preprocess(canvas, rect, hasNormal);
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFSubOpShaderShape::GetSDFSubOpBuilder(bool) const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfSubOpShaderShapeBuilder = nullptr;
    if (sdfSubOpShaderShapeBuilder) {
        return sdfSubOpShaderShapeBuilder;
    }

    static constexpr char prog[] = R"(
        uniform shader left;
        uniform shader right;

        half4 main(vec2 fragCoord)
        {
            vec4 leftShape = left.eval(fragCoord);
            vec4 rightShape = right.eval(fragCoord);
            vec4 inverseLeft = vec4(-leftShape.xyz, -leftShape.a);

            // ShapeX maps to d1 and ShapeY maps to d2: max(-d1, d2).
            return inverseLeft.a > rightShape.a ? inverseLeft : rightShape;
        }
    )";

    auto sdfSubOpShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfSubOpShaderBuilderEffect) {
        LOGE("GESDFSubOpShaderShape::GetSDFSubOpBuilder effect error");
        return nullptr;
    }

    sdfSubOpShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfSubOpShaderBuilderEffect);
    return sdfSubOpShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFSubOpShaderShape::GenerateSubOpDrawingShader(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const
{
    auto builder = GetSDFSubOpBuilder(hasNormal);
    if (!builder) {
        LOGE("GESDFSubOpShaderShape::GenerateSubOpDrawingShader builder error");
        return nullptr;
    }
    builder->SetChild("left", leftShader);
    builder->SetChild("right", rightShader);

    auto sdfSubOpShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfSubOpShapeShader) {
        LOGE("GESDFSubOpShaderShape::GenerateSubOpDrawingShader shaderEffect error");
    }
    return sdfSubOpShapeShader;
}

bool GESDFSubOpShaderShape::HasType(const GESDFShapeType type) const
{
    if (type == GetSDFShapeType()) {
        return true;
    }
    if (params_.left ? params_.left->HasType(type) : false) {
        return true;
    }
    if (params_.right ? params_.right->HasType(type) : false) {
        return true;
    }
    return false;
}

std::shared_ptr<ShaderEffect> GESDFSmoothSubOpShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFSmoothSubOpShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    auto leftShader = params_.left ? params_.left->GenerateDrawingShader(width, height) : nullptr;
    auto rightShader = params_.right ? params_.right->GenerateDrawingShader(width, height) : nullptr;
    if (!leftShader && !rightShader) {
        return nullptr;
    }
    if (!leftShader) {
        return rightShader;
    }
    if (!rightShader) {
        return leftShader;
    }
    return GenerateSmoothSubOpDrawingShader(leftShader, rightShader, false);
}

std::shared_ptr<ShaderEffect> GESDFSmoothSubOpShaderShape::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    GE_TRACE_NAME_FMT(
        "GESDFSmoothSubOpShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g", width, height);
    auto leftShader = params_.left ? params_.left->GenerateDrawingShaderHasNormal(width, height) : nullptr;
    auto rightShader = params_.right ? params_.right->GenerateDrawingShaderHasNormal(width, height) : nullptr;
    if (!leftShader && !rightShader) {
        return nullptr;
    }
    if (!leftShader) {
        return rightShader;
    }
    if (!rightShader) {
        return leftShader;
    }
    return GenerateSmoothSubOpDrawingShader(leftShader, rightShader, true);
}

void GESDFSmoothSubOpShaderShape::Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal)
{
    if (params_.left) {
        params_.left->Preprocess(canvas, rect, hasNormal);
    }
    if (params_.right) {
        params_.right->Preprocess(canvas, rect, hasNormal);
    }
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFSmoothSubOpShaderShape::GetSDFSmoothSubOpBuilder(
    bool hasNormal) const
{
    if (hasNormal) {
        thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfNormalSmoothSubOpShaderShapeBuilder = nullptr;
        if (sdfNormalSmoothSubOpShaderShapeBuilder) {
            return sdfNormalSmoothSubOpShaderShapeBuilder;
        }

        auto sdfNormalSmoothSubOpShaderBuilderEffect =
            Drawing::RuntimeEffect::CreateForShader(SDF_NORMAL_SMOOTH_SUB_OP_PROG);
        if (!sdfNormalSmoothSubOpShaderBuilderEffect) {
            LOGE("GESDFSmoothSubOpShaderShape::GetSDFSmoothSubOpBuilder normal effect error");
            return nullptr;
        }

        sdfNormalSmoothSubOpShaderShapeBuilder =
            std::make_shared<Drawing::RuntimeShaderBuilder>(sdfNormalSmoothSubOpShaderBuilderEffect);
        return sdfNormalSmoothSubOpShaderShapeBuilder;
    }

    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfSmoothSubOpShaderShapeBuilder = nullptr;
    if (sdfSmoothSubOpShaderShapeBuilder) {
        return sdfSmoothSubOpShaderShapeBuilder;
    }

    auto sdfSmoothSubOpShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(SDF_SMOOTH_SUB_OP_PROG);
    if (!sdfSmoothSubOpShaderBuilderEffect) {
        LOGE("GESDFSmoothSubOpShaderShape::GetSDFSmoothSubOpBuilder effect error");
        return nullptr;
    }

    sdfSmoothSubOpShaderShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfSmoothSubOpShaderBuilderEffect);
    return sdfSmoothSubOpShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFSmoothSubOpShaderShape::GenerateSmoothSubOpDrawingShader(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const
{
    auto builder = GetSDFSmoothSubOpBuilder(hasNormal);
    if (!builder) {
        LOGE("GESDFSmoothSubOpShaderShape::GenerateSmoothSubOpDrawingShader builder error");
        return nullptr;
    }
    builder->SetChild("left", leftShader);
    builder->SetChild("right", rightShader);
    builder->SetUniform("spacing", std::max(params_.spacing, MIN_SMOOTH_SUB_SPACING));

    auto sdfSmoothSubOpShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfSmoothSubOpShapeShader) {
        LOGE("GESDFSmoothSubOpShaderShape::GenerateSmoothSubOpDrawingShader shaderEffect error");
    }
    return sdfSmoothSubOpShapeShader;
}

bool GESDFSmoothSubOpShaderShape::HasType(const GESDFShapeType type) const
{
    if (type == GetSDFShapeType()) {
        return true;
    }
    if (params_.left ? params_.left->HasType(type) : false) {
        return true;
    }
    if (params_.right ? params_.right->HasType(type) : false) {
        return true;
    }
    return false;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
