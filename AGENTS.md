# AGENTS.md

This file provides guidance to Agents when working with code in this repository.

## Overview

This is `graphics_effect` (abbreviated `GE`), a component of OpenHarmony's graphics subsystem providing visual effects algorithm capabilities including blur, shadow, gradient, grayscale, edge lighting, and other shader-based effects. The library integrates with the 2D graphics rendering pipeline.

## Build System

This project uses GN (Generate Ninja) as its build system, standard for OpenHarmony projects.

> **Working directory**: `hb build` / `./build.sh` run from the OpenHarmony root (the directory containing `build.py`), NOT from this repo. Python tooling (`tool/`) runs from this repo root.

### Building

```bash
# Build the main graphics_effect library (independent build)
# MUST run from the OpenHarmony root (where build.py lives), NOT from this repo!
hb build graphics_effect -i

# Build with full OH code, product could be rk3568 etc.
./build.sh --product-name <product> --build-target graphics_effect
```

### Testing

```bash
# Build all tests (independent build)
hb build graphics_effect -t

# Build with full OH code
./build.sh --product-name <product> --build-target graphics_effect_test
```

### Test Organization

- **Unit tests**: `test/unittest/` — files follow pattern `ge_*_test.cpp`
- **Fuzz tests**: `test/fuzztest/` — each fuzzer has its own subdirectory
- **Tool tests**: `test/tooltest/` — tool robustness tests
- Tests use cflags `-Dprivate=public -Dprotected=public` to access private members (OpenHarmony test framework cannot access private members without this)

### Test Naming Convention

- **Don't use `_001` suffix** — use descriptive names (e.g., `TestKawaseBlurRadius` not `TestKawaseBlur_001`). Numbered suffixes are a legacy pattern.

## Tool Chain

Python-based tooling for effect scaffolding and parameter metadata generation. See `tool/create_effect/README.md` and `tool/generate_metadata/README.md` for detailed syntax and configuration.

### Effect Scaffolding

```bash
python tool/create_effect/create_effect.py <name> <type>
```

- **Types**: `filter`, `mask`, `shader`, `shape`
- Generates `.params.in`, `.h`, `.cpp` scaffold files — shader logic must be implemented manually

### Code Generation

- **`python tool/generate_metadata/gen_metadata.py`** — Run after any `.params.in` file is **modified**. Generates `ge_params_reflection.h/cpp` (auto-generated — do not manually edit).
- **`python tool/generate_metadata/gen_effect_header.py`** — Run after any `.params.in` file is **added or removed**. Generates `ge_effects_params.h` (auto-generated — do not manually edit).

### Complete New Effect Workflow

1. **Scaffold**: `python tool/create_effect/create_effect.py <name> <type>` (generates `.params.in`, `.h`, `.cpp` stubs)
2. **Register**: Add enum value to `ge_filter_type.h`
3. **Define parameters**: Edit the generated `.params.in` file
4. **Generate metadata**: `python tool/generate_metadata/gen_metadata.py`
5. **Generate effects header**: `python tool/generate_metadata/gen_effect_header.py`
6. **Implement**: Write shader logic in the generated `.cpp` file

## Directory Structure

- `include/` — Public headers (core, effect/{filter,shader,mask,shape}, pipeline, hps, ext, effect_cfg, util)
- `src/` — Implementations (mirrors include structure)
- `tool/` — Code generation and scaffolding (create_effect, generate_metadata)
- `test/` — Tests (unittest, fuzztest, tooltest)

## Architecture

The codebase follows a modular, layered architecture. → [Full architecture details](docs/architecture.md)

| Layer | Directory | Responsibility |
|-------|-----------|---------------|
| Core | `include/core/` | Base interfaces (`IGEFilterType`), visual effect container, type registration, effect factory |
| Pipeline | `include/pipeline/` | Rendering interface (`GERender`), multi-pass composition (`GEFilterComposer`), caching |
| Effect | `include/effect/` | Four effect types: `GEShaderFilter` (image filters), `GEShader` (direct shaders), `GEShaderMask` (masking), `GEShaderShape` (SDF shapes) — see [include/effect/AGENTS.md](include/effect/AGENTS.md) |
| HPS | `include/hps/` | High Performance Shader integration for optimized rendering |
| Extension | `include/ext/` | Dynamic loading of external effects |
| Utility | `include/util/` | Common definitions, logging, tracing, system properties, tone mapping, transform helpers |
| Effect Config | `include/effect_cfg/` | XML configuration parsing (system `graphic_config.xml`) |

### Key Subsystems

- **SDF shapes & effects** — SDF-based shape rendering and edge lighting, shadows, borders, clipping. Flow: define shape → optionally combine shapes → generate SDF shader → bind effect shader → draw. → [Full details](docs/architecture.md#sdf-signed-distance-field-system) · [include/effect/shape/](include/effect/shape/AGENTS.md)
- **Filter composition** — multi-pass effect composition with different rendering strategies. Four passes in fixed order: HpsBuild → MesaFusion → HpsUpscale → DirectDrawOnCanvas. → [Full details](docs/architecture.md#filter-composition-pipeline)
- **Rendering** — main rendering interface via `GERender`. Key entry points: `DrawImageEffect`, `ApplyImageEffect`, `DrawShaderEffect`, `ApplyHpsGEImageEffect`. → [Full details](docs/architecture.md#rendering-system)
- **Caching** — two independent mechanisms: per-effect `std::any` cache and cross-effect `IGECacheProvider` shared store. → [Full details](docs/architecture.md#caching-system)
- **Extension dispatch** — dynamic loading of external effects via `dlopen`. Factory registration with `EXTERNAL`/`EXTERNAL_FALLBACK`/`CUSTOM` macros; runtime dispatch via system property. → [Full details](docs/architecture.md#extension-system)
- **HPS integration** — GPU-optimized rendering via High Performance Shaders. Requires GPU extension support + system property enablement. Pure HPS or mixed GE/HPS composition. → [Full details](docs/architecture.md#hps-integration)

## Effect Development

Shaders are written inline as GLSL/SkSL strings in C++ source files. Key patterns:

1. **RuntimeEffect creation**: Use `Drawing::RuntimeEffect` for shader compilation
2. **Shader parameters**: Set via `RuntimeShaderBuilder` uniform binding
3. **Image inputs**: Pass as shader children via `RuntimeShaderBuilder`
4. **Output**: Shaders generate `Drawing::Image` or draw to `Drawing::Canvas`

Parameters: definition (`.params.in`) → generation (`gen_metadata.py`) → runtime (`SetParam`). Two entry points: tag-based `SetParam(GEParamsMemberTag, T)` (type-safe) and string-based `SetParam(const std::string&, T)`. See `tool/generate_metadata/README.md` for syntax and API details.

## Code Conventions

### Naming

- **Classes**: `GE` prefix (e.g., `GERender`, `GEVisualEffect`)
- **Files**: Match class names with snake_case (e.g., `ge_render.cpp` for `GERender`)
- **SDF classes**: `GESDF` prefix (e.g., `GESDFEdgeLight`)
- **Extension classes**: `GEX` prefix (e.g., `GEXDotMatrixShader`)

### Platform Macros

- `GE_OHOS` — OpenHarmony platform
- `GE_PLATFORM_UNIX` — Unix-like platforms (Linux, OHOS)
- `USE_M133_SKIA` — Skia version flag (M133+)

## Dependencies

- **2D Graphics**: `graphic_2d:2d_graphics` — provides Drawing API (`Canvas`, `Image`, `RuntimeEffect`)
- **Logging**: `hilog:libhilog`

## Boundaries

- ✅ **Always:**
  - Run `gen_metadata.py` after modifying `.params.in`
  - Run `gen_effect_header.py` after adding/removing `.params.in`
  - Follow scaffolding workflow for new effects
  - Use `GE` prefix for new classes
  - Run `git clang-format` on modified code before committing

- ⚠️ **Ask first:**
  - Before changing core interfaces (`IGEFilterType`, `GEVisualEffect`, `GERender`)
  - Before modifying `graphic_config.xml` parsing logic

- 🚫 **Never:**
  - Manually edit `ge_params_reflection.h/cpp` or `ge_effects_params.h` (auto-generated)
  - Run `hb` from this repo root (must run from OpenHarmony root where `build.py` lives)
  - Use `_001` suffix in test names
  - Commit secrets/API keys

## Task Navigation

- **Add new effect** → [tool/create_effect/](tool/create_effect/) + [include/effect/](include/effect/AGENTS.md)
- **SDF / edge lighting / shadow** → [include/effect/shape/](include/effect/shape/AGENTS.md) + [docs/shape/](docs/shape/)
- **Parameter system** → [docs/effect/params_reflection.md](docs/effect/params_reflection.md) + [tool/generate_metadata/](tool/generate_metadata/README.md)
- **C++ conventions** → [docs/cpp_guidelines.md](docs/cpp_guidelines.md) + [docs/cpp_domain_guidelines.md](docs/cpp_domain_guidelines.md)
- **Upstream Drawing API** → [docs/drawing_api_contracts.md](docs/drawing_api_contracts.md)
- **Writing tests** → [docs/testing_guide.md](docs/testing_guide.md) + `test/unittest/`