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
#include <vector>

#include "ge_double_ripple_shader_mask.h"
#include "ge_frame_gradient_shader_mask.h"
#include "ge_image_shader_mask.h"
#include "ge_use_effect_shader_mask.h"
#include "ge_log.h"
#include "ge_external_dynamic_loader.h"
#include "ge_pixel_map_shader_mask.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"
#include "ge_wave_gradient_shader_mask.h"
#include "sdf/ge_sdf_empty_shader_shape.h"
#include "sdf/ge_sdf_pixelmap_shader_shape.h"
#include "sdf/ge_sdf_rrect_shader_shape.h"
#include "sdf/ge_sdf_transform_shader_shape.h"
#include "sdf/ge_sdf_union_op_shader_shape.h"
#include "ge_external_dynamic_loader.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffect::GEVisualEffect(
    const std::string& name, DrawingPaintType type, const std::optional<Drawing::CanvasInfo>& canvasInfo)
    : visualEffectName_(name), type_(type), visualEffectImpl_(std::make_unique<GEVisualEffectImpl>(name, canvasInfo))
{}

GEVisualEffect::~GEVisualEffect() {}

void GEVisualEffect::SetParam(const std::string& tag, int32_t param)
{
    (void)type_;
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, int64_t param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, float param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, double param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const char* const param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::pair<float, float>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const Drawing::Matrix param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::vector<std::pair<float, float>>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::vector<Vector2f>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::vector<Vector4f>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::array<Drawing::Point, POINT_NUM>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, bool param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, uint32_t param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::vector<float>& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderMask> param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderShape> param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const Drawing::Color4f& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const Vector3f& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const Vector4f& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const GERRect& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const GESDFBorderParams& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const GESDFShadowParams& param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetCanvasInfo(Drawing::CanvasInfo info)
{
    visualEffectImpl_->SetCanvasInfo(info);
}

const Drawing::CanvasInfo& GEVisualEffect::GetCanvasInfo() const
{
    return visualEffectImpl_->GetCanvasInfo();
}

const std::shared_ptr<Drawing::GEShaderMask> GEVisualEffect::GenerateShaderMask() const
{
    auto impl = visualEffectImpl_;
    if (impl == nullptr) {
        return nullptr;
    }

    switch (impl->GetFilterType()) {
        case GEVisualEffectImpl::FilterType::RADIAL_GRADIENT_MASK: {
            auto radialParams = impl->GetRadialGradientMaskParams();
            if (radialParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GERadialGradientShaderMask>(*radialParams);
        }
        case GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK: {
            auto doubleRippleParams = impl->GetDoubleRippleMaskParams();
            if (doubleRippleParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEDoubleRippleShaderMask>(*doubleRippleParams);
        }
        case GEVisualEffectImpl::FilterType::RIPPLE_MASK: {
            auto rippleParams = impl->GetRippleMaskParams();
            if (rippleParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GERippleShaderMask>(*rippleParams);
        }
        case GEVisualEffectImpl::FilterType::IMAGE_MASK: {
            auto imageParams = impl->GetImageMaskParams();
            if (imageParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEImageShaderMask>(*imageParams);
        }
        case GEVisualEffectImpl::FilterType::USE_EFFECT_MASK: {
            auto useEffectParams = impl->GetUseEffectMaskParams();
            if (useEffectParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEUseEffectShaderMask>(*useEffectParams);
        }
        case GEVisualEffectImpl::FilterType::PIXEL_MAP_MASK: {
            auto pixelMapParams = impl->GetPixelMapMaskParams();
            if (pixelMapParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEPixelMapShaderMask>(*pixelMapParams);
        }
        case GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK: {
            auto waveParams = impl->GetWaveGradientMaskParams();
            if (waveParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEWaveGradientShaderMask>(*waveParams);
        }
        case GEVisualEffectImpl::FilterType::FRAME_GRADIENT_MASK: {
            auto frameParams = impl->GetFrameGradientMaskParams();
            if (frameParams == nullptr) {
                return nullptr;
            }
            return std::make_shared<GEFrameGradientShaderMask>(*frameParams);
        }
        case GEVisualEffectImpl::FilterType::DUPOLI_NOISE_MASK: {
            auto dupoliNoiseParams = impl->GetDupoliNoiseMaskParams();
            if (dupoliNoiseParams == nullptr) {
                return nullptr;
            }
            auto type = static_cast<uint32_t>(GEVisualEffectImpl::FilterType::DUPOLI_NOISE_MASK);
            auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(GEXDupoliNoiseMaskParams), static_cast<void*>(dupoliNoiseParams.get()));
            if (!impl) {
                GE_LOGE("GEXDupoliNoiseShaderMask::CreateDynamicImpl create object failed.");
                return nullptr;
            }
            std::shared_ptr<Drawing::GEShaderMask> gexShaderMask(static_cast<Drawing::GEShaderMask*>(impl));
            return gexShaderMask;
        }
        case GEVisualEffectImpl::FilterType::NOISY_FRAME_GRADIENT_MASK: {
            if (impl == nullptr) {
                return nullptr;
            }
            auto noisyFrameParams = impl->GetNoisyFrameGradientMaskParams();
            if (noisyFrameParams == nullptr) {
                return nullptr;
            }
            auto mask = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::NOISY_FRAME_GRADIENT_MASK),
                sizeof(Drawing::GEXNoisyFrameGradientMaskParams),
                static_cast<void *>(noisyFrameParams.get()));
            if (!mask) {
                return nullptr;
            }
            std::shared_ptr<GEShaderMask> maskShader(static_cast<GEShaderMask*>(mask));
            return maskShader;
        }
        default:
            return nullptr;
    }
}

const std::shared_ptr<Drawing::GEShaderShape> GEVisualEffect::GenerateShaderShape() const
{
    auto impl = visualEffectImpl_;
    if (impl == nullptr) {
        return nullptr;
    }

    switch (impl->GetFilterType()) {
        case GEVisualEffectImpl::FilterType::SDF_UNION_OP: {
            auto params = impl->GetSDFUnionOpShapeParams();
            if (params == nullptr) {
                return nullptr;
            }
            return std::make_shared<GESDFUnionOpShaderShape>(*params);
        }
        case GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE: {
            auto params = impl->GetSDFRRectShapeParams();
            if (params == nullptr) {
                return nullptr;
            }
            return GenerateExtShaderRRect(params);
        }
        case GEVisualEffectImpl::FilterType::SDF_TRANSFORM_SHAPE: {
            auto params = impl->GetSDFTransformShapeParams();
            if (params == nullptr) {
                return nullptr;
            }
            return std::make_shared<GESDFTransformShaderShape>(*params);
        }
        case GEVisualEffectImpl::FilterType::SDF_PIXELMAP_SHAPE: {
            auto params = impl->GetSDFPixelmapShapeParams();
            if (params == nullptr) {
                return nullptr;
            }
            return std::make_shared<GESDFPixelmapShaderShape>(*params);
        }
        case GEVisualEffectImpl::FilterType::SDF_EMPTY_SHAPE: {
            return std::make_shared<GESDFEmptyShaderShape>();
        }
        default:
            return nullptr;
    }
}

std::shared_ptr<Drawing::GEShaderShape> GEVisualEffect::GenerateExtShaderRRect(
    const std::shared_ptr<GESDFRRectShapeParams>& params) const
{
    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE),
        sizeof(Drawing::GESDFRRectShapeParams),
        static_cast<void *>(params.get()));
    if (!CanBeContinuous(params) || !object) {
        return std::make_shared<GESDFRRectShaderShape>(*params);
    }
    std::shared_ptr<Drawing::GEShaderShape> rrectShape(static_cast<Drawing::GEShaderShape*>(object));
    return rrectShape;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
