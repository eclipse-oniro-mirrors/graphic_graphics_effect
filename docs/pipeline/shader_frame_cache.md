# Shader Frame Cache

## TLDR

This document describes the **frame-level render result cache** added for `GERender::DrawShaderEffect()` — how rendered effect frames are fingerprinted, stored, and reused across frames when nothing has changed.

**Key Points**:
- Caches the **rendered image** of shader effects (GEShader-type, drawn directly to canvas), keyed by a parameter fingerprint + canvas fingerprint
- Two cache levels: `PER_EFFECT` (per-filter cache) and `CONTAINER` (whole-chain composite cache)
- Opt-in per `GEVisualEffectContainer`: nothing is cached until the caller configures a policy via `EnsureCacheStore()` + `SetCachePolicy()`
- Master switch via system property `persist.sys.graphic.shaderFrameCacheEnabled` (default on, OHOS only)
- Debug overlay via `persist.sys.graphic.shaderCacheDebugOverlay` (green = hit, red = miss)

> This is the **third** caching mechanism in GE, alongside the per-effect `std::any` cache and the cross-effect `IGECacheProvider`. See [Caching System](../architecture.md#caching-system) for how they relate.

---

## 1. Scope and Design Goals

`DrawShaderEffect()` renders GEShader-type effects (lighting, flow light, noise, SDF shaders, etc.) directly onto the canvas. Before this feature, every frame re-ran every shader even when parameters, geometry, and target surface were unchanged.

The frame cache eliminates redundant shader execution:

- **Fingerprint-based reuse**: a frame is re-rendered only when its inputs (params, canvas info, target surface) actually change
- **Skip-frame policy**: allows deliberately reusing stale frames for a fixed interval (e.g. to throttle expensive effects)
- **Zero-cost when unused**: with no policy configured, or on non-OHOS platforms, or with the property off, drawing falls back to the original direct path
- **Safe degradation**: any incompatibility (e.g. blend modes the offscreen composite cannot preserve) disables the cache for that container rather than producing wrong output

## 2. Architecture

```
GERender::DrawShaderEffect()
        |
        v
GEShaderCache::DrawWithCache() -----------------------------+ (no store / incompatible /
        |                                                    |  property off / level NONE)
        |                                                    v
        |                                          DrawSingleShaderEffect() x N
        |                                          (original direct path)
        v
+-------------------------------------------------------------+
|  PER_EFFECT:                    |  CONTAINER:               |
|  for each filter i:             |  composite all filters    |
|    hit  -> draw cachedImage     |  on transparent offscreen |
|    miss -> RenderToOffscreen    |    hit  -> draw cached    |
|          -> UpdateEffectCache   |    miss -> snapshot,      |
|          -> draw                |           cache, draw     |
+-------------------------------------------------------------+
```

### 2.1 Core Components

| Component | File | Role |
|-----------|------|------|
| `GEParamHashHelper` | `include/util/ge_param_hash_helper.h` | Low-level hashing primitives (`Hash`, `CombineHash`, `HashValue`, `HashFields`, `HashString`) |
| `GEVisualEffectImpl::Hash()` | `include/core/ge_visual_effect_impl.h` | Per-effect fingerprint: filter type + params + canvas info |
| `GEFilterParams::Hash()` | `include/effect/ge_filter_params.h` | Virtual hash over the type-erased parameter payload (base returns `id`, `GEFilterParamWrapper<T>` hashes type + data) |
| `GEFrameCacheLevel` | `include/pipeline/ge_frame_cache.h` | `NONE` / `PER_EFFECT` / `CONTAINER` |
| `GEFrameSkipMode` / `GEFrameSkipPolicy` | `include/pipeline/ge_frame_cache.h` | `NONE` / `ON_PARAM_SAME` / `FIXED_INTERVAL` + `skipInterval` |
| `GEFrameCacheData` | `include/pipeline/ge_frame_cache.h` | One cache slot: hashes, cached image, remaining skip frames, valid flag |
| `GEIsCacheHit()` | `src/pipeline/ge_frame_cache.cpp` | Hit test with content match and skip-frame counting |
| `GEFrameCacheStore` | `include/pipeline/ge_frame_cache.h` | Owning store: container slot + per-effect slot map + per-effect skip policies; policy changes and invalidation |
| `GEShaderCache` | `include/pipeline/ge_shader_cache.h` | Orchestration: dispatch by level, offscreen rendering, canvas/container hash, debug overlay |
| `GEFrameCacheStore cacheStore_` | `include/core/ge_visual_effect_container.h` | Lazy-owned store on the container (`EnsureCacheStore` / `GetCacheStore` / `RemoveCacheStore`) |

### 2.2 Fingerprints

Two hashes are computed per draw and stored in each `GEFrameCacheData`:

- **`paramHash`** — what is being drawn:
  - Per effect (`GEVisualEffectImpl::Hash()`): filter type, `params_->Hash()` (all effect parameters), canvas info (geometry size, translation, full matrix, material dst rect, `disableFilterCache` flag)
  - Container (`GEShaderCache::ComputeContainerHash()`): ordered fold of all filters' `Hash()`
  - A `paramHash` of `0` means "no fingerprint" and never hits
- **`canvasHash`** (`GEShaderCache::CalcCanvasHash()`) — where it is drawn: bounds (width/height/left/top), target surface color type, serialized color space

A cache slot is reused only when **both** match (`GEIsCacheHit`), so parameter edits, geometry changes, and surface/format changes each invalidate independently.

### 2.3 Hit Test and Skip-Frame Policy

```
GEIsCacheHit(cache, paramHash, canvasHash, policy):
  if !cache.valid                       -> miss
  if paramHash && canvasHash both match -> hit
  if policy.mode == FIXED_INTERVAL
     && cache.remainingSkipFrames > 0   -> decrement, hit (stale frame reused)
  else                                  -> miss
```

- `ON_PARAM_SAME` (skipInterval ignored): reuse only while the fingerprint is unchanged — pure memoization
- `FIXED_INTERVAL`: after a real re-render, the fresh frame is reused for the next `skipInterval` draws even if parameters change — a throttle for effects where brief staleness is acceptable (e.g. animated noise)
- On every cache update the current `skipInterval` is (re)loaded into `remainingSkipFrames`

## 3. Cache Levels

### 3.1 `PER_EFFECT`

Each filter in the chain is rendered and cached independently:

- Cache key: `GEFrameCacheStore::MakeEffectKey(filterType, index)` → `"<filterType>_<index>"`
- Per-effect skip policies can be applied via `ApplyCachePolicy(level, policy, filterTypes)`
- A miss on filter *i* does not invalidate cached frames of other filters
- Offscreen rendering cost per miss: one surface sized to `bounds`

### 3.2 `CONTAINER`

The whole filter chain is composited once onto a **transparent offscreen surface** (translated by `-bounds.Left/Top` so coordinates line up), snapshotted, cached as a single image, and pasted back to the target canvas:

- One hash and one cache slot for the entire chain; cheapest per-frame cost when the chain is static
- Blend/alpha caveats: compositing on a transparent surface means non-default canvas alpha can make overlapping regions over-accumulate, and non-`SRC_OVER` blend modes are lost on paste-back. Such containers must be marked incompatible (below)

## 4. Policy Changes, Invalidation, and Trimming

`GEFrameCacheStore::SetCachePolicy()` applies **graded invalidation**:

| Change | Effect |
|--------|--------|
| `level` changed, or `skipMode` changed | Full invalidation: container cache and all per-effect caches are invalidated |
| Only `skipInterval` changed | **No invalidation** — cached images are still valid (params unchanged); the current skip period finishes naturally and the new interval takes effect on the next cache update |

`GEVisualEffectContainer::UpdateCacheDataFrom()` (called when the filter chain is rebuilt/migrated) transfers the cache store and then calls `TrimEffectCaches(validKeys)`: per-effect slots and per-effect skip policies whose `MakeEffectKey(filterType, index)` no longer matches the current filter list are erased, so removed/changed chain positions do not leak stale entries.

`MarkCacheIncompatible(true)` (only meaningful at `CONTAINER` level) flags the store as incompatible; `GEShaderCache::DrawWithCache()` then bypasses caching and draws each effect directly — wrong-output scenarios are avoided by falling back, never by caching incorrectly.

### Degradation paths (all fall back to `DrawSingleShaderEffect()` per filter)

- No cache store on the container (policy never configured)
- Cache marked incompatible
- System property `persist.sys.graphic.shaderFrameCacheEnabled` = 0
- `GEFrameCacheLevel::NONE` or an unexpected level value

If the offscreen canvas creator is not injected or fails, `RenderToOffscreen`/`DrawContainer` log an error and skip drawing that frame's cached path.

## 5. System Properties and Debugging

| Property | Default | Platform | Effect |
|----------|---------|----------|--------|
| `persist.sys.graphic.shaderFrameCacheEnabled` | `1` | OHOS only (non-OHOS: treated as off) | Master switch for the frame cache |
| `persist.sys.graphic.shaderCacheDebugOverlay` | `0` | OHOS only | Draws a translucent rect over the effect bounds: green = cache hit, red = miss |

Both are read through independent `CachedHandle`s (`CachedParameterCreate`/`CachedParameterGetChanged`), so runtime changes are picked up without restart. Cache hits/misses are also traced via `GE_TRACE_ALWAYS_FMT` (`ShaderCache: perEffect hit/miss`, `ShaderCache: container hit/miss`).

## 6. Integration (Caller-Side API)

GE itself only **reads** the policy during drawing; the caller (upper graphics layer) owns configuration. Typical setup before drawing:

```cpp
auto& store = veContainer.EnsureCacheStore();               // lazily create the store
store.SetCachePolicy(GEFrameCacheLevel::CONTAINER,
                     { GEFrameSkipMode::ON_PARAM_SAME, 0 }); // or ApplyCachePolicy(..., container.GetFilterTypes())
// store.MarkCacheIncompatible(true);                       // if canvas alpha / blend mode require it

GEShaderCache::SetOffscreenCanvasCreator([](Drawing::Canvas& canvas, int32_t w, int32_t h) -> OffscreenCanvas {
    // create a surface/canvas compatible with the target (e.g. GPU surface)
    return { surface, surface->GetCanvas() };
});
```

`SetOffscreenCanvasCreator` is a static injection point: GE does not know how to create render-target-compatible surfaces, so the embedder provides a factory. Without it, only the direct path works.

## 7. Files

| File | Change |
|------|--------|
| `include/pipeline/ge_frame_cache.h`, `src/pipeline/ge_frame_cache.cpp` | New — levels, skip policy, cache data, `GEIsCacheHit`, `GEFrameCacheStore` |
| `include/pipeline/ge_shader_cache.h`, `src/pipeline/ge_shader_cache.cpp` | New — `GEShaderCache` orchestration |
| `include/util/ge_param_hash_helper.h`, `src/util/ge_param_hash_helper.cpp` | New — hashing primitives |
| `include/core/ge_visual_effect_impl.h` | `Hash()` fingerprint |
| `include/effect/ge_filter_params.h` | Virtual `Hash()` on params base and wrapper |
| `include/core/ge_visual_effect_container.h`, `src/core/ge_visual_effect_container.cpp` | `cacheStore_` ownership, `GetFilterTypes()`, store transfer + `TrimEffectCaches` in `UpdateCacheDataFrom()` |
| `include/pipeline/ge_render.h`, `src/pipeline/ge_render.cpp` | `DrawShaderEffect()` routes through `GEShaderCache::DrawWithCache()`; extracted `DrawSingleShaderEffect()` |
| `include/util/ge_system_properties.h`, `src/util/ge_system_properties.cpp` | Frame cache + debug overlay property readers |
| `include/util/ge_trace.h` | Cache trace helpers |
| `test/unittest/ge_frame_cache_test.cpp` | New |
| `test/unittest/ge_param_hash_helper_test.cpp` | New |
| `test/unittest/ge_shader_cache_test.cpp` | New |
| `test/unittest/ge_visual_effect_container_test.cpp`, `ge_render_test.cpp` | Added frame cache cases |
