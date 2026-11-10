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

#include "ge_sdf_uber_pixelmap_atlas_shape.h"

#include <algorithm>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include "draw/brush.h"
#include "draw/surface.h"
#include "ge_log.h"
#include "ge_sdf_pixelmap_shader_shape.h"
#include "ge_shader_diagnostics.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr float SPACING_MIN = 0.0001f;

constexpr int VEC4_FLOATS = 4;
constexpr int ATLAS_PADDING = 1;
constexpr int ATLAS_PADDING_BOTH_SIDES = ATLAS_PADDING * 2;
constexpr int ATLAS_OFFSET_FLOATS = 2;
constexpr int ATLAS_MAX_SIZE = 2048;

constexpr char UBER_PIXELMAP_ATLAS_COMMON_PROG[] = R"(
    uniform float u_spacing;
    uniform vec2 u_centerPos;
    uniform vec4 u_matA[6];
    uniform vec4 u_matB[6];
    uniform vec4 u_matC[6];
    uniform vec4 u_geo[6];
    uniform shader u_atlas;

    const float N_SCALE = 2048.0;

    float EncodeDir(vec2 dir)
    {
        float xPos = floor(dir.x + N_SCALE);
        float yPos = floor(dir.y + N_SCALE);
        return xPos + (yPos / N_SCALE) / 2.0;
    }

    vec2 DecodeDir(float z)
    {
        float xPos = floor(z);
        float yPos = (z - xPos) * N_SCALE * 2.0 - N_SCALE;
        xPos -= N_SCALE;
        return vec2(xPos, yPos);
    }

    vec4 sdgSmoothUnion(vec4 d1, vec4 d2, float k)
    {
        k *= 4.0;
        float h = max(k - abs(d1.a - d2.a), 0.0) / (2.0 * k);
        vec2 centerDir1 = DecodeDir(d1.z);
        vec2 centerDir2 = DecodeDir(d2.z);
        vec2 centerDirUnion = mix(centerDir1, centerDir2, (d1.a < d2.a) ? h : 1.0 - h);
        float encodingZ = EncodeDir(centerDirUnion);
        return vec4(mix(d1.xy, d2.xy, (d1.a < d2.a) ? h : 1.0 - h), encodingZ, min(d1.a, d2.a) - h * h * k);
    }

    float sdfSmoothUnion(float d1, float d2, float k)
    {
        k *= 4.0;
        float h = max(k - abs(d1 - d2), 0.0) / (2.0 * k);
        return min(d1, d2) - h * h * k;
    }

    // Pure leaf evaluation: inlined SafeFetch bounds + channel remap, verbatim
    // from GetSDFPixelmapNormalShaderShapeBuilder (gradient rg: [0,1]->[-1,1],
    // SDF a: [0,1]->[-63.5, 64]; out-of-bounds = far outside +64).
    vec4 EvalAtlasLeafCore(vec2 local, vec4 c, vec4 g)
    {
        vec2 size = c.yz;
        if (local.x < 0.0 || local.x >= size.x || local.y < 0.0 || local.y >= size.y) {
            return vec4(0.0, 0.0, EncodeDir(local - u_centerPos), 127.5 - 63.5);
        }
        vec4 px = u_atlas.eval(local + g.xy);
        return vec4(px.xy * 2.0 - 1.0, EncodeDir(local - u_centerPos), px.a * 127.5 - 63.5);
    }

    // SDF-only fetch for the no-normal mains: same bounds + a-channel remap.
    float EvalAtlasLeafSdf(vec2 local, vec4 c, vec4 g)
    {
        vec2 size = c.yz;
        if (local.x < 0.0 || local.x >= size.x || local.y < 0.0 || local.y >= size.y) {
            return 127.5 - 63.5;
        }
        vec4 px = u_atlas.eval(local + g.xy);
        return px.a * 127.5 - 63.5;
    }

)";

constexpr char UBER_PIXELMAP_ATLAS_SMOOTH_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        // Leaf 0 is evaluated before the loop so the steady-state loop body
        // carries no i == 0 branch and no early-exit branch.
        vec4 result;
        {
            vec4 a = u_matA[0];
            vec4 b = u_matB[0];
            vec4 c = u_matC[0];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            result = EvalAtlasLeafCore(local, c, u_geo[0]);
        }
        for (int i = 1; i < 6; ++i) {
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec4 c = u_matC[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            vec4 d = EvalAtlasLeafCore(local, c, u_geo[i]);
            result = sdgSmoothUnion(result, d, u_spacing);
        }
        return half4(result);
    }
)";

constexpr char UBER_PIXELMAP_ATLAS_UNION_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        vec4 result;
        {
            vec4 a = u_matA[0];
            vec4 b = u_matB[0];
            vec4 c = u_matC[0];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            result = EvalAtlasLeafCore(local, c, u_geo[0]);
        }
        for (int i = 1; i < 6; ++i) {
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec4 c = u_matC[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            vec4 d = EvalAtlasLeafCore(local, c, u_geo[i]);
            result = (result.a >= d.a) ? d : result;
        }
        return half4(result);
    }
)";

constexpr char UBER_PIXELMAP_ATLAS_SMOOTH_NO_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        float sdf;
        {
            vec4 a = u_matA[0];
            vec4 b = u_matB[0];
            vec4 c = u_matC[0];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            sdf = EvalAtlasLeafSdf(local, c, u_geo[0]);
        }
        for (int i = 1; i < 6; ++i) {
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec4 c = u_matC[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            float d = EvalAtlasLeafSdf(local, c, u_geo[i]);
            sdf = sdfSmoothUnion(sdf, d, u_spacing);
        }
        return half4(0.0, 0.0, 0.0, sdf);
    }
)";

constexpr char UBER_PIXELMAP_ATLAS_UNION_NO_NORMAL_PROG[] = R"(
    half4 main(vec2 fragCoord)
    {
        float sdf;
        {
            vec4 a = u_matA[0];
            vec4 b = u_matB[0];
            vec4 c = u_matC[0];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            sdf = EvalAtlasLeafSdf(local, c, u_geo[0]);
        }
        for (int i = 1; i < 6; ++i) {
            vec4 a = u_matA[i];
            vec4 b = u_matB[i];
            vec4 c = u_matC[i];
            vec2 local = vec2(a.x * fragCoord.x + a.y * fragCoord.y + a.z,
                a.w * fragCoord.x + b.x * fragCoord.y + b.y);
            float d = EvalAtlasLeafSdf(local, c, u_geo[i]);
            sdf = min(sdf, d);
        }
        return half4(0.0, 0.0, 0.0, sdf);
    }
)";

struct PixelmapAtlasEntry {
    std::shared_ptr<Drawing::Image> atlasImage;
    std::vector<float> offsets; 
};

bool PackAtlasShelf(const std::vector<GESDFUberPixelmapLeafInfo>& leaves, int& atlasWidth, int& atlasHeight,
    std::vector<std::pair<int, int>>& origins)
{
    int curX = 0;
    int curY = 0;
    int rowHeight = 0;
    atlasWidth = 0;
    atlasHeight = 0;
    origins.clear();
    origins.reserve(leaves.size());
    for (const auto& leaf : leaves) {
        int w = static_cast<int>(leaf.width);
        int h = static_cast<int>(leaf.height);
        if (curX > 0 && curX + w + ATLAS_PADDING_BOTH_SIDES > ATLAS_MAX_SIZE) {
            // Start a new row.
            curY += rowHeight + ATLAS_PADDING_BOTH_SIDES;
            curX = 0;
            rowHeight = 0;
        }
        origins.push_back({ curX + ATLAS_PADDING, curY + ATLAS_PADDING });
        curX += w + ATLAS_PADDING_BOTH_SIDES;
        rowHeight = std::max(rowHeight, h);
        atlasWidth = std::max(atlasWidth, curX);
        atlasHeight = std::max(atlasHeight, curY + rowHeight + ATLAS_PADDING_BOTH_SIDES);
    }
    return atlasWidth <= ATLAS_MAX_SIZE && atlasHeight <= ATLAS_MAX_SIZE;
}

std::shared_ptr<PixelmapAtlasEntry> BuildPixelmapAtlas(const std::vector<GESDFUberPixelmapLeafInfo>& leaves,
    const std::vector<std::pair<int, int>>& origins, int atlasWidth, int atlasHeight)
{
    Drawing::ImageInfo atlasInfo(atlasWidth, atlasHeight, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    auto surface = Drawing::Surface::MakeRaster(atlasInfo);
    if (!surface) {
        GE_LOGE("BuildPixelmapAtlas: make raster surface fail");
        return nullptr;
    }
    auto canvas = surface->GetCanvas();
    if (!canvas) {
        GE_LOGE("BuildPixelmapAtlas: get canvas fail");
        return nullptr;
    }
    Drawing::Brush background;
    background.SetColor(Drawing::Color::COLOR_WHITE);
    canvas->DrawBackground(background);
    Drawing::Brush imageBlit;
    imageBlit.SetBlendMode(Drawing::BlendMode::SRC);
    canvas->AttachBrush(imageBlit);
    for (size_t i = 0; i < leaves.size(); ++i) {
        canvas->DrawImage(*leaves[i].image, static_cast<Drawing::scalar>(origins[i].first),
            static_cast<Drawing::scalar>(origins[i].second), Drawing::SamplingOptions(Drawing::FilterMode::NEAREST));
    }
    canvas->DetachBrush();
    auto entry = std::make_shared<PixelmapAtlasEntry>();
    entry->atlasImage = surface->GetImageSnapshot();
    if (!entry->atlasImage) {
        GE_LOGE("BuildPixelmapAtlas: image snapshot fail");
        return nullptr;
    }
    entry->offsets.reserve(leaves.size() * ATLAS_OFFSET_FLOATS);
    for (const auto& origin : origins) {
        entry->offsets.push_back(static_cast<float>(origin.first));
        entry->offsets.push_back(static_cast<float>(origin.second));
    }
    return entry;
}
} // namespace

std::shared_ptr<Drawing::RuntimeShaderBuilder> GESdfUberPixelmapAtlasShape::GetBuilder(
    bool hasNormal, bool isSmoothUnion)
{
    thread_local std::array<std::shared_ptr<Drawing::RuntimeShaderBuilder>, UBER_SHADER_VARIANT_COUNT> builders {};
    int index = (hasNormal ? UBER_VARIANT_NORMAL_OFFSET : 0) + (isSmoothUnion ? UBER_VARIANT_SMOOTH_OFFSET : 0);
    if (builders[index]) {
        return builders[index];
    }
    static constexpr const char* VARIANTS[UBER_SHADER_VARIANT_COUNT] = {
        UBER_PIXELMAP_ATLAS_UNION_NO_NORMAL_PROG,
        UBER_PIXELMAP_ATLAS_SMOOTH_NO_NORMAL_PROG,
        UBER_PIXELMAP_ATLAS_UNION_NORMAL_PROG,
        UBER_PIXELMAP_ATLAS_SMOOTH_NORMAL_PROG,
    };
    std::string prog = std::string(UBER_PIXELMAP_ATLAS_COMMON_PROG) + VARIANTS[index];
    auto effect = GECreateRuntimeEffectForShader(prog);
    if (!effect) {
        GE_LOGE("GetBuilder: create runtime effect fail, hasNormal=%{public}d, smooth=%{public}d",
            static_cast<int>(hasNormal), static_cast<int>(isSmoothUnion));
        return nullptr;
    }
    builders[index] = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    return builders[index];
}

bool GESdfUberPixelmapAtlasShape::CollectLeaves(const std::shared_ptr<GESDFShaderShape>& root,
    std::vector<GESDFUberPixelmapLeafInfo>& leaves, GESDFUberTreeDesc& desc) const
{
    std::vector<GESDFUberLeafPos> positions;
    if (!WalkUberLeaves(root, Drawing::Matrix(), desc, positions)) {
        return false;
    }
    for (const auto& pos : positions) {
        const Drawing::Matrix& m = pos.parentToLocal;
        if (!IsAffineTransform(m)) {
            return false;
        }
        if (pos.leaf->GetSDFShapeType() != GESDFShapeType::PIXELMAP) {
            GE_LOGD("CollectLeaves: non-PixelMap leaf, fall back");
            return false;
        }
        const auto* pixelmapShape = static_cast<const GESDFPixelmapShaderShape*>(pos.leaf.get());
        std::shared_ptr<Drawing::Image> image = pixelmapShape->GetImage();
        if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
            GE_LOGD("CollectLeaves: empty image, fall back");
            return false;
        }
        GESDFUberPixelmapLeafInfo info;
        info.parentToLocal = pos.parentToLocal;
        info.image = std::move(image);
        info.width = static_cast<float>(info.image->GetWidth());
        info.height = static_cast<float>(info.image->GetHeight());
        leaves.push_back(std::move(info));
    }
    return true;
}

void GESdfUberPixelmapAtlasShape::PackUniforms(const std::shared_ptr<Drawing::RuntimeShaderBuilder>& builder,
    const std::vector<GESDFUberPixelmapLeafInfo>& leaves, const Drawing::Image& atlasImage,
    const std::vector<float>& offsets, const GESDFUberTreeDesc& desc, float width, float height) const
{
    constexpr float HALF = 0.5f;
    int count = static_cast<int>(leaves.size());
    std::array<float, VEC4_FLOATS * PIXELMAP_UBER_LEAF_MAX> matA {};
    std::array<float, VEC4_FLOATS * PIXELMAP_UBER_LEAF_MAX> matB {};
    std::array<float, VEC4_FLOATS * PIXELMAP_UBER_LEAF_MAX> matC {};
    std::array<float, VEC4_FLOATS * PIXELMAP_UBER_LEAF_MAX> geo {};
    for (int i = 0; i < count; ++i) {
        const Drawing::Matrix& m = leaves[i].parentToLocal;
        int base = i * VEC4_FLOATS;
        matA[base] = m.Get(Drawing::Matrix::Index::SCALE_X);
        matA[base + UBER_OFFSET_1] = m.Get(Drawing::Matrix::Index::SKEW_X);
        matA[base + UBER_OFFSET_2] = m.Get(Drawing::Matrix::Index::TRANS_X);
        matA[base + UBER_OFFSET_3] = m.Get(Drawing::Matrix::Index::SKEW_Y);
        matB[base] = m.Get(Drawing::Matrix::Index::SCALE_Y);
        matB[base + UBER_OFFSET_1] = m.Get(Drawing::Matrix::Index::TRANS_Y);
        matC[base + UBER_OFFSET_1] = leaves[i].width;
        matC[base + UBER_OFFSET_2] = leaves[i].height;
        geo[base] = offsets[i * ATLAS_OFFSET_FLOATS];
        geo[base + UBER_OFFSET_1] = offsets[i * ATLAS_OFFSET_FLOATS + UBER_OFFSET_1];
    }
    builder->SetUniform("u_spacing", desc.isSmoothUnion ? std::max(desc.spacing, SPACING_MIN) : 0.f);
    builder->SetUniform("u_centerPos", width * HALF, height * HALF);
    builder->SetUniform("u_matA", matA.data(), matA.size());
    builder->SetUniform("u_matB", matB.data(), matB.size());
    builder->SetUniform("u_matC", matC.data(), matC.size());
    builder->SetUniform("u_geo", geo.data(), geo.size());
    builder->SetChild("u_atlas", Drawing::ShaderEffect::CreateImageShader(atlasImage,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), Drawing::Matrix()));
}

std::shared_ptr<ShaderEffect> GESdfUberPixelmapAtlasShape::GenerateUberShader(
    float width, float height, bool hasNormal) const
{
    std::vector<GESDFUberPixelmapLeafInfo> leaves;
    leaves.reserve(PIXELMAP_UBER_LEAF_MAX);
    GESDFUberTreeDesc desc;
    if (!CollectLeaves(GetRootShape(), leaves, desc)) {
        return nullptr;
    }
    int count = static_cast<int>(leaves.size());
    if (!IsLeafCountInRange(count, PIXELMAP_UBER_LEAF_MAX, PIXELMAP_UBER_LEAF_MIN)) {
        return nullptr;
    }
    int atlasWidth = 0;
    int atlasHeight = 0;
    std::vector<std::pair<int, int>> origins;
    if (!PackAtlasShelf(leaves, atlasWidth, atlasHeight, origins)) {
        GE_LOGD("GenerateUberShader: atlas budget exceeded, leaves=%{public}d, "
            "size=%{public}dx%{public}d", count, atlasWidth, atlasHeight);
        return nullptr;
    }
    std::shared_ptr<PixelmapAtlasEntry> entry = BuildPixelmapAtlas(leaves, origins, atlasWidth, atlasHeight);
    if (!entry) {
        return nullptr;
    }
    auto builder = GetBuilder(hasNormal, desc.isSmoothUnion);
    if (!builder) {
        return nullptr;
    }
    GE_TRACE_NAME_FMT("GESdfUberPixelmapAtlasShape pack uniforms, count: %d, smoothUnion: %d, hasNormal: %d",
        count, static_cast<int>(desc.isSmoothUnion), static_cast<int>(hasNormal));
    PackUniforms(builder, leaves, *entry->atlasImage, entry->offsets, desc, width, height);
    auto uberShader = builder->MakeShader(nullptr, false);
    if (!uberShader) {
        GE_LOGE("GenerateUberShader: make shader fail, count=%{public}d", count);
    }
    return uberShader;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
