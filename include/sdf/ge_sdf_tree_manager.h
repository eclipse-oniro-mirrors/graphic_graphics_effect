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

#ifndef GRAPHICS_EFFECT_GE_SDF_TREE_MANAGER_H
#define GRAPHICS_EFFECT_GE_SDF_TREE_MANAGER_H
 
#include "ge_common.h"
#include "ge_shader_filter_params.h"
 
namespace OHOS {
namespace Rosen {
namespace Drawing {
class GESDFTreeManager final {
public:
    GESDFTreeManager()
    {
        prog_ = R"(
            half4 main(vec2 fragCoord)
            {
        )";
    }
    GESDFTreeManager(const GESDFTreeManager&) = delete;
    virtual ~GESDFTreeManager() = default;
    voild ApplyParam(std::shared_ptr<Drawing::RuntimeShaderBuilder> builder)
    {
        constexpr float HALF = 0.5;
        if (!builder) {
            return;
        }
        for (auto [name, value] : rrectMap_) {
            builder->SetUniform("centerPos" + name, value.rrect.left_ + value.rrect.width_ * HALF,
                value.rrect.top_ + value.rrect.height_ * HALF);
            builder->SetUniform("halfSize" + name, value.rrect.width_ * HALF, value.rrect.height_ * HALF);
            builder->SetUniform("radius" + name, value.rrect.radiusX_);
        }
        for (auto [name, value] : floatMap_) {
            builder->SetUniform(name, value);
        }
        for (auto [name, value] : matrixMap_) {
            builder->SetUniform(name, value);
        }
    }

    void PrependProg(std::string prog)
    {
        prog_ = prog + prog_;
    }

    void AppendProg(std::string prog)
    {
        prog_ += prog;
    }

    void CompleteProg(std::string returnProg)
    {
        prog_ += returnProg;
        prog_ += R"(
            }
        )";
    }

    const std::string& GetProg()
    {
        return prog_;
    }

    std::map<std::string, float>& GetMutableRRectMap()
    {
        return rrectMap_;
    }

    std::map<std::string, float>& GetMutableFloatMap()
    {
        return floatMap_;
    }

    std::map<std::string, Drawing::Matrix>& GetMutableMatrixMap()
    {
        return matrixMap_;
    }

    bool AddSDFType(GESDFShapeType type)
    {
        if (typeSet_.find(type) != typeSet.end()) {
            return false;
        }
        typeSet_.insert(type);
        return true;
    }
    

private:
    std::string prog_;
    std::set<GESDFShapeType> typeSet_;
    std::map<std::string, GERRect> rrectMap_;
    std::map<std::string, float> floatMap_;
    std::map<std::string, Drawing::Matrix> matrixMap_;

};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
 
#endif // GRAPHICS_EFFECT_GE_SDF_TREE_MANAGER_H