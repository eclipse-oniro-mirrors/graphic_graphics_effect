/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_SDF_TREE_PROCESSOR_H
#define GRAPHICS_EFFECT_GE_SDF_TREE_PROCESSOR_H


#include <string>
#include <set>
#include "ge_sdf_effects.h"
#include "ge_sdf_rrect_shader_shape.h"
#include "ge_sdf_union_op_shader_shape.h"

namespace OHOS::Rosen::Drawing {
struct ComparatorForEffects final {
    bool operator()(const std::unique_ptr<GESDFEffect>& effect1,
                    const std::unique_ptr<GESDFEffect>& effect2) const;
};

class GESDFTreeProcessor final {
    void Process(const std::shared_ptr<GESDFShaderShape> sdfShape);
    void Process(const GESDFRRectShaderShape& sdfShape);
    void Process(const GESDFUnionOpShaderShape& sdfShape);

    // Generate shader code
    void GenerateHeader(const GESDFRRectShaderShape& sdfShape);
    void GenerateHeader(const GESDFUnionOpShaderShape& sdfShape);

    void GenerateBody(const GESDFRRectShaderShape& sdfShape);
    void GenerateBody(const GESDFUnionOpShaderShape& sdfShape);

    // Update uniform values
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const std::shared_ptr<GESDFShaderShape> sdfShape);
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const GESDFRRectShaderShape& sdfShape);
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const GESDFUnionOpShaderShape& sdfShape);

public:

    explicit GESDFTreeProcessor(const GESDFFilterParams& params);

    GESDFTreeProcessor(GESDFTreeProcessor&& processor) = default;
    GESDFTreeProcessor(const GESDFTreeProcessor& processor) = delete;

    GESDFTreeProcessor& operator = (GESDFTreeProcessor&& processor) = default;
    GESDFTreeProcessor& operator = (const GESDFTreeProcessor& processor) = delete;

    [[nodiscard]] std::string Process();
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder);

    void UpdateParams(const GESDFFilterParams& params);
private:
    std::shared_ptr<GESDFShaderShape> sdfShape_;
    
    std::string shaderCode_;
    std::string headers_;
    std::string body_;
    std::string effects_;
    std::string effectsFunctions_;

    std::set<std::unique_ptr<GESDFEffect>, ComparatorForEffects> effectsContainer_;
};
} // namespace OHOS::Rosen::Drawing

#endif // GRAPHICS_EFFECT_GE_SDF_TREE_PROCESSOR_H
