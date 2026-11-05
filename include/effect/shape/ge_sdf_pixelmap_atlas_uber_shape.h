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

#ifndef GRAPHICS_EFFECT_GE_SDF_PIXELMAP_ATLAS_UBER_SHAPE_H
#define GRAPHICS_EFFECT_GE_SDF_PIXELMAP_ATLAS_UBER_SHAPE_H

#include "ge_sdf_shader_shape.h"
#include "ge_sdf_uber_shader_shape.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct GESDFPixelmapUberLeafInfo : public GESDFUberLeafInfo {
    std::shared_ptr<Drawing::Image> image;
    float width = 0.f;
    float height = 0.f;
};

// Pixelmap-atlas uber shape: all leaf images are shelf-packed into one atlas
// sampled by a single child shader.
class GE_EXPORT GESdfPixelmapAtlasUberShape : public GESdfUberShaderShape {
public:
    explicit GESdfPixelmapAtlasUberShape(const GESdfUberShaderShapeParams& params)
        : GESdfUberShaderShape(params) {}
    ~GESdfPixelmapAtlasUberShape() override = default;

protected:
    std::shared_ptr<ShaderEffect> GenerateUberShader(float width, float height, bool hasNormal) const override;

private:
    bool CollectLeaves(const std::shared_ptr<GESDFShaderShape>& root,
        std::vector<GESDFPixelmapUberLeafInfo>& leaves, GESDFUberTreeDesc& desc) const;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetBuilder(bool hasNormal, bool isSmoothUnion);
    void PackUniforms(const std::shared_ptr<Drawing::RuntimeShaderBuilder>& builder,
        const std::vector<GESDFPixelmapUberLeafInfo>& leaves, const Drawing::Image& atlasImage,
        const std::vector<float>& offsets, const GESDFUberTreeDesc& desc, float width, float height) const;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_PIXELMAP_ATLAS_UBER_SHAPE_H
