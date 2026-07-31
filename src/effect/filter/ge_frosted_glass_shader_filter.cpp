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
#include "ge_frosted_glass_shader_filter.h"

#include "ge_log.h"
#include "ge_shader_diagnostics.h"

namespace OHOS {
namespace Rosen {
static constexpr char MAIN_SHADER_PROG[] = R"(
    // Frosted glass shader: composites the source screenshot with a material tint
    // masked by the SDF normal field. Only image / sdfNormalImg / materialColor are used.
    uniform shader image;
    uniform shader sdfNormalImg;
    uniform vec4 materialColor;

    vec4 main(vec2 fragCoord)
    {
        vec4 screenshot = image.eval(fragCoord);
        float sd = sdfNormalImg.eval(fragCoord).a;
        float glassMask = clamp(-sd, 0.0, 1.0);
        vec4 glassTint = materialColor;
        vec4 glassColor = vec4(
            mix(screenshot.rgb, glassTint.rgb, glassTint.a),
            screenshot.a);
        return glassColor * glassMask;
    }
)";

GEFrostedGlassShaderFilter::GEFrostedGlassShaderFilter(const Drawing::GEFrostedGlassShaderFilterParams& params)
{
    frostedGlassParams_ = params;
}

namespace {
template<typename V>
inline V Interpolate(const V& base, const V& dark, float t)
{
    return base * (1.0f - t) + dark * t;
}
} // namespace

namespace FrostedGlass {
void InterpolateAdaptiveParams(Drawing::GEFrostedGlassShaderFilterParams& params)
{
    const auto& t = params.darkScale;
    // Only interpolate adaptive parameters defined in the header:
    // Dark-mode set for FrostedGlass shader filter params.
    params.weightsEmboss = Interpolate(params.weightsEmboss, params.darkModeWeightsEmboss, t);
    params.bgRates = Interpolate(params.bgRates, params.darkModeBgRates, t);
    params.bgKBS = Interpolate(params.bgKBS, params.darkModeBgKBS, t);
    params.bgPos = Interpolate(params.bgPos, params.darkModeBgPos, t);
    params.bgNeg = Interpolate(params.bgNeg, params.darkModeBgNeg, t);
    params.edLightAngles = Interpolate(params.edLightAngles, params.darkModeEdLightAngles, t);
    params.edLightKBS = Interpolate(params.edLightKBS, params.darkModeEdLightKBS, t);
}
} // namespace FrostedGlass

Drawing::Matrix GEFrostedGlassShaderFilter::GetSampleMatrix()
{
    Drawing::Matrix canvasMatrix = canvasInfo_.mat;
    canvasMatrix.PostTranslate(-canvasInfo_.materialDst.GetLeft(), -canvasInfo_.materialDst.GetTop());

    Drawing::Matrix sampleMatrix;
    sampleMatrix.PostTranslate(
        canvasInfo_.materialDst.GetLeft() - canvasInfo_.tranX, canvasInfo_.materialDst.GetTop() - canvasInfo_.tranY);
    Drawing::Matrix invertSampleMatrix;
    if (!sampleMatrix.Invert(invertSampleMatrix)) {
        GE_LOGE("GEFrostedGlassShaderFilter::GetSampleMatrix invert sampleMatrix failed");
        return Drawing::Matrix();
    }

    Drawing::Matrix invertMatrixImg;
    if (!canvasMatrix.Invert(invertMatrixImg)) {
        GE_LOGE("GEFrostedGlassShaderFilter::GetSampleMatrix invert canvasMatrix failed");
        return Drawing::Matrix();
    }
    invertMatrixImg.PreConcat(invertSampleMatrix);
    return invertMatrixImg;
}

bool GEFrostedGlassShaderFilter::PrepareDrawing(const std::shared_ptr<Drawing::Image> image, Drawing::Matrix& matrix,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& builder)
{
    if (image->GetHeight() < 1e-6 || image->GetWidth() < 1e-6) {
        GE_LOGE("GEFrostedGlassShaderFilter::PrepareDrawing imageinfo is invalid");
        return false;
    }
    matrix = canvasInfo_.mat;

    auto sdfNormalShader = MakeSDFNormalShader(Drawing::RectF(0, 0, canvasInfo_.geoWidth, canvasInfo_.geoHeight));
    if (!sdfNormalShader) {
        GE_LOGE("GEFrostedGlassShaderFilter::PrepareDrawing sdfNormalShader is null");
        return false;
    }

    auto imageSampleMatrix = GetSampleMatrix();
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), imageSampleMatrix);
    if (shader == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::PrepareDrawing create image shader failed");
        return false;
    }

    builder = MakeFrostedGlassShader(shader, sdfNormalShader);
    return builder != nullptr;
}

std::shared_ptr<Drawing::Image> GEFrostedGlassShaderFilter::GetOriginImage(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src)
{
    auto originImage = std::make_shared<Drawing::Image>();
    auto srcRect = src;
    srcRect.Offset(
        canvasInfo_.materialDst.GetLeft() - canvasInfo_.tranX, canvasInfo_.materialDst.GetTop() - canvasInfo_.tranY);
    auto gpuCtx = canvas.GetGPUContext();
    if (gpuCtx == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::GetOriginImage canvas context is null");
        return nullptr;
    }
    originImage->BuildSubset(
        image, Drawing::RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetRight(), srcRect.GetBottom()), *gpuCtx);
    return originImage;
}

std::shared_ptr<Drawing::Image> GEFrostedGlassShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::OnProcessImage input is invalid");
        return nullptr;
    }
    Drawing::Matrix matrix;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder;
    if (!PrepareDrawing(image, matrix, builder)) {
        return GetOriginImage(canvas, image, src);
    }
    if (builder == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::OnProcessImage builder is null");
        return GetOriginImage(canvas, image, src);
    }
    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), &matrix, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::OnProcessImage resultImage is null");
        return GetOriginImage(canvas, image, src);
    }
    return resultImage;
}

bool GEFrostedGlassShaderFilter::OnDrawImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::Brush& brush)
{
    if (image == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::OnDrawImage input is invalid");
        return false;
    }
    Drawing::Matrix matrix;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder;
    if (!PrepareDrawing(image, matrix, builder)) {
        return false;
    }
    if (builder == nullptr) {
        GE_LOGE("GEFrostedGlassShaderFilter::OnDrawImage builder is null");
        return false;
    }

    Drawing::Matrix canvasMatrix = canvasInfo_.mat;
    canvasMatrix.PostTranslate(-canvasInfo_.materialDst.GetLeft(), -canvasInfo_.materialDst.GetTop());
    canvasMatrix.PostConcat(CreateDestinationTranslateMatrix(dst));
    auto shader = builder->MakeShader(&canvasMatrix, image->IsOpaque());
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    return true;
}

std::shared_ptr<Drawing::ShaderEffect> GEFrostedGlassShaderFilter::MakeSDFNormalShader(const Drawing::Rect& dst) const
{
    auto shape = frostedGlassParams_.sdfShape;
    if (!shape) {
        return nullptr;
    }
    return shape->GenerateDrawingShaderHasNormal(dst.GetWidth(), dst.GetHeight());
}

thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_frostedGlassShaderEffect = nullptr;

bool GEFrostedGlassShaderFilter::InitFrostedGlassEffect()
{
    if (g_frostedGlassShaderEffect == nullptr) {
        g_frostedGlassShaderEffect = GECreateRuntimeEffectForShader(MAIN_SHADER_PROG);
        if (g_frostedGlassShaderEffect == nullptr) {
            GE_LOGE("GEFrostedGlassShaderFilter::InitFrostedGlassEffect create runtime effect failed");
            return false;
        }
    }
    return true;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEFrostedGlassShaderFilter::MakeFrostedGlassShader(
    std::shared_ptr<Drawing::ShaderEffect> imageShader, std::shared_ptr<Drawing::ShaderEffect> sdfNormalShader)
{
    if (g_frostedGlassShaderEffect == nullptr) {
        if (!InitFrostedGlassEffect()) {
            GE_LOGE("GEFrostedGlassShaderFilter::MakeFrostedGlassShader failed when initializing effect");
            return nullptr;
        }
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(g_frostedGlassShaderEffect);
    builder->SetChild("image", imageShader);
    builder->SetChild("sdfNormalImg", sdfNormalShader);
    float materialColor[] = { frostedGlassParams_.materialColor.x_, frostedGlassParams_.materialColor.y_,
        frostedGlassParams_.materialColor.z_, frostedGlassParams_.materialColor.w_ };
    builder->SetUniform("materialColor", materialColor, 4); // 4 means vec4
    return builder;
}
} // namespace Rosen
} // namespace OHOS