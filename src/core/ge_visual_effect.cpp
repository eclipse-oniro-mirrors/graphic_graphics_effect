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

#include "core/ge_effect_factory.h"
#include "ge_log.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"
#include "ge_sdf_transform_shader_shape.h"
#include "ge_sdf_union_op_shader_shape.h"

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

void GEVisualEffect::SetParam(const std::string& tag, const Drawing::Matrix param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::pair<float, float>& param)
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

void GEVisualEffect::SetParam(const std::string& tag, const RectF& param)
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

const std::shared_ptr<Drawing::GEShaderShape> GEVisualEffect::GetGEShaderShape(const std::string& tag) const
{
    return visualEffectImpl_->GetGEShaderShape(tag);
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
    if (visualEffectImpl_ == nullptr) {
        return nullptr;
    }

    return GraphicsEffectEngine::GEEffectFactory::CreateMask(visualEffectImpl_);
}

const std::shared_ptr<Drawing::GEShaderShape> GEVisualEffect::GenerateShaderShape() const
{
    if (visualEffectImpl_ == nullptr) {
        return nullptr;
    }

    return GraphicsEffectEngine::GEEffectFactory::CreateShape(visualEffectImpl_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
