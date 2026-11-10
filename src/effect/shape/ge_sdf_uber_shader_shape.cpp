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

#include "ge_sdf_uber_shader_shape.h"

#include <cmath>

#include "common/rs_common_def.h"
#include "ge_log.h"
#include "ge_sdf_transform_shader_shape.h"
#include "ge_sdf_union_op_shader_shape.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr float SPACING_EPSILON = 0.0001f;
constexpr float HALF = 0.5f;
} // namespace

bool GESdfUberShaderShape::WalkUberLeaves(const std::shared_ptr<GESDFShaderShape>& shape, const Drawing::Matrix& acc,
    GESDFUberTreeDesc& desc, std::vector<GESDFUberLeafPos>& leaves)
{
    if (!shape) {
        return true;
    }
    switch (shape->GetSDFShapeType()) {
        case GESDFShapeType::UNION_OP: {
            const auto* opShape = static_cast<const GESDFUnionOpShaderShape*>(shape.get());
            bool smooth = opShape->GetSDFUnionOp() == GESDFUnionOp::SMOOTH_UNION;
            if (!desc.opResolved) {
                desc.opResolved = true;
                desc.isSmoothUnion = smooth;
                desc.spacing = opShape->GetSpacing();
            } else if (smooth != desc.isSmoothUnion ||
                std::abs(opShape->GetSpacing() - desc.spacing) > SPACING_EPSILON) {
                GE_LOGD("WalkUberLeaves: mixed op type or spacing, fall back");
                return false;
            }
            return WalkUberLeaves(opShape->GetLeftSDFShape(), acc, desc, leaves) &&
                WalkUberLeaves(opShape->GetRightSDFShape(), acc, desc, leaves);
        }
        case GESDFShapeType::TRANSFORM: {
            const auto* transformShape = static_cast<const GESDFTransformShaderShape*>(shape.get());
            if (transformShape->GetUnionMode() != 0) {
                GE_LOGD("WalkUberLeaves: gravity pull transform not supported, fall back");
                return false;
            }
            const Drawing::Matrix& matrix = transformShape->GetMatrix();
            if (matrix.IsIdentity()) {
                return WalkUberLeaves(transformShape->GetShape(), acc, desc, leaves);
            }
            Drawing::Matrix invertMatrix;
            if (!matrix.Invert(invertMatrix)) {
                GE_LOGD("WalkUberLeaves: matrix invert fail, fall back");
                return false;
            }
            Drawing::Matrix childAcc = acc;
            childAcc.PostConcat(invertMatrix);
            return WalkUberLeaves(transformShape->GetShape(), childAcc, desc, leaves);
        }
        default: {
            ++desc.leafCount;
            GESDFUberLeafPos pos;
            pos.leaf = shape;
            pos.parentToLocal = acc;
            leaves.push_back(std::move(pos));
            return true;
        }
    }
}

GEUberBasicShapeType GESdfUberShaderShape::ClassifyUberRRect(const GERRect& rrect)
{
    if (!rrect.HasUniformCornerRadii() || !rrect.HasCircularCornerRadii()) {
        return GEUberBasicShapeType::G1_UNUNIFORM_RRECT;
    }
    float commonRadius = rrect.GetCommonRadiusX();
    float minWH = std::min(rrect.width_, rrect.height_);
    if (commonRadius * UBER_MAX_CURVE_X <= minWH * HALF) {
        return GEUberBasicShapeType::G2_RRECT;
    }
    if (ROSEN_GE(commonRadius, minWH * HALF)) {
        return rrect.width_ <= rrect.height_ ? GEUberBasicShapeType::V_CAPSULE : GEUberBasicShapeType::H_CAPSULE;
    }
    return GEUberBasicShapeType::G1_UNIFORM_RRECT;
}

std::shared_ptr<ShaderEffect> GESdfUberShaderShape::GenerateDrawingShader(float width, float height) const
{
    if (!params_.rootShape) {
        GE_LOGE("GESdfUberShaderShape::GenerateDrawingShader: inner shape null");
        return nullptr;
    }
    auto uberShader = GenerateUberShader(width, height, false);
    if (uberShader != nullptr) {
        return uberShader;
    }
    GE_LOGD("GESdfUberShaderShape::GenerateDrawingShader fall back to inner tree, UberType: %{public}d",
        params_.uberType);
    return params_.rootShape->GenerateDrawingShader(width, height);
}

std::shared_ptr<ShaderEffect> GESdfUberShaderShape::GenerateDrawingShaderHasNormal(
    float width, float height) const
{
    if (!params_.rootShape) {
        GE_LOGE("GESdfUberShaderShape::GenerateDrawingShaderHasNormal: inner shape null");
        return nullptr;
    }
    auto uberShader = GenerateUberShader(width, height, true);
    if (uberShader != nullptr) {
        return uberShader;
    }
    GE_LOGD("GESdfUberShaderShape::GenerateDrawingShaderHasNormal fall back to inner tree, UberType: %{public}d",
        params_.uberType);
    return params_.rootShape->GenerateDrawingShaderHasNormal(width, height);
}

bool GESdfUberShaderShape::HasType(const GESDFShapeType type) const
{
    return params_.rootShape ? params_.rootShape->HasType(type) : false;
}

std::shared_ptr<ShaderEffect> GESdfUberShaderShape::GenerateUberShader(
    float width, float height, bool hasNormal) const
{
    GE_LOGD("GenerateUberShader: uber type %{public}d has no engine, fall back to inner tree", params_.uberType);
    return nullptr;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
