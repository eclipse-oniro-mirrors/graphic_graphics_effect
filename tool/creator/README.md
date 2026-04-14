# Graphics Effect Creator

## Overview

The Graphics Effect Creator is a code generation tool that automates the creation of new Graphics Effect components in the OpenHarmony graphics subsystem. It uses template-based generation to create consistent, well-structured code for different effect types.

## Features

- **Automated Code Generation**: Generates header files, implementation files, and parameter definitions
- **Multiple Effect Types**: Supports four types of graphics effects:
  - **Filter**: Shader-based image processing filters (inherits from `GEShaderFilter`)
  - **Mask**: Masking operations (inherits from `GEShaderMask`)
  - **Shader**: Direct shader effects (inherits from `GEShader`)
  - **Shape**: Shape-based effects using SDF (inherits from `GESDFShaderShape`)
- **Template-Based**: Uses customizable templates for consistent code structure
- **Naming Conventions**: Automatically handles CamelCase/snake_case conversions

## Directory Structure

```
tool/creator/
├── create_ge_effect.py      # Main script for generating effects
└── templates/                # Template files for code generation
    ├── filter.h.tpl         # Filter effect header template
    ├── filter.cpp.tpl       # Filter effect implementation template
    ├── filter.params.tpl    # Filter parameter definition template
    ├── mask.h.tpl           # Mask effect header template
    ├── mask.cpp.tpl         # Mask effect implementation template
    ├── mask.params.tpl      # Mask parameter definition template
    ├── shader.h.tpl         # Shader effect header template
    ├── shader.cpp.tpl       # Shader effect implementation template
    ├── shader.params.tpl    # Shader parameter definition template
    ├── shape.h.tpl          # Shape effect header template
    ├── shape.cpp.tpl        # Shape effect implementation template
    └── shape.params.tpl     # Shape parameter definition template
```

## Usage

### Basic Usage

Create a new effect without parameters:

```bash
# Create a filter effect
python create_ge_effect.py my_blur filter

# Create a mask effect
python create_ge_effect.py my_gradient mask

# Create a shader effect
python create_ge_effect.py my_light shader

# Create a shape effect
python create_ge_effect.py my_shape shape
```

### Advanced Options

```bash
# Specify custom root directory
python create_ge_effect.py my_effect filter --root /path/to/project

# Specify custom templates directory
python create_ge_effect.py my_effect filter --templates /path/to/templates
```

### Command-Line Arguments

- `name` (required): Name of the effect (e.g., 'my_blur', 'my_gradient')
- `type` (required): Type of effect to create (filter, mask, shader, shape)
- `--root`: Root directory of the graphics_effect project (default: parent of tool directory)
- `--templates`: Directory containing template files (default: tool/templates)

## Generated Files

For each effect, the tool generates three files:

### 1. Parameter Definition File (.params)

Location: `include/effect/{type}/ge_{name}_{type}_shader_filter.params`

Defines the parameter structure and type information:

```cpp
// TODO: Add type to GEFilterType in include/core/ge_filter_type.h and remove this line
struct [[ge::params(type=MY_EFFECT, name="MyEffect")]] GEMyEffectShaderFilterParams {
    float radius = 10.0f;
    float intensity = 0.5f;
    Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
};
```

**Notice**: You need to add a new item of type enum GEFilterType to include/core/ge_filter_type.h manually.

### 2. Header File (.h)

Location: `include/effect/{type}/ge_{name}_{type}_shader_filter.h`

Defines the effect class with proper inheritance and interface:

```cpp
class GE_EXPORT GEMyEffectShaderFilter : public GEShaderFilter {
public:
    GEMyEffectShaderFilter(const Drawing::GEMyEffectShaderFilterParams& params);
    ~GEMyEffectShaderFilter() override = default;
    DECLARE_GEFILTER_TYPEFUNC(GEMyEffectShaderFilter, Drawing::GEMyEffectShaderFilterParams);

    std::shared_ptr<Drawing::Image> OnProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> GetEffect();

    float radius_;
    float intensity_;
    Vector4f color_;
};
```

### 3. Implementation File (.cpp)

Location: `src/effect/{type}/ge_{name}_{type}_shader_filter.cpp`

Provides the implementation skeleton:

```cpp
GEMyEffectShaderFilter::GEMyEffectShaderFilter(
    const Drawing::GEMyEffectShaderFilterParams& params)
    : radius_(params.radius),
      intensity_(params.intensity),
      color_(params.color)
{
}

// Implementation of OnProcessImage and GetEffect methods
// ...
```

## Effect Types

### Filter (GEShaderFilter)

**Base Class**: `GEShaderFilter`
**Namespace**: `Rosen`
**Purpose**: Shader-based image processing filters

**Use Cases**:
- Blur effects
- Color adjustments
- Image transformations
- Distortion effects

**Generated Class**: `GE{Name}ShaderFilter`

### Mask (GEShaderMask)

**Base Class**: `GEShaderMask`
**Namespace**: `Drawing`
****Purpose**: Masking operations

**Use Cases**:
- Gradient masks
- Image-based masks
- Animated masks

**Generated Class**: `GE{Name}ShaderMask`

### Shader (GEShader)

**Base Class**: `GEShader`
**Namespace**: `Rosen`
**Purpose**: Direct shader effects

**Use Cases**:
- Lighting effects
- Material effects
- Custom visual effects

**Generated Class**: `GE{Name}Shader`

### Shape (GESDFShaderShape)

**Base Class**: `GESDFShaderShape`
**Namespace**: `Drawing`
**Purpose**: Shape-based effects using Signed Distance Fields

**Use Cases**:
- SDF-based borders
- SDF-based shadows
- SDF-based clipping
- SDF-based coloring

**Generated Class**: `GE{Name}SDFShaderShape`

## Naming Conventions

The tool automatically handles naming conversions:

- **Input Name**: `my_blur` (snake_case)
- **Class Name**: `GEMyBlurShaderFilter` (PascalCase with GE prefix)
- **File Name**: `ge_my_blur_shader_filter.h` (snake_case with ge prefix)
- **Params Class**: `GEMyBlurShaderFilterParams`

## Template Customization

Templates are located in the `templates/` directory and use Python's `string.Template`:

### Template Variables

Common template variables:
- `$YEAR`: Current year
- `$HEADER_GUARD`: Header guard macro
- `$CLASS_NAME`: Generated class name
- `$PARAMS_CLASS`: Parameter class name
- `$MEMBER_DECLARATIONS`: Member variable declarations
- `$INITIALIZATION_LIST`: Constructor initialization list

### Customizing Templates

To customize the generated code, modify the template files:

1. **Header Templates** (`*.h.tpl`): Modify class declarations, includes, and method signatures
2. **Implementation Templates** (`*.cpp.tpl`): Modify method implementations
3. **Parameter Templates** (`*.params.tpl`): Modify parameter structure definitions

## Requirements

- Python 3.6+
- Access to the graphics_effect project directory

## License

Apache License, Version 2.0
