/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H

#include <memory>
#include <vector>
#include <any>

#include "ge_shader.h"
#include "ge_shader_filter.h"
#include "ge_shader_filter_params.h"
#include "ge_visual_effect.h"

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GEVisualEffectImpl {
public:
    enum class FilterType {
        NONE,
        KAWASE_BLUR,
        MESA_BLUR,
        GREY,
        AIBAR,
        LINEAR_GRADIENT_BLUR,
        MAGNIFIER,
        WATER_RIPPLE,
        DISPLACEMENT_DISTORT_FILTER,
        COLOR_GRADIENT,
        // Mask
        RIPPLE_MASK,
        DOUBLE_RIPPLE_MASK,
        PIXEL_MAP_MASK,
        RADIAL_GRADIENT_MASK,
        WAVE_GRADIENT_MASK,
        // Shader
        DOT_MATRIX,
        FLOW_LIGHT_SWEEP,
        COMPLEX_SHADER,
        SOUND_WAVE,
        EDGE_LIGHT,
        BEZIER_WARP,
        DISPERSION,
        CONTENT_LIGHT,
        BORDER_LIGHT,
        CONTOUR_DIAGONAL_FLOW_LIGHT,
        WAVY_RIPPLE_LIGHT,
        AURORA_NOISE,
        PARTICLE_CIRCULAR_HALO,
        MAX
    };

    GEVisualEffectImpl(const std::string& name);

    ~GEVisualEffectImpl();

    void SetParam(const std::string& tag, int32_t param);
    void SetParam(const std::string& tag, int64_t param);
    void SetParam(const std::string& tag, float param);
    void SetParam(const std::string& tag, double param);
    void SetParam(const std::string& tag, const char* const param);

    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::Path> param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param);
    void SetParam(const std::string& tag, const Drawing::Matrix param);
    void SetParam(const std::string& tag, const std::pair<float, float>& param);
    void SetParam(const std::string& tag, const std::vector<std::pair<float, float>>);
    void SetParam(const std::string& tag, const std::array<Drawing::Point, POINT_NUM>& param);
    void SetParam(const std::string& tag, bool param);
    void SetParam(const std::string& tag, uint32_t param);
    void SetParam(const std::string& tag, const std::vector<float>& param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderMask> param);
    void SetParam(const std::string& tag, const Drawing::Color4f& param);
    void SetParam(const std::string& tag, const Vector3f& param);
    void SetParam(const std::string& tag, const Vector4f& param);
    void SetParam(const std::string& tag, const RectF& param);

    void SetFilterType(FilterType type)
    {
        filterType_ = type;
    }

    const FilterType& GetFilterType() const
    {
        return filterType_;
    }

    void MakeMESAParams()
    {
        mesaParams_ = std::make_shared<GEMESABlurShaderFilterParams>();
    }

    const std::shared_ptr<GEMESABlurShaderFilterParams>& GetMESAParams() const
    {
        return mesaParams_;
    }

    void MakeKawaseParams()
    {
        kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    }

    const std::shared_ptr<GEKawaseBlurShaderFilterParams>& GetKawaseParams() const
    {
        return kawaseParams_;
    }

    void MakeWaterRippleParams()
    {
        waterRippleParams_ = std::make_shared<GEWaterRippleFilterParams>();
    }

    const std::shared_ptr<GEWaterRippleFilterParams>& GetWaterRippleParams() const
    {
        return waterRippleParams_;
    }

    void MakeAIBarParams()
    {
        aiBarParams_ = std::make_shared<GEAIBarShaderFilterParams>();
    }

    const std::shared_ptr<GEAIBarShaderFilterParams>& GetAIBarParams() const
    {
        return aiBarParams_;
    }

    void MakeGreyParams()
    {
        greyParams_ = std::make_shared<GEGreyShaderFilterParams>();
    }

    const std::shared_ptr<GEGreyShaderFilterParams>& GetGreyParams() const
    {
        return greyParams_;
    }

    void MakeLinearGradientBlurParams()
    {
        linearGradientBlurParams_ = std::make_shared<GELinearGradientBlurShaderFilterParams>();
    }

    const std::shared_ptr<GELinearGradientBlurShaderFilterParams>& GetLinearGradientBlurParams() const
    {
        return linearGradientBlurParams_;
    }

    void MakeMagnifierParams()
    {
        magnifierParams_ = std::make_shared<GEMagnifierShaderFilterParams>();
    }

    const std::shared_ptr<GEMagnifierShaderFilterParams>& GetMagnifierParams() const
    {
        return magnifierParams_;
    }

    void MakeColorGradientParams()
    {
        colorGradientParams_ = std::make_shared<GEColorGradientShaderFilterParams>();
    }

    const std::shared_ptr<GEColorGradientShaderFilterParams>& GetColorGradientParams() const
    {
        return colorGradientParams_;
    }

    void MakeDisplacementDistortParams()
    {
        displacementDistortParams_ = std::make_shared<GEDisplacementDistortFilterParams>();
    }

    const std::shared_ptr<GEDisplacementDistortFilterParams> &GetDisplacementDistortParams() const
    {
        return displacementDistortParams_;
    }

    void MakeSoundWaveParams()
    {
        soundWaveParams_ = std::make_shared<GESoundWaveFilterParams>();
    }

    const std::shared_ptr<GESoundWaveFilterParams>& GetSoundWaveParams() const
    {
        return soundWaveParams_;
    }

    void MakeEdgeLightParams()
    {
        edgeLightParams_ = std::make_shared<GEEdgeLightShaderFilterParams>();
    }

    const std::shared_ptr<GEEdgeLightShaderFilterParams>& GetEdgeLightParams() const
    {
        return edgeLightParams_;
    }

    void MakeBezierWarpParams()
    {
        bezierWarpParams_ = std::make_shared<GEBezierWarpShaderFilterParams>();
    }

    const std::shared_ptr<GEBezierWarpShaderFilterParams>& GetBezierWarpParams() const
    {
        return bezierWarpParams_;
    }

    void MakeDispersionParams()
    {
        dispersionParams_ = std::make_shared<GEDispersionShaderFilterParams>();
    }

    const std::shared_ptr<GEDispersionShaderFilterParams>& GetDispersionParams() const
    {
        return dispersionParams_;
    }

    void MakeContentLightParams()
    {
        contentLightParams_ = std::make_shared<GEContentLightFilterParams>();
    }

    const std::shared_ptr<GEContentLightFilterParams>& GetContentLightParams() const
    {
        return contentLightParams_;
    }

    void MakeContentDiagonalParams()
    {
        contentDiagonalParams_ = std::make_shared<GEContentDiagonalFlowLightShaderParams>();
    }

    const std::shared_ptr<GEContentDiagonalFlowLightShaderParams>& GetContenDiagonalParams() const
    {
        return contentDiagonalParams_;
    }

    void MakeWavyRippleLightParams()
    {
        wavyRippleLightParams_ = std::make_shared<GEWavyRippleLightShaderParams>();
    }

    const std::shared_ptr<GEWavyRippleLightShaderParams>& GetWavyRippleLightParams() const
    {
        return wavyRippleLightParams_;
    }

    void MakeAuroraNoiseParams()
    {
        auroNoiseParams_ = std::make_shared<GEAuroraNoiseShaderParams>();
    }

    const std::shared_ptr<GEAuroraNoiseShaderParams>& GetAuroraNoiseParams() const
    {
        return auroNoiseParams_;
    }

    void MakeParticleCircularHaloParams()
    {
        particleCircularHaloParams_ = std::make_shared<GEParticleCircularHaloShaderParams>();
    }

    const std::shared_ptr<GEParticleCircularHaloShaderParams>& GetParticleCircularHaloParams() const
    {
        return particleCircularHaloParams_;
    }

    void SetCache(std::shared_ptr<std::any> cacheData)
    {
        cacheAnyPtr_ = cacheData;
    }

    std::shared_ptr<std::any> GetCache()
    {
        return cacheAnyPtr_;
    }

    void MakeRippleMaskParams()
    {
        rippleMaskParams_ = std::make_shared<GERippleShaderMaskParams>();
    }

    const std::shared_ptr<GERippleShaderMaskParams>& GetRippleMaskParams() const
    {
        return rippleMaskParams_;
    }

    void MakeDoubleRippleMaskParams()
    {
        doubleRippleMaskParams_ = std::make_shared<GEDoubleRippleShaderMaskParams>();
    }

    const std::shared_ptr<GEDoubleRippleShaderMaskParams>& GetDoubleRippleMaskParams() const
    {
        return doubleRippleMaskParams_;
    }

    void MakeRadialGradientMaskParams()
    {
        radialGradientMaskParams_ = std::make_shared<GERadialGradientShaderMaskParams>();
    }

    const std::shared_ptr<GERadialGradientShaderMaskParams>& GetRadialGradientMaskParams() const
    {
        return radialGradientMaskParams_;
    }

    void MakePixelMapMaskParams()
    {
        pixelMapMaskParams_ = std::make_shared<GEPixelMapMaskParams>();
    }

    const std::shared_ptr<GEPixelMapMaskParams>& GetPixelMapMaskParams() const
    {
        return pixelMapMaskParams_;
    }

    void MakeWaveGradientMaskParams()
    {
        waveGradientMaskParams_ = std::make_shared<GEWaveGradientShaderMaskParams>();
    }

    const std::shared_ptr<GEWaveGradientShaderMaskParams>& GetWaveGradientMaskParams() const
    {
        return waveGradientMaskParams_;
    }

private:
    static std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> g_initialMap;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;
    void SetMESABlurParams(const std::string& tag, float param);
    void SetAIBarParams(const std::string& tag, float param);
    void SetGreyParams(const std::string& tag, float param);
    void SetLinearGradientBlurParams(const std::string& tag, float param);

    void SetMagnifierParamsFloat(const std::string& tag, float param);
    void SetMagnifierParamsUint32(const std::string& tag, uint32_t param);

    void SetWaterRippleParams(const std::string& tag, float param);
    void SetRippleMaskParamsFloat(const std::string& tag, float param);
    void SetDoubleRippleMaskParamsFloat(const std::string& tag, float param);
    void SetRadialGradientMaskParamsFloat(const std::string& tag, float param);
    void SetWaveGradientMaskParamsFloat(const std::string& tag, float param);
    void SetSoundWaveParamsFloat(const std::string& tag, float param);
    void SetEdgeLightParams(const std::string& tag, float param);
    void SetDispersionParams(const std::string& tag, float param);
    void SetDispersionParams(const std::string& tag, const std::pair<float, float>& param);
    void SetContentLightParams(const std::string& tag, float param);
    void SetContentDiagonalFlowParams(const std::string& tag, const Vector4f& param);
    void SetContentDiagonalFlowParams(const std::string& tag, float param);
    void SetWavyRippleLightParams(const std::string& tag, float param);
    void SetWavyRippleLightParams(const std::string& tag, const std::pair<float, float>& param);
    void SetAuroraNoiseParams(const std::string& tag, float param);
    void SetPixelMapMaskParams(const std::string& tag, const RectF& param);
    void SetPixelMapMaskParams(const std::string& tag, const Vector4f& param);
    void SetParticleCircularHaloParams(const std::string& tag, float param);
    void SetParticleCircularHaloParams(const std::string& tag, const std::pair<float, float>& param);
    FilterType filterType_ = GEVisualEffectImpl::FilterType::NONE;

    // ShaderFilter Params
    std::shared_ptr<GEKawaseBlurShaderFilterParams> kawaseParams_ = nullptr;
    std::shared_ptr<GEMESABlurShaderFilterParams> mesaParams_ = nullptr;
    std::shared_ptr<GEAIBarShaderFilterParams> aiBarParams_ = nullptr;
    std::shared_ptr<GEColorGradientShaderFilterParams> colorGradientParams_ = nullptr;
    std::shared_ptr<GEGreyShaderFilterParams> greyParams_ = nullptr;
    std::shared_ptr<GELinearGradientBlurShaderFilterParams> linearGradientBlurParams_ = nullptr;

    std::shared_ptr<GEMagnifierShaderFilterParams> magnifierParams_ = nullptr;
    std::shared_ptr<GEWaterRippleFilterParams> waterRippleParams_ = nullptr;

    std::shared_ptr<GERadialGradientShaderMaskParams> radialGradientMaskParams_ = nullptr;
    std::shared_ptr<GEPixelMapMaskParams> pixelMapMaskParams_ = nullptr;
    std::shared_ptr<GERippleShaderMaskParams> rippleMaskParams_ = nullptr;
    std::shared_ptr<GEDoubleRippleShaderMaskParams> doubleRippleMaskParams_ = nullptr;
    std::shared_ptr<GEWaveGradientShaderMaskParams> waveGradientMaskParams_ = nullptr;
    std::shared_ptr<GEDisplacementDistortFilterParams> displacementDistortParams_ = nullptr;
    std::shared_ptr<GESoundWaveFilterParams> soundWaveParams_ = nullptr;
    std::shared_ptr<GEEdgeLightShaderFilterParams> edgeLightParams_ = nullptr;
    std::shared_ptr<GEBezierWarpShaderFilterParams> bezierWarpParams_ = nullptr;
    std::shared_ptr<GEDispersionShaderFilterParams> dispersionParams_ = nullptr;
    std::shared_ptr<GEContentLightFilterParams> contentLightParams_ = nullptr;

    std::shared_ptr<GEContentDiagonalFlowLightShaderParams> contentDiagonalParams_ = nullptr;
    std::shared_ptr<GEWavyRippleLightShaderParams> wavyRippleLightParams_ = nullptr;
    std::shared_ptr<GEAuroraNoiseShaderParams> auroNoiseParams_ = nullptr;
    std::shared_ptr<GEParticleCircularHaloShaderParams> particleCircularHaloParams_ = nullptr;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H
