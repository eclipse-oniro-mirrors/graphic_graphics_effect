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

#ifndef GRAPHIC_2D_GE_COMPOSING_FILTER_H
#define GRAPHIC_2D_GE_COMPOSING_FILTER_H

#include <unordered_set>
#include <any>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
class GEFilterComposer {
public:
    GEFilterComposer(const std::shared_ptr<GEShaderFilter>& shaderFilter);
    ~GEFilterComposer() = default;

    bool Compose(const std::shared_ptr<GEShaderFilter> other);
    std::shared_ptr<Drawing::Image> ApplyComposedEffect(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst);

    std::shared_ptr<GEShaderFilter> GetComposedFilter();

private:
    std::shared_ptr<GEShaderFilter> GenerateComposedFilter(
        const std::string composedType, const std::map<std::string, GEShaderFilter::FilterParams> filterParams);
    const std::unordered_set<std::string> composedEffects_ = {
        "GreyBlur",
    };
    std::shared_ptr<GEShaderFilter> composedFilter_ = nullptr;
    std::string composedType_ = "";
    std::map<std::string, GEShaderFilter::FilterParams> filterParams_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_2D_GE_COMPOSING_FILTER_H
