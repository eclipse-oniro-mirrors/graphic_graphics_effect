/*
 * Copyright (c) $YEAR Huawei Device Co., Ltd.
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

#ifndef $HEADER_GUARD
#define $HEADER_GUARD

#include "ge_sdf_shader_shape.h"
#include "ge_shader_filter_params.h"

// Additional includes for complex shapes:
// #include "effect/runtime_shader_builder.h"
// #include "ge_shader.h"
// #include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT $CLASS_NAME : public GESDFShaderShape {
public:
    $CLASS_NAME(const $PARAMS_CLASS& params);
    $CLASS_NAME(const $CLASS_NAME&) = delete;
    $CLASS_NAME operator=(const $CLASS_NAME&) = delete;
    $CLASS_NAME($CLASS_NAME&&) = delete;
    $CLASS_NAME& operator=($CLASS_NAME&&) = delete;
    ~$CLASS_NAME() override = default;
    DECLARE_GEFILTER_TYPEFUNC($CLASS_NAME, $PARAMS_CLASS);

    std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
    std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;

private:
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetShaderBuilder() const;
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> GetNormalShaderBuilder() const;

    // Optional: Common implementation for both shader types
    static std::shared_ptr<ShaderEffect> GenerateShaderEffect(
        std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const;

$MEMBER_DECLARATIONS
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // $HEADER_GUARD
