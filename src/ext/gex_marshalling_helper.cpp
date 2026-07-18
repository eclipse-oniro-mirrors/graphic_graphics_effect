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

#include "ge_log.h"
#include "gex_flow_light_sweep_shader.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

constexpr uint32_t MARSHALLING_SIZE_MAX_LIMIT = 100;  // 100 max length

bool GEXFlowLightSweepParams::Marshalling(Parcel& parcel)
{
    uint32_t effectColorsSize = static_cast<uint32_t>(effectColors_.size());
    if (effectColorsSize > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("GEXFlowLightSweepParams::Marshalling effectColors size exceeded the limit.");
        return false;
    }

    if (!parcel.WriteUint32(effectColorsSize)) {
        GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsSize failed!");
        return false;
    }

    for (size_t i = 0; i < effectColorsSize; i++) {
        if (!parcel.WriteUint32((uint32_t)effectColors_[i].first.CastToColorQuad())) {
            GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsFirst failed!");
            return false;
        }
        if (!parcel.WriteFloat(effectColors_[i].second)) {
            GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsSecond failed!");
            return false;
        }
    }

    return true;
}

bool GEXFlowLightSweepParams::Unmarshalling(Parcel& parcel)
{
        uint32_t effectColorsSize = 0;
        if (!parcel.ReadUint32(effectColorsSize)) {
            GE_LOGE("GEXFlowLightSweepParams::Unmarshalling Read effectColorsSize failed!");
            return false;
        }
        if (effectColorsSize > MARSHALLING_SIZE_MAX_LIMIT) {
            GE_LOGE("GEXFlowLightSweepParams::Unmarshalling effectColors size exceeded the limit.");
            return false;
        }
 
        uint32_t valueUint32 = 0;
        float valueFloat = 0.f;
        effectColors_.clear();
        effectColors_.reserve(effectColorsSize);
        for (size_t i = 0; i < effectColorsSize; i++) {
            if (!parcel.ReadUint32(valueUint32) || !parcel.ReadFloat(valueFloat)) {
                GE_LOGE("GEXFlowLightSweepParams::Unmarshalling Read effectColor failed!");
                return false;
            }
            std::pair<Drawing::Color, float> effectColor;
            effectColor.first = Drawing::Color(valueUint32);
            effectColor.second = valueFloat;
 
            effectColors_.emplace_back(effectColor);
        }
 
        return true;
}

} // namespace Rosen
} // namespace OHOS
