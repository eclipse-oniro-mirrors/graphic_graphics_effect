# effect/ — Effect Implementations

Four effect subdirectories, each with its own `AGENTS.md`: [filter/](filter/AGENTS.md) · [shader/](shader/AGENTS.md) · [mask/](mask/AGENTS.md) · [shape/](shape/AGENTS.md)

## Auto-generated files in this directory

`ge_params_reflection.h`, `ge_effects_params.h` — do not edit (see root AGENTS.md → Tool Chain / Boundaries).

## Parameter infrastructure (source files)

| File | Purpose |
|------|---------|
| `ge_filter_params.h` | `GEFilterParams` type-erased container (boxing/unboxing) |
| `ge_value_transformer.h` / `ge_value_transformer_traits.h` | Custom type conversion for `cast_from`/`custom` params |
| `ge_shader_filter_params.h` / `ge_shader_filter_params_for_kits.h` / `ge_shader_filter_params_namesonly.h` | Params includes / kit-facing / name-only variants |

## Related docs

- [docs/effect/params_reflection.md](../../docs/effect/params_reflection.md) — parameter system architecture
