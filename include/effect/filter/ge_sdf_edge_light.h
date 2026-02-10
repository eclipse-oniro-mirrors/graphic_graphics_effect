
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

#ifndef GRAPHICS_EFFECT_GE_SDF_EDGE_LIGHT_H
#define GRAPHICS_EFFECT_GE_SDF_EDGE_LIGHT_H

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "effect/shader_effect.h"
#include "ge_filter_type_info.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "ge_sdf_shader_shape.h"


namespace OHOS::Rosen {
class GESDFEdgeLight final : public GEShaderFilter {
    std::shared_ptr<Drawing::Image> BlurSdfMap(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> sdfImage, float radius);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeEffectShader(float imageWidth, float imageHeight);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeImageMerger(Drawing::Image& image, Drawing::Image& composeImage);

    std::shared_ptr<Drawing::Image> MergeImage(
        Drawing::Canvas& canvas, std::shared_ptr<Drawing::Image> image, std::shared_ptr<Drawing::Image> compositeImage);

public:
    explicit GESDFEdgeLight(const Drawing::GESDFEdgeLightFilterParams& params);

    GESDFEdgeLight(GESDFEdgeLight&& edgeLight) = delete;
    GESDFEdgeLight(const GESDFEdgeLight& edgeLight) = delete;

    GESDFEdgeLight& operator=(GESDFEdgeLight&& edgeLight) = delete;
    GESDFEdgeLight& operator=(const GESDFEdgeLight& edgeLight) = delete;

    DECLARE_GEFILTER_TYPEFUNC(GESDFEdgeLight, Drawing::GESDFEdgeLightFilterParams);

    GE_EXPORT std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    float sdfSpreadFactor_ = 64.0f;

    float bloomIntensityCutoff_ = 0.1f;
    float maxIntensity_ = 1.0f;
    float maxBloomIntensity_ = 1.0f;
    float bloomFalloffPow_ = 2.0f;

    float minBorderWidth_ = 2;
    float maxBorderWidth_ = 5;

    float innerBorderBloomWidth_ = 30;
    float outerBorderBloomWidth_ = 30;

    std::shared_ptr<Drawing::Image> blurredSdfImage_;
    std::shared_ptr<Drawing::Image> sdfImage_;
    std::shared_ptr<Drawing::GESDFShaderShape> sdfShape_;
    std::shared_ptr<Drawing::GEShaderMask> lightMask_;
};
} // namespace OHOS::Rosen

#endif // GRAPHICS_EFFECT_GE_SDF_EDGE_LIGHT_H
