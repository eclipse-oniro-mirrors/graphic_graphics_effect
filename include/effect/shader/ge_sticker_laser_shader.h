/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_STICKER_LASER_SHADER_H
#define GRAPHICS_EFFECT_GE_STICKER_LASER_SHADER_H

#include "draw/canvas.h"
#include "effect/runtime_shader_builder.h"
#include "ge_filter_type_info.h"
#include "ge_shader.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEStickerLaserShader : public GEShader {
    friend class GEStickerLaserShaderTest;

public:
    GEStickerLaserShader(const Drawing::GEStickerLaserShaderParams& params);
    GEStickerLaserShader(const GEStickerLaserShader&) = delete;
    GEStickerLaserShader& operator=(const GEStickerLaserShader&) = delete;
    GEStickerLaserShader(GEStickerLaserShader&&) = delete;
    GEStickerLaserShader& operator=(GEStickerLaserShader&&) = delete;
    ~GEStickerLaserShader() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEStickerLaserShader, Drawing::GEStickerLaserShaderParams);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override {}

    void MakeDrawingShader(Drawing::Canvas& canvas, const Drawing::Rect& rect, float progress) override;

protected:
    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

private:
    const Drawing::GEStickerLaserShaderParams params_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_STICKER_LASER_SHADER_H
