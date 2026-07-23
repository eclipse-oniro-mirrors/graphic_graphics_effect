/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ge_sdf_shader_shape.h"

#include "draw/surface.h"
#include "ge_sdf_pixelmap_shader_shape.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_sub_op_shader_shape.h"
#include "ge_sdf_transform_shader_shape.h"
#include "ge_sdf_union_op_shader_shape.h"
#include "ge_shader_diagnostics.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
static constexpr char SHAPE_SDF_PROG[] = R"(
    uniform shader sdfNormalImg;
    vec4 main(vec2 fragCoord)
    {
        vec4 c1 = sdfNormalImg.eval(fragCoord).rgba;
        return c1;
    }
)";

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_shapeSDFShaderEffect = nullptr;
}

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShader(float width, float height) const
{
    return nullptr;
}

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    return nullptr;
}

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShader(Canvas& canvas, float width, float height)
{
    Preprocess(canvas, Rect(0.0, 0.0, width, height), false);
    return GenerateDrawingShader(width, height);
}

std::shared_ptr<ShaderEffect> GESDFShaderShape::GenerateDrawingShaderHasNormal(Canvas& canvas,
    float width, float height)
{
    Preprocess(canvas, Rect(0.0, 0.0, width, height), true);
    return GenerateDrawingShaderHasNormal(width, height);
}

void GESDFShaderShape::CopyState(const GESDFShaderShape& shape)
{
    if (GetSDFShapeType() != shape.GetSDFShapeType()) {
        LOGE("GESDFShaderShape::CopyState, forbid to copy state from different type shape!");
        return;
    }
    if (GetSDFShapeType() == GESDFShapeType::RRECT) {
        const auto* rectShape = static_cast<const GESDFRRectShaderShape*>(&shape);
        auto* thisRect = static_cast<GESDFRRectShaderShape*>(this);
        thisRect->CopyState(*rectShape);
    } else if (GetSDFShapeType() == GESDFShapeType::UNION_OP) {
        const auto* unionShape = static_cast<const GESDFUnionOpShaderShape*>(&shape);
        auto* thisUnion = static_cast<GESDFUnionOpShaderShape*>(this);
        thisUnion->CopyState(*unionShape);
    } else if (GetSDFShapeType() == GESDFShapeType::SUB_OP) {
        const auto* subOpShape = static_cast<const GESDFSubOpShaderShape*>(&shape);
        auto* thisSubOp = static_cast<GESDFSubOpShaderShape*>(this);
        thisSubOp->CopyState(*subOpShape);
    } else if (GetSDFShapeType() == GESDFShapeType::SMOOTH_SUB_OP) {
        const auto* smoothSubOpShape = static_cast<const GESDFSmoothSubOpShaderShape*>(&shape);
        auto* thisSmoothSubOp = static_cast<GESDFSmoothSubOpShaderShape*>(this);
        thisSmoothSubOp->CopyState(*smoothSubOpShape);
    } else if (GetSDFShapeType() == GESDFShapeType::TRANSFORM) {
        const auto* transformShape = static_cast<const GESDFTransformShaderShape*>(&shape);
        auto* thisTransform = static_cast<GESDFTransformShaderShape*>(this);
        thisTransform->CopyState(*transformShape);
    } else if (GetSDFShapeType() == GESDFShapeType::PIXELMAP) {
        const auto* pixelmapShape = static_cast<const GESDFPixelmapShaderShape*>(&shape);
        auto* thisPixelmap = static_cast<GESDFPixelmapShaderShape*>(this);
        thisPixelmap->CopyState(*pixelmapShape);
    }
}

bool GESDFShaderShape::TryGetCenterAndHalfSize(float& outX, float& outY, Vector2f& shapeHalfSize) const
{
    LOGE("GESDFShaderShape::TryGetCenterAndHalfSize error");
    return false;
}

std::shared_ptr<Drawing::Image> GESDFShaderShape::MakeSDFImage(Canvas& canvas,
    float width, float height, bool hasNormal)
{
    if (g_shapeSDFShaderEffect == nullptr) {
        Drawing::RuntimeEffectOptions reo{};
        reo.needDrawingslToSksl = false;
        reo.useHighpLocalCoords = true;
        g_shapeSDFShaderEffect = GECreateRuntimeEffectForShader(SHAPE_SDF_PROG, reo);
    }
    if (g_shapeSDFShaderEffect == nullptr) {
        GE_LOGE("GESDFShaderShape::MakeSDFImage create runtime error");
        return nullptr;
    }
    auto shader = hasNormal ? GenerateDrawingShaderHasNormal(canvas, width, height) :
        GenerateDrawingShader(canvas, width, height);
    if (shader == nullptr) {
        GE_LOGE("GESDFShaderShape::MakeSDFImage create shader error");
        return nullptr;
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(g_shapeSDFShaderEffect);
    builder->SetChild("sdfNormalImg", shader);
    auto canvasSurface = canvas.GetSurface();
    std::shared_ptr<Drawing::ColorSpace> colorSpace = canvasSurface ?
        canvasSurface->GetImageInfo().GetColorSpace() : nullptr;
    Drawing::Matrix makeMatrix;
    Drawing::ImageInfo imageInfo(width, height, Drawing::ColorType::COLORTYPE_RGBA_F16,
        Drawing::AlphaType::ALPHATYPE_OPAQUE, colorSpace);
    auto context = canvas.GetGPUContext().get();
    return context ? builder->MakeImage(context, &(makeMatrix), imageInfo, false) : nullptr;
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
