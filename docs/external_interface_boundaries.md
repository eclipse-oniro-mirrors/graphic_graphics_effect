# External Interface Boundaries

GE's public API surface and the trust boundaries at each entry point. Most external input arrives through `graphic_2d` (the consumer of GE) — GE itself does not directly receive IPC or NAPI calls. Load this when modifying code that handles untrusted values from upstream callers.

For C++ coding conventions, see [cpp_guidelines.md](cpp_guidelines.md). For domain-specific patterns, see [cpp_domain_guidelines.md](cpp_domain_guidelines.md).

---

## Task Map

| Task | Section |
|---------|---------|
| Understand which APIs receive untrusted input | [Entry Points](#entry-points) |
| Understand key risk areas | [Key Risk Areas](#key-risk-areas) |
| Modify IPC/Parcel deserialization | [IPC Deserialization](#ipc-deserialization) |
| Modify external .so loading | [Dynamic Extension Loading](#dynamic-extension-loading) |
| Modify XML config parsing | [XML Configuration Parsing](#xml-configuration-parsing) |
| Modify system property reads | [System Property Reads](#system-property-reads) |

---

## Entry Points

GE receives external input through five surfaces. Each has a different trust level and validation requirement.

| Entry Point | Trust Level | Input Source | Validation |
|---|---|---|---|
| `GEVisualEffect::SetParam` | **Untrusted** | IPC from app processes via `graphic_2d` | Tag lookup returns `INVALID` on unrecognized; values get `.params.in` `min`/`max` clamp where defined, but not all params have constraints |
| `GEExternalDynamicLoader::CreateGEXObjectByType` | **Untrusted** | NAPI/NDK app params via `graphic_2d` | `type` is `uint32_t` passed through as-is; `param` is `void*` with **no** type check |
| `GEXmlParserBase::LoadGraphicConfiguration` | **System/privileged** | System config files (see [XML Configuration Parsing](#xml-configuration-parsing)) | libxml2 parsing; paths are fixed system locations |
| `GESystemProperties::GetBoolSystemProperty` | **Privileged** | OpenHarmony system parameter service | Modification requires system-level parameter write access |
| `GECreateRuntimeEffectForShader` | **Trusted** | Compile-time shader strings in GE source | No external input; shader source is passed by the caller from GE's own code |

**Key insight**: `SetParam` is the primary external input surface. IPC-deserialized effect parameters (floats, vectors, colors, matrices, images) flow through `SetParam` into params structs, then into shader uniforms. Validation happens at two layers: (1) `.params.in` `min`/`max` constraints generate automatic `std::clamp` in `SetParamsMemberByTag` (covers some but not all params), and (2) individual effects check values in constructors and `OnProcessImage` (e.g., dimension checks, null checks). Not all params have `.params.in` constraints, so runtime checks remain important. Unrecognized tags return `GEParamsMemberTag::INVALID`, are logged at DEBUG level via `GE_LOGD`, and are ignored. Values are consumed by GE's own effect code (not passed to arbitrary APIs), limiting the attack surface to visual glitches or GPU resource consumption within the rendering process.

---

## Key Risk Areas

From past incident experience, the main risks when modifying external-facing code are:

| Risk | Where | Mitigation | See also |
|---|---|---|---|
| IPC enum/type desync | `SetParam` tag → enum lookup, `CreateGEXObjectByType` type | Unrecognized tags return `INVALID`; factory path range-checks against `MAX_EFFECTS`; direct path does **not** validate | [IPC Enum Validation](cpp_domain_guidelines.md#ipc-enum-validation) |
| NaN/Inf in shader uniforms | `SetParam` float values → params struct → `SetUniform` | `.params.in` `min`/`max` uses `std::clamp`, but NaN/Inf bypass `<`/`>` comparisons — clamp silently passes them through. If GE receives untrusted floats, check for NaN/Inf before `SetUniform` — but prefer catching these at the NAPI/SDK entry point | — |
| Null dereference | `shared_ptr<Image>`/`shared_ptr<PixelMap>` from `SetParam`, `GetGPUContext()` on CPU-only paths | Null-check at the boundary before deref; `CreateImageShader` takes `const Image&` (reference), so the null risk is at the dereference-before-call site, not inside the API; `MakeImage`, `MakeRenderTarget` handle null safely | [Null Safety](cpp_guidelines.md#null-safety) |
| `static RuntimeEffect` cache race | Function-level `static` with manual null-check lazy init (e.g., `ge_kawase_blur_shader_filter.cpp`, `ge_direction_light_shader_filter.cpp`) — the check-then-assign is a data race | Prefer C++11 magic statics (direct init) or `thread_local` (as `ge_blur_bubbles_rise_filter.cpp` does); file-scope `static` init is also safe | [Static & Thread Safety](cpp_guidelines.md#static--thread-safety) |

**Defense layering**: Not all checks need to be in GE. Some validation is better placed at upstream layers — and already exists there:
- **NAPI binding** (`graphic_2d`'s `filter_napi.cpp`): basic range checks like `scale >= 0`, `dBright >= 0 && dBright <= 1`, capacity overflow checks
- **Effect chain** (`graphic_2d`'s `effect_image_chain.cpp`): `radius < 0.0f` rejection, `maskRadiusX <= 0` rejection, dimension checks before calling GE
- **ace_engine** (`rosen_render_context.cpp`): `degree < 0` normalization, `fractials < 0.0f` rejection; `rosen_render_context_multi_thread.cpp`: blur parameter normalization before dispatching to GE
- **IPC deserialization** (`graphic_2d`'s `rs_render_*_base.cpp`): `EFFECT_COUNT_LIMIT` loop bounds, `RSNGEffectType` map lookup

GE should focus on checks that protect its own invariants (shader safety, null safety, thread safety), not repeat guards already enforced upstream.

---

## IPC Deserialization

External app processes construct `Parcel` data that reaches GE through `graphic_2d`'s `RSNGRender{Filter,Mask,Shader,Shape}Base::Unmarshalling` (in `render_service_base/src/effect/`). The deserialized values call `GEVisualEffect::SetParam(tag, value)` — GE's boundary is here.

**What GE controls**: `GEVisualEffectImpl::SetParam` — the sink. String tags are converted to `GEParamsMemberTag` via `GEParamsMemberTagFromString` (an `unordered_map` lookup). Unrecognized tags return `INVALID` and are ignored. Values pass through `SetParamsMemberByTag`, which applies `.params.in`-defined `min`/`max` constraints via `std::clamp` before storing — but not all params have these constraints defined.

**What GE does NOT control** (all in `graphic_2d`, not this repo):
- The `Unmarshalling` loop and `EFFECT_COUNT_LIMIT` (defined in `render_service_base`'s `rs_render_effect_template.h`) — the caller's responsibility to cap chain length
- The `static_cast<RSNGEffectType>` from raw `int16_t` — the caller's enum, not `GEFilterType`
- The `RSNGRenderEffectHelper::CreateGEVisualEffect(type)` bridge that maps `RSNGEffectType` → effect name string → `GEVisualEffect` constructor

**When modifying `SetParam` or params structs**:
- Don't add new `SetParam(string, T)` overloads that bypass `GEParamsMemberTagFromString` — the tag-based API is the safe path
- If a new param type accepts untrusted values, add range validation in the effect's `OnProcessImage`/`MakeDrawingShader` before passing to shader uniforms — see [IPC Enum Validation](cpp_domain_guidelines.md#ipc-enum-validation) for the enum-specific case
- Don't trust `shared_ptr<Image>` or `shared_ptr<PixelMap>` from `SetParam` — null-check before use (see [Null Safety](cpp_guidelines.md#null-safety))

---

## Dynamic Extension Loading

`GEExternalDynamicLoader` loads `/system/lib64/libgraphics_effect_ext.z.so` (32-bit: `/system/lib/`) via `dlopen` + `dlsym`. The loaded function returns `void*`.

There are two call paths:

1. **Factory path** (`GEEffectFactory` via `RegisterExternalEffect`): `type` is a compile-time `GEFilterType` template parameter. `GEEffectFactory::Create` validates the type against `MAX_EFFECTS` (range check) before dispatching. The `void*` return is cast to `IGEFilterType*` and wrapped in `shared_ptr`.

2. **Direct path** (`graphic_2d`'s `effect_image_chain.cpp` calls `CreateGEXObjectByType` directly): `type` is `uint32_t` passed by the caller. `CreateGEXObjectByType` does **not** validate or cast `type` — it passes it through to the external `.so` as-is. The caller does the `static_cast` to `GEShaderFilter*`.

**Boundaries**:
- The `.so` path is a fixed system location — not writable by normal apps
- `param` is `void*` — the external .so receives it and interprets the layout. GE's CFI cross-DSO protection checks vtable types but not object layout match
- System property `rosen.graphic.gex.enable` (default: true) controls whether external loading is active

**When modifying `GEExternalDynamicLoader`**:
- Don't change the `.so` path to a non-system location
- Don't remove the `dlsym` null check after loading
- Don't add new `void*` return paths without documenting the expected type at the call site

---

## XML Configuration Parsing

`GEXmlParserBase` parses `graphic_config.xml` using libxml2 (`xmlParseFile`). The parsed values affect `GEFrostedGlassEffectCfg` (currently only the `isDisableAntiAliasCode` boolean flag).

**Boundaries**:
- Config file is searched in system variant directories: `/system/variant/{phone,tablet,pc,watch,tv,car,smarthomehost}/base/` + `etc/graphic/graphic_config.xml`
- A product config path `/sys_prod/` + `etc/graphic/graphic_config.xml` is also searched
- Requires root/privileged access to modify these files
- libxml2 is used via `xmlParseFile` with no explicit entity resolution configuration — `xmlParseFile` defaults process entities; if modifying the parser, explicitly disable entity processing (`XML_PARSE_NOENT`)

**When modifying XML parsing**:
- Don't change the file path search logic to include non-system directories
- Don't add parsing of complex/nested structures without bounds checking
- See [AGENTS.md → Boundaries → Ask first](../AGENTS.md) — XML parsing changes require review

---

## System Property Reads

`GESystemProperties` reads system parameters via `GetBoolSystemProperty(name, default)` (using `CachedParameterGetChanged`) and `GetEventProperty(name)` (using `system::GetParameter`).

**Boundaries**:
- Modifying system parameters requires system-level parameter write access — not available to normal apps
- Property names used by GE: `persist.sys.graphic.geShaderDiagnosticsEnabled`, `rosen.graphic.gex.enable`, `persist.sys.graphic.hpsEffectEnabled`

**When modifying property reads**:
- Don't add new property reads without a documented default value
- Don't use property values as array indices or allocation sizes without bounds checking
