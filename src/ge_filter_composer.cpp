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
#include "ge_log.h"
#include "ge_render.h"

namespace OHOS {
namespace Rosen {

GEFilterComposer::GEFilterComposer(const std::shared_ptr<GEShaderFilter>& shaderFilter)
{
    if (shaderFilter != nullptr) {
        composedFilter_ = shaderFilter;
        composedType_ = shaderFilter->Type();
        filterParams_[composedType_] = shaderFilter->Params();
    } else {
        LOGE("GEFilterComposer: ctor with nullptr");
    }
}

bool GEFilterComposer::Compose(const std::shared_ptr<GEShaderFilter> other)
{
    if (other == nullptr || other == composedFilter_) {
        LOGE("GEFilterComposer: compose with nullptr or self");
        return false;
    }

    auto otherFilterType = other->Type();
    std::string composedType = composedType_ + otherFilterType;
    if (composedEffects_.count(composedType)) {
        filterParams_[otherFilterType] = other->Params();
        if (auto composedFilter = GenerateComposedFilter(composedType, filterParams_)) {
            composedFilter_ = composedFilter;
            composedType_ = composedType;
            return true;
        }
    }
    return false;
}

const std::shared_ptr<GEShaderFilter>& GEFilterComposer::GetComposedFilter()
{
    return composedFilter_;
}

std::shared_ptr<GEShaderFilter> GEFilterComposer::GenerateComposedFilter(
    const std::string& composedType, const std::map<std::string, GEShaderFilter::FilterParams>& filterParams)
{
    // Only support GreyBlur now, support more complex filter in the future
    if (composedType == "GreyBlur") {
        auto mesaFilter = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR,
            Drawing::DrawingPaintType::BRUSH);
        auto blurIt = filterParams.find("Blur");
        if (blurIt == filterParams.end()) {
            return nullptr;
        }
        const auto* blurParams = std::get_if<Drawing::GEKawaseBlurShaderFilterParams>(&blurIt->second.value());
        if (!blurParams) {
            return nullptr;
        }
        auto greyIt = filterParams.find("Grey");
        if (greyIt == filterParams.end()) {
            return nullptr;
        }

        const auto* greyParams = std::get_if<Drawing::GEGreyShaderFilterParams>(&greyIt->second.value());
        if (!greyParams) {
            return nullptr;
        }
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, blurParams->radius);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, greyParams->greyCoef1);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, greyParams->greyCoef2);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.f);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.f);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.f);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.f);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.f);
        mesaFilter->SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.f);

        GraphicsEffectEngine::GERender render;
        return render.GenerateShaderFilter(mesaFilter);
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
