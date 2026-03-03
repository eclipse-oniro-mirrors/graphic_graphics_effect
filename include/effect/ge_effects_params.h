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

#include <memory>
#include <vector>
#include <array>
#include <utility>
#include <optional>
#include <variant>
#include <type_traits>

// Common types
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
// Drawing types
#include "utils/matrix.h"
// GE types
#include "ge_filter_type.h"
#include "ge_shader_mask.h"
#include "sdf/ge_sdf_shader_shape.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Parameter structure definitions
#include "effect/params/ge_aibar_shader_filter_params.def"
#include "effect/params/ge_aurora_noise_shader_params.def"
#include "effect/params/ge_bezier_warp_shader_filter_params.def"
#include "effect/params/ge_border_light_shader_params.def"
#include "effect/params/ge_circle_flowlight_effect_params.def"
#include "effect/params/ge_color_gradient_shader_filter_params.def"
#include "effect/params/ge_content_diagonal_flow_light_shader_params.def"
#include "effect/params/ge_content_light_filter_params.def"
#include "effect/params/ge_direction_light_shader_filter_params.def"
#include "effect/params/ge_dispersion_shader_filter_params.def"
#include "effect/params/ge_displacement_distort_filter_params.def"
#include "effect/params/ge_dot_matrix_shader_params.def"
#include "effect/params/ge_double_ripple_shader_mask_params.def"
#include "effect/params/ge_edge_light_shader_filter_params.def"
#include "effect/params/ge_frame_gradient_mask_params.def"
#include "effect/params/ge_frosted_glass_blur_shader_filter_params.def"
#include "effect/params/ge_frosted_glass_effect_params.def"
#include "effect/params/ge_frosted_glass_shader_filter_params.def"
#include "effect/params/ge_gasify_blur_filter_params.def"
#include "effect/params/ge_gasify_filter_params.def"
#include "effect/params/ge_gasify_scale_twist_filter_params.def"
#include "effect/params/ge_grey_shader_filter_params.def"
#include "effect/params/ge_grid_warp_shader_filter_params.def"
#include "effect/params/ge_harmonium_effect_shader_params.def"
#include "effect/params/ge_image_mask_params.def"
#include "effect/params/ge_kawase_blur_shader_filter_params.def"
#include "effect/params/ge_linear_gradient_blur_shader_filter_params.def"
#include "effect/params/ge_linear_gradient_shader_mask_params.def"
#include "effect/params/ge_magnifier_shader_filter_params.def"
#include "effect/params/ge_map_color_by_brightness_filter_params.def"
#include "effect/params/ge_mask_transition_shader_filter_params.def"
#include "effect/params/ge_mesablur_shader_filter_params.def"
#include "effect/params/ge_particle_circular_halo_shader_params.def"
#include "effect/params/ge_pixel_map_mask_params.def"
#include "effect/params/ge_radial_gradient_shader_mask_params.def"
#include "effect/params/ge_ripple_shader_mask_params.def"
#include "effect/params/ge_sdf_border_shader_params.def"
#include "effect/params/ge_sdf_clip_shader_params.def"
#include "effect/params/ge_sdf_color_shader_params.def"
#include "effect/params/ge_sdf_edge_light_filter_params.def"
#include "effect/params/ge_sdf_from_image_filter_params.def"
#include "effect/params/ge_sdf_pixelmap_shape_params.def"
#include "effect/params/ge_sdf_rrect_shape_params.def"
#include "effect/params/ge_sdf_shadow_shader_params.def"
#include "effect/params/ge_sdf_transform_shape_params.def"
#include "effect/params/ge_sdf_union_op_shape_params.def"
#include "effect/params/ge_sound_wave_filter_params.def"
#include "effect/params/ge_use_effect_mask_params.def"
#include "effect/params/ge_variable_radius_blur_shader_filter_params.def"
#include "effect/params/ge_water_ripple_filter_params.def"
#include "effect/params/ge_wave_gradient_shader_mask_params.def"
#include "effect/params/ge_wavy_ripple_light_shader_params.def"
#include "effect/params/gex_aibar_glow_effect_params.def"
#include "effect/params/gex_aibar_rect_halo_effect_params.def"
#include "effect/params/gex_color_gradient_effect_params.def"
#include "effect/params/gex_distort_chroma_effect_params.def"
#include "effect/params/gex_dupoli_noise_mask_params.def"
#include "effect/params/gex_gradient_flow_colors_effect_params.def"
#include "effect/params/gex_light_cave_shader_params.def"
#include "effect/params/gex_noisy_frame_gradient_mask_params.def"
#include "effect/params/gex_rounded_rect_flowlight_effect_params.def"

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H