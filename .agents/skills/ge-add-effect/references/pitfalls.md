# Common Pitfalls

Mistakes that have been observed when adding new effects and how to avoid them.

## Table of Contents

1. [clang-format Required](#clang-format-required)
2. [cast_from Same Type](#cast_from-same-type)
3. [Enum Placement](#enum-placement)
4. [Forgot BUILD.gn](#forgot-buildgn)
5. [Skipped Code Generation](#skipped-code-generation)
6. [Premature Code Generation](#premature-code-generation)
7. [Namespace Confusion](#namespace-confusion)
8. [Wrong Base Class Method](#wrong-base-class-method)
9. [gen_metadata.py and gen_effect_header.py Order](#gen_metadatapy-and-gen_effect-headerpy-order)
10. [Skipped Unit Tests](#skipped-unit-tests)
11. [Constraints Not Applied on Direct Access](#constraints-not-applied-on-direct-access)
12. [Manually Editing Generated Files](#manually-editing-generated-files)
13. [Duplicate Prop Tags](#duplicate-prop-tags)
14. [Build Failure Diagnosis](#build-failure-diagnosis)

---

## clang-format Required

**Problem**: If `clang-format` is not installed when running `gen_metadata.py` and `gen_effect_header.py`, the generated files (`ge_params_reflection.h`, `ge_params_reflection.cpp`, `ge_effects_params.h`) will not be formatted. The repository's existing generated files were formatted by clang-format, so unformatted new output creates hundreds of lines of noise in the diff — making it impossible to see the actual changes.

**Detection**: The generation scripts print `[!] clang-format not found. Generated file will not be formatted.` when this happens.

**Fix**: Install `clang-format` before running the generation tools. The scripts search for `clang-format`, `clang-format-18`, `clang-format-17`, `clang-format-16`, `clang-format-15` in order.

**Why this matters**: The repository tracks generated files in git. When clang-format is available, the diff between old and new generated content is clean (only substantive changes). Without it, every generated line may differ in spacing/indentation, obscuring what actually changed.

---

## cast_from Same Type

**Problem**: Using `cast_from` with a type that is identical to the field type causes duplicate switch case labels in the generated setter code, resulting in a compile error.

**Example of the bug**:
```cpp
// WRONG: cast_from type equals field type
[[ge::prop(cast_from="Drawing::Path")]]
Drawing::Path path;
```

This was observed in `ge_sdf_path_shader_shape.params.in` in the real codebase. It caused `duplicate case value 'SDF_PATH_SHAPE_PATH'`.

**Fix**: Remove `cast_from` when the type is the same as the field. Prefer omitting `name=` entirely — auto-generation handles naming correctly:
```cpp
Drawing::Path path;  // Auto-generates "SDFDiamondShape_Path" tag
```
Or use positional syntax if you want an explicit PascalCase tag:
```cpp
[[ge::prop("SDFPathShape_Path")]]
Drawing::Path path;
```

**Rules**:
1. `cast_from` exists for type conversion — the caller passes one type, the field stores another. If they're the same (after type alias resolution, e.g., `int` → `int32_t`), there's nothing to convert, so don't use `cast_from`.
2. **Ask the user before adding `cast_from`** — confirm they want callers to pass a different type. If not, omit it.
3. **`custom` can be used alone** — `custom` without `cast_from` acts as an identity cast (source type = field type), useful for validation, clamping, or sanitization where the type doesn't change but the value needs transformation. When paired with `cast_from`, `custom` handles non-trivial conversions between different types.

---

## Enum Placement

**Problem**: Adding a `GEFilterType` enum value after `MAX` or in the wrong position.

**Fix**: Always add new enum values **before** `MAX`:
```cpp
YOUR_NEW_ENUM,   // Add here
MAX,             // MAX must remain last
```

The `MAX` value is used as a sentinel for iteration bounds and should always be the last enum value.

---

## Forgot BUILD.gn

**Problem**: Adding new `.cpp` files without updating `BUILD.gn`. This causes either a link error (symbol not found) or the source simply not being compiled.

**Fix**: Add the source path to the `sources` list in the main `BUILD.gn`, alphabetically with other sources of the same effect type:
```python
"src/effect/filter/ge_your_effect_shader_filter.cpp",
```

Also consider updating `test/unittest/BUILD.gn` if unit tests are being added.

---

## Skipped Code Generation

**Problem**: After editing `.params.in` files, skipping `gen_metadata.py` and `gen_effect_header.py`. The reflection files won't include the new parameters, so:
- The tag enum won't have the new tag
- The SetParam API won't handle the new fields
- `ge_effects_params.h` won't include the new `.params.in`
- Code that uses `GEParamsMemberHelper` won't work with the new effect

**Fix**: Run both tools after every `.params.in` change:
```bash
python tool/generate_metadata/gen_metadata.py
python tool/generate_metadata/gen_effect_header.py
```

---

## Premature Code Generation

**Problem**: Running `gen_metadata.py` and `gen_effect_header.py` before finishing edits to a `.params.in` file. The generated metadata will reflect the intermediate (unfinished) state of the parameters, not the final definitions. If you later modify `.params.in` and forget to re-run the gen tools, the reflection files will be stale and mismatched.

**Detection**: Build may succeed but runtime behavior will be wrong — the SetParam API won't handle the final field definitions, and tag enum values won't match the final parameter names.

**Fix**: Always finish all `.params.in` edits before running the generation tools. If you make additional edits after running gen tools, re-run both tools again.

---

## Namespace Confusion

**Problem**: Mixing up which namespace the params class lives in for different effect types.

- **filter / shader**: Constructor takes `Drawing::GEParamsClass` — the params are in `Drawing` namespace while the effect class is in `Rosen`
- **mask / shape**: Constructor takes `GEParamsClass` directly — both the effect and params are in `Drawing` namespace

**Fix**: Check the generated template for the correct namespace. The `create_effect.py` tool generates the correct constructor signature automatically.

---

## Wrong Base Class Method

**Problem**: Implementing the wrong virtual method for the effect type. Each type has a different primary method:

| Type | Primary Method | Signature |
|------|---------------|-----------|
| filter | `OnProcessImage` | `(Canvas&, shared_ptr<Image>, Rect&, Rect&) → shared_ptr<Image>` |
| shader | `MakeDrawingShader` | `(Rect&, float progress) → void` |
| mask | `GenerateDrawingShader` | `(float width, float height) → shared_ptr<ShaderEffect>` |
| shape | `GenerateDrawingShader` + `GetSDFShapeType` + `HasType` | Multiple methods |

**Fix**: The scaffolded `.cpp` already has the correct method skeleton. Use it as the starting point.

---

## gen_metadata.py and gen_effect_header.py Order

**Problem**: Running `gen_effect_header.py` before `gen_metadata.py`, or running only one of them.

Both must be run, and order doesn't strictly matter since they generate different files. However, `gen_metadata.py` also calls `gen_effect_header.py`-like logic internally for the unified header, so running `gen_metadata.py` first then `gen_effect_header.py` is the conventional order.

The key point is: **run both after any `.params.in` modification**. Neither is optional.

---

## Skipped Unit Tests

**Problem**: Adding a new effect without any unit tests. Shader logic errors and parameter clamping bugs, and null-pointer crashes in `GetEffect()` all produce wrong output at runtime with no compile error — they break silently in the rendering pipeline.

**Why this is dangerous**: The GE rendering pipeline is highly visual — a shader that produces wrong colors or distorted geometry may not crash but build, but will produce visibly wrong results on screen. Without tests, these bugs are only caught by manual visual inspection, which is unreliable and slow.

**Fix**: Write at minimum three tests:
1. Constructor with valid params — verify filter object is created
2. SetParam round-trrip — set a param, verify getter returns the clamped/validated value
3. GetEffect returns non-null — verify the shader can be compiled and applied to an image

**Test file**: `test/unittest/ge_<name>_test.cpp`
**Build command**: `hb build graphics_effect -t`

---

## Constraints Not Applied on Direct Access

**Problem**: Declaring `min`/`max` in `.params.in` generates constraint metadata, but these constraints are **only enforced when parameters are set through the SetParam API** (`SetParam("tag", value)`). Direct field access — `params->field = value` or `GetParams<T>()->field = value` — bypasses constraints entirely.

**Why this matters**: The effect class constructor reads params via direct field access (`params.radius`, `params.intensity`). If you rely on `min`/`max` to clamp values, you'll get unclamped values in the constructor. The constructor must do its own clamping/validation.

**Detection**: Runtime behavior differs: setting a param via `SetParam("Tag", -5)` clamps to `min=0`, but a param struct initialized with `radius = -5` passes through unchanged.

**Fix**: Always clamp in the constructor:
```cpp
// In your effect constructor
radius_ = std::clamp(params.radius, 0.0f, 200.0f);
```

---

## Manually Editing Generated Files

**Problem**: Editing the generated files (`ge_params_reflection.h`, `ge_params_reflection.cpp`, `ge_effects_params.h`) directly. These files are overwritten on every `gen_metadata.py` + `gen_effect_header.py` run — any manual changes are silently lost.

**Detection**: All generated files contain the header:
```cpp
// AUTO GENERATED BY tool/generate_metadata/gen_metadata.py
// DO NOT EDIT MANUALLY
```

**Fix**: Never edit generated files. Instead:
- To add/modify parameters → edit `.params.in` files, then re-run gen tools
- To add custom transformer → edit `ge_value_transformer.h`
- To configure type aliases/blocked types → edit `tool/generate_metadata/config.json`

---

## Duplicate Prop Tags

**Problem**: Two fields (possibly across different effects) producing the same string tag name. The gen tool fails with a conflict error.

**Detection**: `gen_metadata.py` reports a tag conflict during generation.

**Fix**: Auto-generated tags (fields without `name=`) already include the effect prefix from `[[ge::params(name=...)]]`, producing `StructName_PascalCaseField` format. Collision risk is low since different struct names produce different prefixes. The real danger is explicitly writing bare names like `name="Radius"` without prefix, or using ALL_CAPS format that differs from the codebase convention. Prefer omitting `name=` — auto-generation handles naming correctly. Only add explicit `name=` when the auto-generated name would be misleading or doesn't match the desired exposed string name.

---

## Build Failure Diagnosis

When `hb build graphics_effect -i` fails, or when other problems arise during the workflow, match the error or situation below:

### Build Errors

| Error Contains | Root Cause | Fix |
|---------------|-----------|-----|
| `duplicate case value` | `cast_from` same-type bug | Remove `cast_from` where type equals field type (see [cast_from Same Type](#cast_from-same-type)). Or use `custom` alone for same-type validation/clamping |
| `undefined reference` / `undefined symbol` | Missing BUILD.gn entry | Add `.cpp` path to sources list alphabetically |
| `clang-format not found` (warning only) | Missing clang-format | Install clang-format; not a build error but re-run gen tools to avoid noisy diff |
| `no matching function` / constructor error | Wrong namespace in constructor | Check `references/effect-types.md` for namespace conventions per type |
| Parse error from gen tool | Invalid `.params.in` syntax — positional syntax with multiple options, or missing `[[ge::params]]` | Read `references/params-syntax.md` for correct attribute format. Positional syntax only works for single `name` option |
| Other / unrelated | Not an effect-creation bug | Do NOT modify effect files; diagnose separately |

### Workflow Fallbacks

| Situation | What to Do |
|-----------|-----------|
| Test build or test run fails | Check test file naming, include paths, and BUILD.gn entry |
| Build fails for unrelated reasons | Do NOT modify effect files to fix unrelated build errors. Diagnose the root cause separately |