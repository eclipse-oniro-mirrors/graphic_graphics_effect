#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2026 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
Create GE Effect CLI

Generating new Graphics Effect by templates.
Supports creating filter, mask, shader, and shape effect types with params files.
"""

import argparse
import re
import sys
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional
from string import Template


def get_copyright_header() -> str:
    """Generate copyright header with current year."""
    year = datetime.now().year
    return f"""/*
 * Copyright (c) {year} Huawei Device Co., Ltd.
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


class EffectType:
    FILTER = "filter"
    MASK = "mask"
    SHADER = "shader"
    SHAPE = "shape"

    ALL = [FILTER, MASK, SHADER, SHAPE]


def to_snake_case(name: str) -> str:
    """Convert CamelCase to snake_case."""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def to_pascal_case(name: str) -> str:
    """Convert snake_case to PascalCase."""
    components = name.split('_')
    return ''.join(x.title() for x in components)


def load_template(template_path: Path) -> Template:
    """Load a template file."""
    with open(template_path, 'r') as f:
        return Template(f.read())


def strip_template_copyright(template_content: str) -> str:
    """Strip copyright header from template content.
    
    Tpl files have copyright header for linter compliance, but we generate
    fresh copyright headers when creating actual source files.
    """
    COPYRIGHT_HEADER_LINES = 15
    lines = template_content.split('\n')
    if len(lines) > COPYRIGHT_HEADER_LINES:
        return '\n'.join(lines[COPYRIGHT_HEADER_LINES:])
    return template_content


def get_effect_info(effect_type: str) -> Dict:
    """Get information about an effect type."""
    info = {
        EffectType.FILTER: {
            "base_class": "GEShaderFilter",
            "namespace": "Rosen",
            "params_suffix": "_shader_filter",
        },
        EffectType.MASK: {
            "base_class": "GEShaderMask",
            "namespace": "Drawing",
            "params_suffix": "_shader_mask",
        },
        EffectType.SHADER: {
            "base_class": "GEShader",
            "namespace": "Rosen",
            "params_suffix": "_shader",
        },
        EffectType.SHAPE: {
            "base_class": "GESDFShaderShape",
            "namespace": "Drawing",
            "params_suffix": "_shader_shape",
        },
    }
    return info.get(effect_type, {})


def generate_params_file(name: str, effect_type: str, params: List[Dict], output_dir: Path, templates_dir: Path) -> Path:
    """Generate .params file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    class_name = f"GE{pascal_name}{info['params_suffix'].replace('_', ' ').title().replace(' ', '')}"
    params_class = f"{class_name}Params"

    file_name = f"{to_snake_case(class_name)}.params"

    template = load_template(templates_dir / f"{effect_type}.params.tpl")
    template_content = strip_template_copyright(template.template)

    param_declarations = []
    for param in params:
        param_name = param['name']
        param_type = param['type']
        default_value = param.get('default', "")

        if default_value:
            param_declarations.append(f"    {param_type} {param_name} = {default_value};")
        else:
            param_declarations.append(f"    {param_type} {param_name};")

    content = get_copyright_header() + Template(template_content).substitute(
        TYPE_ENUM=snake_name.upper(),
        DISPLAY_NAME=pascal_name,
        PARAMS_CLASS=params_class,
        PARAM_DECLARATIONS='\n'.join(param_declarations) if param_declarations else ""
    )

    params_file = output_dir / file_name
    params_file.write_text(content)
    return params_file


def generate_header_file(name: str, effect_type: str, params: List[Dict], output_dir: Path, templates_dir: Path) -> Path:
    """Generate .h file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    class_name = f"GE{pascal_name}{info['params_suffix'].replace('_', ' ').title().replace(' ', '')}"
    params_class = f"{class_name}Params"

    file_name = f"ge_{snake_name}{info['params_suffix']}.h"
    header_guard = f"GRAPHICS_EFFECT_{to_snake_case(class_name).upper()}_H"

    template = load_template(templates_dir / f"{effect_type}.h.tpl")
    template_content = strip_template_copyright(template.template)

    member_declarations = []
    for param in params:
        param_name = param['name']
        param_type = param['type']
        member_declarations.append(f"    {param_type} {param_name}_;")

    content = get_copyright_header() + Template(template_content).substitute(
        HEADER_GUARD=header_guard,
        CLASS_NAME=class_name,
        PARAMS_CLASS=params_class,
        MEMBER_DECLARATIONS='\n'.join(member_declarations) if member_declarations else ""
    )

    header_file = output_dir / file_name
    header_file.write_text(content)
    return header_file


def generate_cpp_file(name: str, effect_type: str, params: List[Dict], output_dir: Path, templates_dir: Path) -> Path:
    """Generate .cpp file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    class_name = f"GE{pascal_name}{info['params_suffix'].replace('_', ' ').title().replace(' ', '')}"
    params_class = f"{class_name}Params"

    file_name = f"ge_{snake_name}{info['params_suffix']}.cpp"
    header_file_name = f"ge_{snake_name}{info['params_suffix']}.h"

    template = load_template(templates_dir / f"{effect_type}.cpp.tpl")
    template_content = strip_template_copyright(template.template)

    init_list = []
    for param in params:
        param_name = param['name']
        init_list.append(f"    {param_name}_(params.{param_name})")

    initialization = ""
    if init_list:
        initialization = ":\n" + ',\n'.join(init_list)

    content = get_copyright_header() + Template(template_content).substitute(
        HEADER_FILE=header_file_name,
        CLASS_NAME=class_name,
        PARAMS_CLASS=params_class,
        INITIALIZATION_LIST=initialization
    )

    cpp_file = output_dir / file_name
    cpp_file.write_text(content)
    return cpp_file


def get_output_dir(effect_type: str, target: str, root_dir: Path) -> Path:
    """Get the output directory for a given effect type."""
    if effect_type == EffectType.FILTER:
        return root_dir / target / "effect" / "filter"
    elif effect_type == EffectType.MASK:
        return root_dir / target / "effect" / "mask"
    elif effect_type == EffectType.SHADER:
        return root_dir / target / "effect" / "shader"
    elif effect_type == EffectType.SHAPE:
        return root_dir / target / "effect" / "shape"
    else:
        raise ValueError(f"Unknown effect type: {effect_type}")


def generate_effect(name: str, effect_type: str, params: List[Dict], root_dir: Path, templates_dir: Path) -> bool:
    """Generate all files for a new effect."""
    try:
        output_dir = get_output_dir(effect_type, "include", root_dir)
        src_output_dir = get_output_dir(effect_type, "src", root_dir)

        output_dir.mkdir(parents=True, exist_ok=True)
        src_output_dir.mkdir(parents=True, exist_ok=True)

        print(f"Generating {effect_type} effect: {name}")
        print(f"- Header Output directory: {output_dir}")

        params_file = generate_params_file(name, effect_type, params, output_dir, templates_dir)
        print(f"  Generated: {params_file.name}")

        header_file = generate_header_file(name, effect_type, params, output_dir, templates_dir)
        print(f"  Generated: {header_file.name}")

        print(f"- Source Output directory: {src_output_dir}")
        cpp_file = generate_cpp_file(name, effect_type, params, src_output_dir, templates_dir)
        print(f"  Generated: {cpp_file.name}")

        print(f"\nSuccessfully generated {name} {effect_type} effect!\n")
        print(f"Hint: run `python tool/generate_metadata/gen_effect_header.py` to generate include for your params")
        print(f"      run `python tool/generate_metadata/gen_metadata.py` to generate param setters for your params")
        return True

    except Exception as e:
        print(f"Error generating effect: {e}", file=sys.stderr)
        return False


def main():
    parser = argparse.ArgumentParser(
        description="Create GE Effects - Create new Graphics Effect by templates",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a filter effect
  python create_effect.py my_blur filter

  # Create a mask effect
  python create_effect.py my_gradient mask

  # Create a shader effect
  python create_effect.py my_light shader

  # Create a shape effect
  python create_effect.py my_shape shape

Available effect types:
  filter  - Shader-based image processing filters (GEShaderFilter)
  mask    - Masking operations (GEShaderMask)
  shader  - Direct shader effects (GEShader)
  shape   - Shape-based effects (GESDFShaderShape)
        """
    )

    parser.add_argument(
        "name",
        nargs="?",
        help="Name of the effect (e.g., 'my_blur', 'my_gradient')"
    )

    parser.add_argument(
        "type",
        nargs="?",
        choices=EffectType.ALL,
        help="Type of effect to create"
    )

    parser.add_argument(
        "--root",
        type=Path,
        default=Path(__file__).parent.parent.parent,
        help="Root directory of the graphics_effect project (default: parent of tool directory)"
    )

    parser.add_argument(
        "--templates",
        type=Path,
        default=Path(__file__).parent / "templates",
        help="Directory containing template files (default: tool/templates)"
    )

    args = parser.parse_args()

    if not args.name or not args.type:
        parser.print_help()
        return 1

    params = []
    return 0 if generate_effect(args.name, args.type, params, args.root, args.templates) else 1


if __name__ == "__main__":
    sys.exit(main())
