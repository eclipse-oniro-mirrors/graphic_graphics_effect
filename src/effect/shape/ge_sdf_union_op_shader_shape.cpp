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

#include "ge_sdf_union_op_shader_shape.h"

#include "ge_log.h"
#include "ge_shader_diagnostics.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFUnionOpShaderShape::GenerateDrawingShader, Type: %s , Width: %g, Height: %g",
        params_.op == GESDFUnionOp::UNION ? "UNION" : "SMOOTH_UNION", width, height);

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
    return GenerateUnionOpDrawingShader(leftShader, rightShader, false);
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFUnionOpShaderShape::GenerateDrawingShaderHasNormal, Type: %s , Width: %g, Height: %g",
        params_.op == GESDFUnionOp::UNION ? "UNION" : "SMOOTH_UNION", width, height);
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

    return GenerateUnionOpDrawingShader(leftShader, rightShader, true);
}

void GESDFUnionOpShaderShape::Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal)
{
    if (params_.left) {
        params_.left->Preprocess(canvas, rect, hasNormal);
    }

    if (params_.right) {
        params_.right->Preprocess(canvas, rect, hasNormal);
    }
}

std::shared_ptr<ShaderEffect> GESDFUnionOpShaderShape::GenerateUnionOpDrawingShader(
    std::shared_ptr<ShaderEffect> leftShader, std::shared_ptr<ShaderEffect> rightShader, bool hasNormal) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = params_.op == GESDFUnionOp::UNION ? GetSDFUnionBuilder()
        : hasNormal ? GetSDFNormalSmoothUnionBuilder() : GetSDFSmoothUnionBuilder();
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

    static constexpr char prog[] = R"(
        uniform shader left;
        uniform shader right;
        half4 main(vec2 fragCoord)
        {
            vec4 leftShape = left.eval(fragCoord);
            vec4 rightShape = right.eval(fragCoord);

            // It is required for input SDF to be in this format: vec4(any, any, any, SDF)
            return leftShape.a < rightShape.a ? leftShape : rightShape;
        }
    )";

    auto sdfUnionShaderBuilderEffect = GECreateRuntimeEffectForShader(prog);
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

    static constexpr char prog[] = R"(
        uniform float spacing;
        uniform shader left;
        uniform shader right;

        vec4 sdfSmoothUnion(vec4 d1, vec4 d2, float k)
        {
            k*= 4.0;
            vec4 h = max(k - abs(d1 - d2), 0.0);
            return min(d1, d2) - h * h * 0.25 / k;
        }

        half4 main(vec2 fragCoord)
        {
            vec4 leftShape = left.eval(fragCoord);
            vec4 rightShape = right.eval(fragCoord);
            return sdfSmoothUnion(leftShape, rightShape, spacing);
        }
    )";

    auto sdfSmoothUnionShaderBuilderEffect = GECreateRuntimeEffectForShader(prog);
    if (!sdfSmoothUnionShaderBuilderEffect) {
        LOGE("GESDFUnionOpShaderShape::GetSDFSmoothUnionBuilder effect error");
        return nullptr;
    }

    sdfSmoothUnionShaderShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfSmoothUnionShaderBuilderEffect);
    return sdfSmoothUnionShaderShapeBuilder;
}

static constexpr char SDF_SMOOTH_UNION_NORMAL_PROG[] = R"(
    uniform float spacing;
    uniform shader left;
    uniform shader right;
    const float N_EPS = 1.0e-6;
    const float HIDDEN_TEST_EPS = 1.0;
    const float EXIT_STEP = 2.0;
    const int EXIT_STEPS = 20;
    const float HIDDEN_FIX_START = -0.5;
    const float HIDDEN_FIX_END = 1.5;
    vec2 safeNormalize(vec2 v, vec2 fallback)
    {
        float len2 = dot(v, v);
        return (len2 > N_EPS) ? v * inversesqrt(len2) : fallback;
    }
    float smoothUnionSdfValue(float da, float db, float k)
    {
        k = max(k, 0.0001) * 4.0;
        float h = max(k - abs(da - db), 0.0) / (2.0 * k);
        return min(da, db) - h * h * k;
    }
    float smoothUnionSdfAt(vec2 p)
    {
        vec4 d1 = left.eval(p);
        vec4 d2 = right.eval(p);
        return smoothUnionSdfValue(d1.a, d2.a, spacing);
    }
    bool outsideUnion(vec2 p)
    {
        return smoothUnionSdfAt(p) > 0.0;
    }
    void testExitDir(vec2 p, vec2 dir, inout float bestT, inout vec2 bestDir)
    {
        dir = safeNormalize(dir, bestDir);
        float t = EXIT_STEP;
        for (int i = 0; i < EXIT_STEPS; ++i) {
            vec2 q = p + dir * t;
            if (outsideUnion(q)) {
                if (t < bestT) {
                    bestT = t;
                    bestDir = dir;
                }
                break;
            }
            t += EXIT_STEP;
        }
    }
    vec2 firstExitGrad(vec2 p, vec2 gDominant, vec2 gOther, vec2 gSmooth)
    {
        vec2 bestDir = -gDominant;
        float bestT = 1000000.0;
        testExitDir(p, -gDominant, bestT, bestDir);
        testExitDir(p, gDominant, bestT, bestDir);
        testExitDir(p, gOther, bestT, bestDir);
        testExitDir(p, -gOther, bestT, bestDir);
        testExitDir(p, gSmooth, bestT, bestDir);
        testExitDir(p, -gSmooth, bestT, bestDir);
        return (bestT < 100000.0) ? bestDir : gSmooth;
    }
    float dominantBoundaryHidden(vec2 p, float dDom, vec2 gDom, bool dominantIsLeft)
    {
        vec2 q = p - dDom * gDom;
        vec2 qOut = q + HIDDEN_TEST_EPS * gDom;
        float otherD = dominantIsLeft ? right.eval(qOut).a : left.eval(qOut).a;
        return 1.0 - smoothstep(HIDDEN_FIX_START, HIDDEN_FIX_END, otherD);
    }
    
    const float N_SCALE = 2048.0;

    float EncodeDir(vec2 dir)
    {
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    vec2 DecodeDir(float z)
    {
        float xPos = floor(z);
        float yPos = (z - xPos) * N_SCALE * 2.0 - N_SCALE;
        xPos -= N_SCALE;
        return vec2(xPos, yPos);
    }

    vec4 sdgSmoothUnionAt(vec2 p, vec4 d1, vec4 d2, float k)
    {
        k = max(k, 0.0001) * 4.0;
        float da = d1.a;
        float db = d2.a;
        vec2 g1 = safeNormalize(d1.xy, vec2(0.0, 1.0));
        vec2 g2 = safeNormalize(d2.xy, g1);
        float h = max(k - abs(da - db), 0.0) / (2.0 * k);
        float t = (da < db) ? h : 1.0 - h;
        float d = min(da, db) - h * h * k;
        vec2 gSmooth = safeNormalize(mix(g1, g2, t), g1);

        vec2 centerDir1 = DecodeDir(d1.z);
        vec2 centerDir2 = DecodeDir(d2.z);
        vec2 centerDirUnion = mix(centerDir1, centerDir2, (d1.a < d2.a) ? h : 1.0 - h);
        float z = EncodeDir(centerDirUnion);
        bool dominantIsLeft = da < db;
        float dDom = dominantIsLeft ? da : db;
        vec2 gDom = dominantIsLeft ? g1 : g2;
        vec2 gOther = dominantIsLeft ? g2 : g1;
        float hiddenMask = dominantBoundaryHidden(p, dDom, gDom, dominantIsLeft);
        vec2 gFinal = gSmooth;
        if (hiddenMask > 0.001) {
            vec2 gExit = firstExitGrad(p, gDom, gOther, gSmooth);
            gFinal = safeNormalize(mix(gSmooth, gExit, hiddenMask), gSmooth);
        }
        return vec4(gFinal, z, d);
    }
    half4 main(vec2 fragCoord)
    {
        vec4 leftShape = left.eval(fragCoord);
        vec4 rightShape = right.eval(fragCoord);
        vec4 sdg = sdgSmoothUnionAt(fragCoord, leftShape, rightShape, spacing);
        return half4(sdg.x, sdg.y, sdg.z, sdg.w);
    }
)";

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFUnionOpShaderShape::GetSDFNormalSmoothUnionBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfNormalSmoothUnionShaderShapeBuilder = nullptr;
    if (sdfNormalSmoothUnionShaderShapeBuilder) {
        return sdfNormalSmoothUnionShaderShapeBuilder;
    }

    auto sdfNormalSmoothUnionShaderBuilderEffect =
        GECreateRuntimeEffectForShader(SDF_SMOOTH_UNION_NORMAL_PROG);
    if (!sdfNormalSmoothUnionShaderBuilderEffect) {
        LOGE("GESDFUnionOpShaderShape::GetSDFNormalSmoothUnionBuilder effect error");
        return nullptr;
    }

    sdfNormalSmoothUnionShaderShapeBuilder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(sdfNormalSmoothUnionShaderBuilderEffect);
    return sdfNormalSmoothUnionShaderShapeBuilder;
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
    builder->SetUniform("spacing", std::max(params_.spacing, 0.0001f));

    auto sdfSmoothUnionShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfSmoothUnionShapeShader) {
        LOGE("GESDFRRectShaderShape::GenerateSmoothUnionShaderEffect shaderEffect error");
    }
    return sdfSmoothUnionShapeShader;
}

bool GESDFUnionOpShaderShape::HasType(const GESDFShapeType type) const
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

bool GESDFUnionOpShaderShape::GetInscribedRect(Rect& rect)
{
    Rect leftRect;
    Rect rightRect;
    bool leftSupport = params_.left ? params_.left->GetInscribedRect(leftRect) : false;
    bool rightSupport = params_.right ? params_.right->GetInscribedRect(rightRect) : false;
    if (!leftSupport && !rightSupport) {
        return false;
    }

    // Use the inscribed rect of one shape as the inscribed rect for the union result
    // Take the union of two inscribed rects should be better
    if (leftSupport) {
        rect = leftRect;
    } else {
        rect = rightRect;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS