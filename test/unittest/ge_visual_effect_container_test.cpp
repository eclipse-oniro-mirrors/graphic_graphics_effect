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
 * @tc.name: AddToChainedFilter_ValidVisualEffect
 * @tc.desc: Verify the AddToChainedFilter adds valid visual effect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_ValidVisualEffect, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_ValidVisualEffect start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    visualEffect->SetParam(GE_FILTER_KAWASE_BLUR_RADIUS, 1);

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(visualEffect);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_ValidVisualEffect end";
}

/**
 * @tc.name: AddToChainedFilter_NullVisualEffect
 * @tc.desc: Verify the AddToChainedFilter handles null visual effect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_NullVisualEffect, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_NullVisualEffect start";

    auto visualEffectContainer = std::make_shared<GEVisualEffectContainer>();
    visualEffectContainer->AddToChainedFilter(nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_NullVisualEffect end";
}

/**
 * @tc.name: UpdateDarkScale_FrostedGlassEffect
 * @tc.desc: Verify UpdateDarkScale sets darkScale for FrostedGlass effect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateDarkScale_FrostedGlassEffect, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_FrostedGlassEffect start";

    auto frosted = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    auto other = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(frosted);
    container->AddToChainedFilter(other);

    const float darkScale = 0.7f;
    container->UpdateDarkScale(darkScale);

    auto impl = frosted->GetImpl();
    ASSERT_NE(impl, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateDarkScale_FrostedGlassEffect end";
}

/**
 * @tc.name: GetFilters_QueryFilterList
 * @tc.desc: Verify function GetFilters returns correct filter list size and order
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, GetFilters_QueryFilterList, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetFilters_QueryFilterList start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);

    auto container = std::make_shared<GEVisualEffectContainer>();
    EXPECT_EQ(container->GetFilters().size(), 0);

    container->AddToChainedFilter(visualEffect1);
    ASSERT_EQ(container->GetFilters().size(), 1);
    EXPECT_EQ(container->GetFilters()[0]->GetName(), GE_FILTER_KAWASE_BLUR);

    container->AddToChainedFilter(visualEffect2);
    ASSERT_EQ(container->GetFilters().size(), 2);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetFilters_QueryFilterList end";
}

/**
 * @tc.name: SetGeometry_ValidMatrixAndRects
 * @tc.desc: Verify function SetGeometry with valid matrix and rectangles
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetGeometry_ValidMatrixAndRects, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_ValidMatrixAndRects start";

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

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_ValidMatrixAndRects end";
}

/**
 * @tc.name: SetDisplayHeadroom_PositiveValue
 * @tc.desc: Verify function SetDisplayHeadroom with positive headroom value
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetDisplayHeadroom_PositiveValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_PositiveValue start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float headroom = 1.5f;
    container->SetDisplayHeadroom(headroom);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_PositiveValue end";
}

/**
 * @tc.name: RemoveFilterWithType_RemoveExistingFilters
 * @tc.desc: Verify function RemoveFilterWithType removes filters correctly
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, RemoveFilterWithType_RemoveExistingFilters, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_RemoveExistingFilters start";

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

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_RemoveExistingFilters end";
}

/**
 * @tc.name: UpdateCachedBlurImage_NullCanvasAndImage
 * @tc.desc: Verify function UpdateCachedBlurImage handles null canvas and image
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCachedBlurImage_NullCanvasAndImage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCachedBlurImage_NullCanvasAndImage start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    container->UpdateCachedBlurImage(nullptr, nullptr, 0.0f, 0.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCachedBlurImage_NullCanvasAndImage end";
}

/**
 * @tc.name: UpdateCornerRadius_PositiveRadius
 * @tc.desc: Verify function UpdateCornerRadius with positive corner radius
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCornerRadius_PositiveRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_PositiveRadius start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float cornerRadius = 10.0f;
    container->UpdateCornerRadius(cornerRadius);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_PositiveRadius end";
}

/**
 * @tc.name: UpdateTotalMatrix_IdentityMatrix
 * @tc.desc: Verify function UpdateTotalMatrix with identity matrix
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateTotalMatrix_IdentityMatrix, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateTotalMatrix_IdentityMatrix start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    container->UpdateTotalMatrix(totalMatrix);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateTotalMatrix_IdentityMatrix end";
}

/**
 * @tc.name: UpdateSnapshotRect_ValidRect
 * @tc.desc: Verify function UpdateSnapshotRect with valid rect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateSnapshotRect_ValidRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_ValidRect start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::RectF snapshot(0.0f, 0.0f, 100.0f, 100.0f);
    container->UpdateSnapshotRect(snapshot);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_ValidRect end";
}

/**
 * @tc.name: GetGEVisualEffect_FindExistingAndNonExisting
 * @tc.desc: Verify function GetGEVisualEffect finds existing filters and returns null for non-existing
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, GetGEVisualEffect_FindExistingAndNonExisting, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_FindExistingAndNonExisting start";

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

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_FindExistingAndNonExisting end";
}

/**
 * @tc.name: UpdateCacheDataFrom_SourceContainerWithFilters
 * @tc.desc: Verify function UpdateCacheDataFrom copies data from source container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCacheDataFrom_SourceContainerWithFilters, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_SourceContainerWithFilters start";

    auto container1 = std::make_shared<GEVisualEffectContainer>();
    auto container2 = std::make_shared<GEVisualEffectContainer>();

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    container2->AddToChainedFilter(visualEffect);

    container1->UpdateCacheDataFrom(container2);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_SourceContainerWithFilters end";
}

/**
 * @tc.name: UpdateFrostedGlassEffectParams_NullBlurImage
 * @tc.desc: Verify function UpdateFrostedGlassEffectParams handles null blur image
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateFrostedGlassEffectParams_NullBlurImage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateFrostedGlassEffectParams_NullBlurImage start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_FROSTED_GLASS);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    container->UpdateFrostedGlassEffectParams(nullptr, 0.5f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateFrostedGlassEffectParams_NullBlurImage end";
}

/**
 * @tc.name: UpdateFrostedGlassEffectParams_EmptyContainer
 * @tc.desc: Verify function UpdateFrostedGlassEffectParams handles empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateFrostedGlassEffectParams_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateFrostedGlassEffectParams_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();

    container->UpdateFrostedGlassEffectParams(nullptr, 0.5f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateFrostedGlassEffectParams_EmptyContainer end";
}

/**
 * @tc.name: SetGeometry_EmptyContainer
 * @tc.desc: Verify function SetGeometry handles empty container gracefully
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetGeometry_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();

    Drawing::Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);
    Drawing::RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::RectF materialDst(0.0f, 0.0f, 100.0f, 100.0f);
    float geoWidth = 100.0f;
    float geoHeight = 100.0f;

    container->SetGeometry(matrix, bound, materialDst, geoWidth, geoHeight);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_EmptyContainer end";
}

/**
 * @tc.name: SetGeometry_ZeroSizeRects
 * @tc.desc: Verify function SetGeometry handles zero size rectangles
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetGeometry_ZeroSizeRects, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_ZeroSizeRects start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::Matrix matrix;
    Drawing::RectF bound(0.0f, 0.0f, 0.0f, 0.0f);
    Drawing::RectF materialDst(0.0f, 0.0f, 0.0f, 0.0f);
    float geoWidth = 0.0f;
    float geoHeight = 0.0f;

    container->SetGeometry(matrix, bound, materialDst, geoWidth, geoHeight);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_ZeroSizeRects end";
}

/**
 * @tc.name: SetGeometry_NegativeCoords
 * @tc.desc: Verify function SetGeometry handles negative coordinates
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetGeometry_NegativeCoords, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_NegativeCoords start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_LINEAR_GRADIENT_BLUR);
    visualEffect->SetParam(GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::Matrix matrix;
    Drawing::RectF bound(-10.0f, -10.0f, 100.0f, 100.0f);
    Drawing::RectF materialDst(-5.0f, -5.0f, 50.0f, 50.0f);
    float geoWidth = 100.0f;
    float geoHeight = 100.0f;

    container->SetGeometry(matrix, bound, materialDst, geoWidth, geoHeight);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetGeometry_NegativeCoords end";
}

/**
 * @tc.name: SetDisplayHeadroom_ZeroValue
 * @tc.desc: Verify function SetDisplayHeadroom handles zero headroom
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetDisplayHeadroom_ZeroValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_ZeroValue start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float headroom = 0.0f;
    container->SetDisplayHeadroom(headroom);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_ZeroValue end";
}

/**
 * @tc.name: SetDisplayHeadroom_EmptyContainer
 * @tc.desc: Verify function SetDisplayHeadroom handles empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, SetDisplayHeadroom_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->SetDisplayHeadroom(1.5f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest SetDisplayHeadroom_EmptyContainer end";
}

/**
 * @tc.name: RemoveFilterWithType_NonExistingType
 * @tc.desc: Verify function RemoveFilterWithType handles non-existing filter type
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, RemoveFilterWithType_NonExistingType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_NonExistingType start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect1);
    container->AddToChainedFilter(visualEffect2);
    EXPECT_EQ(container->GetFilters().size(), 2);

    container->RemoveFilterWithType(static_cast<int32_t>(GEVisualEffectImpl::FilterType::MESA_BLUR));
    EXPECT_EQ(container->GetFilters().size(), 2);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_NonExistingType end";
}

/**
 * @tc.name: RemoveFilterWithType_EmptyContainer
 * @tc.desc: Verify function RemoveFilterWithType handles empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, RemoveFilterWithType_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();
    EXPECT_EQ(container->GetFilters().size(), 0);

    container->RemoveFilterWithType(static_cast<int32_t>(GEVisualEffectImpl::FilterType::KAWASE_BLUR));
    EXPECT_EQ(container->GetFilters().size(), 0);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest RemoveFilterWithType_EmptyContainer end";
}

/**
 * @tc.name: UpdateCornerRadius_ZeroRadius
 * @tc.desc: Verify function UpdateCornerRadius handles zero radius
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCornerRadius_ZeroRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_ZeroRadius start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    float cornerRadius = 0.0f;
    container->UpdateCornerRadius(cornerRadius);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_ZeroRadius end";
}

/**
 * @tc.name: UpdateCornerRadius_EmptyContainer
 * @tc.desc: Verify function UpdateCornerRadius handles empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCornerRadius_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->UpdateCornerRadius(10.0f);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCornerRadius_EmptyContainer end";
}

/**
 * @tc.name: UpdateSnapshotRect_ZeroSizeRect
 * @tc.desc: Verify function UpdateSnapshotRect handles zero size rect
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateSnapshotRect_ZeroSizeRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_ZeroSizeRect start";

    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);
    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect);

    Drawing::RectF snapshot(0.0f, 0.0f, 0.0f, 0.0f);
    container->UpdateSnapshotRect(snapshot);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_ZeroSizeRect end";
}

/**
 * @tc.name: UpdateSnapshotRect_EmptyContainer
 * @tc.desc: Verify function UpdateSnapshotRect handles empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateSnapshotRect_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();
    Drawing::RectF snapshot(0.0f, 0.0f, 100.0f, 100.0f);
    container->UpdateSnapshotRect(snapshot);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateSnapshotRect_EmptyContainer end";
}

/**
 * @tc.name: GetGEVisualEffect_EmptyContainer
 * @tc.desc: Verify function GetGEVisualEffect returns null for empty container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, GetGEVisualEffect_EmptyContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_EmptyContainer start";

    auto container = std::make_shared<GEVisualEffectContainer>();

    auto result = container->GetGEVisualEffect(GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(result, nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest GetGEVisualEffect_EmptyContainer end";
}

/**
 * @tc.name: UpdateCacheDataFrom_NullSource
 * @tc.desc: Verify function UpdateCacheDataFrom handles null source container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCacheDataFrom_NullSource, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_NullSource start";

    auto container1 = std::make_shared<GEVisualEffectContainer>();
    container1->UpdateCacheDataFrom(nullptr);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_NullSource end";
}

/**
 * @tc.name: UpdateCacheDataFrom_EmptySourceContainer
 * @tc.desc: Verify function UpdateCacheDataFrom handles empty source container
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, UpdateCacheDataFrom_EmptySourceContainer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_EmptySourceContainer start";

    auto container1 = std::make_shared<GEVisualEffectContainer>();
    auto visualEffect = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    container1->AddToChainedFilter(visualEffect);

    auto container2 = std::make_shared<GEVisualEffectContainer>();
    container1->UpdateCacheDataFrom(container2);
    EXPECT_EQ(container1->GetFilters().size(), 1);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest UpdateCacheDataFrom_EmptySourceContainer end";
}

/**
 * @tc.name: AddToChainedFilter_MultipleFiltersPreserveOrder
 * @tc.desc: Verify AddToChainedFilter preserves filter order when adding multiple filters
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectContainerTest, AddToChainedFilter_MultipleFiltersPreserveOrder, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_MultipleFiltersPreserveOrder start";

    auto visualEffect1 = std::make_shared<GEVisualEffect>(GE_FILTER_KAWASE_BLUR);
    auto visualEffect2 = std::make_shared<GEVisualEffect>(GE_FILTER_GREY);
    auto visualEffect3 = std::make_shared<GEVisualEffect>(GE_FILTER_MESA_BLUR);

    auto container = std::make_shared<GEVisualEffectContainer>();
    container->AddToChainedFilter(visualEffect1);
    container->AddToChainedFilter(visualEffect2);
    container->AddToChainedFilter(visualEffect3);

    auto filters = container->GetFilters();
    EXPECT_EQ(filters.size(), 3);
    EXPECT_EQ(filters[0]->GetName(), GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(filters[1]->GetName(), GE_FILTER_GREY);
    EXPECT_EQ(filters[2]->GetName(), GE_FILTER_MESA_BLUR);

    GTEST_LOG_(INFO) << "GEVisualEffectContainerTest AddToChainedFilter_MultipleFiltersPreserveOrder end";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
