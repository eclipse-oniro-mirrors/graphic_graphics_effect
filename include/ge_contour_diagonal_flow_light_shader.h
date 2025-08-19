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

#include <queue>
#include "common/rs_vector4.h"
#include "effect/runtime_shader_builder.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "utils/matrix.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

const int box4fSize = 4;
using Box4f = std::array<float, box4fSize>;

struct Grid {
    Box4f bbox;
    std::vector<int> curveIndices;
    bool curvesNumExceedLimit = false;
};

class GEKawaseBlurShaderFilter;
REGISTER_GEFILTER_TYPEINFO(CONTOUR_DIAGONAL_FLOW_LIGHT, GEContourDiagonalFlowLightShader,
                           Drawing::GEContentDiagonalFlowLightShaderParams, Drawing::GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT);
class GE_EXPORT GEContourDiagonalFlowLightShader : public GEShader {

public:
    GEContourDiagonalFlowLightShader();
    GEContourDiagonalFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& contourDiagonalFlowLightParams);
    ~GEContourDiagonalFlowLightShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEContourDiagonalFlowLightShader);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    const std::string GetDescription() const { return "GEContourDiagonalFlowLightShader"; }
    void SetGEContentDiagonalFlowLightShaderParams(const Drawing::GEContentDiagonalFlowLightShaderParams& params)
    {
        contourDiagonalFlowLightParams_ = params;
    }
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;
    void DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;
private:
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&&) = delete;
    std::shared_ptr<Drawing::Image> DrawRuntimeShader(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    std::shared_ptr<Drawing::Image> BlendImg(Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::Image> precalculationImg,
        std::shared_ptr<Drawing::Image> lightImg, std::shared_ptr<Drawing::Image> haloImg);
    static std::shared_ptr<GEContourDiagonalFlowLightShader>
        CreateFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& param);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetFlowLightPrecalBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> FlowLightConvertBuilder();
    Box4f ComputeCurveBoundingBox(size_t curveIndex, float maxThickness, int width, int height, float& approxLenPixels);
    void CreateSurfaceAndCanvas(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    void PreCalculateRegion(Drawing::Canvas& mainCanvas, Drawing::Canvas& canvas, int gridIndex,
        const Drawing::Rect& wholeRect, const Drawing::Rect& rect);
    void AutoPartitionCal(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    void AutoGridPartition(int width, int height, float maxThickness);
    void ComputeAllCurveBoundingBoxes(int width, int height, float maxThickness,
        Box4f& canvasBBox, std::vector<Box4f>& curveBBoxes);
    void InitializeWorkQueue(const Box4f& canvasBBox, const std::vector<Box4f>& curveBBoxes,
        std::queue<Grid>& workQueue);
    void SplitGrid(const Grid& current, const std::vector<Box4f>& curveBBoxes,
        std::queue<Grid>& workQueue, float minGridSize);
    void ProcessFinalGrid(Grid& current, const std::vector<Box4f>& curveBBoxes);
    std::shared_ptr<Drawing::Image> CreateImg(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    std::shared_ptr<Drawing::Image> CreateDrawImg(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::Brush& brush);

    void ConvertImg(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        std::shared_ptr<Drawing::Image> sdfImg);
    std::shared_ptr<Drawing::Image> LoopAllCurvesInBatches(Drawing::Canvas& mainCanvas, Drawing::Canvas& canvas,
        int gridIndex, const Drawing::Rect& wholeRect, const Drawing::Rect& rect);
    void ResizeCurvesData(int gridIndex, size_t subCurveCnt, size_t perSubCurveSize);
private:
    int numCurves_ = 0;
    int capacity_ = 0;
    std::shared_ptr<Drawing::Surface> offscreenSurface_;
    std::shared_ptr<Drawing::Canvas> offscreenCanvas_;
    Drawing::GEContentDiagonalFlowLightShaderParams contourDiagonalFlowLightParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
    std::vector<float> controlPoints_{}; // fix 256 X 2 floats for 128 curves
    size_t pointCnt_ = 0; // real input Point Cnt
    std::shared_ptr<GEKawaseBlurShaderFilter> blurShader_ = nullptr;

    // grid : curves, boundingbox(xmin, xmax, ymin, ymax)
    std::vector<std::pair<std::vector<float>, Box4f>> curvesInGrid_{};
    std::vector<std::vector<float>> segmentIndex_{};
    std::vector<float> curveWeightPrefix_{};
    std::vector<float> curveWeightCurrent_{};
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_CONTOUR_DIAGONAL_FLOW_LIGHT_SHADER_H
