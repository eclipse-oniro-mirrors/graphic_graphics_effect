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
#ifndef GRAPHICS_EFFECT_GE_SDF_PATH_SHADER_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_PATH_SHADER_SHAPE_H

#include <queue>

#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {

static constexpr uint32_t BOX_4F_SIZE = 4;
using Box4f = std::array<float, BOX_4F_SIZE>;

struct Grid {
    Box4f bbox;
    std::vector<uint32_t> curveIndices;
};
} // namespace

class GE_EXPORT GESDFPathShaderShape : public GESDFShaderShape {
public:
    using GESDFShaderShape::GenerateDrawingShader;
    using GESDFShaderShape::GenerateDrawingShaderHasNormal;
    GESDFPathShaderShape(const GESDFPathShapeParams& param) : params_(param) {}
    GESDFPathShaderShape(const GESDFPathShaderShape&) = delete;
    virtual ~GESDFPathShaderShape() = default;

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
    void Preprocess(Canvas& canvas, const Rect& rect, bool hasNormal) override;
    static std::vector<std::vector<Vector2f>> GetCurveByPath(const Drawing::Path& path);

    GESDFShapeType GetSDFShapeType() const override
    {
        return GESDFShapeType::PATH;
    }

    const Path& GetPath() const
    {
        return params_.path;
    }

    const Vector2f& GetOffset() const
    {
        return params_.offset;
    }

    void CopyState(const GESDFPathShaderShape& shape)
    {
        params_ = shape.params_;
    }

    bool HasType(const GESDFShapeType type) const override
    {
        return type == GESDFShapeType::PATH;
    }

    static void cubicToQuadraticSingle(const Vector2f& p0, Vector2f& p1, const Vector2f& p2, const Vector2f& p3);

    static float calculateTimeStep(const Vector2f& p0, const Vector2f& p1, const Vector2f& p2, bool isLine);

    static Vector2f calculateCurvePoint(
        const Vector2f& p0, const Vector2f& p1, const Vector2f& p2, const float t, bool isLine);

private:
    std::shared_ptr<Image> RunJFAIterates(Canvas& canvas, std::shared_ptr<Image> seedTex, int width, int height);
    std::shared_ptr<Image> RunSDFPropagation(
        Canvas& canvas, std::shared_ptr<Image> sdfTex, std::shared_ptr<Image> maskTex, int width, int height);
    std::shared_ptr<Image> ComputeDistanceField(
        Canvas& canvas, std::shared_ptr<Image> jfaTex, int width, int height, std::shared_ptr<Image> pathImage);

    std::shared_ptr<Image> GenerateSeedTexture(Canvas& canvas, int width, int height,
        const std::vector<std::vector<Vector2f>>& paramsCoef, std::shared_ptr<Image>& pathImage);
    void UpdateScale(Vector2f& scale, const Drawing::Rect& rect);

    // Helper functions for Preprocess
    Drawing::Path PreparePathForRendering(const Drawing::Rect& rect, float& width, float& height);
    std::vector<Vector2f> ProcessCurveSegments(const std::vector<std::vector<Vector2f>>& paramsCoef);
    void RenderGridsToSurface(const Drawing::Rect& targetRect);

    // ========== Quadtree grid partition ==========
    void AutoGridPartition(float width, float height, float maxThickness);
    void SplitGrid(
        const Grid& current, const std::vector<Box4f>& curveBBoxes, std::queue<Grid>& workQueue, float minGridSize);
    void ComputeAllCurveBoundingBoxes(
        float width, float height, float maxThickness, Box4f& canvasBBox, std::vector<Box4f>& curveBBoxes);
    std::array<float, 4> ComputeCurveBoundingBox(size_t curveIndex, float maxThickness, float width, float height);
    void InitializeWorkQueue(
        const Box4f& canvasBBox, const std::vector<Box4f>& curveBBoxes, std::queue<Grid>& workQueue);
    void ProcessFinalGrid(Grid& current, const std::vector<Box4f>& curveBBoxes);

    // ========== Quadtree grid partition ==========
    void CreateSurfaceAndCanvas(Drawing::Canvas& canvas, const Drawing::Rect& rect);

    std::shared_ptr<Image> DrawPathToImage(Drawing::Canvas& canvas, int width, int height, const Drawing::Path& path);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakePrecalcShaderBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeNormalShaderBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeSdfPropShaderBuilder();


    std::shared_ptr<Image> disResult_ = nullptr;

    GESDFPathShapeParams params_ {};
    uint32_t numPasses_ = 6;
    bool distanceCalc_ = true;

    // Grid partition data storage
    std::vector<std::pair<std::vector<float>, Grid>> curvesInGrid_;
    std::vector<std::vector<float>> segmentIndex_;
    std::vector<float> controlPoints_; // 全局NDC控制点
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> offscreenCanvas_ = nullptr;
    uint32_t numCurves_ = 0;
    uint32_t pointCnt_ = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_PATH_SHADER_SHAPE_H
