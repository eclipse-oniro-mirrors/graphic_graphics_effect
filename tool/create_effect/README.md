# create_effect

Scaffold CLI for creating new visual effects in Graphics Effect (GE) library. Generates boilerplate: `.params.in`, `.h`, `.cpp` files with proper naming and inheritance.

**Note**: Generates scaffold code only. You must implement shader logic and `OnProcessImage()` manually.

---

## TL;DR

```bash
python tool/create_effect/create_effect.py <name> <type> [--no-enum]

**Types**: `filter` | `mask` | `shader` | `shape`

**Options**:
- `--no-enum`: Disable automatic enum addition (useful for testing without modifying ge_filter_type.h)

**Post-scaffold steps**:
1. ~~Add enum to `include/core/ge_filter_type.h`~~ **Auto-added** (if fails, manual required)
2. Implement render logic
3. Run `python tool/generate_metadata/gen_metadata.py` and `python tool/generate_metadata/gen_effect_header.py`

---

## Overview

This tool creates initial file structure for new effects:

| Generated | Purpose |
|-----------|---------|
| `.params.in` | Parameter struct with default fields |
| `.h` | Class declaration with proper inheritance |
| `.cpp` | Skeleton implementation with TODO placeholders |

Generated code follows GE conventions but requires:
- Manual shader code (GLSL/SkSL)
- Implementation of `OnProcessImage()` or equivalent methods
- RuntimeEffect creation with custom shader source

---

## Quick Start

```bash
# Scaffold a filter effect
python tool/create_effect/create_effect.py my_blur filter
```

**Generated files**:

| File | Path |
|------|------|
| `.params.in` | `include/effect/filter/ge_my_blur_shader_filter.params.in` |
| `.h` | `include/effect/filter/ge_my_blur_shader_filter.h` |
| `.cpp` | `src/effect/filter/ge_my_blur_shader_filter.cpp` |

**Generated `.params.in`**:

```cpp
// TODO: Add type to GEFilterType in include/core/ge_filter_type.h and remove this line
struct [[ge::params(type=MY_BLUR, name="MyBlur")]] GEMyBlurShaderFilterParams {
    float radius = 10.0f;
    float intensity = 0.5f;
    Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
};
```

**Complete workflow**:

```bash
# 1. Scaffold
python tool/create_effect/create_effect.py my_blur filter

# 2. Enum auto-added to ge_filter_type.h (or manually if failed)
#    Check console output for "Added enum" or "Manual action required"

# 3. Implement shader logic (manual)
# Edit src/effect/filter/ge_my_blur_shader_filter.cpp

# 4. Generate metadata
python tool/generate_metadata/gen_metadata.py
```

---

## Effect Types

| Type | Base Class | Namespace | Generated Class | Typical Use |
|------|------------|-----------|-----------------|-------------|
| `filter` | `GEShaderFilter` | `Rosen` | `GE{Name}ShaderFilter` | Blur, color, distortion |
| `mask` | `GEShaderMask` | `Drawing` | `GE{Name}ShaderMask` | Gradient/image masks |
| `shader` | `GEShader` | `Rosen` | `GE{Name}Shader` | Lighting, material effects |
| `shape` | `GESDFShaderShape` | `Drawing` | `GE{Name}SDFShaderShape` | SDF borders/shadows/clips |

---

## Command-Line Options

```bash
python tool/create_effect/create_effect.py <name> <type> [--root DIR] [--templates DIR]
```

| Argument | Required | Description |
|----------|----------|-------------|
| `name` | Yes | Effect name in snake_case (e.g., `my_blur`) |
| `type` | Yes | One of: `filter`, `mask`, `shader`, `shape` |

| Option | Default | Description |
|--------|---------|-------------|
| `--root` | Parent of `tool/` | Project root directory |
| `--templates` | `tool/templates/` | Template files directory |

---

## Generated Files

### File Locations

```
include/effect/{type}/ge_{name}_{suffix}.{ext}
src/effect/{type}/ge_{name}_{suffix}.{ext}
```

**Suffix by type**:

| Type | Suffix |
|------|--------|
| `filter` | `shader_filter` |
| `mask` | `shader_mask` |
| `shader` | `shader` |
| `shape` | `sdf_shader_shape` |

### File Contents

| File | Contains |
|------|----------|
| `.params.in` | Struct with `[[ge::params]]` attribute, default fields (radius, intensity, color) |
| `.h` | Class declaration, inheritance, `DECLARE_GEFILTER_TYPEFUNC`, virtual methods |
| `.cpp` | Constructor, TODO placeholders, `GetEffect()` stub |

---

## Naming Conventions

| Input `my_blur` | Output |
|-----------------|--------|
| Class | `GEMyBlurShaderFilter` (PascalCase + GE prefix) |
| File | `ge_my_blur_shader_filter.h` (snake_case + ge prefix) |
| Params | `GEMyBlurShaderFilterParams` (class + Params suffix) |

---

## Template Customization

Templates in `tool/templates/` use Python `string.Template`:

| Variable | Injected Value |
|----------|----------------|
| `$YEAR` | Current year |
| `$HEADER_GUARD` | Header guard macro |
| `$CLASS_NAME` | Generated class name |
| `$PARAMS_CLASS` | Parameter class name |

To modify scaffolds, edit template files:
- `*.h.tpl` - Class declarations
- `*.cpp.tpl` - Method implementations
- `*.params.tpl` - Parameter structures

---

## Implementation Details

### Directory Structure

```
tool/create_effect/
├── create_effect.py
└── templates/
    ├── filter.{h,cpp,params}.tpl
    ├── mask.{h,cpp,params}.tpl
    ├── shader.{h,cpp,params}.tpl
    └── shape.{h,cpp,params}.tpl
```

### Requirements

- Python 3.8+
- Access to graphics_effect project directory

### Related Tool

After scaffolding, generate reflection metadata:

```bash
python tool/generate_metadata/gen_metadata.py
```

See `tool/generate_metadata/README.md` for details.
