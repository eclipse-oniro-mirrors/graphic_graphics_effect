# Graphics Effect Architecture

Detailed architecture and subsystem documentation for the `graphics_effect` (GE) library. See `AGENTS.md` for summaries and links.

## Architecture

The codebase follows a modular, layered architecture:

### 1. Core Layer (`include/core/`, `src/core/`)
- **IGEFilterType** (`ge_filter_type.h`) - Base interface for all filter types
- **GEVisualEffect** (`ge_visual_effect.h`, `ge_visual_effect_impl.h`) - Main effect implementation class
- **GEVisualEffectContainer** (`ge_visual_effect_container.h`) - Manages chains of multiple effects
- **GEFilterTypeInfo** (`ge_filter_type_info.h`) - Type registration and reflection
- **GEEffectFactory** (`ge_effect_factory.h`) - Factory for creating `GEVisualEffect` from `GEFilterType` enum

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
- **Blur**: Kawase blur, Mesa blur, Linear gradient blur, Variable radius blur, Frosted glass blur
- **Distortion**: Displacement distortion, Bezier warp, Grid warp, Magnifier
- **Color**: Grey, Color gradient, Dispersion
- **Light/Glow**: Edge light, Content light, Direction light
- **SDF**: SDF edge light, SDF from image
- **Transition**: Mask transition
- **Other**: AI bar, Sound wave, Water ripple
- See `.params.in` files in `include/effect/filter/` for complete list

#### Shader Effects (`effect/shader/`)
Direct shader effects (inherit from `GEShader`):
- **Lighting**: Border light, Circle flowlight, Contour diagonal flow light, Particle circular halo, Wavy ripple light
- **Material**: Frosted glass effect
- **Color**: Color gradient effect
- **Noise**: Aurora noise shader
- **Spatial**: Spatial point light, Spatial glass effect
- **Extension effects (GEX prefix)**:
  - `GEXComplexShader` - Complex shader composition
  - `GEXDotMatrixShader` - Dot matrix pattern
  - `GEXFlowLightSweepShader` - Flow light sweep animation
- See `.params.in` files in `include/effect/shader/` for complete list

#### Mask Effects (`effect/mask/`)
Masking operations (inherit from `GEShaderMask`):
- **Gradient masks**: Frame gradient, Linear gradient, Radial gradient, Wave gradient
- **Image-based**: Image shader mask, Pixel map shader mask, Use effect shader mask
- **Animated**: Ripple shader mask, Double ripple shader mask, Wave disturb shader mask
- See `.params.in` files in `include/effect/mask/` for complete list

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
- See `.params.in` files in `include/effect/shape/` for complete list

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
- **GETransformHelper** (`ge_transform_helper.h`) - Camera intrinsics/extrinsics and perspective transform calculation
- **Mock** (`util/mock/`) - Mock implementations for testing

### 7. Effect Configuration Layer (`include/effect_cfg/`, `src/effect_cfg/`)
- **GEXmlParserBase** (`ge_xml_parser_base.h`) - Base class for XML configuration parsing (system/product `graphic_config.xml`)
- **GEFrostedGlassEffectCfg** (`ge_frosted_glass_effect_cfg.h`) - Singleton for frosted glass effect configuration (e.g., `disableAntiAliasCode` flag)

### Effect Type Hierarchy

```
IGEFilterType (base interface in core/ge_filter_type.h)
├── GEShaderFilter (effect/filter/ge_shader_filter.h) - Processes images through shaders
│   ├── Blur, Distortion, Color, Light/Glow, SDF, Transition, and Other categories
│   └── See .params.in files in include/effect/filter/ for complete list
├── GEShader (effect/shader/ge_shader.h) - Direct shader effects
│   ├── Lighting, Material, Color, Noise, Spatial, and Extension (GEX) categories
│   └── See .params.in files in include/effect/shader/ for complete list
├── GEShaderMask (effect/mask/ge_shader_mask.h) - Masking operations
│   ├── Gradient, Image-based, Animated, and Extension (GEX) categories
│   └── See .params.in files in include/effect/mask/ for complete list
└── GEShaderShape (effect/shape/ge_shader_shape.h) - Shape-based effects
    ├── SDF Shapes (RRect, Pixelmap, Transform, Union Op, Ellipse, Path, Triangle, etc.)
    └── SDF Effects (Border, Color, Clip, Shadow)
    └── See .params.in files in include/effect/shape/ for complete list
```

## Key Subsystems

### SDF (Signed Distance Field) System

- **Location**: `src/effect/shape/`, `include/effect/shape/`
- **Purpose**: SDF-based shape rendering and effects (edge lighting, shadows, borders, clipping)
- **Flow**:
  1. Define primitive shape: construct Params struct → instantiate shape class (e.g. `GESDFRRectShaderShape`)
  2. Combine shapes (optional): union/sub/smooth-sub/distort ops take child shapes → produce combined `GESDFShaderShape`
  3. Generate SDF distance: `shape->GenerateDrawingShader()` → `RuntimeShaderBuilder` → outputs SDF distance in alpha channel (`.a`)
  4. Bind effect shader: `builder->SetChild("sdfShape", sdfShader)` → effect shader reads `sdfShape.eval(fragCoord).a` for distance value
  5. Draw: `brush.SetShaderEffect()` → `canvas.DrawRect()`

### Filter Composition Pipeline

- **Location**: `src/pipeline/`, `include/pipeline/`
- **Purpose**: Multi-pass effect composition with different rendering strategies
- **Flow**:
  1. Create `GEFilterComposer`, add 4 passes in fixed order
  2. `BuildComposables()` wraps each `GEVisualEffect` into `GEFilterComposable`
  3. `composer.Run()` executes passes sequentially, mutating composables in-place:
     - `GEHpsBuildPass`: convert GE effects → `HpsEffectFilter` where GPU supports
     - `GEMesaFusionPass`: fuse Grey + Kawase blur → Mesa blur
     - `GEHpsUpscalePass`: set `NeedUpscale` on HPS effects followed by GE effects
     - `GEDirectDrawOnCanvasPass`: mark `FROSTED_GLASS` for direct canvas draw
  4. Iterate composables: GE → `ProcessShaderFilter`/`DrawShaderFilter`; HPS → `ApplyHpsEffect`; chain output→input

### Rendering System

- **Location**: `src/pipeline/ge_render.cpp`, `include/pipeline/ge_render.h`
- **Purpose**: Main rendering interface and effect pipeline orchestration
- **Key Components**:
  - `DrawImageEffect()`: `ApplyImageEffect()` → `canvas.DrawImageRect()` (blit result to canvas)
  - `ApplyImageEffect()`: iterate container filters → `ProcessShaderFilter()` per filter → chain input→output image
  - `DrawShaderEffect()`: iterate container shaders → `DrawShader()` per shader → draw directly to canvas (GEShader-type effects, no offscreen image)
  - `ApplyHpsGEImageEffect()`: `ComposeEffects()` → iterate composables → dispatch GE/HPS → chain output→input

### Caching System

- **Location**: `src/pipeline/`, `include/pipeline/`
- **Purpose**: Two independent caching mechanisms serving different scopes
- **Key Components**:
  - **Per-effect cache** (`std::any`): each effect holds frame-to-frame state. Round-trip per filter: before → `SetCache(ve->GetCache())`; after → `ve->SetCache(geShaderFilter.GetCache())`
  - **`IGECacheProvider`**: dependency-injection-style cross-effect shared store — higher layers own cache lifecycle, GE injects concrete implementations, separating construction from storage to safely share data across effects. `GEImageCacheProvider` (image caching) is one implementation; the interface supports any data type. Usage: `geShaderFilter->SetCacheProvider(context.geCacheProvider)` → effect calls `cacheProvider_->GetFirst()` to retrieve and `cacheProvider_->Store(cacheData)` to persist

### HPS Integration

- **Location**: `src/hps/`, `include/hps/`
- **Purpose**: High Performance Shaders (HPS) integration for optimized rendering
- **Flow**:
  1. Effect HPS support requires external GPU dependency: (a) type registered in support map with corresponding GPU extension string, (b) GPU hardware reports that extension, (c) any special conditions met
  2. Check enablement: system property `persist.sys.graphic.hpsEffectEnabled`
  3. Check support per effect: `IsEffectSupported()` validates type + GPU extension + conditions
  4. Pure HPS: all effects supported → convert params → `DrawImageEffectHPS()`
  5. Mixed: `ComposeEffects()` → `GEFilterComposer` → dispatch per composable

### Extension System

- **Location**: `src/ext/`, `include/ext/`
- **Purpose**: Dynamic loading and marshalling for external effects
- **Flow**:
  1. Load library: `dlopen("libgraphics_effect_ext.z.so")` → resolve `CreateGEXObjectByType` symbol
  2. Register factory: macros (`EXTERNAL`, `EXTERNAL_FALLBACK`, `CUSTOM`) register creation lambdas in `GEEffectFactory`
  3. Create effect: `GEEffectFactory::CreateFilter()` → `GEExternalDynamicLoader::CreateGEXObjectByType()` → external `IGEFilterType`
  4. Runtime dispatch: check system property `rosen.graphic.gex.enable` → call external function