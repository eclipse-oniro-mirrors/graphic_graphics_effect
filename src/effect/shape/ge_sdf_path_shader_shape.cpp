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

#include "ge_sdf_path_shader_shape.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stack>
#include <string>
#include <vector>

#include "common/rs_common_def.h"
#include "draw/surface.h"
#include "ge_log.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
static constexpr float MIN_SCALE = 100.0f;
static constexpr uint32_t LINE = 2;
constexpr uint32_t MAX_CURVES_SUBMIT_PER_GRID = 20;
constexpr uint32_t MIN_GRID_SIZE = 64;
constexpr uint32_t CURVE_FLOAT_COUNT = 6;
constexpr uint32_t XMIN_I = 0;
constexpr uint32_t XMAX_I = 1;
constexpr uint32_t YMIN_I = 2;
constexpr uint32_t YMAX_I = 3;
constexpr Drawing::ColorType RGBA_F16 = Drawing::ColorType::COLORTYPE_RGBA_F16;
constexpr float MIN_SCALE_CLAMP = 0.001f;
constexpr float NDC_MULTIPLIER = 2.0f;      // multiplier for NDC coordinate conversion
constexpr float NDC_OFFSET = 1.0f;          // offset for NDC coordinate conversion
constexpr float MIDPOINT_FACTOR = 0.5f;     // factor for calculating midpoint

bool IntersectBBox(const Box4f& a, const Box4f& b)
{
    return !(a[XMAX_I] < b[XMIN_I] || a[XMIN_I] > b[XMAX_I] || a[YMAX_I] < b[YMIN_I] || a[YMIN_I] > b[YMAX_I]);
}

// 150.0f: default base width, 250.0f: default base height
std::vector<Vector2f> ConvertPixelToNDC(const std::vector<Vector2f>& pixelPoints, float baseWidth = 150.0f,
    float baseHeight = 250.0f)
{
    if (baseWidth < 1.0f || baseHeight < 1.0f) { // 1.0f is minimum pixel size
        return {};
    }
    std::vector<Vector2f> ndcPoints;
    ndcPoints.reserve(pixelPoints.size());
    for (const auto& p : pixelPoints) {
        float normalizedX = p[0] / baseWidth;
        float normalizedY = p[1] / baseHeight; // 1 is index of y in point
        float ndcX = (normalizedX * NDC_MULTIPLIER - NDC_OFFSET) * (baseWidth / baseHeight);
        float ndcY = normalizedY * NDC_MULTIPLIER - NDC_OFFSET;
        ndcPoints.emplace_back(ndcX, ndcY);
    }
    return ndcPoints;
}

void ConvertPointsToFloats(const std::vector<Vector2f>& in, std::vector<float>& out)
{
    out.clear();
    for (auto& p : in) {
        out.push_back(p[0]);
        out.push_back(p[1]);
    }
}

Drawing::ImageInfo MakeOffscreenImageInfo(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    auto* canvasSurface = canvas.GetSurface();
    auto colorSpace = canvasSurface ? canvasSurface->GetImageInfo().GetColorSpace() : nullptr;
    return {rect.GetWidth(), rect.GetHeight(), RGBA_F16, Drawing::AlphaType::ALPHATYPE_OPAQUE, colorSpace};
}

using AddCurveSegment =
    std::function<void(std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers, Vector2f& currentPos)>;
static std::unordered_map<char, AddCurveSegment> addCurveSegmentMap_ = {
    { 'M', // M is the start of a path
        [](std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers, Vector2f& currentPos) {
            if (numbers.size() < 2) { // 2: the number size of one point
                LOGE("AddCurveSegment: M command requires 2 numbers");
                return;
            }
            currentPos = Vector2f(numbers[0], numbers[1]);
        } },
    { 'L', // L is liner segment
        [](std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers, Vector2f& currentPos) {
            if (numbers.size() < 2) { // 2: the number size of liner
                LOGE("AddCurveSegment: L command requires 2 numbers");
                return;
            }
            std::vector<Vector2f> segment;
            segment.push_back(currentPos);             // start point of curve (end point of the previous curve)
            Vector2f end = { numbers[0], numbers[1] }; // end point
            segment.push_back(end);
            currentPos = end;
            result.push_back(segment);
        } },
    { 'Q', // Q is quadratic bezier curve
        [](std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers, Vector2f& currentPos) {
            if (numbers.size() < 4) { // 4: the number size of quadratic bezier curve
                LOGE("AddCurveSegment: Q command requires 4 numbers");
                return;
            }
            std::vector<Vector2f> segment;
            segment.push_back(currentPos);                 // start point of curve (end point of the previous curve)
            segment.push_back({ numbers[0], numbers[1] }); // control point
            Vector2f end = { numbers[2], numbers[3] };     // end point
            segment.push_back(end);
            currentPos = end;
            result.push_back(segment);
        } },
    { 'C', // C is cubic bezier cuerve
        [](std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers, Vector2f& currentPos) {
            if (numbers.size() < 6) { // 6: the number size of cubic bezier cuerve
                LOGE("AddCurveSegment: C command requires 6 numbers");
                return;
            }
            std::vector<Vector2f> segment;
            segment.push_back(currentPos);                 // start point of curve (end point of the previous curve)
            segment.push_back({ numbers[0], numbers[1] }); // first control point
            segment.push_back({ numbers[2], numbers[3] }); // second control point
            Vector2f end = { numbers[4], numbers[5] };     // end point
            segment.push_back(end);
            currentPos = end;
            result.push_back(segment);
        } },
    { 'Z', // Z M is the end of a path
        [](std::vector<std::vector<Vector2f>>& result, std::vector<float>& numbers,
               Vector2f& currentPos) { LOGD("AddCurveSegment: Z a path ends"); } }
};
} // namespace

static const std::string SDF_PROPAGATION_SHADER = R"(
    uniform vec2 iResolution;
    uniform float u_step;
    uniform shader u_sdfTex;
    uniform shader u_maskTex;

    const float INF_1E4 = 1e4;
    const float EPSILON_1E_MINUS_3 = 0.001;

    float sampleSdf(vec2 baseUv, vec2 offset) {
        float sdf = abs(u_sdfTex.eval(baseUv + offset).r);
        if (sdf < INF_1E4 - EPSILON_1E_MINUS_3) {
            float ndcOffset = length(offset) * (2.0 / iResolution.y);
            return sdf + ndcOffset;
        }
        return INF_1E4;
    }

    vec4 main(vec2 fragCoord) {
        float mask = u_maskTex.eval(fragCoord).r;
        float currentSdf = u_sdfTex.eval(fragCoord).r;
        // Use the sign of the mask outside the curve
        if (mask < EPSILON_1E_MINUS_3) {
            return vec4(vec3(abs(currentSdf)), 1.0);
        }
        if (mask < 1.0) {
            return vec4(vec3(currentSdf), 1.0);
        }

        float step = u_step;
        float minSdf = abs(currentSdf);
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(-step, -step)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(0.0, -step)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(step, -step)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(-step, 0.0)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(step, 0.0)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(-step, step)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(0.0, step)));
        minSdf = min(minSdf, sampleSdf(fragCoord, vec2(step, step)));

        minSdf *= -1.0;
        return vec4(vec3(minSdf), 1.0);
    }
)";

static const std::string PRECALCULATION_FOR_SDF_SHADER = R"(
    uniform vec2 iResolution;
    const int CAPACITY = 60;
    const int MAX_CURVES_IN_GRID = 20;
    uniform float u_curveCount;
    uniform vec2 controlPoints[CAPACITY];
    uniform float segmentIndex[MAX_CURVES_IN_GRID];
    uniform shader u_prevD;
    uniform float u_isFirstBatch;
    uniform vec2 u_validRange;

    const float INF_1E4 = 1e4;
    const float SQRT3 = 1.7320508;
    const float ENDPOINT_MATCH_EPS = 1e-3;
    const float CORNER_TYPE_EPS = 1e-6;
    const float EPSILON_1E_MINUS_8 = 1e-8;
    const float ONE_THIRD = 1.0 / 3.0;

    float cro(vec2 a, vec2 b) {
        return a.x * b.y - a.y * b.x;
    }

    bool isDistinct(vec2 p1, vec2 p2) {
        return length(p1 - p2) > 1e-4;
    }

    float distanceToAABBSq(vec2 p, vec2 minP, vec2 maxP) {
        vec2 d = max(minP - p, max(p - maxP, vec2(0.0)));
        return dot(d, d);
    }

    float sdSegmentSigned(vec2 p, vec2 a, vec2 b) {
        vec2 pa = p - a;
        vec2 ba = b - a;
        float cross_val = cro(ba, pa);
        float side = cross_val > 0.0 ? -1.0 : 1.0;
        float ba2 = dot(ba, ba);
        if (ba2 < EPSILON_1E_MINUS_8) {
            return dot(pa, pa) * side;
        }
        float h = clamp(dot(pa, ba) / ba2, 0.0, 1.0);
        vec2 dvec = pa - h * ba;
        return dot(dvec, dvec) * side;
    }

    float sdfBezierStable(vec2 p, vec2 v0, vec2 v1, vec2 v2)
    {
        vec2 a = v1 - v0;
        float area2 = abs(cro(a, v2 - v0));
        if (area2 < CORNER_TYPE_EPS) {
            return sdSegmentSigned(p, v0, v2);
        }
        vec2 b = v0 - 2.0 * v1 + v2;

        // Solving cubic equations
        vec2 c = a * 2.0;
        vec2 d = v0 - p;
        float kk = 1.0 / dot(b, b);
        float kx = kk * dot(a, b);
        float ky = kk * (2.0 * dot(a, a) + dot(d, b)) * ONE_THIRD;
        float kz = kk * dot(d, a);
        float res = 0.0;
        float pp = ky - kx * kx;
        float qq = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
        float hh = qq * qq + 4.0 * pp * pp * pp;

        float tCubic;
        vec2 qvCubic;
        float distCubic;
        bool ishh = false;

        if (hh >= 0.0) {
            hh = sqrt(hh);
            vec2 x = 0.5 * (vec2(hh, -hh) - qq);
            vec2 uv = sign(x) * pow(abs(x), vec2(ONE_THIRD));
            tCubic = clamp(uv.x + uv.y - kx, 0.0, 1.0);
            qvCubic = d + (c + b * tCubic) * tCubic;
            distCubic = dot(qvCubic, qvCubic);
            ishh = true;
        }
        if (!ishh) {
            float z = sqrt(-pp);
            float v = acos(clamp(qq / (pp * z * 2.0), -1.0, 1.0)) * ONE_THIRD;
            float m = cos(v);
            float n = sin(v) * SQRT3;

            float t1 = clamp((m + m) * z - kx, 0.0, 1.0);
            vec2 q1 = d + (c + b * t1) * t1;
            float d1 = dot(q1, q1);

            float t2 = clamp((-n - m) * z - kx, 0.0, 1.0);
            vec2 q2 = d + (c + b * t2) * t2;
            float d2 = dot(q2, q2);

            tCubic = (d1 <= d2) ? t1 : t2;
            qvCubic = (d1 <= d2) ? q1 : q2;
            distCubic = min(d1, d2);
        }

        // calculate signs
        float distV0 = dot(d, d);
        vec2 dV2 = v2 - p;
        float distV2 = dot(dV2, dV2);
        float bestDist2 = min(distCubic, min(distV0, distV2));

        float bestT = tCubic;
        vec2 bestVec = qvCubic;
        bool check1 = false;
        if (distV0 <= distCubic && distV0 <= distV2) {
            bestT = 0.0;
            bestVec = d;
            check1 = true;
        }
        if (!check1 && distV2 <= distCubic && distV2 <= distV0) {
            bestT = 1.0;
            bestVec = dV2;
        }

        vec2 tangent = 2.0 * (a + b * bestT);
        float side = cro(tangent, bestVec) < 0.0 ? -1.0 : 1.0;
        return bestDist2 * side;
    }

    int bezierCornerType(vec2 p1, vec2 p2, vec2 p3, float eps)
    {
        vec2 v1 = p1 - p2;
        vec2 v2 = p3 - p2;
        float cross = cro(v1, v2);
        return cross > eps ? 1 : (cross < -eps ? -1 : 0);
    }

    float calculateMinimumWithCorrectSign(vec2 p, float d1, float d2,
        vec2 c1_0, vec2 c1_1, vec2 c1_2,
        vec2 c2_0, vec2 c2_1, vec2 c2_2)
    {
        float sgn1 = sign(d1);
        float sgn2 = sign(d2);
        float absD1 = abs(d1);
        float absD2 = abs(d2);

        float finalD = absD1 < absD2 ? d1 : d2;

        bool share1 = length(c1_2 - c2_0) < ENDPOINT_MATCH_EPS;
        bool share2 = length(c2_2 - c1_0) < ENDPOINT_MATCH_EPS;

        vec2 p1, p2, p3;
        bool isCorner = false;
        bool isShare1 = false;

        if (share1) {
            p2 = c1_2;
            float distToCorner = length(p - p2);
            if (absD1 >= distToCorner - 1e-3 && absD2 >= distToCorner - 1e-3) {
                p1 = isDistinct(c1_1, c1_2) ? c1_1 : c1_0;
                p3 = isDistinct(c2_0, c2_1) ? c2_1 : c2_2;
                isCorner = true;
            }
            isShare1 = true;
        }
        if (!isShare1 && share2) {
            p2 = c2_2;
            float distToCorner = length(p - p2);
            if (absD1 >= distToCorner - 1e-3 && absD2 >= distToCorner - 1e-3) {
                p1 = isDistinct(c2_1, c2_2) ? c2_1 : c2_0;
                p3 = isDistinct(c1_0, c1_1) ? c1_1 : c1_2;
                isCorner = true;
            }
        }

        if (!isCorner) {
            return finalD;
        }
        int cornerType = bezierCornerType(p1, p2, p3, CORNER_TYPE_EPS);
        if (cornerType > 0) {
            finalD = min(absD1, absD2) * min(sgn1, sgn2);
        }
        if (cornerType < 0) {
            finalD = min(absD1, absD2) * max(sgn1, sgn2);
        }

        return finalD;
    }

    void FindTwoClosestCurves(vec2 p, out float d1, out float d2,
        out vec2 c1_0, out vec2 c1_1, out vec2 c1_2,
        out vec2 c2_0, out vec2 c2_1, out vec2 c2_2, out float closestIdx)
    {
        d1 = INF_1E4;
        d2 = INF_1E4;
        closestIdx = -1.0;
        c1_0 = vec2(0.0);
        c1_1 = vec2(0.0);
        c1_2 = vec2(0.0);
        c2_0 = vec2(0.0);
        c2_1 = vec2(0.0);
        c2_2 = vec2(0.0);

        for (int i = 0; i < MAX_CURVES_IN_GRID; i++) {
            int global_curve_idx = int(segmentIndex[i]);
            if (global_curve_idx < 0) break;

            vec2 A = controlPoints[i * 3];
            vec2 B = controlPoints[i * 3 + 1];
            vec2 C = controlPoints[i * 3 + 2];

            vec2 minP = min(min(A, B), C);
            vec2 maxP = max(max(A, B), C);
            float aabbDistSq = distanceToAABBSq(p, minP, maxP);

            // Skip if the AABB distance is greater than the second smallest distance
            if (aabbDistSq > abs(d2)) {
                continue;
            }

            float d = sdfBezierStable(p, A, B, C);
            float absD = abs(d);
            bool isd1 = false;

            if (absD < abs(d1)) {
                d2 = d1; c2_0 = c1_0; c2_1 = c1_1; c2_2 = c1_2;
                d1 = d;  c1_0 = A;    c1_1 = B;    c1_2 = C;
                isd1 = true;
                closestIdx = float(i);
            }
            if (!isd1 && absD < abs(d2)) {
                d2 = d;  c2_0 = A;    c2_1 = B;    c2_2 = C;
            }
        }
    }

    vec4 main(vec2 fragCoord) {
        vec2 uv = fragCoord / iResolution;
        float ndcAspect = iResolution.x / iResolution.y;
        vec2 ndc = uv * 2.0 - 1.0;
        ndc.x *= ndcAspect;

        float d1;
        float d2;
        vec2 c1[3];
        vec2 c2[3];
        float closestIdx;
        FindTwoClosestCurves(ndc, d1, d2, c1[0], c1[1], c1[2], c2[0], c2[1], c2[2], closestIdx);
        d1 = sqrt(abs(d1)) * sign(d1);
        d2 = sqrt(abs(d2)) * sign(d2);

        float currentD = calculateMinimumWithCorrectSign(ndc, d1, d2, c1[0], c1[1], c1[2], c2[0], c2[1], c2[2]);
        if (closestIdx > -0.1 && (closestIdx < u_validRange.x - 0.1 || closestIdx > u_validRange.y + 0.1)) {
            currentD = 1e4;
        }

        if (u_isFirstBatch > 0.0) {
            return vec4(vec3(currentD), 1.0);
        }
        float prevD = u_prevD.eval(fragCoord).r;
        if (abs(prevD) < abs(currentD)) {
            currentD = prevD;
        }
        return vec4(vec3(currentD), 1.0);
    }
)";

static const std::string NORMAL_CALCULATION_SHADER = R"(
    uniform vec2 iResolution;
    uniform shader u_seeds;
    uniform float pixelScale;

    vec4 main(vec2 fragCoord) {
        float centerSdf = u_seeds.eval(fragCoord).r * pixelScale;
        float h = 1.0 + clamp(abs(centerSdf) * 0.2, 0.0, 8.0);

        float L = u_seeds.eval(fragCoord + vec2(-h, 0.0)).r;
        float R = u_seeds.eval(fragCoord + vec2(h, 0.0)).r;
        float T = u_seeds.eval(fragCoord + vec2(0.0, -h)).r;
        float B = u_seeds.eval(fragCoord + vec2(0.0, h)).r;

        vec2 normal = vec2(R - L, B - T) * 0.5;
        float len = length(normal);
        vec2 tempNormal = vec2(0.0, 0.0);
        if (len > 0.00001) {
            tempNormal = normal / len;
        }
        normal = tempNormal;

        return vec4(normal, 0.0, centerSdf);
    }
)";

static const std::string CLEAR_INF_SHADER = R"(
        const float INF_1E4 = 1e4;
        vec4 main(vec2 fragCoord) {
            return vec4(vec3(INF_1E4), 1.0);
    }
)";

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_precalcShaderEffect_ = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_normalShaderEffect_ = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_sdfPropEffect_ = nullptr;
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_clearInfEffect = nullptr;

static std::vector<float> parseNumbers(const std::string& s, size_t& i)
{
    std::vector<float> result;
    while (i < s.length()) {
        // Skip spaces
        while (i < s.length() && std::isspace(s[i])) {
            i++;
        }
        if (i >= s.length()) {
            break;
        }

        // check whether it is the next command (letter).
        char c = static_cast<char>(std::toupper(s[i]));
        if (std::isalpha(c) && !(i + 1 < s.length() && (s[i + 1] == '-' || std::isdigit(s[i + 1])))) {
            LOGD("next command: %{public}c", c);
            break;
        }

        // Parse a number
        size_t start = i;
        bool hasDigits = false;
        while (i < s.length() && (std::isdigit(s[i]) || s[i] == '-' || s[i] == '.')) {
            if (std::isdigit(s[i])) {
                hasDigits = true;
            }
            i++;
        }

        if (!hasDigits || i == start) {
            break;
        }

        // converting numbers
        double val = std::stod(s.substr(start, i - start));
        result.push_back(static_cast<float>(val));
    }
    return result;
}

std::vector<std::vector<Vector2f>> GESDFPathShaderShape::GetCurveByPath(const Drawing::Path& path)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::GetCurveByPath");
    std::vector<std::vector<Vector2f>> result = {};
    if (!path.IsValid()) {
        LOGE("GESDFPathShaderShape::GetCurveByPath: path is invalid.");
        return result;
    }
    std::string svgPath = path.ConvertToSVGString();
    Vector2f currentPos; // Curren position is

    size_t i = 0;
    while (i < svgPath.length()) {
        // Skip spaces
        while (i < svgPath.length() && std::isspace(svgPath[i])) {
            i++;
        }
        if (i >= svgPath.length()) {
            break;
        }

        char cmd = static_cast<char>(std::toupper(svgPath[i]));
        i++;
        // Read the corresponding number of digits according to the letters
        std::vector<float> numbers = parseNumbers(svgPath, i);
        auto it = addCurveSegmentMap_.find(cmd);
        if (it != addCurveSegmentMap_.end()) {
            it->second(result, numbers, currentPos);
        } else {
            LOGE("The %{public}c is not supported", cmd);
            continue;
        }
    }
    return result;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFPathShaderShape::MakePrecalcShaderBuilder()
{
    if (g_precalcShaderEffect_ == nullptr) {
        g_precalcShaderEffect_ = RuntimeEffect::CreateForShader(PRECALCULATION_FOR_SDF_SHADER);
        if (g_precalcShaderEffect_ == nullptr) {
            LOGE("GESDFPathShaderShape::MakePrecalcShaderBuilder failed to create effect");
            return nullptr;
        }
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_precalcShaderEffect_);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFPathShaderShape::MakeNormalShaderBuilder()
{
    if (g_normalShaderEffect_ == nullptr) {
        g_normalShaderEffect_ = RuntimeEffect::CreateForShader(NORMAL_CALCULATION_SHADER);
        if (g_normalShaderEffect_ == nullptr) {
            LOGE("GESDFPathShaderShape::MakeNormalShaderBuilder failed to create effect");
            return nullptr;
        }
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_normalShaderEffect_);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESDFPathShaderShape::MakeSdfPropShaderBuilder()
{
    if (g_sdfPropEffect_ == nullptr) {
        g_sdfPropEffect_ = RuntimeEffect::CreateForShader(SDF_PROPAGATION_SHADER);
        if (g_sdfPropEffect_ == nullptr) {
            LOGE("GESDFPathShaderShape::MakeSdfPropShaderBuilder failed to create effect");
            return nullptr;
        }
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_sdfPropEffect_);
}

std::shared_ptr<Image> GESDFPathShaderShape::RunSDFPropagation(
    Canvas& canvas, std::shared_ptr<Image> sdfTex, std::shared_ptr<Image> maskTex, int width, int height)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::RunSDFPropagation");

    if (numPasses_ <= 0) {
        return sdfTex;
    }

    auto gpuContext = canvas.GetGPUContext();
    if (!gpuContext) {
        LOGE("GESDFPathShaderShape::RunSDFPropagation no GPU context");
        return sdfTex;
    }

    SamplingOptions nearest(FilterMode::NEAREST, MipmapMode::NONE);
    auto outputImageInfo = sdfTex->GetImageInfo();

    auto builder = MakeSdfPropShaderBuilder();
    if (!builder) {
        LOGE("GESDFPathShaderShape::RunSDFPropagation pass builder failed");
        return sdfTex;
    }

    auto maskShader = ShaderEffect::CreateImageShader(*maskTex, TileMode::CLAMP, TileMode::CLAMP, nearest, Matrix());
    builder->SetChild("u_maskTex", maskShader);
    builder->SetUniform("iResolution", static_cast<float>(width), static_cast<float>(height));

    std::shared_ptr<Image> input = sdfTex;
    std::shared_ptr<Image> output = nullptr;
    for (size_t i = 0; i < numPasses_; i++) {
        int pixelStep = static_cast<int>(pow(2.0f, numPasses_ - i - 1)); // 2.0f: power base
        auto inputSdfShader =
            ShaderEffect::CreateImageShader(*input, TileMode::CLAMP, TileMode::CLAMP, nearest, Matrix());
        builder->SetChild("u_sdfTex", inputSdfShader);
        builder->SetUniform("u_step", static_cast<float>(pixelStep));
        output = builder->MakeImage(gpuContext.get(), nullptr, outputImageInfo, false);
        if (!output) {
            LOGE("GESDFPathShaderShape::RunSDFPropagation pass %zu MakeImage failed", i);
            return input;
        }

        std::swap(input, output);
    }
    return input;
}

std::shared_ptr<Image> GESDFPathShaderShape::ComputeDistanceField(
    Canvas& canvas, std::shared_ptr<Image> sdfTex, int width, int height)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::ComputeDistanceField");
    auto gpuContext = canvas.GetGPUContext();
    if (!gpuContext) {
        LOGE("GESDFPathShaderShape::ComputeDistanceField no GPU context");
        return nullptr;
    }

    SamplingOptions nearest(FilterMode::LINEAR, MipmapMode::LINEAR);
    auto inputImageInfo = sdfTex->GetImageInfo();

    auto normalBuilder = MakeNormalShaderBuilder();
    if (!normalBuilder) {
        LOGE("GESDFPathShaderShape::ComputeDistanceField effect create failed");
        return nullptr;
    }

    auto inputShader = ShaderEffect::CreateImageShader(*sdfTex, TileMode::CLAMP, TileMode::CLAMP, nearest, Matrix());
    normalBuilder->SetChild("u_seeds", inputShader);
    normalBuilder->SetUniform("pixelScale", static_cast<float>(height * (1.0f / params_.scale.y_)));
    normalBuilder->SetUniform("iResolution", static_cast<float>(width), static_cast<float>(height));
#ifdef RS_ENABLE_GPU
    return normalBuilder->MakeImage(gpuContext.get(), nullptr, inputImageInfo, false);
#else
    return normalBuilder->MakeImage(nullptr, nullptr, inputImageInfo, false);
#endif
}

void GESDFPathShaderShape::CreateSurfaceAndCanvas(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::CreateSurfaceAndCanvas");
    if (!rect.IsValid()) {
        GE_LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas rect is nullptr.");
        return;
    }
    auto imageInfo = MakeOffscreenImageInfo(canvas, rect);
    offscreenSurface_ = Drawing::Surface::MakeRenderTarget(canvas.GetGPUContext().get(), false, imageInfo);
    if (!offscreenSurface_) {
        LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas offscreenSurface is invalid");
        return;
    }
    offscreenCanvas_ = offscreenSurface_->GetCanvas();
    if (!offscreenCanvas_) {
        LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas offscreenCanvas is invalid");
        return;
    }
    auto FallbackClear = [this, &rect]() {
        Drawing::Brush brush;
        brush.SetColor(0x00000000);
        offscreenCanvas_->AttachBrush(brush);
        offscreenCanvas_->DrawRect(rect);
        offscreenCanvas_->DetachBrush();
    };
    if (!g_clearInfEffect) {
        g_clearInfEffect = RuntimeEffect::CreateForShader(CLEAR_INF_SHADER);
        if (!g_clearInfEffect) {
            LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas CreateForShader failed");
            FallbackClear();
            return;
        }
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(g_clearInfEffect);
    if (!builder) {
        LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas create builder failed");
        FallbackClear();
        return;
    }
    auto clearShader = builder->MakeShader(nullptr, false);
    if (!clearShader) {
        LOGE("GESDFPathShaderShape::CreateSurfaceAndCanvas clear Shader failed");
        FallbackClear();
        return;
    }
    Drawing::Brush clearBrush;
    clearBrush.SetShaderEffect(clearShader);
    offscreenCanvas_->AttachBrush(clearBrush);
    offscreenCanvas_->DrawRect(rect);
    offscreenCanvas_->DetachBrush();
}

void GESDFPathShaderShape::AutoGridPartition(float width, float height, const std::vector<Vector2f>& pixelControlPoints)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::AutoGridPartition");
    if (width < MIN_SCALE_CLAMP || height < MIN_SCALE_CLAMP) {
        GE_LOGE("GESDFPathShaderShape::AutoGridPartition surface size not valid");
        return;
    }
    curvesInGrid_.clear();
    allGridsCovered_ = true;
    maxEmptyGridShortSide_ = 0.0f;

    std::vector<Box4f> curveBBoxes;
    Box4f canvasBBox;
    ComputeAllCurveBoundingBoxes(width, height, pixelControlPoints, canvasBBox, curveBBoxes);

    std::queue<Grid> workQueue;
    InitializeWorkQueue(canvasBBox, curveBBoxes, workQueue);

    while (!workQueue.empty()) {
        Grid current = workQueue.front();
        workQueue.pop();
        float w = current.bbox[XMAX_I] - current.bbox[XMIN_I];
        float h = current.bbox[YMAX_I] - current.bbox[YMIN_I];
        bool needsSplit = (current.curveIndices.size() > 4) && // 4: max curves per grid
                          (w > MIN_GRID_SIZE || h > MIN_GRID_SIZE);
        if (needsSplit) {
            SplitGrid(current, curveBBoxes, workQueue, MIN_GRID_SIZE);
        } else {
            ProcessFinalGrid(current, curveBBoxes);
        }
    }
}

void GESDFPathShaderShape::SplitGrid(
    const Grid& current, const std::vector<Box4f>& curveBBoxes, std::queue<Grid>& workQueue, float minGridSize)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::SplitGrid");
    float midX = (current.bbox[XMIN_I] + current.bbox[XMAX_I]) * MIDPOINT_FACTOR;
    float midY = (current.bbox[YMIN_I] + current.bbox[YMAX_I]) * MIDPOINT_FACTOR;

    // Divide into four quadrants
    std::vector<Box4f> quadrants = {
        { current.bbox[XMIN_I], midX, current.bbox[YMIN_I], midY }, // Top-Left
        { midX, current.bbox[XMAX_I], current.bbox[YMIN_I], midY }, // Top-Right
        { current.bbox[XMIN_I], midX, midY, current.bbox[YMAX_I] }, // Bottom-Left
        { midX, current.bbox[XMAX_I], midY, current.bbox[YMAX_I] }  // Bottom-Right
    };

    if (current.bbox[XMAX_I] - current.bbox[XMIN_I] <= MIN_GRID_SIZE) {
        quadrants = {
            { current.bbox[XMIN_I], current.bbox[XMAX_I], current.bbox[YMIN_I], midY }, // Top
            { current.bbox[XMIN_I], current.bbox[XMAX_I], midY, current.bbox[YMAX_I] }  // Bottom
        };
    } else if (current.bbox[YMAX_I] - current.bbox[YMIN_I] <= MIN_GRID_SIZE) {
        quadrants = {
            { current.bbox[XMIN_I], midX, current.bbox[YMIN_I], current.bbox[YMAX_I] }, // Left
            { midX, current.bbox[XMAX_I], current.bbox[YMIN_I], current.bbox[YMAX_I] }  // Right
        };
    }

    for (const auto& quad : quadrants) {
        Grid child { quad, {} };
        for (int idx : current.curveIndices) {
            if (IntersectBBox(child.bbox, curveBBoxes[idx])) {
                child.curveIndices.push_back(idx);
            }
        }
        if (!child.curveIndices.empty()) {
            workQueue.push(child);
        } else {
            allGridsCovered_ = false;
            float side = std::min(quad[XMAX_I] - quad[XMIN_I],
                                  quad[YMAX_I] - quad[YMIN_I]);
            if (side > maxEmptyGridShortSide_ && side < canvasMinSide_ * 0.25f) { // 0.25: Corresponds to 2 partitions
                maxEmptyGridShortSide_ = side;
            }
        }
    }
}

void GESDFPathShaderShape::ComputeAllCurveBoundingBoxes(float width, float height,
    const std::vector<Vector2f>& pixelControlPoints, Box4f& canvasBBox, std::vector<Box4f>& curveBBoxes)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::ComputeAllCurveBoundingBoxes");
    curveBBoxes.clear();
    curveBBoxes.reserve(numCurves_);
    canvasBBox = { width, 0.0f, height, 0.0f };
    float maxThickness = 0.05f / NDC_MULTIPLIER * height; // 0.05f: Expanding the bounding box in NDC space
    for (size_t i = 0; i < numCurves_; ++i) {
        Box4f bbox = ComputeCurveBoundingBox(i, maxThickness, pixelControlPoints, width, height);
        curveBBoxes.push_back(bbox);
        canvasBBox[XMIN_I] = std::min(bbox[XMIN_I], canvasBBox[XMIN_I]);
        canvasBBox[XMAX_I] = std::max(bbox[XMAX_I], canvasBBox[XMAX_I]);
        canvasBBox[YMIN_I] = std::min(bbox[YMIN_I], canvasBBox[YMIN_I]);
        canvasBBox[YMAX_I] = std::max(bbox[YMAX_I], canvasBBox[YMAX_I]);
    }
    float canvasW = canvasBBox[XMAX_I] - canvasBBox[XMIN_I];
    float canvasH = canvasBBox[YMAX_I] - canvasBBox[YMIN_I];
    canvasMinSide_ = std::min(canvasW, canvasH);

    const float expandPixels = 10.0f * params_.scale.y_; // 10.0:Expand the overall bbox ensures enough pixels outside
    canvasBBox[XMIN_I] = std::max(canvasBBox[XMIN_I] - expandPixels, 0.0f);
    canvasBBox[XMAX_I] = std::min(canvasBBox[XMAX_I] + expandPixels, width);
    canvasBBox[YMIN_I] = std::max(canvasBBox[YMIN_I] - expandPixels, 0.0f);
    canvasBBox[YMAX_I] = std::min(canvasBBox[YMAX_I] + expandPixels, height);
}

Box4f GESDFPathShaderShape::ComputeCurveBoundingBox(size_t curveIndex, float maxThickness,
    const std::vector<Vector2f>& pixelControlPoints, float width, float height)
{
    size_t baseIdx = curveIndex * 3; // 3:Three control points of a quadratic curve
    Vector2f A = pixelControlPoints[baseIdx]; // startPoint
    Vector2f B = pixelControlPoints[baseIdx + 1]; // controlPoint
    Vector2f C = pixelControlPoints[baseIdx + 2]; // endPoint

    float minX = std::max(std::floor(std::min({A[0], B[0], C[0]}) - maxThickness), 0.0f);
    float maxX = std::min(std::ceil(std::max({A[0], B[0], C[0]}) + maxThickness), width);
    float minY = std::max(std::floor(std::min({A[1], B[1], C[1]}) - maxThickness), 0.0f);
    float maxY = std::min(std::ceil(std::max({A[1], B[1], C[1]}) + maxThickness), height);
    return { minX, maxX, minY, maxY };
}

void GESDFPathShaderShape::InitializeWorkQueue(
    const Box4f& canvasBBox, const std::vector<Box4f>& curveBBoxes, std::queue<Grid>& workQueue)
{
    std::vector<uint32_t> initialCurves(numCurves_);
    for (size_t i = 0; i < numCurves_; ++i) {
        initialCurves[i] = i;
    }
    workQueue.push({ canvasBBox, initialCurves });
}

void GESDFPathShaderShape::ProcessFinalGrid(Grid& current, const std::vector<Box4f>& curveBBoxes)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::ProcessFinalGrid");
    Grid processedGrid = current;
    std::vector<float> gridCurves;
    std::vector<float> inOrderSeg;
    for (size_t idx : processedGrid.curveIndices) {
        size_t baseIdx = idx * CURVE_FLOAT_COUNT;
 
        gridCurves.push_back(controlPoints_[baseIdx]);     // startPoint.x
        gridCurves.push_back(controlPoints_[baseIdx + 1]); // 1: startPoint.y
        gridCurves.push_back(controlPoints_[baseIdx + 2]); // 2: controlPoint.x
        gridCurves.push_back(controlPoints_[baseIdx + 3]); // 3: controlPoint.y
        gridCurves.push_back(controlPoints_[baseIdx + 4]); // 4: endPoint.x
        gridCurves.push_back(controlPoints_[baseIdx + 5]); // 5: endPoint.y
        inOrderSeg.push_back(static_cast<float>(idx));
    }
    curvesInGrid_.push_back(std::make_pair(gridCurves, processedGrid));
    segmentIndex_.push_back(inOrderSeg);
}

Drawing::Path GESDFPathShaderShape::PreparePathForRendering(const Drawing::Rect& rect, float& width, float& height)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::PreparePathForRendering");
    UpdateScale(params_.scale, rect);
    width = rect.GetWidth() * params_.scale.x_;
    height = rect.GetHeight() * params_.scale.y_;

    Drawing::Matrix matrix;
    matrix.SetScale(params_.scale.x_, params_.scale.y_);
    Drawing::Path path = params_.path;
    path.Offset(params_.offset.x_, params_.offset.y_);
    path.Transform(matrix);
    return path;
}

std::vector<Vector2f> GESDFPathShaderShape::ProcessCurveSegments(const std::vector<std::vector<Vector2f>>& paramsCoef)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::ProcessCurveSegments");
    std::vector<Vector2f> pixelControlPoints;
    numCurves_ = 0;

    for (auto& segment : paramsCoef) {
        if (segment.size() == LINE) {
            pixelControlPoints.push_back(segment[0]);
            pixelControlPoints.push_back(segment[0]);
            pixelControlPoints.push_back(segment[1]); // 1 is index
            numCurves_++;
        } else if (segment.size() == 3) { // 3 is size of QUADRATIC BEZIER
            pixelControlPoints.push_back(segment[0]);
            pixelControlPoints.push_back(segment[1]); // 1 is index
            pixelControlPoints.push_back(segment[2]); // 2 is index of QUADRATIC BEZIER
            numCurves_++;
        } else if (segment.size() == 4) { // 4 is size of CUBIC BEZIER
            Vector2f controlPoint = segment[1]; // 1 is index
            // 0, 2, 3 is index of CUBIC BEZIER
            cubicToQuadraticSingle(segment[0], controlPoint, segment[2], segment[3]);
            pixelControlPoints.push_back(segment[0]);
            pixelControlPoints.push_back(controlPoint);
            pixelControlPoints.push_back(segment[3]); // 3 is index of CUBIC BEZIER
            numCurves_++;
        }
    }
    return pixelControlPoints;
}

void GESDFPathShaderShape::ProcessSingleBatch(Drawing::RuntimeShaderBuilder& builder, size_t gridIndex, size_t batch,
    size_t start, size_t end, float vStart, float vEnd, std::shared_ptr<Drawing::Image>& prevSdf,
    std::shared_ptr<Drawing::ShaderEffect>& prevShader)
{
    const auto& grid = curvesInGrid_[gridIndex];
    const auto& allCurves = grid.first;
    const auto& allSegments = segmentIndex_[gridIndex];
    const Box4f area = grid.second.bbox;
    const Drawing::Rect rectN(area[XMIN_I], area[YMIN_I], area[XMAX_I], area[YMAX_I]);

    std::vector<float> batchCurves;
    std::vector<float> batchSegments;
    batchCurves.reserve(MAX_CURVES_SUBMIT_PER_GRID * CURVE_FLOAT_COUNT);
    batchSegments.reserve(MAX_CURVES_SUBMIT_PER_GRID);

    size_t base = start * CURVE_FLOAT_COUNT;
    batchCurves.insert(batchCurves.end(), allCurves.begin() + base, allCurves.begin() + end * CURVE_FLOAT_COUNT);
    batchSegments.insert(batchSegments.end(), allSegments.begin() + start, allSegments.begin() + end);

    batchCurves.resize(MAX_CURVES_SUBMIT_PER_GRID * CURVE_FLOAT_COUNT, -2.0f); // -2.0f: not included in the NDC space
    batchSegments.resize(MAX_CURVES_SUBMIT_PER_GRID, -1.0f); // -1.0f: invalid negative curve index

    builder.SetUniform("controlPoints", batchCurves.data(), batchCurves.size());
    builder.SetUniform("segmentIndex", batchSegments.data(), batchSegments.size());
    builder.SetUniform("u_isFirstBatch", (batch == 0) ? 1.0f : 0.0f);
    builder.SetUniform("u_validRange", vStart, vEnd);

    SamplingOptions nearest(FilterMode::LINEAR, MipmapMode::LINEAR);
    if (batch > 0) {
        prevSdf = offscreenSurface_->GetImageSnapshot();
        if (!prevSdf) {
            LOGE("GESDFPathShaderShape::ProcessSingleBatch: failed to get image snapshot from offscreen surface");
            return;
        }
        prevShader = ShaderEffect::CreateImageShader(*prevSdf,
            TileMode::CLAMP, TileMode::CLAMP, nearest, Matrix());
        if (!prevShader) {
            LOGE("GESDFPathShaderShape::ProcessSingleBatch failed: failed to create image shader");
            return;
        }
        builder.SetChild("u_prevD", prevShader);
    }

    auto shader = builder.MakeShader(nullptr, false);
    if (!shader) {
        LOGE("GESDFPathShaderShape::ProcessSingleBatch shader create failed");
        return;
    }

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    offscreenCanvas_->AttachBrush(brush);
    offscreenCanvas_->DrawRect(rectN);
    offscreenCanvas_->DetachBrush();
}

void GESDFPathShaderShape::RenderGridsToSurface(const Drawing::Rect& targetRect)
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::RenderGridsToSurface");
    auto builder = MakePrecalcShaderBuilder();
    if (!builder) {
        LOGE("GESDFPathShaderShape::RenderGridsToSurface builder create failed");
        return;
    }

    builder->SetUniform("iResolution", targetRect.GetWidth(), targetRect.GetHeight());
    builder->SetUniform("u_curveCount", static_cast<float>(numCurves_));
    std::shared_ptr<Drawing::Image> prevSdf = nullptr;
    std::shared_ptr<Drawing::ShaderEffect> prevShader = nullptr;
    for (size_t i = 0; i < curvesInGrid_.size(); i++) {
        size_t totalCurves = curvesInGrid_[i].first.size() / CURVE_FLOAT_COUNT;
        if (totalCurves == 0) continue;
        const size_t curvesPerBatch = MAX_CURVES_SUBMIT_PER_GRID;
        const size_t step = curvesPerBatch > 2 ? curvesPerBatch - 2 : 1; // 2-curve overlap between adjacent batches
        size_t start = 0;
        size_t batch = 0;
        while (start < totalCurves) {
            size_t end = std::min(start + curvesPerBatch, totalCurves);
            float validStart = (batch == 0) ? 0.0f : 1.0f;
            float validEnd = (end == totalCurves) ? (end - start - 1.0f) : (curvesPerBatch - 2.0f);
            ProcessSingleBatch(*builder, i, batch++, start, end, validStart, validEnd, prevSdf, prevShader);
            if (end == totalCurves) break;
            start += step;
        }
    }
}

std::shared_ptr<Image> GESDFPathShaderShape::DrawPathToImage(
    Drawing::Canvas& canvas, int width, int height, const Drawing::Path& path)
{
    if (width <= 0 || height <= 0) {
        LOGE("GESDFPathShaderShape::DrawPathToImage error, width or height is invalid");
        return nullptr;
    }
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::DrawPathToImage");
    auto canvasSurface = canvas.GetSurface();
    auto colorSpace = canvasSurface ? canvasSurface->GetImageInfo().GetColorSpace() : nullptr;
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo { width, height, Drawing::ColorType::COLORTYPE_RGBA_F16,
        Drawing::AlphaType::ALPHATYPE_PREMUL, colorSpace };
    std::shared_ptr<Drawing::Surface> surface =
        Drawing::Surface::MakeRenderTarget(canvas.GetGPUContext().get(), false, imageInfo);
    if (!surface) {
        LOGE("Null surface");
        return nullptr;
    }
    auto canvas1 = surface->GetCanvas();
    if (canvas1 == nullptr) {
        return nullptr;
    }
    Drawing::Brush brush;
    brush.SetColor(Color::COLOR_RED);
    brush.SetAntiAlias(true);
    canvas1->AttachBrush(brush);
    canvas1->DrawPath(path);
    canvas1->DetachBrush();
    return surface->GetImageSnapshot();
}

void GESDFPathShaderShape::cubicToQuadraticSingle(
    const Vector2f& p0, Vector2f& p1, const Vector2f& p2, const Vector2f& p3)
{
    // Keep endpoint unchanged
    Vector2f q0 = p0;
    Vector2f q2 = p3;

    // Bezier curve coefficients for cubic to quadratic conversion
    const double c0 = 0.125;
    const double c1 = 0.375;
    const double c2 = 0.375;
    const double c3 = 0.125;
    Vector2f midCubic = { c0 * p0.x_ + c1 * p1.x_ + c2 * p2.x_ + c3 * p3.x_,
        c0 * p0.y_ + c1 * p1.y_ + c2 * p2.y_ + c3 * p3.y_ };

    const float weightDouble = 2.0f;
    const float weightHalf = 0.5f;
    Vector2f q1 = { weightDouble * midCubic.x_ - weightHalf * q0.x_ - weightHalf * q2.x_,
        weightDouble * midCubic.y_ - weightHalf * q0.y_ - weightHalf * q2.y_ };
    p1 = q1;
}

void GESDFPathShaderShape::UpdateScale(Vector2f& scale, const Drawing::Rect& rect)
{
    float originalW = rect.GetWidth();
    float originalH = rect.GetHeight();
    float originalMinWH = std::min(originalW, originalH);
    if (originalMinWH < MIN_SCALE) {
        scale = Vector2f(1.0f, 1.0f); // 1.0f: maximum scale
        return;
    }
    float scaleX = std::clamp(scale.x_, MIN_SCALE_CLAMP, 1.0f); // 1.0f: maximum scale
    float scaleY = std::clamp(scale.y_, MIN_SCALE_CLAMP, 1.0f); // 1.0f: maximum scale
    float width = originalW * scaleX;
    float height = originalH * scaleY;
    float minWH = std::min(width, height);
    if (minWH < MIN_SCALE) {
        scaleX = MIN_SCALE / originalMinWH;
        scaleY = scaleX;
        width = originalW * scaleX;
        height = originalH * scaleY;
    }
    if (height > (width * 3.0f)) { // 3.0f: Large aspect ratio difference
        scaleY *= (width * 3.0f) / height; // 3.0f: Reduce aspect ratio differences
    }

    scaleX = std::max(scaleX, MIN_SCALE_CLAMP);
    scaleY = std::max(scaleY, MIN_SCALE_CLAMP);
    scale = Vector2f(scaleX, scaleY);
}

void GESDFPathShaderShape::UpdateNumPasses(float height)
{
    if (height < 150.f) { // 150.0:Add iterative correction When the component is small
        numPasses_ = 1;
        return;
    }
    if (allGridsCovered_) {
        numPasses_ = 0; // perform 0 iterations If all the grids have curves passing
        return;
    }
    float requiredStep = maxEmptyGridShortSide_ * 0.5f;
    if (requiredStep < 1.0f) {
        requiredStep = 1.0f;
    }
    requiredStep = std::clamp(requiredStep, 1.0f, 32.0f); // 32.0:max step for JFA
    int passCount = static_cast<int>(std::floor(std::log2(requiredStep))) + 1;
    numPasses_ = static_cast<size_t>(std::clamp(passCount, 1, 6)); // at least 1 times, at most 6 times
}

void GESDFPathShaderShape::Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal)
{
    GE_TRACE_FUNC();
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::Preprocess");
    if (!params_.path.IsValid()) {
        LOGE("GESDFPathShaderShape::Preprocess: the paths is invalid.");
        return;
    }
    float width = 0.0f;
    float height = 0.0f;
    Drawing::Path path = PreparePathForRendering(rect, width, height);

    std::vector<std::vector<Vector2f>> paramsCoef = GetCurveByPath(path);
    std::vector<Vector2f> pixelControlPoints = ProcessCurveSegments(paramsCoef);
    if (pixelControlPoints.empty()) {
        LOGE("GESDFPathShaderShape::Preprocess: no valid curves.");
        return;
    }
    // Convert Pixel To NDC
    std::vector<Vector2f> ndcControlPoints = ConvertPixelToNDC(pixelControlPoints, width, height);
    ConvertPointsToFloats(ndcControlPoints, controlPoints_);
    pointCnt_ = controlPoints_.size();

    AutoGridPartition(width, height, pixelControlPoints);
    if (curvesInGrid_.empty()) {
        LOGE("GESDFPathShaderShape::Preprocess: AutoGridPartition failed.");
        return;
    }

    Drawing::Rect targetRect(0, 0, width, height);
    CreateSurfaceAndCanvas(canvas, targetRect);
    if (!offscreenSurface_ || !offscreenCanvas_) {
        LOGE("GESDFPathShaderShape::PrePass create surface failed");
        return;
    }

    RenderGridsToSurface(targetRect);

    UpdateNumPasses(height);
    std::shared_ptr<Image> pathImage = nullptr;
    if (numPasses_ > 0) {
        pathImage = DrawPathToImage(canvas, static_cast<int>(width), static_cast<int>(height), path);
        if (pathImage == nullptr) {
            LOGE("GESDFPathShaderShape::Preprocess: Failed to draw the paths");
            return;
        }
    }
    
    std::shared_ptr<Image> propagatedSdf = RunSDFPropagation(canvas, offscreenSurface_->GetImageSnapshot(),
        pathImage, static_cast<int>(width), static_cast<int>(height));

    disResult_ =
        ComputeDistanceField(canvas, propagatedSdf, static_cast<int>(width), static_cast<int>(height));
    if (!disResult_) {
        LOGE("GESDFPathShaderShape::Preprocess ComputeDistanceField failed");
        return;
    }
}

std::shared_ptr<ShaderEffect> GESDFPathShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFPathShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    if (!disResult_) {
        LOGE("GESDFPathShaderShape::GenerateDrawingShader no sdfResult_, call PrePass first");
        return nullptr;
    }
    auto inputImageInfo = disResult_->GetImageInfo();
    float inputHeight = inputImageInfo.GetHeight();
    float inputWidth = inputImageInfo.GetWidth();
    // upScale to target resolution
    // check for division by zero
    if (inputWidth < 1.0f || inputHeight < 1.0f) { // 1.0f is minimum pixel size
        LOGE("GESDFPathShaderShape::GenerateDrawingShader invalid input size");
        return nullptr;
    }
    float scaleX = width / inputWidth;
    float scaleY = height / inputHeight;
    Drawing::Matrix upscaleMatrix;
    upscaleMatrix.SetScale(scaleX, scaleY);
    SamplingOptions sampling(FilterMode::LINEAR, MipmapMode::LINEAR);
    return ShaderEffect::CreateImageShader(*disResult_, TileMode::CLAMP, TileMode::CLAMP, sampling, upscaleMatrix);
}

std::shared_ptr<ShaderEffect> GESDFPathShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    return GenerateDrawingShader(width, height);
}

void GESDFPathShaderShape::ClearTemp()
{
    disResult_ = nullptr;
    offscreenSurface_ = nullptr;
    offscreenCanvas_ = nullptr;
}

std::shared_ptr<ShaderEffect> GESDFPathShaderShape::GenerateDrawingShader(Canvas& canvas, float width, float height)
{
    Preprocess(canvas, Rect(0.0, 0.0, width, height), false);
    auto out = GenerateDrawingShader(width, height);
    ClearTemp(); // delete member variables that are no longer used.
    return out;
}

std::shared_ptr<ShaderEffect> GESDFPathShaderShape::GenerateDrawingShaderHasNormal(Canvas& canvas,
    float width, float height)
{
    Preprocess(canvas, Rect(0.0, 0.0, width, height), true);
    auto out = GenerateDrawingShaderHasNormal(width, height);
    ClearTemp(); // delete member variables that are no longer used.
    return out;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
