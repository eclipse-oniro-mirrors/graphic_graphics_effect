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
#include "ge_system_properties.h"
#include "ge_trace.h"

namespace OHOS {
namespace Rosen {
namespace{
static constexpr const char* PROPERTY_DRAW_SHADER_OPT = "persist.sys.graphic.drawShaderOptEnable";
static constexpr const char* PROPERTY_DRAW_SHADER_VISUALIZED_OPT = "persist.sys.graphic.drawShaderVisualizedOptEnable";

bool GetDrawShaderOptimizationEnabled()
{
    return GESystemProperties::GetBoolSystemProperty(PROPERTY_DRAW_SHADER_OPT, true);
}

bool GetDrawShaderVisualizedOptimizationEnabled()
{
    return GESystemProperties::GetBoolSystemProperty(PROPERTY_DRAW_SHADER_VISUALIZED_OPT, false);
}

void DrawShaderVisualizedOptimizationEnabled(
    Drawing::Canvas& canvas, const Drawing::Rect& rect, const Drawing::Rect& subRect)
{
    auto visualizedSubRect = Drawing::Rect(rect.GetLeft() + subRect.GetLeft() * width,
            rect.GetTop() + subRect.GetTop() * width,
            rect.GetLeft() + subRect.GetRight() * width,
            rect.GetTop() + subRect.GetBottom() * width);
    Drawing::Brush brush;
    brush.SetColor(0x88FF0000);
    canvas.AttachBrush(brush);
    canvas.DrawRect(visualizedSubRect);
    canvas.DetachBrush();
    return;
}
}

bool GEShader::TryDrawShaderWithPen(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (UNLIKELY(!GetDrawShaderOptimizationEnabled())) {
        return false;
    }
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    auto subRect = GetSubtractedRect(width, height);
    bool rectNotValid = subRect.IsEmpty() || !subRect.IsValid();

    if (rectNotValid) {
        return false;
    }

    auto renderRect = Drawing::Rect(0.f, 0.f, 1.f, 1.f); // normalized render rectangle
    if (subRect.Contains(renderRect)) {
        // The render rectangle is fully subtracted, so the draw call can be skipped.
        return true;
    }
    if (!renderRect.Contains(subRect)) {
        // No optimazition for the subRect which reaches the edge of the render rectangle in any direction
        return false;
    }
    float maxWidth = std::max({ width * subRect.GetLeft(), width * (1.f - subRect.GetRight()),
        height * subRect.GetTop(), height * (1.f - subRect.GetBottom())});
    float penWidth = maxWidth * 2.f; // needs to be doubled since the border meets the middle of pen
    Drawing::Pen pen;
    pen.SetWidth(penWidth);
    pen.SetShaderEffect(drShader_);
    canvas.AttachPen(pen);
    canvas.DrawRect(rect);
    canvas.DetachPen();
    if (LIKELY(!GetDrawShaderVisualizedOptimizationEnabled())) {
        DrawShaderVisualizedOptimizationEnabled(canvas, rect, subRect);
    }
    return true;
}

void GEShader::DrawShader(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    Preprocess(canvas, rect); // to calculate your cache data
    MakeDrawingShader(rect, -1.f); // not use progress
    auto shader = GetDrawingShader();
    if (!shader) {
        GE_LOGE("GEShader::DrawShader: no shader generated, draw nothing");
        return;
    }
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