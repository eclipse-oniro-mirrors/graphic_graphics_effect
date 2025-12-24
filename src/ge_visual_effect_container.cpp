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

#include "ge_visual_effect_container.h"

#include "ge_log.h"
#include "ge_visual_effect_impl.h"
#include "ge_use_effect_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffectContainer::GEVisualEffectContainer() {}

void GEVisualEffectContainer::AddToChainedFilter(std::shared_ptr<Drawing::GEVisualEffect> visualEffect)
{
    if (!visualEffect) {
        LOGD("GEVisualEffectContainer::AddToChainedFilter visualEffect is nullptr");
        return;
    }
    LOGD("GEVisualEffectContainer::AddToChainedFilter %{public}s", visualEffect->GetName().c_str());
    filterVec_.push_back(visualEffect);
}

void GEVisualEffectContainer::UpdateCacheDataFrom(const std::shared_ptr<GEVisualEffectContainer>& ge)
{
    if (ge == nullptr) {
        return;
    }
    for (auto vef : ge->GetFilters()) {
        if (vef == nullptr || vef->GetImpl() == nullptr) {
            LOGD("GEVisualEffectContainer::UpdateCacheDataFrom vef is null");
            continue;
        }
        auto vefTarget = GetGEVisualEffect(vef->GetName());
        if (vefTarget == nullptr || vefTarget->GetImpl() == nullptr) {
            LOGD("GEVisualEffectContainer::UpdateCacheDataFrom ve is null");
            continue;
        }
        vefTarget->GetImpl()->SetCache(vef->GetImpl()->GetCache());
    }
}

std::shared_ptr<GEVisualEffect> GEVisualEffectContainer::GetGEVisualEffect(const std::string& name)
{
    for (auto vef : filterVec_) {
        if (vef == nullptr) {
            LOGD("GEVisualEffectContainer::GetGEVisualEffect vef is null");
            continue;
        }
        if (vef->GetName() == name) {
            return vef;
        }
    }
    return nullptr;
}

void GEVisualEffectContainer::SetGeometry(const Drawing::Matrix& matrix, const Drawing::RectF& bound,
    const Drawing::RectF& materialDst, float geoWidth, float geoHeight)
{
    Drawing::CanvasInfo info { std::ceil(geoWidth), std::ceil(geoHeight),
        bound.GetLeft(), bound.GetTop(), matrix, materialDst};
    for (auto vef : GetFilters()) {
        if (vef) {
            vef->SetCanvasInfo(info);
        }
    }
}

void GEVisualEffectContainer::UpdateCachedBlurImage(Drawing::Canvas* canvas,
    std::shared_ptr<Drawing::Image> cachedImage, float left, float top)
{
    for (auto vef : GetFilters()) {
        if (vef->GetName() == "HarmoniumEffect") {
            vef->SetParam(GE_SHADER_HARMONIUM_EFFECT_BLURLEFT, left);
            vef->SetParam(GE_SHADER_HARMONIUM_EFFECT_BLURTOP, top);
            if (vef->GetImpl() == nullptr) {
                continue;
            }
            std::shared_ptr<GEHarmoniumEffectShaderParams> params = vef->GetImpl()->GetHarmoniumEffectParams();
            if (params->useEffectMask != nullptr) {
                GEUseEffectMaskParams maskParam;
                maskParam.useEffect = params->useEffectMask->GetUseEffect();
                maskParam.image = cachedImage;
                std::shared_ptr<GEUseEffectShaderMask> useeffectMask =
                    std::make_shared<GEUseEffectShaderMask>(maskParam);
                vef->SetParam(GE_SHADER_HARMONIUM_EFFECT_USEEFFECTMASK, useeffectMask);
            }
        }
        if (vef->GetName() == GE_SHADER_FROSTED_GLASS_EFFECT) {
            vef->SetParam(GE_SHADER_FROSTED_GLASS_EFFECT_BLURIMAGE, cachedImage);
        }
    }
}

void GEVisualEffectContainer::UpdateFrostedGlassEffectParams(std::shared_ptr<Drawing::Image> blurImageForEdge, float value)
{
    for (auto vef : GetFilters()) {
        if (vef->GetName() == GE_SHADER_FROSTED_GLASS_EFFECT) {
            vef->SetParam(GE_SHADER_FROSTED_GLASS_EFFECT_REFRACTOUTPX, value);
            vef->SetParam(GE_SHADER_FROSTED_GLASS_EFFECT_BLURIMAGEFOREDGE, blurImageForEdge);
        }
    }
}

void GEVisualEffectContainer::UpdateCornerRadius(float cornerRadius)
{
    for (auto vef : GetFilters()) {
        if (vef->GetName() == "HarmoniumEffect") {
            vef->SetParam(GE_SHADER_HARMONIUM_EFFECT_CORNERRADIUS, cornerRadius);
        }
     }
}

void GEVisualEffectContainer::UpdateTotalMatrix(Drawing::Matrix totalMatrix)
{
    for (auto vef : GetFilters()) {
        if (vef->GetName() == "HarmoniumEffect") {
            vef->SetParam(GE_SHADER_HARMONIUM_EFFECT_TOTALMATRIX, totalMatrix);
        }
        if (vef->GetName() == GE_SHADER_FROSTED_GLASS_EFFECT) {
            vef->SetParam(GE_SHADER_FROSTED_GLASS_EFFECT_SNAPSHOTMATRIX, totalMatrix);
        }
    }
}

void GEVisualEffectContainer::SetDisplayHeadroom(float headroom)
{
    for (auto vef : GetFilters()) {
        if (vef) {
            vef->SetSupportHeadroom(headroom);
        }
    }
}

void GEVisualEffectContainer::UpdateDarkScale(float darkScale)
{
    for (auto vef : GetFilters()) {
        if (vef->GetName() == GE_FILTER_FROSTED_GLASS) {
            vef->SetParam(GE_SHADER_FROSTED_GLASS_DARK_SCALE, darkScale);
        }
    }
}

void GEVisualEffectContainer::RemoveFilterWithType(int32_t typeToRemove)
{
    filterVec_.erase(
        std::remove_if(filterVec_.begin(), filterVec_.end(),
            [&](const std::shared_ptr<Drawing::GEVisualEffect>& ptr) {
                return ptr && ptr->GetImpl() && static_cast<int32_t>(ptr->GetImpl()->GetFilterType()) == typeToRemove;
            }),
        filterVec_.end()
    );
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
