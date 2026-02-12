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

#include <gtest/gtest.h>
#include "ge_sdf_empty_shader_shape.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GESDFEmptyShaderShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GESDFEmptyShaderShapeTest::SetUpTestCase(void) {}
void GESDFEmptyShaderShapeTest::TearDownTestCase(void) {}

void GESDFEmptyShaderShapeTest::SetUp() {}
void GESDFEmptyShaderShapeTest::TearDown() {}

/**
 * @tc.name: GetSDFShapeType
 * @tc.desc: Verify GetSDFShapeType of SDFEmptyShaderShape
 * @tc.type: FUNC
 */
HWTEST_F(GESDFEmptyShaderShapeTest, GetSDFShapeType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GESDFEmptyShaderShapeTest GetSDFShapeType start";
    GESDFEmptyShaderShape shape;
    EXPECT_EQ(shape.GetSDFShapeType(), GESDFShapeType::EMPTY);
    GTEST_LOG_(INFO) << "GESDFEmptyShaderShapeTest GetSDFShapeType end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS