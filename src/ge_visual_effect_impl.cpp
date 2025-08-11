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
#include <map>

#include "ge_shader_filter_params.h"
#include "ge_visual_effect_impl.h"
#include "ge_log.h"
#include "ge_external_dynamic_loader.h"
#include "common/rs_vector4.h"
#include "common/rs_vector3.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> GEVisualEffectImpl::g_initialMap = {
    { GE_FILTER_KAWASE_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::KAWASE_BLUR);
            impl->MakeKawaseParams();
        }
    },
    { GE_FILTER_MESA_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::MESA_BLUR);
            impl->MakeMESAParams();
        }
    },
    { GE_FILTER_GREY,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GREY);
            impl->MakeGreyParams();
        }
    },
    { GE_FILTER_AI_BAR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AIBAR);
            impl->MakeAIBarParams();
        }
    },
    { GE_FILTER_LINEAR_GRADIENT_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
            impl->MakeLinearGradientBlurParams();
        }
    },
    { GE_FILTER_MAGNIFIER,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::MAGNIFIER);
            impl->MakeMagnifierParams();
        }
    },
    { GE_FILTER_WATER_RIPPLE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::WATER_RIPPLE);
            impl->MakeWaterRippleParams();
        }
    },
    { GE_FILTER_COLOR_GRADIENT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::COLOR_GRADIENT);
            impl->MakeColorGradientParams();
        }
    },
    { GE_FILTER_DISPLACEMENT_DISTORT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DISPLACEMENT_DISTORT_FILTER);
            impl->MakeDisplacementDistortParams();
        }
    },
    { GE_FILTER_SOUND_WAVE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SOUND_WAVE);
            impl->MakeSoundWaveParams();
        }
    },
    { GE_FILTER_EDGE_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::EDGE_LIGHT);
            impl->MakeEdgeLightParams();
        }
    },
    { GE_FILTER_BEZIER_WARP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::BEZIER_WARP);
            impl->MakeBezierWarpParams();
        }
    },
    { GE_FILTER_DISPERSION,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DISPERSION);
            impl->MakeDispersionParams();
        }
    },
    { GE_FILTER_CONTENT_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::CONTENT_LIGHT);
            impl->MakeContentLightParams();
        }
    },
    { GE_FILTER_DIRECTION_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DIRECTION_LIGHT);
            impl->MakeDirectionLightParams();
        }
    },
    { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT);
            impl->MakeContentDiagonalParams();
        }
    },
    { GE_SHADER_WAVY_RIPPLE_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::WAVY_RIPPLE_LIGHT);
            impl->MakeWavyRippleLightParams();
        }
    },
    { GE_SHADER_AURORA_NOISE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AURORA_NOISE);
            impl->MakeAuroraNoiseParams();
        }
    },
    { GE_MASK_RIPPLE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::RIPPLE_MASK);
            impl->MakeRippleMaskParams();
        }
    },
    { GE_MASK_DOUBLE_RIPPLE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DOUBLE_RIPPLE_MASK);
            impl->MakeDoubleRippleMaskParams();
        }
    },
    { GE_MASK_RADIAL_GRADIENT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::RADIAL_GRADIENT_MASK);
            impl->MakeRadialGradientMaskParams();
        }
    },
    { GE_MASK_PIXEL_MAP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::PIXEL_MAP_MASK);
            impl->MakePixelMapMaskParams();
        }
    },
    { GE_MASK_WAVE_GRADIENT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::WAVE_GRADIENT_MASK);
            impl->MakeWaveGradientMaskParams();
        }
    },
    { GE_SHADER_PARTICLE_CIRCULAR_HALO,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::PARTICLE_CIRCULAR_HALO);
            impl->MakeParticleCircularHaloParams();
        }
    },
    { GE_FILTER_MASK_TRANSITION,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::MASK_TRANSITION);
            impl->MakeMaskTransitionParams();
        }
    },
    { GE_FILTER_VARIABLE_RADIUS_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::VARIABLE_RADIUS_BLUR);
            impl->MakeVariableRadiusBlurParams();
        }
    },
    { GEX_SHADER_COLOR_GRADIENT_EFFECT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::COLOR_GRADIENT_EFFECT);
            impl->MakeColorGradientEffectParams();
        }
    },
    { GE_SHADER_BORDER_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::BORDER_LIGHT);
            impl->MakeBorderLightParams();
        }
    },
};

GEVisualEffectImpl::GEVisualEffectImpl(const std::string& name)
{
    auto iter = g_initialMap.find(name);
    if (iter != g_initialMap.end()) {
        iter->second(this);
    }
}

GEVisualEffectImpl::~GEVisualEffectImpl() {}

void GEVisualEffectImpl::SetParam(const std::string& tag, int32_t param)
{
    switch (filterType_) {
        case FilterType::KAWASE_BLUR: {
            if (kawaseParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_KAWASE_BLUR_RADIUS) {
                kawaseParams_->radius = param;
            }
            break;
        }
        case FilterType::MESA_BLUR: {
            if (mesaParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_MESA_BLUR_RADIUS) {
                mesaParams_->radius = param;
            }
            if (tag == GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE) {
                mesaParams_->tileMode = param;
            }
            break;
        }
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION) {
                linearGradientBlurParams_->direction = param;
            }
            break;
        }
        case FilterType::MAGNIFIER: {
            if (magnifierParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_MAGNIFIER_ROTATE_DEGREE) {
                magnifierParams_->rotateDegree = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, bool param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN) {
                linearGradientBlurParams_->isOffscreenCanvas = param;
            }
            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_IS_RADIUS_GRADIENT) {
                linearGradientBlurParams_->isRadiusGradient = param;
            }

            break;
        }
        case FilterType::EDGE_LIGHT: {
            if (edgeLightParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR) {
                edgeLightParams_->useRawColor = param;
            }
            if (tag == GE_FILTER_EDGE_LIGHT_BLOOM) {
                edgeLightParams_->bloom = param;
            }
            break;
        }
        case FilterType::MASK_TRANSITION: {
            if (maskTransitionParams_ == nullptr) {
                GE_LOGE("GEVisualEffectImpl set inverse failed, maskTransitionParams is nullptr");
                return;
            }
            if (tag == GE_FILTER_MASK_TRANSITION_INVERSE) {
                maskTransitionParams_->inverse = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, int64_t param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, float param)
{
    switch (filterType_) {
        case FilterType::MESA_BLUR: {
            SetMESABlurParams(tag, param);
            break;
        }
        case FilterType::AIBAR: {
            SetAIBarParams(tag, param);
            break;
        }
        case FilterType::GREY: {
            SetGreyParams(tag, param);
            break;
        }

        case FilterType::LINEAR_GRADIENT_BLUR: {
            SetLinearGradientBlurParams(tag, param);
            break;
        }
        case FilterType::MAGNIFIER: {
            SetMagnifierParamsFloat(tag, param);
            break;
        }
        case FilterType::WATER_RIPPLE: {
            SetWaterRippleParams(tag, param);
            break;
        }
        case FilterType::RIPPLE_MASK : {
            SetRippleMaskParamsFloat(tag, param);
            break;
        }
        case FilterType::DOUBLE_RIPPLE_MASK : {
            SetDoubleRippleMaskParamsFloat(tag, param);
            break;
        }
        case FilterType::RADIAL_GRADIENT_MASK: {
            SetRadialGradientMaskParamsFloat(tag, param);
            break;
        }
        case FilterType::WAVE_GRADIENT_MASK : {
            SetWaveGradientMaskParamsFloat(tag, param);
            break;
        }
        case FilterType::SOUND_WAVE: {
            SetSoundWaveParams(tag, param);
            break;
        }
        case FilterType::EDGE_LIGHT: {
            SetEdgeLightParams(tag, param);
            break;
        }
        case FilterType::DISPERSION: {
            SetDispersionParams(tag, param);
            break;
        }
        case FilterType::CONTENT_LIGHT: {
            SetContentLightParams(tag, param);
            break;
        }
        case FilterType::DIRECTION_LIGHT: {
            SetDirectionLightParamsFloat(tag, param);
            break;
        }
        case FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT: {
            SetContentDiagonalFlowParams(tag, param);
            break;
        }
        case FilterType::WAVY_RIPPLE_LIGHT: {
            SetWavyRippleLightParams(tag, param);
            break;
        }
        case FilterType::AURORA_NOISE: {
            SetAuroraNoiseParams(tag, param);
            break;
        }
        case FilterType::PARTICLE_CIRCULAR_HALO: {
            SetParticleCircularHaloParams(tag, param);
            break;
        }
        case FilterType::MASK_TRANSITION: {
            SetMaskTransitionParamsFloat(tag, param);
            break;
        }
        case FilterType::VARIABLE_RADIUS_BLUR: {
            SetVariableRadiusBlurParams(tag, param);
            break;
        }
        case FilterType::COLOR_GRADIENT_EFFECT: {
            SetColorGradientEffectParams(tag, param);
            break;
        }
        case FilterType::BORDER_LIGHT: {
            SetBorderLightParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, double param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const char* const param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param)
{
    switch (filterType_) {
        case FilterType::PIXEL_MAP_MASK: {
            if (pixelMapMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_PIXEL_MAP_PIXEL_MAP) {
                pixelMapMaskParams_->image = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Drawing::Matrix param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT) {
                linearGradientBlurParams_->mat = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::pair<float, float>& param)
{
    switch (filterType_) {
        case FilterType::RIPPLE_MASK: {
            if (rippleMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_RIPPLE_CENTER) {
                rippleMaskParams_->center_ = param;
            }
            break;
        }
        case FilterType::DOUBLE_RIPPLE_MASK: {
            if (doubleRippleMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_DOUBLE_RIPPLE_CENTER1) {
                doubleRippleMaskParams_->center1_ = param;
            }
            if (tag == GE_MASK_DOUBLE_RIPPLE_CENTER2) {
                doubleRippleMaskParams_->center2_ = param;
            }
            break;
        }
        case FilterType::DISPLACEMENT_DISTORT_FILTER: {
            if (displacementDistortParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_DISPLACEMENT_DISTORT_FACTOR) {
                displacementDistortParams_->factor_ = param;
            }
            break;
        }
        case FilterType::RADIAL_GRADIENT_MASK: {
            if (radialGradientMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_RADIAL_GRADIENT_CENTER) {
                radialGradientMaskParams_->center_ = param;
            }
            break;
        }
        case FilterType::WAVY_RIPPLE_LIGHT: {
            SetWavyRippleLightParams(tag, param);
            break;
        }
        case FilterType::WAVE_GRADIENT_MASK: {
            if (waveGradientMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_WAVE_GRADIENT_CENTER) {
                waveGradientMaskParams_->center_ = param;
            }
            break;
        }
        case FilterType::PARTICLE_CIRCULAR_HALO: {
            SetParticleCircularHaloParams(tag, param);
            break;
        }
        case FilterType::DISPERSION: {
            SetDispersionParams(tag, param);
            break;
        }
        case FilterType::BEZIER_WARP: {
            SetBezierWarpParams(tag, param);
            break;
        }
        case FilterType::COLOR_GRADIENT_EFFECT: {
            SetColorGradientEffectParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<std::pair<float, float>>& param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS) {
                linearGradientBlurParams_->fractionStops = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<Vector2f>& param)
{
    switch (filterType_) {
        case FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT: {
            if (contentDiagonalParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_CONTOUR) {
                contentDiagonalParams_->contour_ = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::array<Drawing::Point, POINT_NUM>& param)
{
    switch (filterType_) {
        case FilterType::BEZIER_WARP: {
            if (bezierWarpParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_BEZIER_WARP_DESTINATION_PATCH) {
                bezierWarpParams_->destinationPatch = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<float>& param)
{
    switch (filterType_) {
        case FilterType::COLOR_GRADIENT: {
            if (colorGradientParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_COLOR_GRADIENT_COLOR) {
                colorGradientParams_->colors = param;
            }
            if (tag == GE_FILTER_COLOR_GRADIENT_POSITION) {
                colorGradientParams_->positions = param;
            }
            if (tag == GE_FILTER_COLOR_GRADIENT_STRENGTH) {
                colorGradientParams_->strengths = param;
            }
            break;
        }
        case FilterType::RADIAL_GRADIENT_MASK: {
            if (radialGradientMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_RADIAL_GRADIENT_COLORS) {
                radialGradientMaskParams_->colors_ = param;
            }
            if (tag == GE_MASK_RADIAL_GRADIENT_POSITIONS) {
                radialGradientMaskParams_->positions_ = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const uint32_t param)
{
    switch (filterType_) {
        case FilterType::MAGNIFIER: {
            SetMagnifierParamsUint32(tag, param);
            break;
        }
        case FilterType::WATER_RIPPLE: {
            if (waterRippleParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_WATER_RIPPLE_RIPPLE_MODE) {
                waterRippleParams_->rippleMode = param;
            } else if (tag == GE_FILTER_WATER_RIPPLE_WAVE_NUM) {
                waterRippleParams_->waveCount = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderMask> param)
{
    switch (filterType_) {
        case FilterType::DISPLACEMENT_DISTORT_FILTER: {
            if (displacementDistortParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_DISPLACEMENT_DISTORT_MASK) {
                displacementDistortParams_->mask_ = param;
            }
            break;
        }
        case FilterType::COLOR_GRADIENT: {
            if (colorGradientParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_COLOR_GRADIENT_MASK) {
                colorGradientParams_->mask = param;
            }
            break;
        }
        case FilterType::EDGE_LIGHT: {
            if (edgeLightParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_EDGE_LIGHT_MASK) {
                edgeLightParams_->mask = param;
            }
            break;
        }
        case FilterType::DISPERSION: {
            if (dispersionParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_DISPERSION_MASK) {
                dispersionParams_->mask = param;
            }
            break;
        }
        case FilterType::DIRECTION_LIGHT: {
            if (directionLightParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_DIRECTION_LIGHT_MASK) {
                directionLightParams_->mask = param;
            }
            break;
        }
        case FilterType::MASK_TRANSITION: {
            if (maskTransitionParams_ == nullptr) {
                GE_LOGE("GEVisualEffectImpl set mask failed, maskTransitionParams is nullptr");
                return;
            }

            if (tag == GE_FILTER_MASK_TRANSITION_MASK) {
                maskTransitionParams_->mask = param;
            }
            break;
        }
        case FilterType::VARIABLE_RADIUS_BLUR: {
            if (variableRadiusBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_VARIABLE_RADIUS_BLUR_MASK) {
                variableRadiusBlurParams_->mask = param;
            }
            break;
        }
        case FilterType::COLOR_GRADIENT_EFFECT: {
            if (colorGradientEffectParams_ == nullptr) {
                return;
            }

            if (tag == GEX_SHADER_COLOR_GRADIENT_EFFECT_MASK) {
                colorGradientEffectParams_->mask_ = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Vector3f& param)
{
    switch (filterType_) {
        case FilterType::CONTENT_LIGHT: {
            if (contentLightParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_CONTENT_LIGHT_POSITION) {
                contentLightParams_->position = param;
            }
            if (tag == GE_FILTER_CONTENT_LIGHT_ROTATION_ANGLE) {
                contentLightParams_->rotationAngle = param;
            }
            break;
        }
        case FilterType::DIRECTION_LIGHT: {
            if (directionLightParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_DIRECTION_LIGHT_DIRECTION) {
                directionLightParams_->lightDirection = param;
            }
            break;
        }
        case FilterType::BORDER_LIGHT: {
            SetBorderLightParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Vector4f& param)
{
    switch (filterType_) {
        case FilterType::CONTENT_LIGHT: {
            if (contentLightParams_ != nullptr && tag == GE_FILTER_CONTENT_LIGHT_COLOR) {
                contentLightParams_->color = param;
            }
            break;
        }
        case FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT: {
            SetContentDiagonalFlowParams(tag, param);
            break;
        }
        case FilterType::PIXEL_MAP_MASK: {
            if (pixelMapMaskParams_ == nullptr) {
                return;
            }
            SetPixelMapMaskParams(tag, param);
            break;
        }
        case FilterType::DIRECTION_LIGHT: {
            if (directionLightParams_ != nullptr && tag == GE_FILTER_DIRECTION_LIGHT_COLOR) {
                directionLightParams_->lightColor = param;
            }
            break;
        }
        case FilterType::EDGE_LIGHT: {
            if (edgeLightParams_ != nullptr && tag == GE_FILTER_EDGE_LIGHT_COLOR) {
                edgeLightParams_->color = param;
            }
            break;
        }
        case FilterType::COLOR_GRADIENT_EFFECT: {
            SetColorGradientEffectParams(tag, param);
            break;
        }
        case FilterType::SOUND_WAVE: {
            Drawing::Color4f color {param.x_, param.y_, param.z_, param.w_};
            SetSoundWaveParams(tag, color);
            break;
        }
        case FilterType::BORDER_LIGHT: {
            SetBorderLightParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const RectF& param)
{
    switch (filterType_) {
        case FilterType::PIXEL_MAP_MASK: {
            SetPixelMapMaskParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetPixelMapMaskParams(const std::string& tag, const RectF& param)
{
    if (pixelMapMaskParams_ == nullptr) {
        return;
    }
    if (tag == GE_MASK_PIXEL_MAP_SRC) {
        pixelMapMaskParams_->src = param;
    }
    if (tag == GE_MASK_PIXEL_MAP_DST) {
        pixelMapMaskParams_->dst = param;
    }
}

void GEVisualEffectImpl::SetPixelMapMaskParams(const std::string& tag, const Vector4f& param)
{
    if (pixelMapMaskParams_ == nullptr) {
        return;
    }
    if (tag == GE_MASK_PIXEL_MAP_SRC) {
        Drawing::RectF rect(param.x_, param.y_, param.z_, param.w_);
        pixelMapMaskParams_->src = rect;
    }
    if (tag == GE_MASK_PIXEL_MAP_DST) {
        Drawing::RectF rect(param.x_, param.y_, param.z_, param.w_);
        pixelMapMaskParams_->dst = rect;
    }
    if (tag == GE_MASK_PIXEL_MAP_FILL_COLOR) {
        pixelMapMaskParams_->fillColor = param;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Drawing::Color4f& param)
{
    switch (filterType_) {
        case FilterType::SOUND_WAVE: {
            SetSoundWaveParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetMESABlurParams(const std::string& tag, float param)
{
    if (mesaParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_MESA_BLUR_GREY_COEF_1,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->greyCoef1 = p; } },
        { GE_FILTER_MESA_BLUR_GREY_COEF_2,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->greyCoef2 = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetX = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetY = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetZ = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetW = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->width = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->height = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetAIBarParams(const std::string& tag, float param)
{
    if (aiBarParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_AI_BAR_LOW,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarLow        = p; } },
        { GE_FILTER_AI_BAR_HIGH,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarHigh       = p; } },
        { GE_FILTER_AI_BAR_THRESHOLD,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarThreshold  = p; } },
        { GE_FILTER_AI_BAR_OPACITY,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarOpacity    = p; } },
        { GE_FILTER_AI_BAR_SATURATION,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarSaturation = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetGreyParams(const std::string& tag, float param)
{
    if (greyParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_GREY_COEF_1, [](GEVisualEffectImpl* obj, float p) { obj->greyParams_->greyCoef1 = p; } },
        { GE_FILTER_GREY_COEF_2, [](GEVisualEffectImpl* obj, float p) { obj->greyParams_->greyCoef2 = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetLinearGradientBlurParams(const std::string& tag, float param)
{
    if (linearGradientBlurParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->blurRadius = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->geoWidth   = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->geoHeight  = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->tranX      = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->tranY      = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetMagnifierParamsFloat(const std::string& tag, float param)
{
    if (magnifierParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_MAGNIFIER_FACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->factor = p; } },
        { GE_FILTER_MAGNIFIER_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->width = p; } },
        { GE_FILTER_MAGNIFIER_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->height = p; } },
        { GE_FILTER_MAGNIFIER_CORNER_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->cornerRadius = p; } },
        { GE_FILTER_MAGNIFIER_BORDER_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->borderWidth = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowOffsetX = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowOffsetY = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_SIZE,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowSize = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_STRENGTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowStrength = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetWaveGradientMaskParamsFloat(const std::string& tag, float param)
{
    if (waveGradientMaskParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_MASK_WAVE_GRADIENT_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->waveGradientMaskParams_->width_ = p; } },
        { GE_MASK_WAVE_GRADIENT_PROPAGATION_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->waveGradientMaskParams_->propagationRadius_ = p; } },
        { GE_MASK_WAVE_GRADIENT_BLUR_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->waveGradientMaskParams_->blurRadius_ = p; } },
        { GE_MASK_WAVE_GRADIENT_TURBULENCE_STRENGTH,
            [](GEVisualEffectImpl* obj, float p) { obj->waveGradientMaskParams_->turbulenceStrength_ = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetDoubleRippleMaskParamsFloat(const std::string& tag, float param)
{
    if (doubleRippleMaskParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_MASK_DOUBLE_RIPPLE_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->doubleRippleMaskParams_->radius_ = p; } },
        { GE_MASK_DOUBLE_RIPPLE_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->doubleRippleMaskParams_->width_ = p; } },
        { GE_MASK_DOUBLE_RIPPLE_TURBULENCE,
            [](GEVisualEffectImpl* obj, float p) { obj->doubleRippleMaskParams_->turbulence_ = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetRippleMaskParamsFloat(const std::string& tag, float param)
{
    if (rippleMaskParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_MASK_RIPPLE_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->rippleMaskParams_->radius_ = p; } },
        { GE_MASK_RIPPLE_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->rippleMaskParams_->width_ = p; } },
        { GE_MASK_RIPPLE_WIDTH_CENTER_OFFSET,
            [](GEVisualEffectImpl* obj, float p) { obj->rippleMaskParams_->widthCenterOffset_ = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetRadialGradientMaskParamsFloat(const std::string& tag, float param)
{
    if (radialGradientMaskParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_MASK_RADIAL_GRADIENT_RADIUSX,
            [](GEVisualEffectImpl* obj, float p) { obj->radialGradientMaskParams_->radiusX_ = p; } },
        { GE_MASK_RADIAL_GRADIENT_RADIUSY,
            [](GEVisualEffectImpl* obj, float p) { obj->radialGradientMaskParams_->radiusY_ = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetMagnifierParamsUint32(const std::string& tag, uint32_t param)
{
    if (magnifierParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, uint32_t)>> actions = {
        { GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_1,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->gradientMaskColor1 = p; } },
        { GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_2,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->gradientMaskColor2 = p; } },
        { GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_1,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->outerContourColor1 = p; } },
        { GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_2,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->outerContourColor2 = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetWaterRippleParams(const std::string& tag, float param)
{
    if (waterRippleParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {

        { GE_FILTER_WATER_RIPPLE_PROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->progress = p; } },
        { GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->rippleCenterX = p; } },
        { GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->rippleCenterY = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetSoundWaveParams(const std::string& tag, float param)
{
    if (soundWaveParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {

        { GE_FILTER_SOUND_WAVE_COLORPROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->colorProgress = p; } },
        { GE_FILTER_SOUND_WAVE_SOUNDINTENSITY,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->soundIntensity = p; } },
        { GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->shockWaveAlphaA = p; } },
        { GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->shockWaveAlphaB = p; } },
        { GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->shockWaveProgressA = p; } },
        { GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->shockWaveProgressB = p; } },
        { GE_FILTER_SOUND_WAVE_TOTAL_ALPHA,
            [](GEVisualEffectImpl* obj, float p) { obj->soundWaveParams_->shockWaveTotalAlpha = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetSoundWaveParams(const std::string& tag, const Drawing::Color4f& param)
{
    if (soundWaveParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, const Drawing::Color4f&)>>
        actions = {
            { GE_FILTER_SOUND_WAVE_COLOR_A,
                [](GEVisualEffectImpl* obj, const Drawing::Color4f& p) { obj->soundWaveParams_->colorA = p; } },
            { GE_FILTER_SOUND_WAVE_COLOR_B,
                [](GEVisualEffectImpl* obj, const Drawing::Color4f& p) { obj->soundWaveParams_->colorB = p; } },
            { GE_FILTER_SOUND_WAVE_COLOR_C,
                [](GEVisualEffectImpl* obj, const Drawing::Color4f& p) { obj->soundWaveParams_->colorC = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetEdgeLightParams(const std::string& tag, float param)
{
    if (edgeLightParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_EDGE_LIGHT_ALPHA,
            [](GEVisualEffectImpl* obj, float p) { obj->edgeLightParams_->alpha = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetDispersionParams(const std::string& tag, float param)
{
    if (dispersionParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_DISPERSION_OPACITY,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->opacity = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetDispersionParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (dispersionParams_ == nullptr) {
        return;
    }

    if (tag == GE_FILTER_DISPERSION_RED_OFFSET) {
        dispersionParams_->redOffset = param;
    } else if (tag == GE_FILTER_DISPERSION_GREEN_OFFSET) {
        dispersionParams_->greenOffset = param;
    } else if (tag == GE_FILTER_DISPERSION_BLUE_OFFSET) {
        dispersionParams_->blueOffset = param;
    }
}

void GEVisualEffectImpl::SetBezierWarpParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (bezierWarpParams_ == nullptr) {
        return;
    }

    std::array<const char*, GE_FILTER_BEZIER_WARP_POINT_NUM> ctrlPointsName = {
        GE_FILTER_BEZIER_WARP_CONTROL_POINT0,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT1,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT2,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT3,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT4,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT5,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT6,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT7,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT8,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT9,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT10,
        GE_FILTER_BEZIER_WARP_CONTROL_POINT11,
    };

    auto it = std::find(ctrlPointsName.begin(), ctrlPointsName.end(), tag);
    if (it != ctrlPointsName.end()) {
        size_t index = std::distance(ctrlPointsName.begin(), it);
        bezierWarpParams_->destinationPatch[index].Set(param.first, param.second);
    }
}

void GEVisualEffectImpl::SetContentLightParams(const std::string& tag, float param)
{
    if (contentLightParams_ == nullptr) {
        return;
    }
 
    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_CONTENT_LIGHT_INTENSITY,
            [](GEVisualEffectImpl* obj, float p) { obj->contentLightParams_->intensity = p; } },
    };
 
    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetDirectionLightParamsFloat(const std::string& tag, float param)
{
    if (directionLightParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_DIRECTION_LIGHT_INTENSITY,
            [](GEVisualEffectImpl* obj, float p) { obj->directionLightParams_->lightIntensity = p; } },
        { GE_FILTER_DIRECTION_LIGHT_FACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->directionLightParams_->maskFactor = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetContentDiagonalFlowParams(const std::string& tag, const Vector4f& param)
{
    if (contentDiagonalParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_COLOR) {
        contentDiagonalParams_->line1Color_ = param;
    }
    if (tag == GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_COLOR) {
        contentDiagonalParams_->line2Color_ = param;
    }
}

void GEVisualEffectImpl::SetContentDiagonalFlowParams(const std::string& tag, float param)
{
    if (contentDiagonalParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_START,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->line1Start_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE1_LENGTH,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->line1Length_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_START,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->line2Start_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LINE2_LENGTH,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->line2Length_ = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetWavyRippleLightParams(const std::string& tag, float param)
{
    if (wavyRippleLightParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_WAVY_RIPPLE_LIGHT_RIADIUS) {
        wavyRippleLightParams_->radius_ = param;
    }
    if (tag == GE_SHADER_WAVY_RIPPLE_LIGHT_THICKNESS) {
        wavyRippleLightParams_->thickness_ = param;
    }
}

void GEVisualEffectImpl::SetWavyRippleLightParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (wavyRippleLightParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_WAVY_RIPPLE_LIGHT_CENTER) {
        wavyRippleLightParams_->center_ = param;
    }
}
void GEVisualEffectImpl::SetAuroraNoiseParams(const std::string& tag, float param)
{
    if (auroNoiseParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_AURORA_NOISE_VALUE) {
        auroNoiseParams_->noise_ = param;
    }
}

void GEVisualEffectImpl::SetParticleCircularHaloParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (particleCircularHaloParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_PARTICLE_CIRCULAR_HALO_CENTER) {
        particleCircularHaloParams_->center_.first = std::clamp(param.first, 0.0f, 1.0f);
        particleCircularHaloParams_->center_.second = std::clamp(param.second, 0.0f, 1.0f);
    }
}

void GEVisualEffectImpl::SetParticleCircularHaloParams(const std::string& tag, float param)
{
    if (particleCircularHaloParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_PARTICLE_CIRCULAR_HALO_RADIUS) {
        particleCircularHaloParams_->radius_ = std::clamp(param, 0.0f, 1.0f);
    }
    if (tag == GE_SHADER_PARTICLE_CIRCULAR_HALO_NOISE) {
        particleCircularHaloParams_->noise_ = std::max(param, 0.0f);
    }
}

void GEVisualEffectImpl::SetBorderLightParams(const std::string& tag, float param)
{
    if (borderLightParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_INTENSITY) {
        borderLightParams_->intensity = param;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_WIDTH) {
        borderLightParams_->width = param;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_CORNER_RADIUS) {
        borderLightParams_->cornerRadius = param;
    }
}

void GEVisualEffectImpl::SetBorderLightParams(const std::string& tag, const Vector3f& param)
{
    if (borderLightParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_POSITION) {
        borderLightParams_->position = param;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_ROTATION_ANGLE) {
        borderLightParams_->rotationAngle = param;
    }
}

void GEVisualEffectImpl::SetBorderLightParams(const std::string& tag, const Vector4f& param)
{
    if (borderLightParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_BORDER_LIGHT_COLOR) {
        borderLightParams_->color = param;
    }
}

void GEVisualEffectImpl::SetMaskTransitionParamsFloat(const std::string& tag, float param)
{
    if (maskTransitionParams_ == nullptr) {
        GE_LOGE("GEVisualEffectImpl set factor failed, maskTransitionParams is nullptr");
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_MASK_TRANSITION_FACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->maskTransitionParams_->factor = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetVariableRadiusBlurParams(const std::string& tag, float param)
{
    if (variableRadiusBlurParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_VARIABLE_RADIUS_BLUR_RADIUS) {
        variableRadiusBlurParams_->blurRadius = param;
    }
}

void GEVisualEffectImpl::SetColorGradientEffectParams(const std::string& tag, float param)
{
    if (colorGradientEffectParams_ == nullptr) {
        return;
    }
    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR_NUMBER,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->colorNum_ = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH0,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[0] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH1,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[1] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH2,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[2] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH3,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[3] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH4,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[4] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH5,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[5] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH6,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[6] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH7,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[7] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH8,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[8] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH9,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[9] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH10,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[10] = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_STRENGTH11,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->strengths_[11] = p;}},
    };
    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetColorGradientEffectParams(const std::string& tag, const Vector4f& param)
{
    if (colorGradientEffectParams_ == nullptr) {
        return;
    }
    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, Vector4f)>> actions = {
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR0, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[0] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR1, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[1] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR2, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[2] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR3, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[3] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR4, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[4] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR5, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[5] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR6, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[6] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR7, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[7] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR8, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[8] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR9, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[9] = Drawing::Color4f{param[0], param[1], param[2], param[3]};}
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR10, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[10] = Drawing::Color4f{param[0], param[1], param[2], param[3]};
            }
        },
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_COLOR11, [](GEVisualEffectImpl* obj, Vector4f param) {
                obj->colorGradientEffectParams_->colors_[11] = Drawing::Color4f{param[0], param[1], param[2], param[3]};
            }
        },
       
    };
    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetColorGradientEffectParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (colorGradientEffectParams_ == nullptr) {
        return;
    }
    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, std::pair<float, float> params)>>
        actions = {
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS0, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[0] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS1, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[1] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS2, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[2] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS3, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[3] = Drawing::Point{param.first, param.second};} },
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS4, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[4] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS5, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[5] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS6, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[6] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS7, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[7] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS8, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[8] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS9, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[9] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS10, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[10] = Drawing::Point{param.first, param.second};}},
            {GEX_SHADER_COLOR_GRADIENT_EFFECT_POS11, [](GEVisualEffectImpl* obj, std::pair<float, float> param) {
                obj->colorGradientEffectParams_->positions_[11] = Drawing::Point{param.first, param.second};}}
        };
    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
