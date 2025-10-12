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
#ifndef GRAPHICS_EFFECT_GE_SDF_UNION_OP_MASK_H
#define GRAPHICS_EFFECT_GE_SDF_UNION_OP_MASK_H

#include "ge_sdf_shader_mask.h"
#include "ge_shader_filter_params.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {

// REGISTER_GE_FILTER_TYPEINFO(SDF_UNION_OP, GESDFUnionOpShaderMask,
//      Drawing::GESDFUnionOpMaskParams, Drawing::GE_MASK_SDF_UNION_OP);
class GE_EXPORT GESDFUnionOpShaderMask : public GESDFShaderMask {
public:
    GESDFUnionOpShaderMask(const GESDFUnionOpMaskParams& param) : params_(param) {}
    GESDFUnionOpShaderMask(const GESDFUnionOpShaderMask&) = delete;
    virtual ~GESDFUnionOpShaderMask() = default;
    DECLARE_GEFILTER_TYPEFUNC(GESDFUnionOpShaderMask);

    GESDFMaskType GetSDFMaskType() const override
    {
        return GESDFMaskType::UNION_OP;
    }

    inline float GetSpacing() const
    {
        return params_.spacing;
    }

    inline const std::shared_ptr<GESDFShaderMask>& GetLeftSDFMask() const
    {
        return params_.left;
    }

    inline const std::shared_ptr<GESDFShaderMask>& GetRightSDFMask() const
    {
        return params_.right;
    }

    inline const GESDFUnionOp& GetSDFUnionOp() const
    {
        return params_.op;
    }

    void CopyState(const GESDFUnionOpShaderMask& mask);

private:
    GESDFUnionOpMaskParams params_ {};
};
} // Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_UNION_OP_MASK_H
