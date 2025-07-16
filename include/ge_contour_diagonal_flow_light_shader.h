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
 
#ifndef GRAPHICS_EFFECT_CONTOUR_DIAGONAL_FLOW_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_CONTOUR_DIAGONAL_FLOW_LIGHT_SHADER_H
#include <any>

#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "common/rs_vector4.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEContourDiagonalFlowLightShader : public GEShader {
public:
    GEContourDiagonalFlowLightShader();
    GEContourDiagonalFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& contourDiagonalFlowLightParams);

    ~GEContourDiagonalFlowLightShader() override = default;

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    const std::string GetDescription() const { return "GEContourDiagonalFlowLightShader"; }

    void SetGEContentDiagonalFlowLightShaderParams(const Drawing::GEContentDiagonalFlowLightShaderParams& params)
    {
        contourDiagonalFlowLightParams_ = params;
    }

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    static std::shared_ptr<GEContourDiagonalFlowLightShader>
        CreateContourDiagonalFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& param);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightBuilder();

    std::shared_ptr<Drawing::Image> MakeContourDiagonalFlowLightPrecalculationShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightPrecalculationBuilder();

    std::shared_ptr<Drawing::Image> MakeContourDiagonalFlowLightHaloAtlasShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightHaloAtlasBuilder();

private:
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&&) = delete;
    std::shared_ptr<Drawing::Image> DrawRuntimeShader(Drawing::Canvas& canvas, const Drawing::Rect& rect);

    Drawing::GEContentDiagonalFlowLightShaderParams contourDiagonalFlowLightParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
    std::vector<float> controlPoints_{}; // fix 64 X 2
    size_t pointCnt_ = 0; // real input Point Cnt
};
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H