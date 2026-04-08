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

#include "ge_sdf_distort_op_shader_shape.h"

#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffect> GESDFDistortOpShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFDistortOpShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    auto shapeShader = params_.shape ? params_.shape->GenerateDrawingShader(width, height) : nullptr;
    if (!shapeShader) {
        return nullptr;
    }
    return GenerateDistortOpDrawingShader(shapeShader, false, width, height);
}

std::shared_ptr<ShaderEffect> GESDFDistortOpShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFDistortOpShaderShape::GenerateDrawingShaderHasNormal, Width: %g, Height: %g",
        width, height);
    auto shapeShader = params_.shape ? params_.shape->GenerateDrawingShader(width, height) : nullptr;
    if (!shapeShader) {
        return nullptr;
    }
    return GenerateDistortOpDrawingShader(shapeShader, true, width, height);
}

std::shared_ptr<ShaderEffect> GESDFDistortOpShaderShape::GenerateDistortOpDrawingShader(
    std::shared_ptr<ShaderEffect> shapeShader, bool hasNormal, float width, float height) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetSDFDistortBuilder();
    if (!builder) {
        LOGE("GESDFDistortOpShaderShape::GenerateDrawingShader has builder error");
        return nullptr;
    }
    return GenerateDistortShaderEffect(shapeShader, builder, width, height);
}

constexpr char SHADER[] = R"(
    uniform shader shape;

    uniform vec2 lu;
    uniform vec2 ru;
    uniform vec2 rb;
    uniform vec2 lb;

    uniform vec4 barrelDistortion;

    uniform vec2 iResolution;

    float cross2d(vec2 a, vec2 b) {
        return a.x*b.y - a.y*b.x;
    }

    vec2 invBilinear(vec2 p, vec2 a, vec2 b, vec2 c, vec2 d) {
        vec2 res = vec2(-1.0);

        vec2 e = b-a;
        vec2 f = d-a;
        vec2 g = a-b+c-d;
        vec2 h = p-a;

        float k2 = cross2d(g, f);
        float k1 = cross2d(e, f) + cross2d(h, g);
        float k0 = cross2d(h, e);

        if (abs(k2) < 0.001) {
            res = vec2((h.x*k1+f.x*k0)/(e.x*k1-g.x*k0), -k0/k1);
        } else {
            float w = k1*k1 - 4.0*k0*k2;
            if (w < 0.0) return vec2(-1.0);
            w = sqrt(w);

            float ik2 = 0.5 / k2;
            float v = (-k1 - w) * ik2;
            float u = (h.x - f.x*v) / (e.x + g.x*v);

            if (u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0)
            {
                v = (-k1 + w) * ik2;
                u = (h.x - f.x*v) / (e.x + g.x*v);
            }
            res = vec2(u, v);
        }

        return res;
    }

    half4 main(vec2 fragCoord)
    {
        vec2 uv = fragCoord / iResolution.xy;
        vec2 newUV = invBilinear(uv, lu, ru, rb, lb);

        // Distortion
        {
            vec2 lerpDistortion = vec2(mix(barrelDistortion.x, barrelDistortion.y, newUV.x),
                                       mix(barrelDistortion.z, barrelDistortion.w, newUV.y));
            vec2 centerNewUV = newUV - vec2(0.5);
            vec2 normFactor = 1.0 / (1.0 + lerpDistortion * 0.5); // makes the corners align with params
            float l = length(centerNewUV);
            centerNewUV *= 1.0 + lerpDistortion*l*l;
            centerNewUV *= normFactor;
            newUV = centerNewUV + vec2(0.5);
        }

        vec2 newCoord = newUV * iResolution;
        return shape.eval(newCoord);
    }
)";

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFDistortOpShaderShape::GetSDFDistortBuilder() const
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfDistortShaderShapeBuilder = nullptr;
    if (sdfDistortShaderShapeBuilder) {
        return sdfDistortShaderShapeBuilder;
    }

    auto sdfDistortShaderBuilderEffect = Drawing::RuntimeEffect::CreateForShader(SHADER);
    if (!sdfDistortShaderBuilderEffect) {
        LOGE("GESDFDistortOpShaderShape::GetSDFDistortBuilder effect error");
        return nullptr;
    }

    sdfDistortShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfDistortShaderBuilderEffect);
    return sdfDistortShaderShapeBuilder;
}

std::shared_ptr<ShaderEffect> GESDFDistortOpShaderShape::GenerateDistortShaderEffect(
    std::shared_ptr<ShaderEffect> shapeShader, std::shared_ptr<Drawing::RuntimeShaderBuilder> builder, float width,
    float height) const
{
    if (!builder) {
        LOGE("GESDFDistortOpShaderShape::GenerateDistortShaderEffect builder error");
        return nullptr;
    }

    builder->SetChild("shape", shapeShader);

    builder->SetUniform("lu", params_.LUCorner.GetX(), params_.LUCorner.GetY());
    builder->SetUniform("ru", params_.RUCorner.GetX(), params_.RUCorner.GetY());
    builder->SetUniform("rb", params_.RBCorner.GetX(), params_.RBCorner.GetY());
    builder->SetUniform("lb", params_.LBCorner.GetX(), params_.LBCorner.GetY());

    const int barrelDistortionSize = 4;
    float barrelDistortion[] = {params_.barrelDistortion[0], params_.barrelDistortion[1],
        params_.barrelDistortion[2], params_.barrelDistortion[3]};
    builder->SetUniform("barrelDistortion", barrelDistortion, barrelDistortionSize);
    builder->SetUniform("iResolution", width, height);

    auto sdfDistortShapeShader = builder->MakeShader(nullptr, false);
    if (!sdfDistortShapeShader) {
        LOGE("GESDFDistortOpShaderShape::GenerateDistortShaderEffect shaderEffect error");
    }
    return sdfDistortShapeShader;
}

bool GESDFDistortOpShaderShape::HasType(const GESDFShapeType type) const
{
    if (type == GetSDFShapeType()) {
        return true;
    }
    if (params_.shape ? params_.shape->HasType(type) : false) {
        return true;
    }
    return false;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
