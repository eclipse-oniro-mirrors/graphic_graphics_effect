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

#include "sdf/ge_sdf_union_op_shader_shape.h"
#include "utils/ge_trace.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void GESDFUnionOpShaderShape::CopyState(const GESDFUnionOpShaderShape& shape)
{
    params_.spacing = shape.params_.spacing;
    params_.op = shape.params_.op;
    if (params_.left) {
        params_.left->CopyState(*static_cast<const GESDFShaderShape*>(shape.params_.left.get()));
    }

    if (params_.right) {
        params_.right->CopyState(*static_cast<const GESDFShaderShape*>(shape.params_.right.get()));
    }
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFUnionOpShaderShape::GenerateDrawingShader, Type: %s , Width: %g, Height: %g",
        params_.op == GESDFUnionOp::UNION ? "UNION" : "SMOOTH_UNION", width, height);

    if (params_.left == nullptr && params_.right == nullptr) {
        return nullptr;
    }

    if (!params_.left) {
        return params_.right->GenerateDrawingShader(width, height);
    }

    if (!params_.right) {
        return params_.left->GenerateDrawingShader(width, height);
    }

    auto leftShader = params_.left->GenerateDrawingShader(width, height);
    auto rightShader = params_.right->GenerateDrawingShader(width, height);
    
    return GenerateUnionOpDrawingShader(leftShader, rightShader);
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFUnionOpShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g", width, height);

    if (params_.left == nullptr && params_.right == nullptr) {
        return nullptr;
    }

    if (!params_.left) {
        return params_.right->GenerateDrawingShaderHasNormal(width, height);
    }

    if (!params_.right) {
        return params_.left->GenerateDrawingShaderHasNormal(width, height);
    }

    auto leftShader = params_.left->GenerateDrawingShaderHasNormal(width, height);
    auto rightShader = params_.right->GenerateDrawingShaderHasNormal(width, height);
    
    return GenerateUnionOpDrawingShader(leftShader, rightShader);
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateUnionOpDrawingShader(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;

    builder = params_.op == GESDFUnionOp::UNION ? GetSDFUnionBuilder() : GetSDFSmoothUnionBuilder();
    if (!builder) {
        LOGE("GESDFUnionOpShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    return params_.op == GESDFUnionOp::UNION ? GenerateUnionShaderEffect(leftShader, rightShader, builder)
                                             : GenerateSmoothUnionShaderEffect(leftShader, rightShader, builder);
}


std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFUnionOpShaderShape::GetSDFUnionBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfUnionShaderShapeBuilder = nullptr;
    if (sdfUnionShaderShapeBuilder) {
        return sdfUnionShaderShapeBuilder;
    }

    //todo 为了通用性，可以让sdf + normal都用，用的三通道union，在只有sdf的时候是浪费的
    static constexpr char prog[] = R"(
        uniform shader left;
        uniform shader right;
        half4 main(vec2 fragCoord)
        {
            vec4 leftShape = left.eval(fragCoord);
            vec4 rightShape = right.eval(fragCoord);
            return min(leftShape, rightShape);
        }
    )";

    auto sdfUnionShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfUnionShaderBuilderEffect) {
        LOGE("GESDFUnionOpShaderShape::GetSDFUnionBuilder effect error");
        return nullptr;
    }

    sdfUnionShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfUnionShaderBuilderEffect);
    return sdfUnionShaderShapeBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFUnionOpShaderShape::GetSDFSmoothUnionBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfSmoothUnionShaderShapeBuilder = nullptr;
    if (sdfSmoothUnionShaderShapeBuilder) {
        return sdfSmoothUnionShaderShapeBuilder;
    }

    //todo 为了通用性，可以让sdf + normal都用，用的三通道union，在只有sdf的时候是浪费的
    static constexpr char prog[] = R"(
        uniform float spacing;
        uniform shader left;
        uniform shader right;

        vec4 sdfSmoothUnion(vec4 d1, vec4 d2, float k)
        {
            k*= 4.0;
            vec4 h = max(k - abs(d1 - d2), 0.0);
            return min(d1, d2) - h*h*0.25 / k;
        }

        half4 main(vec2 fragCoord)
        {
            vec4 leftShape = left.eval(fragCoord);
            vec4 rightShape = right.eval(fragCoord);
            return sdfSmoothUnion(leftShape, rightShape, spacing);
        }
    )";

    auto sdfSmoothUnionShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!sdfSmoothUnionShaderBuilderEffect) {
        LOGE("GESDFUnionOpShaderShape::GetSDFSmoothUnionBuilder effect error");
        return nullptr;
    }

    sdfSmoothUnionShaderShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfSmoothUnionShaderBuilderEffect);
    return sdfSmoothUnionShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateUnionShaderEffect(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFUnionOpShaderShape::GenerateUnionShaderEffect builder error");
        return nullptr;
    }

    builder->SetChild("left", leftShader);
    builder->SetChild("right", rightShader);

    auto sdfUnionShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfUnionShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateUnionShaderEffect shaderEffect error");
    }
    return sdfUnionShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateSmoothUnionShaderEffect(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    if (!builder) {
        LOGE("GESDFUnionOpShaderShape::GenerateSmoothUnionShaderEffect builder error");
        return nullptr;
    }

    builder->SetChild("left", leftShader);
    builder->SetChild("right", rightShader);
    builder->SetUniform("spacing", params_.spacing);

    auto sdfSmoothUnionShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfSmoothUnionShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateSmoothUnionShaderEffect shaderEffect error");
    }
    return sdfSmoothUnionShapeShader;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
