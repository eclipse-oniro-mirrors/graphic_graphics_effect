/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H
#define GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H

#include <array>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// Common types
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
// Drawing types
#include "utils/matrix.h"
// GE types
#include "ge_filter_type.h"
#include "ge_params_associate_types.h"
#include "ge_sdf_shader_shape.h"
#include "ge_shader_mask.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif

// Parameter structure definitions
#include "effect/filter/ge_aibar_shader_filter.params"
#include "effect/filter/ge_bezier_warp_shader_filter.params"
#include "effect/filter/ge_blur_bubbles_rise_filter.params"
#include "effect/filter/ge_color_gradient_shader_filter.params"
#include "effect/filter/ge_content_light_shader_filter.params"
#include "effect/filter/ge_direction_light_shader_filter.params"
#include "effect/filter/ge_dispersion_shader_filter.params"
#include "effect/filter/ge_displacement_distort_shader_filter.params"
#include "effect/filter/ge_distortion_collapse_filter.params"
#include "effect/filter/ge_edge_light_shader_filter.params"
#include "effect/filter/ge_frosted_glass_blur_shader_filter.params"
#include "effect/filter/ge_frosted_glass_shader_filter.params"
#include "effect/filter/ge_gasify_blur_filter.params"
#include "effect/filter/ge_gasify_filter.params"
#include "effect/filter/ge_gasify_scale_twist_filter.params"
#include "effect/filter/ge_grey_shader_filter.params"
#include "effect/filter/ge_grid_warp_shader_filter.params"
#include "effect/filter/ge_heat_distortion_filter.params"
#include "effect/filter/ge_kawase_blur_shader_filter.params"
#include "effect/filter/ge_linear_gradient_blur_shader_filter.params"
#include "effect/filter/ge_magnifier_shader_filter.params"
#include "effect/filter/ge_map_color_by_brightness_filter.params"
#include "effect/filter/ge_mask_transition_shader_filter.params"
#include "effect/filter/ge_mesa_blur_shader_filter.params"
#include "effect/filter/ge_sdf_edge_light_filter.params"
#include "effect/filter/ge_sdf_from_image_filter.params"
#include "effect/filter/ge_sound_wave_filter.params"
#include "effect/filter/ge_variable_radius_blur_shader_filter.params"
#include "effect/filter/ge_water_droplet_transition_filter.params"
#include "effect/filter/ge_water_ripple_filter.params"
#include "effect/mask/ge_double_ripple_shader_mask.params"
#include "effect/mask/ge_frame_gradient_shader_mask.params"
#include "effect/mask/ge_image_shader_mask.params"
#include "effect/mask/ge_linear_gradient_shader_mask.params"
#include "effect/mask/ge_pixel_map_shader_mask.params"
#include "effect/mask/ge_radial_gradient_shader_mask.params"
#include "effect/mask/ge_ripple_shader_mask.params"
#include "effect/mask/ge_use_effect_shader_mask.params"
#include "effect/mask/ge_wave_disturb_shader_mask.params"
#include "effect/mask/ge_wave_gradient_shader_mask.params"
#include "effect/mask/gex_dupoli_noise_mask.params"
#include "effect/mask/gex_noisy_frame_gradient_mask.params"
#include "effect/shader/ge_aurora_noise_shader.params"
#include "effect/shader/ge_border_light_shader.params"
#include "effect/shader/ge_circle_flowlight_effect.params"
#include "effect/shader/ge_contour_diagonal_flow_light_shader.params"
#include "effect/shader/ge_dot_matrix_shader.params"
#include "effect/shader/ge_frosted_glass_effect.params"
#include "effect/shader/ge_harmonium_effect_shader.params"
#include "effect/shader/ge_particle_circular_halo_shader.params"
#include "effect/shader/ge_sdf_edge_light_effect.params"
#include "effect/shader/ge_wavy_ripple_light_shader.params"
#include "effect/shader/gex_aibar_glow_effect.params"
#include "effect/shader/gex_aibar_rect_halo_effect.params"
#include "effect/shader/gex_color_gradient_effect.params"
#include "effect/shader/gex_distort_chroma_effect.params"
#include "effect/shader/gex_gradient_flow_colors_effect.params"
#include "effect/shader/gex_light_cave_shader.params"
#include "effect/shader/gex_rounded_rect_flowlight_effect.params"
#include "effect/shape/ge_sdf_border_shader.params"
#include "effect/shape/ge_sdf_clip_shader.params"
#include "effect/shape/ge_sdf_color_shader.params"
#include "effect/shape/ge_sdf_distort_op_shader_shape.params"
#include "effect/shape/ge_sdf_pixelmap_shader_shape.params"
#include "effect/shape/ge_sdf_rrect_shader_shape.params"
#include "effect/shape/ge_sdf_shadow_shader.params"
#include "effect/shape/ge_sdf_transform_shader_shape.params"
#include "effect/shape/ge_sdf_triangle_shape.params"
#include "effect/shape/ge_sdf_union_op_shape.params"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H