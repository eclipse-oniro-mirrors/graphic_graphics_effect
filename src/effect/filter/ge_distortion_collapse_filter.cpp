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

#include "ge_distortion_collapse_filter.h"

#include "ge_log.h"

namespace OHOS::Rosen {
namespace {

constexpr char SHADER[] = R"(
    uniform vec2 iResolution;

    uniform shader imageShader;
    uniform vec2 imageSize;

    uniform vec2 lu;
    uniform vec2 ru;
    uniform vec2 rb;
    uniform vec2 lb;

    uniform vec4 barrelDistortion;

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

    half4 main(vec2 fragCoord) {
        vec2 uv = fragCoord / imageSize.xy;
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

        vec2 newCoord = newUV * imageSize;
        if (newCoord.x < 0.0 || newCoord.y < 0.0 || newCoord.x > imageSize.x || newCoord.y > imageSize.y) {
            return half4(0.0);
        }
        return imageShader.eval(newCoord);
    }
)";
} // namespace

GEDistortionCollapseFilter::GEDistortionCollapseFilter(const Drawing::GEDistortionCollapseFilterParams& params)
    : LUCorner_(params.LUCorner_),
      RUCorner_(params.RUCorner_),
      RBCorner_(params.RBCorner_),
      LBCorner_(params.LBCorner_),
      barrelDistortion_(params.barrelDistortion_)
{}

std::shared_ptr<Drawing::Image> GEDistortionCollapseFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEDistortionCollapseFilter::OnProcessImage image is null");
        return image;
    }

    auto builder = MakeEffectShader(image, src, dst);
    if (!builder) {
        LOGE("GEDistortionCollapseFilter::OnProcessImage builder is null");
        return image;
    }

    auto outputImageInfo = image->GetImageInfo();

    Drawing::Matrix matrix;
    matrix.PreScale(src.GetWidth() / dst.GetWidth(), src.GetHeight() / dst.GetHeight());
    matrix.PreTranslate(src.GetLeft() - dst.GetLeft(), src.GetTop() - dst.GetTop());

    outputImageInfo.SetWidth(dst.GetWidth());
    outputImageInfo.SetHeight(dst.GetHeight());

    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), &(matrix), image->GetImageInfo(), false);
    if (!resultImage) {
        LOGE("GEDistortionCollapseFilter::OnProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEDistortionCollapseFilter::MakeEffectShader(
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    static auto effectShader = Drawing::RuntimeEffect::CreateForShader(SHADER);
    if (!effectShader) {
        LOGE("MakeEffectShader::RuntimeShader effect error\n");
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(effectShader);

    builder->SetUniform("iResolution", dst.GetWidth(), dst.GetHeight());

    Drawing::Matrix matrix;
    const Drawing::SamplingOptions sampling(Drawing::FilterMode::LINEAR);
    builder->SetChild("imageShader", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, sampling, matrix));
    builder->SetUniform("imageSize", src.GetWidth(), src.GetHeight());

    builder->SetUniform("lu", LUCorner_.GetX(), LUCorner_.GetY());
    builder->SetUniform("ru", RUCorner_.GetX(), RUCorner_.GetY());
    builder->SetUniform("rb", RBCorner_.GetX(), RBCorner_.GetY());
    builder->SetUniform("lb", LBCorner_.GetX(), LBCorner_.GetY());

    const int barrelDistortionSize = 4;
    float barrelDistortion[] = {barrelDistortion_[0], barrelDistortion_[1], barrelDistortion_[2],
        barrelDistortion_[3]};
    builder->SetUniform("barrelDistortion", barrelDistortion, barrelDistortionSize);

    return builder;
}
} // namespace OHOS::Rosen