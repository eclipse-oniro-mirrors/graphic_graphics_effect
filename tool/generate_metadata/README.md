# EffectGen - Graphics Effect Metadata Generator

EffectGen is a code generation tool for the OpenHarmony Graphics Effect (GE) library. It automatically generates C++ reflection metadata from parameter definition files (`.params`), enabling runtime introspection and type-safe parameter access for visual effects.

## TL;DR
**Create new effect params file:**
```bash
# Use the creator tool to scaffold a new effect
python tool/creator/create_ge_effect.py my_effect filter 
```

**Generate metadata from .params files:**
```bash
# Default: scan standard effect directories
python tool/effectgen/gen_metadata.py
```

**Generate effect params unified include header:**
```bash
python tool/effectgen/gen_effect_header.py
```
---

## Table of Contents

- [Quick Start Guide](#quick-start-guide)
- [Usage](#usage)
- [Parameter Definition Format](#parameter-definition-format)
- [Advanced Topics](#advanced-topics)
- [Implementation Details](#implementation-details)
- [Testing](#testing)
- [Build Integration](#build-integration)

---

## Quick Start Guide

### What EffectGen Does

The GE library uses a custom attribute-based system to define effect parameters. EffectGen parses these definitions and generates:

- **Reflection metadata** (`ge_params_reflection.h/cpp`) - Type-safe field accessors, constraint metadata, and enum mappings
- **Unified header** (`ge_effects_params.h`) - Aggregates all parameter definitions
- **Filter type info** (`ge_filter_type_info.h`) - Links filter classes to their parameter types

This enables:
- Runtime parameter setting by string names
- Type-safe parameter access with compile-time checking
- Automatic constraint validation (min/max, type conversion)
- Reduced boilerplate code

### Creating a New Effect

The easiest way to create a new effect is to use the `create_ge_effect.py` tool:

```bash
# Create a filter effect with parameters
python tool/creator/create_ge_effect.py my_blur filter
```

This generates:
- `.params` file with parameter definitions
- `.h` header file with effect class
- `.cpp` implementation file

**Note**: After creating the effect, you need to:
1. Add the enum value to `include/core/ge_filter_type.h`
2. Run `python tool/effectgen/gen_metadata.py` to generate reflection metadata

### Basic Parameter Definition

```cpp
// include/effect/filter/ge_my_filter.params
struct [[ge::params(type=MY_FILTER, name="MY_FILTER")]] GEMyFilterParams {
    [[ge::prop("INTENSITY")]]
    float intensity;

    [[ge::prop(name="RADIUS", min=0.0, max=100.0)]]
    float radius;
};
```

**Key Points:**
- `[[ge::params]]` is mandatory and `type=` must match a value in `GEFilterType` enum
- If `[[ge::prop]]` is omitted, the property name is auto-generated from the field name (converted to PascalCase)
- Specify `[[ge::prop]]` only when you want to control the code generation or add constraints.
    - Single parameter: `[[ge::prop("NAME")]]` (positional form allowed)
    - Multiple parameters: `[[ge::prop(name="NAME", ...other...)]]` (must use `name=`)


---

## Usage

### gen_metadata.py

Generates C++ reflection metadata from `.params` files.

**Features:**
- Parses `.params` files with custom `[[ge::params]]` and `[[ge::prop]]` attributes
- Generates `GEParamsMemberTag` enum for all fields
- Creates type-safe field accessors via `GEParamsFieldAccessor<>` template
- Generates constraint metadata (min/max, type conversion)
- Creates string-to-enum mappings for runtime lookup
- Scans for `DECLARE_GEFILTER_TYPEFUNC` macros to link filters to params

**Generated Files:**
- `include/effect/ge_params_reflection.h` - Header with type traits and declarations
- `src/effect/ge_params_reflection.cpp` - Implementation of helper functions

**Command-line Options:**
- `--params-dirs`: Directories containing `.params` files (default: include/effect/{filter,mask,shader,shape})
- `--output-file`: Output header path (default: include/effect/ge_params_reflection.h)
- `--output-cpp-file`: Output cpp path (default: src/effect/ge_params_reflection.cpp)
- `--config-file`: Config file path (default: tool/effectgen/config.json)
- `--effect-dirs`: Directories containing effect headers (for DECLARE_GEFILTER_TYPEFUNC scanning)

### gen_effect_header.py

Generates `ge_effects_params.h` that includes all `.params` files.

**Purpose:**
- Aggregates all parameter definitions into a single header
- Provides a unified include for effect parameter types
- Designed to be manually editable if needed

**Generated File:**
- `include/effect/ge_effects_params.h`

---

## Parameter Definition Format

`.params` files define parameter structures with custom attributes. They support standard C++ syntax for struct definitions, but have limitations on complex type nesting (e.g., deeply nested templates, function pointers, etc.).

### Struct Attributes

`[[ge::params(type=ENUM_TYPE, name="FILTER_NAME")]]`

- `type`: Enum value from `GEFilterType` enum (defined in `include/core/ge_filter_type.h`)
  - Must be a valid enum value (e.g., KAWASE_BLUR, BEZIER_WARP, AIBAR)
  - Links the params struct to its corresponding filter type
- `name`: String name for the filter (used for string-based lookup)

**Example:**
```cpp
// GEFilterType enum (from ge_filter_type.h):
// enum class GEFilterType : int32_t {
//     NONE,
//     KAWASE_BLUR,
//     BEZIER_WARP,
//     AIBAR,
//     ...
// };

struct [[ge::params(type=KAWASE_BLUR, name="KAWASE_BLUR")]] GEKawaseBlurShaderFilterParams {
    // ...
};
```

### Field Attributes

`[[ge::prop(...options...)]]`

**Syntax Rules:**
- The first parameter can be a positional string `"PROPERTY_NAME"` only if it's the **only** parameter
- When multiple parameters are present, `name=` must be used explicitly
- All other parameters must use `key=value` syntax
- If `[[ge::prop]]` is omitted entirely, the property name is auto-generated from the field name (converted to PascalCase)

**Basic Usage:**
```cpp
// Single parameter - positional form allowed
[[ge::prop("RADIUS")]]
float radius;

// Multiple parameters - must use name=
[[ge::prop(name="INTENSITY", min=0.0, max=1.0)]]
float intensity;

// No prop attribute - name auto-generated from field name
float opacity;  // Generates: Opacity (PascalCase of "opacity")
```

**Parameter Options and Their Effects:**

| Parameter | Type | Effect on Generated Code |
|-----------|-------|------------------------|
| `name="STRING"` | string | Property name for string-based lookup (required when multiple params) |
| `array_accessor_length=N` | int | Generates N separate tags: FIELD0, FIELD1, ..., FIELD(N-1) |
| `array_accessor_type="Type"` | string | Type for array element accessors (defaults to field type) |
| `alias="STRING"` | string | Alternative name for string-to-tag mapping (backward compatibility) |
| `cast_from="Type"` | string | Generates SetParamsMemberByTag overload accepting this type |
| `custom="Transformer"` | string | Custom transformer class for type conversion |
| `min=Value` | numeric | Generates GEParamsConstraintMinInfo with HAS_MIN=true |
| `max=Value` | numeric | Generates GEParamsConstraintMaxInfo with HAS_MAX=true |

**Examples:**

```cpp
// Simple property (single parameter - positional form)
[[ge::prop("RADIUS")]]

float radius;
// Generates: GEParamsMemberTag::KAWASE_BLUR_RADIUS
// Property name: "RADIUS"

// Array accessor - splits field into 3 separate properties
[[ge::prop(name="CONTROL_POINT", array_accessor_length=3)]]
Vector3f position;
// Generates: CONTROL_POINT0, CONTROL_POINT1, CONTROL_POINT2
// Each accessor uses Vector3f::value_type (float)

// With constraints - generates constraint metadata
[[ge::prop(name="INTENSITY", min=0.0, max=1.0)]]
float intensity;
// Generates: GEParamsConstraintMinInfo<INTENSITY_TAG> with MIN=0.0
// Generates: GEParamsConstraintMaxInfo<INTENSITY_TAG> with MAX=1.0

// Type conversion - generates overload for int
[[ge::prop(name="SIZE", cast_from=int)]]
float size;
// Generates: SetParamsMemberByTag(params, tag, int) overload
// Generates: GEParamsConstraintConvertInfo with CastFromType=int

// Custom transformer with cast_from
[[ge::prop(name="POINT", cast_from="std::pair<float,float>",
           custom="PairToPointTransformer")]]
Drawing::Point point;
// Generates: SetParamsMemberByTag accepting std::pair<float,float>
// Uses PairToPointTransformer for conversion

// Alias for backward compatibility
[[ge::prop(name="OPACITY", alias="alpha")]]
float opacity;
// String lookup: "OPACITY" and "alpha" both map to same tag

// Multiple prop attributes on same field (array use case)
// One prop for whole array, one for individual element accessors
[[ge::prop(name="BEZIER_WARP_DESTINATION_PATCH")]]
[[ge::prop(name="BezierWarping_ControlPoint", array_accessor_length=12)]]
std::array<int, 12> destinationPatch;
// Generates:
// - BEZIER_WARP_DESTINATION_PATCH (whole array accessor)
// - BezierWarping_ControlPoint0-11 (individual element accessors)
```

---

## Advanced Topics

### GEFilterType and Generated Metadata

The `GEFilterType` enum (defined in `include/core/ge_filter_type.h`) is the central type identifier for all filters. EffectGen uses this enum to:

1. **Link params structs to filter types** via `ge::params(type=...)` attribute
2. **Generate type info mappings** that allow runtime type identification
3. **Create bidirectional conversions** between enum values and string names

**Relationship Flow:**

```
GEFilterType enum (ge_filter_type.h)
    ↓
[[ge::params(type=MY_FILTER, ...)]]  // Must match enum value
    ↓
GEFilterParamsTypeInfo<MY_PARAMS>::ID == GEFilterType::MY_FILTER
    ↓
GEParamsMemberHelper::GetFilterTypeFromTag(tag) → GEFilterType
```

**Example:**

```cpp
// In ge_filter_type.h:
enum class GEFilterType : int32_t {
    NONE,
    KAWASE_BLUR,  // ← enum value
    BEZIER_WARP,
    AIBAR,
    // ...
};

// In .params file:
struct [[ge::params(type=KAWASE_BLUR, name="KAWASE_BLUR")]]  // ← matches enum
    GEKawaseBlurShaderFilterParams {
    // ...
};

// Generated in ge_params_reflection.h:
template<>
struct GEFilterParamsTypeInfo<GEKawaseBlurShaderFilterParams> {
    static constexpr GEFilterType ID = GEFilterType::KAWASE_BLUR;  // ← link to enum
    static constexpr std::string_view FilterName = "KAWASE_BLUR";
};

// Usage:
auto filterType = GEFilterParamsTypeInfo<GEKawaseBlurShaderFilterParams>::ID;
// filterType == GEFilterType::KAWASE_BLUR
```

### Configuration

`config.json` defines type aliases and blocked types:

```json
{
  "type_aliases": {
    "int": "int32_t",
    "GESDFShaderShape": "Drawing::GESDFShaderShape"
  },
  "member_setter_blocked_types": [
    {
      "type": "std::shared_ptr<Drawing::GESDFShaderShapeShape>",
      "reason": "ambiguous setter with std::shared_ptr<Drawing::GEShaderShape>"
    }
  ]
}
```

- **type_aliases**: Normalize type names (e.g., `int` → `int32_t`)
- **member_setter_blocked_types**: Types excluded from SetParamsMemberByTag generation (prevents ambiguous overloads)

### Using Generated Metadata

```cpp
#include "ge_params_reflection.h"

// Create params
auto params = GEParamsBuilder::Build(GEFilterType::MY_FILTER);

// Set by tag (type-safe)
GEParamsMemberHelper::SetParamsMemberByTag(*params,
    GEParamsMemberTag::MY_FILTER_INTENSITY, 0.5f);

// Set array elements (POSITION0, POSITION1, POSITION2)
GEParamsMemberHelper::SetParamsMemberByTag(*params,
    GEParamsMemberTag::MY_FILTER_POSITION0, 1.0f);
GEParamsMemberHelper::SetParamsMemberByTag(*params,
    GEParamsMemberTag::MY_FILTER_POSITION1, 2.0f);
GEParamsMemberHelper::SetParamsMemberByTag(*params,
    GEParamsMemberTag::MY_FILTER_POSITION2, 3.0f);

// Get by tag
auto& intensity = GEParamsFieldAccessor<
    GEParamsMemberTag::MY_FILTER_INTENSITY>::Get(*params);

// String-to-tag lookup
auto tag = GEParamsMemberHelper::GEParamsMemberTagFromString("INTENSITY");
// Returns: GEParamsMemberTag::MY_FILTER_INTENSITY

// Tag-to-filter-type lookup
auto filterType = GEParamsMemberHelper::GetFilterTypeFromTag(
    GEParamsMemberTag::MY_FILTER_INTENSITY);
// Returns: GEFilterType::MY_FILTER
```

---

## Implementation Details

### Directory Structure

```
tool/effectgen/
├── gen_metadata.py          # Main metadata generator
├── gen_effect_header.py      # Header aggregator
├── cpp_parser.py            # C++ struct parser
├── cpp_tokenizer.py         # C++ tokenizer
├── attribute_parser.py       # Attribute parser ([[ge::...]])
├── value_parser.py          # Value expression parser
├── cli_utils.py             # CLI utilities
├── config.json              # Type aliases and blocked types
└── pyproject.toml           # Python project config
```

### Supporting Modules

#### cpp_tokenizer.py

C++ tokenizer that handles:
- Whitespace, comments (single/multi-line)
- String/char literals with escape sequences
- Numbers (integer and floating-point)
- Identifiers and keywords
- Operators and punctuation
- Attributes `[[...]]`
- Raw string literals `C++11`

#### cpp_parser.py

Parses C++ struct definitions with attributes:
- Extracts struct name, enum type, filter name
- Parses field types, names, and attributes
- Handles default attributes
- Collects all `[[ge::...]]` attributes

#### attribute_parser.py

Parses C++ attribute syntax:
- `[[ge::params(type=X, name="Y")]]`
- `[[ge::prop("NAME", min=0, max=100)]]`
- Supports C#/Rust-style key=value syntax
- Handles raw string literals, nested brackets, quotes

#### value_parser.py

Parses value expressions:
- Component values: `{1.0, 2.0, 3.0}`
- Numeric types: infers int vs float
- Used for constraint metadata generation

### Generated Metadata

#### GEParamsMemberTag Enum

```cpp
enum class GEParamsMemberTag : uint32_t {
    INVALID = 0,
    KAWASE_BLUR_RADIUS,
    BEZIER_WARP_DESTINATIONPATCH0,
    BEZIER_WARP_DESTINATIONPATCH1,
    // ... all fields
};
```

#### GEParamsFieldAccessor Template

```cpp
template<GEParamsMemberTag Tag>
struct GEParamsFieldAccessor;

template<>
struct GEParamsFieldAccessor<GEParamsMemberTag::KAWASE_BLUR_RADIUS> {
    using ParamsType = GEKawaseBlurShaderFilterParams;
    using FieldType = int;
    static constexpr const char name[] = "KAWASE_BLUR_RADIUS";
    static FieldType& Get(ParamsType& params);
    static void Set(ParamsType& params, const FieldType& value);
};
```

#### GEParamsMemberHelper

```cpp
class GEParamsMemberHelper {
public:
    static GEFilterType GetFilterTypeFromTag(GEParamsMemberTag tag);
    static GEParamsMemberTag GEParamsMemberTagFromString(const std::string& str);
    static void SetParamsMemberByTag(GEParamsParams& params, GEParamsMemberTag tag, const T& value);
};
```

#### Constraint Metadata

```cpp
template<GEParamsMemberTag Tag>
struct GEParamsConstraintMinInfo {
    static constexpr bool HAS_MIN = true;
    static constexpr Type MIN = 0.0f;
};

template<GEParamsMemberTag Tag>
struct GEParamsConstraintConvertInfo {
    static constexpr bool HAS_CONVERT = true;
    using CastFromType = int;
};
```

---

## Testing

Test scripts are in `test/tooltest/effectgen/`:

### run_parser_tests.py

Comprehensive parser robustness test suite.

**Test Structure:**
```
test/tooltest/effectgen/
├── syntax_valid_tests/     # Valid .params files
│   ├── basic_valid.params
│   ├── with_attributes.params
│   ├── string_literals.params
│   └── ...
└── syntax_invalid_tests/   # Invalid .params files
    ├── unmatched_brace.params
    ├── missing_comma.params
    └── ...
```

**Expected Results:**
Each test can have a `.params.json` file with expected results:
```json
{
  "name": "basic_valid",
  "should_parse": true,
  "expected_error_count": 0,
  "structs": [
    {
      "name": "TestParams",
      "enum_type": "TEST",
      "filter_name": "Test",
      "fields": [
        {
          "type": "int",
          "name": "value",
          "prop_name": "VALUE"
        }
      ]
    }
  ]
}
```

**Sample Test Cases:**

```cpp
// syntax_valid_tests/with_attributes.params
struct [[ge::params(type=TEST, name="TEST")]] TestParams {
    [[ge::prop("VALUE")]]
    int value;
    [[ge::prop(name="POSITION", array_accessor_length=3)]]
    Vector3f position;
};

// syntax_valid_tests/prop_with_convert_and_range.params
struct [[ge::params(type=TEST, name="TEST")]] TestParams {
    [[ge::prop(name="SIZE", cast_from=int, min=0.0, max=100.0)]]
    float size;
};

// syntax_invalid_tests/missing_comma_after_identifier.params
struct [[ge::params(type=TEST, name="TEST")]] TestParams {
    [[ge::prop("VALUE" "EXTRA")]]  // Error: missing comma
    int value;
};
```

**Running Tests:**
```bash
# Run tests (validates against .params.json if present)
python test/tooltest/effectgen/run_parser_tests.py

# Save current results as expected
python test/tooltest/effectgen/run_parser_tests.py --save-expected

# Show all details
python test/tooltest/effectgen/run_parser_tests.py --show-all-details
```

**Test Categories:**
- **Syntax valid**: Should parse successfully, validated against expected results
- **Syntax invalid**: Should fail gracefully with appropriate error messages

---

## Build Integration

EffectGen is typically run during the build process:

```bash
# Generate metadata
python tool/effectgen/gen_metadata.py

# Generate unified header
python tool/effectgen/gen_effect_header.py

# Format with clang-format (automatic if available)
```

Generated files are then compiled as part of the graphics_effect library.

### Example Workflow

1. **Define parameters** in `.params` file:
   ```cpp
   // include/effect/filter/ge_my_filter.params
   // Note: MY_FILTER must be a valid GEFilterType enum value
   struct [[ge::params(type=MY_FILTER, name="MY_FILTER")]] GEMyFilterParams {
       // Single parameter - positional form allowed
       [[ge::prop("INTENSITY")]]
       float intensity;

       // Multiple parameters - must use name=
       [[ge::prop(name="RADIUS", min=0.0, max=100.0)]]
       float radius;

       // Array accessor - generates 3 separate properties
       [[ge::prop(name="POSITION", array_accessor_length=3)]]
       Vector3f position;
   };
   ```

2. **Add enum value** to `include/core/ge_filter_type.h`:
   ```cpp
   enum class GEFilterType : int32_t {
       NONE,
       // ... existing values ...
       MY_FILTER,  // Add your new filter type here
   };
   ```

3. **Run generator**:
   ```bash
   python tool/effectgen/gen_metadata.py
   ```

4. **Use generated metadata** in C++:
   ```cpp
   #include "ge_params_reflection.h"

   // Create params
   auto params = GEParamsBuilder::Build(GEFilterType::MY_FILTER);

   // Set by tag (type-safe)
   GEParamsMemberHelper::SetParamsMemberByTag(*params,
       GEParamsMemberTag::MY_FILTER_INTENSITY, 0.5f);

   // Set array elements (POSITION0, POSITION1, POSITION2)
   GEParamsMemberHelper::SetParamsMemberByTag(*params,
       GEParamsMemberTag::MY_FILTER_POSITION0, 1.0f);
   GEParamsMemberHelper::SetParamsMemberByTag(*params,
       GEParamsMemberTag::MY_FILTER_POSITION1, 2.0f);
   GEParamsMemberHelper::SetParamsMemberByTag(*params,
       GEParamsMemberTag::MY_FILTER_POSITION2, 3.0f);

   // Get by tag
   auto& intensity = GEParamsFieldAccessor<
       GEParamsMemberTag::MY_FILTER_INTENSITY>::Get(*params);

   // String-to-tag lookup
   auto tag = GEParamsMemberHelper::GEParamsMemberTagFromString("INTENSITY");
   // Returns: GEParamsMemberTag::MY_FILTER_INTENSITY

   // Tag-to-filter-type lookup
   auto filterType = GEParamsMemberHelper::GetFilterTypeFromTag(
       GEParamsMemberTag::MY_FILTER_INTENSITY);
   // Returns: GEFilterType::MY_FILTER
   ```

---

## Dependencies

- Python 3.8+
- No external Python dependencies (uses only stdlib)
- clang-format (optional, for formatting generated code)

## Notes

- Generated files are marked with `// AUTO GENERATED BY tools/gen_metadata.py`
- Do not manually edit generated files (they will be overwritten)
- For custom modifications, edit `.params` source files instead
- The parser is "best-effort" - it focuses on parsing the specific patterns used by GE
