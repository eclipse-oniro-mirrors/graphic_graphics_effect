#include "ge_direct_draw_on_canvas_pass.h"

namespace OHOS {
namespace Rosen {
    
std::string_view GEDirectDrawOnCanvasPass::GetLogName() const
{
    return "GEDirectDrawOnCanvasPass";
}

GEFilterComposerPassResult GEDirectDrawOnCanvasPass::Run(std::vector<GEFilterComposable>& composables)
{
    if (composables.empty()) {
        return GEFilterComposerPassResult { false };
    }
    // now it won't be empty, it's safe to access .back()
    auto effect = composables.back().GetEffect();
    if (effect == nullptr) {
        return GEFilterComposerPassResult { false };
    }
    effect->SetAllowDirectDrawOnCanvas(true);
    return GEFilterComposerPassResult { true };
}

} // namespace Rosen
} // namespace OHOS