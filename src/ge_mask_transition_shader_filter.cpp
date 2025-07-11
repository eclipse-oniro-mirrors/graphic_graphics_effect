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

#include "ge_mask_transition_shader_filter.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_transitionBlendShader_ = nullptr;

GEMaskTransitionShaderFilter::GEMaskTransitionShaderFilter(const Drawing::GEMaskTransitionShaderFilterParams& params)
    : params_(params)
{}

std::shared_ptr<Drawing::Image> GEMaskTransitionShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || params_.mask == nullptr) {
        GE_LOGE("GEMaskTransitionShaderFilter::ProcessImage input parameter is invalid");
        return image;
    }

    auto cache = GetCache();
    if (cache == nullptr) {
        SetCache(std::make_shared<std::any>(std::make_any<std::shared_ptr<Drawing::Image>>(image)));
        return image;
    }

    auto cacheImage = std::any_cast<std::shared_ptr<Drawing::Image>>(*cache);
    if (cacheImage == nullptr || cacheImage.get() == nullptr) {
        GE_LOGE("GEMaskTransitionShaderFilter::ProcessImage cacheImage is nullptr");
        return image;
    }

    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    auto maskEffectShader = params_.mask->GenerateDrawingShader(width, height);
    if (maskEffectShader == nullptr) {
        GE_LOGE("GEMaskTransitionShaderFilter::ProcessImage maskEffectShader generate failed");
        return image;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = GetMaskTransitionEffect();
    if (builder == nullptr) {
        GE_LOGE("GEMaskTransitionShaderFilter::ProcessImage get mask transition effect failed");
        return image;
    }

    Drawing::Matrix topLayerMatrix;
    auto topLayer = Drawing::ShaderEffect::CreateImageShader(*cacheImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), topLayerMatrix);

    Drawing::Matrix bottomLayerMatrix;
    auto bottomLayer = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), bottomLayerMatrix);

    builder->SetChild("alphaMask", maskEffectShader);
    builder->SetChild("topLayer", topLayer);
    builder->SetChild("bottomLayer", bottomLayer);
    builder->SetUniform("factor", params_.factor);
    builder->SetUniform("inverseFlag", params_.inverse);
    auto transitionImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (!transitionImage) {
        GE_LOGE("GEMaskTransitionShaderFilter::ProcessImage make image failed");
        return image;
    }
    return transitionImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEMaskTransitionShaderFilter::GetMaskTransitionEffect()
{
    if (g_transitionBlendShader_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader alphaMask;
            uniform shader topLayer;
            uniform shader bottomLayer;
            uniform float factor;
            uniform float inverseFlag;

            vec4 main(float2 fragCoord)
            {
                float alpha = alphaMask.eval(fragCoord).b * factor;
                alpha = mix(alpha, 1.0 - alpha, inverseFlag);
                vec4 topColor = topLayer.eval(fragCoord);
                vec4 bottomColor = bottomLayer.eval(fragCoord);
                vec4 finalColor = mix(bottomColor, topColor, 1.0 - alpha);
                return finalColor;
            }
        )";

        g_transitionBlendShader_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (g_transitionBlendShader_ == nullptr) {
            GE_LOGE("GEMaskTransitionShaderFilter::MakeTransitionBlendShadereffect failed");
            return nullptr;
        }
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(g_transitionBlendShader_);
}

} // namespace Rosen
} // namespace OHOS