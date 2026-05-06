# SDF Ellipse Notes

Implementation at [src/effect/shape/ge_sdf_ellipse_shader_shape.cpp](../../src/effect/shape/ge_sdf_ellipse_shader_shape.cpp)

## Shape Model

The ellipse shape is defined by two uniforms:

- `centerPos`: ellipse center in fragment coordinates
- `width`: ellipse width in pixels
- `height`: ellipse height in pixels

For each fragment, the shader first moves into ellipse-local space:

```glsl
vec2 p = fragCoord - centerPos;
```

The valid input requirement is that both `width` and `height` are positive. The
CPU side rejects a size smaller than `MIN_SIZE`, and the shader still clamps the
derived half-size with `EPS` to avoid division by zero in generated GPU code.

## Distance Formula

The shader uses this approximation:

```glsl
vec2 safeR = max(r, vec2(EPS));
vec2 invR = p / safeR;
vec2 invR2 = p / max(safeR * safeR, vec2(EPS));
float k0 = length(invR);
float k1 = max(length(invR2), EPS);
return k0 * (k0 - 1.0) / k1;
```

This is based on the implicit ellipse field:

```text
F(p) = length(p / r) - 1
```

The signed-distance estimate is:

```text
distance ~= F(p) / |grad F(p)|
```

For this field:

```text
F(p)        = k0 - 1
|grad F(p)| = k1 / k0
```

So:

```text
F(p) / |grad F(p)| = k0 * (k0 - 1) / k1
```

The sign comes from `k0 - 1`: negative inside the ellipse, zero on the boundary,
and positive outside.

## Local Approximation

This is not the exact Euclidean distance to an ellipse in the general case. It
is the distance implied by one first-order Taylor expansion of the implicit
field:

```text
F(p + delta) ~= F(p) + grad F(p) . delta
```

Solving that linearized equation gives the distance to the local tangent line,
not the exact closest point on the curved ellipse.

The approximation is exact for a circle because the circle field has unit
gradient in the radial direction and the closest-point direction is radial
everywhere. For a non-circular ellipse, curvature and normal direction vary, so
the approximation is only guaranteed to be accurate near the boundary.

## Why It Works Well Enough

The formula is still useful for shader SDF effects because it corrects the main
distortion introduced by scaling a circle into an ellipse.

The `k0` term measures how far the point is from the unit circle in normalized
ellipse space. Dividing by `k1 / k0` converts that normalized-space error back
into an approximate pixel distance along the local gradient. In other words, the
shader does not treat the normalized field value as a world-space distance; it
rescales it by how quickly the field changes at the fragment.

This can also be read as the step length of one Newton-style update toward
`F(p) = 0`:

```text
p_new = p - F(p) * grad F(p) / |grad F(p)|^2
step_length = F(p) / |grad F(p)|
```

Near the surface this converges quickly, which is the region most important for
anti-aliased edges, borders, clipping, shadows, and edge lighting. Far from the
surface the value is approximate, but for smooth convex ellipses it usually
degrades gradually.

## Center Case

At the exact center, `k0` is zero and `grad F(p)` is undefined. The shader
returns:

```glsl
return -min(safeR.x, safeR.y);
```

That is the correct signed distance from the center to the nearest point on the
ellipse boundary.

For the normal-capable shader, the center normal also needs a stable fallback.
`ellipseGrad()` returns `vec2(0.0, -1.0)` when the scaled position length is too
small.

## Normal Variant Output Layout

The distance-only variant writes:

```glsl
half4(0, 0, 0, sdf)
```

The normal-capable variant writes:

```glsl
vec4(grad.x, grad.y, packedDir, sdf)
```

The channels are:

- `r` and `g`: normalized ellipse gradient
- `b`: packed auxiliary direction/position data from `posFromCenter`
- `a`: signed distance

Consumers that only need the shape should read the alpha channel. Consumers that
need lighting or directional effects can use the normal channels.

## Error Cases

The approximation is least accurate when the ellipse is highly elongated and the
fragment is far from the boundary, especially around regions where the local
tangent direction differs strongly from the true closest-point direction.

Typical risk cases:

- `width` much larger than `height`
- `height` much larger than `width`
- fragments far away along the high-curvature side of the ellipse

Those cases can change far-field falloff, but they should not affect the most
important property: stable sign and accurate behavior close to the visible edge.

## Performance Notes

The ellipse shader has two variants:

- distance only
- distance + normal

Both are single-pass fragment shaders with a small number of vector operations.
The normal variant adds one gradient evaluation, one normalization, and the
packed direction encode. There is no iterative closest-point solve, which keeps
the implementation cheap and predictable for effects that evaluate the SDF over
large render targets.
