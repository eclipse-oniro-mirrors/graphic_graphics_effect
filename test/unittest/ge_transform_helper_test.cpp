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

#include <gtest/gtest.h>
#include "util/ge_transform_helper.h"
#include <cmath>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GETransformHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GETransformHelperTest::SetUpTestCase(void) {}
void GETransformHelperTest::TearDownTestCase(void) {}
void GETransformHelperTest::SetUp() {}
void GETransformHelperTest::TearDown() {}

/**
 * @tc.name: QuaternionToRotationMatrix_NaN
 * @tc.desc: Verify QuaternionToRotationMatrix returns identity matrix when quaternion is NaN
 * @tc.type: FUNC
 */
HWTEST_F(GETransformHelperTest, QuaternionToRotationMatrix_NaN, TestSize.Level0)
{
    Quaternion q;
    q.x_ = NAN;
    q.y_ = 0.0f;
    q.z_ = 0.0f;
    q.w_ = 1.0f;
    auto result = PerspectiveTransformCalculator::QuaternionToRotationMatrix(q);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[4], 1.0f);
    EXPECT_FLOAT_EQ(result[8], 1.0f);
}

/**
 * @tc.name: QuaternionToRotationMatrix_Infinite
 * @tc.desc: Verify QuaternionToRotationMatrix returns identity matrix when quaternion is infinite
 * @tc.type: FUNC
 */
HWTEST_F(GETransformHelperTest, QuaternionToRotationMatrix_Infinite, TestSize.Level0)
{
    Quaternion q;
    q.x_ = 0.0f;
    q.y_ = INFINITY;
    q.z_ = 0.0f;
    q.w_ = 1.0f;
    auto result = PerspectiveTransformCalculator::QuaternionToRotationMatrix(q);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[4], 1.0f);
    EXPECT_FLOAT_EQ(result[8], 1.0f);
}

/**
 * @tc.name: QuaternionToRotationMatrix_Identity
 * @tc.desc: Verify QuaternionToRotationMatrix returns identity matrix for identity quaternion
 * @tc.type: FUNC
 */
HWTEST_F(GETransformHelperTest, QuaternionToRotationMatrix_Identity, TestSize.Level0)
{
    Quaternion q;
    q.x_ = 0.0f;
    q.y_ = 0.0f;
    q.z_ = 0.0f;
    q.w_ = 1.0f;
    auto result = PerspectiveTransformCalculator::QuaternionToRotationMatrix(q);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[4], 1.0f);
    EXPECT_FLOAT_EQ(result[8], 1.0f);
}

/**
 * @tc.name: BuildProjectionMatrixRhZo_Invalid
 * @tc.desc: Verify BuildProjectionMatrixRhZo returns identity matrix when aspectRatio is zero
 * @tc.type: FUNC
 */
HWTEST_F(GETransformHelperTest, BuildProjectionMatrixRhZo_Invalid, TestSize.Level0)
{
    Drawing::GECameraIntrinsics intrinsics;
    intrinsics.fov = 60.0f;
    intrinsics.aspectRatio = 0.0f;
    intrinsics.near = 0.1f;
    intrinsics.far = 100.0f;
    auto result = PerspectiveTransformCalculator::BuildProjectionMatrixRhZo(intrinsics);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[5], 1.0f);
    EXPECT_FLOAT_EQ(result[10], 1.0f);
    EXPECT_FLOAT_EQ(result[15], 1.0f);
}

/**
 * @tc.name: ComputeVPMatrix_Valid
 * @tc.desc: Verify ComputeVPMatrix produces non-zero result with valid camera parameters
 * @tc.type: FUNC
 */
HWTEST_F(GETransformHelperTest, ComputeVPMatrix_Valid, TestSize.Level0)
{
    Drawing::GECameraIntrinsics intrinsics;
    intrinsics.fov = 60.0f;
    intrinsics.aspectRatio = 1.0f;
    intrinsics.near = 0.1f;
    intrinsics.far = 100.0f;
    Drawing::GECameraExtrinsics extrinsics;
    extrinsics.rotation = Quaternion();
    extrinsics.position[0] = 0.0f;
    extrinsics.position[1] = 0.0f;
    extrinsics.position[2] = 5.0f;
    std::array<Vector3f, 4> corners = {
        Vector3f(-1, -1, 0), Vector3f(1, -1, 0),
        Vector3f(1, 1, 0), Vector3f(-1, 1, 0)
    };
    auto result = PerspectiveTransformCalculator::ComputeVPMatrix(intrinsics, extrinsics, corners);
    EXPECT_NE(result[0], 0.0f);
}

} // namespace Rosen
} // namespace OHOS