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
    // Initialize filter type from name using existing factory
    // TODO: Port initialization logic from GEVisualEffectImpl
}

GEVisualEffectImplV2::~GEVisualEffectImplV2()
{
    // Cleanup handled by smart pointers
}
// #define IMPLEMENT_SET_PARAM_INTERNAL(Type) \
//     void GEVisualEffectImplV2::SetParamInternal(GEParamsMemberTag tag, const Type& value) \
//     { \
//         if (!params_) { \
//             return; \
//         } \
//         GEParamsMemberHelper::SetParamsMemberByTag(params_, tag, value); \
//     }

// FOR_EACH_PARAM_TYPE(IMPLEMENT_SET_PARAM_INTERNAL)
// #undef IMPLEMENT_SET_PARAM_INTERNAL

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
