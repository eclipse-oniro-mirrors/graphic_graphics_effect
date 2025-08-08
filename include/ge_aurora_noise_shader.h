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

#ifndef GRAPHICS_EFFECT_AURORA_NOISE_SHADER_H
#define GRAPHICS_EFFECT_AURORA_NOISE_SHADER_H

#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"
namespace OHOS {
namespace Rosen {
REGISTER_GEFILTER_TYPEINFO(AURORA_NOISE, GEAuroraNoiseShader, Drawing::GEAuroraNoiseShaderParams);
class GE_EXPORT GEAuroraNoiseShader : public GEShader {
public:
    GEAuroraNoiseShader();
    GEAuroraNoiseShader(Drawing::GEAuroraNoiseShaderParams& auroraNoiseParams);
    ~GEAuroraNoiseShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEAuroraNoiseShader);
    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    const std::string GetDescription() const { return "GEAuroraNoiseShader"; }
    void SetAuroraNoiseParams(const Drawing::GEAuroraNoiseShaderParams& params)
    {
        auroraNoiseParams_ = params;
    }
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;
    std::shared_ptr<Drawing::Image> MakeAuroraNoiseGeneratorShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);
    std::shared_ptr<Drawing::Image> MakeAuroraNoiseVerticalBlurShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);
    std::shared_ptr<Drawing::ShaderEffect> MakeAuroraNoiseShader(const Drawing::Rect& rect);
    static std::shared_ptr<GEAuroraNoiseShader> CreateAuroraNoiseShader(Drawing::GEAuroraNoiseShaderParams& param);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetAuroraNoiseBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetAuroraNoiseVerticalBlurBuilder();
    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetAuroraNoiseUpSamplingBuilder();
private:
    GEAuroraNoiseShader(const GEAuroraNoiseShader&) = delete;
    GEAuroraNoiseShader(const GEAuroraNoiseShader&&) = delete;
    GEAuroraNoiseShader& operator=(const GEAuroraNoiseShader&) = delete;
    GEAuroraNoiseShader& operator=(const GEAuroraNoiseShader&&) = delete;
    Drawing::GEAuroraNoiseShaderParams auroraNoiseParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> verticalBlurBuilder_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> upSamplingBuilder_;
    std::shared_ptr<Drawing::Image> noiseImg_;
    std::shared_ptr<Drawing::Image> verticalBlurImg_;
};
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H