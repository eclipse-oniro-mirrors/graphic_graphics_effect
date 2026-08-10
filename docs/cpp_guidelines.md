# C++ Effect Implementation Guidelines

Conventions for `src/effect/`, `include/effect/`, and `test/`. Load only the section matching your task.

Aligned with [C++ Core Guidelines](https://raw.githubusercontent.com/isocpp/CppCoreGuidelines/refs/heads/master/CppCoreGuidelines.md). For GE-specific patterns, see [cpp_domain_guidelines.md](cpp_domain_guidelines.md). For upstream Drawing API behavior, see [drawing_api_contracts.md](drawing_api_contracts.md).

---

## Task Map

| Task | Section | Core Guideline |
|---------|---------|----------------|
| Cache a `RuntimeEffect` | [Static & Thread Safety](#static--thread-safety) | CP.2, CP.110 |
| Null-check external `shared_ptr` | [Null Safety](#null-safety) | ES.65 |
| Pass non-owning params by ref, not `shared_ptr` | [Non-Owning Params](#non-owning-params) | F.7 |
| Use `assert` correctly | [Assert in Production](#assert-in-production) | I.6 |
| Make local variables `const` | [Const-Correctness](#const-correctness) | Con.1 |
| Declare overrides without `virtual` | [Override Without `virtual`](#override-without-virtual) | C.128 |
| Share flags across threads | [Thread-Safe Flags](#thread-safe-flags) | CP.2 |
| Use typed enums over string tags | [Strongly Typed Params](#strongly-typed-params) | I.4 |

---

## Static & Thread Safety

Cached `RuntimeEffect` objects appear in two scopes — the fix differs per scope:

**Function-level `static`:** Two patterns — direct init is thread-safe; lazy init is a data race unless guarded.

- **Direct init** (`static auto x = MakeEffect(...)`): magic statics guarantee one-time thread-safe initialization; subsequent reads are safe. ✅
- **Lazy init** (`static shared_ptr x = nullptr; if (x == nullptr) x = MakeEffect(...)`): the check-then-assign is a data race (CP.2, CP.110 — don't hand-roll double-checked locking). Fix with `thread_local`, or restructure to direct init if the shader string is a compile-time constant.

```cpp
std::shared_ptr<Drawing::RuntimeEffect> GetEffect()
{
    // ✅ direct init — safe (magic statics)
    static auto g_effect = GECreateRuntimeEffectForShader(shaderStr);
    return g_effect;
}

std::shared_ptr<Drawing::RuntimeEffect> GetEffectLazy()
{
    // ✅ lazy init — thread_local
    thread_local static std::shared_ptr<Drawing::RuntimeEffect> g_effect = nullptr;
    if (g_effect == nullptr) { g_effect = GECreateRuntimeEffectForShader(shaderStr); }
    return g_effect;
}
```

**Namespace/global-level `static`:** Concurrent threads access the same object — check-then-assign races. Use `static thread_local` here too, or guard with `std::call_once` if the effect must be shared.

```cpp
// ✅ namespace-level: thread_local per-thread cache
static thread_local std::shared_ptr<Drawing::RuntimeEffect> g_effect = nullptr;
```

```cpp
// 🚫 data race on the shared_ptr itself (either scope)
static std::shared_ptr<Drawing::RuntimeEffect> g_effect = nullptr;
```

---

## Null Safety

**Null-check every external `shared_ptr` at the boundary** before deref (ES.65). Caller-ownership is unverifiable. Return the original image on failure, not `nullptr`, unless the contract requires a new image.

```cpp
// ✅ deref-safe: null-check before ->
if (image == nullptr) { LOGE("...image is null"); return image; }
auto mask = BuildMask(...);
if (mask == nullptr) { LOGE("...mask is null"); return image; }
mask->GetImageInfo();  // safe
```

Not all APIs need pre-checks — distinguish two cases:

- **Deref crash**: `gpuCtx.get()->SomeMethod()` or `*ptr` on a null `shared_ptr` is UB — the dereference itself is the problem, not the `.get()` call. Check before deref. Example: `canvas.GetGPUContext()` returns a `shared_ptr` that may be null on CPU-only paths (see [Drawing API Contracts](drawing_api_contracts.md#canvasgetgpucontext)).
- **Safe null return**: some APIs accept null internally and return `nullptr` themselves. Check the **return value**, not the input. Example: `builder->MakeImage(gpuCtx.get(), ...)` — passing null `gpuCtx` is safe, but the result must be checked (see [Drawing API Contracts](drawing_api_contracts.md#runtimeshaderbuildermakeimage)).

Also check: `Surface::MakeRenderTarget()` return — returns `nullptr` on failure without crashing. `CreateImageShader()` always returns a non-null `shared_ptr` but the internal shader may be null if the image is invalid; validate the image before calling (see [Drawing API Contracts](drawing_api_contracts.md#shadereffectcreateimageshader)).

---

## Non-Owning Params

**Don't pass `shared_ptr<T>` by value when the function doesn't store or transfer ownership** (F.7). The by-value copy bumps an atomic ref count for nothing — the function only uses the object during the call. For non-owning parameters, pass `const std::shared_ptr<T>&` (or `T&` / `T*` if you can drop the smart pointer entirely).

```cpp
// ✅ non-owning — ref, no ref-count bump
std::shared_ptr<Drawing::Image> GEMyFilter::BuildShader(
    const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src);
```

```cpp
// 🚫 ref-count bump for a function that never stores the pointer
std::shared_ptr<Drawing::Image> GEMyFilter::BuildShader(
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src);
```

Pass `shared_ptr<T>` by value only when the function intends to store or transfer it (caching, async queueing). This rule applies to `shared_ptr` — for cheap-to-copy types (`int`, `float`, raw pointers), pass by value is fine (F.16).

---
## Assert in Production

**No `assert()` in production paths.** Release builds define `NDEBUG`, making `assert` a no-op — "the wrong semantics (do you always want to abort in debug mode and check nothing in production runs?)" (I.6). Use explicit checks + `LOGE`.

```cpp
// 🚫 release builds silently disable this
assert(params != nullptr);
```

```cpp
// ✅
if (params == nullptr) { LOGE("...params is null"); return; }
```

---

## Thread-Safe Flags

**`std::atomic<T>` for cross-thread shared flags** (CP.2). Don't use `volatile` for synchronization.

```cpp
// ✅
static std::atomic<bool> isFeatureEnabled_;
bool IsFeatureEnabled() { return isFeatureEnabled_.load(); }
```

```cpp
// 🚫 data race
static bool isFeatureEnabled_;
```

---

## Const-Correctness

**Mark local variables `const` when they're not modified after initialization** (Con.1). Immutable locals are easier to reason about, prevent accidental mutation, and document intent at the point of use.

```cpp
// ✅
const auto inputShader = Drawing::ShaderEffect::CreateImageShader(*image, ...);
const float eps = 1e-6f;
const int n = GRID_ROW_OR_COL_NUM;
```

```cpp
// 🚫 mutable without reason — readers must trace all uses to confirm no reassignment
auto inputShader = Drawing::ShaderEffect::CreateImageShader(*image, ...);
```

**Exception**: a local that is returned by value and cheaper to move than copy should not be `const` — it can force an unnecessary copy (NRVO still applies, but `std::move` is blocked).

---

## Override Without `virtual`

**Use exactly one of `virtual`, `override`, or `final` — never `virtual ... override` together** (C.128). In a derived class, `override` alone is sufficient and self-documenting; adding `virtual` is redundant and masks intent.

```cpp
// ✅ override alone — compiler checks base match, intent is clear
void OnParamsChanged(const Drawing::GEFilterParams& params) override;
```

```cpp
// 🚫 redundant — virtual + override together
virtual void OnParamsChanged(const Drawing::GEFilterParams& params) override;
```

In the base class (first declaration), use `virtual` alone. In derived classes, use `override` (or `final` if sealed) alone.

---

## Strongly Typed Params

**Prefer typed enums over string tags for parameter dispatch** (I.4). String-based lookup defers errors to runtime — a typo in a tag name silently fails. Enum-based dispatch gives compile-time checking, IDE autocompletion, and catches mismatches at build time.

```cpp
// ✅ tag-based — compile-time checked, type-safe
ve->SetParam(GEParamsMemberTag::KAWASE_BLUR_RADIUS, 5);
```

```cpp
// 🚫 string-based — runtime lookup, typo silently fails
ve->SetParam("KAWASE_BLUR_RADIUS", 5);  // no error, no effect
```

The string-based overload exists for dynamic/scripting callers, but always prefer the tag-based API when the tag is known at compile time. See `tool/generate_metadata/README.md` for `.params.in` syntax that generates the `GEParamsMemberTag` enum.

