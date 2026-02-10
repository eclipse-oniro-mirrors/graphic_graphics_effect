# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is `graphics_effect`, a component of OpenHarmony's graphics subsystem providing visual effects algorithm capabilities including blur, shadow, gradient, grayscale, edge lighting, and other shader-based effects. The library is part of the OpenHarmony foundation graphics stack and integrates with the 2D graphics rendering pipeline.

## Build System

This project uses GN (Generate Ninja) as its build system, which is standard for OpenHarmony projects.

### Building the Library

```bash
# Build the main graphics_effect library
./build.sh --product-name <product> --build-target graphics_effect:graphics_effect_core

# Build with specific platform flags (defined in config.gni)
```

### Running Tests

```bash
# Build and run unit tests
./build.sh --product-name <product> --build-target graphics_effect:GraphicsEffectTest

# Build fuzz tests
./build.sh --product-name <product> --build-target graphics_effect:fuzztest
```

### Test Organization

- **Unit tests**: `test/unittest/` - Test files follow pattern `ge_*_test.cpp`
- **Fuzz tests**: `test/fuzztest/` - Each fuzzer has its own subdirectory
- Tests use cflags `-Dprivate=public -Dprotected=public` to test private members

## Architecture

The codebase follows a three-layer architecture:

### 1. GERender (Rendering Layer)
- **Location**: `src/ge_render.cpp`, `include/ge_render.h`
- **Purpose**: Provides drawing capabilities and orchestrates the effect pipeline
- **Key APIs**:
  - `DrawImageEffect()` - Applies effects and draws to canvas
  - `ApplyImageEffect()` - Applies effects and returns resulting image
  - `DrawShaderEffect()` - Draws shader-based effects

### 2. GEVisualEffect (Effect Implementation Layer)
- **Location**: `src/ge_visual_effect.cpp`, `include/ge_visual_effect.h`
- **Purpose**: Individual visual effect implementations
- **Pattern**: Each effect is parameterized via `SetParam()` with string tags
- **Types**: Blur filters, shader filters, masks, shapes, and composited effects

### 3. GEVisualEffectContainer (Effect Container)
- **Location**: `src/ge_visual_effect_container.cpp`, `include/ge_visual_effect_container.h`
- **Purpose**: Manages chains of multiple effects
- **Pattern**: Effects added via `AddToChainedFilter()` and applied sequentially

### Effect Type Hierarchy

```
IGEFilterType (base interface in ge_filter_type.h)
├── GEShaderFilter (ge_shader_filter.h) - Processes images through shaders
│   ├── Blur filters (Kawase, Mesa, Linear Gradient, Variable Radius)
│   ├── Distortion filters (Displacement, Bezier Warp, Grid Warp)
│   ├── Color filters (Grey, Color Gradient, Dispersion)
│   └── Light/Glow filters (Edge Light, Content Light, Border Light)
├── GEShader (ge_shader.h) - Direct shader effects
├── GEShaderMask (ge_shader_mask.h) - Masking operations
└── GEShaderShape (ge_shader_shape.h) - Shapes operations
```

## Key Subsystems

### SDF (Signed Distance Field) System
- **Location**: `src/sdf/`, `include/sdf/`
- **Purpose**: SDF-based shape rendering and effects (edge lighting, shadows, borders)
- **Components**:
  - `GESDFShaderShape` - Base shape representation
  - `GESDFRRectShaderShape` - Rounded rectangle shapes
  - `GESDFPixelmapShaderShape` - Pixelmap-based shapes
  - `GESDFEdgeLight` - SDF-based edge lighting effects
  - `GESDFShadowShader` - SDF-based shadow rendering
  - `GESDFFromImageFilter` - Converts images to SDF representation

### Filter Composition
- **Location**: `src/ge_filter_composer.cpp`, `include/ge_filter_composer.h`
- **Purpose**: Multi-pass effect composition system
- **Passes**: Different rendering strategies (direct draw, HPS upscale, mesa fusion)

### Caching System
- **Location**: `include/cache/`
- **Purpose**: Provides caching infrastructure for rendered images
- **Interface**: `IGECacheProvider`, `GEImageCacheProvider`

## Code Conventions

### Naming
- **Classes**: `GE` prefix for Graphics Effect classes (e.g., `GERender`, `GEVisualEffect`)
- **Files**: Match class names with snake_case (e.g., `ge_render.cpp` for `GERender`)
- **Parameters**: Shader parameters use string tags (e.g., `"radius"`, `"intensity"`)
- **SDF classes**: `GESDF` prefix (e.g., `GESDFEdgeLight`)
- **Extension classes**: `GEX` prefix (e.g., `GEXDotMatrixShader`)

### Platform Macros
- `GE_OHOS` - OpenHarmony platform
- `GE_PLATFORM_UNIX` - Unix-like platforms (Linux, OHOS)
- `USE_M133_SKIA` - Skia version flag (M133+)

### Compilation Flags
- C++17 standard (`-std=c++17`)
- Security hardening: `-D_FORTIFY_SOURCE=2`, `-ftrapv`
- Hidden visibility by default: `-fvisibility=hidden`
- Branch protection: `branch_protector_ret = "pac_ret"`

## Dependencies

- **2D Graphics**: `graphic_2d:2d_graphics` (Drawing API, Canvas, Image)
- **Logging**: `hilog:libhilog`
- **Security**: `bounds_checking_function:libsec_shared`
- **Utilities**: `c_utils:utils` (OpenHarmony only)
- **Test dependencies**: `graphic_surface:surface`, `egl:libEGL`, `opengles:libGLES`

## Shader Development

Shaders are typically written inline as GLSL/SkSL strings within C++ source files. Key patterns:

1. **RuntimeEffect creation**: Effects use `Drawing::RuntimeEffect` for shader compilation
2. **Shader parameters**: Set via `RuntimeShaderBuilder` uniform binding
3. **Image inputs**: Passed as shader children via `RuntimeShaderBuilder`
4. **Output**: Shaders generate `Drawing::Image` or draw to `Drawing::Canvas`

## Parameter System

Visual effects use a strongly-typed parameter system via overloaded `SetParam()` methods:

- Numeric: `int32_t`, `float`, `double`
- Geometric: `Vector2f`, `Vector3f`, `Vector4f`, `Matrix`, `GERRect`
- Images: `std::shared_ptr<Drawing::Image>`
- Shader objects: `GEShaderMask`, `GEShaderShape`
- SDF parameters: `GESDFBorderParams`, `GESDFShadowParams`

Parameters are identified by string tags defined in `ge_shader_filter_params.h`.