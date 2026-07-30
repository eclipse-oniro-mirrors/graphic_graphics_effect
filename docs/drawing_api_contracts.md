# Drawing API Contracts

Verified behavior of `graphic_2d` Drawing APIs that GE depends on. Load the section matching the API you're calling.

These are **upstream contracts**, not GE conventions. When in doubt, read the upstream headers under `graphic_2d/rosen/modules/2d_graphics/include/`.

---

## Task Map

| Task | Section |
|---------|---------|
| Get GPU context from canvas | [Canvas::GetGPUContext](#canvasgetgpucontext) |
| Compile a shader string to RuntimeEffect | [RuntimeEffect::CreateForShader](#runtimeeffectcreateforshader) |
| Create image from RuntimeShaderBuilder | [RuntimeShaderBuilder::MakeImage](#runtimeshaderbuildermakeimage) |
| Wrap image as shader | [ShaderEffect::CreateImageShader](#shadereffectcreateimageshader) |
| Create GPU render target | [Surface::MakeRenderTarget](#surfacemakerendertarget) |

---

## Canvas::GetGPUContext

**Signature**: `std::shared_ptr<GPUContext> Canvas::GetGPUContext() const`

**May return a null `shared_ptr`** — notably on CPU-only render paths where no GPU context is available.

**Implication**: Always null-check before passing `.get()` to downstream APIs. All downstream APIs (`MakeImage`, `MakeRenderTarget`) handle null safely by returning `nullptr`, but a null result means no GPU work was done.

```cpp
auto gpuCtx = canvas.GetGPUContext();
if (gpuCtx == nullptr) { LOGE("...gpuContext is null"); return image; }
// safe to pass gpuCtx.get() to MakeImage / MakeRenderTarget
```

---

## RuntimeEffect::CreateForShader

**Signature**: `static std::shared_ptr<RuntimeEffect> CreateForShader(const std::string& sl, const RuntimeEffectOptions& options)` (overload without `options` also available)

**Always returns a valid `shared_ptr`**, but the effect may fail to compile (syntax error, unsupported feature). A failed effect produces no output when used — verify by checking that downstream operations (`RuntimeShaderBuilder`, `MakeImage`) succeed.

**In GE, use `GECreateRuntimeEffectForShader` instead of the raw upstream API.** The wrapper (`include/util/ge_shader_diagnostics.h`) forwards directly to `CreateForShader` with negligible overhead, but optionally records shader source + call site for diagnostics when the runtime property `persist.sys.graphic.geShaderDiagnosticsEnabled` is enabled.

```cpp
// ✅ use the GE wrapper
auto effect = GECreateRuntimeEffectForShader(shaderStr);
// with options (e.g., highp local coords for nonlinear distortion)
Drawing::RuntimeEffectOptions reo;
reo.useHighpLocalCoords = true;
auto effect = GECreateRuntimeEffectForShader(shaderStr, reo);
```

```cpp
// 🚫 bypasses diagnostics infrastructure
auto effect = Drawing::RuntimeEffect::CreateForShader(shaderStr);
```

The effect is immutable once created — cache it and reuse.

---

## RuntimeShaderBuilder::MakeImage

**Signature**: `std::shared_ptr<Image> MakeImage(GPUContext* grContext, const Matrix* localMatrix, ImageInfo resultInfo, bool mipmapped)`

**Returns `nullptr`** when `grContext` is null or when GPU surface creation fails. There is no CPU/raster fallback — a null GPU context produces no image.

**Implication**: Passing null `grContext` is safe (no crash), but produces no image. Always check the return value.

```cpp
// ✅ check the return value
auto result = builder->MakeImage(gpuCtx.get(), nullptr, imageInfo, false);
if (result == nullptr) { LOGE("...MakeImage failed"); return image; }
```

---

## ShaderEffect::CreateImageShader

**Signature**: `std::shared_ptr<ShaderEffect> CreateImageShader(const Image& image, TileMode xTile, TileMode yTile, const SamplingOptions& sampling, const Matrix& matrix)`

**Always returns a valid `shared_ptr`**, but the internal shader may be null if the image is invalid. The null manifests later at draw time. Validate the image before calling.

```cpp
// ✅ validate image first, then use
if (image == nullptr) { LOGE("...image is null"); return image; }
auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
builder.SetChild("imageShader", shader);
```

---

## Surface::MakeRenderTarget

**Signature**: `std::shared_ptr<Surface> MakeRenderTarget(GPUContext* gpuContext, bool budgeted, const ImageInfo& imageInfo)`

**Returns `nullptr`** on failure (null GPU context, unsupported format, GPU memory exhaustion). GPU-only — no fallback path. Passing null `gpuContext` is safe but always produces `nullptr`.
