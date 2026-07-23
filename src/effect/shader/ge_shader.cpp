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
#include "ge_shader.h"

#include "draw/surface.h"
#include "ge_cache_helper.h"
#include "ge_log.h"
#include "ge_trace.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif
namespace OHOS {
namespace Rosen {
namespace {
struct SDFImageCache {
    uint32_t hash = 0;
    std::shared_ptr<Drawing::Image> sdfImage = nullptr;
};
auto PackSDFCacheAnyFunc = GECacheHelper::PackCacheAny<SDFImageCache>;
auto UnpackSDFCacheAnyOrDefaultFunc = GECacheHelper::UnpackCacheAnyOrDefault<SDFImageCache>;

uint32_t CalHash(std::shared_ptr<Drawing::ColorSpace> colorSpace, uint32_t hashIn = 0)
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    uint32_t hashOut = hashIn;
    auto colorData = colorSpace ? colorSpace->Serialize() : nullptr;
    int32_t colorNull = -1;
    hashOut = colorData ? hashFunc(colorData->GetData(), colorData->GetSize(), hashOut) :
        hashFunc(&colorNull, sizeof(colorNull), hashOut);
    return hashOut;
}

uint32_t CalHash(Drawing::Size size, bool hasNormal, uint32_t shapeHash,
    std::shared_ptr<Drawing::ColorSpace> colorSpace)
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    uint32_t hashOut = CalHash(colorSpace);
    int width = static_cast<int>(size.Width());
    int height = static_cast<int>(size.Height());
    hashOut = hashFunc(&width, sizeof(width), hashOut);
    hashOut = hashFunc(&height, sizeof(height), hashOut);
    uint8_t normalFlag = hasNormal ? 1 : 0;
    hashOut = hashFunc(&normalFlag, sizeof(normalFlag), hashOut);
    hashOut = hashFunc(&shapeHash, sizeof(shapeHash), hashOut);
    return hashOut;
}
}

bool GEShader::TryDrawShaderWithPen(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    auto subRect = GetSubtractedRect(width, height);
    bool rectNotValid = subRect.IsEmpty() || !subRect.IsValid();
    if (rectNotValid) {
        return false;
    }
    auto renderRect = Drawing::Rect(0.f, 0.f, 1.f, 1.f); // normalized render rectangle
    if (subRect.Contains(renderRect)) {
        // The render rectangle is fully subtracted, so the draw call can be skipped.
        return true;
    }
    if (!renderRect.Contains(subRect)) {
        // No optimazation for the subRect which reaches the edge of the render rectangle in any direction
        return false;
    }
    float maxWidth = std::max({ width * subRect.GetLeft(), width * (1.f - subRect.GetRight()),
        height * subRect.GetTop(), height * (1.f - subRect.GetBottom())});
    float penWidth = maxWidth * 2.f; // needs to be doubled since the border meets the middle of pen
    Drawing::Pen pen;
    pen.SetWidth(penWidth);
    pen.SetShaderEffect(drShader_);
    canvas.AttachPen(pen);
    canvas.DrawRect(rect);
    canvas.DetachPen();
    return true;
}

void GEShader::MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress)
{
    MakeDrawingShader(rect, progress);
}

void GEShader::OnDrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Preprocess(canvas, rect); // to calculate your cache data
    MakeDrawingShader(canvas, rect, -1.f); // not use progress
    auto shader = GetDrawingShader();
    if (!shader) {
        GE_LOGE("GEShader::OnDrawShader: no shader generated, draw nothing");
        return;
    }
    if (TryDrawShaderWithPen(canvas, rect)) {
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}

void GEShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    GE_TRACE_NAME_FMT("GEShader::DrawShader, Type: %s, Rect Width: %f, Height: %f",
        TypeName().data(), rect.GetWidth(), rect.GetHeight());
    OnDrawShader(canvas, rect);
}

std::shared_ptr<Drawing::ShaderEffect> GEShader::MakeSDFShaderWithCache(
    std::shared_ptr<Drawing::GESDFShaderShape> sdfShape, Drawing::Canvas& canvas, const Drawing::Rect& rect,
    bool hasNormal) const
{
    if (!sdfShape) {
        cacheAnyPtr_ = nullptr;
        return nullptr;
    }
    const SDFImageCache emptyCache{};
    auto canvasSurface = canvas.GetSurface();
    std::shared_ptr<Drawing::ColorSpace> colorSpace = canvasSurface ?
        canvasSurface->GetImageInfo().GetColorSpace() : nullptr;
    auto inputHash = CalHash(Drawing::Size(rect.GetWidth(), rect.GetHeight()), hasNormal, sdfShape->GetHash(),
        colorSpace);
    if (inputHash != UnpackSDFCacheAnyOrDefaultFunc(cacheAnyPtr_, emptyCache).hash) {
        // gernerate cache
        auto sdfImg = sdfShape->MakeSDFImage(canvas, rect.GetWidth(), rect.GetHeight(), hasNormal);
        if (sdfImg == nullptr) {
            GE_LOGE("GEShader::MakeSDFShaderWithCache make SDFImage is null");
            cacheAnyPtr_ = nullptr;
            return nullptr;
        }
        GE_LOGD("GEShader::MakeSDFShaderWithCache %{public}s update sdf cache with %{public}u %{public}u",
            TypeName().data(), inputHash, UnpackSDFCacheAnyOrDefaultFunc(cacheAnyPtr_, emptyCache).hash);
        SDFImageCache cacheData{inputHash, sdfImg};
        cacheAnyPtr_ = PackSDFCacheAnyFunc(std::move(cacheData));
    }
    auto cacheSDFImg = UnpackSDFCacheAnyOrDefaultFunc(cacheAnyPtr_, emptyCache).sdfImage;
    if (cacheSDFImg == nullptr) {
        GE_LOGE("GEShader::MakeSDFShaderWithCache get SDFImage is null");
        cacheAnyPtr_ = nullptr;
        return nullptr;
    }
    // use cache
    GE_TRACE_NAME_FMT("GEShader::MakeSDFShaderWithCache, Type: %s, use cache %u %u Width: %f, Height: %f  %u",
        TypeName().data(), inputHash, sdfShape->GetHash(), rect.GetWidth(), rect.GetHeight(), CalHash(colorSpace));
    GE_LOGD("GEShader::MakeSDFShaderWithCache %{public}s use cache with hash %{public}u %{public}u "
        "%{public}f %{public}f %{public}u", TypeName().data(), inputHash, sdfShape->GetHash(),
        rect.GetWidth(), rect.GetHeight(), CalHash(colorSpace));
    Drawing::Matrix sampleMatrix;
    return Drawing::ShaderEffect::CreateImageShader(*cacheSDFImg, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), sampleMatrix);
}
} // namespace Rosen
} // namespace OHOS