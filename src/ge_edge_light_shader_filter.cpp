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

#include "ge_edge_light_shader_filter.h"

#include <array>
#include <vector>

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {
namespace {
    static constexpr float MIN_IMAGE_SIZE = 1e-6;
}

GEEdgeLightShaderFilter::GEEdgeLightShaderFilter(const Drawing::GEEdgeLightShaderFilterParams& params)
{
    edgeLightEdgeThreshold_ = params.edgeLightEdgeThreshold;
    edgeLightEdgeIntensity_ = params.edgeLightEdgeIntensity;
    edgeLightEdgeSoftThreshold_ = params.edgeLightEdgeSoftThreshold;
    edgeDetectColorR_ = params.edgeDetectColorR;
    edgeDetectColorG_ = params.edgeDetectColorG;
    edgeDetectColorB_ = params.edgeDetectColorB;
    edgeColorR_ = params.edgeColorR;
    edgeColorG_ = params.edgeColorG;
    edgeColorB_ = params.edgeColorB;
    edgeRawColor_ = params.edgeRawColor;
    edgeBloomLevel_ = params.edgeBloomLevel;
    edgeLightIfGradient_ = params.edgeLightIfGradient;
    alphaProgress_ = params.alphaProgress;
    addImage_ = params.addImage;
}

std::shared_ptr<Drawing::Image>GEEdgeLightShaderFilter::ProcessImage(Drawing::Canvas &canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect &src, const Drawing::Rect &dst)
{
    if (image == nullptr) {
        LOGE("GEEdgeLightShaderFilter:: ProcessImage input image is invalid.");
        return nullptr;
    }

    auto originImageInfo = image->GetImageInfo();
    float height = originImageInfo.GetHeight();
    float width = originImageInfo.GetWidth();
    if (height < MIN_IMAGE_SIZE || width < MIN_IMAGE_SIZE) {
        LOGE("GEEdgeLightShaderFilter::Input image size invalid.");
        return nullptr;
    }
    
    return nullptr;
}
}
}

 