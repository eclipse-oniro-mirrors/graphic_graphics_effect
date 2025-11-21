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

#include "sdf/ge_sdf_tree_processor.h"

#include "ge_log.h"

namespace OHOS::Rosen::Drawing {
    GESDFTreeProcessor::GESDFTreeProcessor(const GESDFFilterParams& params) : sdfShape_(params.shape)
    {
        if (params.shadow) {
            const auto& shadow = params.shadow.value();
            effectsContainer_.insert(std::make_unique<Drawing::GESDFShadow>(shadow.color, shadow.offsetX,
                                                                            shadow.offsetY, shadow.radius,
                                                                            shadow.path, shadow.isFilled));
        }
    }

    void GESDFTreeProcessor::ProcessSDFShape(std::string& headers, std::string& functions)
    {
        if (!sdfShape_) {
            GE_LOGE("GESDFTreeProcessor::Process shape is null.");
            return;
        }

        Process(sdfShape_);

        const auto returnNodeId = reinterpret_cast<size_t>(sdfShape_.get());
        const std::string returnCode = "return var_" + std::to_string(returnNodeId) + ";";
    
        headers += headers_;

        functions += R"(

                float SDFSmoothUnion(float d1, float d2, float k)
                {
                    k*= 4.0;
                    float h = max(k - abs(d1 - d2), 0.0);
                    return min(d1, d2) - h*h*0.25 / k;
                }

                float SDFRRect(in vec2 coord, in vec2 p, in vec2 b, in float r)
                {
                    vec2 d = abs(coord - p) - (b - r);
                    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
                }

                float SDFMap(in vec2 uv)
                {
            )" + body_ + returnCode + "\n}\n";

        headers_.clear();
        body_.clear();
    }

    std::string GESDFTreeProcessor::Process()
    {
        if (!sdfShape_) {
            GE_LOGE("GESDFTreeProcessor::Process shape is null.");
            return "";
        }
        
        if (shaderCode_.empty()) {
            std::string headers = "uniform shader image;\n";
            std::string sdfShapeFunctions;
            ProcessSDFShape(headers, sdfShapeFunctions);

            constexpr std::string_view mainFunctionCodeStart = R"(

                vec4 main(vec2 fragCoord)
                {
                    vec4 color = image.eval(fragCoord).rgba;
                    vec2 coord = fragCoord;
                    float d = SDFMap(coord);
                )";

            for (const auto& effect: effectsContainer_) {
                effect->Process(headers, effects_, effectsFunctions_);
            }

            constexpr std::string_view mainFunctionCodeEnd = R"(

                    return color;
                }
            )";

            shaderCode_ += headers;
            shaderCode_ += sdfShapeFunctions;
            shaderCode_ += effectsFunctions_;
            shaderCode_ += mainFunctionCodeStart;
            shaderCode_ += effects_;
            shaderCode_ += mainFunctionCodeEnd;

            effects_.clear();
            effectsFunctions_.clear();
        }

        return shaderCode_;
    }

    void GESDFTreeProcessor::Process(const std::shared_ptr<GESDFShaderShape> sdfShape)
    {
        if (!sdfShape) {
            return;
        }

        const auto type = sdfShape->GetSDFShapeType();
        if (type == GESDFShapeType::RRECT) {
            auto rrectShape = std::static_pointer_cast<GESDFRRectShaderShape>(sdfShape);
            Process(*rrectShape);
        } else if (type == GESDFShapeType::UNION_OP) {
            auto unionShape = std::static_pointer_cast<GESDFUnionOpShaderShape>(sdfShape);
            Process(*unionShape);
        } else {
            GE_LOGE("GESDFTreeProcessor::Process undefined sdf shape type");
        }
    }

    void GESDFTreeProcessor::UpdateUniformDatas(RuntimeShaderBuilder& builder,
        const std::shared_ptr<GESDFShaderShape> sdfShape)
    {
        if (!sdfShape) {
            return;
        }

        const auto type = sdfShape->GetSDFShapeType();
        if (type == GESDFShapeType::RRECT) {
            auto rrectShape = std::static_pointer_cast<GESDFRRectShaderShape>(sdfShape);
            UpdateUniformDatas(builder, *rrectShape);
        } else if (type == GESDFShapeType::UNION_OP) {
            auto unionShape = std::static_pointer_cast<GESDFUnionOpShaderShape>(sdfShape);
            UpdateUniformDatas(builder, *unionShape);
        } else {
            GE_LOGE("GESDFTreeProcessor::UpdateUniformDatas undefined sdf shape type");
        }
    }

    void GESDFTreeProcessor::Process(const GESDFRRectShaderShape& sdfShape)
    {
        GenerateHeader(sdfShape);
        GenerateBody(sdfShape);
    }

    void GESDFTreeProcessor::GenerateBody(const GESDFRRectShaderShape& sdfShape)
    {
        auto nodeId = reinterpret_cast<size_t>(&sdfShape);
        body_ += "float var_" + std::to_string(nodeId) +
                 " = SDFRRect(uv, u_pos_" + std::to_string(nodeId) +
                 ", u_size_" + std::to_string(nodeId) +
                 ", u_radius_" + std::to_string(nodeId) +
                 ");\n";
    }

    void GESDFTreeProcessor::GenerateHeader(const GESDFRRectShaderShape& sdfShape)
    {
        auto nodeId = reinterpret_cast<size_t>(&sdfShape);
        headers_ += "uniform vec2 u_pos_" + std::to_string(nodeId) + ";\n";
        headers_ += "uniform vec2 u_size_" + std::to_string(nodeId) + ";\n";
        headers_ += "uniform float u_radius_" + std::to_string(nodeId) + ";\n";
    }

    void GESDFTreeProcessor::UpdateUniformDatas(RuntimeShaderBuilder& builder,
        const GESDFRRectShaderShape& sdfShape)
    {
        auto rect = sdfShape.GetRRect();

        auto left = rect.left_;
        auto top = rect.top_;
        auto width = rect.width_;
        auto height = rect.height_;
        GE_LOGE("GESDFTreeProcessor::UpdateUniformDatas: l, t, w, h"
                "%{public}f, %{public}f, %{public}f, %{public}f",
                left, top, width, height);
        auto radius = std::max(rect.radiusX_, rect.radiusY_);
        auto right = left + width;
        auto bottom = top - height;

        float posX = (right + left) / 2.0;
        float posY = (top + bottom) / 2.0;

        float sizeX = (std::max(right, left) - std::min(right, left)) / 2.0;
        float sizeY = (std::max(top, bottom) - std::min(top, bottom)) / 2.0;

        auto nodeId = reinterpret_cast<size_t>(&sdfShape);

        builder.SetUniform("u_pos_" + std::to_string(nodeId), posX, posY);
        builder.SetUniform("u_size_" + std::to_string(nodeId), sizeX, sizeY);
        builder.SetUniform("u_radius_" + std::to_string(nodeId), radius);
    }

    void GESDFTreeProcessor::Process(const GESDFUnionOpShaderShape& sdfShape)
    {
        const auto leftNode = sdfShape.GetLeftSDFShape();
        const auto rightNode = sdfShape.GetRightSDFShape();
        if (!leftNode || !rightNode) {
            GE_LOGE("GESDFTreeProcessor::Process: one of the child nodes in the "
                    "sdf union shape is null.");
        }

        GenerateHeader(sdfShape);
        Process(leftNode);
        Process(rightNode);

        GenerateBody(sdfShape);
    }

    void GESDFTreeProcessor::GenerateHeader(const GESDFUnionOpShaderShape& sdfShape)
    {
        if (sdfShape.GetSDFUnionOp() == GESDFUnionOp::SMOOTH_UNION) {
            auto nodeId = reinterpret_cast<size_t>(&sdfShape);
            headers_ += "uniform float u_factor_" + std::to_string(nodeId) + ";\n";
        }
    }

    void GESDFTreeProcessor::GenerateBody(const GESDFUnionOpShaderShape& sdfShape)
    {
        auto nodeIdLeft = reinterpret_cast<size_t>(sdfShape.GetLeftSDFShape().get());
        auto nodeIdRight = reinterpret_cast<size_t>(sdfShape.GetRightSDFShape().get());
        auto nodeId = reinterpret_cast<size_t>(&sdfShape);

        if (sdfShape.GetSDFUnionOp() == GESDFUnionOp::SMOOTH_UNION) {
            body_ += "float var_" + std::to_string(nodeId) + " = SDFSmoothUnion(" +
                        "var_" + std::to_string(nodeIdLeft) +
                        ", var_" + std::to_string(nodeIdRight) +
                        ", u_factor_" + std::to_string(nodeId) + ");\n";
        } else {
            body_ += "float var_" + std::to_string(nodeId) + " = SDFUnion(" +
                        "var_" + std::to_string(nodeIdLeft) +
                        ", var_" + std::to_string(nodeIdRight) + ");\n";
        }
    }

    void GESDFTreeProcessor::UpdateUniformDatas(RuntimeShaderBuilder& builder,
        const GESDFUnionOpShaderShape& sdfShape)
    {
        auto nodeId = reinterpret_cast<size_t>(&sdfShape);
        if (sdfShape.GetSDFUnionOp() == GESDFUnionOp::SMOOTH_UNION) {
            float spacing = sdfShape.GetSpacing();
            spacing = std::max(spacing, 0.0001f);
            builder.SetUniform("u_factor_" + std::to_string(nodeId), spacing);
        }
        UpdateUniformDatas(builder, sdfShape.GetLeftSDFShape());
        UpdateUniformDatas(builder, sdfShape.GetRightSDFShape());
    }

    void GESDFTreeProcessor::UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder)
    {
        GE_LOGE("GESDFTreeProcessor::UpdateUniformDatas: UpdateUniformfromData.");
        if (sdfShape_) {
            for (const auto& effect: effectsContainer_) {
                effect->UpdateUniformDatas(builder);
            }

            UpdateUniformDatas(builder, sdfShape_);
        }
    }

    void GESDFTreeProcessor::UpdateParams(const GESDFFilterParams& params)
    {
        if (params.shape) {
            sdfShape_->CopyState(*params.shape);
        }
    }

    bool ComparatorForEffects::operator()(const std::unique_ptr<GESDFEffect>& effect1,
        const std::unique_ptr<GESDFEffect>& effect2) const
    {
        return GESDFEffect::InOrderComparator(*effect1, *effect2);
    }
}