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

#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT $CLASS_NAME : public GEShader {
public:
    $CLASS_NAME(const Drawing::$PARAMS_CLASS& params);
    $CLASS_NAME(const $CLASS_NAME&) = delete;
    $CLASS_NAME operator=(const $CLASS_NAME&) = delete;
    $CLASS_NAME($CLASS_NAME&&) = delete;
    $CLASS_NAME& operator=($CLASS_NAME&&) = delete;
    ~$CLASS_NAME() override = default;
    DECLARE_GEFILTER_TYPEFUNC($CLASS_NAME, Drawing::$PARAMS_CLASS);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
    const std::string GetDescription() const { return "$CLASS_NAME"; }

private:
$MEMBER_DECLARATIONS
};

} // namespace Rosen
} // namespace OHOS

#endif // $HEADER_GUARD
