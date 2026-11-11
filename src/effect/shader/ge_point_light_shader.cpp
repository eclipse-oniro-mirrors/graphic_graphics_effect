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

#include "ge_point_light_shader.h"

#include <algorithm>
#include <cmath>

#include "draw/brush.h"
#include "draw/pen.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"
#include "ge_trace.h"

#undef LOG_TAG
#define LOG_TAG "GEPointLightShader"
namespace OHOS {
namespace Rosen {
namespace {
constexpr int PARAM_TWO = 2;
constexpr float DITHER_ALPHA_THRESHOLD = 0.15f;

constexpr char NORMAL_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float cornerRadius;
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float roundCurvature;
    uniform float bumpFactor;
    uniform float gradientRadius;
    uniform float bulgeRadius;
    uniform float attenuationCoeff;
    uniform float eps;

    float sdf_g2_rounded_box(vec2 p, vec2 b, float r, float k)
    {
        vec2 q = abs(p) - b;
        float v_x =  max(q.x, 0.0);
        float v_y =  max(q.y, 0.0);
        float dist_corner_shape;
        if (abs(k - 2.0) < 0.001) {
            dist_corner_shape = length(vec2(v_x, v_y));
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        } else {
            if (v_x == 0.0 && v_y == 0.0) {
                dist_corner_shape = 0.0;
            } else {
                dist_corner_shape = pow(pow(v_x, k) + pow(v_y, k), 1.0 / k);
            }
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        }
    }

    float sdScene(vec2 fragCoord)
    {
        vec2 boxSize = iResolution.xy * 0.5 - cornerRadius;
        vec2 boxCenter = iResolution.xy * 0.5;
        vec2 p = fragCoord - boxCenter;
        return sdf_g2_rounded_box(p, boxSize, cornerRadius, roundCurvature);
    }

    float height(vec2 p, float radius)
    {
        float d = sdScene(p);
        float h = 0.0;
        if (d < 0.0 && d > -radius) {
            float arg = 2.0 * radius * abs(d) - d * d;
            h = sqrt(max(arg, 0.0));
        } else if (d <= -radius) {
            h = radius;
        }
        return h;
    }

    vec2 computeGradient(vec2 p) {
        vec2 epsVec = vec2(eps, 0.0);
        float dx = height(p - epsVec.xy, gradientRadius) - height(p + epsVec.xy, gradientRadius);
        float dy = height(p - epsVec.yx, gradientRadius) - height(p + epsVec.yx, gradientRadius);
        return vec2(dx, dy);
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 grad = computeGradient(drawing_coord);
        float h = height(drawing_coord, bulgeRadius);
        vec3 norm = normalize(vec3(grad * bumpFactor, h));

        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec3 lightVec = vec3(lightPos[i].xy - drawing_coord, lightPos[i].z);
                vec3 lightDir = normalize(lightVec);
                vec3 viewDir = normalize(lightVec);
                vec3 halfwayDir = normalize(lightDir + viewDir);

                float normalizedDistance = length(lightVec) / lightPos[i].w;
                float smoothFactor = smoothstep(0.8, 1.0, normalizedDistance);
                float attenuation = (1.0 - smoothFactor) / (1.0 + attenuationCoeff *
                    normalizedDistance * normalizedDistance);
                float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
                vec4 specularColor = specularLightColor[i];
                fragColor += specularColor * spec * specularStrength[i] * attenuation;
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");

constexpr char NORMAL_SINGLE_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float cornerRadius;
    uniform vec4 lightPos[1];
    uniform vec4 viewPos[1];
    uniform vec4 specularLightColor[1];
    uniform float specularStrength[1];
    uniform float roundCurvature;
    uniform float bumpFactor;
    uniform float gradientRadius;
    uniform float bulgeRadius;
    uniform float attenuationCoeff;
    uniform float eps;

    float sdf_g2_rounded_box(vec2 p, vec2 b, float r, float k)
    {
        vec2 q = abs(p) - b;
        float v_x =  max(q.x, 0.0);
        float v_y =  max(q.y, 0.0);
        float dist_corner_shape;
        if (abs(k - 2.0) < 0.001) {
            dist_corner_shape = length(vec2(v_x, v_y));
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        } else {
            if (v_x == 0.0 && v_y == 0.0) {
                dist_corner_shape = 0.0;
            } else {
                dist_corner_shape = pow(pow(v_x, k) + pow(v_y, k), 1.0 / k);
            }
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        }
    }

    float sdScene(vec2 fragCoord)
    {
        vec2 boxSize = iResolution.xy * 0.5 - cornerRadius;
        vec2 boxCenter = iResolution.xy * 0.5;
        vec2 p = fragCoord - boxCenter;
        return sdf_g2_rounded_box(p, boxSize, cornerRadius, roundCurvature);
    }

    float height(vec2 p, float radius)
    {
        float d = sdScene(p);
        float h = 0.0;
        if (d < 0.0 && d > -radius) {
            float arg = 2.0 * radius * abs(d) - d * d;
            h = sqrt(max(arg, 0.0));
        } else if (d <= -radius) {
            h = radius;
        }
        return h;
    }

    vec2 computeGradient(vec2 p) {
        vec2 epsVec = vec2(eps, 0.0);
        float dx = height(p - epsVec.xy, gradientRadius) - height(p + epsVec.xy, gradientRadius);
        float dy = height(p - epsVec.yx, gradientRadius) - height(p + epsVec.yx, gradientRadius);
        return vec2(dx, dy);
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 grad = computeGradient(drawing_coord);
        float h = height(drawing_coord, bulgeRadius);
        vec3 norm = normalize(vec3(grad * bumpFactor, h));

        if (abs(specularStrength[0]) > 0.01) {
            vec3 lightVec = vec3(lightPos[0].xy - drawing_coord, lightPos[0].z);
            vec3 lightDir = normalize(lightVec);
            vec3 viewDir = normalize(lightVec);
            vec3 halfwayDir = normalize(lightDir + viewDir);

            float normalizedDistance = length(lightVec) / lightPos[0].w;
            float smoothFactor = smoothstep(0.8, 1.0, normalizedDistance);
            float attenuation = (1.0 - smoothFactor) / (1.0 + attenuationCoeff *
                normalizedDistance * normalizedDistance);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
            vec4 specularColor = specularLightColor[0];
            fragColor += specularColor * spec * specularStrength[0] * attenuation;
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");

static constexpr char FEATHERING_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float contentBorderRadius;
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float borderWidth;

    float sdRoundedBox(vec2 p, vec2 b, float r)
    {
        vec2 q = abs(p) - b + r;
        return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
    }

    vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
    {
        vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
        vec2 q = abs(p) - b + r;
        vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
        nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
        return signs * nor;
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 halfResolution = iResolution.xy * 0.5;

        float gradRadius = min(max(contentBorderRadius, abs(borderWidth) * 3.0), iResolution.y * 0.5);
        float drawRadius = min(max(contentBorderRadius, abs(borderWidth) * 1.1), iResolution.y * 0.5);
        float realRoundedBoxSDF =
            sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius);
        float virtualRoundedBoxSDF = sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, drawRadius);
        vec2 grad = sdRoundedBoxGradient(drawing_coord.xy - halfResolution, halfResolution, gradRadius);
        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec2 lightGrad = sdRoundedBoxGradient(lightPos[i].xy - halfResolution,
                    halfResolution,
                    contentBorderRadius);
                float angleEfficient = dot(grad, lightGrad);
                if (angleEfficient > 0.0) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z));
                    vec3 halfwayDir = normalize(lightDir + viewDir);
                    float spec = pow(max(halfwayDir.z, 0.0), shininess);
                    spec *= specularStrength[i];
                    spec *= smoothstep(-borderWidth, 0.0, virtualRoundedBoxSDF);
                    spec *= (smoothstep(1.0, 0.0, spec) * 0.2 + 0.8);
                    vec4 specularColor = specularLightColor[i];
                    fragColor += specularColor * (spec * angleEfficient);
                }
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");

static constexpr char SINGLE_FEATHERING_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float contentBorderRadius;
    uniform vec4 lightPos[1];
    uniform vec4 viewPos[1];
    uniform vec4 specularLightColor[1];
    uniform float specularStrength[1];
    uniform float borderWidth;

    float sdRoundedBox(vec2 p, vec2 b, float r)
    {
        vec2 q = abs(p) - b + r;
        return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
    }

    vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
    {
        vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
        vec2 q = abs(p) - b + r;
        vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
        nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
        return signs * nor;
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 halfResolution = iResolution.xy * 0.5;

        float gradRadius = min(max(contentBorderRadius, abs(borderWidth) * 3.0), iResolution.y * 0.5);
        float drawRadius = min(max(contentBorderRadius, abs(borderWidth) * 1.1), iResolution.y * 0.5);
        float realRoundedBoxSDF =
            sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius);
        float virtualRoundedBoxSDF = sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, drawRadius);
        vec2 grad = sdRoundedBoxGradient(drawing_coord.xy - halfResolution, halfResolution, gradRadius);
        if (abs(specularStrength[0]) > 0.01) {
            vec2 lightGrad = sdRoundedBoxGradient(lightPos[0].xy - halfResolution,
                halfResolution,
                contentBorderRadius);
            float angleEfficient = dot(grad, lightGrad);
            if (angleEfficient > 0.0) {
                vec3 lightDir = normalize(vec3(lightPos[0].xy - drawing_coord, lightPos[0].z));
                vec3 viewDir = normalize(vec3(viewPos[0].xy - drawing_coord, viewPos[0].z));
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(halfwayDir.z, 0.0), shininess);
                spec *= specularStrength[0];
                spec *= smoothstep(-borderWidth, 0.0, virtualRoundedBoxSDF);
                spec *= (smoothstep(1.0, 0.0, spec) * 0.2 + 0.8);
                vec4 specularColor = specularLightColor[0];
                fragColor += specularColor * (spec * angleEfficient);
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");

static constexpr char PHONG_SHADER_STRING[](R"(
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float enableDither;
    uniform float ditherAlphaThreshold;

    mediump vec4 main(vec2 drawing_coord) {
        vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
        float ambientStrength = 0.0;
        vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
        float diffuseStrength = 0.0;
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
        vec4 ambient = lightColor * ambientStrength;
        vec3 norm = normalize(NormalMap.rgb);

        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                float diff = max(dot(norm, lightDir), 0.0);
                vec4 diffuse = diff * lightColor;
                vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z));
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
                vec4 specular = lightColor * spec;
                vec4 o = ambient + diffuse * diffuseStrength * diffuseColor;
                vec4 specularColor = specularLightColor[i];
                fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
            }
        }
        if (enableDither > 0.5 && fragColor.a < ditherAlphaThreshold) {
            float ditherVal = fract(52.9829 * fract(dot(vec2(0.06711056, 0.00583715), floor(drawing_coord))));
            fragColor += (ditherVal - 0.5) / 255.0;
            fragColor.a = clamp(fragColor.a, 0.0, 1.0);
        }
        return fragColor;
    }
)");

static constexpr char SINGLE_PHONG_SHADER_STRING[](R"(
    uniform vec4 lightPos[1];
    uniform vec4 viewPos[1];
    uniform vec4 specularLightColor[1];
    uniform float specularStrength[1];
    uniform float enableDither;
    uniform float ditherAlphaThreshold;

    mediump vec4 main(vec2 drawing_coord) {
        vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
        float ambientStrength = 0.0;
        vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
        float diffuseStrength = 0.0;
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
        vec4 ambient = lightColor * ambientStrength;
        vec3 norm = normalize(NormalMap.rgb);

        if (abs(specularStrength[0]) > 0.01) {
            vec3 lightDir = normalize(vec3(lightPos[0].xy - drawing_coord, lightPos[0].z));
            float diff = max(dot(norm, lightDir), 0.0);
            vec4 diffuse = diff * lightColor;
            vec3 viewDir = normalize(vec3(viewPos[0].xy - drawing_coord, viewPos[0].z));
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
            vec4 specular = lightColor * spec;
            vec4 o = ambient + diffuse * diffuseStrength * diffuseColor;
            vec4 specularColor = specularLightColor[0];
            fragColor = fragColor + o + specular * specularStrength[0] * specularColor;
        }
        if (enableDither > 0.5 && fragColor.a < ditherAlphaThreshold) {
            float ditherVal = fract(52.9829 * fract(dot(vec2(0.06711056, 0.00583715), floor(drawing_coord))));
            fragColor += (ditherVal - 0.5) / 255.0;
            fragColor.a = clamp(fragColor.a, 0.0, 1.0);
        }
        return fragColor;
    }
)");

static constexpr char SDF_CONTENT_LIGHT_SHADER_STRING[](R"(
    uniform shader light;
    uniform shader sdf;

    mediump vec4 main(vec2 coord)
    {
        vec4 lightColor = light.eval(coord);
        float sdfColor = sdf.eval(coord).a;
        return lightColor * mix(1.0, 0.0, step(0.0, sdfColor)) * mix(1.0, -sdfColor, step(-1.0, sdfColor));
    }
)");

static constexpr char SDF_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform shader light;
    uniform shader sdf;
    uniform float borderWidth;

    mediump vec4 main(vec2 coord)
    {
        vec4 lightColor = light.eval(coord);
        float sdfColor = sdf.eval(coord).a;
        float halfBorderWidth = borderWidth * 0.5;
        sdfColor += min(halfBorderWidth, step(halfBorderWidth, -sdfColor));
        return lightColor * clamp((halfBorderWidth - abs(sdfColor + halfBorderWidth)), 0.0, 1.0);
    }
)");

static constexpr char SDF_CONTENT_AND_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform shader lightContent;
    uniform shader light;
    uniform shader sdf;
    uniform float borderWidth;
    mediump vec4 main(vec2 coord)
    {
        vec4 lightContentColor = lightContent.eval(coord);
        vec4 lightColor = light.eval(coord);
        float sdfColor = sdf.eval(coord).a;
        float halfBorderWidth = borderWidth * 0.5;

        vec4 firstColor =
            lightContentColor * mix(1.0, 0.0, step(0.0, sdfColor)) * mix(1.0, -sdfColor, step(-1.0, sdfColor));

        float borderSdfColor = sdfColor;
        borderSdfColor += min(halfBorderWidth, step(halfBorderWidth, -borderSdfColor));
        vec4 secondColor = lightColor * clamp((halfBorderWidth - abs(borderSdfColor + halfBorderWidth)), 0.0, 1.0);

        return firstColor + secondColor * (1.0 - firstColor.a);
    }
)");

Drawing::RuntimeEffectOptions g_highpUVOption{ .useHighpLocalCoords = true };

Drawing::RoundRect Convert(const Drawing::GERRect geRect)
{
    std::vector<Drawing::Point> radius;
    for (uint32_t i = 0; i < Drawing::GERRect::Corner::CORNER_COUNT; i++) {
        Drawing::Point p{geRect.radius_[i].x_, geRect.radius_[i].y_};
        radius.push_back(p);
    }
    return Drawing::RoundRect{
        Drawing::Rect{geRect.left_, geRect.top_, geRect.left_ + geRect.width_, geRect.top_ + geRect.height_}, radius};
}

GEPointLightShaderParamsUser Convert(const Drawing::GEPointLightShaderParams& in)
{
    GEPointLightShaderParamsUser out{};
    auto illuminatedType = static_cast<uint32_t>(in.illuminatedType);
    if (illuminatedType > static_cast<uint32_t>(IlluminatedType::NORMAL_BORDER_CONTENT)) {
        out.illuminatedType = IlluminatedType::INVALID;
    } else {
        out.illuminatedType = static_cast<IlluminatedType>(illuminatedType);
    }
    out.borderWidth = in.illuminatedBorderWidth;
    auto size_light = in.lightIntensities.size();
    if (in.lightColors.size() != size_light || in.lightPositions.size() != size_light) {
        GE_LOGE("DrawLight mismatch Size:%{public}zu, %{public}zu %{public}zu", size_light, in.lightColors.size(),
            in.lightPositions.size());
        return out;
    }
    for (size_t i = 0; i < size_light; i++) {
        GEPointLightSourceData lightData{in.lightPositions[i], in.lightIntensities[i], in.lightColors[i]};
        out.lightSources.push_back(lightData);
    }
    out.displayHeadroom = in.displayHeadroom;
    out.bloom = in.bloom;
    out.borderRRect = Convert(in.borderRRect);
    out.contentRRect = Convert(in.contentRRect);
    out.sdfDrawRect = Drawing::Rect{in.drawRect.x_, in.drawRect.y_, in.drawRect.z_, in.drawRect.w_};
    if (in.shape) {
        out.sdfShaderEffect = in.shape->GenerateDrawingShader(out.sdfDrawRect.GetWidth(), out.sdfDrawRect.GetHeight());
    }
    return out;
}
} // namespace

GEPointLightShader::GEPointLightShader(const Drawing::GEPointLightShaderParams& params)
    : params_(Convert(params))
{}

void GEPointLightShader::OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    DrawLight(canvas);
}

void GEPointLightShader::DrawLight(Drawing::Canvas& canvas) const
{
    GE_LOGD("DrawLight type:%{public}u lightSize:%{public}zu enableEDR:%{public}d",
        static_cast<uint32_t>(params_.illuminatedType), params_.lightSources.size(), params_.enableEDREffect);
    if (params_.lightSources.empty()) {
        return;
    }
    size_t lightLength = params_.lightSources.size();
    auto builder = CreateShaderBuilder(lightLength);
    if (!builder) {
        return;
    }
    builder->SetUniform("ditherAlphaThreshold", DITHER_ALPHA_THRESHOLD);
    builder->SetUniform("enableDither", params_.illuminatedType == IlluminatedType::CONTENT ? 1.0f : 0.0f);

    if (lightLength == 1) {
        std::array<float, 1> lightIntensityArray = { 0 };
        ProcessSingleLightSourcesData(lightIntensityArray, builder);
        GE_LOGD("DrawLight type:%{public}d intensity:%{public}f enableEDR:%{public}d",
            static_cast<uint32_t>(params_.illuminatedType), lightIntensityArray[0], params_.enableEDREffect);
        GE_TRACE_NAME_FMT("DrawLight intensity:%{public}f", lightIntensityArray[0]);
        DrawSingleLightByIlluminatedType(canvas, builder, lightIntensityArray);
    } else {
        std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 0 };
        ProcessLightSourcesData(lightIntensityArray, builder);
        GE_LOGD("DrawLight type:%{public}d intensity:%{public}f enableEDR:%{public}d",
            static_cast<uint32_t>(params_.illuminatedType), lightIntensityArray[0], params_.enableEDREffect);
        GE_TRACE_NAME_FMT("DrawLight intensity:%{public}f", lightIntensityArray[0]);
        DrawLightByIlluminatedType(canvas, builder, lightIntensityArray);
    }
}

void GEPointLightShader::DrawLightByIlluminatedType(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if ((params_.illuminatedType == IlluminatedType::BORDER_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT)) {
        DrawContentAndBorderLight(canvas, builder, brush, pen, lightIntensityArray);
    } else if ((params_.illuminatedType == IlluminatedType::CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_CONTENT)) {
        DrawContentLight(canvas, builder, brush, lightIntensityArray);
    } else if ((params_.illuminatedType == IlluminatedType::BORDER) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER) ||
        (params_.illuminatedType == IlluminatedType::FEATHERING_BORDER)) {
        DrawBorderLight(canvas, builder, pen, lightIntensityArray);
    }
}

void GEPointLightShader::DrawSingleLightByIlluminatedType(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
    const std::array<float, 1>& lightIntensityArray) const
{
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if ((params_.illuminatedType == IlluminatedType::BORDER_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT)) {
        DrawSingleContentAndBorderLight(canvas, builder, brush, pen, lightIntensityArray);
    } else if ((params_.illuminatedType == IlluminatedType::CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_CONTENT)) {
        DrawSingleContentLight(canvas, builder, brush, lightIntensityArray);
    } else if ((params_.illuminatedType == IlluminatedType::BORDER) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER) ||
        (params_.illuminatedType == IlluminatedType::FEATHERING_BORDER)) {
        DrawSingleBorderLight(canvas, builder, pen, lightIntensityArray);
    }
}

void GEPointLightShader::ProcessLightSourcesData(std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    auto iter = params_.lightSources.begin();
    int cnt = 0;
    bool needToneMapping = GEToneMappingHelper::NeedToneMapping(params_.displayHeadroom);
    while (iter != params_.lightSources.end() && cnt < MAX_LIGHT_SOURCES) {
        const auto& lightPos = iter->lightPosition;
        auto lightIntensity = iter->lightIntensity;
        const auto& lightColor = iter->lightColor;
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            float lightColorNorm = lightColor[i] / UINT8_MAX;
            if (params_.enableEDREffect && needToneMapping) {
                lightColorNorm = GEToneMappingHelper::GetBrightnessMapping(params_.displayHeadroom, lightColorNorm);
            }
            lightColorArray[cnt * vectorLen + i] = lightColorNorm;
        }
        lightIntensityArray[cnt] = std::abs(lightIntensity);
        iter++;
        cnt++;
    }
    builder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
}

void GEPointLightShader::ProcessSingleLightSourcesData(std::array<float, 1>& lightIntensityArray,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen] = { 0 };
    float viewPosArray[vectorLen] = { 0 };
    float lightColorArray[vectorLen] = { 0 };
    auto iter = params_.lightSources.begin();
    auto cnt = 0;
    bool needToneMapping = GEToneMappingHelper::NeedToneMapping(params_.displayHeadroom);
    while (iter != params_.lightSources.end() && cnt < 1) {
        const auto& lightPos = iter->lightPosition;
        auto lightIntensity = iter->lightIntensity;
        const auto& lightColor = iter->lightColor;
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            float lightColorNorm = lightColor[i] / UINT8_MAX;
            if (params_.enableEDREffect && needToneMapping) {
                lightColorNorm = GEToneMappingHelper::GetBrightnessMapping(params_.displayHeadroom, lightColorNorm);
            }
            lightColorArray[cnt * vectorLen + i] = lightColorNorm;
        }
        lightIntensityArray[cnt] = std::abs(lightIntensity);
        iter++;
        cnt++;
    }
    builder->SetUniform("lightPos", lightPosArray, vectorLen);
    builder->SetUniform("viewPos", viewPosArray, vectorLen);
    builder->SetUniform("specularLightColor", lightColorArray, vectorLen);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::CreateShaderBuilder(size_t lightLength) const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT) {
        builder = MakeNormalLightShaderBuilder(lightLength);
    } else if (params_.illuminatedType == IlluminatedType::FEATHERING_BORDER) {
        if (params_.sdfShaderEffect != nullptr) {
            return nullptr;
        }
        builder = MakeFeatheringBoardLightShaderBuilder(lightLength);
    } else {
        builder = GetPhongShaderBuilder(lightLength);
    }
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetPhongShaderBuilder(size_t lightLength)
{
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> singleLightShaderBuilder;
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> multiLightShaderBuilder;
    auto& shaderBuilder = lightLength == 1 ? singleLightShaderBuilder : multiLightShaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect = lightLength == 1 ?
        GECreateRuntimeEffectForShader(std::string(SINGLE_PHONG_SHADER_STRING), g_highpUVOption) :
        GECreateRuntimeEffectForShader(std::string(PHONG_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetPhongShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetFeatheringBorderLightShaderBuilder
    (size_t lightLength)
{
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> singleLightShaderBuilder;
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> multiLightShaderBuilder;
    auto& shaderBuilder = lightLength == 1 ? singleLightShaderBuilder : multiLightShaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect = lightLength == 1 ?
        GECreateRuntimeEffectForShader(std::string(SINGLE_FEATHERING_BORDER_LIGHT_SHADER_STRING), g_highpUVOption) :
        GECreateRuntimeEffectForShader(std::string(FEATHERING_BORDER_LIGHT_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetFeatheringBorderLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetNormalLightShaderBuilder(size_t lightLength)
{
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> singleLightShaderBuilder;
    static thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> multiLightShaderBuilder;
    auto& shaderBuilder = lightLength == 1 ? singleLightShaderBuilder : multiLightShaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect = lightLength == 1 ?
        GECreateRuntimeEffectForShader(std::string(NORMAL_SINGLE_LIGHT_SHADER_STRING), g_highpUVOption) :
        GECreateRuntimeEffectForShader(std::string(NORMAL_LIGHT_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetNormalLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetSDFContentLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        GECreateRuntimeEffectForShader(std::string(SDF_CONTENT_LIGHT_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetSDFContentLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetSDFBorderLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        GECreateRuntimeEffectForShader(std::string(SDF_BORDER_LIGHT_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetSDFBorderLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::GetSDFContentAndBorderLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        GECreateRuntimeEffectForShader(std::string(SDF_CONTENT_AND_BORDER_LIGHT_SHADER_STRING), g_highpUVOption);
    if (!effect) {
        GE_LOGE("GEPointLightShader::GetSDFContentAndBorderLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::MakeFeatheringBoardLightShaderBuilder
    (size_t lightLength) const
{
    auto builder = GetFeatheringBorderLightShaderBuilder(lightLength);
    if (!builder) {
        GE_LOGE("GEPointLightShader::MakeFeatheringBoardLightShaderBuilder effect is null");
        return nullptr;
    }
    float rectWidth = params_.contentRRect.GetRect().GetWidth();
    float rectHeight = params_.contentRRect.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    builder->SetUniform("contentBorderRadius",
        params_.contentRRect.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX());
    builder->SetUniform("borderWidth", params_.borderWidth);
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEPointLightShader::MakeNormalLightShaderBuilder
    (size_t lightLength) const
{
    auto builder = GetNormalLightShaderBuilder(lightLength);
    if (!builder) {
        GE_LOGE("GEPointLightShader::GetNormalLightShaderBuilder effect is null");
        return nullptr;
    }
    constexpr float DEFAULT_BUMP_FACTOR = 1.0f;
    constexpr float DEFAULT_GRADIENT_RADIUS = 6.0f;
    constexpr float DEFAULT_BULGE_RADIUS = 8.0f;
    constexpr float DEFAULT_EPSILON = 2.0f;
    constexpr float DEFAULT_ATTENUATION_COEFF = 0.3f;
    constexpr float CORNER_RADIUS_SCALE_FACTOR = 1.31f;
    constexpr float CORNER_THRESHOLD_FACTOR = 1.7f;
    constexpr float G2_CURVEATURE_K = 2.77f;
    constexpr float G1_CURVEATURE_K = 2.0f;
    float rectWidth = params_.contentRRect.GetRect().GetWidth();
    float rectHeight = params_.contentRRect.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    float cornerRadius = params_.contentRRect.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX();
    builder->SetUniform("bumpFactor", DEFAULT_BUMP_FACTOR);
    builder->SetUniform("gradientRadius", DEFAULT_GRADIENT_RADIUS);
    builder->SetUniform("bulgeRadius", DEFAULT_BULGE_RADIUS);
    builder->SetUniform("eps", DEFAULT_EPSILON);
    builder->SetUniform("attenuationCoeff", DEFAULT_ATTENUATION_COEFF);
    if (cornerRadius * CORNER_THRESHOLD_FACTOR * PARAM_TWO < std::min(rectWidth, rectHeight)) {
        builder->SetUniform("roundCurvature", G2_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius * CORNER_RADIUS_SCALE_FACTOR);
    } else {
        builder->SetUniform("roundCurvature", G1_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius);
    }
    return builder;
}

bool GEPointLightShader::DrawSDFContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const
{
    GE_LOGD("GEPointLightShader::DrawSDFContentLight");
    auto sdfLightBuilder = GetSDFContentLightShaderBuilder();
    if (!sdfLightBuilder || !lightShaderEffect) {
        GE_LOGE("GEPointLightShader::DrawSDFContentLight effect is null");
        return false;
    }
    sdfLightBuilder->SetChild("light", lightShaderEffect);
    sdfLightBuilder->SetChild("sdf", params_.sdfShaderEffect);
    lightShaderEffect = sdfLightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(lightShaderEffect);
    if ((params_.illuminatedType == IlluminatedType::BLEND_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&params_.contentRRect.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.contentRRect.GetRect());
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.sdfDrawRect);
        canvas.DetachBrush();
    }
    return true;
}

void GEPointLightShader::DrawContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    GE_LOGD("GEPointLightShader::DrawContentLight");
    float contentIntensityCoefficient = params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (params_.sdfShaderEffect) {
        DrawSDFContentLight(canvas, shader, brush);
        return;
    }

    brush.SetShaderEffect(shader);
    if ((params_.illuminatedType == IlluminatedType::BLEND_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&params_.contentRRect.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(params_.contentRRect);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(params_.contentRRect);
        canvas.DetachBrush();
    }
}

void GEPointLightShader::DrawSingleContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, 1>& lightIntensityArray) const
{
    GE_LOGD("GEPointLightShader::DrawSingleContentLight");
    float contentIntensityCoefficient = params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
    float specularStrengthArr[1] = { 0 };
    specularStrengthArr[0] = lightIntensityArray[0] * contentIntensityCoefficient;
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, 1);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (params_.sdfShaderEffect) {
        DrawSDFContentLight(canvas, shader, brush);
        return;
    }

    brush.SetShaderEffect(shader);
    if ((params_.illuminatedType == IlluminatedType::BLEND_CONTENT) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&params_.contentRRect.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(params_.contentRRect);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(params_.contentRRect);
        canvas.DetachBrush();
    }
}

bool GEPointLightShader::DrawSDFBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect) const
{
    GE_LOGD("GEPointLightShader::DrawSDFBorderLight");
    auto sdfLightBuilder = GetSDFBorderLightShaderBuilder();
    if (!sdfLightBuilder || !lightShaderEffect) {
        GE_LOGE("GEPointLightShader::DrawSDFBorderLight effect is null");
        return false;
    }
    sdfLightBuilder->SetChild("light", lightShaderEffect);
    sdfLightBuilder->SetChild("sdf", params_.sdfShaderEffect);
    sdfLightBuilder->SetUniform("borderWidth", params_.borderWidth);
    lightShaderEffect = sdfLightBuilder->MakeShader(nullptr, false);

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetShaderEffect(lightShaderEffect);
    if ((params_.illuminatedType == IlluminatedType::BLEND_BORDER) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::Brush maskPaint;
        Drawing::SaveLayerOps slo(&params_.contentRRect.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.contentRRect.GetRect());
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.sdfDrawRect);
        canvas.DetachBrush();
    }
    return true;
}

void GEPointLightShader::DrawBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    GE_LOGD("GEPointLightShader::DrawBorderLight");
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i];
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (params_.sdfShaderEffect) {
        DrawSDFBorderLight(canvas, shader);
        return;
    }

    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(params_.borderWidth);
    pen.SetWidth(borderWidth);
    if ((params_.illuminatedType == IlluminatedType::BLEND_BORDER) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&params_.borderRRect.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(params_.borderRRect);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(params_.borderRRect);
        canvas.DetachPen();
    }
}

void GEPointLightShader::DrawSingleBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, 1>& lightIntensityArray) const
{
    GE_LOGD("GEPointLightShader::DrawSingleBorderLight");
    float specularStrengthArr[1] = { 0 };
    specularStrengthArr[0] = lightIntensityArray[0];
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, 1);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (params_.sdfShaderEffect) {
        DrawSDFBorderLight(canvas, shader);
        return;
    }

    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(params_.borderWidth);
    pen.SetWidth(borderWidth);
    if ((params_.illuminatedType == IlluminatedType::BLEND_BORDER) ||
        (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&params_.borderRRect.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(params_.borderRRect);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(params_.borderRRect);
        canvas.DetachPen();
    }
}

void GEPointLightShader::DrawContentAndBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    if (params_.sdfShaderEffect) {
        std::shared_ptr<Drawing::RuntimeShaderBuilder> lightBuilderContent =
            std::make_shared<Drawing::RuntimeShaderBuilder>(*lightBuilder);
        float contentIntensityCoefficient =
            params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
        float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };

        for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
            specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
        }
        lightBuilderContent->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
        std::shared_ptr<Drawing::ShaderEffect> shaderContent = lightBuilderContent->MakeShader(nullptr, false);

        for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
            specularStrengthArr[i] = lightIntensityArray[i];
        }
        lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
        std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);

        DrawSDFContentAndBorderLight(canvas, shaderContent, shader, brush);
    } else {
        DrawContentLight(canvas, lightBuilder, brush, lightIntensityArray);
        DrawBorderLight(canvas, lightBuilder, pen, lightIntensityArray);
    }
}

void GEPointLightShader::DrawSingleContentAndBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Drawing::Pen& pen,
    const std::array<float, 1>& lightIntensityArray) const
{
    if (params_.sdfShaderEffect) {
        std::shared_ptr<Drawing::RuntimeShaderBuilder> lightBuilderContent =
            std::make_shared<Drawing::RuntimeShaderBuilder>(*lightBuilder);
        float contentIntensityCoefficient =
            params_.illuminatedType == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
        float specularStrengthArr[1] = { 0 };

        specularStrengthArr[0] = lightIntensityArray[0] * contentIntensityCoefficient;
        lightBuilderContent->SetUniform("specularStrength", specularStrengthArr, 1);
        std::shared_ptr<Drawing::ShaderEffect> shaderContent = lightBuilderContent->MakeShader(nullptr, false);

        specularStrengthArr[0] = lightIntensityArray[0];
        lightBuilder->SetUniform("specularStrength", specularStrengthArr, 1);
        std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);

        DrawSDFContentAndBorderLight(canvas, shaderContent, shader, brush);
    } else {
        DrawSingleContentLight(canvas, lightBuilder, brush, lightIntensityArray);
        DrawSingleBorderLight(canvas, lightBuilder, pen, lightIntensityArray);
    }
}

void GEPointLightShader::DrawSDFContentAndBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffectContent,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const
{
    auto sdfLightBuilder = GetSDFContentAndBorderLightShaderBuilder();
    bool isValidLightShader = sdfLightBuilder && lightShaderEffect && lightShaderEffectContent;
    if (!isValidLightShader) {
        GE_LOGE("GEPointLightShader::DrawSDFContentAndBorderLight effect is null");
        return;
    }
    sdfLightBuilder->SetChild("lightContent", lightShaderEffectContent);
    sdfLightBuilder->SetChild("light", lightShaderEffect);
    sdfLightBuilder->SetChild("sdf", params_.sdfShaderEffect);
    sdfLightBuilder->SetUniform("borderWidth", params_.borderWidth);

    lightShaderEffect = sdfLightBuilder->MakeShader(nullptr, false);

    brush.SetAntiAlias(true);
    brush.SetShaderEffect(lightShaderEffect);

    if (params_.illuminatedType == IlluminatedType::BLEND_BORDER_CONTENT) {
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::Brush maskPaint;
        Drawing::SaveLayerOps slo(&params_.contentRRect.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.contentRRect.GetRect());
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRect(params_.sdfDrawRect);
        canvas.DetachBrush();
    }
}
} // namespace Rosen
} // namespace OHOS