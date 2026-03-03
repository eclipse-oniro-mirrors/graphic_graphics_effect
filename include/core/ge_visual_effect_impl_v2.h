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
#include "ge_shader_filter_params.h"
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
            GE_LOGE("GEVisualEffectImplV2::SetParam: params_ is null");
            return;
        }

        // Check if tag is valid for the current params type
        if (!IsTagValidForCurrentType(tag)) {
            GE_LOGE("GEVisualEffectImplV2::SetParam: tag %u not valid for current params type", static_cast<uint32_t>(tag));
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
            GE_LOGE("GEVisualEffectImplV2::SetParam: unknown tag '%s'", tagStr.c_str());
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

    void MakeMESAParams() { MakeParams<GEMESABlurShaderFilterParams>(); }
    const std::shared_ptr<GEMESABlurShaderFilterParams>& GetMESAParams() const
    { return GetParams<GEMESABlurShaderFilterParams>(); }

    void MakeKawaseParams() { MakeParams<GEKawaseBlurShaderFilterParams>(); }
    const std::shared_ptr<GEKawaseBlurShaderFilterParams>& GetKawaseParams() const
    { return GetParams<GEKawaseBlurShaderFilterParams>(); }

    void MakeAIBarParams() { MakeParams<GEAIBarShaderFilterParams>(); }
    const std::shared_ptr<GEAIBarShaderFilterParams>& GetAIBarParams() const
    { return GetParams<GEAIBarShaderFilterParams>(); }

    void MakeGreyParams() { MakeParams<GEGreyShaderFilterParams>(); }
    const std::shared_ptr<GEGreyShaderFilterParams>& GetGreyParams() const
    { return GetParams<GEGreyShaderFilterParams>(); }

    void MakeWaterRippleParams() { MakeParams<GEWaterRippleFilterParams>(); }
    const std::shared_ptr<GEWaterRippleFilterParams>& GetWaterRippleParams() const
    { return GetParams<GEWaterRippleFilterParams>(); }

    void MakeLinearGradientBlurParams() { MakeParams<GELinearGradientBlurShaderFilterParams>(); }
    const std::shared_ptr<GELinearGradientBlurShaderFilterParams>& GetLinearGradientBlurParams() const
    { return GetParams<GELinearGradientBlurShaderFilterParams>(); }

    void MakeMagnifierParams() { MakeParams<GEMagnifierShaderFilterParams>(); }
    const std::shared_ptr<GEMagnifierShaderFilterParams>& GetMagnifierParams() const
    { return GetParams<GEMagnifierShaderFilterParams>(); }

    void MakeColorGradientParams() { MakeParams<GEColorGradientShaderFilterParams>(); }
    const std::shared_ptr<GEColorGradientShaderFilterParams>& GetColorGradientParams() const
    { return GetParams<GEColorGradientShaderFilterParams>(); }

    void MakeDisplacementDistortParams() { MakeParams<GEDisplacementDistortFilterParams>(); }
    const std::shared_ptr<GEDisplacementDistortFilterParams>& GetDisplacementDistortParams() const
    { return GetParams<GEDisplacementDistortFilterParams>(); }

    void MakeSoundWaveParams() { MakeParams<GESoundWaveFilterParams>(); }
    const std::shared_ptr<GESoundWaveFilterParams>& GetSoundWaveParams() const
    { return GetParams<GESoundWaveFilterParams>(); }

    void MakeEdgeLightParams() { MakeParams<GEEdgeLightShaderFilterParams>(); }
    const std::shared_ptr<GEEdgeLightShaderFilterParams>& GetEdgeLightParams() const
    { return GetParams<GEEdgeLightShaderFilterParams>(); }

    void MakeBezierWarpParams() { MakeParams<GEBezierWarpShaderFilterParams>(); }
    const std::shared_ptr<GEBezierWarpShaderFilterParams>& GetBezierWarpParams() const
    { return GetParams<GEBezierWarpShaderFilterParams>(); }

    void MakeDispersionParams() { MakeParams<GEDispersionShaderFilterParams>(); }
    const std::shared_ptr<GEDispersionShaderFilterParams>& GetDispersionParams() const
    { return GetParams<GEDispersionShaderFilterParams>(); }

    void MakeDirectionLightParams() { MakeParams<GEDirectionLightShaderFilterParams>(); }
    const std::shared_ptr<GEDirectionLightShaderFilterParams>& GetDirectionLightParams() const
    { return GetParams<GEDirectionLightShaderFilterParams>(); }

    void MakeContentLightParams() { MakeParams<GEContentLightFilterParams>(); }
    const std::shared_ptr<GEContentLightFilterParams>& GetContentLightParams() const
    { return GetParams<GEContentLightFilterParams>(); }

    void MakeContentDiagonalParams() { MakeParams<GEContentDiagonalFlowLightShaderParams>(); }
    const std::shared_ptr<GEContentDiagonalFlowLightShaderParams>& GetContenDiagonalParams() const
    { return GetParams<GEContentDiagonalFlowLightShaderParams>(); }

    void MakeDotMatrixShaderParams() { MakeParams<GEDotMatrixShaderParams>(); }
    const std::shared_ptr<GEDotMatrixShaderParams>& GetDotMatrixShaderParams() const
    { return GetParams<GEDotMatrixShaderParams>(); }

    void MakeWavyRippleLightParams() { MakeParams<GEWavyRippleLightShaderParams>(); }
    const std::shared_ptr<GEWavyRippleLightShaderParams>& GetWavyRippleLightParams() const
    { return GetParams<GEWavyRippleLightShaderParams>(); }

    void MakeAuroraNoiseParams() { MakeParams<GEAuroraNoiseShaderParams>(); }
    const std::shared_ptr<GEAuroraNoiseShaderParams>& GetAuroraNoiseParams() const
    { return GetParams<GEAuroraNoiseShaderParams>(); }

    void MakeParticleCircularHaloParams() { MakeParams<GEParticleCircularHaloShaderParams>(); }
    const std::shared_ptr<GEParticleCircularHaloShaderParams>& GetParticleCircularHaloParams() const
    { return GetParams<GEParticleCircularHaloShaderParams>(); }

    void MakeVariableRadiusBlurParams() { MakeParams<GEVariableRadiusBlurShaderFilterParams>(); }
    const std::shared_ptr<GEVariableRadiusBlurShaderFilterParams>& GetVariableRadiusBlurParams() const
    { return GetParams<GEVariableRadiusBlurShaderFilterParams>(); }

    void MakeSdfEdgeLightParams() { MakeParams<GESDFEdgeLightFilterParams>(); }
    const std::shared_ptr<GESDFEdgeLightFilterParams>& GetSDFEdgeLightParams() const
    { return GetParams<GESDFEdgeLightFilterParams>(); }

    void MakeSdfFromImageParams() { MakeParams<GESDFFromImageFilterParams>(); }
    const std::shared_ptr<GESDFFromImageFilterParams>& GetSdfFromImageParams() const
    { return GetParams<GESDFFromImageFilterParams>(); }

    void MakeSdfBorderParams() { MakeParams<GESDFBorderShaderParams>(); }
    const std::shared_ptr<GESDFBorderShaderParams>& GetSdfBorderShaderParams() const
    { return GetParams<GESDFBorderShaderParams>(); }

    void MakeSdfShadowParams() { MakeParams<GESDFShadowShaderParams>(); }
    const std::shared_ptr<GESDFShadowShaderParams>& GetSdfShadowShaderParams() const
    { return GetParams<GESDFShadowShaderParams>(); }

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

        auto tagRange = GEParamsMemberHelper::GetParamsMemberTagRange(params_->GetType());
        if (!tagRange) {
            return false;
        }

        // Check if tag is in range [RangeStart, RangeEnd]
        uint32_t tagValue = static_cast<uint32_t>(tag);
        uint32_t startValue = static_cast<uint32_t>(tagRange->RangeStart);
        uint32_t endValue = static_cast<uint32_t>(tagRange->RangeEnd);
        return tagValue >= startValue && tagValue <= endValue;
    }

    /// Internal set param implementation - uses reflection metadata to set field value
    template<typename T>
    void SetParamInternal(GEParamsMemberTag tag, T value)
    {
        if (!params_) {
            return;
        }

        // Dispatch based on filter type using generated high-order template
        // This will automatically update when metadata is regenerated
        DispatchGEFilterType<SetParamByTagHelper, T>(params_->GetType(), params_, tag, value);
    }

    /// Helper functor for DispatchGEFilterType - sets param by tag with value transformation
    template<typename ParamsType, typename ValueType>
    struct SetParamByTagHelper {
        static void Invoke(const std::shared_ptr<GEFilterParams>& boxedParams,
                          GEParamsMemberTag tag, const ValueType& value)
        {
            auto specificParams = GEFilterParams::Unbox<ParamsType>(boxedParams);
            if (!specificParams) {
                return;
            }

            // Apply value transformation using tag-based transformer
            using Transformer = GEParamsValueTransformer<GEParamsMemberTag(tag), ValueType>;
            auto transformed = Transformer::Transform(value);

            // Set the field using the generated reflection helper
            GEParamsMemberHelper::SetParamsMemberByTag(*specificParams, tag, transformed);
        }
    };

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
