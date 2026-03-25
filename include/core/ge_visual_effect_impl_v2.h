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
#include "effect/ge_value_transformer.h"

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
 * 3. Supports value transformation via GEParamsValueTransformer specialization
 * 4. String-based SetParam uses generated GEParamsMemberTagFromString mapping
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
    void SetParam(GEParamsMemberTag tag, T value)
    {
        if (!params_) {
            LOGE("GEVisualEffectImplV2::SetParam: params_ is null");
            return;
        }

        // Check if tag is valid for the current params type
        if (!IsTagValidForCurrentType(tag)) {
            LOGE("GEVisualEffectImplV2::SetParam: tag %u not valid for current params type", static_cast<uint32_t>(tag));
            return;
        }

        // Get field reference and set value with transformation
        SetParamInternal(tag, value);
    }

    /// Set parameter by string tag (runtime lookup to enum, then dispatch)
    template<typename T>
    void SetParam(const std::string& tagStr, T value)
    {
        auto tag = GEParamsMemberTagFromString(tagStr);
        if (tag != GEParamsMemberTag::INVALID) {
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
        params_ = GEFilterParams::Box(ParamsType{});
    }

    /// Get params as specific type (returns nullptr if type mismatch)
    template<typename ParamsType>
    std::shared_ptr<ParamsType> GetParams() const
    {
        return GEFilterParams::Unbox<ParamsType>(params_);
    }

    // ========================================================================
    // Backward-compatible convenience methods
    // ========================================================================

// #define GE_DECLARE_MAKE_GET_PARAMS(MethodName, ParamsType) \
//     void Make##MethodName() { MakeParams<ParamsType>(); } \
//     const std::shared_ptr<ParamsType>& Get##MethodName() const \
//     { return GetParams<ParamsType>(); }

//     GE_DECLARE_MAKE_GET_PARAMS(MESAParams, GEMESABlurShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(KawaseParams, GEKawaseBlurShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(AIBarParams, GEAIBarShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GreyParams, GEGreyShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(WaterRippleParams, GEWaterRippleFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(LinearGradientBlurParams, GELinearGradientBlurShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(MagnifierParams, GEMagnifierShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ColorGradientParams, GEColorGradientShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DisplacementDistortParams, GEDisplacementDistortFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SoundWaveParams, GESoundWaveFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(EdgeLightParams, GEEdgeLightShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(BezierWarpParams, GEBezierWarpShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DispersionParams, GEDispersionShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DirectionLightParams, GEDirectionLightShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ContentLightParams, GEContentLightFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ContenDiagonalParams, GEContentDiagonalFlowLightShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DotMatrixShaderParams, GEDotMatrixShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(WavyRippleLightParams, GEWavyRippleLightShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(AuroraNoiseParams, GEAuroraNoiseShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ParticleCircularHaloParams, GEParticleCircularHaloShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(VariableRadiusBlurParams, GEVariableRadiusBlurShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFEdgeLightParams, GESDFEdgeLightFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SdfFromImageParams, GESDFFromImageFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SdfBorderShaderParams, GESDFBorderShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SdfShadowShaderParams, GESDFShadowShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(RippleMaskParams, GERippleShaderMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DoubleRippleMaskParams, GEDoubleRippleShaderMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(WaveDisturbanceMaskParams, GEWaveDisturbanceShaderMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(RadialGradientMaskParams, GERadialGradientShaderMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(PixelMapMaskParams, GEPixelMapMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ImageMaskParams, GEImageMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(UseEffectMaskParams, GEUseEffectMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(WaveGradientMaskParams, GEWaveGradientShaderMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(MaskTransitionParams, GEMaskTransitionShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFUnionOpShapeParams, GESDFUnionOpShapeParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFRRectShapeParams, GESDFRRectShapeParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFPixelmapShapeParams, GESDFPixelmapShapeParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFTransformShapeParams, GESDFTransformShapeParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFClipShaderParams, GESDFClipShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(SDFColorShaderParams, GESDFColorShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(LightCaveParams, GEXLightCaveShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DistortChromaParams, GEXDistortChromaEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(DupoliNoiseMaskParams, GEXDupoliNoiseMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(BorderLightParams, GEBorderLightShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GasifyScaleTwistFilterParams, GEGasifyScaleTwistFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GasifyBlurFilterParams, GEGasifyBlurFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GasifyFilterParams, GEGasifyFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(ColorGradientEffectParams, GEXColorGradientEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(HarmoniumEffectParams, GEHarmoniumEffectShaderParams)
//     GE_DECLARE_MAKE_GET_PARAMS(AIBarGlowEffectParams, GEXAIBarGlowEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(AIBarRectHaloEffectParams, GEXAIBarRectHaloEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(RoundedRectFlowlightEffectParams, GEXRoundedRectFlowlightEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GradientFlowColorsEffectParams, GEXGradientFlowColorsEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(FrameGradientMaskParams, GEFrameGradientMaskParams)
//     GE_DECLARE_MAKE_GET_PARAMS(CircleFlowlightEffectParams, GECircleFlowlightEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(FrostedGlassEffectParams, GEFrostedGlassEffectParams)
//     GE_DECLARE_MAKE_GET_PARAMS(FrostedGlassBlurParams, GEFrostedGlassBlurShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(GridWarpFilterParams, GEGridWarpShaderFilterParams)
//     GE_DECLARE_MAKE_GET_PARAMS(NoisyFrameGradientMaskParams, GEXNoisyFrameGradientMaskParams)

// #undef GE_DECLARE_MAKE_GET_PARAMS

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

    std::shared_ptr<std::any> GetCache()
    {
        return cacheAnyPtr_;
    }

private:
    /// Check if tag belongs to current params type's tag range
    bool IsTagValidForCurrentType(GEParamsMemberTag tag) const
    {
        if (!params_) {
            return false;
        }

        auto tagType = GEParamsMemberHelper::GetFilterTypeFromTag(tag);
        return tagType == params_->GetType();
    }

    /// Internal set param implementation - uses reflection metadata to set field value
    /// Non-template overloads for each type, generated from FOR_EACH_PARAM_TYPE
    /// Implementations in ge_visual_effect_impl_v2.cpp
// #define DECLARE_SET_PARAM_INTERNAL(Type) \
//     void SetParamInternal(GEParamsMemberTag tag, const Type& value);

//     FOR_EACH_PARAM_TYPE(DECLARE_SET_PARAM_INTERNAL)
// #undef DECLARE_SET_PARAM_INTERNAL

    FilterType filterType_ = GEVisualEffectImplV2::FilterType::NONE;
    Drawing::CanvasInfo canvasInfo_;
    std::shared_ptr<std::any> cacheAnyPtr_ = nullptr;

    // Single type-erased params member replacing 60+ individual params_ members
    std::shared_ptr<GEFilterParams> params_ = nullptr;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_V2_H
