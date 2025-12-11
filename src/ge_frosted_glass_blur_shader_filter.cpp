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
#include "ge_frosted_glass_blur_shader_filter.h"

#include "ge_log.h"
#include "ge_mesa_blur_shader_filter.h"

namespace OHOS {
namespace Rosen {
GEFrostedGlassBlurShaderFilter::GEFrostedGlassBlurShaderFilter(
    const Drawing::GEFrostedGlassBlurShaderFilterParams& params)
{
    blurParams_ = params;
}

std::shared_ptr<Drawing::Image> GEFrostedGlassBlurShaderFilter::OnProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (ROSEN_LE(blurParams_.radius, 0.0f)) {
        return image;
    }
    Drawing::GEMESABlurShaderFilterParams params;
    params.radius = blurParams_.radius;

    GEMESABlurShaderFilter blurFilter(params);
    auto blurImage = blurFilter.OnProcessImageWithoutUpSampling(canvas, image, src, dst);
    return blurImage;
}
} // namespace Rosen
} // namespace OHOS