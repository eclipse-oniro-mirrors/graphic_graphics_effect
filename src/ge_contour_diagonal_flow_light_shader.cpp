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

#include "ge_contour_diagonal_flow_light_shader.h"

#include <algorithm>
#include <sstream>
#include "ge_log.h"
#include "ge_visual_effect_impl.h"
#include "common/rs_vector2.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif
namespace OHOS {
namespace Rosen {
using CacheDataType = struct CacheData {
    std::shared_ptr<Drawing::Image> first = nullptr;
    std::shared_ptr<Drawing::Image> second = nullptr;
    uint32_t hash = 0;
};

using RuntimeShaderBuilderPtr = std::shared_ptr<Drawing::RuntimeShaderBuilder>;
using namespace Drawing;
namespace {
constexpr size_t NUM0 = 0;
constexpr size_t NUM1 = 1;
constexpr size_t NUM2 = 2;
constexpr size_t MIN_NUM = 6;
constexpr static uint8_t POSITION_CHANNEL = 2; // 2 floats per point
constexpr static uint8_t ARRAY_SIZE = 64;  // 32 segments need 64 control points

static constexpr char PRECAL_PROG[] = R"(
    uniform vec2 iResolution;
    uniform float count;
    const int capacity = 64; // 64 control points for 32 Bezier curves
    uniform vec2 controlPoints[capacity];

    // ===== Constant =====
    const float INF = 1e10; // infinity, i.e., 1.0 / 0.0
    const float SQRT3 = 1.7320508;

    // ===== Vector Math Utilities =====
    float Cross2(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }
    float SaturateFloat(float a) { return clamp(a, 0.0, 1.0); }
    vec3 SaturateVec3(vec3 a) { return clamp(a, 0.0, 1.0); }
    float AbsMin(float a, float b) { return abs(a) < abs(b) ? a : b; }

    vec2 GetElement(vec2 arr[capacity], int index) {
        for (int i = 0; i < capacity; ++i) {
            if (i == index) return arr[i];
        }
        return vec2(0.0);
    }

    vec2 Bezier(vec2 pointA, vec2 pointB, vec2 pointC, float t)
    {
        return pointA + t * (-2.0 * pointA + 2.0 * pointB) + t * t * (pointA - 2.0 * pointB + pointC);
    }

    float SdfLine(vec2 p, vec2 a, vec2 b)
    {
        float h = SaturateFloat(dot(p - a, b - a) / dot(b - a, b - a));
        return length(p - a - h * (b - a));
    }

    float SdfLinePartition(vec2 p, vec2 a, vec2 b, inout vec2 closestPoint, inout float bestTLocal)
    {
        vec2 ba = b - a;
        vec2 pa = p - a;
        float h = SaturateFloat(dot(pa, ba) / dot(ba, ba));
        closestPoint = a + h * ba;
        bestTLocal = h;
        vec2 k = pa - h * ba;
        vec2 n = vec2(ba.y, -ba.x);
        return (dot(k, n) >= 0.0) ? length(k) : -length(k);
    }

    float SdfBezier(vec2 pos, vec2 pointA, vec2 pointB, vec2 pointC,
        inout vec2 closestPoint, inout float bestTLocal)
    {
        const float EPSILON = 1e-3;
        const float ONE_THIRD = 1.0 / 3.0;
        bool abEqual = all(lessThan(abs(pointA - pointB), vec2(EPSILON)));
        bool bcEqual = all(lessThan(abs(pointB - pointC), vec2(EPSILON)));
        bool acEqual = all(lessThan(abs(pointA - pointC), vec2(EPSILON)));
        if (abEqual && bcEqual) {
            closestPoint = pointA;
            return distance(pos, pointA);
        }
        if (abEqual || acEqual) return SdfLinePartition(pos, pointB, pointC, closestPoint, bestTLocal);
        if (bcEqual) return SdfLinePartition(pos, pointA, pointC, closestPoint, bestTLocal);
        if (abs(dot(normalize(pointB - pointA), normalize(pointC - pointB)) - 1.0) < EPSILON)
            return SdfLinePartition(pos, pointA, pointC, closestPoint, bestTLocal);
        vec2 a = pointB - pointA;
        vec2 b = pointA - 2.0 * pointB + pointC;
        vec2 c = a * 2.0;
        vec2 d = pointA - pos;
        float kk = 1.0 / dot(b, b);
        float kx = kk * dot(a, b);
        float ky = kk * (2.0 * dot(a, a) + dot(d, b)) * ONE_THIRD;
        float kz = kk * dot(d, a);
        float res = 0.0;
        float sgn = 0.0;
        float p = ky - kx * kx;
        float q = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
        float h = q * q + 4.0 * p * p * p;
        if (h >= 0.0) {
            h = sqrt(h);
            vec2 x = 0.5 * (vec2(h, -h) - q);
            vec2 uv = vec2((x.x > 0.0 ? 1.0 : -1.0) * pow(abs(x.x), ONE_THIRD),
                        (x.y > 0.0 ? 1.0 : -1.0) * pow(abs(x.y), ONE_THIRD));
            float t = SaturateFloat(uv.x + uv.y - kx) + EPSILON;
            closestPoint = Bezier(pointA, pointB, pointC, t);
            bestTLocal = t;
            vec2 qv = d + (c + b * t) * t;
            res = dot(qv, qv);
            sgn = Cross2(c + 2.0 * b * t, qv);
            return (sgn > 0.0 ? 1.0 : -1.0) * sqrt(res);
        }
        float z = sqrt(-p);
        float v = acos(q / (p * z * 2.0)) * ONE_THIRD;
        float m = cos(v);
        float n = sin(v) * SQRT3;
        vec3 t = SaturateVec3(vec3(m + m, -n - m, n - m) * z - kx) + EPSILON;
        vec2 qx = d + (c + b * t.x) * t.x;
        float dx = dot(qx, qx);
        float sx = Cross2(c + 2.0 * b * t.x, qx);
        vec2 qy = d + (c + b * t.y) * t.y;
        float dy = dot(qy, qy);
        float sy = Cross2(c + 2.0 * b * t.y, qy);
        res = (dx < dy) ? dx : dy;
        sgn = (dx < dy) ? sx : sy;
        float bestT = (dx < dy) ? t.x : t.y;
        closestPoint = Bezier(pointA, pointB, pointC, bestT);
        bestTLocal = bestT;
        return (sgn > 0.0 ? 1.0 : -1.0) * sqrt(res);
    }

    // ================= SDF Polygon =================
    float SdfControlSegment(vec2 p, vec2 pointA, vec2 pointB, vec2 pointC)
    {
        return AbsMin(SdfLine(p, pointA, pointB), SdfLine(p, pointB, pointC));
    }

    float SdfControlPolygon(vec2 p, vec2 controlPoly[capacity], int size,
        inout vec2 closest[3], inout float closestSegmentIndex)
    {
        float minDist = INF;
        int segmentCount = size / 2;
        // i = 0
        int iMidPrev = size - 1; // (iPrev + 1) % size, iPrev = (i - 2 + size) % size
        vec2 pointA = GetElement(controlPoly, iMidPrev);
        vec2 pointB = controlPoly[0]; // i
        vec2 pointC = controlPoly[1]; // iMid = (i + 1) % size
        float d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 0.0;
        }
        // i = 2
        pointA = controlPoly[1]; // iMidPrev = i - 1
        pointB = controlPoly[2]; // i
        pointC = controlPoly[3]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 1.0;
        }
        // i = 4
        pointA = controlPoly[3]; // iMidPrev = i - 1
        pointB = controlPoly[4]; // i
        pointC = controlPoly[5]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 2.0;
        }
        if (size <= 6) return minDist; // 6: next i beyond legal size
        pointA = controlPoly[5]; // iMidPrev = i - 1
        pointB = controlPoly[6]; // i
        pointC = controlPoly[7]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 3.0;
        }
        if (size <= 8) return minDist; // 8: next i beyond legal size
        pointA = controlPoly[7]; // iMidPrev = i - 1
        pointB = controlPoly[8]; // i
        pointC = controlPoly[9]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 4.0;
        }
        if (size <= 10) return minDist; // 10: next i beyond legal size
        pointA = controlPoly[9]; // iMidPrev = i - 1
        pointB = controlPoly[10]; // i
        pointC = controlPoly[11]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 5.0;
        }
        if (size <= 12) return minDist; // 12: next i beyond legal size
        pointA = controlPoly[11]; // iMidPrev = i - 1
        pointB = controlPoly[12]; // i
        pointC = controlPoly[13]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 6.0;
        }
        if (size <= 14) return minDist; // 14: next i beyond legal size
        pointA = controlPoly[13]; // iMidPrev = i - 1
        pointB = controlPoly[14]; // i
        pointC = controlPoly[15]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 7.0;
        }
        if (size <= 16) return minDist; // 16: next i beyond legal size
        pointA = controlPoly[15]; // iMidPrev = i - 1
        pointB = controlPoly[16]; // i
        pointC = controlPoly[17]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 8.0;
        }
        if (size <= 18) return minDist; // 18: next i beyond legal size
        pointA = controlPoly[17]; // iMidPrev = i - 1
        pointB = controlPoly[18]; // i
        pointC = controlPoly[19]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 9.0;
        }
        if (size <= 20) return minDist; // 20: next i beyond legal size
        pointA = controlPoly[19]; // iMidPrev = i - 1
        pointB = controlPoly[20]; // i
        pointC = controlPoly[21]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 10.0;
        }
        if (size <= 22) return minDist; // 22: next i beyond legal size
        pointA = controlPoly[21]; // iMidPrev = i - 1
        pointB = controlPoly[22]; // i
        pointC = controlPoly[23]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 11.0;
        }
        if (size <= 24) return minDist; // 24: next i beyond legal size
        pointA = controlPoly[23]; // iMidPrev = i - 1
        pointB = controlPoly[24]; // i
        pointC = controlPoly[25]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 12.0;
        }
        if (size <= 26) return minDist; // 26: next i beyond legal size
        pointA = controlPoly[25]; // iMidPrev = i - 1
        pointB = controlPoly[26]; // i
        pointC = controlPoly[27]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 13.0;
        }
        if (size <= 28) return minDist; // 28: next i beyond legal size
        pointA = controlPoly[27]; // iMidPrev = i - 1
        pointB = controlPoly[28]; // i
        pointC = controlPoly[29]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 14.0;
        }
        if (size <= 30) return minDist; // 30: next i beyond legal size
        pointA = controlPoly[29]; // iMidPrev = i - 1
        pointB = controlPoly[30]; // i
        pointC = controlPoly[31]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 15.0;
        }
        if (size <= 32) return minDist; // 32: next i beyond legal size
        pointA = controlPoly[31]; // iMidPrev = i - 1
        pointB = controlPoly[32]; // i
        pointC = controlPoly[33]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 16.0;
        }
        if (size <= 34) return minDist; // 34: next i beyond legal size
        pointA = controlPoly[33]; // iMidPrev = i - 1
        pointB = controlPoly[34]; // i
        pointC = controlPoly[35]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 17.0;
        }
        if (size <= 36) return minDist; // 36: next i beyond legal size
        pointA = controlPoly[35]; // iMidPrev = i - 1
        pointB = controlPoly[36]; // i
        pointC = controlPoly[37]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 18.0;
        }
        if (size <= 38) return minDist; // 38: next i beyond legal size
        pointA = controlPoly[37]; // iMidPrev = i - 1
        pointB = controlPoly[38]; // i
        pointC = controlPoly[39]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 19.0;
        }
        if (size <= 40) return minDist; // 40: next i beyond legal size
        pointA = controlPoly[39]; // iMidPrev = i - 1
        pointB = controlPoly[40]; // i
        pointC = controlPoly[41]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 20.0;
        }
        if (size <= 42) return minDist; // 42: next i beyond legal size
        pointA = controlPoly[41]; // iMidPrev = i - 1
        pointB = controlPoly[42]; // i
        pointC = controlPoly[43]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 21.0;
        }
        if (size <= 44) return minDist; // 44: next i beyond legal size
        pointA = controlPoly[43]; // iMidPrev = i - 1
        pointB = controlPoly[44]; // i
        pointC = controlPoly[45]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 22.0;
        }
        if (size <= 46) return minDist; // 46: next i beyond legal size
        pointA = controlPoly[45]; // iMidPrev = i - 1
        pointB = controlPoly[46]; // i
        pointC = controlPoly[47]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 23.0;
        }
        if (size <= 48) return minDist; // 48: next i beyond legal size
        pointA = controlPoly[47]; // iMidPrev = i - 1
        pointB = controlPoly[48]; // i
        pointC = controlPoly[49]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 24.0;
        }
        if (size <= 50) return minDist; // 50: next i beyond legal size
        pointA = controlPoly[49]; // iMidPrev = i - 1
        pointB = controlPoly[50]; // i
        pointC = controlPoly[51]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 25.0;
        }
        if (size <= 52) return minDist; // 52: next i beyond legal size
        pointA = controlPoly[51]; // iMidPrev = i - 1
        pointB = controlPoly[52]; // i
        pointC = controlPoly[53]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 26.0;
        }
        if (size <= 54) return minDist; // 54: next i beyond legal size
        pointA = controlPoly[53]; // iMidPrev = i - 1
        pointB = controlPoly[54]; // i
        pointC = controlPoly[55]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 27.0;
        }
        if (size <= 56) return minDist; // 56: next i beyond legal size
        pointA = controlPoly[55]; // iMidPrev = i - 1
        pointB = controlPoly[56]; // i
        pointC = controlPoly[57]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 28.0;
        }
        if (size <= 58) return minDist; // 58: next i beyond legal size
        pointA = controlPoly[57]; // iMidPrev = i - 1
        pointB = controlPoly[58]; // i
        pointC = controlPoly[59]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 29.0;
        }
        if (size <= 60) return minDist; // 60: next i beyond legal size
        pointA = controlPoly[59]; // iMidPrev = i - 1
        pointB = controlPoly[60]; // i
        pointC = controlPoly[61]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 30.0;
        }
        if (size <= 62) return minDist; // 62: next i beyond legal size
        pointA = controlPoly[61]; // iMidPrev = i - 1
        pointB = controlPoly[62]; // i
        pointC = controlPoly[63]; // iMid = i + 1
        d = SdfControlSegment(p, pointA, pointB, pointC);
        if (abs(d) < abs(minDist)) {
            minDist = d;
            closest[0] = pointA;
            closest[1] = pointB;
            closest[2] = pointC;
            closestSegmentIndex = 31.0;
        }
        return minDist;
    }

    float SdfBezierShape(vec2 p, vec2 controlPoly[capacity], int controlPolySize,
        inout vec2 closestPoint, inout float bestTLocal, inout float closestSegmentIndex)
    {
        vec2 closest[3];
        SdfControlPolygon(p, controlPoly, controlPolySize, closest, closestSegmentIndex);
        return SdfBezier(p, closest[0], closest[1], closest[2], closestPoint, bestTLocal);
    }

    vec4 main(vec2 fragCoord)
    {
        int size = int(count);
        float segmentCount = 0.5 * count; // 0.5: half amount of the control point count
        vec2 p = (2.0 * fragCoord - iResolution.xy) / iResolution.y; // 2.0: normalized screen coordinates
        vec2 closestPoint = vec2(0.0);
        float tLocal = 0.0;
        float closestSegmentIndex = 0.0;
        float sdf = SdfBezierShape(p, controlPoints, size, closestPoint, tLocal, closestSegmentIndex);
        float tGlobal = (closestSegmentIndex + tLocal) / segmentCount;
        return vec4(abs(sdf), tGlobal, 0.0, 1.0);
    }
)";

static constexpr char HALO_ATLAS_PROG[] = R"(
    uniform shader precalculationImage;
    uniform vec2 iResolution;
    uniform float line1Start;
    uniform float line1Length;
    uniform float line2Start;
    uniform float line2Length;
    uniform float count;
    const int capacity = 64; // 64 control points for 32 Bezier curves
    uniform vec2 controlPoints[capacity];

    const float tileCount = 8.0;
    const float haloBellSigma = 0.18;
    const float haloArcLenRatio = 1.7; // halo length with respect to glow length
    const float haloThicknessFactor = 24.0; // the greater, the thinner

    float SampleTGlobal(vec2 coord)
    {
        return precalculationImage.eval(coord).g;
    }

    vec2 NdcToFragCoord(vec2 ndc, vec2 res)
    {
        vec2 uv = 0.5 * (ndc * res.y + res) / res;
        return uv * res;
    }

    vec2 GetElement(vec2 arr[capacity], int index)
    {
        for (int i = 0; i < capacity; ++i) {
            if (i == index) return arr[i];
        }
        return vec2(0.0);
    }

    float BellShape(float t, float sigma)
    {
        float x = (t - 0.5) / sigma;
        return exp(-x * x);
    }

    void ComputeHaloWindow(float tGlobal, float tGlowStart, float arcLen, float windowLen, float sigma,
        inout float weight)
    {
        float tOffset = mod(tGlobal - tGlowStart + 0.5 * (windowLen - arcLen) + 1.0, 1.0);
        float inWindow = step(0.0, tOffset) * step(tOffset, windowLen);
        weight = inWindow * BellShape(tOffset / windowLen, sigma);
    }

    float InverseDistanceSquared(vec2 p1, vec2 p2, vec2 ndc)
    {
        const float integralEpsilon = 0.03; // the lower, the brighter
        vec2 dir = p2 - p1;
        float lenSquared = dot(dir, dir);
        float t = dot(ndc - p1, dir) / lenSquared;
        t = clamp(t, 0.0, 1.0);
        vec2 projection = p1 + t * dir;
        vec2 d = ndc - projection;
        float distSq = dot(d, d) * haloThicknessFactor;
        return 1.0 / (distSq + integralEpsilon);
    }

    void GetAccumulateIntensity(vec2 p, vec2 controlPoints[capacity], int size, float tGlobal,
        float lineStart, float lineLength, inout float accumulateIntensity)
    {
        float weightedIntensity = 0.0;
        int segmentCount = size / 2;
        float haloLength = mod(lineLength * haloArcLenRatio, 1.0);
        float haloWeightA = 0.0, haloWeightC = 0.0;
        // i = 0
        int iMidPrev = size - 1; // (iPrev + 1) % size, iPrev = (i - 2 + size) % size
        vec2 pointA = GetElement(controlPoints, iMidPrev);
        vec2 pointC = controlPoints[1]; // (i + 1) % size
        vec2 coordA = NdcToFragCoord(pointA, iResolution.xy);
        vec2 coordC = NdcToFragCoord(pointC, iResolution.xy);
        float tgA = SampleTGlobal(coordA);
        float tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        // i = 2
        pointA = controlPoints[1]; // iMidPrev = i - 1
        pointC = controlPoints[3]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        // i = 4
        pointA = controlPoints[3]; // iMidPrev = i - 1
        pointC = controlPoints[5]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 6) { // 6: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[5]; // iMidPrev = i - 1
        pointC = controlPoints[7]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 8) { // 8: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[7]; // iMidPrev = i - 1
        pointC = controlPoints[9]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 10) { // 10: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[9]; // iMidPrev = i - 1
        pointC = controlPoints[11]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 12) { // 12: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[11]; // iMidPrev = i - 1
        pointC = controlPoints[13]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 14) { // 14: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[13]; // iMidPrev = i - 1
        pointC = controlPoints[15]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 16) { // 16: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[15]; // iMidPrev = i - 1
        pointC = controlPoints[17]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 18) { // 18: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[17]; // iMidPrev = i - 1
        pointC = controlPoints[19]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 20) { // 20: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[19]; // iMidPrev = i - 1
        pointC = controlPoints[21]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 22) { // 22: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[21]; // iMidPrev = i - 1
        pointC = controlPoints[23]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 24) { // 24: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[23]; // iMidPrev = i - 1
        pointC = controlPoints[25]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 26) { // 26: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[25]; // iMidPrev = i - 1
        pointC = controlPoints[27]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 28) { // 28: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[27]; // iMidPrev = i - 1
        pointC = controlPoints[29]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 30) { // 30: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[29]; // iMidPrev = i - 1
        pointC = controlPoints[31]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 32) { // 32: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[31]; // iMidPrev = i - 1
        pointC = controlPoints[33]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 34) { // 34: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[33]; // iMidPrev = i - 1
        pointC = controlPoints[35]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 36) { // 36: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[35]; // iMidPrev = i - 1
        pointC = controlPoints[37]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 38) { // 38: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[37]; // iMidPrev = i - 1
        pointC = controlPoints[39]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 40) { // 40: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[39]; // iMidPrev = i - 1
        pointC = controlPoints[41]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 42) { // 42: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[41]; // iMidPrev = i - 1
        pointC = controlPoints[43]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 44) { // 44: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[43]; // iMidPrev = i - 1
        pointC = controlPoints[45]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 46) { // 46: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[45]; // iMidPrev = i - 1
        pointC = controlPoints[47]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 48) { // 48: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[47]; // iMidPrev = i - 1
        pointC = controlPoints[49]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 50) { // 50: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[49]; // iMidPrev = i - 1
        pointC = controlPoints[51]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 52) { // 52: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[51]; // iMidPrev = i - 1
        pointC = controlPoints[53]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 54) { // 54: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[53]; // iMidPrev = i - 1
        pointC = controlPoints[55]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 56) { // 56: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[55]; // iMidPrev = i - 1
        pointC = controlPoints[57]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 58) { // 58: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[57]; // iMidPrev = i - 1
        pointC = controlPoints[59]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 60) { // 60: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[59]; // iMidPrev = i - 1
        pointC = controlPoints[61]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        if (size <= 62) { // 62: next i beyond legal size
            accumulateIntensity = weightedIntensity / float(segmentCount);
            return;
        }
        pointA = controlPoints[61]; // iMidPrev = i - 1
        pointC = controlPoints[63]; // (i + 1) % size
        coordA = NdcToFragCoord(pointA, iResolution.xy);
        coordC = NdcToFragCoord(pointC, iResolution.xy);
        tgA = SampleTGlobal(coordA);
        tbC = SampleTGlobal(coordC);
        ComputeHaloWindow(tgA, lineStart, lineLength, haloLength, haloBellSigma, haloWeightA);
        ComputeHaloWindow(tbC, lineStart, lineLength, haloLength, haloBellSigma, haloWeightC);
        weightedIntensity += InverseDistanceSquared(pointA, pointC, p) * (haloWeightA + haloWeightC);
        accumulateIntensity = weightedIntensity / float(segmentCount);
    }

    vec4 main(vec2 fragCoord)
    {
        vec2 tileSize = iResolution.xy / tileCount;
        vec2 tileCoord = floor(fragCoord / tileSize);
        float zIndex = tileCoord.y * tileCount + tileCoord.x;
        float t = (zIndex + 0.5) / (tileCount * tileCount); // [0, 1)

        vec2 localXY = mod(fragCoord, tileSize);
        vec2 uv = (localXY + 0.5) / tileSize;

        int size = int(count);
        float aspect = iResolution.x / iResolution.y;
        vec2 p = (2.0 * uv - 1.0) * vec2(aspect, 1.0);

        float tGlobal = SampleTGlobal(fragCoord);

        float haloIntensity1 = 0.0;
        float haloIntensity2 = 0.0;
        GetAccumulateIntensity(p, controlPoints, size, tGlobal,
            mod(line1Start + t, 1.0), line1Length, haloIntensity1);
        GetAccumulateIntensity(p, controlPoints, size, tGlobal,
            mod(line2Start + t, 1.0), line2Length, haloIntensity2);

        haloIntensity1 = clamp(haloIntensity1 * 0.1, 0.0, 1.0); // 0.1: scale to [0, 1]
        haloIntensity2 = clamp(haloIntensity2 * 0.1, 0.0, 1.0); // 0.1: scale to [0, 1]
        return vec4(haloIntensity1, haloIntensity2, mod(line1Start, 1.0), 1.0);
    }
)";
} // namespacesu

std::vector<Vector2f> ConvertUVToNDC(const std::vector<Vector2f>& uvPoints, int width, int height)
{
    if (height < 1) {
        return {};
    }
    std::vector<Vector2f> ndcPoints;
    ndcPoints.reserve(uvPoints.size());
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    for (const auto& uv : uvPoints) {
        float ndcX = (uv[0] * 2.0f - 1.0f) * aspect;
        float ndcY = uv[1] * 2.0f - 1.0f;
        ndcPoints.emplace_back(ndcX, ndcY);
    }
    return ndcPoints;
}

void ConvertPointsTo(const std::vector<Vector2f>& in, std::vector<float>& out)
{
    out.clear();
    for (auto& p : in) {
        out.emplace_back(p[0]);
        out.emplace_back(p[1]);
    }
}

uint32_t CalHash(const std::vector<Vector2f>& in)
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::Hash;
#endif
    uint32_t hashOut = 0;
    for (auto& p : in) {
        hashOut = hashFunc(&p, sizeof(p), hashOut);
    }
    return hashOut;
}

GEContourDiagonalFlowLightShader::GEContourDiagonalFlowLightShader() {}

GEContourDiagonalFlowLightShader::GEContourDiagonalFlowLightShader(GEContentDiagonalFlowLightShaderParams& param)
{
    contourDiagonalFlowLightParams_ = param;
}

std::shared_ptr<GEContourDiagonalFlowLightShader>GEContourDiagonalFlowLightShader::CreateContourDiagonalFlowLightShader(
    GEContentDiagonalFlowLightShaderParams& param)
{
    std::shared_ptr<GEContourDiagonalFlowLightShader> contourDiagonalFlowLightShader =
        std::make_shared<GEContourDiagonalFlowLightShader>(param);
    return contourDiagonalFlowLightShader;
}

void GEContourDiagonalFlowLightShader::MakeDrawingShader(const Drawing::Rect& rect,
    float progress)
{
}

RuntimeShaderBuilderPtr GEContourDiagonalFlowLightShader::GetContourDiagonalFlowLightPrecalculationBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> contourDiagonalFlowLightShaderEffectPrecalculation_ = nullptr;
    if (contourDiagonalFlowLightShaderEffectPrecalculation_ == nullptr) {
        contourDiagonalFlowLightShaderEffectPrecalculation_ = Drawing::RuntimeEffect::CreateForShader(PRECAL_PROG);
    }

    if (contourDiagonalFlowLightShaderEffectPrecalculation_ == nullptr) {
        GE_LOGE("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShaderEffectPrecalculation_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(contourDiagonalFlowLightShaderEffectPrecalculation_);
}

RuntimeShaderBuilderPtr GEContourDiagonalFlowLightShader::GetContourDiagonalFlowLightHaloAtlasBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> contourDiagonalFlowLightShaderEffectHaloAtlas_ = nullptr;

    if (contourDiagonalFlowLightShaderEffectHaloAtlas_ == nullptr) {
        contourDiagonalFlowLightShaderEffectHaloAtlas_ = Drawing::RuntimeEffect::CreateForShader(HALO_ATLAS_PROG);
    }
    if (contourDiagonalFlowLightShaderEffectHaloAtlas_ == nullptr) {
        GE_LOGE("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShaderEffectHaloAtlas_ is nullptr");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(contourDiagonalFlowLightShaderEffectHaloAtlas_);
}

void GEContourDiagonalFlowLightShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    GE_LOGD(" GEContourDiagonalFlowLightShader Preprocess start");
    if (contourDiagonalFlowLightParams_.contour_.size() < MIN_NUM) {
        GE_LOGW("GEContourDiagonalFlowLightShader less point %{public}zu",
            contourDiagonalFlowLightParams_.contour_.size());
        cacheAnyPtr_ = nullptr;
        return;
    }
    pointCnt_ = contourDiagonalFlowLightParams_.contour_.size();
    auto inHash = CalHash(contourDiagonalFlowLightParams_.contour_);
    if (cacheAnyPtr_ == nullptr || inHash != std::any_cast<CacheDataType>(*cacheAnyPtr_).hash) {
        GE_LOGD(" GEContourDiagonalFlowLightShader Preprocess start");
        auto ndcPoints = ConvertUVToNDC(contourDiagonalFlowLightParams_.contour_, rect.GetWidth(), rect.GetHeight());
        ConvertPointsTo(ndcPoints, controlPoints_);
        controlPoints_.resize(ARRAY_SIZE * POSITION_CHANNEL);
        CacheDataType cacheData;
        cacheData.hash = inHash;
        Drawing::ImageInfo cacheImgInf(rect.GetWidth(), rect.GetHeight(),
            Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
        auto cacheImg = MakeContourDiagonalFlowLightPrecalculationShader(canvas, cacheImgInf);
        if (cacheImg) {
            cacheData.first = cacheImg;
            cacheData.second = nullptr;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
        if (std::any_cast<CacheDataType>(*cacheAnyPtr_).second == nullptr) {
            Drawing::ImageInfo cacheAtlaasImgInf(rect.GetWidth(), rect.GetHeight(),
                Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
            auto cacheAtlasImg = MakeContourDiagonalFlowLightHaloAtlasShader(canvas, cacheAtlaasImgInf);
            if (cacheAtlasImg) {
                cacheData.second = cacheAtlasImg;
                cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
            } else {
                cacheAnyPtr_ = nullptr; // next frame to caculate the cache data
                GE_LOGD(" GEContourDiagonalFlowLightShader Preprocess second NG");
            }
        } else {
            GE_LOGD(" GEContourDiagonalFlowLightShader Preprocess NG");
        }
    }
}

std::shared_ptr<Drawing::Image> GEContourDiagonalFlowLightShader::MakeContourDiagonalFlowLightHaloAtlasShader(
    Drawing::Canvas& canvas, const Drawing::ImageInfo& imageInfo)
{
    // requires at least 6 controlpoints, corresponding to 3 quadratic bezier curves
    if (pointCnt_ < MIN_NUM) {
        GE_LOGD("GEContourDiagonalFlowLightShader MakeContourDiagonalFlowLightHaloAtlasShader path less points.");
        return nullptr;
    }
    if (cacheAnyPtr_ == nullptr) {
        GE_LOGD("GEContourDiagonalFlowLightShader MakeContourDiagonalFlowLightHaloAtlasShader cache is nullptr.");
        return nullptr;
    }
    if (std::any_cast<CacheDataType>(*cacheAnyPtr_).first == nullptr) {
        GE_LOGD("GEContourDiagonalFlowLightShader MakeContourDiagonalFlowLightHaloAtlasShader first cache is nullptr.");
        return nullptr;
    }
    auto precalculationImage = std::any_cast<CacheDataType>(*cacheAnyPtr_).first;
    auto width = imageInfo.GetWidth();
    auto height = imageInfo.GetHeight();
    auto builder = GetContourDiagonalFlowLightHaloAtlasBuilder();
    Drawing::Matrix matrix;
    auto preCalculationShader = Drawing::ShaderEffect::CreateImageShader(*precalculationImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    const float defaultLineLength = 0.3f;
    float line1Length = (contourDiagonalFlowLightParams_.line1Length_ < 0.0f ||
        std::abs(contourDiagonalFlowLightParams_.line1Length_) < std::numeric_limits<float>::epsilon()) ?
        defaultLineLength : contourDiagonalFlowLightParams_.line1Length_;
    float line2Length = (contourDiagonalFlowLightParams_.line2Length_ < 0.0f ||
        std::abs(contourDiagonalFlowLightParams_.line2Length_) < std::numeric_limits<float>::epsilon()) ?
        defaultLineLength : contourDiagonalFlowLightParams_.line2Length_;
    builder->SetChild("precalculationImage", preCalculationShader);
    builder->SetUniform("iResolution", static_cast<float>(width), static_cast<float>(height));
    builder_->SetUniform("line1Start", contourDiagonalFlowLightParams_.line1Start_);
    builder_->SetUniform("line1Length", std::clamp(line1Length, 0.0f, 1.0f));
    builder_->SetUniform("line2Start", contourDiagonalFlowLightParams_.line2Start_);
    builder_->SetUniform("line2Length", std::clamp(line2Length, 0.0f, 1.0f));
    builder_->SetUniform("count", static_cast<float>(pointCnt_));
    builder_->SetUniform("controlPoints", controlPoints_.data(), controlPoints_.size());
    auto contourDiagonalFlowLightShader = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (contourDiagonalFlowLightShader == nullptr) {
        GE_LOGE("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShader is nullptr");
    }
    return contourDiagonalFlowLightShader;
}

std::shared_ptr<Drawing::Image> GEContourDiagonalFlowLightShader::MakeContourDiagonalFlowLightPrecalculationShader(
    Drawing::Canvas& canvas, const Drawing::ImageInfo& imageInfo)
{
     // requires at least 6 control points, corresponding to 3 quadratic bezier curves
    if (pointCnt_ < MIN_NUM) {
        GE_LOGD("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShader path less points.");
        return nullptr;
    }
    float width = imageInfo.GetWidth();
    float height = imageInfo.GetHeight();
    auto builder = GetContourDiagonalFlowLightPrecalculationBuilder();
    builder->SetUniform("iResolution", width, height);
    builder->SetUniform("count", static_cast<float>(pointCnt_));
    builder->SetUniform("controlPoints", controlPoints_.data(), controlPoints_.size());
    auto contourDiagonalFlowLightShader = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (contourDiagonalFlowLightShader == nullptr) {
        GE_LOGE("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShader is nullptr.");
        return nullptr;
    }
    return contourDiagonalFlowLightShader;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEContourDiagonalFlowLightShader::GetContourDiagonalFlowLightBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> contourDiagonalFlowLightShaderEffect_ = nullptr;

    if (contourDiagonalFlowLightShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader precalculationImage;
            uniform shader haloAtlasImage;
            uniform vec2 iResolution;
            uniform float line1Start;
            uniform float line1Length;
            uniform vec3 line1Color;
            uniform float line2Start;
            uniform float line2Length;
            uniform vec3 line2Color;
            uniform float lineThickness;

            // ===== Constant =====
            const float tileCount = 8.0;
            const float thickness = 0.02;
            const float glowBellSigma = 0.28;
            const float haloBellSigma = 0.18;
            const float haloArcLenRatio = 1.7;

            const float distMin = 1e-4;
            const float intensity = 1.8;
            const float glowRadius = 0.05;
            const float haloFactor = 0.001;
            const float integralHaloBrightness = 0.4;

            // Gaussian bell-shaped profile centered at t = 0.5
            float BellShape(float t, float sigma)
            {
                float x = (t - 0.5) / sigma;
                return exp(-x * x);
            }

            // Compute glowing intensity based on SDF and local t on segment
            void GetSegmentGlowSDF(float sdfData, vec3 color, float tLocal, inout vec3 glowCol)
            {
                float w = BellShape(tLocal, glowBellSigma); // tLocal: [0,1]
                float th = thickness * w * lineThickness;

                glowCol = color * pow(glowRadius / max(sdfData - th, distMin), intensity);
            }

            // Compute a global halo effect for the entire path
            void GetGlobalGlowSDF(float sdfData, vec3 color, inout vec3 glowCol)
            {
                glowCol = color * pow(glowRadius * haloFactor / max(sdfData, distMin), intensity * haloFactor);
            }

            void ComputeArcWindow(float tGlobal, float tStart, float windowLen, float sigma,
                inout float tLocal, inout float weight)
            {
                float shiftedT = mod(tGlobal - tStart + 1.0, 1.0);
                tLocal = shiftedT / windowLen;
                float inWindow = step(0.0, shiftedT) * step(shiftedT, windowLen);
                weight = inWindow * BellShape(tLocal, sigma);
            }

            void GetSegmentHalo(float weightedIntensity, vec3 color, inout vec3 glowCol)
            {
                float brightness = log(1.0 + weightedIntensity) * integralHaloBrightness;
                glowCol = brightness * color;
            }

            vec2 SamplePrecalculationImage(vec2 coord)
            {
                return precalculationImage.eval(coord).rg;
            }

            vec2 SampleHaloIntensity(vec2 coord, float atlasOffset)
            {
                float tileTotal = tileCount * tileCount;
                float z = mod(mod(line1Start, 1.0) - atlasOffset, 1.0) * tileTotal;
                float z0 = floor(z);
                float z1 = min(z0 + 1.0, tileTotal - 1.0);
                float alpha = smoothstep(0.0, 1.0, fract(z));

                float tileX0 = mod(z0, tileCount);
                float tileY0 = floor(z0 / tileCount);
                float tileX1 = mod(z1, tileCount);
                float tileY1 = floor(z1 / tileCount);

                vec2 tileSize = iResolution.xy / tileCount;
                vec2 uvInTile = coord / iResolution.xy;
                vec2 pixel = uvInTile * (tileSize - 1.0);

                vec2 texCoord0 = (pixel + vec2(tileX0, tileY0) * tileSize + 0.5);
                vec2 texCoord1 = (pixel + vec2(tileX1, tileY1) * tileSize + 0.5);

                vec2 intensity0 = 10.0 * haloAtlasImage.eval(texCoord0).rg; // 10.0: recover original intensity
                vec2 intensity1 = 10.0 * haloAtlasImage.eval(texCoord1).rg; // 10.0: recover original intensity
                return mix(intensity0, intensity1, alpha);
            }

            vec4 main(vec2 fragCoord)
            {
                vec2 p = (2.0 * fragCoord - iResolution.xy) / iResolution.y; // 2.0: map uv to [-1, 1]
                vec2 precalculationData = SamplePrecalculationImage(fragCoord);
                float sdf = precalculationData.r;
                float tGlobal = precalculationData.g;

                // === Sliding window for glow arcs ===
                float weight1 = 0.0;
                float tLocal1 = 0.0;
                float weight2 = 0.0;
                float tLocal2 = 0.0;
                ComputeArcWindow(tGlobal, line1Start, line1Length, glowBellSigma, tLocal1, weight1);
                ComputeArcWindow(tGlobal, line2Start, line2Length, glowBellSigma, tLocal2, weight2);

                // === Glow and Halo color composition ===
                vec3 glowCol1 = vec3(0.0);
                vec3 glowCol2 = vec3(0.0);
                vec3 haloCol = vec3(0.0);
                GetSegmentGlowSDF(sdf, line1Color * weight1, tLocal1, glowCol1);
                GetSegmentGlowSDF(sdf, line2Color * weight2, tLocal2, glowCol2);
                GetGlobalGlowSDF(sdf, mix(line1Color, line2Color, 0.5), haloCol);

                vec3 glow = (glowCol1 + glowCol2 + haloCol) * 0.01; // 0.01: light strength scaling factor
                glow = 1.0 - exp(-glow); // tone mapping
                glow = pow(glow, vec3(0.4545)); // 0.4545: gamma

                float atlasOffset = haloAtlasImage.eval(fragCoord).b;
                vec2 haloIntensity = SampleHaloIntensity(fragCoord, atlasOffset);
                vec3 halo1 = vec3(0.0);
                vec3 halo2 = vec3(0.0);
                GetSegmentHalo(haloIntensity.r, line1Color, halo1);
                GetSegmentHalo(haloIntensity.g, line2Color, halo2);

                vec3 col = glow + halo1 + halo2;
                float alpha = max(max(col.r, col.g), col.b);
                return vec4(col, alpha);
            }
        )";
        contourDiagonalFlowLightShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (contourDiagonalFlowLightShaderEffect_ == nullptr) {
        GE_LOGE("GEContourDiagonalFlowLightShader contourDiagonalFlowLightShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(contourDiagonalFlowLightShaderEffect_);
}

std::shared_ptr<Drawing::Image> GEContourDiagonalFlowLightShader::DrawRuntimeShader(Drawing::Canvas& canvas,
    const Drawing::Rect& rect)
{
    if (cacheAnyPtr_ == nullptr) {
        GE_LOGW("GEContourDiagonalFlowLightShader DrawRuntimeShader cache is nullptr.");
        return nullptr;
    }
    auto precalculationImage = std::any_cast<CacheDataType>(*cacheAnyPtr_).first;
    auto haloAtlasImage = std::any_cast<CacheDataType>(*cacheAnyPtr_).second;
    if (precalculationImage == nullptr || haloAtlasImage == nullptr) {
        cacheAnyPtr_ = nullptr;
        GE_LOGW("GEContourDiagonalFlowLightShader DrawRuntimeShader cache img is nullptr.");
        return nullptr;
    }
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    builder_ = GetContourDiagonalFlowLightBuilder();
    Drawing::Matrix matrix;
    auto precalculationShader = Drawing::ShaderEffect::CreateImageShader(*precalculationImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto haloAtlasShader = Drawing::ShaderEffect::CreateImageShader(*haloAtlasImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    const float defaultLineLength = 0.3f;
    float line1Length = (contourDiagonalFlowLightParams_.line1Length_ < 0.0f ||
        std::abs(contourDiagonalFlowLightParams_.line1Length_) < std::numeric_limits<float>::epsilon()) ?
        defaultLineLength : contourDiagonalFlowLightParams_.line1Length_;
    float line2Length = (contourDiagonalFlowLightParams_.line2Length_ < 0.0f ||
        std::abs(contourDiagonalFlowLightParams_.line2Length_) < std::numeric_limits<float>::epsilon()) ?
        defaultLineLength : contourDiagonalFlowLightParams_.line2Length_;
    builder_->SetChild("precalculationImage", precalculationShader);
    builder_->SetChild("haloAtlasImage", haloAtlasShader);
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("line1Start", contourDiagonalFlowLightParams_.line1Start_);
    builder_->SetUniform("line1Length", std::clamp(line1Length, 0.0f, 1.0f));
    builder_->SetUniform("line1Color", std::clamp(contourDiagonalFlowLightParams_.line1Color_[NUM0], 0.0f, 1.0f),
        std::clamp(contourDiagonalFlowLightParams_.line1Color_[NUM1], 0.0f, 1.0f),
        std::clamp(contourDiagonalFlowLightParams_.line1Color_[NUM2], 0.0f, 1.0f));
    builder_->SetUniform("line2Start", contourDiagonalFlowLightParams_.line2Start_);
    builder_->SetUniform("line2Length", std::clamp(line2Length, 0.0f, 1.0f));
    builder_->SetUniform("line2Color", std::clamp(contourDiagonalFlowLightParams_.line2Color_[NUM0], 0.0f, 1.0f),
        std::clamp(contourDiagonalFlowLightParams_.line2Color_[NUM1], 0.0f, 1.0f),
        std::clamp(contourDiagonalFlowLightParams_.line2Color_[NUM2], 0.0f, 1.0f));
    builder_->SetUniform("lineThickness", std::clamp(contourDiagonalFlowLightParams_.thickness_, 0.0f, 1.0f));

    Drawing::ImageInfo imageInfo(rect.GetWidth(), rect.GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto img = builder_->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    return img;
}

void GEContourDiagonalFlowLightShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Preprocess(canvas, rect); // to calculate your cache data
    auto resImg = DrawRuntimeShader(canvas, rect);
    if (resImg) {
        Drawing::Brush brush;
        canvas.AttachBrush(brush);
        canvas.DrawImageRect(*resImg, rect, rect, Drawing::SamplingOptions());
        canvas.DetachBrush();
    }
}
} // namespace Rosen
} // namespace OHOS