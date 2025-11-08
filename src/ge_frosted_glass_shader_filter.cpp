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
#include "ge_frosted_glass_shader_filter.h"

#include "ge_log.h"
#include "ge_mesa_blur_shader_filter.h"

namespace OHOS {
namespace Rosen {
constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
static constexpr char MAIN_SHADER_PROG[] = R"(
    uniform shader imageShader;
    uniform shader blurredImage;
    uniform vec2 iResolution;

    uniform vec2 halfsize;
    uniform float cornerRadius;
    uniform float borderWidth;
    uniform vec2 direction;  // 对角高光
    uniform float offset;
    uniform float embossBlurRadius;
    uniform float embossSampleNum;
    uniform float refractOutPx; // 向外（凸）取样位移（全分辨率像素）
    uniform float refractInPx; // 向内（凹）取样位移（全分辨率像素）
    uniform loat antiAliasing;

    // --------- Darken and vibrate blurred image parameters ---------
    uniform float BG_FACTOR; // Darken
    uniform float envK; // 环境光（边缘提亮）
    uniform float envB;
    uniform float envS;
    uniform float sdK; // 内阴影（边缘压暗后再做色彩重映射）
    uniform float sdB;
    uniform float sdS;
    uniform float hlK; // 对角高光（描边色）
    uniform float hlB;
    uniform float hlS;

    // --------- Inner edge shadow parameters -----------------------
    uniform float shadowRefractRate;
    uniform float innerShadowExp;  //4.62
    uniform float shadowWidth;
    uniform float shadowStrength;

    // ---------- Edge diagonal highlight parameters ----------------
    uniform float highLightAngleDeg;    // 扇形角宽（度）。越大覆盖越宽
    uniform float highLightFeatherDeg;  // 扇形边缘羽化宽度（度）
    uniform float highLightWidthPx;      // 对角高光带宽（像素，沿法线向内）
    uniform float highLightFeatherPx;    // 对角高光内外两侧的羽化（像素）
    uniform float highLightShiftPx;  // 对角高光相对外边界向内偏移（像素），>0 往内移，<0 可往外移

    // Common constants
    uniform float aaWidth;
    const float PI = 3.14159265358979323846;
    const float EPSILON = 0.0001;
    const float N_EPS = 1e-6;
    const float downSampleFactor = 2.0;

    // ================= Subfunctions =====================
    float DiagonalFanMask(vec2 posFromCenter, vec2 dir, float angleDeg, float featherDeg)
    {
        vec2 p = normalize(posFromCenter);
        vec2 d = normalize(dir);

        float angle = angleDeg * (PI / 180.0);
        float feather = max(1e-4, featherDeg * (PI / 180.0));

        float c1 = clamp(dot(d, p), -1.0, 1.0);
        float c2 = clamp(dot(-d, p), -1.0, 1.0);

        float theta1 = acos(c1);
        float theta2 = acos(c2);

        float lobe1 = 1.0 - smoothstep(angle * 0.5, angle * 0.5 + feather, theta1);
        float lobe2 = 1.0 - smoothstep(angle * 0.5, angle * 0.5 + feather, theta2);

        return clamp(lobe1 + lobe2, 0.0, 1.0);
    }

    float SdfRRect(vec2 p, vec2 b, float r)
    {
        vec2 q = abs(p) - b + r;
        return length(max(q, 0.0)) - r;
    }

    // ========= grad for Rounded-Rect ==============
    vec2 GradRRect(vec2 p, vec2 b, float r)
    {
        float rr = min(r, min(b.x, b.y));
        vec2 w = abs(p) - b - vec2(rr);
        float g = max(w.x, w.y);
        vec2 q = max(w, 0.0);
        float l = length(q);

        vec2 grad;
        if (g > 0.0) {
            grad = (l > N_EPS) ? (q / l) : vec2(1.0, 0.0);
        } else {
            grad = (w.x > w.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
        }
        grad *= sign(p);
        return grad;
    }

    vec4 SampleUV(vec2 uv, vec2 res)
    {
        return iImage1.eval(uv + 0.5 * res);
    }

    int IntMin(int a, int b)
    {
        return (a <= b) ? a : b;
    }

    vec4 BaseBlur(vec2 coord)
    {
        return blurredImage.eval(coord);
    }

    vec3 Sat(vec3 src255, float n, float p1r, float p2r, float p1g, float p2g, float p1b, float p2b)
    {
        float r = src255.r;
        float g = src255.g;
        float b = src255.b;
        float rn = r * (0.2412016 * (1. - n) + n) + g * (0.6922296 * (1. - n)) + b * (0.0665688 * (1. - n));
        float gn = r * (0.2412016 * (1. - n)) + g * (0.6922296 * (1. - n) + n) + b * (0.0665688 * (1. - n));
        float bn = r * (0.2412016 * (1. - n)) + g * (0.6922296 * (1. - n)) + b * (0.0665688 * (1. - n) + n);

        float dr = rn - r;
        float grt = step(0., dr);
        float rr = (r + dr * p1r) * grt + (r + dr * p2r) * (1. - grt);

        float dg = gn - g;
        grt = step(0., dg);
        float gg = (g + dg * p1g) * grt + (g + dg * p2g) * (1. - grt);

        float db = bn - b;
        grt = step(0., db);
        float bb = (b + db * p1b) * grt + (b + db * p2b) * (1. - grt);

        return vec3(rr, gg, bb);
    }

    vec3 BlurVibrancy(vec3 c01)
    {
        vec3 x = c01 * 255.0;
        x = -0.0000289 * pow(x, vec3(3.)) + 0.0108341 * pow(x, vec3(2.)) + 0.0073494 * x + 25.4709106;
        x = Sat(x, 1.2, 0.3, 0.5, 0.5, 0.5, 1., 1.);
        return clamp(x / 255.0, 0.0, 1.0);
    }

    vec3 ApplyKBS(vec3 c01, float K, float B, float S, float p1r, float p2r, float p1g, float p2g,
                  float p1b, float p2b)
    {
        vec3 x = c01 * 255.0;
        x = x * K + vec3(B);
        x = Sat(x, S, p1r, p2r, p1g, p2g, p1b, p2b);
        return clamp(x / 255.0, 0.0, 1.0);
    }

    vec3 EdgeLightVibrancy(vec3 c01)
    {
        return ApplyKBS(c01, envK, envB, envS, 1.0, 1.7, 1.5, 3.0, 2.0, 1.0);
    }

    vec3 InnerShadowVibrancy(vec3 c01)
    {
        return ApplyKBS(c01, sdK, sdB, sdS, 1.0, 1.7, 1.5, 3.0, 2.0, 1.0);
    }

    vec3 EdgeHighlightVibrancy(vec3 c01)
    {
        return ApplyKBS(c01, hlK, hlB, hlS, 1.0, 1.7, 1.5, 3.0, 2.0, 1.0);
    }

    vec3 CompareBlend(vec3 src, vec3 dst)
    {
        const vec3 grayBase = vec3(0.2412016, 0.6922296, 0.0665688);
        float lb = dot(src, grayBase);  // src.r * 0.2412016 + src.g * 0.6922296 + src.b * 0.0665688;
        float le = dot(dst, grayBase);  // dst.r * 0.2412016 + dst.g * 0.6922296 + dst.b * 0.0665688;
        return mix(src, dst, le / max((lb + le), EPSILON));
    }

    float EdgeExpInAA(vec2 p, vec2 b, float r, float decayLenPx, float aaPx)
    {
        float sd = SdfRRect(p, b, r);
        float x = max(0.0, -sd);                        // 内部深度
        float core = exp(-x / max(decayLenPx, N_EPS));  // 指数衰减核
        float cover = smoothstep(aaPx, -aaPx, sd);      // 边界 AA 覆盖度（外0→内1）
        return core * cover;
    }

    float EdgeBandAA(float sd, float widthPx, float featherPx, float shiftPx)
    {
        float a = max(featherPx, N_EPS);
        float coverOuter = smoothstep(a, -a, sd + shiftPx);
        float coverInner = smoothstep(a, -a, sd + shiftPx + max(widthPx, 0.0));
        return clamp(coverOuter - coverInner, 0.0, 1.0);
    }

    // UX Functions
    // Compute refractive offset from SDF using screen-space gradients
    float dF(vec3 sdf)
    {
        return ((sdf.y - sdf.x) / sdf.z);
    }

    vec2 ComputeRefractOffset(vec3 sdfx, vec3 sdfy, float rate)
    {
        vec2 grad = normalize(vec2(dF(sdfx), dF(sdfy)));
        return grad * rate;
    }

    vec4 AlphaBlend(in vec4 c1, in vec4 c2)
    {
        return vec4(c1.rgb * (1. - c2.a) + c2.rgb * c2.a, 1.);
    }

    vec4 main(vec2 fragCoord)
    {
        vec4 baseTex = iImage1.eval(fragCoord) * BG_FACTOR;  // layer0: background image

        vec2 uv = (fragCoord + fragCoord - iResolution.xy) * 0.5;
        vec2 centerPos = iMouse.xy - iResolution.xy * 0.5;  // vec2(0.0);

        float sd = SdfRRect(uv - centerPos, halfsize, cornerRadius);
        vec2 sdGrad = GradRRect(uv - centerPos, halfsize, cornerRadius);
        float sdBlack = SdfRRect(uv - centerPos, halfsize - offset, cornerRadius - offset);

        float border =
            smoothstep(-1.0 + antiAliasing, max(1.0, borderWidth * antiAliasing * 0.5), -sd * antiAliasing) -
            smoothstep(min(-borderWidth * antiAliasing * 0.5, -1.), 1.0 - antiAliasing, (-sd - borderWidth) *
            antiAliasing);
        float borderBlack =
            smoothstep(-1.0 + antiAliasing, max(1.0, borderWidth * antiAliasing * 0.5), -sdBlack * 
                        antiAliasing) -
            smoothstep(min(-borderWidth * antiAliasing * 0.5, -1.), 1.0 - antiAliasing,
                    (-sdBlack - borderWidth) * antiAliasing);
        float embossPos = (border - borderBlack + 1.0) * 0.5 * clamp(border + borderBlack, 0.0, 1.0);
        float embossNeg = (-border + borderBlack + 1.0) * 0.5 * clamp(border + borderBlack, 0.0, 1.0);
        vec2 offsetUV = uv - centerPos;

        // emboss with refraction
        vec4 refractionPos = vec4(0.0);
        vec4 refractionNeg = vec4(0.0);
        vec4 blurredBgColor;

        // sample blurred image
        vec4 blurSample = BaseBlur(fragCoord) * BG_FACTOR;

        // Light up
        blurredBgColor.rgb = BlurVibrancy(blurSample.rgb);

        // Edge inner shadow
        vec2 biasUV0 = fragCoord + normalize(sdGrad) * shadowRefractRate;
        refractionNeg.rgb = blurredImage.eval(biasUV0).rgb * BG_FACTOR;
        refractionNeg.rgb = BlurVibrancy(refractionNeg.rgb);
        refractionNeg.rgb = CompareBlend(blurSample.rgb, refractionNeg.rgb);
        vec3 shadowEdge = InnerShadowVibrancy(refractionNeg.rgb).rgb;
        float edge0 = clamp(exp(innerShadowExp * (sd - 1.0 + shadowWidth)), 0.0, 1.0);
        blurredBgColor.rgb = mix(blurredBgColor.rgb, shadowEdge, edge0 * shadowStrength);

        // Edge Ambient Light Effect
        refractionPos.rgb = CompareBlend(blurredBgColor.rgb, refractionPos.rgb);
        refractionPos.rgb = EdgeLightVibrancy(refractionPos.rgb);
        blurredBgColor = mix(blurredBgColor, refractionPos, clamp(embossPos, 0.0, 1.0));

        // Edge highlight, diagonal
        float widthClamped = min(highLightWidthPx, max(borderWidth, 0.0));
        float edgeBand = EdgeBandAA(sd, widthClamped, highLightFeatherPx, highLightShiftPx);
        float diagMask = DiagonalFanMask(offsetUV, normalize(direction), highLightAngleDeg,
                                            highLightFeatherDeg);
        float edge = edgeBand * diagMask;
        vec3 hlBase = EdgeHighlightVibrancy(blurredBgColor.rgb);
        blurredBgColor = mix(blurredBgColor, vec4(hlBase, 1.0), edge);

        float boxMask = 1.0 - clamp(sd, 0.0, 1.0);
        vec4 l1 = vec4(blurredBgColor.rgb, boxMask);
        return AlphaBlend(baseTex, l1);
    }
)";

// for init shader effect only once.
// thread_local for thread safety and freeing variables.
thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_frostedGlassShaderEffect = nullptr;

GEFrostedGlassShaderFilter::GEFrostedGlassShaderFilter(const Drawing::GEFrostedGlassShaderFilterParams& params)
{
    frostedGlassParams_ = params;
}

std::shared_ptr<Drawing::Image> GEFrostedGlassShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEFrostedGlassShaderFilter::OnProcessImage input is invalid");
        return nullptr;
    }

    int imageHeight = image->GetHeight();
    int imageWidth = image->GetWidth();
    if (imageHeight < 1e-6 || imageWidth < 1e-6) {
        LOGE("GEFrostedGlassShaderFilter::OnProcessImage imageinfo is invalid");
        return nullptr;
    }

    Drawing::Matrix matrix = canvasInfo_.mat;
    matrix.PostTranslate(-canvasInfo_.tranX, -canvasInfo_.tranY);
    Drawing::Matrix invertMatrix;
    if (!matrix.Invert(invertMatrix)) {
        LOGE("GEFrostedGlassShaderFilter::OnProcessImage Invert matrix failed");
        return image;
    }

    // image to shader
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertMatrix);
    if (shader == nullptr) {
        LOGE("GEFrostedGlassShaderFilter::create shader failed.");
        return nullptr;
    }

    // base blur image to shader
    auto baseBlurImg = MakeBaseBlurImg(canvas, src, dst, image);
    auto baseBlurShader = Drawing::ShaderEffect::CreateImageShader(*baseBlurImg, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), invertMatrix);
    if (baseBlurShader == nullptr) {
        LOGE("GEFrostedGlassShaderFilter::create baseBlurShader failed.");
        return nullptr;
    }

    // init shader and set uniform
    // TODO: get edge blur image
    auto builder = MakeFrostedGlassShader(shader, baseBlurShader, canvasInfo_.geoWidth, canvasInfo_.geoHeight);
    if (builder == nullptr) {
        LOGE("GEFrostedGlassShaderFilter::OnProcessImage builder is null");
        return image;
    }
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), &matrix, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEFrostedGlassShaderFilter::OnProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

std::shared_ptr<Drawing::Image> GEFrostedGlassShaderFilter::MakeBaseBlurImg(Drawing::Canvas& canvas,
    const Drawing::Rect& src, const Drawing::Rect& dst, std::shared_ptr<Drawing::Image> image)
{
    Drawing::GEMESABlurShaderFilterParams blurImgParas{};
    blurImgParas.radius = frostedGlassParams_.blurRadius;
    blurShader_ = std::make_shared<GEMESABlurShaderFilter>(blurImgParas);
    return blurShader_->OnProcessImage(canvas, image, src, dst);
}

bool GEFrostedGlassShaderFilter::InitFrostedGlassEffect()
{
    if (g_frostedGlassShaderEffect == nullptr) {
        g_frostedGlassShaderEffect = Drawing::RuntimeEffect::CreateForShader(MAIN_SHADER_PROG);
        if (g_frostedGlassShaderEffect == nullptr) {
            LOGE("InitFrostedGlassEffect::RuntimeShader effect error\n");
            return false;
        }
    }
    return true;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEFrostedGlassShaderFilter::MakeFrostedGlassShader(
    std::shared_ptr<Drawing::ShaderEffect> imageShader, std::shared_ptr<Drawing::ShaderEffect> baseBlurShader,
    float imageWidth, float imageHeight)
{
    if (g_frostedGlassShaderEffect == nullptr) {
        if (!InitFrostedGlassEffect()) {
            LOGE("GEFrostedGlassShaderFilter::failed when initializing MagnifierEffect.");
            return nullptr;
        }
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(g_frostedGlassShaderEffect);

    builder->SetChild("imageShader", imageShader);
    builder->SetChild("baseBlurImage", baseBlurShader);
    builder->SetUniform("iResolution", imageWidth, imageHeight);
    builder->SetUniform("halfsize", frostedGlassParams_.borderSize[NUM_0], frostedGlassParams_.borderSize[NUM_1]);
    builder->SetUniform("cornerRadius", frostedGlassParams_.cornerRadius);
    builder->SetUniform("borderWidth", frostedGlassParams_.borderWidth);
    builder->SetUniform("direction", frostedGlassParams_.direction);  // 对角高光
    builder->SetUniform("offset", frostedGlassParams_.offset);
    builder->SetUniform("embossBlurRadius", frostedGlassParams_.embossBlurRadius);
    builder->SetUniform("embossSampleNum", frostedGlassParams_.embossSampleNum);
    builder->SetUniform("refractOutPx", frostedGlassParams_.refractOutPx); // 向外（凸）取样位移（全分辨率像素）
    builder->SetUniform("refractInPx", frostedGlassParams_.refractInPx); // 向内（凹）取样位移（全分辨率像素）
    builder->SetUniform("antiAliasing", frostedGlassParams_.antiAliasing);

    // --------- Darken and vibrate blurred image parameters ---------
    builder->SetUniform("BG_FACTOR", frostedGlassParams_.BG_FACTOR); // Darken
    builder->SetUniform("envK", frostedGlassParams_.envParams[0]); // 环境光（边缘提亮）
    builder->SetUniform("envB", frostedGlassParams_.envParams[1]);
    builder->SetUniform("envS", frostedGlassParams_.envParams[2]);
    builder->SetUniform("sdK", frostedGlassParams_.sdParams[0]); // 内阴影（边缘压暗后再做色彩重映射）
    builder->SetUniform("sdB", frostedGlassParams_.sdParams[1]);
    builder->SetUniform("sdS", frostedGlassParams_.sdParams[2]);
    builder->SetUniform("hlK", frostedGlassParams_.hlParams[0]); // 对角高光（描边色）
    builder->SetUniform("hlB", frostedGlassParams_.hlParams[1]);
    builder->SetUniform("hlS", frostedGlassParams_.hlParams[2]);

    // --------- Inner edge shadow parameters -----------------------
    builder->SetUniform("shadowRefractRate", frostedGlassParams_.innerShadowParams[0]);
    builder->SetUniform("innerShadowExp", frostedGlassParams_.innerShadowParams[1]);  //4.62
    builder->SetUniform("shadowWidth", frostedGlassParams_.innerShadowParams[2]);
    builder->SetUniform("shadowStrength", frostedGlassParams_.innerShadowParams[3]);

    // ---------- Edge diagonal highlight parameters ----------------
    builder->SetUniform("highLightAngleDeg", frostedGlassParams_.highLightParams[0]);    // 扇形角宽（度）。越大覆盖越宽
    builder->SetUniform("highLightFeatherDeg", frostedGlassParams_.highLightParams[1]);  // 扇形边缘羽化宽度（度）
    builder->SetUniform("highLightWidthPx", frostedGlassParams_.highLightParams[2]);      // 对角高光带宽（像素，沿法线向内）
    builder->SetUniform("highLightFeatherPx",  frostedGlassParams_.highLightParams[3]);    // 对角高光内外两侧的羽化（像素）
    builder->SetUniform("highLightShiftPx", frostedGlassParams_.highLightParams[4]);  // 对角高光相对外边界向内偏移（像素），>0 往内移，<0 可往外移

    // Common constants
    return builder;
}
} // namespace Rosen
} // namespace OHOS