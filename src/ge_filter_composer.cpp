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

#include "ge_filter_composer.h"
#include "ge_render.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

GEFilterComposer::GEFilterComposer(const std::shared_ptr<GEShaderFilter> shaderFilter)
    : composedFilter_(shaderFilter), composedType_(shaderFilter->Type())
{
    filterParams_[composedType_] = composedFilter_->Params();
}

bool GEFilterComposer::Compose(const std::shared_ptr<GEShaderFilter> other)
{
    auto otherFilterType = other->Type();
    std::string composedType = composedType_ + otherFilterType;
    if (composedEffects_.count(composedType)) {
        filterParams_[otherFilterType] = other->Params();
        if (auto composedFilter = GetComposedFilter(composedType, filterParams_)) {
            composedFilter_ = composedFilter;
            composedType_ = composedType;
            return true;
        }
    }
    return false;
}

std::shared_ptr<GEShaderFilter> GEFilterComposer::GetComposedFilter(
    const std::string composedType, const std::map<std::string, GEShaderFilter::FilterParams> filterParams)
{
    if (composedType == "GreyBlur") {
        auto mesaFilter = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR,
            Drawing::DrawingPaintType::BRUSH);
        auto blurIt = filterParams.find("Blur");
        if (blurIt == filterParams.end()) {
            return nullptr;
        }
        if (const auto* blurParams = std::get_if<Drawing::GEKawaseBlurShaderFilterParams>(&blurIt->second.value())) {
            mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, blurParams->radius);
        } else {
            return nullptr;
        }
        auto greyIt = filterParams.find("Grey");
        if (greyIt == filterParams.end()) {
            return nullptr;
        }

        if (const auto* greyParams = std::get_if<Drawing::GEGreyShaderFilterParams>(&greyIt->second.value())) {
            mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, greyParams->greyCoef1);
            mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, greyParams->greyCoef2);
        } else {
            return nullptr;
        }

        GraphicsEffectEngine::GERender render;
        return render.GenerateShaderFilter(mesaFilter);
    }
    return nullptr;
}

std::shared_ptr<Drawing::Image> GEFilterComposer::ApplyComposedEffect(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto resImage = composedFilter_->ProcessImage(canvas, image, src, dst);
    return resImage;
}

} // namespace Rosen
} // namespace OHOS
