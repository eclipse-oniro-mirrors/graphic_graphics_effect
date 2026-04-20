# generate_metadata

Auto-generates C++ reflection metadata from `.params.in` files for runtime parameter access in Graphics Effect (GE) library.

## TL;DR

Run the following scripts after modifying/adding .params.in files.
```bash
# Generate reflection metadata
python tool/generate_metadata/gen_metadata.py

# Generate unified header (ge_effects_params.h)
python tool/generate_metadata/gen_effect_header.py
```

For creating new effects, you can use `tool/create_effect/create_effect.py` to scaffold code first.

---

## Overview

The GE library uses attribute-based parameter definitions. This tool parses `.params.in` files and generates:

| Output | Purpose |
|--------|---------|
| `ge_params_reflection.h/cpp` | Type-safe field accessors, constraint metadata, enum mappings |
| `ge_effects_params.h` | Unified include aggregating all `.params.in` files |

**Benefits**:
- Runtime parameter setting via string names
- Type-safe access with compile-time checking
- Automatic constraint validation (min/max, type conversion)
- Reduced boilerplate

---

## Quick Start

### Step 1: Define Parameters

Create a `.params.in` file in `include/effect/filter/` (or `shader/`, `mask/`, `shape/`):

```cpp
// include/effect/filter/ge_my_filter.params.in
struct [[ge::params(type=MY_FILTER, name="MY_FILTER")]] GEMyFilterParams {
    [[ge::prop("INTENSITY")]]
    float intensity;

    [[ge::prop(name="RADIUS", min=0.0, max=100.0)]]
    float radius;
};
```

For creating new effects, you can use `tool/create_effect/create_effect.py` to scaffold code boilerplate.

### Step 2: Add Enum Value

Add `MY_FILTER` to `include/core/ge_filter_type.h`:

```cpp
enum class GEFilterType : int32_t {
    NONE,
    // ... existing values ...
    MY_FILTER,  // Add here
};
```

### Step 3: Run Generator

```bash
python tool/generate_metadata/gen_metadata.py
```

### Step 4: Use Generated API

```cpp
#include "ge_params_reflection.h"

// Type-safe parameter setting
GEParamsMemberHelper::SetParamsMemberByTag(params,
    GEParamsMemberTag::MY_FILTER_INTENSITY, 0.5f);

// String-based lookup
auto tag = GEParamsMemberHelper::GEParamsMemberTagFromString("INTENSITY");

// Get filter type from tag
auto filterType = GEParamsMemberHelper::GetFilterTypeFromTag(tag);
```

---

## Parameter Syntax

### Struct Attribute

```cpp
struct [[ge::params(type=ENUM_VALUE, name="FILTER_NAME")]] ParamsStruct { ... };
```

| Parameter | Required | Description |
|-----------|----------|-------------|
| `type` | Yes | Must match a `GEFilterType` enum value (e.g., `KAWASE_BLUR`) |
| `name` | Yes | String name for runtime lookup |

### Field Attribute

```cpp
[[ge::prop("NAME")]]              // Single param - positional form
[[ge::prop(name="NAME", ...)]]    // Multiple params - key=value form
float field;                      // No attribute - auto-generates "Field" (PascalCase)
```

**Syntax Rules**:
- Single parameter: positional form `[[ge::prop("NAME")]]` allowed
- Multiple parameters: must use `name=` explicitly
- No attribute: name auto-generated from field name (PascalCase)

### Field Options

| Option | Example | Generated Effect |
|--------|---------|------------------|
| `min` / `max` | `min=0.0, max=1.0` | `GEParamsConstraintMinInfo` / `MaxInfo` with value |
| `array_accessor_length=N` | `array_accessor_length=3` | Generates FIELD0, FIELD1, FIELD2 tags |
| `array_accessor_type="Type"` | `array_accessor_type="float"` | Type for element accessors (default: field type) |
| `cast_from="Type"` | `cast_from=int` | `SetParamsMemberByTag` overload accepting that type |
| `custom="Transformer"` | `custom="PairToPointTransformer"` | Custom type conversion class |
| `alias="STRING"` | `alias="alpha"` | Alternative string-to-tag mapping (backward compat) |

### Examples

```cpp
// Simple property
[[ge::prop("RADIUS")]]
float radius;  // Generates: GEParamsMemberTag::FILTER_RADIUS

// With constraints
[[ge::prop(name="INTENSITY", min=0.0, max=1.0)]]
float intensity;  // Generates constraint metadata for validation

// Array accessor - splits into 3 tags
[[ge::prop(name="POSITION", array_accessor_length=3)]]
Vector3f position;  // Generates: POSITION0, POSITION1, POSITION2

// Type conversion
[[ge::prop(name="SIZE", cast_from=int)]]
float size;  // Accepts both float and int via SetParamsMemberByTag

// Alias for backward compatibility
[[ge::prop(name="OPACITY", alias="alpha")]]
float opacity;  // "OPACITY" and "alpha" both map to same tag

// Multiple props on same field
[[ge::prop(name="DESTINATION_PATCH")]]
[[ge::prop(name="ControlPoint", array_accessor_length=12)]]
std::array<int, 12> patch;  // Whole array + individual element accessors
```

---

## Generated API Reference

### GEParamsMemberTag Enum

```cpp
enum class GEParamsMemberTag : uint32_t {
    INVALID = 0,
    KAWASE_BLUR_RADIUS,
    BEZIER_WARP_DESTINATIONPATCH0,
    // ... all fields from all .params.in files
};
```

### GEParamsFieldAccessor Template

```cpp
template<GEParamsMemberTag Tag>
struct GEParamsFieldAccessor;

template<>
struct GEParamsFieldAccessor<GEParamsMemberTag::KAWASE_BLUR_RADIUS> {
    using ParamsType = GEKawaseBlurShaderFilterParams;
    using FieldType = int;
    static FieldType& Get(ParamsType& params);
    static void Set(ParamsType& params, const FieldType& value);
};
```

### GEParamsMemberHelper Class

```cpp
class GEParamsMemberHelper {
    // Tag ↔ String conversion
    static GEParamsMemberTag GEParamsMemberTagFromString(const std::string& str);
    static std::string GEParamsMemberTagToString(GEParamsMemberTag tag);

    // Tag → Filter type
    static GEFilterType GetFilterTypeFromTag(GEParamsMemberTag tag);

    // Type-safe setter (supports cast_from overloads)
    template<typename T>
    static void SetParamsMemberByTag(GEParamsParams& params, GEParamsMemberTag tag, const T& value);
};
```

### GEFilterParamsTypeInfo Template

```cpp
template<typename ParamsStruct>
struct GEFilterParamsTypeInfo {
    static constexpr GEFilterType ID;       // Links to GEFilterType enum
    static constexpr std::string_view FilterName;
};
```

### Constraint Metadata

```cpp
template<GEParamsMemberTag Tag>
struct GEParamsConstraintMinInfo {
    static constexpr bool HAS_MIN = true;
    static constexpr auto MIN = 0.0f;  // Value from min= parameter
};

template<GEParamsMemberTag Tag>
struct GEParamsConstraintMaxInfo { ... };

template<GEParamsMemberTag Tag>
struct GEParamsConstraintConvertInfo {
    static constexpr bool HAS_CONVERT = true;
    using CastFromType = int;  // Type from cast_from= parameter
};
```

---

## Configuration

`config.json` controls type normalization and blocked types:

```json
{
  "type_aliases": {
    "int": "int32_t",
    "GESDFShaderShape": "Drawing::GESDFShaderShape"
  },
  "member_setter_blocked_types": [
    {
      "type": "std::shared_ptr<Drawing::GESDFShaderShapeShape>",
      "reason": "Ambiguous overload with std::shared_ptr<Drawing::GEShaderShape>"
    }
  ]
}
```

| Section | Purpose |
|---------|---------|
| `type_aliases` | Normalize type names (e.g., `int` → `int32_t`) |
| `member_setter_blocked_types` | Exclude types from `SetParamsMemberByTag` generation to prevent ambiguous overloads |

---

## Implementation Details

### Directory Structure

```
tool/generate_metadata/
├── gen_metadata.py        # Main generator
├── gen_effect_header.py   # Header aggregator
├── cpp_parser.py          # C++ struct parser
├── cpp_tokenizer.py       # C++ tokenizer
├── attribute_parser.py    # Attribute parser ([[ge::...]])
├── value_parser.py        # Value expression parser
├── cli_utils.py           # CLI utilities
├── config.json            # Type aliases and blocked types
└── pyproject.toml         # Python project config
```

### Parsing Pipeline

1. **Tokenizer** (`cpp_tokenizer.py`): Handles C++ syntax including attributes `[[...]]`, raw strings, comments
2. **Parser** (`cpp_parser.py`): Extracts struct names, fields, and `[[ge::...]]` attributes
3. **Attribute Parser** (`attribute_parser.py`): Parses key=value syntax inside attributes
4. **Value Parser** (`value_parser.py`): Parses numeric/array default values for constraints
5. **Generator**: Outputs C++ templates and enums

### GEFilterType Integration

```
GEFilterType enum (ge_filter_type.h)
    ↓ [[ge::params(type=KAWASE_BLUR, ...)]]
GEFilterParamsTypeInfo<Params>::ID == GEFilterType::KAWASE_BLUR
    ↓
GEParamsMemberHelper::GetFilterTypeFromTag(tag) → GEFilterType
```

---

## Testing

Tests located in `test/tooltest/generate_metadata/`:

```
test/tooltest/generate_metadata/
├── syntax_valid_tests/     # Valid .params.in files
└── syntax_invalid_tests/   # Invalid .params.in files (error handling)
```

Each test can have a `.params.in.json` specifying expected results.

```bash
# Run tests
python test/tooltest/generate_metadata/run_parser_tests.py

# Save current output as expected
python test/tooltest/generate_metadata/run_parser_tests.py --save-expected

# Show all details
python test/tooltest/generate_metadata/run_parser_tests.py --show-all-details
```

---

## Command-Line Options

### gen_metadata.py

```bash
python tool/generate_metadata/gen_metadata.py \
    --params-dirs include/effect/filter include/effect/shader \
    --output-file include/effect/ge_params_reflection.h \
    --output-cpp-file src/effect/ge_params_reflection.cpp \
    --config-file tool/generate_metadata/config.json \
    --effect-dirs include/effect
```

| Option | Default | Description |
|--------|---------|-------------|
| `--params-dirs` | `include/effect/{filter,mask,shader,shape}` | `.params.in` file directories |
| `--output-file` | `include/effect/ge_params_reflection.h` | Output header path |
| `--output-cpp-file` | `src/effect/ge_params_reflection.cpp` | Output cpp path |
| `--config-file` | `tool/generate_metadata/config.json` | Config file path |
| `--effect-dirs` | `include/effect` | Effect headers for `DECLARE_GEFILTER_TYPEFUNC` scanning |

### gen_effect_header.py

```bash
python tool/generate_metadata/gen_effect_header.py
```

Generates `include/effect/ge_effects_params.h` - a unified include aggregating all `.params.in` files.

---

## Dependencies

- Python 3.8+
- No external dependencies (stdlib only)
- clang-format (optional, auto-formats generated code)

---

## Notes

- Generated files contain `// AUTO GENERATED by ...` header
- Do not manually edit generated files (will be overwritten on next run)
- Parser focuses on GE-specific patterns; complex C++ constructs (nested templates, function pointers) may not parse correctly
