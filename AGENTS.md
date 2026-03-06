# AGENTS.md

This file provides guidance to Agents when working with code in this repository.

## Overview

This is `graphics_effect`, often abbreviated as `GE`, a component of OpenHarmony's graphics subsystem providing visual effects algorithm capabilities including blur, shadow, gradient, grayscale, edge lighting, and other shader-based effects. The library is part of the OpenHarmony foundation graphics stack and integrates with the 2D graphics rendering pipeline.

## Build System

This project uses GN (Generate Ninja) as its build system, which is standard for OpenHarmony projects.

### Building

Build commands are typically run from the OpenHarmony root directory (not this repository root):

```bash
hb build graphics_effect -i # full build of graphics_effect
hb build graphics_effect -i --skip-download --build-target <target> # fast incremental build
```

### Testing

```bash
# Build all tests for graphics_effect
hb build graphics_effect -t
# Fast rebuild of specific target. Full path usually works
hb build graphics_effect -t --skip-download --build-target <target>
```

### Test Organization

- **Unit tests**: `test/unittest/` - Test files follow pattern `ge_*_test.cpp`
- **Fuzz tests**: `test/fuzztest/` - Each fuzzer has its own subdirectory
- Tests use cflags `-Dprivate=public -Dprotected=public` to test private members

## Directory Structure

The codebase is organized into the following structure:

```
graphics_effect/
├── include/              # Public header files
│   ├── core/            # Core components (interfaces, base classes)
│   ├── effect/          # Effect implementations
│   │   ├── filter/      # Shader-based filters
│   │   ├── shader/      # Direct shader effects
│   │   ├── mask/        # Masking operations
│   │   └── shape/       # Shape-based effects (including SDF)
│   ├── pipeline/        # Rendering pipeline and composition
│   ├── hps/             # HPS (High Performance Shaders) integration
│   ├── ext/             # Extension functionality
│   └── util/            # Utility classes
├── src/                 # Implementation files (mirrors include structure)
│   ├── core/
│   ├── effect/
│   │   ├── filter/
│   │   ├── shader/
│   │   ├── mask/
│   │   └── shape/
│   ├── pipeline/
│   ├── hps/
│   ├── ext/
│   └── util/
└── test/                # Unit tests and fuzz tests
    ├── unittest/
    └── fuzztest/
```

## Architecture

The codebase follows a modular, layered architecture:

### 1. Core Layer (`include/core/`, `src/core/`)
- **IGEFilterType** (`ge_filter_type.h`) - Base interface for all filter types
- **GEVisualEffect** (`ge_visual_effect.h`, `ge_visual_effect_impl.h`) - Main effect implementation class
- **GEVisualEffectContainer** (`ge_visual_effect_container.h`) - Manages chains of multiple effects
- **GEFilterTypeInfo** (`ge_filter_type_info.h`) - Type registration and reflection

### 2. Pipeline Layer (`include/pipeline/`, `src/pipeline/`)
- **GERender** (`ge_render.h`) - Main rendering interface and effect pipeline orchestration
  - `DrawImageEffect()` - Applies effects and draws to canvas
  - `ApplyImageEffect()` - Applies effects and returns resulting image
  - `DrawShaderEffect()` - Draws shader-based effects
  - `ApplyHpsGEImageEffect()` - Applies effects using mixed GE/HPS pipeline with composition system
- **GEFilterComposer** (`ge_filter_composer.h`) - Multi-pass effect composition system
- **Rendering Passes**:
  - `GEDirectDrawOnCanvasPass` - Direct drawing to canvas
  - `GEHPSBuildPass`, `GEHPSUpscalePass` - HPS-specific passes
  - `GEMesaFusionPass` - Mesa fusion rendering strategy
  - `GEFilterComposerPass` - Generic filter composition pass
- **Caching**:
  - `IGECacheProvider` (`ge_cache_provider.h`) - Cache provider interface
  - `GEImageCacheProvider` (`ge_image_cache_provider.h`) - Image-based caching implementation

### 3. Effect Layer (`include/effect/`, `src/effect/`)

#### Filter Effects (`effect/filter/`)
Shader-based image processing filters (all inherit from `GEShaderFilter`):
- **Blur filters**: Kawase blur, Mesa blur, Linear gradient blur, Variable radius blur, Frosted glass blur
- **Distortion filters**: Displacement distortion, Bezier warp, Grid warp, Magnifier
- **Color filters**: Grey, Color gradient, Dispersion
- **Light/Glow filters**: Edge light, Content light, Direction light, Border light
- **SDF filters**: SDF edge light, SDF from image
- **Transition filters**: Mask transition
- **Other**: AI bar, Sound wave, Water ripple

#### Shader Effects (`effect/shader/`)
Direct shader effects (inherit from `GEShader`):
- **Lighting**: Border light, Circle flowlight, Contour diagonal flow light, Particle circular halo, Wavy ripple light
- **Material**: Frosted glass effect
- **Color**: Color gradient effect
- **Noise**: Aurora noise shader
- **Extension effects (GEX prefix)**:
  - `GEXComplexShader` - Complex shader composition
  - `GEXDotMatrixShader` - Dot matrix pattern
  - `GEXFlowLightSweepShader` - Flow light sweep animation

#### Mask Effects (`effect/mask/`)
Masking operations (inherit from `GEShaderMask`):
- **Gradient masks**: Frame gradient, Linear gradient, Radial gradient, Wave gradient
- **Image-based**: Image shader mask, Pixel map shader mask, Use effect shader mask
- **Animated**: Ripple shader mask, Double ripple shader mask, Wave disturb shader mask

#### Shape Effects (`effect/shape/`)
Shape-based effects including SDF (Signed Distance Field) system:
- **SDF Base** (`ge_sdf_shader_shape.h`) - Base SDF shape representation
- **SDF Shapes**:
  - `GESDFRRectShaderShape` - Rounded rectangle shapes
  - `GESDFPixelmapShaderShape` - Pixelmap-based shapes
  - `GESDFTransformShaderShape` - Transformable SDF shapes
  - `GESDFUnionOpShaderShape` - Union operation for combining shapes
- **SDF Effects**:
  - `GESDFBorderShader` - SDF-based borders
  - `GESDFColorShader` - SDF-based coloring
  - `GESDFClipShader` - SDF-based clipping
  - `GESDFShadowShader` - SDF-based shadow rendering
- **Base**: `GEShaderShape` (`ge_shader_shape.h`) - Base shape interface

### 4. HPS Layer (`include/hps/`, `src/hps/`)
- **HpsEffectFilter** (`ge_hps_effect_filter.h`) - HPS-specific effect filter integration

### 5. Extension Layer (`include/ext/`, `src/ext/`)
- **GEExternalDynamicLoader** (`ge_external_dynamic_loader.h`) - Dynamic loading of external effects
- **GEXMarshallingHelper** (`gex_marshalling_helper.cpp`) - Parameter marshalling for extension effects

### 6. Utility Layer (`include/util/`, `src/util/`)
- **GECommon** (`ge_common.h`) - Common definitions and utilities
- **GEDowncast** (`ge_downcast.h`) - Safe downcasting utilities
- **GELog** (`ge_log.h`) - Logging interface
- **GETrace** (`ge_trace.h`) - Tracing utilities
- **GESystemProperties** (`ge_system_properties.h`) - System property queries
- **GEToneMappingHelper** (`ge_tone_mapping_helper.h`) - Tone mapping utilities
- **Mock** (`util/mock/`) - Mock implementations for testing

### Effect Type Hierarchy

```
IGEFilterType (base interface in core/ge_filter_type.h)
├── GEShaderFilter (effect/filter/ge_shader_filter.h) - Processes images through shaders
│   ├── Blur filters (Kawase, Mesa, Linear Gradient, Variable Radius, Frosted Glass)
│   ├── Distortion filters (Displacement, Bezier Warp, Grid Warp, Magnifier)
│   ├── Color filters (Grey, Color Gradient, Dispersion)
│   ├── Light/Glow filters (Edge Light, Content Light, Direction Light, Border Light)
│   ├── SDF filters (SDF Edge Light, SDF From Image)
│   └── Transition/Other (Mask Transition, Water Ripple, Sound Wave, AI Bar)
├── GEShader (effect/shader/ge_shader.h) - Direct shader effects
│   ├── Lighting effects (Border Light, Flow Light, etc.)
│   ├── Material effects (Frosted Glass, Color Gradient)
│   └── Extension effects (GEX prefix - Complex, Dot Matrix, etc.)
├── GEShaderMask (effect/mask/ge_shader_mask.h) - Masking operations
│   ├── Gradient masks (Frame, Linear, Radial, Wave)
│   ├── Image-based masks (Image, Pixel Map, Use Effect)
│   └── Animated masks (Ripple, Double Ripple, Wave Disturb)
└── GEShaderShape (effect/shape/ge_shader_shape.h) - Shape-based effects
    ├── SDF shapes (RRect, Pixelmap, Transform, Union Op)
    └── SDF effects (Border, Color, Clip, Shadow)
```

## Key Subsystems

### SDF (Signed Distance Field) System

- **Location**: `src/effect/shape/`, `include/effect/shape/`
- **Purpose**: SDF-based shape rendering and effects (edge lighting, shadows, borders, clipping)
- **Components**:
  - `GESDFShaderShape` - Base SDF shape representation
  - `GESDFRRectShaderShape` - Rounded rectangle SDF shapes
  - `GESDFPixelmapShaderShape` - Pixelmap-based SDF shapes
  - `GESDFTransformShaderShape` - Transformable SDF shapes
  - `GESDFUnionOpShaderShape` - Union operations for combining SDF shapes
  - `GESDFBorderShader` - SDF-based border rendering
  - `GESDFColorShader` - SDF-based color effects
  - `GESDFClipShader` - SDF-based clipping
  - `GESDFShadowShader` - SDF-based shadow rendering
  - `GESDFEdgeLight` (in `effect/filter/`) - SDF-based edge lighting effects
  - `GESDFFromImageFilter` (in `effect/filter/`) - Converts images to SDF representation

### Filter Composition Pipeline

- **Location**: `src/pipeline/`, `include/pipeline/`
- **Purpose**: Multi-pass effect composition system with different rendering strategies
- **Components**:
  - `GEFilterComposer` - Orchestrates multi-pass effect composition
  - `GEDirectDrawOnCanvasPass` - Direct drawing to canvas
  - `GEHPSBuildPass` - HPS build pass
  - `GEHPSUpscalePass` - HPS upscale pass
  - `GEMesaFusionPass` - Mesa fusion rendering pass
  - `GEFilterComposerPass` - Generic filter composition pass

### Rendering System

- **Location**: `src/pipeline/ge_render.cpp`, `include/pipeline/ge_render.h`
- **Purpose**: Main rendering interface and effect pipeline orchestration
- **Key APIs**:
  - `DrawImageEffect()` - Applies effects and draws to canvas
  - `ApplyImageEffect()` - Applies effects and returns resulting image
  - `DrawShaderEffect()` - Draws shader-based effects
  - `ApplyHpsGEImageEffect()` - Applies effects using mixed GE/HPS pipeline with composition system

### Caching System

- **Location**: `src/pipeline/`, `include/pipeline/`
- **Purpose**: Provides caching infrastructure for rendered images
- **Interface**:
  - `IGECacheProvider` - Cache provider interface
  - `GEImageCacheProvider` - Image-based caching implementation

### HPS Integration

- **Location**: `src/hps/`, `include/hps/`
- **Purpose**: High Performance Shaders (HPS) integration for optimized rendering
- **Components**:
  - `HpsEffectFilter` - HPS-specific effect filter integration

### Extension System

- **Location**: `src/ext/`, `include/ext/`
- **Purpose**: Dynamic loading and marshalling for external effects
- **Components**:
  - `GEExternalDynamicLoader` - Dynamic loading of external effect libraries
  - `GEXMarshallingHelper` - Parameter marshalling for extension effects

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

Parameters are identified by string tags defined in `effect/ge_shader_filter_params.h`.
