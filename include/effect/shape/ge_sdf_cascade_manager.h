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

#ifndef GRAPHICS_EFFECT_GE_SDF_CASCADE_MANAGER_H
#define GRAPHICS_EFFECT_GE_SDF_CASCADE_MANAGER_H

#include <set>
#include <sstream>
#include <variant>

#include "common/rs_vector2.h"
#include "ge_sdf_shader_shape.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
using uniformValue = std::variant<float, Vector2f, Drawing::Matrix, std::shared_ptr<ShaderEffect>>;

enum class SDFUniformType : uint8_t {
    FLOAT = 0,
    VECTOR2F,
    MATRIX,
    SHADER,
    MAX,
};

const std::array<const std::string, static_cast<size_t>(SDFUniformType::MAX)> SDFUniformTypeString = {
    "float", "vec2", "float3x3", "shader"};

struct UniformData {
    SDFUniformType type;
    std::string name;
    uniformValue value;
};

class GE_EXPORT GESDFCascadeManager final {
public:
    GESDFCascadeManager();
    GESDFCascadeManager(const GESDFCascadeManager&) = delete;
    virtual ~GESDFCascadeManager() = default;

    bool AddSDFType(GESDFShapeType type);
    void AddUniformData(const uint16_t& uniformIndex, const std::vector<UniformData>& data);
    void PrependShaderFunction(const std::string& func);
    void AppendMainCall(const std::string& call);
    void AppendSDFCall(const uint16_t& index, const std::string& call);
    void AppendReturnCall(const uint16_t& index);

    uint16_t GenerateUniformIndex();
    std::shared_ptr<ShaderEffect> GenerateShaderEffectOnePass();

private:
    std::set<GESDFShapeType> typeSet_;
    uint16_t uniformIndex_;
    std::string prog_;
    std::map<uint16_t, std::vector<UniformData>> uniform_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SDF_CASCADE_MANAGER_H
