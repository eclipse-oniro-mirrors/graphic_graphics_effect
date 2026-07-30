# mask/ — Coverage Mask Effects

Base class: `GEShaderMask` · Namespace: `Rosen::Drawing` · Params namespace: `Drawing`

| Property | Value |
|----------|-------|
| Key virtual methods | `GenerateDrawingShader(float w, float h)` · `GenerateDrawingShaderHasNormal(float w, float h)` |
| Constructor param type | `GE{Name}ShaderMaskParams` (**no** `Drawing::` prefix — class is already in `Drawing` namespace) |
| File suffix | `shader_mask` |

**Namespace pitfall**: unlike filter/shader, mask constructors and `DECLARE_GEFILTER_TYPEFUNC` omit the `Drawing::` prefix because masks are declared in Drawing by default.
