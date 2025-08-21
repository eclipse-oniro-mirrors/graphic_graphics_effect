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
#include "ge_visual_effect.h"

#include "ge_log.h"
#include "ge_visual_effect_impl.h"
#include "ge_pixel_map_shader_mask.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "ge_double_ripple_shader_mask.h"
#include "ge_wave_gradient_shader_mask.h"
#include "ge_frame_gradient_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffect::GEVisualEffect(
    const std::string& name, DrawingPaintType type, const std::optional<Drawing::CanvasInfo>& canvasInfo)
    : visualEffectName_(name), type_(type), visualEffectImpl_(std::make_unique<GEVisualEffectImpl>(name))
{
    canvasInfo_ = canvasInfo.value_or(Drawing::CanvasInfo());
}

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
        default:
            return nullptr;
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
