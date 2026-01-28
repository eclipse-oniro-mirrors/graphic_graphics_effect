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

#include "sdf/ge_sdf_cascade_manager.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
GESDFCascadeManager::GESDFCascadeManager()
{
    prog_ = R"(
        half4 main(vec2 fragCoord)
        {
    )";
    uniformIndex_ = 1;
}

bool GESDFCascadeManager::AddSDFType(GESDFShapeType type)
{
    if (typeSet_.find(type) != typeSet_.end()) {
        return true;
    }
    typeSet_.insert(type);
    return false;
}

void GESDFCascadeManager::AddUniformData(const uint16_t& uniformIndex, const std::vector<UniformData>& data)
{
    uniform_.emplace(uniformIndex, data);
}

void GESDFCascadeManager::PrependShaderFunction(const std::string& func)
{
    prog_ = func + prog_;
}

void GESDFCascadeManager::AppendMainCall(const std::string& call)
{
    prog_ += call;
}

void GESDFCascadeManager::AppendSDFCall(const uint16_t& index, const std::string& call)
{
    std::ostringstream fullSdfCallOss;
    fullSdfCallOss << "vec4 sdf" << index << "=" << call << ";";
    prog_ += fullSdfCallOss.str();
}

void GESDFCascadeManager::AppendReturnCall(const uint16_t& index)
{
    std::ostringstream returnCallOss;
    returnCallOss << "return sdf" << index << ";}";
    prog_ += returnCallOss.str();
}

uint16_t GESDFCascadeManager::GenerateUniformIndex()
{
    return uniformIndex_++;
}

std::shared_ptr<ShaderEffect> GESDFCascadeManager::GenerateShaderEffectOnePass()
{
    // Uniform Statement
    for (const auto& uniformPair : uniform_) {
        for (const auto& uniformData : uniformPair.second) {
            std::ostringstream uniformStringOss;
            uniformStringOss << "uniform " << SDFUniformTypeString[static_cast<size_t>(uniformData.type)] << " "
                             << uniformData.name << uniformPair.first << ";";
            prog_ = uniformStringOss.str() + prog_;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> sdfShaderShapeBuilder = nullptr;
    auto sdfShaderShapeBuilderEffect = Drawing::RuntimeEffect::CreateForShader(prog_);
    if (!sdfShaderShapeBuilderEffect) {
        GE_LOGE("Creating SDF runtimeEffect failed");
        return nullptr;
    }
    sdfShaderShapeBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(sdfShaderShapeBuilderEffect);
    if (!sdfShaderShapeBuilder) {
        GE_LOGE("Creating SDF runtimeShaderBuilder failed");
        return nullptr;
    }
    // Setting uniform
    for (const auto& [uniformIndex, uniformDataList] : uniform_) {
        for (const auto& uniformData : uniformDataList) {
            std::string fullUniformName = uniformData.name + std::to_string(uniformIndex);
            switch (uniformData.value.index()) {
                case static_cast<size_t>(SDFUniformType::FLOAT): {
                    sdfShaderShapeBuilder->SetUniform(fullUniformName, std::get<float>(uniformData.value));
                    break;
                }
                case static_cast<size_t>(SDFUniformType::VECTOR2F): {
                    Vector2f data = std::get<Vector2f>(uniformData.value);
                    sdfShaderShapeBuilder->SetUniform(fullUniformName, data.x_, data.y_);
                    break;
                }
                case static_cast<size_t>(SDFUniformType::MATRIX): {
                    sdfShaderShapeBuilder->SetUniform(fullUniformName, std::get<Drawing::Matrix>(uniformData.value));
                    break;
                }
                case static_cast<size_t>(SDFUniformType::SHADER): {
                    sdfShaderShapeBuilder->SetChild(
                        fullUniformName, std::get<std::shared_ptr<ShaderEffect>>(uniformData.value));
                    break;
                }
                default:
                    GE_LOGE("Invalid uniform type");
                    break;
            }
        }
    }
    return sdfShaderShapeBuilder->MakeShader(nullptr, false);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS