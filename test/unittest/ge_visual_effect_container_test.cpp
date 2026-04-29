/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_visual_effect_container.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectContainerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectContainerTest::SetUpTestCase(void) {}
void GEVisualEffectContainerTest::TearDownTestCase(void) {}

void GEVisualEffectContainerTest::SetUp() {}
void GEVisualEffectContainerTest::TearDown() {}

/**
 * @tc.name: AddToChainedFilter_001
 * @tc.desc: Verify the AddToChainedFilter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(visualEffect);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_001 end";
}

/**
 * @tc.name: AddToChainedFilter_002
 * @tc.desc: Verify the AddToChainedFilter
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_002 start";

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_002 end";
}

/**
 * @tc.name: UpdateDarkScale_001
 * @tc.desc: Verify UpdateDarkScale sets darkScale for FrostedGlass
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateDarkScale_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_001 start";

    auto frosted = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    auto other = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(frosted);
    container->AddToChainedFilter(other);

    const float darkScale = 0.7f;
    container->UpdateDarkScale(darkScale);

    auto impl = frosted->GetImpl();
    ASSERT_NE(impl, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_001 end";
}

/**
 * @tc.name: GetFilters_001
 * @tc.desc: Verify function GetFilters
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, GetFilters_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetFilters_001 start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);

    auto container = std::make_shared<GEVisualEffectContainer>();
    EXPECT_EQ(container->GetFilters().size(), 0);

    container->AddToChainedFilter(visualEffect1);
    EXPECT_EQ(container->GetFilters().size(), 1);
    EXPECT_EQ(container->GetFilters()[0]->GetName(), GE_FILTER_KAWASE_BLUR);

    container->AddToChainedFilter(visualEffect2);
    EXPECT_EQ(container->GetFilters().size(), 2);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetFilters_001 end";
}

/**
 * @tc.name: SetGeometry_001
 * @tc.desc: Verify function SetGeometry
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetGeometry_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);
    Drawing::RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::RectF materialDst(0.0f, 0.0f, 100.0f, 100.0f);
    float geoWidth = 100.0f;
    float geoHeight = 100.0f;

    container->SetGeometry(matrix, bound, materialDst, geoWidth, geoHeight);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_001 end";
}

/**
 * @tc.name: SetDisplayHeadroom_001
 * @tc.desc: Verify function SetDisplayHeadroom
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetDisplayHeadroom_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float headroom = 1.5f;
    container->SetDisplayHeadroom(headroom);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_001 end";
}

/**
 * @tc.name: RemoveFilterWithType_001
 * @tc.desc: Verify function RemoveFilterWithType
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, RemoveFilterWithType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_001 start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);
    auto visualEffect3 = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect1);
    container->AddToChainedFilter(visualEffect2);
    container->AddToChainedFilter(visualEffect3);
    EXPECT_EQ(container->GetFilters().size(), 3);

    container->RemoveFilterWithType(static_cast<int32_t>(GEVisualEffectImpl::FilterType::KAWASE_BLUR));
    EXPECT_EQ(container->GetFilters().size(), 2);

    container->RemoveFilterWithType(static_cast<int32_t>(GEVisualEffectImpl::FilterType::GREY));
    EXPECT_EQ(container->GetFilters().size(), 1);

    container->RemoveFilterWithType(static_cast<int32_t>(GEVisualEffectImpl::FilterType::MESA_BLUR));
    EXPECT_EQ(container->GetFilters().size(), 0);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_001 end";
}

/**
 * @tc.name: UpdateCachedBlurImage_001
 * @tc.desc: Verify function UpdateCachedBlurImage
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCachedBlurImage_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCachedBlurImage_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    container->UpdateCachedBlurImage(nullptr, nullptr, 0.0f, 0.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCachedBlurImage_001 end";
}

/**
 * @tc.name: UpdateCornerRadius_001
 * @tc.desc: Verify function UpdateCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCornerRadius_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float cornerRadius = 10.0f;
    container->UpdateCornerRadius(cornerRadius);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_001 end";
}

/**
 * @tc.name: UpdateTotalMatrix_001
 * @tc.desc: Verify function UpdateTotalMatrix
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateTotalMatrix_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateTotalMatrix_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    container->UpdateTotalMatrix(totalMatrix);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateTotalMatrix_001 end";
}

/**
 * @tc.name: UpdateSnapshotRect_001
 * @tc.desc: Verify function UpdateSnapshotRect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateSnapshotRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::RectF snapshot(0.0f, 0.0f, 100.0f, 100.0f);
    container->UpdateSnapshotRect(snapshot);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_001 end";
}

/**
 * @tc.name: GetGEVisualEffect_001
 * @tc.desc: Verify function GetGEVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, GetGEVisualEffect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_001 start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect1);
    container->AddToChainedFilter(visualEffect2);

    auto result1 = container->GetGEVisualEffect(GE_FILTER_KAWASE_BLUR);
    EXPECT_NE(result1, nullptr);
    EXPECT_EQ(result1->GetName(), GE_FILTER_KAWASE_BLUR);

    auto result2 = container->GetGEVisualEffect(GE_FILTER_GREY);
    EXPECT_NE(result2, nullptr);
    EXPECT_EQ(result2->GetName(), GE_FILTER_GREY);

    auto result3 = container->GetGEVisualEffect("nonexistent_effect");
    EXPECT_EQ(result3, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_001 end";
}

/**
 * @tc.name: UpdateCacheDataFrom_001
 * @tc.desc: Verify function UpdateCacheDataFrom
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCacheDataFrom_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_001 start";

    auto container1 = std::make_shared<GEVisualEffectContainer>();
    auto container2 = std::make_shared<GEVisualEffectContainer>();

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    container2->AddToChainedFilter(visualEffect);

    container1->UpdateCacheDataFrom(container2);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_001 end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
