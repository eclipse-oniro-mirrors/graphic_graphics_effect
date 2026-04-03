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

#include "util/ge_transform_helper.h"
#include <cmath>

namespace OHOS {
namespace Rosen {

static constexpr float EPSILON = 1e-6f;

std::array<float, 16> PerspectiveTransformCalculator::BuildProjectionMatrixRhZo(
    const Drawing::GECameraIntrinsics &intrinsics)
{
    std::array<float, 16> identityMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    std::array<float, 16> projMatrix = {0.0f};
    float tanHalfFov = tanf(intrinsics.fov / 2.0f);

    if (GE_EQ(intrinsics.aspectRatio, 0.0f, EPSILON) || GE_EQ(tanHalfFov, 0.0f, EPSILON) ||
        GE_EQ(intrinsics.far - intrinsics.near, 0.0f, EPSILON)) {
        GE_LOGE("BuildProjectionMatrixRhZo: invalid projection parameters. "
            "aspectRatio=%{public}f, tanHalfFov=%{public}f, far=%{public}f, near=%{public}f",
            intrinsics.aspectRatio, tanHalfFov, intrinsics.far, intrinsics.near);
        return identityMatrix;
    }
    projMatrix[0] = 1.0f / (tanHalfFov * intrinsics.aspectRatio);
    projMatrix[5] = - 1.0f / tanHalfFov;
    projMatrix[10] = - intrinsics.far / (intrinsics.far - intrinsics.near);
    projMatrix[11] = -1.0f;
    projMatrix[14] = -(intrinsics.far * intrinsics.near) / (intrinsics.far - intrinsics.near);

    return projMatrix;
}

std::array<float, 16> PerspectiveTransformCalculator::BuildViewMatrixRh(const Drawing::GECameraExtrinsics &extrinsics)
{
    std::array<float, 16> viewMatrix = {0.0f};

    auto rotMatrix = QuaternionToRotationMatrix(extrinsics.rotation);

    viewMatrix[0] = rotMatrix[0];
    viewMatrix[1] = rotMatrix[3];
    viewMatrix[2] = rotMatrix[6];

    viewMatrix[4] = rotMatrix[1];
    viewMatrix[5] = rotMatrix[4];
    viewMatrix[6] = rotMatrix[7];

    viewMatrix[8] = rotMatrix[2];
    viewMatrix[9] = rotMatrix[5];
    viewMatrix[10] = rotMatrix[8];

    viewMatrix[12] = -(viewMatrix[0] * extrinsics.position[0] + viewMatrix[4] * extrinsics.position[1] +
                       viewMatrix[8] * extrinsics.position[2]);

    viewMatrix[13] = -(viewMatrix[1] * extrinsics.position[0] + viewMatrix[5] * extrinsics.position[1] +
                       viewMatrix[9] * extrinsics.position[2]);

    viewMatrix[14] = -(viewMatrix[2] * extrinsics.position[0] + viewMatrix[6] * extrinsics.position[1] +
                       viewMatrix[10] * extrinsics.position[2]);

    viewMatrix[15] = 1.0f;

    return viewMatrix;
}

std::array<float, 16> PerspectiveTransformCalculator::ComputeVPMatrix(const Drawing::GECameraIntrinsics &intrinsics,
    const Drawing::GECameraExtrinsics &extrinsics, const std::array<Vector3f, 4> &cornerPoints)
{
    auto viewMatrix = BuildViewMatrixRh(extrinsics);
    auto projMatrix = BuildProjectionMatrixRhZo(intrinsics);

    std::array<float, 16> vpMatrix;
    MatrixMultiply(vpMatrix, projMatrix, viewMatrix);
    return vpMatrix;
}

std::array<float, 9> PerspectiveTransformCalculator::QuaternionToRotationMatrix(const Quaternion &quaternion)
{
    std::array<float, 9> rotationMatrix = {};
    float qx = quaternion.x_;
    float qy = quaternion.y_;
    float qz = quaternion.z_;
    float qw = quaternion.w_;

    float len = std::sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
    if (len > EPSILON) {
        qx /= len;
        qy /= len;
        qz /= len;
        qw /= len;
    }

    rotationMatrix[0] = 1.0f - 2.0f * (qy * qy + qz * qz);
    rotationMatrix[1] = 2.0f * (qx * qy + qw * qz);
    rotationMatrix[2] = 2.0f * (qx * qz - qw * qy);

    rotationMatrix[3] = 2.0f * (qx * qy - qw * qz);
    rotationMatrix[4] = 1.0f - 2.0f * (qx * qx + qz * qz);
    rotationMatrix[5] = 2.0f * (qy * qz + qw * qx);

    rotationMatrix[6] = 2.0f * (qx * qz + qw * qy);
    rotationMatrix[7] = 2.0f * (qy * qz - qw * qx);
    rotationMatrix[8] = 1.0f - 2.0f * (qx * qx + qy * qy);

    return rotationMatrix;
}

// a * b (column major order)
void PerspectiveTransformCalculator::MatrixMultiply(
    std::array<float, 16> &result, const std::array<float, 16> &a, const std::array<float, 16> &b)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result[i * 4 + j] += a[k * 4 + j] * b[i * 4 + k];
            }
        }
    }
}

} // Rosen
} // OHOS