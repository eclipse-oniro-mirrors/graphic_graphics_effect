# SDF RRect Notes

## Variant Selection

There are four shader variants:

- Uniform radius, distance only
- Uniform radius, distance + normal
- Per-corner radius, distance only
- Per-corner radius, distance + normal

The fast path is only used when the rounded rect is both:

- uniform across all corners
- circular, not elliptical

If any corner differs, or if any corner uses different `x/y` radii, the code
falls back to the more expensive per-corner shader.

## CPU-side Radius Adjustment

Two CPU-side details matter more than they first appear:

### `EXTEND = 0.5`

The shader does not use the raw input size and radii directly. The CPU adds
`EXTEND` to `halfSize` and to the resolved radii before binding uniforms.

This is a bias to better match Skia's rounded-rect edge placement. If a render
looks off by about half a pixel, this is the first thing to remember.

### Per-corner radii are normalized as a group

`ResolveCornerRadii()` does more than clamp each corner independently. It also
scales radii like CSS border-radius resolution when opposite corners would
otherwise overflow the rect width or height.

This means:

- a corner can be valid by itself but still get reduced
- the shader may not receive exactly the same radii that were originally set

## Per-corner Radius Selection

The per-corner shader chooses the active radius from the sign of the fragment
position relative to the center, not from any explicit corner index.

That means asymmetric test cases like `TL=0, TR=30, BR=0, BL=30` are a good way
to validate the quadrant mapping visually.

## Normal Variant Output Layout

The normal-capable shaders do not output a color image. They pack data as:

`vec4(normal.x, normal.y, packedDir, sdf)`

The important consequence is:

- `r` and `g` are the 2D normal
- `b` is packed auxiliary direction/position data
- `a` is the signed distance

If the result is viewed as a normal RGBA image, it can look "wrong" or "boxy"
even when the SDF in `a` is correct. Consumers that only need the shape should
read `a`.

## Why The Interior Normal Looks Flat In Regions

Inside the core rectangle, the shader does not compute a smooth vector pointing
to the nearest edge. It picks a dominant axis instead.

That creates a piecewise-constant interior normal field with wedge-like regions.
This is intentional in the current implementation. If a future effect needs a
fully smooth interior normal, this is the place that would need to change.

## Performance Notes

The uniform-radius shaders are clearly cheaper than the per-corner shaders.
The main reason is that the per-corner normal path always evaluates extra corner
math for every fragment:

- corner selection
- ellipse distance in `sdEllipse()`
- ellipse gradient in `ellipseGrad()`

Because those values are computed before the final `mix()`, even center pixels
pay for the ellipse path.

A practical estimate:

- Per-corner normal vs uniform normal: about `1.7x` to `2.5x` fragment ALU cost
- Per-corner distance-only vs uniform distance-only: about `1.5x` to `2.0x`

These are source-level estimates, not measured GPU cycle counts. Real hardware
cost depends on the backend shader compiler and the target GPU.
