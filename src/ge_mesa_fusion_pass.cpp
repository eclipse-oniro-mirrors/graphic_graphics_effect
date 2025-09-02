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
#include <memory>

#include "ge_mesa_fusion_pass.h"
#include "ge_filter_type.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

std::string_view GEMesaFusionPass::GetLogName() const
{
    return "GEMesaFusionPass";
}

GEFilterComposerPassResult GEMesaFusionPass::Run(std::vector<GEFilterComposable>& composables)
{
    using Drawing::GEFilterType;
    std::vector<GEFilterComposable> resultComposables;
    bool composed = false;
    for (size_t i = 0; i < composables.size() - 1; ++i) {
        size_t j = i + 1;
        auto iEffect = composables[i].GetEffect();
        auto jEffect = composables[j].GetEffect();
        if (iEffect == nullptr || jEffect == nullptr) {
            resultComposables.push_back(composables[i]);
            continue;
        }
        auto iImpl = iEffect->GetImpl();
        auto jImpl = jEffect->GetImpl();
        if (iImpl == nullptr || jImpl == nullptr) {
            resultComposables.push_back(composables[i]);
            continue;
        }
        if (iImpl->GetFilterType() == GEFilterType::GREY
            && jImpl->GetFilterType() == GEFilterType::KAWASE_BLUR) {
            auto&& greyParams = iImpl->GetGreyParams();
            auto&& blurParams = jImpl->GetKawaseParams();
            auto mesaFilter = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_MESA_BLUR,
                Drawing::DrawingPaintType::BRUSH);
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
            resultComposables.push_back(mesaFilter);
            i = j; // consumed 2 effects, skip composed j
            composed = true;
            LOGD("GEMesaFusionPass::Run Grey Kawase fused");
        } else {
            resultComposables.push_back(composables[i]);
        }
    }
    if (composed) {
        composables.swap(resultComposables);
    }
    return GEFilterComposerPassResult {composed};
}

}
}
