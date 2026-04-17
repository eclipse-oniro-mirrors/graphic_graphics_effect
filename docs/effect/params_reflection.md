# Parameter Reflection Metadata System

## TLDR

This document describes the architecture of GE's **declarative parameter system with code generation** — how `.params` files define effect parameters, and how the generator produces reflection metadata for runtime access.

**Key Points**:
- **Generated files** (marked with `// AUTO GENERATED` header) must **NOT be edited manually** — they are overwritten on each generator run
- To modify behavior: edit `.params` source files, not generated output
- The system replaces 60+ individual parameter members with a single type-erased container

| Generated File | Generator | Status |
|----------------|-----------|--------|
| `ge_params_reflection.h/cpp` | `gen_metadata.py` | **Auto-generated** |
| `ge_effects_params.h` | `gen_effect_header.py` | **Auto-generated** |

> For tool usage (commands, syntax, config), see [`tool/generate_metadata/README.md`](../../tool/generate_metadata/README.md).

---

## Design Motivation

### Problem

Before v2, `GEVisualEffectImpl` stored **60+ independent parameter member variables**, causing:

- **High maintenance cost**: New effects required adding member variables and `SetParam` methods
- **Type safety issues**: `SetParam` used string tags without compile-time checking
- **Code duplication**: Similar parameter management logic repeated per effect
- **Runtime errors**: String tag typos only detected at runtime

### Solution

**Declarative parameter definition + code generation**:

```
.params files (declarative definitions)
        ↓
    Code generator
        ↓
Reflection metadata (ge_params_reflection.h/cpp)
        ↓
    GEVisualEffectImpl (unified usage)
```

**Core changes**:
- Single `params_` member replaces 60+ individual variables
- Type-erased container `GEFilterParams` stores any parameter struct
- Compile-time generated metadata provides type-safe field access

---

## Architecture Overview

### Component Relationships

```
┌─────────────────────────────────────────────────────────────────────┐
│                        .params files                                 │
│   [[ge::params]] struct GEKawaseBlurShaderFilterParams { ... }      │
└───────────────────────────────────┬─────────────────────────────────┘
                                    │ parse
                    ┌───────────────┴───────────────┐
                    │      gen_metadata.py          │
                    └───────────────┬───────────────┘
                                    │ generate
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌───────────────┐         ┌─────────────────┐         ┌─────────────────┐
│ GEParamsMember│         │GEFilterParams   │         │GEParamsMember   │
│    Tag enum   │         │  TypeInfo<T>    │         │    Helper       │
│               │         │                 │         │                 │
│ KAWASE_BLUR_  │         │ ID = KAWASE_BLUR│         │ SetParamsMember │
│    RADIUS     │         │ FilterName=...  │         │    ByTag()      │
└───────────────┘         └─────────────────┘         │ TagFromString() │
                                                    └─────────────────┘
        │                           │                           │
        └───────────────────────────┼───────────────────────────┘
                                    │ usage
                                    ▼
                    ┌───────────────────────────────┐
                    │     GEVisualEffectImpl        │
                    │                               │
                    │  std::unique_ptr<GEFilterParams> params_
                    │                               │
                    │  SetParam<Tag>(tag, value)    │
                    │  SetParam("string", value)    │
                    └───────────────────────────────┘
```

### Data Flow

```
Caller
    │ SetParam("RADIUS", 10)
    ▼
GEVisualEffectImpl::SetParam(string, value)
    │ GEParamsMemberTagFromString("RADIUS") → Tag
    ▼
GEVisualEffectImpl::SetParam(tag, value)
    │ IsTagValidForCurrentType() check
    ▼
GEParamsMemberHelper::SetParamsMemberByTag(params, tag, value)
    │ GetFilterTypeFromTag() → verify type match
    │ GEFilterParams::Unbox<Params>() → unwrap
    │ GEParamsValueTransformer::Transform() → convert + constraints
    ▼
GEParamsFieldAccessor<Tag>::Set(params, transformed_value)
    │ params.field = value
    ▼
Done
```

---

## Key Design Points

### 1. Type-Erased Container

`GEFilterParams` is a **type-erased container** using template wrappers:

```cpp
// Base: holds GEFilterType as runtime type ID
class GEFilterParams {
    GEFilterType id;
    virtual ~GEFilterParams() = default;
};

// Wrapper: holds concrete shared_ptr<T>
template<typename T>
class GEFilterParamsWrapper : public GEFilterParams {
    std::shared_ptr<T> data;
};

// Type-safe boxing/unboxing
auto params = GEFilterParams::Box(std::make_shared<GEKawaseBlurShaderFilterParams>());
auto unboxed = GEFilterParams::Unbox<GEKawaseBlurShaderFilterParams>(*params);
```

**Why `shared_ptr<T>` instead of storing `T` directly**:
- Compatibility with existing code (multiple places use `shared_ptr` access)
- Avoid copying large parameter structs
- Support cross-module parameter sharing

### 2. Compile-Time Generated Metadata

Reflection info is fully generated at compile-time, no runtime registration:

```cpp
// Each tag corresponds to a FieldAccessor specialization
template<>
struct GEParamsFieldAccessor<GEParamsMemberTag::KAWASE_BLUR_RADIUS> {
    using ParamsType = GEKawaseBlurShaderFilterParams;  // owning struct
    using FieldType = int;                              // field type
    static constexpr const char name[] = "KAWASE_BLUR_RADIUS";
    static FieldType& Get(ParamsType& p) { return p.radius; }
    static void Set(ParamsType& p, const FieldType& v) { p.radius = v; }
};
```

**Benefits**:
- No runtime initialization overhead
- Compile-time type checking
- IDE auto-completion support

### 3. Constraint System

Constraints are implemented via template specializations, applied in `GEParamsValueTransformer::Transform()`:

```cpp
// min/max constraints
template<>
struct GEParamsConstraintMinInfo<GEParamsMemberTag::AURORA_NOISE_FREQ_X> {
    static constexpr bool HAS_MIN = true;
    static constexpr float MIN = 0.1f;
};

// type conversion constraints
template<>
struct GEParamsConstraintConvertInfo<GEParamsMemberTag::BEZIER_WARP_DESTINATION_PATCH0> {
    static constexpr bool HAS_CAST_FROM = true;
    using CastFromType = std::pair<float, float>;
    static constexpr bool HAS_CUSTOM = true;
    using CustomTransformer = PairToPointTransformer;
};
```

**Constraint processing pipeline**:
1. `ApplyConversion()`: type conversion (`cast_from` or custom transformer)
2. `ApplyRangeConstraints()`: range constraints (`std::clamp` or component-wise clamp)

---

## Development Guidelines

### DO NOT Edit Generated Files

All generated files contain the header:

```cpp
// AUTO GENERATED BY tools/generate_metadata/gen_metadata.py
// DO NOT EDIT MANUALLY
```

**Manual edits will be lost** on next generator run.

**Where to edit instead**:

| Need | Location |
|------|----------|
| Add/modify parameters | Edit `.params` files |
| Add custom transformer | Edit `ge_value_transformer.h` |
| Type alias config | Edit `tool/generate_metadata/config.json` |
| Generator logic | Edit `gen_metadata.py` |

### Workflow for Adding New Effects

```bash
# 1. Create .params file
vim include/effect/filter/ge_new_filter.params

# 2. Add GEFilterType enum value
vim include/core/ge_filter_type.h

# 3. Run generators
python tool/generate_metadata/gen_metadata.py
python tool/generate_metadata/gen_effect_header.py

# 4. Implement effect class
vim src/effect/filter/ge_new_filter.cpp
```

**Critical order**: Define `.params` → Generate → Implement. Reverse order causes build failures.

### String Tag Uniqueness

`GEParamsMemberTagFromString()` uses `unordered_map` to map strings to tags. **String conflicts cause generation failure**.

Avoid conflicts:
- Use effect name as prefix: `KAWASE_BLUR_RADIUS`, `MESA_BLUR_RADIUS`
- Check existing `.params` files for naming patterns
- Use `alias` attribute for backward compatibility, not duplicate tags

### Type Conversion Usage

`cast_from` generates additional `SetParamsMemberByTag` overloads:

```cpp
// Field type is float, but accepts int
[[ge::prop(name="SIZE", cast_from=int)]]
float size;

// Generated code allows both call styles:
SetParamsMemberByTag(params, SIZE_TAG, 10.0f);  // direct float
SetParamsMemberByTag(params, SIZE_TAG, 10);     // int → float
```

**Note**: Conversion happens before setting; original value is not stored.

### Custom Transformer Requirements

Transformer must satisfy:

```cpp
struct MyTransformer {
    // Must be static method
    // Return bool for success/failure
    static bool Transform(const FromType& value, ToType& out) {
        // ... conversion logic
        return true;  // or false for failure
    }
};
```

Reference in `.params`:

```cpp
[[ge::prop(name="FIELD", cast_from="FromType", custom="MyTransformer")]]
FieldType field;
```

### blocked_types Configuration

When two types cause ambiguity in `SetParamsMemberByTag`, block one in `config.json`:

```json
{
  "member_setter_blocked_types": [
    {
      "type": "std::shared_ptr<Drawing::GESDFShaderShapeShape>",
      "reason": "Ambiguous with std::shared_ptr<Drawing::GEShaderShape>"
    }
  ]
}
```

**Effect**: Blocked type won't generate `SetParamsMemberByTag` overload, cannot be set via `SetParam`.

---

## Integration with Existing Code

### GEVisualEffectImpl Usage Pattern

```cpp
// Create effect instance
auto effect = std::make_shared<GEVisualEffectImpl>("KAWASE_BLUR");
effect->MakeParams<GEKawaseBlurShaderFilterParams>();

// Set parameter - prefer tag (compile-time check)
effect->SetParam(GEParamsMemberTag::KAWASE_BLUR_RADIUS, 5);

// Set parameter - string (runtime lookup, for dynamic APIs)
effect->SetParam("KAWASE_BLUR_RADIUS", 5);

// Get parameter
auto params = effect->GetParams<GEKawaseBlurShaderFilterParams>();
int radius = params->radius;
```

### Accessing Parameters in Filter Implementation

```cpp
void GEMyBlurShaderFilter::DoFilter(...) {
    // Get params from GEVisualEffectImpl
    auto params = visualEffect_->GetParams<GEMyBlurShaderFilterParams>();
    
    // Direct field access
    int radius = params->radius;
    
    // Use params for rendering...
}
```

---

## Common Issues

### Q: Build fails after modifying `.params`?

**Cause**: Generated `.h/.cpp` files not updated.

**Solution**: Run `gen_metadata.py` before building.

### Q: SetParam call has no effect?

**Debug steps**:
1. Check `GEFilterType` match (`IsTagValidForCurrentType`)
2. Check tag exists in `GEParamsMemberTag` enum
3. Check value type not blocked by `blocked_types`
4. Check transformer returns `true`

### Q: String tag lookup returns INVALID?

**Cause**: String not registered in `GEParamsMemberTagFromString` map.

**Solution**:
- Confirm `[[ge::prop("NAME")]]` definition in `.params`
- Check spelling
- Use `alias` attribute for backward compatibility

### Q: Constraints not applied?

**Cause**: Constraints applied in `GEParamsValueTransformer::Transform`, only when setting via `SetParamsMemberByTag`.

**Not applied when**:
- Direct access `params->field = value`
- Modifying after `GetParams()` retrieval

---

## References

- **Tool usage guide**: [`tool/generate_metadata/README.md`](../../tool/generate_metadata/README.md)
- **Parameter definition examples**: `include/effect/filter/*.params`
- **Generated reflection code**: `include/effect/ge_params_reflection.h`
- **Transformer definitions**: `include/effect/ge_value_transformer.h`
- **Core implementation**: `include/core/ge_visual_effect_impl.h`
- **Type-erased container**: `include/effect/ge_filter_params.h`