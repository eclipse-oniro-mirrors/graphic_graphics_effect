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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H

#include <memory>
#include <vector>

#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

#include "ge_shader_filter_params.h"
#include "ge_common.h"
#include "ge_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr size_t POINT_NUM = 12;
constexpr float MAX_CURVE_X = 1.70000000;   // MAX rounded area of a unit rrect

enum class DrawingPaintType { NONE, BRUSH, PEN, PAINT, BRUSH_PEN };

class GEVisualEffectImpl;

class GE_EXPORT GEVisualEffect {
public:
    GEVisualEffect(const std::string& name, DrawingPaintType type = DrawingPaintType::BRUSH,
        const std::optional<Drawing::CanvasInfo>& canvasInfo = std::nullopt);
    ~GEVisualEffect();

    void SetParam(const std::string& tag, int32_t param);
    void SetParam(const std::string& tag, int64_t param);
    void SetParam(const std::string& tag, float param);
    void SetParam(const std::string& tag, double param);
    void SetParam(const std::string& tag, const char* const param);

    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param) {}
    void SetParam(const std::string& tag, const Drawing::Matrix param);
    void SetParam(const std::string& tag, const std::pair<float, float>& param);
    void SetParam(const std::string& tag, const std::vector<std::pair<float, float>>&);
    void SetParam(const std::string& tag, const std::vector<Vector2f>& param);
    void SetParam(const std::string& tag, const std::vector<Vector4f>& param);
    void SetParam(const std::string& tag, const std::array<Drawing::Point, POINT_NUM>& param);
    void SetParam(const std::string& tag, bool param);
    void SetParam(const std::string& tag, uint32_t param);
    void SetParam(const std::string& tag, const std::vector<float>& param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderMask> param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::GEShaderShape> param);
    void SetParam(const std::string& tag, const Drawing::Color4f& param);
    void SetParam(const std::string& tag, const Vector3f& param);
    void SetParam(const std::string& tag, const Vector4f& param);
    void SetParam(const std::string& tag, const GERRect& param);
    void SetParam(const std::string& tag, const GESDFBorderParams& param);
    void SetParam(const std::string& tag, const GESDFShadowParams& param);

    const std::shared_ptr<Drawing::GEShaderShape> GetGEShaderShape(const std::string& tag) const;

    const std::string& GetName() const
    {
        return visualEffectName_;
    }

    const std::shared_ptr<GEVisualEffectImpl> GetImpl() const
    {
        return visualEffectImpl_;
    }

    void SetCanvasInfo(Drawing::CanvasInfo info);

    const Drawing::CanvasInfo& GetCanvasInfo() const;

    void SetSupportHeadroom(float headroom)
    {
        supportHeadroom_ = headroom;
    }

    float GetSupportHeadroom() const
    {
        return supportHeadroom_;
    }

    // refer to skia-SkContinuousRRect.cpp
    static inline bool CanBeContinuous(const std::shared_ptr<GESDFRRectShapeParams>& params)
    {
        float commonRadius = params->rrect.radiusX_;
        float extendsRadius = params->rrect.radiusX_ * MAX_CURVE_X;
        if (extendsRadius <= fmin(params->rrect.width_, params->rrect.height_) * 0.5f) {
            return true;
        } else if (ROSEN_EQ(commonRadius, params->rrect.height_ * 0.5f) &&
                   (extendsRadius < params->rrect.width_ * 0.5f)) {
            return true;
        } else if (ROSEN_EQ(commonRadius, params->rrect.width_ * 0.5f) &&
                   (extendsRadius < params->rrect.height_ * 0.5f)) {
            return true;
        }
        return false;
    }

    const std::shared_ptr<Drawing::GEShaderMask> GenerateShaderMask() const;
    const std::shared_ptr<Drawing::GEShaderShape> GenerateShaderShape() const;

private:
    std::shared_ptr<Drawing::GEShaderShape> GenerateExtShaderRRect(
        const std::shared_ptr<GESDFRRectShapeParams>& params) const;
    std::string visualEffectName_;
    DrawingPaintType type_;
    std::shared_ptr<GEVisualEffectImpl> visualEffectImpl_;
    float supportHeadroom_ = 0.0f;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H
