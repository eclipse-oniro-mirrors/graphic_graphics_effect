# .params.in Syntax Reference

Complete reference for defining effect parameters in `.params.in` files.

## Table of Contents

1. [Struct Attribute](#struct-attribute)
2. [Field Attribute](#field-attribute)
3. [Field Options](#field-options)
4. [cast_from Rules](#cast_from-rules)
5. [Examples](#examples)
6. [Auto-generated Tag Naming](#auto-generated-tag-naming)

---

## Struct Attribute

Every `.params.in` file defines one struct with the `[[ge::params]]` attribute:

```cpp
struct [[ge::params(type=ENUM_VALUE, name="DisplayName")]] GEYourParams {
    // fields...
};
```

| Parameter | Required | Description |
|-----------|----------|-------------|
| `type` | Yes | Must match a `GEFilterType` enum value in `ge_filter_type.h` |
| `name` | Yes | String name for runtime lookup |

**Naming convention**: Params struct name = `GE<ClassName>Params`.

---

## Field Attribute

Three syntax forms, ordered by preference:

**No attribute** (preferred — auto-generates `StructName_PascalCaseField`):
```cpp
float radius;  // Auto-generates "FrostedGlassBlur_Radius" from struct name= + PascalCase field name
```
Use this when you don't need `min`, `max`, `cast_from`, or other options. It produces PascalCase names, matching the existing codebase convention.

**Positional** (single tag name, no other options):
```cpp
[[ge::prop("FrostedGlassBlur_Radius")]]
float field;
```
Only viable when `name` is the sole option — this is shorthand for `name="FrostedGlassBlur_Radius"`. **Invalid** when other options like `min`, `max`, `cast_from`, or `array_accessor_length` are present.

**Key-value** (other options, name= optional):
```cpp
// With name= omitted — auto-generates tag:
[[ge::prop(min=0.0, max=100.0)]]
float field;

// With explicit name= — only when auto-gen would be misleading:
[[ge::prop(name="FrostedGlassBlur_Radius", min=0.0, max=100.0)]]
float field;
```
Required syntax when any option beyond `name` is present (`min`, `max`, `cast_from`, `custom`, `array_accessor_length`, or `alias`). Within key-value syntax, you can still omit `name=` — the tag will be auto-generated. Only add explicit `name=` when the auto-generated name would be misleading or doesn't match the desired exposed string name.

**Naming convention**: Auto-generated tags use PascalCase (`StructName_PascalCaseField`). Explicit `name=` also uses PascalCase format (`EffectName_ParamTag`, e.g., `FrostedGlassBlur_Radius`). **Avoid ALL_CAPS prop names** — the existing codebase uses PascalCase exclusively for new effects.

---

## Field Options

| Option | Example | Effect |
|--------|---------|--------|
| `min` | `min=0.0` | Runtime validation constraint — only enforced via SetParam API, not on direct field access |
| `max` | `max=100.0` | Runtime validation constraint — only enforced via SetParam API, not on direct field access |
| `cast_from` | `cast_from="int32_t"` | Creates an additional setter accepting that type, which converts to the field type |
| `custom` | `custom="PairToVector2fTransformer"` | Transformer class for non-trivial `cast_from` conversion |
| `array_accessor_length` | `array_accessor_length=3` | Splits into TAG0, TAG1, TAG2 element accessors |
| `array_accessor_type` | `array_accessor_type="float"` | Type for element accessors (default: field type) |
| `alias` | `alias="alpha"` | Alternative string-to-tag mapping (backward compat) |

---

## cast_from Rules

`cast_from` specifies a type that callers can pass which differs from the actual field type. The code generator creates an additional setter that accepts the `cast_from` type and converts it to the field type.

**Three critical rules**:

1. **`cast_from` must specify a different type** than the field itself. If both are the same, the generator produces duplicate switch cases, causing a compile error.
2. **Ask the user before adding `cast_from`** — confirm they want callers to pass a different type. If the user doesn't need a different type, omit `cast_from`.
3. **`custom` must be paired with `cast_from`** — `custom` without `cast_from` triggers a compile error. Use `custom` for non-trivial conversions (e.g., `std::pair` → `Vector2f`).

**Valid uses** (different types):
```cpp
// Accept int32_t, store as float
[[ge::prop(name="MyBlur_Size", cast_from="int32_t")]]
float size;

// Accept pair, convert to Vector2f via transformer
[[ge::prop(name="MyEffect_Offset", cast_from="std::pair<float,float>", custom="PairToVector2fTransformer")]]
Vector2f offset;

// Accept base class pointer, store as derived
[[ge::prop(name="MyEffect_Shape", cast_from="std::shared_ptr<GEShaderShape>")]]
std::shared_ptr<Drawing::GESDFShaderShape> shape;
```

**Invalid use** (same type — causes duplicate switch case):
```cpp
// WRONG: cast_from type equals field type
[[ge::prop(cast_from="Drawing::Path")]]
Drawing::Path path;  // Compile error: duplicate case value
```

Fix: Remove `cast_from` and use a simple prop tag instead:
```cpp
[[ge::prop("SDFPathShape_Path")]]
Drawing::Path path;
```

---

## Examples

### Simple property with constraints

```cpp
struct [[ge::params(type=KAWASE_BLUR, name="KawaseBlur")]] GEKawaseBlurShaderFilterParams {
    [[ge::prop(name="KawaseBlur_Radius", min=0.0)]]
    int radius = 0;
};
```

### Type conversion with transformer

```cpp
[[ge::prop(name="MyEffect_Offset", cast_from="std::pair<float,float>", custom="PairToVector2fTransformer")]]
Vector2f offset = Vector2f(0.0f, 0.0f);
```

The transformer class must provide a static `Transform` method with the following signature:

```cpp
struct MyTransformer {
    // Must be static method
    // Return bool: true for success, false for failure (failed transform = param silently not set)
    static bool Transform(const FromType& value, ToType& out) {
        // ... conversion logic
        out = /* converted value */;
        return true;  // or false for conversion failure
    }
};
```

The `Transform` method is called during the SetParam pipeline. If `Transform()` returns `false`, the parameter value is silently discarded — the field is not modified.

### Array accessor

```cpp
[[ge::prop(name="BezierWarp_DestinationPatch", array_accessor_length=12)]]
std::array<int, 12> patch;
```

Generates: `BezierWarp_DestinationPatch0` through `BezierWarp_DestinationPatch11` tags for individual element access, plus `BezierWarp_DestinationPatch` for the whole array.

### Multiple props on same field

```cpp
[[ge::prop(name="BezierWarp_DestinationPatch")]]
[[ge::prop(name="BezierWarp_ControlPoint", array_accessor_length=12)]]
std::array<int, 12> patch;
```

### Alias for backward compatibility

```cpp
[[ge::prop(name="MyEffect_Opacity", alias="alpha")]]
float opacity;
```

Both "MyEffect_Opacity" and "alpha" map to the same param at runtime. Use `alias` when renaming a tag to preserve compatibility with callers that still use the old string name. **Never use `alias` to create a duplicate tag for a different field** — string conflicts cause gen tool failure.

---

## Auto-generated Tag Naming

When no explicit `name=` is provided, the tag is auto-generated as `StructName_PascalCaseField` using the `name=` from `[[ge::params]]` as prefix. E.g., struct `name="FrostedGlassBlur"` with field `radius` → `FrostedGlassBlur_Radius`. Collision risk is low since different struct names produce different prefixes.

**Best practice**: Prefer omitting `name=` — auto-generation produces PascalCase names matching the codebase convention. Only add explicit `name=` when the auto-generated name would be misleading or doesn't match the desired exposed string name (e.g., you want callers to use `"Radius"` but auto-gen would produce `"FrostedGlassBlur_Radius"`). Other options like `min`, `max`, `cast_from` determine syntax form (key-value vs positional), not whether you need `name=` — you can omit `name=` even in key-value syntax.

**Note on `name=` format**: The struct's `name=` parameter uses PascalCase (e.g., `name="FrostedGlassBlur"`), so auto-generated tags naturally produce PascalCase. Some older files use ALL_CAPS `type` values (e.g., `type=KAWASE_BLUR`) with PascalCase `name` — the auto-generation still produces PascalCase tags because it derives from `name=`, not `type=`.

---

## Type Aliases and Blocked Types

The `tool/generate_metadata/config.json` controls type normalization:

```json
{
  "type_aliases": {
    "int": "int32_t",
    "GESDFShaderShape": "Drawing::GESDFShaderShape"
  },
  "member_setter_blocked_types": [
    {
      "type": "std::shared_ptr<Drawing::GESDFShaderShape>",
      "reason": "ambiguous setter with std::shared_ptr<Drawing::GEShaderShape>"
    }
  ]
}
```

- `type_aliases`: Normalize type names so `int` and `int32_t` are treated as the same type
- `member_setter_blocked_types`: Prevent ambiguous setter overloads when a subclass pointer would match both base and derived types (e.g., when `std::shared_ptr<Derived>` and `std::shared_ptr<Base>` both satisfy the same setter signature)

**When to add a blocked type**: If your effect uses `std::shared_ptr<Derived>` where a base class `std::shared_ptr<Base>` setter already exists, the generated code creates two setters that both accept `shared_ptr<Base>` — causing compile errors. Add the derived type to `blocked_types` to suppress its setter. The field is still accessible via direct params access, but cannot be set through the SetParam API.