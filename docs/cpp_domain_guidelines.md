# C++ Domain-Specific Guidelines

GE-specific patterns for effect implementation. Load only the section matching your task.

For general C++ conventions, see [cpp_guidelines.md](cpp_guidelines.md). For upstream Drawing API behavior, see [drawing_api_contracts.md](drawing_api_contracts.md).

---

## Task Map

| Task | Section |
|---------|---------|
| Write error log messages | [Log Accuracy](#log-accuracy) |
| Propagate color space to offscreen | [Color Space Propagation](#color-space-propagation) |
| Invalidate cache on failure | [Stale Cache on Failure](#stale-cache-on-failure) |
| Range-check deserialized enums | [IPC Enum Validation](#ipc-enum-validation) |

---

## Log Accuracy

**`LOGE` must name the actual function.** Copy-pasted function names make multi-pass failures un-diagnosable.

```cpp
std::shared_ptr<Drawing::Image> GEMyFilter::OnProcessImage(...)
{
    auto downsampled = BuildDownsampledShader(canvas, image);
    // ...pass downsampled to next stage...
}

std::shared_ptr<Drawing::Image> GEMyFilter::BuildDownsampledShader(...)
{
    Drawing::Matrix inv;
    if (!context.matrix.Invert(inv)) {
        // ✅ names the actual function — grep finds it immediately
        LOGE("GEMyFilter::BuildDownsampledShader invert matrix failed");
        return nullptr;
    }
    // ...
}
```

```cpp
// 🚫 copy-pasted from OnProcessImage — grep lands on the wrong function
LOGE("GEMyFilter::OnProcessImage invert matrix failed"); // should be BuildDownsampledShader
```

---

## Color Space Propagation

**Propagate the canvas's color space to offscreen surfaces.** Constructing `ImageInfo` without the source canvas's color space causes HDR/EDR content to render with wrong colors or missing brightness range — the GPU composites the offscreen result back with a mismatched color space.

```cpp
// 🚫 hardcoded color space, drops source color space
Drawing::ImageInfo imageInfo(width, height, Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL);
auto surface = Drawing::Surface::MakeRenderTarget(gpuCtx.get(), false, imageInfo);
```

```cpp
// ✅ propagate canvas surface color space
auto* canvasSurface = canvas.GetSurface();
auto colorSpace = canvasSurface ? canvasSurface->GetImageInfo().GetColorSpace() : nullptr;
auto surface = Drawing::Surface::MakeRenderTarget(gpuCtx.get(), false, imageInfo);
```

---

## Stale Cache on Failure

**Reset cache to `nullptr` when cache-building fails.** If an intermediate image or atlas build returns `nullptr`, the cache pointer retains its previous value — the next frame uses stale data.

```cpp
// 🚫 cache stays stale on failure
auto cacheImg = BuildCacheImage(canvas, params);
if (cacheImg) {
    cacheAnyPtr_ = std::make_shared<std::any>(cacheImg);
}
// failure path: cacheAnyPtr_ still points to old data
```

```cpp
// ✅ null on failure so next frame rebuilds
auto cacheImg = BuildCacheImage(canvas, params);
if (cacheImg) {
    cacheAnyPtr_ = std::make_shared<std::any>(cacheImg);
} else {
    cacheAnyPtr_ = nullptr;
    LOGE("...cache build failed, will retry next frame");
}
```

---

## IPC Enum Validation

**Range-check enums deserialized from `Parcel`.** Casting a raw `uint32_t` to an enum without bounds checking allows out-of-range values to slip through and desync the Parcel cursor (ES.49 — use a named cast, validate before use). Validate the value is within `[enum::first, enum::MAX)` before use.

```cpp
// 🚫 no range check — cursor desyncs on invalid input
uint32_t typeVal;
parcel.ReadUint32(typeVal);
auto type = static_cast<GEMyType>(typeVal);  // may be out of range
```

```cpp
// ✅ validate range before cast
uint32_t typeVal;
if (!parcel.ReadUint32(typeVal)) { return false; }
if (typeVal >= static_cast<uint32_t>(GEMyType::MAX)) {
    LOGE("...invalid enum value");
    return false;
}
auto type = static_cast<GEMyType>(typeVal);
```

Also use the correct `static_cast<uint32_t>` (not `int32_t`) when comparing against `uint32_t` — signed/unsigned mismatch can produce wrong results because negative signed values convert to large unsigned values.
