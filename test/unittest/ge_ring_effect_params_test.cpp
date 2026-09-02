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

#include "ge_effects_params.h"
#include "ge_ripple_shader_mask.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GERingEffectParamsTest : public testing::Test {};

/**
 * @tc.name: SpinBlurReflectionStoresAllNewArguments
 * @tc.desc: Verify SpinBlur reflection stores center, angle, samples and optional mask
 * @tc.type: FUNC
 */
HWTEST_F(GERingEffectParamsTest, SpinBlurReflectionStoresAllNewArguments, TestSize.Level1)
{
    GEVisualEffectImpl spinBlur(GE_FILTER_SPIN_BLUR);
    const Vector2f center(0.25f, 0.75f);
    constexpr float angle = 45.0f;
    constexpr int32_t samples = 16;

    spinBlur.SetParam(GE_FILTER_SPIN_BLUR_CENTER, center);
    spinBlur.SetParam(GE_FILTER_SPIN_BLUR_ANGLE, angle);
    spinBlur.SetParam(GE_FILTER_SPIN_BLUR_SAMPLES, samples);

    auto params = spinBlur.GetParams<GESpinBlurShaderFilterParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(spinBlur.GetFilterType(), GEFilterType::SPIN_BLUR);
    EXPECT_EQ(params->center, center);
    EXPECT_FLOAT_EQ(params->angle, angle);
    EXPECT_EQ(params->samples, samples);
}

/**
 * @tc.name: HaloBloomReflectionStoresAllNewArguments
 * @tc.desc: Verify HaloBloom reflection stores tint color, bloom factor, glow exposure and optional mask
 * @tc.type: FUNC
 */
HWTEST_F(GERingEffectParamsTest, HaloBloomReflectionStoresAllNewArguments, TestSize.Level1)
{
    GEVisualEffectImpl haloBloom(GE_FILTER_HALO_BLOOM);
    const Vector4f tintColor(0.1f, 0.2f, 0.3f, 0.4f);
    constexpr float bloomFactor = 0.65f;
    constexpr float glowExposure = 1.25f;

    haloBloom.SetParam(GE_FILTER_HALO_BLOOM_TINT_COLOR, tintColor);
    haloBloom.SetParam(GE_FILTER_HALO_BLOOM_BLOOM_FACTOR, bloomFactor);
    haloBloom.SetParam(GE_FILTER_HALO_BLOOM_GLOW_EXPOSURE, glowExposure);

    auto params = haloBloom.GetParams<GEHaloBloomShaderFilterParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(haloBloom.GetFilterType(), GEFilterType::HALO_BLOOM);
    EXPECT_EQ(params->tintColor, tintColor);
    EXPECT_FLOAT_EQ(params->bloomFactor, bloomFactor);
    EXPECT_FLOAT_EQ(params->glowExposure, glowExposure);
}

/**
 * @tc.name: WarpedRingReflectionStoresProgressAndRingParameters
 * @tc.desc: Verify WarpedRingMask reflection stores progress and every ring parameter
 * @tc.type: FUNC
 */
HWTEST_F(GERingEffectParamsTest, WarpedRingReflectionStoresProgressAndRingParameters, TestSize.Level1)
{
    GEVisualEffectImpl warpedRing(GE_MASK_WARPED_RING);
    constexpr float radius = 0.8f;
    constexpr float baseHalfWidth = 0.12f;
    constexpr float widthVariation = 0.05f;
    constexpr float rotate3DProgress = 0.6f;

    warpedRing.SetParam(GE_MASK_WARPED_RING_RADIUS, radius);
    warpedRing.SetParam(GE_MASK_WARPED_RING_BASE_HALF_WIDTH, baseHalfWidth);
    warpedRing.SetParam(GE_MASK_WARPED_RING_WIDTH_VARIATION, widthVariation);
    warpedRing.SetParam(GE_MASK_WARPED_RING_ROTATE_3D_PROGRESS, rotate3DProgress);

    auto params = warpedRing.GetParams<GEWarpedRingShaderMaskParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(warpedRing.GetFilterType(), GEFilterType::WARPED_RING);
    EXPECT_FLOAT_EQ(params->radius_, radius);
    EXPECT_FLOAT_EQ(params->baseHalfWidth_, baseHalfWidth);
    EXPECT_FLOAT_EQ(params->widthVariation_, widthVariation);
    EXPECT_FLOAT_EQ(params->rotate3DProgress_, rotate3DProgress);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS