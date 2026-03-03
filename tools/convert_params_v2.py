#!/usr/bin/env python3
"""
Convert GE Shader Filter Params from old format to new .def format.

Improved version with better property name extraction.
"""

import re
import os
from pathlib import Path
from typing import Dict, Tuple, List, Optional

# Copyright header for generated files
COPYRIGHT_HEADER = """/*
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
"""

# List of all params to extract, with (struct_name, enum_value, filter_name_constant)
PARAMS_DEFINITIONS = [
    ("GEAIBarShaderFilterParams", "AIBAR", "GE_FILTER_AI_BAR"),
    ("GEWaterRippleFilterParams", "WATER_RIPPLE", "GE_FILTER_WATER_RIPPLE"),
    ("GESoundWaveFilterParams", "SOUND_WAVE", "GE_FILTER_SOUND_WAVE"),
    ("GEGreyShaderFilterParams", "GREY", "GE_FILTER_GREY"),
    ("GEKawaseBlurShaderFilterParams", "KAWASE_BLUR", "GE_FILTER_KAWASE_BLUR"),
    ("GEMESABlurShaderFilterParams", "MESA_BLUR", "GE_FILTER_MESA_BLUR"),
    ("GELinearGradientBlurShaderFilterParams", "LINEAR_GRADIENT_BLUR", "GE_FILTER_LINEAR_GRADIENT_BLUR"),
    ("GESDFFromImageFilterParams", "SDF_FROM_IMAGE", "GE_FILTER_SDF_FROM_IMAGE"),
    ("GEMagnifierShaderFilterParams", "MAGNIFIER", "GE_FILTER_MAGNIFIER"),
    ("GESDFEdgeLightFilterParams", "SDF_EDGE_LIGHT", "GE_FILTER_SDF_EDGE_LIGHT"),
    ("GERippleShaderMaskParams", "RIPPLE_MASK", "GE_MASK_RIPPLE"),
    ("GEDoubleRippleShaderMaskParams", "DOUBLE_RIPPLE_MASK", "GE_MASK_DOUBLE_RIPPLE"),
    ("GEDisplacementDistortFilterParams", "DISPLACEMENT_DISTORT_FILTER", "GE_FILTER_DISPLACEMENT_DISTORT"),
    ("GEColorGradientShaderFilterParams", "COLOR_GRADIENT", "GE_FILTER_COLOR_GRADIENT"),
    ("GEImageMaskParams", "IMAGE_MASK", "GE_MASK_IMAGE"),
    ("GEEdgeLightShaderFilterParams", "EDGE_LIGHT", "GE_FILTER_EDGE_LIGHT"),
    ("GEBezierWarpShaderFilterParams", "BEZIER_WARP", "GE_FILTER_BEZIER_WARP"),
    ("GEPixelMapMaskParams", "PIXEL_MAP_MASK", "GE_MASK_PIXEL_MAP"),
    ("GEDispersionShaderFilterParams", "DISPERSION", "GE_FILTER_DISPERSION"),
    ("GEDirectionLightShaderFilterParams", "DIRECTION_LIGHT", "GE_FILTER_DIRECTION_LIGHT"),
    ("GERadialGradientShaderMaskParams", "RADIAL_GRADIENT_MASK", "GE_MASK_RADIAL_GRADIENT"),
    ("GEWaveGradientShaderMaskParams", "WAVE_GRADIENT_MASK", "GE_MASK_WAVE_GRADIENT"),
    ("GEContentLightFilterParams", "CONTENT_LIGHT", "GE_FILTER_CONTENT_LIGHT"),
    ("GEContentDiagonalFlowLightShaderParams", "CONTOUR_DIAGONAL_FLOW_LIGHT", "GE_SHADER_CONTOUR_DIAGONAL_FLOW_LIGHT"),
    ("GEDotMatrixShaderParams", "DOT_MATRIX", "GE_SHADER_DOT_MATRIX_SHADER"),
    ("GEWavyRippleLightShaderParams", "WAVY_RIPPLE_LIGHT", "GE_SHADER_WAVY_RIPPLE_LIGHT"),
    ("GEAuroraNoiseShaderParams", "AURORA_NOISE", "GE_SHADER_AURORA_NOISE"),
    ("GEParticleCircularHaloShaderParams", "PARTICLE_CIRCULAR_HALO", "GE_SHADER_PARTICLE_CIRCULAR_HALO"),
    ("GEMaskTransitionShaderFilterParams", "MASK_TRANSITION", "GE_FILTER_MASK_TRANSITION"),
    ("GEVariableRadiusBlurShaderFilterParams", "VARIABLE_RADIUS_BLUR", "GE_FILTER_VARIABLE_RADIUS_BLUR"),
    ("GELinearGradientShaderMaskParams", "LINEAR_GRADIENT_MASK", "GE_MASK_LINEAR_GRADIENT"),
    ("GEMapColorByBrightnessFilterParams", "MAP_COLOR_BY_BRIGHTNESS", "GE_FILTER_MAP_COLOR_BY_BRIGHTNESS"),
    ("GEXColorGradientEffectParams", "COLOR_GRADIENT_EFFECT", "GEX_SHADER_COLOR_GRADIENT_EFFECT"),
    ("GEFrostedGlassEffectParams", "FROSTED_GLASS_EFFECT", "GE_SHADER_FROSTED_GLASS_EFFECT"),
    ("GEFrostedGlassBlurShaderFilterParams", "FROSTED_GLASS_BLUR", "GE_FILTER_FROSTED_GLASS_BLUR"),
    ("GEXLightCaveShaderParams", "LIGHT_CAVE", "GEX_SHADER_LIGHT_CAVE"),
    ("GEXDupoliNoiseMaskParams", "DUPOLI_NOISE_MASK", "GEX_MASK_DUPOLI_NOISE"),
    ("GEXDistortChromaEffectParams", "DISTORT_CHROMA", "GEX_SHADER_DISTORT_CHROMA"),
    ("GEBorderLightShaderParams", "BORDER_LIGHT", "GE_SHADER_BORDER_LIGHT"),
    ("GEGridWarpShaderFilterParams", "GRID_WARP", "GE_FILTER_GRID_WARP"),
    ("GECircleFlowlightEffectParams", "CIRCLE_FLOWLIGHT", "GE_SHADER_CIRCLE_FLOWLIGHT"),
    ("GEXAIBarRectHaloEffectParams", "AIBAR_RECT_HALO", "GEX_SHADER_AIBAR_RECT_HALO"),
    ("GEFrostedGlassShaderFilterParams", "FROSTED_GLASS", "GE_FILTER_FROSTED_GLASS"),
    ("GEXNoisyFrameGradientMaskParams", "NOISY_FRAME_GRADIENT_MASK", "GEX_MASK_NOISY_FRAME_GRADIENT"),
]


def camel_to_upper(name: str) -> str:
    """Convert camelCase to UPPER_CASE."""
    result = []
    for i, c in enumerate(name):
        if c.isupper():
            if result:
                result.append('_')
            result.append(c)
        else:
            result.append(c.upper())
    return ''.join(result)


def to_snake_case(name: str) -> str:
    """Convert CamelCase to snake_case, keeping consecutive capitals together."""
    # Handle consecutive capitals - keep them together
    result = []
    i = 0
    while i < len(name):
        # Find consecutive capitals
        if name[i].isupper():
            j = i + 1
            while j < len(name) and name[j].isupper() and j < i + 4:  # Limit to 4 consecutive
                j += 1
            # If next char is lowercase, we have an acronym
            if j < len(name) and name[j].islower() and j > i + 1:
                # This is an acronym (e.g., "AIBAR" in "GEXAIBAR...")
                result.append(name[i:j].lower())
                i = j
            else:
                # Single capital or end of consecutive
                if i > 0 and name[i-1].islower():
                    result.append('_')
                result.append(name[i].lower())
                i += 1
        else:
            result.append(name[i].lower())
            i += 1
    return ''.join(result)


def extract_property_mapping(content: str, struct_name: str, filter_name_const: str) -> Dict[str, str]:
    """Extract property name to constant value mapping for a struct.

    Returns a dict mapping field names to their actual property string values.
    """
    # Find the struct position
    struct_pattern = r'struct ' + re.escape(struct_name) + r'\s*\{'
    struct_match = re.search(struct_pattern, content)
    if not struct_match:
        return {}

    struct_pos = struct_match.start()

    # Find all property constants before this struct
    # Pattern: constexpr char CONSTANT_NAME[] = "VALUE";
    prop_pattern = r'constexpr\s+char\s+(\w+)\[\]\s*=\s*"([^"]+)"'

    # Extract all constants before the struct
    before_struct = content[:struct_pos]
    all_constants = re.findall(prop_pattern, before_struct)

    # Build mapping from constant name to value
    const_to_value = dict(all_constants)

    # Build field name to property value mapping
    field_to_prop = {}

    # First, find the filter name value (e.g., "MESA_BLUR")
    filter_value = const_to_value.get(filter_name_const, "")

    # Now try to match fields to property constants
    # For each field, we'll look for a property constant that might match
    for const_name, prop_value in all_constants:
        # Skip the main filter name constant
        if const_name == filter_name_const:
            continue

        # The property constants typically follow patterns like:
        # GE_FILTER_XXX_FIELD -> value
        # We need to extract the field part and try to match it to struct fields

        # Extract the suffix after the filter name prefix
        # e.g., GE_FILTER_MESA_BLUR_RADIUS -> RADIUS (when filter is GE_FILTER_MESA_BLUR)
        if filter_name_const and const_name.startswith(filter_name_const + '_'):
            field_part = const_name[len(filter_name_const) + 1:]
            # Convert to camelCase
            # RADIUS -> radius
            # GREY_COEF_1 -> greyCoef1
            field_name_guess = field_part.lower()
            # Handle underscores to camelCase
            parts = field_name_guess.split('_')
            if len(parts) > 1:
                field_name_guess = parts[0] + ''.join(p.capitalize() for p in parts[1:])
            field_to_prop[field_name_guess] = prop_value
        elif filter_name_const and filter_value and prop_value.startswith(filter_value + '_'):
            # Property value starts with filter value (e.g., "MESA_BLUR_RADIUS")
            # Extract the field part
            field_part = prop_value[len(filter_value) + 1:]
            field_name_guess = field_part.lower()
            parts = field_name_guess.split('_')
            if len(parts) > 1:
                field_name_guess = parts[0] + ''.join(p.capitalize() for p in parts[1:])
            field_to_prop[field_name_guess] = prop_value
        elif '_' in prop_value:
            # Property value has underscores but doesn't start with filter prefix
            # e.g., "OFFSET_X" -> try both "offsetX" and "offset_X"
            field_name_guess = prop_value.lower()
            parts = field_name_guess.split('_')
            if len(parts) > 1:
                field_name_guess = parts[0] + ''.join(p.capitalize() for p in parts[1:])
            # Add to mapping (prefer simple field names)
            if field_name_guess not in field_to_prop:
                field_to_prop[field_name_guess] = prop_value
            # Also add the version with underscores for fuzzy matching
            field_name_with_underscores = prop_value.lower()
            if field_name_with_underscores not in field_to_prop:
                field_to_prop[field_name_with_underscores] = prop_value
        elif '_' in const_name:
            # Try to extract field name from constant name
            # Remove common prefixes
            suffix = const_name
            for prefix in ['GE_FILTER_', 'GE_SHADER_', 'GEX_SHADER_', 'GE_MASK_', 'GEX_MASK_']:
                if suffix.startswith(prefix):
                    suffix = suffix[len(prefix):]
                    break
            # Try to match to field
            if suffix:
                field_name_guess = suffix.lower()
                parts = field_name_guess.split('_')
                if len(parts) > 1:
                    field_name_guess = parts[0] + ''.join(p.capitalize() for p in parts[1:])
                # Only add if not already present
                if field_name_guess not in field_to_prop:
                    field_to_prop[field_name_guess] = prop_value

    return field_to_prop


def extract_struct_content(content: str, struct_name: str, filter_name_const: str) -> Tuple[Optional[List[str]], Dict[str, str], List[str]]:
    """Extract struct definition and property names from the header file content.

    Returns:
        (struct_body_lines, prop_name_map, related_constants)
    """
    # Find the struct definition using brace counting instead of regex
    # This handles nested braces like Color4f color = {1.0, 1.0, 1.0, 1.0};
    struct_start_pattern = r'struct ' + re.escape(struct_name) + r'\s*\{'
    struct_start_match = re.search(struct_start_pattern, content)

    if not struct_start_match:
        return None, {}, []

    # Find the struct body by counting braces
    start_pos = struct_start_match.end() - 1  # Position of opening brace
    brace_count = 0
    i = start_pos

    while i < len(content):
        if content[i] == '{':
            brace_count += 1
        elif content[i] == '}':
            brace_count -= 1
            if brace_count == 0:
                # Found the closing brace of the struct
                struct_body = content[start_pos + 1:i]
                break
        i += 1
    else:
        # Couldn't find matching closing brace
        return None, {}, []

    body = struct_body
    lines = []
    prop_map = {}
    related_constants = []

    # Extract non-string constexpr constants that are used in this struct
    # Pattern: constexpr int/size_t TYPE_CONST_NAME = value;
    # or: constexpr int SPECIFIC_CONST_NAME = value; // comment
    const_pattern = r'constexpr\s+(?:int|size_t|uint32_t|float|double)\s+(\w+)\s*=\s*([^;]+);'
    for const_match in re.finditer(const_pattern, content):
        const_name = const_match.group(1)
        const_type = const_match.group(0).split()[1]  # int, size_t, etc.
        const_value = const_match.group(2).strip()
        const_def = const_match.group(0)

        # Check if this constant is used in the struct body
        if const_name in struct_body:
            # Add comment if present
            full_line_start = content.rfind('\n', 0, const_match.start()) + 1
            full_line_end = content.find('\n', const_match.end())
            full_line = content[full_line_start:full_line_end].strip()

            related_constants.append(full_line)

    # Get property mapping
    field_to_prop = extract_property_mapping(content, struct_name, filter_name_const)

    # Process each line in the struct body
    for line in body.split('\n'):
        stripped = line.strip()
        if not stripped or stripped.startswith('//'):
            lines.append(line)
            continue

        # Extract field name
        field_match = re.match(
            r'(?:.*?\s+)?:?\s*(\w+)\s*(?:\[[^\]]*\])?\s*(?:=\s*[^;]+)?\s*;',
            stripped
        )

        if field_match:
            field_name = field_match.group(1)
            # Skip common keywords
            if field_name in ['public', 'private', 'protected', 'struct', 'class']:
                lines.append(line)
                continue

            # Generate expected property name
            expected_prop = camel_to_upper(field_name)

            # Check if we have a mapping for this field
            if field_name in field_to_prop:
                actual_prop = field_to_prop[field_name]
                # Compare by removing underscores from both (they're semantically the same)
                expected_normalized = expected_prop.replace('_', '')
                actual_normalized = actual_prop.upper().replace('_', '')
                if actual_normalized != expected_normalized:
                    prop_map[field_name] = actual_prop
            else:
                # No direct mapping found, check if there's a close match
                # Try lowercase version, and versions with/without underscores
                field_lower = field_name.lower()
                for key, value in field_to_prop.items():
                    key_lower = key.lower()
                    if key_lower == field_lower or key.replace('_', '').lower() == field_lower:
                        actual_prop = value
                        expected_normalized = expected_prop.replace('_', '')
                        actual_normalized = actual_prop.upper().replace('_', '')
                        if actual_normalized != expected_normalized:
                            prop_map[field_name] = actual_prop
                        break

            lines.append(line)
        else:
            lines.append(line)

    return lines, prop_map, related_constants


def convert_field_line(line: str, prop_map: dict) -> str:
    """Convert a field line to include ge::prop if needed."""
    stripped = line.strip()

    # Skip empty lines and comments
    if not stripped or stripped.startswith('//'):
        return line

    # Extract field name
    field_match = re.match(
        r'(?:.*?\s+)?:?\s*(\w+)\s*(?:\[[^\]]*\])?\s*(?:=\s*[^;]+)?\s*;',
        stripped
    )

    if not field_match:
        return line

    field_name = field_match.group(1)

    # Skip common keywords
    if field_name in ['public', 'private', 'protected', 'struct', 'class']:
        return line

    # Check if we need a custom property name
    if field_name in prop_map:
        indent = len(line) - len(line.lstrip())
        prop_attr = f'[[ge::prop("{prop_map[field_name]}")]]\n'
        return ' ' * indent + prop_attr + line

    return line


def extract_string_value(content: str, constant_name: str) -> str:
    """Extract the actual string value from a constant definition."""
    pattern = r'constexpr\s+char\s+' + re.escape(constant_name) + r'\[\]\s*=\s*"([^"]+)";'
    match = re.search(pattern, content)
    if match:
        return match.group(1)
    return constant_name  # Fallback to constant name if not found


def generate_def_file(struct_name: str, enum_value: str, filter_name: str,
                      struct_lines: list, prop_map: dict, related_constants: list = None) -> Tuple[str, str]:
    """Generate the .def file content."""

    # Convert struct name to file name
    # GEAIBarShaderFilterParams -> ge_aibar_shader_filter_params.def
    # GEXColorGradientEffectParams -> gex_color_gradient_effect_params.def
    file_prefix = "ge_"
    file_name_base = struct_name
    if file_name_base.startswith('GEX'):
        file_name_base = file_name_base[3:]
        file_prefix = "gex_"
    elif file_name_base.startswith('GE'):
        file_name_base = file_name_base[2:]

    file_name_base = to_snake_case(file_name_base)
    file_name = f"{file_prefix}{file_name_base}.def"

    output = []
    output.append(COPYRIGHT_HEADER)
    output.append("")

    # Output related constants before the struct definition
    if related_constants:
        for const_def in related_constants:
            output.append(const_def)
        output.append("")

    output.append(f'[[ge::params({{ type: {enum_value}, name: "{filter_name}" }})]]')
    output.append(f"struct {struct_name} {{")

    for line in struct_lines:
        stripped = line.strip()

        # Skip empty lines and comments at start
        if not stripped or (stripped.startswith('//') and not output[-1].startswith('[')):
            output.append(line)
            continue

        # Check if this is a field declaration
        if stripped.endswith(';') and 'struct' not in stripped.lower():
            converted = convert_field_line(line, prop_map)
            if converted != line:
                # Line was modified (has prop attribute)
                for converted_line in converted.split('\n'):
                    output.append(converted_line)
            else:
                output.append(line)
        else:
            output.append(line)

    output.append("};")
    output.append("")

    return '\n'.join(output), file_name


def main():
    """Main conversion function."""
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    header_file = root_dir / "include" / "ge_shader_filter_params.h"
    params_dir = root_dir / "params"

    # Ensure params directory exists
    params_dir.mkdir(exist_ok=True)

    # Read original header
    with open(header_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Process each params definition
    for struct_name, enum_value, filter_name_const in PARAMS_DEFINITIONS:
        print(f"Processing {struct_name}...")

        # Extract struct body
        struct_lines, prop_map, related_constants = extract_struct_content(content, struct_name, filter_name_const)

        if struct_lines is None:
            print(f"  WARNING: Could not find struct {struct_name}")
            continue

        # Extract the actual filter name string value
        filter_name_value = extract_string_value(content, filter_name_const)

        # Generate .def file content
        def_content, file_name = generate_def_file(
            struct_name, enum_value, filter_name_value, struct_lines, prop_map, related_constants
        )

        # Write to file
        output_path = params_dir / file_name
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(def_content)

        print(f"  Generated: {file_name}")

    print("\nConversion complete!")
    print(f"Generated {len(PARAMS_DEFINITIONS)} .def files in {params_dir}")


if __name__ == '__main__':
    main()
