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
#include "ge_sdf_rrect_shader_mask.h"
#include "ge_sdf_union_op_shader_mask.h"

namespace OHOS::Rosen::Drawing {
struct ComparatorForEffects final {
    bool operator()(const std::unique_ptr<GESDFEffect>& effect1,
                    const std::unique_ptr<GESDFEffect>& effect2) const;
};

class GESDFTreeProcessor final {
    void Process(const std::shared_ptr<GESDFShaderMask> sdfMask);
    void Process(const GESDFRRectShaderMask& sdfMask);
    void Process(const GESDFUnionOpShaderMask& sdfMask);

    // Generate shader code
    void GenerateHeader(const GESDFRRectShaderMask& sdfMask);
    void GenerateHeader(const GESDFUnionOpShaderMask& sdfMask);

    void GenerateBody(const GESDFRRectShaderMask& sdfMask);
    void GenerateBody(const GESDFUnionOpShaderMask& sdfMask);

    // Update uniform values
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const std::shared_ptr<GESDFShaderMask> sdfMask);
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const GESDFRRectShaderMask& sdfMask);
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder, const GESDFUnionOpShaderMask& sdfMask);

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
    std::shared_ptr<GESDFShaderMask> sdfMask_;
    
    std::string shaderCode_;
    std::string headers_;
    std::string body_;
    std::string effects_;
    std::string effectsFunctions_;

    std::set<std::unique_ptr<GESDFEffect>, ComparatorForEffects> effectsContainer_;
};
} // namespace OHOS::Rosen::Drawing

#endif // GRAPHICS_EFFECT_GE_SDF_TREE_PROCESSOR_H
