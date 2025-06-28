#ifndef GRAPHICS_EFFECT_CONTOUR_DIAGONAL_FLOW_LIGHT_SHADER_H
#define GRAPHICS_EFFECT_CONTOUR_DIAGONAL_FLOW_LIGHT_SHADER_H

#include "ge_shader.h"
#include "common/rs_vector4.h"
#include "effect/runtime_shader_builder.h"
#include "utils/matrix.h"
#include "ge_shader_filter_params.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEContourDiagonalFlowLightShader : public GEShader {
public:
    GEContourDiagonalFlowLightShader();
    GEContourDiagonalFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& contourDiagonalFlowLightParams);

    ~GEContourDiagonalFlowLightShader() override = default;

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;

    const std::string GetDescription() const { return "GEContourDiagonalFlowLightShader"; }

    void SetGEContentDiagonalFlowLightShaderParams(const Drawing::GEContentDiagonalFlowLightShaderParams& params)
    {
        contourDiagonalFlowLightParams_ = params;
    }

    void Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect) override;

    std::shared_ptr<Drawing::ShaderEffect> MakeContourDiagonalFlowLightShader(const Drawing::Rect& rect);

    static std::shared_ptr<GEContourDiagonalFlowLightShader>
        CreateContourDiagonalFlowLightShader(Drawing::GEContentDiagonalFlowLightShaderParams& param);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightBuilder();

    std::shared_ptr<Drawing::Image> MakeContourDiagonalFlowLightPrecalculationShader(Drawing::Canvas& canvas,
        const Drawing::ImageInfo& imageInfo);

    std::shared_ptr<Drawing::RuntimeShaderBuilder> GetContourDiagonalFlowLightPrecalculationBuilder();
private:
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader(const GEContourDiagonalFlowLightShader&&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&) = delete;
    GEContourDiagonalFlowLightShader& operator=(const GEContourDiagonalFlowLightShader&&) = delete;
    static std::vector<float> PathStringToFloats(const std::string& str);
    Drawing::GEContentDiagonalFlowLightShaderParams contourDiagonalFlowLightParams_;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder_;
    std::vector<float> controlPoints_;
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H