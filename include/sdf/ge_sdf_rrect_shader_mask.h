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

#ifndef GRAPHICS_EFFECT_GE_SDF_RRECT_MASK_H
#define GRAPHICS_EFFECT_GE_SDF_RRECT_MASK_H

#include "ge_sdf_shader_mask.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// REGISTER_GE_FILTER_TYPEINFO(SDF_RRECT_MASK, GESDF_RRectShaderMask,
//      Drawing::GESDFRRectMaskParams, Drawing::GE_MASK_SDF_RRECT_MASK);
class GE_EXPORT GESDFRRectShaderMask : public GESDFShaderMask {
public:
    GESDFRRectShaderMask(const GESDFRRectMaskParams& param) : params_(param) {}
    GESDFRRectShaderMask(const GESDFRRectShaderMask&) = delete;
    virtual ~GESDFRRectShaderMask() = default;
    // DECLARE_GEFILTER_TYPEFUNC(GESDFRRectShaderMask);

    GESDFMaskType GetSDFMaskType() const override
    {
        return GESDFMaskType::RRECT;
    }

    const RRect& GetRRect() const
    {
        return params_.rrect;
    }

    void CopyState(const GESDFRRectShaderMask& mask)
    {
        params_ = mask.params_;
    }

private:
    GESDFRRectMaskParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_RRECT_MASK_H
