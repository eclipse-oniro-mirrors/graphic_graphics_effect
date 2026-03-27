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
#include "core/ge_visual_effect_impl_v2.h"
#include "effect/ge_params_reflection_v2.h"
#include "ge_log.h"
#include "ge_external_dynamic_loader.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffectImplV2::GEVisualEffectImplV2(
    const std::string& name, const std::optional<Drawing::CanvasInfo>& canvasInfo)
    : canvasInfo_(canvasInfo ? *canvasInfo : Drawing::CanvasInfo{})
{
    // Initialize filter type from name using generated helper
    filterType_ = GEV2::GEParamsBuilder::GetFilterTypeFromString(name);
    
    // Build params of appropriate type using generated helper
    params_ = GEV2::GEParamsBuilder::Build(filterType_);
}

GEVisualEffectImplV2::~GEVisualEffectImplV2()
{
}

const std::shared_ptr<Drawing::GEShaderShape> GEVisualEffectImplV2::GetGEShaderShape(const std::string& tag) const
{
    auto shaderShapeTag = GEV2::GEParamsMemberHelper::GEParamsMemberTagFromString(tag);
    if (shaderShapeTag == GEV2::GEParamsMemberTag::INVALID) {
        return nullptr;
    }

    auto tagType = GEV2::GEParamsMemberHelper::GetFilterTypeFromTag(shaderShapeTag);
    if (tagType != params_->GetType()) {
        return nullptr;
    }

    // TODO: According to GEVisualEffectImpl::GetGEShaderShape, this migration should be delegated to those two tags' 
    // getter, check the type and convert back
    // FROSTED_GLASS_SDF_SHAPE
    // FROSTED_GLASS_EFFECT_SDF_SHAPE

    return nullptr;
}

#define IMPLEMENT_SET_PARAM_INTERNAL(Type) \
    void GEVisualEffectImplV2::SetParamInternal(GEV2::GEParamsMemberTag tag, const Type& value) \
    { \
        if (!params_) { \
            return; \
        } \
        GEV2::GEParamsMemberHelper::SetParamsMemberByTag(params_, tag, value); \
    }

FOR_EACH_PARAM_TYPE(IMPLEMENT_SET_PARAM_INTERNAL)
#undef IMPLEMENT_SET_PARAM_INTERNAL

void GEVisualEffectImplV2::SetSDFEdgeLightParams(const std::string& tag, float param)
{
    SetParam(tag, param);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
