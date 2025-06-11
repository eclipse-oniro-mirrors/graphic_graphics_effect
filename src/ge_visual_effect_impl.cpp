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
    }
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
        case FilterType::RADIAL_GRADIENT_MASK: {
            SetRadialGradientMaskParamsFloat(tag, param);
            break;
        }
        case FilterType::SOUND_WAVE: {
            SetSoundWaveParamsFloat(tag, param);
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
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, double param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const char* const param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param) {}

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
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<std::pair<float, float>> param)
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
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Drawing::Color4f& param)
{
    switch (filterType_) {
        case FilterType::SOUND_WAVE: {
            if (soundWaveParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_SOUND_WAVE_COLOR_A) {
                soundWaveParams_->colorA = param;
            }
            if (tag == GE_FILTER_SOUND_WAVE_COLOR_B) {
                soundWaveParams_->colorB = param;
            }
            if (tag == GE_FILTER_SOUND_WAVE_COLOR_C) {
                soundWaveParams_->colorC = param;
            }
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

void GEVisualEffectImpl::SetSoundWaveParamsFloat(const std::string& tag, float param)
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


void GEVisualEffectImpl::SetEdgeLightParams(const std::string& tag, float param)
{
    if (edgeLightParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R,
            [](GEVisualEffectImpl* obj, float p) { obj->edgeLightParams_->edgeColorR = p; } },
        { GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G,
            [](GEVisualEffectImpl* obj, float p) { obj->edgeLightParams_->edgeColorG = p; } },
        { GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B,
            [](GEVisualEffectImpl* obj, float p) { obj->edgeLightParams_->edgeColorB = p; } },
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
        { GE_FILTER_DISPERSION_RED_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->redOffsetX = p; } },
        { GE_FILTER_DISPERSION_RED_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->redOffsetY = p; } },
        { GE_FILTER_DISPERSION_GREEN_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->greenOffsetX = p; } },
        { GE_FILTER_DISPERSION_GREEN_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->greenOffsetY = p; } },
        { GE_FILTER_DISPERSION_BLUE_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->blueOffsetX = p; } },
        { GE_FILTER_DISPERSION_BLUE_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->dispersionParams_->blueOffsetY = p; } },
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
