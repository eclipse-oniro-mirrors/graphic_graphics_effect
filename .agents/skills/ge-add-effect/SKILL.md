---
name: ge-add-effect
description: |
  Add, review, or complete new visual effects (filter, shader, mask, shape) in the OpenHarmony graphics_effect project. Triggers on: "add effect", "new filter/shader/mask/shape", "graphics_effect", "review params.in", "fix params.in", "run code generation", "code generation", ".params.in file", SkSL shader creation — even without explicit project name. Does NOT trigger on: generic shader tutorials, OpenGL/Vulkan questions, other OpenHarmony subsystems, pure build debugging unrelated to effect creation, or creating new effect types / adding new base classes / modifying core effect infrastructure (these are architectural changes, not effect additions).
---

# Task and Boundaries

This skill handles adding new visual effects to the graphics_effect (GE) project — a component of OpenHarmony's graphics stack providing shader-based visual effects (blur, shadow, gradient, etc.).

**Path context**: All file paths below are relative to the **GE project root** — the directory containing `include/`, `src/`, `tool/`, and `test/`. Locate it by finding `tool/create_effect/create_effect.py`. Scripts must be run from this directory. The build command (`hb build`) runs from the OpenHarmony source root (the parent directory above `foundation/`).

**What this skill covers**:
- Creating a new effect from scratch (scaffold → params → enum → implementation → build → test)
- Reviewing and fixing an existing `.params.in` file, then completing the workflow
- Running code generation tools after parameter changes
- Debugging common mistakes that cause compile failures in the graphics_effect code generation process
- Writing unit tests for new effects

**What this skill does NOT cover**:
- Writing SkSL shader algorithms from scratch (the skill tells you where to put shader code, not how to design algorithms)
- Modifying existing effects beyond their `.params.in` parameters
- Debugging general build issues unrelated to effect creation
- Creating a new effect type beyond filter/shader/mask/shape — this requires core infrastructure changes (new base class, namespace, virtual method system, code-gen support) not covered by this workflow

**Input**: Effect name (snake_case), effect type (filter/shader/mask/shape), parameter specifications.
**Output**: A complete, compilable, tested new effect added to the project with all generated metadata updated.

---

# Initial Checks

Before starting, determine the entry mode:

| User has | Mode | First action |
|----------|------|-------------|
| Nothing — starting fresh | **Full** | Step 1: scaffold via create_effect.py |
| Only `.params.in` written | **Existing params** | Step A: review the file first |
| `.params.in` + `.h` + `.cpp` but no gen output | **Existing code** | Step 6: run gen tools |
| Only an issue/bug to fix | **Fix** | Read `references/pitfalls.md` only. **Do NOT load** the other two reference files — a debug case doesn't need parameter syntax or type comparison tables. |

Then confirm required inputs with the user:

1. **Effect name** — snake_case identifier (e.g., `my_blur`, `posterize`). Derive from filename if already given.
2. **Effect type** — filter (image processing), shader (direct effect), mask (coverage shader), shape (SDF geometry). Infer from directory or user context.
3. **Parameters** — either the user describes them, or read from an existing `.params.in`. For each param: name, C++ type, default value, optional min/max range, optional cast_from type.

If any required input is missing, ask before proceeding. If the user doesn't specify parameters, propose defaults based on effect type (blur → radius + intensity, color → Vector4f) and get confirmation.

---

# Decision Principles

Before any effect work, resolve three questions — wrong answers cascade into broken builds:

1. **Entry mode** — Fresh start / existing params / existing code / fix only? → determines first action (see Initial Checks table)
2. **Effect type** — filter / shader / mask / shape? → determines base class, namespace, key virtual method, and file naming (see Step 1 type table)
3. **Closest reference** — Which existing effect in the same type directory is most similar? → read its `.h`/`.cpp` as your implementation template. If no similar effect exists, use the simplest effect of that type as a minimal reference — it still shows the correct namespace, constructor pattern, and virtual method signature.

The most dangerous mistake is choosing the wrong type: it cascades into wrong namespace, wrong virtual method, wrong file naming, and wrong constructor signature. Confirm type before writing any code.

---

# Execution Strategy

## Full Mode: New Effect from Scratch

```
1. Scaffold       → create_effect.py generates .params.in, .h, .cpp
2. Design + write → Design parameters then define them in .params.in
3. Add enum       → Add value to GEFilterType before MAX
4. Edit code      → Implement .h members + .cpp shader logic
5. Update GN      → Add .cpp to BUILD.gn sources
6. Generate       → Run gen_metadata.py + gen_effect_header.py
7. Build          → hb build graphics_effect -i
8. Test           → Write unit test + build test target
```

## Existing Params Mode: Review then Complete

```
A. Review    → Validate .params.in (see Review Checklist below)
3. Add enum  → Add value to GEFilterType before MAX
4. Edit code → Scaffold .h/.cpp with --no-enum if missing, then implement
5. Update GN → Add .cpp to BUILD.gn sources
6. Generate  → Run gen_metadata.py + gen_effect_header.py
7. Build     → hb build graphics_effect -i
8. Test      → Write unit test + build test target
```

## Step Details

### Step 1: Scaffold (Full Mode Only)

```bash
python tool/create_effect/create_effect.py <name> <type>
```

Effect types and their base classes:

| Type | Base Class | Namespace | Class Pattern | Use Case |
|------|-----------|-----------|--------------|----------|
| `filter` | `GEShaderFilter` | `Rosen` | `GE{Name}ShaderFilter` | Image processing |
| `mask` | `GEShaderMask` | `Drawing` | `GE{Name}ShaderMask` | Gradient/image masks |
| `shader` | `GEShader` | `Rosen` | `GE{Name}Shader` | Lighting, material effects |
| `shape` | `GESDFShaderShape` | `Drawing` | `GE{Name}SDFShaderShape` | SDF borders/shadows/clips |

The tool auto-adds the enum to `ge_filter_type.h`. If it fails, handle manually in Step 3.

**MANDATORY**: Read `references/effect-types.md` if you need per-type method signatures, namespace conventions, or file naming patterns. **Do NOT load** `references/params-syntax.md` or `references/pitfalls.md` for this step.

### Step 2: Design Parameters (Before Writing .params.in)

Before writing syntax, decide what parameters your effect needs. A well-designed parameter set makes the shader flexible while keeping the API intuitive:

1. **What uniform inputs does the shader need?** — Each shader uniform should map to a `.params.in` field. Think about what the GPU needs: radius, intensity, color vectors, direction angles, texture inputs.
2. **Which values need runtime clamping?** — Visual-sensitive params (blur radius, intensity, color components) should have `min`/`max` to prevent garbage output from out-of-range callers. Values with natural bounds (e.g., color 0-1) always need clamping. Values where any range is valid (e.g., offsets) can skip `min`/`max`.
3. **Does any parameter need `cast_from` for type conversion?** — Ask the user first: "Should callers pass a different type than what the shader stores internally?" Only add `cast_from` when the answer is yes (e.g., caller sends `int` index but shader needs `float` weight). Never use `cast_from` when types match — that creates duplicate switch cases and compile errors. When `cast_from` is needed for non-trivial conversions (e.g., `std::pair` → `Vector2f`), it must be paired with `custom` transformer — `custom` without `cast_from` triggers a compile error.

### Step 2b: Write Parameters (.params.in)

Replace the skeleton struct with actual parameter definitions:

```cpp
struct [[ge::params(type=YOUR_ENUM, name="YourName")]] GEYourParams {
    float yourParam = 50.0f;  // Auto-generates "YourName_YourParam" tag
};
```

**Key rules** — the most common source of errors:
- `type` must match the `GEFilterType` enum value
- **Prefer omitting `name=` on `[[ge::prop]]`** — auto-generation produces `StructName_PascalCaseField` (e.g., `name="FrostedGlassBlur"` + field `radius` → `FrostedGlassBlur_Radius`). Only add explicit `name=` when the auto-generated name would be misleading or doesn't match the desired exposed string name. Other options like `min`, `max`, `cast_from` determine syntax form (key-value vs positional), not whether you need `name=`
- **Positional syntax** `[[ge::prop("TagName")]]` is only viable when `name` is the sole option — it's shorthand for `name="TagName"`. When any other option is present (`min`, `max`, `cast_from`, `custom`, `array_accessor_length`), you must use key-value syntax — but you can still omit `name=` within it: `[[ge::prop(min=0.0, max=100.0)]]` auto-generates the tag
- `cast_from` is **only** for type conversion where the caller type differs from the field type — never when both are the same type (causes duplicate switch case). **Ask the user before adding `cast_from`** — confirm they want callers to pass a different type
- `cast_from` for non-trivial conversions (e.g., `std::pair` → `Vector2f`) must be paired with `custom` transformer — `custom` without `cast_from` triggers a compile error
- `min`/`max` on numeric fields generate constraint metadata for runtime validation

**MANDATORY**: Read `references/params-syntax.md` entirely before writing a new `.params.in` from scratch. **Do NOT load** `references/effect-types.md` or `references/pitfalls.md` for this step.

### Step 3: Add Enum Value

Add before `MAX` in `include/core/ge_filter_type.h`:

```cpp
YOUR_ENUM,   // Before MAX
MAX,
```

Verify if `create_effect.py` auto-added it. If auto-add failed or placed wrong, fix manually.

### Step 4: Edit Implementation

Before implementing, ask yourself:
- **What uniform names and types does the shader need?** — derive from the .params.in fields you defined in Step 2 (each param field → one shader uniform)
- **Which existing effect is closest to this one?** — find it in the same type category, read its .h/.cpp as reference
- **What is the output?** — filters produce modified images, shaders draw directly, masks/shapes return shader objects

**Header (.h)**: Add private member variables for each parameter:
```cpp
float yourParam_ = 50.0f;
```

**Implementation (.cpp)**:
1. **Constructor** — MUST: (1) read params fields via direct access, (2) clamp any numeric field that has `min`/`max` using `std::clamp(params.field, min, max)`, (3) store each as a private member
2. **GetEffect()** — Define SkSL shader as static string, create `RuntimeEffect`
3. **Key virtual method** — varies by type: filters → `OnProcessImage()`, shaders → `MakeDrawingShader()`, masks/shapes → `GenerateDrawingShader()`

The scaffolded `.cpp` has a complete `OnProcessImage` skeleton for filters. Add your uniforms:
```cpp
builder.SetUniform("yourParam", yourParam_);
```

**MANDATORY**: Read `references/effect-types.md` for per-type method signatures, namespace, and file naming. **Do NOT load** `references/params-syntax.md` for this step.

### Step 5: Update BUILD.gn

Add the `.cpp` path alphabetically in the `sources` list:
```python
"src/effect/filter/ge_your_effect_shader_filter.cpp",
```

### Step 6: Run Code Generation

**clang-format must be installed** — without it, generated files produce noisy diffs that obscure real changes.

```bash
python tool/generate_metadata/gen_metadata.py
python tool/generate_metadata/gen_effect_header.py
```

These regenerate 3 files: `ge_effects_params.h`, `ge_params_reflection.h`, `ge_params_reflection.cpp`.

### Step 7: Build and Verify

```bash
# From OpenHarmony root, NOT GE project root
hb build graphics_effect -i
```

Verify all targets compile and `libgraphics_effect.z.so` is produced.

### Step 8: Write Unit Test

Create test file at `test/unittest/ge_<name>_test.cpp`. Use the closest existing test as a template — find it by matching your effect type:

| Effect type | Reference test file |
|------------|--------------------|
| filter | `test/unittest/ge_kawase_blur_shader_filter_test.cpp` |
| shader | `test/unittest/ge_border_light_shader_test.cpp` |
| mask | `test/unittest/ge_frame_gradient_shader_mask_test.cpp` |
| shape | `test/unittest/ge_sdf_rrect_shader_shape_test.cpp` |

Read the reference test, copy its structure, and adapt for your effect. Minimum coverage:

1. **Constructor with valid params** — verify effect object creation
2. **SetParam round-trip** — set a param value, verify it's stored correctly (with clamping if applicable)
3. **Key method returns non-null** — verify the effect's key method produces a valid result:
 `GetEffect`/`OnProcessImage` for filter, `MakeDrawingShader` in shader, `GenerateDrawingShader` in mask, `GenerateDrawingShader`/`GetSDFShapeType` in shape

Add test to `test/unittest/BUILD.gn` sources list. Build and run:
```bash
hb build graphics_effect -t
```

### Step A: Review Existing .params.in (Existing Params Mode)

Read the file and check this checklist. Report issues and ask whether to fix or proceed.

| # | Check | Why It Matters |
|---|-------|---------------|
| 1 | `[[ge::params(type=..., name=...)]]` present, `type` is valid enum-style identifier | gen_metadata.py won't parse the struct without it |
| 2 | Each field either has `[[ge::prop(...)]]` or accepts auto-tag | Auto-generated tags use PascalCase (`StructName_PascalCaseField`), not ALL_CAPS. Prefer omitting `name=` unless the auto-generated name would be misleading |
| 3 | No `cast_from` where type equals field type | Duplicate switch cases → compile error. Most common params.in mistake. Also: `cast_from` should only be added when the user explicitly wants callers to pass a different type |
| 4 | `cast_from` paired with `custom` for non-trivial conversions | `custom` without `cast_from` triggers compile error. `std::pair` → `Vector2f` needs a transformer, not bare cast |
| 5 | Positional syntax used only when `name` is sole option | `[[ge::prop("TagName")]]` is shorthand for `name="TagName"` — invalid when other options like `min`, `max`, or `cast_from` are present |
| 6 | `min`/`max` values valid for the type | Negative min on unsigned fields is wrong |
| 7 | Defaults specified and within min/max range | Missing defaults → uninitialized values |
| 8 | Struct named `GE<ClassName>Params` | Wrong naming breaks DECLARE_GEFILTER_TYPEFUNC |
| 9 | `type` enum value matches what will be added to `GEFilterType` | Mismatch breaks type-info specialization |

**MANDATORY**: Read `references/params-syntax.md` during review. **Do NOT load** `references/effect-types.md` or `references/pitfalls.md` for this step — focus on syntax correctness, not type comparison or debugging.

---

# Prohibited Practices

| Practice | Why It's Wrong | What to Do Instead |
|----------|---------------|-------------------|
| Use `cast_from` with same type as the field | Duplicate switch cases → compile error | Remove `cast_from`; use auto-generated tag or `[[ge::prop("TagName")]]` when types match |
| Add `cast_from` without confirming user wants it | Unnecessary type conversion complicates the API | Ask the user: "Should callers pass a different type?" If no, omit `cast_from` |
| Use `custom` without `cast_from` | Compile error — `custom` only works with type conversion | Only use `custom` when `cast_from` is also specified |
| Use positional syntax with multiple options | `[[ge::prop("TagName", min=0.0)]]` is invalid — positional only works for single `name` option | Use key-value syntax: `[[ge::prop(name="TagName", min=0.0)]]` |
| Add explicit `name=` when auto-generation suffices | Adds verbosity without benefit; auto-generated PascalCase is the convention | Omit `name=` unless the auto-generated name would be misleading or doesn't match the desired exposed string name |
| Skip clang-format installation | Generated files produce 100s of lines of noise diff | Install clang-format before running gen tools |
| Add enum after `MAX` | Breaks type-info dispatch — `MAX` is a sentinel | Always place new values before `MAX` |
| Skip gen_metadata.py + gen_effect_header.py | Reflection files won't include new parameters | Run both tools after any `.params.in` change |
| Run gen tools before finishing .params.in edits | Generates stale metadata that doesn't match final definitions | Finish editing `.params.in` first, then run gen tools |
| Forget BUILD.gn source entry | Link error for missing symbols | Add `.cpp` to sources list alphabetically |
| Skip unit tests for new effects | Silent bugs in shader logic or parameter clamping | Write at least constructor, SetParam, and GetEffect tests |
| Manually edit generated files (ge_params_reflection.h/cpp, ge_effects_params.h) | Changes overwritten on next gen tool run — all manual edits silently lost | Edit `.params.in` source files instead, then re-run gen tools |
| Rely on min/max constraints in constructor | Constraints only apply via the SetParam API, not on direct field access | Clamp values explicitly in constructor: `std::clamp(params.field, min, max)` |
| Attempt to add an effect outside the four existing types | No corresponding base class, namespace, or code-gen support exists — will produce broken code that cannot compile | Inform the user this requires architectural changes beyond this skill's scope |

For detailed root cause analysis and real examples, read `references/pitfalls.md`.

---

# Exceptions and Fallbacks

| Situation | What to Do |
|-----------|-----------|
| `create_effect.py` fails to auto-add enum | Manually add enum before `MAX` in `ge_filter_type.h` — the tool prints "Manual action required" |
| `gen_metadata.py` reports a parse error on your `.params.in` | Check syntax: missing `[[ge::params]]` attribute, invalid attribute format, or unclosed brackets. Read `references/params-syntax.md` |
| `.h` / `.cpp` missing but `.params.in` exists | Run `create_effect.py <name> <type> --no-enum` to scaffold code files without re-adding the enum |
| User provides incomplete parameter spec | Propose defaults based on effect type (blur → radius + intensity, color → Vector4f), ask for confirmation before proceeding |

For build failure diagnosis (duplicate case value, undefined symbol, clang-format warning, namespace errors, etc.) and workflow fallback scenarios, see `references/pitfalls.md` → Build Failure Diagnosis section.

---

# References

Load conditionally — only the reference files needed for your current task, not all at once.

| File | Load When | Do NOT Load When | Content |
|------|------------|-------------------|---------|
| `references/params-syntax.md` | Writing or reviewing `.params.in` | Fixing build error (use pitfalls.md) or choosing effect type (use effect-types.md) | Full attribute syntax, field options, cast_from rules, examples |
| `references/effect-types.md` | Choosing effect type or implementing per-type methods | Writing `.params.in` (use params-syntax.md) or debugging build error (use pitfalls.md) | Base classes, namespaces, file naming, key virtual methods per type |
| `references/pitfalls.md` | Debugging gen tool errors or build failures | Writing `.params.in` (use params-syntax.md) or choosing effect type (use effect-types.md) | Common mistakes, root cause analysis, build failure diagnosis, workflow fallbacks |
