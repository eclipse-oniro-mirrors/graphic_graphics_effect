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

#ifndef GRAPHICS_EFFECT_GE_EFFECTS_COMMON_PARAMS_TYPES_H
#define GRAPHICS_EFFECT_GE_EFFECTS_COMMON_PARAMS_TYPES_H

/* 
 * Common associate types for params definition
 * DO NOT PUT THESE TYPES IN THIS FILE:
 * 1. ACTUAL params definition
 * 2. Types that only work for exactly ONE param definition
 * 3. Any complex helpers with implementations
 */

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace GEV2 {

struct GERRect {
    float left_ = 0.f;
    float top_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float radiusX_ = 0.f;
    float radiusY_ = 0.f;
};

} // namespace GEV2
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EFFECTS_RRECT_H