# Effect Types Reference

Detailed comparison of the four effect types in graphics_effect.

## Table of Contents

1. [Type Comparison](#type-comparison)
2. [Filter Effects](#filter-effects)
3. [Shader Effects](#shader-effects)
4. [Mask Effects](#mask-effects)
5. [Shape Effects](#shape-effects)
6. [File Naming](#file-naming)
7. [Namespace Differences](#namespace-differences)

---

## Type Comparison

| Property | filter | shader | mask | shape |
|----------|--------|---------|------|-------|
| Base class | `GEShaderFilter` | `GEShader` | `GEShaderMask` | `GESDFShaderShape` |
| Namespace | `Rosen` | `Rosen` | `Drawing` | `Drawing` |
| Class pattern | `GE{Name}ShaderFilter` | `GE{Name}Shader` | `GE{Name}ShaderMask` | `GE{Name}SDFShaderShape` |
| Params namespace | `Drawing` | `Drawing` | `Drawing` | `Drawing` |
| Key method | `OnProcessImage()` | `MakeDrawingShader()` | `GenerateDrawingShader()` | `GenerateDrawingShader()` |
| Input | Image + canvas | Rect + progress | Width + height | Width + height |
| Output | `shared_ptr<Image>` | Shader effect | Shader effect | Shader effect + SDF type |
| File suffix | `shader_filter` | `shader` | `shader_mask` | `sdf_shader_shape` |
| Typical use | Blur, color, distortion | Lighting, material | Gradient, image masks | SDF borders, shadows |

---

## Filter Effects

Filters process images — they take an input image, apply a shader, and return a modified image.

**Key virtual method**:
```cpp
std::shared_ptr<Drawing::Image> OnProcessImage(
    Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src,
    const Drawing::Rect& dst) override;
```

**Constructor receives params**: `GESomethingShaderFilter(const Drawing::GESomethingShaderFilterParams& params)`

**Typical OnProcessImage flow**:
1. Validate input image and dimensions
2. Get `RuntimeEffect` from static `GetEffect()` method
3. Create image shader via `ShaderEffect::CreateImageShader` with matrix inversion
4. Build `RuntimeShaderBuilder`, set children (`image`) and uniforms
5. Call `builder.MakeImage()` with GPU context fallback

**Optional method**: `Preprocess(Drawing::Canvas&, const Drawing::Rect&, const Drawing::Rect&)` — preprocessing before effect application.

**Existing examples**: Kawase blur, Mesa blur, Grey, Color gradient, Edge light, Displacement distortion, Water ripple, etc.

---

## Shader Effects

Shaders generate shader effects directly — they don't consume input images but produce visual effects based on geometry and parameters.

**Key virtual method**:
```cpp
void MakeDrawingShader(const Drawing::Rect& rect, float progress) override;
```

**Optional methods**:
- `Preprocess(Drawing::Canvas&, const Drawing::Rect&)` — preprocessing
- `OnDrawShader(Drawing::Canvas&, const Drawing::Rect&)` — custom drawing behavior

**Constructor**: `GESomethingShader(const Drawing::GESomethingShaderParams& params)`

**Typical flow**: Create `RuntimeShaderBuilder`, set uniforms based on rect and progress, generate `ShaderEffect` for composition.

**Existing examples**: Border light, Circle flowlight, Aurora noise, Frosted glass effect, Color gradient effect, Wavy ripple light, etc.

---

## Mask Effects

Masks produce alpha/coverage shaders used for compositing. They take width/height and return a shader that defines mask coverage.

**Key virtual methods**:
```cpp
std::shared_ptr<ShaderEffect> GenerateDrawingShader(float width, float height) const override;
std::shared_ptr<ShaderEffect> GenerateDrawingShaderHasNormal(float width, float height) const override;
```

**Optional methods**:
- `GetSubtractedRect(float, float)` — nine-patch layout optimization
- `GetImage()` — return associated image (for image-based masks)
- `GetUseEffect()` — whether effect is used (for use-effect masks)

**Constructor**: `GESomethingShaderMask(const GESomethingShaderMaskParams& params)` — note: params are in `Drawing` namespace, constructor doesn't prefix `Drawing::`.

**Helper methods in generated template**:
- `GetShaderBuilder()` — get/create runtime shader builder
- `GetNormalShaderBuilder()` — for the normal-channel variant

**Existing examples**: Frame gradient, Linear gradient, Radial gradient, Wave gradient, Image mask, Pixel map mask, Ripple mask, etc.

---

## Shape Effects

Shapes define SDF (Signed Distance Field) geometry for borders, shadows, clips, and other shape-based effects.

**Key virtual methods** (in addition to `GenerateDrawingShader`):
```cpp
GESDFShapeType GetSDFShapeType() const override;
bool HasType(const GESDFShapeType type) const override;
```

**Constructor**: `GESomethingSDFShaderShape(const GESomethingShapeParams& params)` — note: params class doesn't always have `ShaderFilter` suffix.

**SDF shape types** must be defined and returned from `GetSDFShapeType()`. Existing types include RRect, Pixelmap, Path, Transform, UnionOp, Triangle, DistortOp.

**Existing examples**: RRect shape, Pixelmap shape, Path shape, Transform shape, UnionOp shape, Triangle shape.

**SDF companion effects**: `GESDFBorderShader`, `GESDFColorShader`, `GESDFClipShader`, `GESDFShadowShader` — these operate on SDF shapes defined by the shape effects above.

---

## File Naming

Files follow a strict convention based on the effect type:

| Effect Name | Type | Header File | Source File | Params File |
|-------------|------|-------------|-------------|-------------|
| `my_blur` | filter | `ge_my_blur_shader_filter.h` | `ge_my_blur_shader_filter.cpp` | `ge_my_blur_shader_filter.params.in` |
| `my_light` | shader | `ge_my_light_shader.h` | `ge_my_light_shader.cpp` | `ge_my_light_shader.params.in` |
| `my_gradient` | mask | `ge_my_gradient_shader_mask.h` | `ge_my_gradient_shader_mask.cpp` | `ge_my_gradient_shader_mask.params.in` |
| `my_shape` | shape | `ge_my_shape_sdf_shader_shape.h` | `ge_my_shape_sdf_shader_shape.cpp` | `ge_my_shape_sdf_shader_shape.params.in` |

**Directory placement**:
- Headers: `include/effect/<type>/`
- Sources: `src/effect/<type>/`
- Params: `include/effect/<type>/`

---

## Namespace Differences

This is a common source of confusion:

| Type | Class namespace | Params namespace | Constructor param type |
|------|----------------|-----------------|----------------------|
| filter | `OHOS::Rosen` | `Drawing` | `Drawing::GE...Params` |
| shader | `OHOS::Rosen` | `Drawing` | `Drawing::GE...Params` |
| mask | `OHOS::Rosen::Drawing` | `Drawing` | `GE...Params` (no `Drawing::` prefix in constructor) |
| shape | `OHOS::Rosen::Drawing` | `Drawing` | `GE...Params` (no `Drawing::` prefix in constructor) |

For **filter** and **shader**: the constructor takes `Drawing::GE...Params`.
For **mask** and **shape**: the constructor takes `GE...Params` directly (both are in `Drawing` namespace already).

The `DECLARE_GEFILTER_TYPEFUNC` macro also differs:
- filter/shader: `DECLARE_GEFILTER_TYPEFUNC(ClassName, Drawing::GEParamsClass)`
- mask/shape: `DECLARE_GEFILTER_TYPEFUNC(ClassName, GEParamsClass)`