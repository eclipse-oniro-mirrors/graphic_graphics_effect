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

#include "ge_sdf_uber_basic_shape.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include "ge_log.h"
#include "ge_sdf_ellipse_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_triangle_shader_shape.h"
#include "ge_shader_diagnostics.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr float HALF = 0.5;
constexpr float EXTEND = 0.5; // Fixing edge difference between SDF and Skia RRect
constexpr float MIN_SIZE = 0.0001f;
constexpr float SPACING_MIN = 0.0001f;

constexpr int VEC4_FLOATS = 4;

float ClampCornerRadius(float radius, float maxRadius)
{
    return std::clamp(radius, 0.0f, maxRadius);
}

// Common uniforms + per-shape SDF math; per-leaf u_matA=(m0..m3),
// u_matB=(m4,m5,shapeType), u_matC=(p0..p3), u_geo=(p4..p7).
constexpr char UBER_BASICSHAPE_COMMON_PROG[] = R"(
    uniform float u_count;
    uniform float u_spacing;
    uniform vec4 u_matA[30];
    uniform vec4 u_matB[30];
    uniform vec4 u_matC[30];
    uniform vec4 u_geo[30];

    const float N_EPS = 1e-6;
    const float N_SCALE = 2048.0;

    vec2 safeNorm(vec2 v)
    {
        return v / max(length(v), N_EPS);
    }

    // ---- Ellipse: verbatim from the ellipse SDF_GRAD_PROG (EPS == N_EPS); the no-normal path reuses sdEllipse ----
    float sdEllipse(vec2 p, vec2 halfSize)
    {
        vec2 safeHalfSize = max(halfSize, vec2(N_EPS));
        vec2 invR = p / safeHalfSize;
        vec2 invR2 = p / max(safeHalfSize * safeHalfSize, vec2(N_EPS));
        float k0 = length(invR);
        if (k0 < N_EPS) {
            return -min(safeHalfSize.x, safeHalfSize.y);
        }
        float k1 = max(length(invR2), N_EPS);
        return k0 * (k0 - 1.0) / k1;
    }

    vec2 ellipseGrad(vec2 p, vec2 halfSize)
    {
        vec2 safeHalfSize = max(halfSize, vec2(N_EPS));
        vec2 scaled = p / safeHalfSize;
        float scaledLen = length(scaled);
        if (scaledLen < N_EPS) {
            return vec2(0.0, -1.0);
        }
        return safeNorm(p / max(safeHalfSize * safeHalfSize * scaledLen, vec2(N_EPS)));
    }

    // ---- RRect general (G1 non-uniform): sdg = normal / sdf = no-normal, verbatim from the general SDF_GRAD_PROG ----
    vec2 selectCornerRadius(vec2 p, vec2 radiusTL, vec2 radiusTR, vec2 radiusBR, vec2 radiusBL)
    {
        float isRight = step(0.0, p.x);
        float isBottom = step(0.0, p.y);
        vec2 topRadius = mix(radiusTL, radiusTR, isRight);
        vec2 bottomRadius = mix(radiusBL, radiusBR, isRight);
        return mix(topRadius, bottomRadius, isBottom);
    }

    vec3 sdgRRectGeneral(vec2 p, vec2 b, vec2 radiusTL, vec2 radiusTR, vec2 radiusBR, vec2 radiusBL)
    {
        vec2 r = selectCornerRadius(p, radiusTL, radiusTR, radiusBR, radiusBL);
        vec2 s = sign(p);
        vec2 local = abs(p);
        vec2 cornerCenter = b - r;
        vec2 cornerPos = local - cornerCenter;
        float useCorner = step(0.0, min(cornerPos.x, cornerPos.y));

        float edgeSd = max(local.x - b.x, local.y - b.y);
        float cornerSd = sdEllipse(cornerPos, r);
        float sd = mix(edgeSd, cornerSd, useCorner);

        vec2 edgeDelta = local - b;
        float pickX = step(edgeDelta.y, edgeDelta.x);
        vec2 gradEdge = vec2(pickX, 1.0 - pickX);
        vec2 gradCorner = ellipseGrad(cornerPos, r);
        vec2 grad = s * mix(gradEdge, gradCorner, useCorner);
        return vec3(sd, grad);
    }

    float sdfRRectGeneral(vec2 p, vec2 b, vec2 radiusTL, vec2 radiusTR, vec2 radiusBR, vec2 radiusBL)
    {
        vec2 local = abs(p);
        vec2 radius = selectCornerRadius(p, radiusTL, radiusTR, radiusBR, radiusBL);
        vec2 cornerPos = local - (b - radius);
        if (cornerPos.x >= 0.0 && cornerPos.y >= 0.0) {
            return sdEllipse(cornerPos, radius);
        }
        return max(local.x - b.x, local.y - b.y);
    }

    // ---- RRect uniform fast path (G1_UNIFORM / G2 / V / H): sdg = normal / sdf = no-normal,
    // verbatim from the uniform-radius SDF_GRAD_PROGs. These kinds have uniform circular radii
    // by ClassifyUberRRect construction, so the four packed radii are identical and p4 carries
    // the radius; gradOut reuses the already computed l instead of a second safeNorm length. ----
    vec3 sdgRRectUniform(vec2 p, vec2 b, float r)
    {
        vec2 s = sign(p);
        vec2 w = abs(p) - b + r;
        float g = max(w.x, w.y);
        vec2 q = max(w, 0.0);
        float l = length(q);
        float outside = step(0.0, g);
        float sd = mix(g, l, outside) - r;
        float pickX = step(w.y, w.x);
        vec2 gradIn = vec2(pickX, 1.0 - pickX);
        vec2 gradOut = q / max(l, N_EPS);
        vec2 grad = s * mix(gradIn, gradOut, outside);
        return vec3(sd, grad);
    }

    float sdfRRectUniform(vec2 coord, vec2 p, vec2 b, float r)
    {
        vec2 d = abs(coord - p) - (b - r);
        return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
    }

    // ---- Triangle: sdg = normal / sdf = no-normal, verbatim from the triangle SDF_GRAD_PROG ----
    float sign2(vec2 p, vec2 v0, vec2 v1)
    {
        return (p.x - v1.x) * (v0.y - v1.y) - (v0.x - v1.x) * (p.y - v1.y);
    }

    vec3 sdgTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2)
    {
        vec2 e0 = v1 - v0;
        vec2 e1 = v2 - v1;
        float crossValue = e0.x * e1.y - e0.y * e1.x;

        if (abs(crossValue) < 1e-6) {
            float d = length(p - v0);
            d = max(d, 1e-6);
            return vec3(d, (p - v0) / d);
        }
        bool isClockwise = crossValue > 0.0;

        vec2 adjustedV1 = isClockwise ? v1 : v2;
        vec2 adjustedV2 = isClockwise ? v2 : v1;

        vec2 ae0 = adjustedV1 - v0;
        vec2 ae1 = adjustedV2 - adjustedV1;
        vec2 ae2 = v0 - adjustedV2;

        vec2 v0p = p - v0;
        vec2 v1p = p - adjustedV1;
        vec2 v2p = p - adjustedV2;

        vec2 pq0 = v0p - ae0 * clamp(dot(v0p, ae0) / dot(ae0, ae0), 0.0, 1.0);
        vec2 pq1 = v1p - ae1 * clamp(dot(v1p, ae1) / dot(ae1, ae1), 0.0, 1.0);
        vec2 pq2 = v2p - ae2 * clamp(dot(v2p, ae2) / dot(ae2, ae2), 0.0, 1.0);

        float d0 = dot(pq0, pq0);
        float d1 = dot(pq1, pq1);
        float d2 = dot(pq2, pq2);

        float d = min(min(d0, d1), d2);

        vec2 pq = (d == d0) ? pq0 : ((d == d1) ? pq1 : pq2);

        float s0 = sign2(p, v0, adjustedV1);
        float s1 = sign2(p, adjustedV1, adjustedV2);
        float s2 = sign2(p, adjustedV2, v0);

        bool inside = (s0 >= 0.0) && (s1 >= 0.0) && (s2 >= 0.0);

        float sd = sqrt(d) * (inside ? -1.0 : 1.0);
        vec2 grad = safeNorm(pq) * (inside ? -1.0 : 1.0);

        return vec3(sd, grad);
    }

    vec3 sdgRoundedTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2, float r)
    {
        float a = length(v1 - v0);
        float b = length(v2 - v1);
        float c = length(v0 - v2);
        float perimeter = a + b + c;

        vec2 incenter = (b * v0 + c * v1 + a * v2) / perimeter;
        float area = abs((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y)) * 0.5;
        float inradius = area / (perimeter * 0.5);

        float clampedR = clamp(r, 0.0, inradius);

        float scale = (inradius - clampedR) / inradius;
        vec2 v0p = incenter + (v0 - incenter) * scale;
        vec2 v1p = incenter + (v1 - incenter) * scale;
        vec2 v2p = incenter + (v2 - incenter) * scale;

        vec3 sdg = sdgTriangle(p, v0p, v1p, v2p);

        return vec3(sdg.x - clampedR, sdg.yz);
    }

    float sdfTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2)
    {
        vec2 e0 = v1 - v0;
        vec2 e1 = v2 - v1;
        float crossValue = e0.x * e1.y - e0.y * e1.x;

        if (abs(crossValue) < 1e-6) {
            return length(p - v0);
        }
        bool isClockwise = crossValue > 0.0;

        vec2 adjustedV1 = isClockwise ? v1 : v2;
        vec2 adjustedV2 = isClockwise ? v2 : v1;

        vec2 ae0 = adjustedV1 - v0;
        vec2 ae1 = adjustedV2 - adjustedV1;
        vec2 ae2 = v0 - adjustedV2;

        vec2 v0p = p - v0;
        vec2 v1p = p - adjustedV1;
        vec2 v2p = p - adjustedV2;

        vec2 pq0 = v0p - ae0 * clamp(dot(v0p, ae0) / dot(ae0, ae0), 0.0, 1.0);
        vec2 pq1 = v1p - ae1 * clamp(dot(v1p, ae1) / dot(ae1, ae1), 0.0, 1.0);
        vec2 pq2 = v2p - ae2 * clamp(dot(v2p, ae2) / dot(ae2, ae2), 0.0, 1.0);

        float d = min(min(dot(pq0, pq0), dot(pq1, pq1)), dot(pq2, pq2));

        float s0 = sign2(p, v0, adjustedV1);
        float s1 = sign2(p, adjustedV1, adjustedV2);
        float s2 = sign2(p, adjustedV2, v0);

        bool inside = (s0 >= 0.0) && (s1 >= 0.0) && (s2 >= 0.0);

        return sqrt(d) * (inside ? -1.0 : 1.0);
    }

    float sdfRoundedTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2, float r)
    {
        float a = length(v1 - v0);
        float b = length(v2 - v1);
        float c = length(v0 - v2);
        float perimeter = a + b + c;

        vec2 incenter = (b * v0 + c * v1 + a * v2) / perimeter;
        float area = abs((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y)) * 0.5;
        float inradius = area / (perimeter * 0.5);

        float clampedR = clamp(r, 0.0, inradius);
        float scale = (inradius - clampedR) / inradius;
        vec2 v0p = incenter + (v0 - incenter) * scale;
        vec2 v1p = incenter + (v1 - incenter) * scale;
        vec2 v2p = incenter + (v2 - incenter) * scale;

        return sdfTriangle(p, v0p, v1p, v2p) - clampedR;
    }

    // ---- Shared encode / fusion: sdg = normal / sdf = no-normal ----
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

    vec4 sdgSmoothUnion(vec4 d1, vec4 d2, float k)
    {
        k *= 4.0;
        float h = max(k - abs(d1.a - d2.a), 0.0) / (2.0 * k);
        vec2 centerDir1 = DecodeDir(d1.z);
        vec2 centerDir2 = DecodeDir(d2.z);
        vec2 centerDirUnion = mix(centerDir1, centerDir2, (d1.a < d2.a) ? h : 1.0 - h);
        float encodingZ = EncodeDir(centerDirUnion);
        return vec4(mix(d1.xy, d2.xy, (d1.a < d2.a) ? h : 1.0 - h), encodingZ, min(d1.a, d2.a) - h * h * k);
    }

    float sdfSmoothUnion(float d1, float d2, float k)
    {
        k *= 4.0;
        float h = max(k - abs(d1 - d2), 0.0) / (2.0 * k);
        return min(d1, d2) - h * h * k;
    }

    // Pure leaf evaluation, no array access: the caller passes the per-leaf
    // params as arguments, dispatching on the shape type.
    //   RRect G1 non-uniform: c = (centerX, centerY, halfW, halfH), g = (radiusTL, radiusTR, radiusBR, radiusBL)
    //   RRect uniform kinds (G1_UNIFORM/G2/V/H): same layout, the four radii are identical, g.x carries the radius
    //   Triangle:  c = (v0.x, v0.y, v1.x, v1.y), g = (v2.x, v2.y, radius, 0)
    //   Ellipse:   c = (centerX, centerY, width, height)
    vec4 EvalBasicShapeLeafCore(vec2 local, float shapeType, vec4 c, vec4 g)
    {
        if (shapeType < 0.5) {
            // G1 non-uniform: per-corner general evaluation.
            vec2 posFromCenter = local - c.xy;
            vec3 sdg = sdgRRectGeneral(posFromCenter, c.zw, vec2(g.x), vec2(g.y), vec2(g.z), vec2(g.w));
            return vec4(sdg.yz, EncodeDir(posFromCenter), sdg.x);
        }
        if (shapeType < 4.5) {
            // G1_UNIFORM / G2 / V / H capsules: uniform circular radii by
            // ClassifyUberRRect construction, one shared fast evaluation.
            vec2 posFromCenter = local - c.xy;
            vec3 sdg = sdgRRectUniform(posFromCenter, c.zw, g.x);
            return vec4(sdg.yz, EncodeDir(posFromCenter), sdg.x);
        }
        if (shapeType < 5.5) {
            vec3 sdg = sdgRoundedTriangle(local, c.xy, c.zw, g.xy, g.z);
            vec2 centerPos = (c.xy + c.zw + g.xy) / 3.0;
            return vec4(sdg.yz, EncodeDir(local - centerPos), sdg.x);
        }
        vec2 posFromCenter = local - c.xy;
        vec2 halfSize = c.zw * 0.5;
        float sdf = sdEllipse(posFromCenter, halfSize);
        vec2 grad = ellipseGrad(posFromCenter, halfSize);
        return vec4(grad, EncodeDir(posFromCenter), sdf);
    }

    // SDF-only dispatch for the no-normal mains, same kinds as above.
    float EvalBasicShapeLeafSdf(vec2 local, float shapeType, vec4 c, vec4 g)
    {
        if (shapeType < 0.5) {
            vec2 posFromCenter = local - c.xy;
            return sdfRRectGeneral(posFromCenter, c.zw, vec2(g.x), vec2(g.y), vec2(g.z), vec2(g.w));
        }
        if (shapeType < 4.5) {
            return sdfRRectUniform(local, c.xy, c.zw, g.x);
        }
        if (shapeType < 5.5) {
            return sdfRoundedTriangle(local, c.xy, c.zw, g.xy, g.z);
        }
        return sdEllipse(local - c.xy, c.zw * 0.5);
    }
)";

// All four variants walk the packed leaves [0, count); `count` is a uniform so
// the loop breaks early instead of always running BASIC_SHAPE_LEAF_MAX times.
constexpr char UBER_BASICSHAPE_SMOOTH_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        int count = int(u_count + 0.5);
        vec4 result = vec4(0.0);
        for (int i = 0; i < 30; ++i) {
            if (i >= count) { break; }
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            vec4 d = EvalBasicShapeLeafCore(local, b.z, u_matC[i], u_geo[i]);
            result = (i == 0) ? d : sdgSmoothUnion(result, d, u_spacing);
        }
        return half4(result);
    }
)";

// Plain union, normal version: keep the smaller-SDF leaf's channels, same as
// the cascaded GESDFUnionOpShaderShape plain-union pass-through.
constexpr char UBER_BASICSHAPE_UNION_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        int count = int(u_count + 0.5);
        vec4 result = vec4(0.0);
        for (int i = 0; i < 30; ++i) {
            if (i >= count) { break; }
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            vec4 d = EvalBasicShapeLeafCore(local, b.z, u_matC[i], u_geo[i]);
            result = (i == 0 || result.a >= d.a) ? d : result;
        }
        return half4(result);
    }
)";

// Smooth union, no-normal version: SDF channel only, same math as the
// cascaded no-normal smooth union on the .a channel.
constexpr char UBER_BASICSHAPE_SMOOTH_NO_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        int count = int(u_count + 0.5);
        float sdf = 0.0;
        for (int i = 0; i < 30; ++i) {
            if (i >= count) { break; }
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            float d = EvalBasicShapeLeafSdf(local, b.z, u_matC[i], u_geo[i]);
            sdf = (i == 0) ? d : sdfSmoothUnion(sdf, d, u_spacing);
        }
        return half4(0.0, 0.0, 0.0, sdf);
    }
)";

// Plain union, no-normal version: SDF channel only.
constexpr char UBER_BASICSHAPE_UNION_NO_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        int count = int(u_count + 0.5);
        float sdf = 0.0;
        for (int i = 0; i < 30; ++i) {
            if (i >= count) { break; }
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            float d = EvalBasicShapeLeafSdf(local, b.z, u_matC[i], u_geo[i]);
            sdf = (i == 0) ? d : min(sdf, d);
        }
        return half4(0.0, 0.0, 0.0, sdf);
    }
)";

} // namespace

bool HasNonG1RRectLeaf(const std::shared_ptr<GESDFShaderShape>& root)
{
    if (!root) {
        return false;
    }
    GESDFUberTreeDesc desc;
    std::vector<GESDFUberLeafPos> positions;
    // Skip when not uberable or too small: those trees never reach an uber shader.
    if (!GESdfUberShaderShape::WalkUberLeaves(root, Drawing::Matrix(), desc, positions)) {
        return false;
    }
    bool hasNonG1 = false;
    int effectiveCount = 0;
    for (const auto& pos : positions) {
        switch (pos.leaf->GetSDFShapeType()) {
            case GESDFShapeType::RRECT: {
                const auto* rrectShape = static_cast<const GESDFRRectShaderShape*>(pos.leaf.get());
                const GERRect& rrect = rrectShape->GetRRect();
                if (rrect.width_ < MIN_SIZE || rrect.height_ < MIN_SIZE) {
                    // Degenerate leaves are absorbed by the collector.
                    continue;
                }
                ++effectiveCount;
                if (GESdfUberShaderShape::ClassifyUberRRect(rrect) != GEUberBasicShapeType::G1_UNUNIFORM_RRECT) {
                    hasNonG1 = true;
                }
                break;
            }
            case GESDFShapeType::TRIANGLE:
                ++effectiveCount;
                break;
            case GESDFShapeType::ELLIPSE: {
                const auto* ellipseShape = static_cast<const GESDFEllipseShaderShape*>(pos.leaf.get());
                if (ellipseShape->GetWidth() < MIN_SIZE || ellipseShape->GetHeight() < MIN_SIZE) {
                    // Same degenerate skip as the collector.
                    continue;
                }
                ++effectiveCount;
                break;
            }
            default:
                // Not a basic-shape tree: cascades anyway, no GEX object needed.
                return false;
        }
    }
    return hasNonG1 && effectiveCount >= BASIC_SHAPE_LEAF_MIN && effectiveCount <= BASIC_SHAPE_LEAF_MAX;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESdfBasicShapeUberShape::GetBuilder(
    bool hasNormal, bool isSmoothUnion)
{
    // Variant = hasNormal * 2 + isSmoothUnion.
    thread_local std::array<std::shared_ptr<Drawing::RuntimeShaderBuilder>, UBER_SHADER_VARIANT_COUNT> builders {};
    int index = (hasNormal ? UBER_VARIANT_NORMAL_OFFSET : 0) + (isSmoothUnion ? UBER_VARIANT_SMOOTH_OFFSET : 0);
    if (builders[index]) {
        return builders[index];
    }
    static constexpr const char* VARIANTS[UBER_SHADER_VARIANT_COUNT] = {
        UBER_BASICSHAPE_UNION_NO_NORMAL_PROG,
        UBER_BASICSHAPE_SMOOTH_NO_NORMAL_PROG,
        UBER_BASICSHAPE_UNION_NORMAL_PROG,
        UBER_BASICSHAPE_SMOOTH_NORMAL_PROG,
    };
    std::string prog = std::string(UBER_BASICSHAPE_COMMON_PROG) + VARIANTS[index];
    auto effect = GECreateRuntimeEffectForShader(prog);
    if (!effect) {
        GE_LOGE("GetBuilder: create runtime effect fail, hasNormal=%{public}d, smooth=%{public}d",
            static_cast<int>(hasNormal), static_cast<int>(isSmoothUnion));
        return nullptr;
    }
    builders[index] = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builders[index];
}

bool GESdfBasicShapeUberShape::CollectLeaves(const std::shared_ptr<GESDFShaderShape>& root,
    std::vector<GESDFBasicShapeUberLeafInfo>& leaves, GESDFUberTreeDesc& desc) const
{
    std::vector<GESDFUberLeafPos> positions;
    if (!WalkUberLeaves(root, Drawing::Matrix(), desc, positions)) {
        return false;
    }
    for (const auto& pos : positions) {
        if (!IsAffineTransform(pos.parentToLocal)) {
            return false;
        }
        GESDFBasicShapeUberLeafInfo info;
        info.parentToLocal = pos.parentToLocal;
        auto type = pos.leaf->GetSDFShapeType();
        if (type == GESDFShapeType::RRECT) {
            const auto& rrect = static_cast<const GESDFRRectShaderShape*>(pos.leaf.get())->GetRRect();
            if (rrect.width_ < MIN_SIZE || rrect.height_ < MIN_SIZE) {
                continue;
            }
            info.shapeType = ClassifyUberRRect(rrect);
            float halfW = rrect.width_ * HALF;
            float halfH = rrect.height_ * HALF;
            float maxRadius = std::min(halfW, halfH) + EXTEND;
            info.params = { rrect.left_ + halfW, rrect.top_ + halfH, halfW + EXTEND, halfH + EXTEND,
                ClampCornerRadius(rrect.radius_[GERRect::TOP_LEFT].x_ + EXTEND, maxRadius),
                ClampCornerRadius(rrect.radius_[GERRect::TOP_RIGHT].x_ + EXTEND, maxRadius),
                ClampCornerRadius(rrect.radius_[GERRect::BOTTOM_RIGHT].x_ + EXTEND, maxRadius),
                ClampCornerRadius(rrect.radius_[GERRect::BOTTOM_LEFT].x_ + EXTEND, maxRadius) };
        } else if (type == GESDFShapeType::TRIANGLE) {
            const auto* triangle = static_cast<const GESDFTriangleShaderShape*>(pos.leaf.get());
            info.shapeType = GEUberBasicShapeType::TRIANGLE;
            info.params = { triangle->GetVertex0().x_, triangle->GetVertex0().y_, triangle->GetVertex1().x_,
                triangle->GetVertex1().y_, triangle->GetVertex2().x_, triangle->GetVertex2().y_,
                std::max(0.0f, triangle->GetRadius()) };
        } else if (type == GESDFShapeType::ELLIPSE) {
            const auto* ellipse = static_cast<const GESDFEllipseShaderShape*>(pos.leaf.get());
            if (ellipse->GetWidth() < MIN_SIZE || ellipse->GetHeight() < MIN_SIZE) {
                continue;
            }
            info.shapeType = GEUberBasicShapeType::ELLIPSE;
            info.params = { ellipse->GetCenter().x_, ellipse->GetCenter().y_, ellipse->GetWidth(),
                ellipse->GetHeight() };
        } else {
            GE_LOGD("CollectLeaves: unsupported leaf type, fall back");
            return false;
        }
        leaves.push_back(info);
    }
    return true;
}

void GESdfBasicShapeUberShape::PackUniforms(const std::shared_ptr<Drawing::RuntimeShaderBuilder>& builder,
    const std::vector<GESDFBasicShapeUberLeafInfo>& leaves, const GESDFUberTreeDesc& desc) const
{
    int count = static_cast<int>(leaves.size());
    std::array<float, VEC4_FLOATS * BASIC_SHAPE_LEAF_MAX> matA {};
    std::array<float, VEC4_FLOATS * BASIC_SHAPE_LEAF_MAX> matB {};
    std::array<float, VEC4_FLOATS * BASIC_SHAPE_LEAF_MAX> matC {};
    std::array<float, VEC4_FLOATS * BASIC_SHAPE_LEAF_MAX> geo {};
    for (int i = 0; i < count; ++i) {
        const auto& leaf = leaves[i];
        const Drawing::Matrix& m = leaf.parentToLocal;
        int base = i * VEC4_FLOATS;
        matA[base] = m.Get(Drawing::Matrix::Index::SCALE_X);
        matA[base + UBER_OFFSET_1] = m.Get(Drawing::Matrix::Index::SKEW_X);
        matA[base + UBER_OFFSET_2] = m.Get(Drawing::Matrix::Index::TRANS_X);
        matA[base + UBER_OFFSET_3] = m.Get(Drawing::Matrix::Index::SKEW_Y);
        matB[base] = m.Get(Drawing::Matrix::Index::SCALE_Y);
        matB[base + UBER_OFFSET_1] = m.Get(Drawing::Matrix::Index::TRANS_Y);
        matB[base + UBER_OFFSET_2] = static_cast<float>(static_cast<int>(leaf.shapeType));
        matC[base] = leaf.params[0];
        matC[base + UBER_OFFSET_1] = leaf.params[UBER_OFFSET_1];
        matC[base + UBER_OFFSET_2] = leaf.params[UBER_OFFSET_2];
        matC[base + UBER_OFFSET_3] = leaf.params[UBER_OFFSET_3];
        geo[base] = leaf.params[UBER_OFFSET_4];
        geo[base + UBER_OFFSET_1] = leaf.params[UBER_OFFSET_5];
        geo[base + UBER_OFFSET_2] = leaf.params[UBER_OFFSET_6];
        geo[base + UBER_OFFSET_3] = leaf.params[UBER_OFFSET_7];
    }
    builder->SetUniform("u_count", static_cast<float>(count));
    builder->SetUniform("u_spacing", desc.isSmoothUnion ? std::max(desc.spacing, SPACING_MIN) : 0.f);
    builder->SetUniform("u_matA", matA.data(), matA.size());
    builder->SetUniform("u_matB", matB.data(), matB.size());
    builder->SetUniform("u_matC", matC.data(), matC.size());
    builder->SetUniform("u_geo", geo.data(), geo.size());
}

std::shared_ptr<ShaderEffect> GESdfBasicShapeUberShape::GenerateUberShader(
    float width, float height, bool hasNormal) const
{
    std::vector<GESDFBasicShapeUberLeafInfo> leaves;
    leaves.reserve(BASIC_SHAPE_LEAF_MAX);
    GESDFUberTreeDesc desc;
    if (!CollectLeaves(GetRootShape(), leaves, desc)) {
        return nullptr;
    }
    int count = static_cast<int>(leaves.size());
    if (!IsLeafCountInRange(count, BASIC_SHAPE_LEAF_MAX, BASIC_SHAPE_LEAF_MIN)) {
        return nullptr;
    }
    auto builder = GetBuilder(hasNormal, desc.isSmoothUnion);
    if (!builder) {
        return nullptr;
    }
    GE_TRACE_NAME_FMT("GESdfBasicShapeUberShape pack uniforms, count: %d, smooth: %d, hasNormal: %d",
        count, static_cast<int>(desc.isSmoothUnion), static_cast<int>(hasNormal));
    PackUniforms(builder, leaves, desc);
    auto uberShader = builder->MakeShader(nullptr, false);
    if (!uberShader) {
        GE_LOGE("GenerateUberShader: make shader fail, count=%{public}d", count);
    }
    return uberShader;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
