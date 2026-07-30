# C++ Domain-Specific Guidelines

GE-specific patterns from bug-fix history. Load only the section matching your task.

For general C++ conventions, see [cpp_guidelines.md](cpp_guidelines.md). For upstream Drawing API behavior, see [drawing_api_contracts.md](drawing_api_contracts.md).

---

## Task Map

| Task | Section |
|---------|---------|
| Write error log messages | [Log Accuracy](#log-accuracy) |
| Propagate color space to offscreen | [Color Space Propagation](#color-space-propagation) |
| Invalidate cache on failure | [Stale Cache on Failure](#stale-cache-on-failure) |

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

**Propagate the canvas's color space to offscreen surfaces.** Constructing `ImageInfo` without the source canvas's color space causes HDR/EDR content to render with missing thickness or wrong colors — the GPU composites the offscreen result back with a mismatched color space.

```cpp
// 🚫 hardcoded color space, drops source color space
Drawing::ImageInfo imageInfo(width, height, Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL);
auto surface = Drawing::Surface::MakeRenderTarget(ctx, false, imageInfo);
```

```cpp
// ✅ propagate canvas surface color space
auto* canvasSurface = canvas.GetSurface();
Drawing::ImageInfo imageInfo(width, height, Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL, colorSpace);
auto surface = Drawing::Surface::MakeRenderTarget(ctx, false, imageInfo);
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
