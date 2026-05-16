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

#ifndef GE_TRANSFORM_HELPER
#define GE_TRANSFORM_HELPER
#include <array>
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "ge_log.h"
#include <iomanip>
#include "util/ge_common.h"
#include <sstream>

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct GECameraIntrinsics {
    float fov = 60.0f;
    float aspectRatio = 1.0f;
    float near = 0.1f;
    float far = 100.0f;
    float xOffset = 0.f;
    float yOffset = 0.f;
};

struct GECameraExtrinsics {
    Quaternion rotation = Quaternion();
    float position[3] = {0.0f};
};
}  // namespace Drawing

template <size_t Rows, size_t Cols>
std::string MatrixToString(const std::array<float, Rows * Cols> &matrix)
{
    std::ostringstream oss;
    for (size_t i = 0; i < Rows; ++i) {
        oss << "[";
        for (size_t j = 0; j < Cols; ++j) {
            size_t index = i * Cols + j;
            oss << std::fixed << std::setprecision(8) << std::setw(20) << matrix[index];
        }
        oss << "]";
    }
    return oss.str();
}

class GE_EXPORT PerspectiveTransformCalculator {
public:
    static std::array<float, 16> ComputeVPMatrix(const Drawing::GECameraIntrinsics &intrinsics,
        const Drawing::GECameraExtrinsics &extrinsics, const std::array<Vector3f, 4> &cornerPoints);

private:
    static std::array<float, 16> BuildProjectionMatrixRhZo(const Drawing::GECameraIntrinsics &intrinsics);

    static std::array<float, 16> BuildViewMatrixRh(const Drawing::GECameraExtrinsics &extrinsics);

    static void MatrixMultiply(std::array<float, 16> &result, const std::array<float, 16> &a,
        const std::array<float, 16> &b);
    
    static std::array<float, 9> QuaternionToRotationMatrix(const Quaternion &quaternion);
};
}  // Rosen
} // OHOS
#endif  // GE_TRANSFORM_HELPER