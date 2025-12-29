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
#include "ge_shader.h"
#include "ge_log.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "utils/rect.h"
#include "draw/color.h"
#include "draw/path.h"
#include "ge_shader_filter_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace Drawing;

namespace OHOS {
namespace Rosen {
class GEShaderEffectTest : public Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {}
    void TearDown() override
    {}

    static inline Canvas canvas_;
    static inline Rect rect_{0.0, 0.0, 100.0, 100.0};
};

using CacheDataType = std::shared_ptr<Image>;
class GETestShader : public GEShader {
public:
    void MakeDrawingShader(const Rect& rect, float progress) override
    {
        static constexpr char prog[] = R"(
            vec4 main(float2 fragCoord)
            {
                return vec4(0.0, 0.0, 0.0, 1.0);
            }
        )";
        auto testShaderEffect = RuntimeEffect::CreateForShader(prog);
        auto build = std::make_shared<RuntimeShaderBuilder>(testShaderEffect);
        drShader_ = build->MakeShader(nullptr, false);
    }

protected:
    void Preprocess(Canvas& canvas, const Rect& rect) override
    {
        if (cacheAnyPtr_ == nullptr) {
            CacheDataType cacheData = nullptr;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
    }
};

class GETestSubstractedShader : public GEShader {
public:
    explicit GETestSubstractedShader(const Rect& rect)
    {
        testSubtractedShader = rect;
    }

    void MakeDrawingShader(const Rect& rect, float progress) override
    {
        static constexpr char prog[] = R"(
            vec4 main(float2 fragCoord)
            {
                return vec4(0.0, 0.0, 0.0, 1.0);
            }
        )";
        auto testShaderEffect = RuntimeEffect::CreateForShader(prog);
        auto build = std::make_shared<RuntimeShaderBuilder>(testShaderEffect);
        drShader_ = build->MakeShader(nullptr, false);
    }

    Drawing::Rect GetSubtractedRect(float width, float height) const override { return testSubtractedShader; }

protected:
    Drawing::Rect testSubtractedShader;
    void Preprocess(Canvas& canvas, const Rect& rect) override
    {
        if (cacheAnyPtr_ == nullptr) {
            CacheDataType cacheData = nullptr;
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheData));
        }
    }
};

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_01, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestShader>();
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_NE(testShader->GetDrawingShader(), nullptr);
    std::shared_ptr<std::any> cachPtr = nullptr;
    testShader->SetCache(cachPtr);
    EXPECT_EQ(testShader->GetCache(), nullptr);
    testShader->DrawShader(canvas_, rect_);
    EXPECT_NE(testShader->GetCache(), nullptr);
    EXPECT_FALSE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_Contain, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(0.25, 0.25, 0.75, 0.75));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_TRUE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_NotContain, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(0.25, 0.25, 1.2, 1.2));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_FALSE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}

HWTEST_F(GEShaderEffectTest, GEShaderEffectTest_TryDrawShaderWithPen_RenderRectInsideSubRect, TestSize.Level1)
{
    auto testShader = std::make_shared<GETestSubstractedShader>(Drawing::Rect(-0.1, -0.1, 1.2, 1.2));
    uint32_t hash = 0;
    EXPECT_EQ(testShader->Hash(), hash);
    testShader->MakeDrawingShader(rect_, -1.f);
    EXPECT_TRUE(testShader->TryDrawShaderWithPen(canvas_, rect_));
}
}
}