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
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from string import Template
from typing import Dict

# Import clang-format utilities from generate_metadata
sys.path.insert(0, str(Path(__file__).parent.parent))
from generate_metadata.cli_utils import Console, find_clang_format


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
    lines = template_content.split('\n')
    end_idx = 0
    for i, line in enumerate(lines):
        if '*/' in line:
            end_idx = i + 1
            break
    if end_idx > 0 and end_idx < len(lines):
        return '\n'.join(lines[end_idx:])
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


def generate_params_file(name: str, effect_type: str, output_dir: Path, templates_dir: Path) -> Path:
    """Generate .params.in file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    suffix_pascal = to_pascal_case(info['params_suffix'])
    class_name = f"GE{pascal_name}{suffix_pascal}"
    params_class = f"{class_name}Params"

    file_name = f"{to_snake_case(class_name)}.params.in"

    template = load_template(templates_dir / f"{effect_type}.params.tpl")
    template_content = strip_template_copyright(template.template)

    content = get_copyright_header() + Template(template_content).substitute(
        TYPE_ENUM=snake_name.upper(),
        DISPLAY_NAME=pascal_name,
        PARAMS_CLASS=params_class
    )

    params_file = output_dir / file_name
    params_file.write_text(content)
    return params_file


def generate_header_file(name: str, effect_type: str, output_dir: Path, templates_dir: Path) -> Path:
    """Generate .h file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    suffix_pascal = to_pascal_case(info['params_suffix'])
    class_name = f"GE{pascal_name}{suffix_pascal}"
    params_class = f"{class_name}Params"

    file_name = f"ge_{snake_name}{info['params_suffix']}.h"
    header_guard = f"GRAPHICS_EFFECT_{to_snake_case(class_name).upper()}_H"

    template = load_template(templates_dir / f"{effect_type}.h.tpl")
    template_content = strip_template_copyright(template.template)

    content = get_copyright_header() + Template(template_content).substitute(
        HEADER_GUARD=header_guard,
        CLASS_NAME=class_name,
        PARAMS_CLASS=params_class
    )

    header_file = output_dir / file_name
    header_file.write_text(content)
    return header_file


def generate_cpp_file(name: str, effect_type: str, output_dir: Path, templates_dir: Path) -> Path:
    """Generate .cpp file."""
    info = get_effect_info(effect_type)
    snake_name = to_snake_case(name)
    pascal_name = to_pascal_case(name)
    suffix_pascal = to_pascal_case(info['params_suffix'])
    class_name = f"GE{pascal_name}{suffix_pascal}"
    params_class = f"{class_name}Params"

    file_name = f"ge_{snake_name}{info['params_suffix']}.cpp"
    header_file_name = f"ge_{snake_name}{info['params_suffix']}.h"

    template = load_template(templates_dir / f"{effect_type}.cpp.tpl")
    template_content = strip_template_copyright(template.template)

    content = get_copyright_header() + Template(template_content).substitute(
        HEADER_FILE=header_file_name,
        CLASS_NAME=class_name,
        PARAMS_CLASS=params_class
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


def try_add_filter_type_enum(name: str, root_dir: Path, console: Console) -> bool:
    """Try to add enum entry to ge_filter_type.h before MAX."""
    snake_name = to_snake_case(name)
    enum_name = snake_name.upper()
    
    filter_type_file = root_dir / "include" / "core" / "ge_filter_type.h"
    if not filter_type_file.exists():
        console.warning(f"{filter_type_file.relative_to(root_dir)} not found")
        console.step(f"Add {enum_name} enum to include/core/ge_filter_type.h manually")
        return False
    
    content = filter_type_file.read_text()
    
    if re.search(rf'\b{enum_name}\b', content):
        return True
    
    lines = content.split('\n')
    max_line_idx = -1
    for i, line in enumerate(lines):
        if 'MAX,' in line:
            max_line_idx = i
            break
    
    if max_line_idx == -1:
        console.warning(f"Could not find 'MAX,' in ge_filter_type.h")
        console.step(f"Add {enum_name} enum to include/core/ge_filter_type.h manually")
        return False
    
    max_line = lines[max_line_idx]
    indent = len(max_line) - len(max_line.lstrip())
    indent_str = max_line[:indent]
    
    new_enum_line = f"{indent_str}{enum_name},"
    lines.insert(max_line_idx, new_enum_line)
    
    filter_type_file.write_text('\n'.join(lines))
    console.step(f"Added enum {enum_name} to include/core/ge_filter_type.h")
    return True


def format_generated_file(file_path: Path, clang_format_path: str, console: Console) -> bool:
    """Format a generated file using clang-format."""
    try:
        result = subprocess.run(
            [clang_format_path, "-i", str(file_path)],
            capture_output=True,
            text=True,
        )
        if result.returncode == 0:
            return True
        console.warning(f"clang-format failed for {file_path}: {result.stderr}")
        return False
    except Exception as e:
        console.warning(f"Failed to run clang-format: {e}")
        return False


def generate_effect(name: str, effect_type: str, root_dir: Path, templates_dir: Path, add_enum: bool = True) -> bool:
    """Generate all files for a new effect."""
    console = Console()
    
    try:
        output_dir = get_output_dir(effect_type, "include", root_dir)
        src_output_dir = get_output_dir(effect_type, "src", root_dir)

        output_dir.mkdir(parents=True, exist_ok=True)
        src_output_dir.mkdir(parents=True, exist_ok=True)

        console.header(f"Creating {effect_type} effect: {name}")

        params_file = generate_params_file(name, effect_type, output_dir, templates_dir)
        console.file(str(params_file.relative_to(root_dir)))

        header_file = generate_header_file(name, effect_type, output_dir, templates_dir)
        console.file(str(header_file.relative_to(root_dir)))

        cpp_file = generate_cpp_file(name, effect_type, src_output_dir, templates_dir)
        console.file(str(cpp_file.relative_to(root_dir)))

        clang_format_path = find_clang_format()
        if clang_format_path:
            format_generated_file(params_file, clang_format_path, console)
            format_generated_file(header_file, clang_format_path, console)
            format_generated_file(cpp_file, clang_format_path, console)

        if add_enum:
            try_add_filter_type_enum(name, root_dir, console)
        
        console.summary()
        
        if console.error_count == 0:
            if not add_enum:
                console.step(f"Add {to_snake_case(name).upper()} enum to include/core/ge_filter_type.h manually")
            console.info("\nNext steps:")
            console.step(f"Add \"{cpp_file.relative_to(root_dir)}\" to sources in BUILD.gn")
            console.step("Implement shader logic in the generated .cpp file")
            console.step("Run `python tool/generate_metadata/gen_effect_header.py` to generate include")
            console.step("Run `python tool/generate_metadata/gen_metadata.py` to generate param setters")
        
        return console.error_count == 0

    except Exception as e:
        console.error(f"Error generating effect: {e}")
        return False


def main():
    """Parse CLI arguments and generate effect."""
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

    parser.add_argument(
        "--no-enum",
        action="store_true",
        help="Disable automatic enum addition to ge_filter_type.h (useful for testing)"
    )

    args = parser.parse_args()

    if not args.name or not args.type:
        parser.print_help()
        return 1

    return 0 if generate_effect(args.name, args.type, args.root, args.templates, add_enum=not args.no_enum) else 1


if __name__ == "__main__":
    sys.exit(main())
