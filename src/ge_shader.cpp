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
#include "ge_shader.h"

#include "ge_log.h"

namespace OHOS {
namespace Rosen {

bool GEShader::TryDrawShaderWithPen(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    auto subRect = GetSubtractedRect();
    bool rectNotValid = subRect.IsEmpty() || !subRect.IsValid() || !rect.Contains(subRect);
    if (rectNotValid) {
        return false;
    }

    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    float maxWidth = std::max({ width * subRect.GetLeft(), width * (1.f - subRect.GetRight()),
        height * subRect.GetTop(), height * (1.f - subRect.GetBottom())});
    float penWidth = maxWidth * 2.f; // needs to be doubled since the border meets the middle of pen
    Drawing::Pen pen;
    pen.SetWidth(penWidth);
    pen.SetShaderEffect(drShader_);
    canvas.AttachPen(pen);
    canvas.DrawRect(rect);
    canvas.DetachPen();
    return true;
}

void GEShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Preprocess(canvas, rect); // to calculate your cache data
    MakeDrawingShader(rect, -1.f); // not use progress
    auto shader = GetDrawingShader();
    if (TryDrawShaderWithPen(canvas, rect)) {
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS