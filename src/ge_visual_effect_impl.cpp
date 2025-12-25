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
namespace {
constexpr size_t NUM_0 = 0;
constexpr size_t NUM_1 = 1;
constexpr size_t NUM_2 = 2;
constexpr size_t NUM_3 = 3;

enum class DotDirection {
    TOP = 0,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    TOP_LEFT
};

template <typename T>
using TagMap = std::map<std::string, std::function<void(std::shared_ptr<T>&, const std::any&)>>;

template <typename T>
void ApplyTagParams(const std::string& tag, const std::any& value,
    std::shared_ptr<T>& params, const TagMap<T>& tagMap)
{
    if (!params) {
        GE_LOGE("GEVisualEffectImpl params for tag %{public}s not found", tag.c_str());
        return;
    }
    auto it = tagMap.find(tag);
    if (it != tagMap.end()) {
        it->second(params, value);
    } else {
        GE_LOGE("GEVisualEffectImpl tag %{public}s not found", tag.c_str());
    }
}

#define ADD_TAG_HANDLER(type, tag, member, valueType) \
    {tag, [](std::shared_ptr<type>&params, const std::any& value) { \
        params->member = std::any_cast<valueType>(value); \
    }}

using PairFloat = std::pair<float, float>;

TagMap<GEXAIBarGlowEffectParams> AIBarGlowEffectTagMap_{
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_LTWH, LTWH, Vector4f),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_STRECTCH_FACTOR,
        stretchFactor, float),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_BAR_ANGLE, barAngle, float),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_COLOR0, colors[0], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_COLOR1, colors[1], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_COLOR2, colors[2], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_COLOR3, colors[3], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_POS0, positions[0], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_POS1, positions[1], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_POS2, positions[2], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_POS3, positions[3], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_BRIGHTNESS, brightness, float),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_PROGRESS, progress, float),
    ADD_TAG_HANDLER(GEXAIBarGlowEffectParams, GEX_SHADER_AIBAR_GLOW_STRENGTH, strengths, Vector4f),
};

TagMap<GEXAIBarRectHaloEffectParams> AIBarRectHaloEffectTagMap_{
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_LTWH, LTWH, Vector4f),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_COLOR0, colors[0], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_COLOR1, colors[1], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_COLOR2, colors[2], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_COLOR3, colors[3], Vector4f),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_POS0, positions[0], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_POS1, positions[1], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_POS2, positions[2], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_POS3, positions[3], PairFloat),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_BRIGHTNESS, brightness, float),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_PROGRESS, progress, float),
    ADD_TAG_HANDLER(GEXAIBarRectHaloEffectParams, GEX_SHADER_AIBAR_RECT_HALO_STRENGTH, strengths, Vector4f),
};

TagMap<GEXRoundedRectFlowlightEffectParams> roundedRectFlowlightEffectTagMap_{
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_START_END_POS,
        startEndPosition, PairFloat),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_WAVE_LENGTH,
        waveLength, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_WAVE_TOP,
        waveTop, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_CORNER_RADIUS,
        cornerRadius, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_BRIGHTNESS,
        brightness, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_SCALE,
        scale, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_SHARPING,
        sharping, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_FEATHERING,
        feathering, float),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams,
        GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_FEATHERING_BEZIER_CONTROL_POINTS,
        featheringBezierControlPoints, Vector4f),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams,
        GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_GRADIENT_BEZIER_CONTROL_POINTS,
        gradientBezierControlPoints, Vector4f),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_COLOR,
        color, Vector4f),
    ADD_TAG_HANDLER(GEXRoundedRectFlowlightEffectParams, GEX_SHADER_ROUNDED_RECT_FLOWLIGHT_PROGRESS,
        progress, float),
};

TagMap<GEXGradientFlowColorsEffectParams> gradientFlowColorsEffectTagMap_{
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR0, colors[0], Vector4f),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR1, colors[1], Vector4f),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR2, colors[2], Vector4f),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_COLOR3, colors[3], Vector4f),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_GRADIENT_BEGIN,
        gradientBegin, float),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_GRADIENT_END,
        gradientEnd, float),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_EFFECT_ALPHA,
        effectAlpha, float),
    ADD_TAG_HANDLER(GEXGradientFlowColorsEffectParams, GEX_SHADER_GRADIENT_FLOW_COLORS_PROGRESS, progress, float),
};

TagMap<GEFrameGradientMaskParams> frameGradientMaskTagMap_{
    ADD_TAG_HANDLER(GEFrameGradientMaskParams, GE_MASK_FRAME_GRADIENT_GRADIENT_BEZIER_CONTROL_POINTS,
        gradientBezierControlPoints, Vector4f),
    ADD_TAG_HANDLER(GEFrameGradientMaskParams, GE_MASK_FRAME_GRADIENT_CORNER_RADIUS, cornerRadius, float),
    ADD_TAG_HANDLER(GEFrameGradientMaskParams, GE_MASK_FRAME_GRADIENT_FRAME_WIDTH, frameWidth, float),
};

TagMap<GEGridWarpShaderFilterParams> gridWarpShaderFilterTagMap_{
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT0, gridPoints[0], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT1, gridPoints[1], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT2, gridPoints[2], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT3, gridPoints[3], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT4, gridPoints[4], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT5, gridPoints[5], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT6, gridPoints[6], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT7, gridPoints[7], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_GRID_POINT8, gridPoints[8], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE0, rotationAngles[0], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE1, rotationAngles[1], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE2, rotationAngles[2], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE3, rotationAngles[3], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE4, rotationAngles[4], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE5, rotationAngles[5], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE6, rotationAngles[6], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE7, rotationAngles[7], PairFloat),
    ADD_TAG_HANDLER(GEGridWarpShaderFilterParams, GE_FILTER_GRID_WARP_ROTATION_ANGLE8, rotationAngles[8], PairFloat),
    };

TagMap<GECircleFlowlightEffectParams> circleFlowlightEffectTagMap_{
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_COLOR0, colors[0], Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_COLOR1, colors[1], Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_COLOR2, colors[2], Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_COLOR3, colors[3], Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_FREQUENCY, rotationFrequency,
        Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_AMPLITUDE, rotationAmplitude,
        Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_ROTATION_SEED, rotationSeed, Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_GRADIENTX, gradientX, Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_GRADIENTY, gradientY, Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_PROGRESS, progress, float),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_STRENGTH, strength, Vector4f),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_DISTORT_STRENGTH, distortStrength, float),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_BLEND_GRADIENT, blendGradient, float),
    ADD_TAG_HANDLER(GECircleFlowlightEffectParams, GE_SHADER_CIRCLE_FLOWLIGHT_MASK, mask,
        std::shared_ptr<Drawing::GEShaderMask>),
    };

TagMap<GEXDupoliNoiseMaskParams> dupoliNoiseMaskTagMap_{
    ADD_TAG_HANDLER(GEXDupoliNoiseMaskParams, GEX_MASK_DUPOLI_NOISE_GRANULARITY, granularity, float),
    ADD_TAG_HANDLER(GEXDupoliNoiseMaskParams, GEX_MASK_DUPOLI_NOISE_VERTICALMOVEDISTANCE, verticalMoveDistance, float),
    ADD_TAG_HANDLER(GEXDupoliNoiseMaskParams, GEX_MASK_DUPOLI_NOISE_PROGRESS, progress, float),
};

TagMap<GEXDistortChromaEffectParams> distortChromaTagMap_{
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_PROGRESS, progress, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_TURBULENTMASK, turbulentMask,
                    std::shared_ptr<Drawing::GEShaderMask>),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_MASK, mask,
                    std::shared_ptr<Drawing::GEShaderMask>),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_GRANULARITY, granularity, Vector2f),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_FREQUENCY, frequency, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_SHARPNESS, sharpness, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_BRIGHTNESS, brightness, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_DISTORTFACTOR, distortFactor, Vector2f),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_DISPERSION, dispersion, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_SATURATION, saturation, float),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_COLORSTRENGTH, colorStrength, Vector3f),
    ADD_TAG_HANDLER(GEXDistortChromaEffectParams, GEX_SHADER_DISTORT_CHROMA_VERTICALMOVEDISTANCE,
                    verticalMoveDistance, float),
};

TagMap<GEXNoisyFrameGradientMaskParams> noisyFrameGradientMaskTagMap_{
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_GRADIENT_BEZIER_COMTROL_POINTS,
        gradientBezierControlPoints, Vector4f),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_CORNER_RADIUS, cornerRadius, float),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_INNER_FRAME_WIDTH,
        innerFrameWidth, PairFloat),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_MIDDLE_FRAME_WIDTH,
        middleFrameWidth, PairFloat),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_OUTSIDE_FRAME_WIDTH,
        outsideFrameWidth, PairFloat),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_RRECT_WH, RRectWH, PairFloat),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_RRECT_POS, RRectPos, PairFloat),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_SLOPE, slope, float),
    ADD_TAG_HANDLER(GEXNoisyFrameGradientMaskParams, GEX_MASK_NOISY_FRAME_GRADIENT_PROGRESS, progress, float),
};

#undef ADD_TAG_HANDLER
}

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
    { GE_FILTER_SDF_FROM_IMAGE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_FROM_IMAGE);
            impl->MakeSdfFromImageParams();
        }
    },
    { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::CONTOUR_DIAGONAL_FLOW_LIGHT);
            impl->MakeContentDiagonalParams();
        }
    },
    { GE_SHADER_DOT_MATRIX_SHADER,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DOT_MATRIX);
            impl->MakeDotMatrixShaderParams();
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
    { GE_MASK_IMAGE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::IMAGE_MASK);
            impl->MakeImageMaskParams();
        }
    },
    { GE_MASK_USE_EFFECT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::USE_EFFECT_MASK);
            impl->MakeUseEffectMaskParams();
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
    { GE_SHADER_HARMONIUM_EFFECT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::HARMONIUM_EFFECT);
            impl->MakeHarmoniumEffectParams();
        }
    },
    { GE_SHAPE_SDF_UNION_OP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_UNION_OP);
            impl->MakeSDFUnionOpShapeParams(GESDFUnionOp::UNION);
        }
    },
    { GE_SHAPE_SDF_SMOOTH_UNION_OP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_UNION_OP);
            impl->MakeSDFUnionOpShapeParams(GESDFUnionOp::SMOOTH_UNION);
        }
    },
    { GE_SHAPE_SDF_RRECT_SHAPE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_RRECT_SHAPE);
            impl->MakeSDFRRectShapeParams();
        }
    },
    { GE_SHAPE_SDF_TRANSFORM_SHAPE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_TRANSFORM_SHAPE);
            impl->MakeSDFTransformShapeParams();
        }
    },
    { GE_SHAPE_SDF_PIXELMAP_SHAPE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_PIXELMAP_SHAPE);
            impl->MakeSDFPixelmapShapeParams();
        }
    },
    { GE_SHAPE_SDF_EMPTY_SHAPE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_EMPTY_SHAPE);
        }
    },
    { GEX_SHADER_LIGHT_CAVE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::LIGHT_CAVE);
            impl->MakeLightCaveParams();
        }
    },
    { GE_SHADER_BORDER_LIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::BORDER_LIGHT);
            impl->MakeBorderLightParams();
        }
    },
    { GEX_SHADER_AIBAR_GLOW,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AIBAR_GLOW);
            impl->MakeAIBarGlowEffectParams();
        }
    },
    { GEX_SHADER_AIBAR_RECT_HALO,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AIBAR_RECT_HALO);
            impl->MakeAIBarRectHaloEffectParams();
        }
    },
    { GEX_SHADER_ROUNDED_RECT_FLOWLIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::ROUNDED_RECT_FLOWLIGHT);
            impl->MakeRoundedRectFlowlightEffectParams();
        }
    },
    { GEX_SHADER_GRADIENT_FLOW_COLORS,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GRADIENT_FLOW_COLORS);
            impl->MakeGradientFlowColorsEffectParams();
        }
    },
    { GE_MASK_FRAME_GRADIENT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::FRAME_GRADIENT_MASK);
            impl->MakeFrameGradientMaskParams();
        }
    },
    { GEX_MASK_DUPOLI_NOISE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DUPOLI_NOISE_MASK);
            impl->MakeDupoliNoiseMaskParams();
        }
    },
    { GEX_SHADER_DISTORT_CHROMA,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::DISTORT_CHROMA);
            impl->MakeDistortChromaParams();
        }
    },
    { GE_FILTER_FROSTED_GLASS,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::FROSTED_GLASS);
            impl->MakeFrostedGlassParams();
        }
    },
    { GE_FILTER_GASIFY_SCALE_TWIST,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GASIFY_SCALE_TWIST);
            impl->MakeGasifyScaleTwistFilterParams();
        }
    },
    { GE_FILTER_GASIFY_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GASIFY_BLUR);
            impl->MakeGasifyBlurFilterParams();
        }
    },
    { GE_FILTER_GASIFY,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GASIFY);
            impl->MakeGasifyFilterParams();
        }
    },
    {  GE_SHADER_SDF_BORDER,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_BORDER);
            impl->MakeSDFBorderParams();
        }
    },
    { GE_SHADER_SDF_CLIP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_CLIP);
            impl->MakeSDFClipParams();
        }
    },
    { GE_SHADER_SDF_SHADOW,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::SDF_SHADOW);
            impl->MakeSDFShadowParams();
        }
    },
    { GE_FILTER_GRID_WARP,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GRID_WARP);
            impl->MakeGridWarpFilterParams();
        }
    },
    { GE_SHADER_CIRCLE_FLOWLIGHT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::CIRCLE_FLOWLIGHT);
            impl->MakeCircleFlowlightEffectParams();
        }
    },
    { GE_SHADER_FROSTED_GLASS_EFFECT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::FROSTED_GLASS_EFFECT);
            impl->MakeFrostedGlassEffectParams();
        }
    },
    { GE_FILTER_FROSTED_GLASS_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::FROSTED_GLASS_BLUR);
            impl->MakeFrostedGlassBlurParams();
        }
    },
    { GEX_MASK_NOISY_FRAME_GRADIENT,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::NOISY_FRAME_GRADIENT_MASK);
            impl->MakeNoisyFrameGradientMaskParams();
        }
    }
};

GEVisualEffectImpl::GEVisualEffectImpl(const std::string& name, const std::optional<Drawing::CanvasInfo>& canvasInfo)
{
    auto iter = g_initialMap.find(name);
    if (iter != g_initialMap.end()) {
        iter->second(this);
    }
    canvasInfo_ = canvasInfo.value_or(Drawing::CanvasInfo());
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
        case FilterType::DOT_MATRIX: {
            SetDotMatrixShaderParamsInitData(tag, param);
            break;
        }
        case FilterType::SDF_FROM_IMAGE: {
            if (sdfFromImageParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_SDF_FROM_IMAGE_SPREAD_FACTOR) {
                sdfFromImageParams_->spreadFactor = param;
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
        case FilterType::USE_EFFECT_MASK: {
            if (useEffectMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_USE_EFFECT_USE_EFFECT) {
                useEffectMaskParams_->useEffect = param;
            }
            break;
        }
        case FilterType::FROSTED_GLASS: {
            SetFrostedGlassParams(tag, param);
            break;
        }
        case FilterType::DOT_MATRIX: {
            if (dotMatrixShaderParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_DOT_MATRIX_SHADER_INVERSEEFFECT) {
                dotMatrixShaderParams_->inverseEffect_ = param;
            }
            break;
        }
        case FilterType::SDF_FROM_IMAGE: {
            if (sdfFromImageParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_SDF_FROM_IMAGE_GENERATE_DERIVS) {
                sdfFromImageParams_->generateDerivs = param;
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
        case FilterType::DOT_MATRIX: {
            SetDotMatrixShaderParams(tag, param);
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
        case FilterType::HARMONIUM_EFFECT: {
            SetHarmoniumEffectParams(tag, param);
            break;
        }
        case FilterType::LIGHT_CAVE: {
            SetLightCaveParams(tag, param);
            break;
        }
        case FilterType::BORDER_LIGHT: {
            SetBorderLightParams(tag, param);
            break;
        }
        case FilterType::GASIFY_SCALE_TWIST: {
            SetGasifyScaleTwistParams(tag, param);
            break;
        }
        case FilterType::GASIFY_BLUR: {
            SetGasifyBlurParams(tag, param);
            break;
        }
        case FilterType::GASIFY: {
            SetGasifyParams(tag, param);
            break;
        }
        case FilterType::FRAME_GRADIENT_MASK: {
            ApplyTagParams(tag, param, frameGradientMaskParams_, frameGradientMaskTagMap_);
            break;
        }
        case FilterType::AIBAR_GLOW: {
            ApplyTagParams(tag, param, AIBarGlowEffectParams_, AIBarGlowEffectTagMap_);
            break;
        }
        case FilterType::AIBAR_RECT_HALO: {
            ApplyTagParams(tag, param, AIBarRectHaloEffectParams_, AIBarRectHaloEffectTagMap_);
            break;
        }
        case FilterType::ROUNDED_RECT_FLOWLIGHT: {
            ApplyTagParams(tag, param, roundedRectFlowlightEffectParams_, roundedRectFlowlightEffectTagMap_);
            break;
        }
        case FilterType::GRADIENT_FLOW_COLORS: {
            ApplyTagParams(tag, param, gradientFlowColorsEffectParams_, gradientFlowColorsEffectTagMap_);
            break;
        }
        case FilterType::SDF_UNION_OP: {
            if (sdfUnionOpShapeParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHAPE_SDF_SMOOTH_UNION_OP_SPACING) {
                sdfUnionOpShapeParams_->spacing = param;
            }
            break;
        }
        case FilterType::FROSTED_GLASS: {
            SetFrostedGlassParams(tag, param);
            break;
        }
        case FilterType::CIRCLE_FLOWLIGHT: {
            ApplyTagParams(tag, param, circleFlowlightEffectParams_, circleFlowlightEffectTagMap_);
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            SetFrostedGlassEffectParams(tag, param);
            break;
        }
        case FilterType::FROSTED_GLASS_BLUR: {
            SetFrostedGlassBlurParams(tag, param);
            break;
        }
        case FilterType::DUPOLI_NOISE_MASK: {
            ApplyTagParams(tag, param, dupoliNoiseMaskParams_, dupoliNoiseMaskTagMap_);
            break;
        }
        case FilterType::DISTORT_CHROMA: {
            ApplyTagParams(tag, param, distortChromaParams_, distortChromaTagMap_);
            break;
        }
        case FilterType::NOISY_FRAME_GRADIENT_MASK: {
            ApplyTagParams(tag, param, noisyFrameGradientMaskParams_, noisyFrameGradientMaskTagMap_);
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
        case FilterType::GASIFY_SCALE_TWIST: {
            SetGasifyScaleTwistParams(tag, param);
            break;
        }
        case FilterType::GASIFY_BLUR: {
            SetGasifyBlurParams(tag, param);
            break;
        }
        case FilterType::GASIFY: {
            SetGasifyParams(tag, param);
            break;
        }
        case FilterType::IMAGE_MASK: {
            if (imageMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_IMAGE_IMAGE) {
                imageMaskParams_->image = param;
            }
            break;
        }
        case FilterType::USE_EFFECT_MASK: {
            if (useEffectMaskParams_ == nullptr) {
                return;
            }
            if (tag == GE_MASK_USE_EFFECT_PIXEL_MAP) {
                useEffectMaskParams_->image = param;
            }
            break;
        }
        case FilterType::SDF_PIXELMAP_SHAPE: {
            if (sdfPixelmapShapeParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHAPE_SDF_PIXELMAP_SHAPE_IMAGE) {
                sdfPixelmapShapeParams_->image = param;
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
        case FilterType::HARMONIUM_EFFECT: {
            if (harmoniumEffectParams_ == nullptr) {
                return;
            }

            if (tag == GE_SHADER_HARMONIUM_EFFECT_TOTALMATRIX) {
                harmoniumEffectParams_->totalMatrix = param;
            }
            break;
        }
        case FilterType::SDF_TRANSFORM_SHAPE: {
            if (sdfTransformShapeParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHAPE_SDF_TRANSFORM_SHAPE_MATRIX) {
                sdfTransformShapeParams_->matrix = param;
            }
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            if (frostedGlassEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_MASKMATRIX) {
                frostedGlassEffectParams_->maskMatrix = param;
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
        case FilterType::DOT_MATRIX: {
            if (dotMatrixShaderParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_DOT_MATRIX_SHADER_COLORFRACTIONS) {
                dotMatrixShaderParams_->colorFractions_ = Vector2f(param.first, param.second);
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
        case FilterType::LIGHT_CAVE: {
            SetLightCaveParams(tag, param);
            break;
        }
        case FilterType::GASIFY_SCALE_TWIST: {
            SetGasifyScaleTwistParams(tag, param);
            break;
        }
        case FilterType::AIBAR_GLOW: {
            ApplyTagParams(tag, param, AIBarGlowEffectParams_, AIBarGlowEffectTagMap_);
            break;
        }
        case FilterType::AIBAR_RECT_HALO: {
            ApplyTagParams(tag, param, AIBarRectHaloEffectParams_, AIBarRectHaloEffectTagMap_);
            break;
        }
        case FilterType::ROUNDED_RECT_FLOWLIGHT: {
            ApplyTagParams(tag, param, roundedRectFlowlightEffectParams_, roundedRectFlowlightEffectTagMap_);
            break;
        }
        case FilterType::FROSTED_GLASS: {
            SetFrostedGlassParams(tag, param);
            break;
        }
        case FilterType::GRID_WARP: {
            ApplyTagParams(tag, param, gridWarpFilterParams_, gridWarpShaderFilterTagMap_);
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            SetFrostedGlassEffectParams(tag, param);
            break;
        }
        case FilterType::NOISY_FRAME_GRADIENT_MASK: {
            ApplyTagParams(tag, param, noisyFrameGradientMaskParams_, noisyFrameGradientMaskTagMap_);
            break;
        }
        case FilterType::DUPOLI_NOISE_MASK: {
            ApplyTagParams(tag, Vector2f(param.first, param.second), dupoliNoiseMaskParams_, dupoliNoiseMaskTagMap_);
            break;
        }
        case FilterType::DISTORT_CHROMA: {
            ApplyTagParams(tag, Vector2f(param.first, param.second), distortChromaParams_, distortChromaTagMap_);
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
        case FilterType::HARMONIUM_EFFECT: {
            if (harmoniumEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_HARMONIUM_EFFECT_RIPPLEPOSITION) {
                harmoniumEffectParams_->ripplePosition = param;
            }
            break;
        }
        case FilterType::DOT_MATRIX: {
            if (dotMatrixShaderParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_DOT_MATRIX_SHADER_STARTPOINTS) {
                dotMatrixShaderParams_->startPoints_ = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<Vector4f>& param)
{
    switch (filterType_) {
        case FilterType::DOT_MATRIX: {
            if (dotMatrixShaderParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_DOT_MATRIX_SHADER_EFFECTCOLORS) {
                dotMatrixShaderParams_->effectColors_ = param;
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
        case FilterType::SDF_UNION_OP: {
            bool isParamValid = param < static_cast<uint32_t>(GESDFUnionOp::MAX);
            if (sdfUnionOpShapeParams_ == nullptr || !isParamValid) {
                return;
            }
            if (tag == GE_SHAPE_SDF_UNION_OP_TYPE) {
                sdfUnionOpShapeParams_->op = static_cast<GESDFUnionOp>(param);
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
        case FilterType::HARMONIUM_EFFECT: {
            if (harmoniumEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_HARMONIUM_EFFECT_MASK) {
                harmoniumEffectParams_->mask = param;
            }

            if (tag == GE_SHADER_HARMONIUM_EFFECT_MASKCLOCK) {
                harmoniumEffectParams_->maskClock = param;
            }

            if (tag == GE_SHADER_HARMONIUM_EFFECT_USEEFFECTMASK) {
                harmoniumEffectParams_->useEffectMask = param;
            }
            break;
        }
        case FilterType::CIRCLE_FLOWLIGHT: {
            ApplyTagParams(tag, param, circleFlowlightEffectParams_, circleFlowlightEffectTagMap_);
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            if (frostedGlassEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_USEEFFECTMASK) {
                frostedGlassEffectParams_->useEffectMask = param;
            }
            break;
        }
        case FilterType::DISTORT_CHROMA: {
            ApplyTagParams(tag, param, distortChromaParams_, distortChromaTagMap_);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderShape> param)
{
    switch (filterType_) {
        case FilterType::SDF_UNION_OP: {
            if (sdfUnionOpShapeParams_ == nullptr || !param) {
                return;
            }

            if (tag == GE_SHAPE_SDF_UNION_OP_SHAPEX || tag == GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEX) {
                sdfUnionOpShapeParams_->left = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            } else if (tag == GE_SHAPE_SDF_UNION_OP_SHAPEY || tag == GE_SHAPE_SDF_SMOOTH_UNION_OP_SHAPEY) {
                sdfUnionOpShapeParams_->right = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            }
            break;
        }
        case FilterType::SDF_BORDER: {
            if (sdfBorderShaderParams_ && param) {
                sdfBorderShaderParams_->shape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            }
            break;
        }
        case FilterType::SDF_SHADOW: {
            SetSDFShadowParams(tag, param);
            break;
        }
        case FilterType::SDF_CLIP: {
            SetSDFClipParams(tag, param);
            break;
        }
        case FilterType::FROSTED_GLASS: {
            if (frostedGlassParams_ == nullptr || !param) {
                return;
            }
            if (tag == GE_FILTER_FROSTED_GLASS_SHAPE) {
                frostedGlassParams_->sdfShape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            }
            break;
        }
        case FilterType::SDF_TRANSFORM_SHAPE: {
            if (sdfTransformShapeParams_ == nullptr || !param) {
                return;
            }
            if (tag == GE_SHAPE_SDF_TRANSFORM_SHAPE_SHAPE) {
                sdfTransformShapeParams_->shape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            }
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            if (frostedGlassEffectParams_ == nullptr || !param) {
                return;
            }
            if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SHAPE) {
                frostedGlassEffectParams_->sdfShape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetSDFClipParams(const std::string& tag, const std::shared_ptr<Drawing::GEShaderShape> param)
{
    if (sdfClipShaderParams_ && param) {
        sdfClipShaderParams_->shape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
    }
}

void GEVisualEffectImpl::SetSDFShadowParams(const std::string& tag, const std::shared_ptr<Drawing::GEShaderShape> param)
{
    if (sdfShadowShaderParams_ && param) {
        sdfShadowShaderParams_->shape = std::static_pointer_cast<Drawing::GESDFShaderShape>(param);
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
        case FilterType::HARMONIUM_EFFECT: {
            if (harmoniumEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_HARMONIUM_EFFECT_POSRGB) {
                harmoniumEffectParams_->posRGB = param;
            }
            if (tag == GE_SHADER_HARMONIUM_EFFECT_NEGRGB) {
                harmoniumEffectParams_->negRGB = param;
            }
            break;
        }
        case FilterType::FROSTED_GLASS: {
            SetFrostedGlassParams(tag, param);
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            SetFrostedGlassEffectParams(tag, param);
            break;
        }
        case FilterType::DISTORT_CHROMA: {
            ApplyTagParams(tag, param, distortChromaParams_, distortChromaTagMap_);
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
        case FilterType::DOT_MATRIX: {
            SetDotMatrixShaderParams(tag, param);
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
        case FilterType::HARMONIUM_EFFECT: {
            if (harmoniumEffectParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHADER_HARMONIUM_EFFECT_TINTCOLOR) {
                harmoniumEffectParams_->tintColor = param;
            }
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
        case FilterType::LIGHT_CAVE: {
            SetLightCaveParams(tag, param);
            break;
        }
        case FilterType::FRAME_GRADIENT_MASK: {
            ApplyTagParams(tag, param, frameGradientMaskParams_, frameGradientMaskTagMap_);
            break;
        }
        case FilterType::AIBAR_GLOW: {
            ApplyTagParams(tag, param, AIBarGlowEffectParams_, AIBarGlowEffectTagMap_);
            break;
        }
        case FilterType::AIBAR_RECT_HALO: {
            ApplyTagParams(tag, param, AIBarRectHaloEffectParams_, AIBarRectHaloEffectTagMap_);
            break;
        }
        case FilterType::ROUNDED_RECT_FLOWLIGHT: {
            ApplyTagParams(tag, param, roundedRectFlowlightEffectParams_, roundedRectFlowlightEffectTagMap_);
            break;
        }
        case FilterType::GRADIENT_FLOW_COLORS: {
            ApplyTagParams(tag, param, gradientFlowColorsEffectParams_, gradientFlowColorsEffectTagMap_);
            break;
        }
        case FilterType::CIRCLE_FLOWLIGHT: {
            ApplyTagParams(tag, param, circleFlowlightEffectParams_, circleFlowlightEffectTagMap_);
            break;
        }
        case FilterType::FROSTED_GLASS: {
            SetFrostedGlassParams(tag, param);
            break;
        }
        case FilterType::FROSTED_GLASS_EFFECT: {
            SetFrostedGlassEffectParams(tag, param);
            break;
        }
        case FilterType::NOISY_FRAME_GRADIENT_MASK: {
            ApplyTagParams(tag, param, noisyFrameGradientMaskParams_, noisyFrameGradientMaskTagMap_);
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

void GEVisualEffectImpl::SetParam(const std::string& tag, const GERRect& param)
{
    switch (filterType_) {
        case FilterType::SDF_RRECT_SHAPE: {
            if (sdfRRectShapeParams_ == nullptr) {
                return;
            }
            if (tag == GE_SHAPE_SDF_RRECT_SHAPE_RRECT) {
                sdfRRectShapeParams_->rrect = param;
            }
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const GESDFBorderParams& border)
{
    if (tag == GE_SHADER_SDF_BORDER_BORDER && sdfBorderShaderParams_) {
        sdfBorderShaderParams_->border = border;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const GESDFShadowParams& shadow)
{
    if (tag == GE_SHADER_SDF_SHADOW_SHADOW && sdfShadowShaderParams_) {
        sdfShadowShaderParams_->shadow = shadow;
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
        { GE_FILTER_MAGNIFIER_ZOOM_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->zoomOffsetX = p; } },
        { GE_FILTER_MAGNIFIER_ZOOM_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->zoomOffsetY = p; } },
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
            [](GEVisualEffectImpl* obj, float p) { obj->doubleRippleMaskParams_->turbulence_ = p; } },
        { GE_MASK_DOUBLE_RIPPLE_HALOTHICKNESS,
            [](GEVisualEffectImpl* obj, float p) { obj->doubleRippleMaskParams_->haloThickness_ = p; } }
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
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_THICKNESS,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->thickness_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->haloRadius_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_LIGHTWEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->lightWeight_ = p; } },
        { GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT_HALOWEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->contentDiagonalParams_->haloWeight_ = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetDotMatrixShaderParamsInitData(const std::string &tag, int32_t param)
{
    if (dotMatrixShaderParams_ == nullptr) {
        return;
    }

    if (tag == GE_SHADER_DOT_MATRIX_SHADER_PATHDIRECTION) {
        SetDotMatrixShaderParamsPathDirection(tag, param);
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_EFFECTTYPE) {
        SetDotMatrixShaderParamsEffectType(tag, param);
    }
}

void GEVisualEffectImpl::SetDotMatrixShaderParamsPathDirection(const std::string& tag, int32_t param)
{
    DotDirection direction = static_cast<DotDirection>(param);
    switch (direction) {
        case DotDirection ::TOP:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::TOP;
            break;
        case DotDirection ::TOP_RIGHT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::TOP_RIGHT;
            break;
        case DotDirection ::RIGHT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::RIGHT;
            break;
        case DotDirection ::BOTTOM_RIGHT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::BOTTOM_RIGHT;
            break;
        case DotDirection ::BOTTOM:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::BOTTOM;
            break;
        case DotDirection ::BOTTOM_LEFT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::BOTTOM_LEFT;
            break;
        case DotDirection ::LEFT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::LEFT;
            break;
        case DotDirection ::TOP_LEFT:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::TOP_LEFT;
            break;
        default:
            dotMatrixShaderParams_->pathDirection_ = DotMatrixDirection::TOP_LEFT;
            break;
    }
}

void GEVisualEffectImpl::SetDotMatrixShaderParamsEffectType(const std::string& tag, int32_t param)
{
    enum class EffectTypeParam {
        NONE = 0,
        ROTATE,
        RIPPLE
    };
    EffectTypeParam effectTypeParam = static_cast<EffectTypeParam>(param);
    switch (effectTypeParam) {
        case EffectTypeParam::NONE:
            dotMatrixShaderParams_->effectType_ = DotMatrixEffectType::NONE;
            break;
        case EffectTypeParam::ROTATE:
            dotMatrixShaderParams_->effectType_ = DotMatrixEffectType::ROTATE;
            break;
        case EffectTypeParam::RIPPLE:
            dotMatrixShaderParams_->effectType_ = DotMatrixEffectType::RIPPLE;
            break;
        default:
            dotMatrixShaderParams_->effectType_ = DotMatrixEffectType::NONE;
            break;
    }
}
void GEVisualEffectImpl::SetDotMatrixShaderParams(const std::string& tag, float param)
{
    if (dotMatrixShaderParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_PATHWIDTH) {
        dotMatrixShaderParams_->pathWidth_ = param;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_DOTSPACING) {
        dotMatrixShaderParams_->dotSpacing_ = param;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_DOTRADIUS) {
        dotMatrixShaderParams_->dotRadius_ = param;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_PROGRESS) {
        dotMatrixShaderParams_->progress_ = param;
    }
}

void GEVisualEffectImpl::SetDotMatrixShaderParams(const std::string& tag, const Vector4f& param)
{
    if (dotMatrixShaderParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_DOTCOLOR) {
        dotMatrixShaderParams_->dotColor_ = param;
    }
    if (tag == GE_SHADER_DOT_MATRIX_SHADER_BGCOLOR) {
        dotMatrixShaderParams_->bgColor_ = param;
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
        particleCircularHaloParams_->radius_ = std::clamp(param, 0.001f, 10.0f);
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
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_BLEND,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->blend_ = p;}},
        {GEX_SHADER_COLOR_GRADIENT_EFFECT_BLEND_K,
            [](GEVisualEffectImpl* obj, float p) {obj->colorGradientEffectParams_->blendk_ = p;}},
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

void GEVisualEffectImpl::SetHarmoniumEffectParams(const std::string& tag, float param)
{
    if (harmoniumEffectParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_SHADER_HARMONIUM_EFFECT_RIPPLEPROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->rippleProgress = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_DISTORTPROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->distortProgress = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_MASKPROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->maskProgress = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_DISTORTFACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->distortFactor = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_REFLECTIONFACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->reflectionFactor = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_REFRACTIONFACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->refractionFactor = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_MATERIALFACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->materialFactor = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_BLURLEFT,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->blurLeft = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_BLURTOP,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->blurTop = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_CORNERRADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->cornerRadius = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_RATE,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->rate = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_LIGHTUPDEGREE,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->lightUpDegree = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_CUBICCOEFF,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->cubicCoeff = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_QUADCOEFF,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->quadCoeff = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_SATURATION,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->saturation = p; } },
        { GE_SHADER_HARMONIUM_EFFECT_FRACTION,
            [](GEVisualEffectImpl* obj, float p) { obj->harmoniumEffectParams_->fraction = p; } },
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


void GEVisualEffectImpl::SetLightCaveParams(const std::string& tag, const Vector4f& param)
{
    if (lightCaveShaderParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, const Vector4f&)>> actions = {
        { GEX_SHADER_LIGHT_CAVE_COLORA,
            [](GEVisualEffectImpl* obj, const Vector4f& p) { obj->lightCaveShaderParams_->colorA = p; }
        },
        { GEX_SHADER_LIGHT_CAVE_COLORB,
            [](GEVisualEffectImpl* obj, const Vector4f& p) { obj->lightCaveShaderParams_->colorB = p; }
        },
        { GEX_SHADER_LIGHT_CAVE_COLORC,
            [](GEVisualEffectImpl* obj, const Vector4f& p) { obj->lightCaveShaderParams_->colorC = p; }
        }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetLightCaveParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (lightCaveShaderParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*,
        const std::pair<float, float>&)>> actions = {
        { GEX_SHADER_LIGHT_CAVE_POSITION,
            [](GEVisualEffectImpl* obj, const std::pair<float, float>& p) {
                obj->lightCaveShaderParams_->position = Vector2f(p.first, p.second);
            }
        },
        { GEX_SHADER_LIGHT_CAVE_RADIUSXY,
            [](GEVisualEffectImpl* obj, const std::pair<float, float>& p) {
                obj->lightCaveShaderParams_->radiusXY = Vector2f(p.first, p.second);
            }
        }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetLightCaveParams(const std::string& tag, float param)
{
    if (lightCaveShaderParams_ == nullptr) {
        return;
    }

    if (tag == GEX_SHADER_LIGHT_CAVE_PROGRESS) {
        lightCaveShaderParams_->progress = param;
    }
}

void GEVisualEffectImpl::SetGasifyScaleTwistParams(const std::string& tag, float param)
{
    if (gasifyScaleTwistFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_SCALE_TWIST_PROGRESS) {
        gasifyScaleTwistFilterParams_->progress_ = std::clamp(param, 0.0f, 1.0f);
    }
}

void GEVisualEffectImpl::SetGasifyScaleTwistParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (gasifyScaleTwistFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_SCALE_TWIST_SCALE) {
        gasifyScaleTwistFilterParams_->scale_.first = std::max(param.first, 0.0f);
        gasifyScaleTwistFilterParams_->scale_.second = std::max(param.second, 0.0f);
    }
}

void GEVisualEffectImpl::SetGasifyScaleTwistParams(const std::string& tag, const std::shared_ptr<Drawing::Image> param)
{
    if (gasifyScaleTwistFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_SCALE_TWIST_SOURCEIMAGE) {
        gasifyScaleTwistFilterParams_->sourceImage_ = param;
    }
    if (tag == GE_FILTER_GASIFY_SCALE_TWIST_MASK) {
        gasifyScaleTwistFilterParams_->maskImage_ = param;
    }
}

void GEVisualEffectImpl::SetGasifyBlurParams(const std::string& tag, float param)
{
    if (gasifyBlurFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_BLUR_PROGRESS) {
        gasifyBlurFilterParams_->progress_ = std::clamp(param, 0.0f, 1.0f);
    }
}

void GEVisualEffectImpl::SetGasifyBlurParams(const std::string& tag, const std::shared_ptr<Drawing::Image> param)
{
    if (gasifyBlurFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_BLUR_SOURCEIMAGE) {
        gasifyBlurFilterParams_->sourceImage_ = param;
    }
    if (tag == GE_FILTER_GASIFY_BLUR_MASK) {
        gasifyBlurFilterParams_->maskImage_ = param;
    }
}

void GEVisualEffectImpl::SetGasifyParams(const std::string& tag, float param)
{
    if (gasifyFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_PROGRESS) {
        gasifyFilterParams_->progress_ = std::clamp(param, 0.0f, 1.0f);
    }
}

void GEVisualEffectImpl::SetGasifyParams(const std::string& tag, const std::shared_ptr<Drawing::Image> param)
{
    if (gasifyFilterParams_ == nullptr) {
        return;
    }
    if (tag == GE_FILTER_GASIFY_SOURCEIMAGE) {
        gasifyFilterParams_->sourceImage_ = param;
    }
    if (tag == GE_FILTER_GASIFY_MASK) {
        gasifyFilterParams_->maskImage_ = param;
    }
}

void GEVisualEffectImpl::SetFrostedGlassParams(const std::string& tag, const float& param)
{
    if (frostedGlassParams_ == nullptr) {
        return;
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_CORNERRADIUS) {
        constexpr float V_MIN = 0.0f;
        frostedGlassParams_->cornerRadius = std::max(param, V_MIN);
    }
    if (tag == GE_FILTER_FROSTED_GLASS_BASEMATERIALTYPE) {
        frostedGlassParams_->baseMaterialType = param;
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARK_SCALE) {
        constexpr float MIN_S = 0.0f;
        constexpr float MAX_S = 1.0f;
        frostedGlassParams_->darkScale = std::clamp(param, MIN_S, MAX_S);
    }
}

void GEVisualEffectImpl::SetFrostedGlassParams(const std::string& tag, const bool& param)
{
    if (frostedGlassParams_ == nullptr) {
        return;
    }

    if (tag == GE_FILTER_FROSTED_GLASS_BASEVIBRANCYENABLED) {
        frostedGlassParams_->baseVibrancyEnabled = param;
    }
    if (tag == GE_FILTER_FROSTED_GLASS_REFRACTENABLED) {
        frostedGlassParams_->refractEnabled = param;
    }
    if (tag == GE_FILTER_FROSTED_GLASS_INNERSHADOWENABLED) {
        frostedGlassParams_->innerShadowEnabled = param;
    }
    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTENABLED) {
        frostedGlassParams_->envLightEnabled = param;
    }
    if (tag == GE_FILTER_FROSTED_GLASS_HIGHLIGHTENABLED) {
        frostedGlassParams_->highLightEnabled = param;
    }
}

void GEVisualEffectImpl::SetFrostedGlassParams(const std::string& tag, const Vector4f& param)
{
    if (frostedGlassParams_ == nullptr) {
        return;
    }

    if (tag == GE_FILTER_FROSTED_GLASS_MATERIALCOLOR) {
        frostedGlassParams_->materialColor = param;
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassRates(const std::string& tag, const std::pair<float, float>& param)
{
    constexpr float V_MIN = -20.0f;
    constexpr float V_MAX = 20.0f;
 
    if (tag == GE_FILTER_FROSTED_GLASS_BGRATES) {
        frostedGlassParams_->bgRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_BGRATES) {
        frostedGlassParams_->darkModeBgRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_SDRATES) {
        frostedGlassParams_->sdRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTRATES) {
        frostedGlassParams_->envLightRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_EDLRATES) {
        frostedGlassParams_->edLightRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassWeights(const std::string& tag, const std::pair<float, float>& param)
{
    constexpr float MIN_W = 0.0f;
    constexpr float MAX_W = 1.0f;
    if (tag == GE_FILTER_FROSTED_GLASS_WEIGHTSEMBOSS) {
        frostedGlassParams_->weightsEmboss = Vector2f(std::clamp(param.first, MIN_W, MAX_W),
            std::clamp(param.second, MIN_W, MAX_W));
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_WEIGHTSEMBOSS) {
        frostedGlassParams_->darkModeWeightsEmboss = Vector2f(std::clamp(param.first, MIN_W, MAX_W),
            std::clamp(param.second, MIN_W, MAX_W));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_WEIGHTSEDL) {
        frostedGlassParams_->weightsEdl = Vector2f(std::clamp(param.first, MIN_W, MAX_W),
            std::clamp(param.second, MIN_W, MAX_W));
    }
}

void GEVisualEffectImpl::SetFrostedGlassParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (frostedGlassParams_ == nullptr) {
        return;
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_BORDERSIZE) {
        constexpr float MIN_V = 0.0f;
        frostedGlassParams_->borderSize = Vector2f(std::max(param.first, MIN_V), std::max(param.second, MIN_V));
    }
    {
        constexpr float MIN_V = 1e-6f;
        constexpr float MAX_V = 200.0f;
        constexpr float MIN_K = 1.0f;
        constexpr float MAX_K = 200.0f;
        if (tag == GE_FILTER_FROSTED_GLASS_BLURPARAMS) {
            frostedGlassParams_->blurParams =
                Vector2f(std::clamp(param.first, MIN_V, MAX_V), std::clamp(param.second, MIN_K, MAX_K));
        }
        if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_BLURPARAM) {
            frostedGlassParams_->darkModeBlurParam =
                Vector2f(std::clamp(param.first, MIN_V, MAX_V), std::clamp(param.second, MIN_K, MAX_K));
        }
    }
    
    HandleSetFrostedGlassWeights(tag, param);
 
    if (tag == GE_FILTER_FROSTED_GLASS_EDLPARAMS) {
        constexpr float MIN_P = 0.0f;
        constexpr float MAX_P = 500.0f;
        frostedGlassParams_->edLightParams = Vector2f(std::clamp(param.first, MIN_P, MAX_P),
            std::clamp(param.second, MIN_P, MAX_P));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_EDLANGLES) {
        constexpr float MIN_V = 0.0f;
        constexpr float MAX_V = 360.0f;
        frostedGlassParams_->edLightAngles = Vector2f(std::clamp(param.first, MIN_V, MAX_V),
            std::clamp(param.second, MIN_V, MAX_V));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_EDLDIR) {
        constexpr float MIN_V = -1.0f;
        constexpr float MAX_V = 1.0f;
        constexpr float EPS = 1e-6f;
 
        float dx = std::clamp(param.first, MIN_V, MAX_V);
        float dy = std::clamp(param.second, MIN_V, MAX_V);
        if (std::abs(dx) < EPS && std::abs(dy) < EPS) {
            dx = 1.0f;
            dy = 0.0f;
        }
        frostedGlassParams_->edLightDir = Vector2f(dx, dy);
    }
 
    HandleSetFrostedGlassRates(tag, param);
}

void GEVisualEffectImpl::HandleSetFrostedGlassKBS(const std::string& tag, const Vector3f& param)
{
    constexpr float KB_MIN = -20.0f;
    constexpr float KB_MAX = 20.0f;
    constexpr float S_MIN = 0.0f;
    constexpr float S_MAX = 20.0f;
    
    if (tag == GE_FILTER_FROSTED_GLASS_BGKBS) {
        frostedGlassParams_->bgKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_BGKBS) {
        frostedGlassParams_->darkModeBgKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_SDKBS) {
        frostedGlassParams_->sdKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTKBS) {
        frostedGlassParams_->envLightKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
 
    if (tag == GE_FILTER_FROSTED_GLASS_EDLKBS) {
        frostedGlassParams_->edLightKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassPosNegCoefs(const std::string& tag, const Vector3f& param)
{
    constexpr float V_MIN = -20.0f;
    constexpr float V_MAX = 20.0f;
    
    if (tag == GE_FILTER_FROSTED_GLASS_BGPOS) {
        frostedGlassParams_->bgPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_BGPOS) {
        frostedGlassParams_->darkModeBgPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_BGNEG) {
        frostedGlassParams_->bgNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }
    if (tag == GE_SHADER_FROSTED_GLASS_DARKMODE_BGNEG) {
        frostedGlassParams_->darkModeBgNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_SDPOS) {
        frostedGlassParams_->sdPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_SDNEG) {
        frostedGlassParams_->sdNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTPOS) {
        frostedGlassParams_->envLightPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTNEG) {
        frostedGlassParams_->envLightNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_EDLPOS) {
        frostedGlassParams_->edLightPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_EDLNEG) {
        frostedGlassParams_->edLightNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }
}

void GEVisualEffectImpl::SetFrostedGlassParams(const std::string& tag, const Vector3f& param)
{
    if (frostedGlassParams_ == nullptr) {
        return;
    }
    constexpr float MIN_S = -500.0f;
    constexpr float MAX_S = 500.0f;
    constexpr float MIN_V = 0.0f;
    constexpr float MAX_V = 250.0f;
    constexpr float MIN_F = 1.0f;
    constexpr float MAX_F = 250.0f;

    if (tag == GE_FILTER_FROSTED_GLASS_SDPARAMS) {
        frostedGlassParams_->sdParams = Vector3f(std::clamp(param[NUM_0], MIN_S, MAX_S),
            std::clamp(param[NUM_1], MIN_V, MAX_V), std::clamp(param[NUM_2], MIN_F, MAX_F));
    }

    if (tag == GE_FILTER_FROSTED_GLASS_ENVLIGHTPARAMS) {
        frostedGlassParams_->envLightParams = Vector3f(std::clamp(param[NUM_0], MIN_S, MAX_S),
            std::clamp(param[NUM_1], MIN_V, MAX_V), std::clamp(param[NUM_2], MIN_F, MAX_F));
    }

    HandleSetFrostedGlassKBS(tag, param);
    HandleSetFrostedGlassPosNegCoefs(tag, param);

    if (tag == GE_FILTER_FROSTED_GLASS_REFRACTPARAMS) {
        constexpr float MIN_R = -1.0f;
        constexpr float MAX_R = 1.0f;
        constexpr float MIN_C = 0.0f;
        constexpr float MAX_C = 10.0f;
        constexpr float MIN_D = 0.0f;
        constexpr float MAX_D = 1.0f;
        frostedGlassParams_->refractParams = Vector3f(std::clamp(param[NUM_0], MIN_R, MAX_R),
            std::clamp(param[NUM_1], MIN_C, MAX_C), std::clamp(param[NUM_2], MIN_D, MAX_D));
    }
}

void GEVisualEffectImpl::SetFrostedGlassEffectParams(const std::string& tag, const float& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_BLURPARAM) {
        constexpr float MIN_K = 1.0f;
        constexpr float MAX_K = 200.0f;
        frostedGlassEffectParams_->blurParam = std::clamp(param, MIN_K, MAX_K);
    }
    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_REFRACTOUTPX) {
        constexpr float MIN_S = -500.0f;
        constexpr float MAX_S = 500.0f;
        frostedGlassEffectParams_->refractOutPx = std::clamp(param, MIN_S, MAX_S);
    }
}

void GEVisualEffectImpl::SetFrostedGlassEffectParams(const std::string& tag, const std::pair<float, float>& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    HandleSetFrostedGlassEffectWeights(tag, param);

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLPARAMS) {
        constexpr float MIN_P = 0.0f;
        constexpr float MAX_P = 500.0f;
        frostedGlassEffectParams_->edLightParams = Vector2f(std::clamp(param.first, MIN_P, MAX_P),
            std::clamp(param.second, MIN_P, MAX_P));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLANGLES) {
        constexpr float MIN_V = 0.0f;
        constexpr float MAX_V = 360.0f;
        frostedGlassEffectParams_->edLightAngles = Vector2f(std::clamp(param.first, MIN_V, MAX_V),
            std::clamp(param.second, MIN_V, MAX_V));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLDIR) {
        constexpr float MIN_V = -1.0f;
        constexpr float MAX_V = 1.0f;
        constexpr float EPS = 1e-6f;

        float dx = std::clamp(param.first, MIN_V, MAX_V);
        float dy = std::clamp(param.second, MIN_V, MAX_V);
        if (std::abs(dx) < EPS && std::abs(dy) < EPS) {
            dx = 1.0f;
            dy = 0.0f;
        }
        frostedGlassEffectParams_->edLightDir = Vector2f(dx, dy);
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_MASKLEFTTOP) {
        frostedGlassEffectParams_->maskLeftTop = Vector2f(param.first, param.second);
    }

    HandleSetFrostedGlassEffectRates(tag, param);
}

void GEVisualEffectImpl::HandleSetFrostedGlassEffectRates(const std::string& tag, const std::pair<float, float>& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    constexpr float V_MIN = -20.0f;
    constexpr float V_MAX = 20.0f;

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_BGRATES) {
        frostedGlassEffectParams_->bgRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SDRATES) {
        frostedGlassEffectParams_->sdRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTRATES) {
        frostedGlassEffectParams_->envLightRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLRATES) {
        frostedGlassEffectParams_->edLightRates = Vector2f(std::clamp(param.first, V_MIN, V_MAX),
            std::clamp(param.second, V_MIN, V_MAX));
    }

    constexpr float MIN_V = 0.0f;
    constexpr float MAX_V = 250.0f;
    constexpr float MIN_F = 1.0f;
    constexpr float MAX_F = 250.0f;
    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTPARAMS) {
        frostedGlassEffectParams_->envLightParams = Vector2f(std::clamp(param.first, MIN_V, MAX_V),
            std::clamp(param.second, MIN_F, MAX_F));
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassEffectWeights(
    const std::string& tag, const std::pair<float, float>& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    constexpr float MIN_W = 0.0f;
    constexpr float MAX_W = 5.0f;
    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_WEIGHTSEMBOSS) {
        frostedGlassEffectParams_->weightsEmboss = Vector2f(std::clamp(param.first, MIN_W, MAX_W),
            std::clamp(param.second, MIN_W, MAX_W));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_WEIGHTSEDL) {
        frostedGlassEffectParams_->weightsEdl = Vector2f(std::clamp(param.first, MIN_W, MAX_W),
            std::clamp(param.second, MIN_W, MAX_W));
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassEffectKBS(const std::string& tag, const Vector3f& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    constexpr float KB_MIN = -20.0f;
    constexpr float KB_MAX = 20.0f;
    constexpr float S_MIN = 0.0f;
    constexpr float S_MAX = 20.0f;

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_BGKBS) {
        frostedGlassEffectParams_->bgKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SDKBS) {
        frostedGlassEffectParams_->sdKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTKBS) {
        frostedGlassEffectParams_->envLightKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLKBS) {
        frostedGlassEffectParams_->edLightKBS = Vector3f(std::clamp(param[NUM_0], KB_MIN, KB_MAX),
            std::clamp(param[NUM_1], KB_MIN, KB_MAX), std::clamp(param[NUM_2], S_MIN, S_MAX));
    }
}

void GEVisualEffectImpl::HandleSetFrostedGlassEffectPosNegCoefs(const std::string& tag, const Vector3f& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    constexpr float V_MIN = -20.0f;
    constexpr float V_MAX = 20.0f;

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_BGPOS) {
        frostedGlassEffectParams_->bgPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_BGNEG) {
        frostedGlassEffectParams_->bgNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SDPOS) {
        frostedGlassEffectParams_->sdPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SDNEG) {
        frostedGlassEffectParams_->sdNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTPOS) {
        frostedGlassEffectParams_->envLightPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_ENVLIGHTNEG) {
        frostedGlassEffectParams_->envLightNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLPOS) {
        frostedGlassEffectParams_->edLightPos = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_EDLNEG) {
        frostedGlassEffectParams_->edLightNeg = Vector3f(std::clamp(param[NUM_0], V_MIN, V_MAX),
            std::clamp(param[NUM_1], V_MIN, V_MAX), std::clamp(param[NUM_2], V_MIN, V_MAX));
    }
}

void GEVisualEffectImpl::SetFrostedGlassEffectParams(const std::string& tag, const Vector3f& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    constexpr float MIN_S = -500.0f;
    constexpr float MAX_S = 500.0f;
    constexpr float MIN_V = 0.0f;
    constexpr float MAX_V = 250.0f;
    constexpr float MIN_F = 1.0f;
    constexpr float MAX_F = 250.0f;

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_SDPARAMS) {
        frostedGlassEffectParams_->sdParams = Vector3f(std::clamp(param[NUM_0], MIN_S, MAX_S),
            std::clamp(param[NUM_1], MIN_V, MAX_V), std::clamp(param[NUM_2], MIN_F, MAX_F));
    }

    HandleSetFrostedGlassEffectKBS(tag, param);
    HandleSetFrostedGlassEffectPosNegCoefs(tag, param);

    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_REFRACTPARAMS) {
        constexpr float MIN_R = -1.0f;
        constexpr float MAX_R = 1.0f;
        constexpr float MIN_C = 0.0f;
        constexpr float MAX_C = 10.0f;
        constexpr float MIN_D = 0.0f;
        constexpr float MAX_D = 1.0f;
        frostedGlassEffectParams_->refractParams = Vector3f(std::clamp(param[NUM_0], MIN_R, MAX_R),
            std::clamp(param[NUM_1], MIN_C, MAX_C), std::clamp(param[NUM_2], MIN_D, MAX_D));
    }
}

void GEVisualEffectImpl::SetFrostedGlassEffectParams(const std::string& tag, const Vector4f& param)
{
    if (frostedGlassEffectParams_ == nullptr) {
        return;
    }
    
    if (tag == GE_SHADER_FROSTED_GLASS_EFFECT_MATERIALCOLOR) {
        constexpr float MIN_C = 0.0f;
        constexpr float MAX_C = 1.0f;
        frostedGlassEffectParams_->materialColor = Vector4f(std::clamp(param[NUM_0], MIN_C, MAX_C),
            std::clamp(param[NUM_1], MIN_C, MAX_C), std::clamp(param[NUM_2], MIN_C, MAX_C),
            std::clamp(param[NUM_3], MIN_C, MAX_C));
    }
}

void GEVisualEffectImpl::SetFrostedGlassBlurParams(const std::string& tag, const float& param)
{
    if (frostedGlassBlurParams_ == nullptr) {
        GE_LOGE("GEVisualEffectImpl::SetFrostedGlassBlurParams frostedGlassBlurParams_ is nullptr");
        return;
    }

    if (tag == GE_FILTER_FROSTED_GLASS_BLUR_RADIUS) {
        frostedGlassBlurParams_->radius = param;
    }
    if (tag == GE_FILTER_FROSTED_GLASS_BLUR_REFRACTOUTPX) {
        frostedGlassBlurParams_->refractOutPx = param;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
