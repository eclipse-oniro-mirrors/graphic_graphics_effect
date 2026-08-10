# Testing Guide

How to write tests for GE effects. For running/building tests, see AGENTS.md → Build System.

---

## Task Map

| Task | Section |
|---------|---------|
| Write a unit test | [Unit Test Structure](#unit-test-structure) |
| Set up GPU-backed canvas | [GPU Context Setup](#gpu-context-setup) |
| Name a test | [Test Naming](#test-naming) |
| Write a fuzzer | [Fuzz Testing](#fuzz-testing) |
| Access private members | [Accessing Private Members](#accessing-private-members) |
| Check if test is effective | [Test Effectiveness](#test-effectiveness) |

---

## Unit Test Structure

Tests use the **googletest** framework (`HWTEST_F` — `TEST_F` extended with a `test_flags` parameter like `TestSize.Level1` for runtime filtering). Fixture class per effect:

```cpp
class GEMyEffectTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    Drawing::Canvas canvasNoGpu_; // CPU canvas — for param/null-input tests
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEMyEffectTest::SetUp()
{
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}
```

**Test case pattern**:

```cpp
HWTEST_F(GEMyEffectTest, OnProcessImageNullImage, TestSize.Level1)
{
    auto filter = std::make_unique<GEMyFilter>(params);
    EXPECT_EQ(filter->OnProcessImage(canvasNoGpu_, nullptr, src_, dst_), nullptr);
}
```

For tests that need a GPU-backed canvas, see [GPU Context Setup](#gpu-context-setup).

---

## GPU Context Setup

`OnProcessImage`/`MakeDrawingShader` exercise `MakeImage`, which returns `nullptr` without a GPU context (see [Drawing API Contracts](drawing_api_contracts.md#runtimeshaderbuildermakeimage)). Tests that only check param getters or null-input handling can use a CPU canvas; tests that expect a non-null image/shader result must set up a GPU-backed surface.

```cpp
#include "render_context/render_context.h"

// Use shared_ptr<Surface>/<Canvas> (heap) — GPU resources own canvas lifecycle.
// Avoid static inline Canvas: it is global mutable state shared across tests (CP.3).

class GEMyEffectTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
private:
    std::shared_ptr<Drawing::Surface> CreateSurface();

    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr; // GPU canvas
    Drawing::Canvas canvasNoGpu_; // for CPU-only tests
    std::shared_ptr<Drawing::Image> image_ { nullptr };
    Drawing::ImageInfo imageInfo_ = {};
};

std::shared_ptr<Drawing::Surface> GEMyEffectTest::CreateSurface()
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto context = renderContext->GetSharedDrGPUContext();
    if (context == nullptr) {
        GTEST_LOG_(INFO) << "CreateSurface: gpuContext failed.";
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
}

void GEMyEffectTest::SetUp()
{
    surface_ = CreateSurface();
    if (surface_) {
        canvas_ = surface_->GetCanvas();
    }
    ASSERT_NE(canvas_, nullptr);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}
```

**Key points**:
- `RenderContext::Create()` → `Init()` → `SetUpGpuContext()` — skipping any step yields a null `GPUContext`
- `GetSharedDrGPUContext()` may return `nullptr` in CPU-only CI — guard with `ASSERT_NE`

---

## Test Naming

- **Descriptive names**, not numbered suffixes: `OnProcessImageNullImage`, not `TestProcessImage001`
- Name should describe the **contract being verified**, not the implementation detail
- `TestSize.Level0`–`Level4` — runtime filter flags (see `gtest-tag.h`); lower levels are must-run core tests, higher levels are supplementary — `Level1` is the default in this codebase, `Level4` is unused

---

## Fuzz Testing

Fuzzers live in `test/fuzztest/<name>/`, each with its own `BUILD.gn` target (`ohos_fuzztest`), `project.xml` (resource limits), and `corpus/` (seed files).

Entry point is `LLVMFuzzerTestOneInput` (libfuzzer). First byte selects a test case; remaining bytes feed `FuzzedDataProvider`:

```cpp
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) { return 0; }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>() % TEST_CASE_COUNT;
    switch (choice) {
        case TEST_CASE_MY_EFFECT: MyEffectFuzz(fdp); break;
        // ...
    }
    return 0;
}

void MyEffectFuzz(FuzzedDataProvider& fdp)
{
    auto radius = fdp.ConsumeFloatingPoint<float>();
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(GE_FILTER_MY_EFFECT);
    visualEffect->SetParam("MY_EFFECT_RADIUS", radius);
    // ... call GERender or filter directly
}
```

---

## Accessing Private Members

Tests compile with `-Dprivate=public -Dprotected=public` (see AGENTS.md → Test Organization), which rewrites access specifiers so tests can reach private members — don't add `friend` declarations or public accessors just for tests. **Prefer testing through the public API** — only access internals when the public API doesn't expose enough to assert the behavior you need.

---

## Test Effectiveness

**Verify the test reaches the target branch.** A test that "passes" by exercising an early-return is worse than no test — false confidence.

Before writing a test:
1. Trace the execution path mentally from the entry point
2. Confirm the target line is reachable given the inputs you plan to use
3. If an upstream guard returns before your target line, the test is dead — choose different inputs or test a different contract

```cpp
// 🚫 dead test — passes, but only exercises the null-image guard, not the shader logic
HWTEST_F(GEMyEffectTest, OnProcessImageNoGpu, TestSize.Level1)
{
    auto filter = std::make_unique<GEMyFilter>(params);
    // canvasNoGpu_ has no GPU context → MakeImage returns nullptr → early return
    EXPECT_EQ(filter->OnProcessImage(canvasNoGpu_, image_, src_, dst_), image_);
}
```

**What to test at minimum**:
1. **Constructor with valid params** — effect object creation succeeds
2. **SetParam round-trip** — set a value, verify it's stored (with clamping if applicable)
3. **Key method behavior** — `OnProcessImage`/`MakeDrawingShader`/`GenerateDrawingShader` returns non-null with valid GPU input, handles null input gracefully
