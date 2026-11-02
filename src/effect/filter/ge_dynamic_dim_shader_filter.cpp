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
#include "ge_dynamic_dim_shader_filter.h"

#include "ge_common.h"
#include "ge_log.h"
#include "ge_shader_diagnostics.h"
#include "include/core/SkTileMode.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

GEDynamicDimShaderFilter::GEDynamicDimShaderFilter(const Drawing::GEDynamicDimShaderFilterParams& params)
    : value_(params.value)
{
    InitDynamicDimEffect();
}

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_dynamicDimEffect;

std::shared_ptr<Drawing::Image> GEDynamicDimShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEDynamicDimShaderFilter::input image is null");
        return image;
    }
    if (!GE_GE(value_, 0.0f) || !GE_LNE(value_, 1.0f)) {
        LOGE("GEDynamicDimShaderFilter::OnProcessImage invalid value");
        return image;
    }
    if (!g_dynamicDimEffect) {
        LOGE("GEDynamicDimShaderFilter::dynamicDimEffect is null");
        return image;
    }
    Drawing::RuntimeShaderBuilder builder(g_dynamicDimEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder.SetChild("imageShader", imageShader);
    builder.SetUniform("dynamicDimDeg", value_);
#ifdef RS_ENABLE_GPU
    auto outImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
#else
    auto outImage = builder.MakeImage(nullptr, nullptr, image->GetImageInfo(), false);
#endif
    if (outImage == nullptr) {
        LOGE("GEDynamicDimShaderFilter::OnProcessImage MakeImage failed");
        return image;
    }
    return outImage;
}

bool GEDynamicDimShaderFilter::InitDynamicDimEffect()
{
    if (g_dynamicDimEffect != nullptr) {
        return true;
    }
    static std::string dynamicDimString(R"(
        uniform shader imageShader;
        uniform half dynamicDimDeg;
        vec3 rgb2hsv(in vec3 c)
        {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }
        vec3 hsv2rgb(in vec3 c)
        {
            vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
            return c.z * mix(vec3(1.0), rgb, c.y);
        }

        half4 main(float2 coord) {
            vec3 hsv = rgb2hsv(imageShader.eval(coord).rgb);
            float value = max(0.8, dynamicDimDeg);
            hsv.y = hsv.y * (1.75 - value * 0.75);
            hsv.z = min(hsv.z * dynamicDimDeg, 1.0);
            return vec4(hsv2rgb(hsv), 1.0);
        }
    )");
    g_dynamicDimEffect = GECreateRuntimeEffectForShader(dynamicDimString);
    if (g_dynamicDimEffect == nullptr) {
        LOGE("GEDynamicDimShaderFilter::InitDynamicDimEffect effect create failed");
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
