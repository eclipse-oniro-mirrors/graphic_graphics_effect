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

#ifndef GRAPHICS_EFFECT_GE_SDF_EFFECTS_H
#define GRAPHICS_EFFECT_GE_SDF_EFFECTS_H

#include "draw/color.h"
#include "draw/path.h"
#include <string>

namespace OHOS::Rosen::Drawing {
    class RuntimeShaderBuilder;
}

namespace OHOS::Rosen::Drawing {
enum class SDFEffectOrder {
    SHADOW,
    BORDER
};

class GESDFEffect {
public:
    virtual ~GESDFEffect() = default;
    virtual void Process(std::string& headers, std::string& calls, std::string& functions) const = 0;
    virtual void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder) const = 0;

    [[nodiscard]]
    virtual SDFEffectOrder GetOrder() const = 0;
    
    static bool InOrderComparator(const GESDFEffect& effect1, const GESDFEffect& effect2);
};

class GESDFBorder final : public GESDFEffect {
    void Process(std::string& headers, std::string& calls, std::string& functions) const override;
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder) const override;

    SDFEffectOrder GetOrder() const override;
public:
    explicit GESDFBorder(const Color& color, float width);

private:
    Color color_;
    float width_ = 0.0f;
};

class GESDFShadow final : public GESDFEffect {
    void Process(std::string& headers, std::string& calls, std::string& functions) const override;
    void UpdateUniformDatas(Drawing::RuntimeShaderBuilder& builder) const override;

    SDFEffectOrder GetOrder() const override;
public:
    explicit GESDFShadow(const Color& color, float offsetX, float offsetY,
                        float radius, const Path& path, bool isFilled);

private:
    Color color_;
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;
    float radius_ = 0.0f;
    // Path path_;
    bool isFilled_ = false;
};
}
#endif // GRAPHICS_EFFECT_GE_SDF_EFFECTS_H
