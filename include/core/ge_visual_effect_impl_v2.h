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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_V2_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_V2_H

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <array>
#include <optional>
#include <vector>
#include <any>

#include "core/ge_filter_type.h"
#include "ge_shader.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"
#include "effect/ge_filter_params.h"
#include "effect/ge_params_reflection_v2.h"

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

/**
 * GEVisualEffectImplV2 - Refactored implementation using GEFilterParams variant.
 *
 * Key differences from GEVisualEffectImpl:
 * 1. Uses single `params_` member (type-erased GEFilterParams) instead of 60+ individual params
 * 2. SetParam uses reflection metadata (GEParamsMemberTag) for type-safe field access
 * 3. String-based SetParam uses generated GEParamsMemberTagFromString mapping
 */
class GE_EXPORT GEVisualEffectImplV2 {
public:
    using FilterType = GEFilterType;

    GEVisualEffectImplV2(const std::string& name, const std::optional<Drawing::CanvasInfo>& canvasInfo = std::nullopt);

    ~GEVisualEffectImplV2();

    // ========================================================================
    // Main SetParam overloads using reflection
    // ========================================================================

    /// Set parameter by tag enum (type-safe, uses reflection)
    template<typename T>
    void SetParam(GEV2::GEParamsMemberTag tag, T value)
    {
        if (!params_) {
            LOGE("GEVisualEffectImplV2::SetParam: params_ is null");
            return;
        }

        // Check if tag is valid for current params type
        if (!IsTagValidForCurrentType(tag)) {
            LOGE("GEVisualEffectImplV2::SetParam: tag %u not valid for current params type", static_cast<uint32_t>(tag));
            return;
        }

        // Use generated helper to set parameter by tag
        SetParamInternal(tag, value);
    }

    /// Set parameter by string tag (runtime lookup to enum, then dispatch)
    template<typename T>
    void SetParam(const std::string& tagStr, T value)
    {
        auto tag = GEV2::GEParamsMemberHelper::GEParamsMemberTagFromString(tagStr);
        if (tag != GEV2::GEParamsMemberTag::INVALID) {
            SetParam(tag, value);
        } else {
            LOGE("GEVisualEffectImplV2::SetParam: unknown tag '%s'", tagStr.c_str());
        }
    }

    // ========================================================================
    // Filter type management
    // ========================================================================

    void SetFilterType(FilterType type)
    {
        filterType_ = type;
    }

    const FilterType& GetFilterType() const
    {
        return filterType_;
    }

    // ========================================================================
    // Params management - using GEFilterParams variant
    // ========================================================================

    /// Create params of specific type and store in params_
    template<typename ParamsType>
    void MakeParams()
    {
        params_ = GEV2::GEFilterParams::Box(ParamsType{});
    }

    template<typename ParamsType>
    auto GetParams() const
    {
        return GEV2::GEFilterParams::Unbox<ParamsType>(params_);
    }

    // ========================================================================
    // Backward-compatible convenience methods
    // ========================================================================

#define GE_DECLARE_MAKE_GET_PARAMS(MethodName, ParamsType) \
    void Make##MethodName() { MakeParams<ParamsType>(); } \
    auto Get##MethodName() const { return GetParams<ParamsType>(); }

    GE_DECLARE_MAKE_GET_PARAMS(MESAParams, GEV2::GEMESABlurShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(KawaseParams, GEV2::GEKawaseBlurShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(AIBarParams, GEV2::GEAIBarShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(GreyParams, GEV2::GEGreyShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(WaterRippleParams, GEV2::GEWaterRippleFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(LinearGradientBlurParams, GEV2::GELinearGradientBlurShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(MagnifierParams, GEV2::GEMagnifierShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(ColorGradientParams, GEV2::GEColorGradientShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(DisplacementDistortParams, GEV2::GEDisplacementDistortFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(SoundWaveParams, GEV2::GESoundWaveFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(EdgeLightParams, GEV2::GEEdgeLightShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(BezierWarpParams, GEV2::GEBezierWarpShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(DispersionParams, GEV2::GEDispersionShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(DirectionLightParams, GEV2::GEDirectionLightShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(ContentLightParams, GEV2::GEContentLightFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(ContenDiagonalParams, GEV2::GEContentDiagonalFlowLightShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(DotMatrixShaderParams, GEV2::GEDotMatrixShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(WavyRippleLightParams, GEV2::GEWavyRippleLightShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(AuroraNoiseParams, GEV2::GEAuroraNoiseShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(ParticleCircularHaloParams, GEV2::GEParticleCircularHaloShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(VariableRadiusBlurParams, GEV2::GEVariableRadiusBlurShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(SdfEdgeLightParams, GEV2::GESDFEdgeLightFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(SdfFromImageParams, GEV2::GESDFFromImageFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFBorderShaderParams, GEV2::GESDFBorderShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFShadowShaderParams, GEV2::GESDFShadowShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(RippleMaskParams, GEV2::GERippleShaderMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(DoubleRippleMaskParams, GEV2::GEDoubleRippleShaderMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(WaveDisturbanceMaskParams, GEV2::GEWaveDisturbanceShaderMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(RadialGradientMaskParams, GEV2::GERadialGradientShaderMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(PixelMapMaskParams, GEV2::GEPixelMapMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(ImageMaskParams, GEV2::GEImageMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(UseEffectMaskParams, GEV2::GEUseEffectMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(WaveGradientMaskParams, GEV2::GEWaveGradientShaderMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(MaskTransitionParams, GEV2::GEMaskTransitionShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFUnionOpShapeParams, GEV2::GESDFUnionOpShapeParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFRRectShapeParams, GEV2::GESDFRRectShapeParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFPixelmapShapeParams, GEV2::GESDFPixelmapShapeParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFTransformShapeParams, GEV2::GESDFTransformShapeParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFClipParams, GEV2::GESDFClipShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(SDFColorParams, GEV2::GESDFColorShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(LightCaveParams, GEV2::GEXLightCaveShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(DistortChromaParams, GEV2::GEXDistortChromaEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(DupoliNoiseMaskParams, GEV2::GEXDupoliNoiseMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(BorderLightParams, GEV2::GEBorderLightShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(GasifyScaleTwistFilterParams, GEV2::GEGasifyScaleTwistFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(GasifyBlurFilterParams, GEV2::GEGasifyBlurFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(GasifyFilterParams, GEV2::GEGasifyFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(ColorGradientEffectParams, GEV2::GEXColorGradientEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(HarmoniumEffectParams, GEV2::GEHarmoniumEffectShaderParams)
    GE_DECLARE_MAKE_GET_PARAMS(AIBarGlowEffectParams, GEV2::GEXAIBarGlowEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(AIBarRectHaloEffectParams, GEV2::GEXAIBarRectHaloEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(RoundedRectFlowlightEffectParams, GEV2::GEXRoundedRectFlowlightEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(GradientFlowColorsEffectParams, GEV2::GEXGradientFlowColorsEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(FrameGradientMaskParams, GEV2::GEFrameGradientMaskParams)
    GE_DECLARE_MAKE_GET_PARAMS(CircleFlowlightEffectParams, GEV2::GECircleFlowlightEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(FrostedGlassEffectParams, GEV2::GEFrostedGlassEffectParams)
    GE_DECLARE_MAKE_GET_PARAMS(FrostedGlassParams, GEV2::GEFrostedGlassBlurShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(GridWarpFilterParams, GEV2::GEGridWarpShaderFilterParams)
    GE_DECLARE_MAKE_GET_PARAMS(NoisyFrameGradientMaskParams, GEV2::GEXNoisyFrameGradientMaskParams)

    // Special overload: MakeSDFUnionOpShapeParams with op parameter
    void MakeSDFUnionOpShapeParams(const GEV2::GESDFUnionOp& op);
#undef GE_DECLARE_MAKE_GET_PARAMS

    void SetCanvasInfo(Drawing::CanvasInfo info)
    {
        canvasInfo_ = info;
    }

    const Drawing::CanvasInfo& GetCanvasInfo() const
    {
        return canvasInfo_;
    }

    void SetCache(std::shared_ptr<std::any> cacheData)
    {
        cacheAnyPtr_ = cacheData;
    }

    std::shared_ptr<std::any> GetCache() const
    {
        return cacheAnyPtr_;
    }

    const std::shared_ptr<Drawing::GEShaderShape> GetGEShaderShape(const std::string& tag) const;

    void SetSDFEdgeLightParams(const std::string& tag, float param);

private:
    /// Check if tag belongs to current params type's tag range
    bool IsTagValidForCurrentType(GEV2::GEParamsMemberTag tag) const
    {
        if (!params_) {
            return false;
        }

        auto tagType = GEV2::GEParamsMemberHelper::GetFilterTypeFromTag(tag);
        return tagType == params_->GetType();
    }

    /// Internal set param implementation - uses reflection metadata to set field value
    /// Non-template overloads for each type, generated from FOR_EACH_PARAM_TYPE
    /// Implementations in ge_visual_effect_impl_v2.cpp
#define DECLARE_SET_PARAM_INTERNAL(Type) \
    void SetParamInternal(GEV2::GEParamsMemberTag tag, const Type& value);

    FOR_EACH_PARAM_TYPE(DECLARE_SET_PARAM_INTERNAL)
#undef DECLARE_SET_PARAM_INTERNAL

    FilterType filterType_ = GEVisualEffectImplV2::FilterType::NONE;
    Drawing::CanvasInfo canvasInfo_;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;

    // Single type-erased params member replacing 60+ individual params_ members
    std::shared_ptr<GEV2::GEFilterParams> params_ = nullptr;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_V2_H
