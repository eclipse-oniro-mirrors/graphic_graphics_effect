# shape/ — SDF Shape Effects

Base class: `GESDFShaderShape` (extends `GEShaderShape`) · Namespace: `Rosen::Drawing` · Params namespace: `Drawing`

| Property | Value |
|----------|-------|
| Key virtual methods | `GenerateDrawingShader(float w, float h)` · `GetSDFShapeType() const` · `HasType(GESDFShapeType) const` |
| Constructor param type | `GE{Name}ShapeParams` (**no** `Drawing::` prefix — same namespace) |
| File suffix | `sdf_shader_shape` |

**SDF companion effects** (also in this directory): `GESDFBorderShader`, `GESDFColorShader`, `GESDFClipShader`, `GESDFShadowShader`.

**Implementation notes**: [docs/shape/](../../../docs/shape/)
