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
#ifndef GRAPHICS_EFFECT_PRTICLE_CIRCULAR_HALO_SHADER_H
#define GRAPHICS_EFFECT_PRTICLE_CIRCULAR_HALO_SHADER_H

#include "common/rs_vector2.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"

#include "ge_shader.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {
REGISTER_GEFILTER_TYPEINFO(PARTICLE_CIRCULAR_HALO, GEParticleCircularHaloShader,
                           Drawing::GEParticleCircularHaloShaderParams, Drawing::GE_SHADER_PARTICLE_CIRCULAR_HALO);
class GE_EXPORT GEParticleCircularHaloShader : public GEShader {
public:
    GEParticleCircularHaloShader();
    GEParticleCircularHaloShader(Drawing::GEParticleCircularHaloShaderParams& params);
    ~GEParticleCircularHaloShader() override = default;
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    const std::string GetDescription() const
    {
        return "GEParticleCircularHaloShader";
    }

private:
    bool FloatEqual(float a, float b);
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    static std::shared_ptr<GEParticleCircularHaloShader> CreateParticleCircularHaloShader(
        Drawing::GEParticleCircularHaloShaderParams& params);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetGlowHaloBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetSingleParticleHaloBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetParticleHaloBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetParticleCircularHaloBuilder();

    std::shared_ptr<Drawing::Image> MakeGlowHaloShader(Drawing::Canvas& canvas, const Drawing::ImageInfo& imageInfo);
    std::shared_ptr<Drawing::Image> MakeSingleParticleHaloShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);
    std::shared_ptr<Drawing::Image> MakeParticleHaloShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);
    std::shared_ptr<Drawing::ShaderEffect> MakeParticleCircularHaloShader(const Drawing::Rect& rect);

    GEParticleCircularHaloShader(const GEParticleCircularHaloShader&) = delete;
    GEParticleCircularHaloShader(const GEParticleCircularHaloShader&&) = delete;
    GEParticleCircularHaloShader& operator=(const GEParticleCircularHaloShader&) = delete;
    GEParticleCircularHaloShader& operator=(const GEParticleCircularHaloShader&&) = delete;

    Drawing::GEParticleCircularHaloShaderParams particleCircularHaloParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> glowHaloBuilder_ = nullptr;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> singleParticleHaloBuilder_ = nullptr;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> particleHaloBuilder_ = nullptr;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_ = nullptr;

    std::shared_ptr<Drawing::Image> particleHaloImg_ = nullptr;
    std::shared_ptr<Drawing::Image> glowHaloImg_ = nullptr;
};

}  // namespace Rosen
}  // namespace OHOS
#endif  // GRAPHICS_EFFECT_PRTICLE_CIRCULAR_HALO_SHADER_H