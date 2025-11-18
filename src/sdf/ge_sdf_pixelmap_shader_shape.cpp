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

#include "sdf/ge_sdf_pixelmap_shader_shape.h"
#include "ge_log.h"
#include "utils/ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GenerateDrawingShader(float width, float height) const
{
    GE_TRACE_NAME_FMT("GESDFPixelmapShaderShape::GenerateDrawingShader, Width: %g, Height: %g", width, height);
    if (params_.image == nullptr) {
        LOGE("GESDFPixelmapShaderShape::GenerateDrawingShader has empty image");
        return nullptr;
    }
    Drawing::Matrix matrix;
    auto sdfPixelmapShapeShader = Drawing::ShaderEffect::CreateImageShader(*params_.image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    return sdfPixelmapShapeShader;
}

std::shared_ptr<ShaderEffect> GESDFPixelmapShaderShape::GenerateDrawingShaderHasNormal(float width, float height) const
{
    return GenerateDrawingShader(width, height);
}
} // Drawing
} // namespace Rosen
} // namespace OHOS
