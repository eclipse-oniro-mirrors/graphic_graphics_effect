#!/usr/bin/env python3
"""
Generate ge_effects_params.h header file.

This script generates a header file that includes all .params files from effect subdirectories.
The file is designed to be manually editable if needed.
"""

import re
from pathlib import Path
from typing import List

# Copyright header
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


def get_params_files(params_dirs: List[Path]) -> List[Path]:
    params_files = []
    for params_dir in params_dirs:
        params_files.extend(list(params_dir.glob("*.params")))
    return sorted(params_files, key=lambda p: p.name)


def generate_header() -> str:
    """Generate the complete ge_effects_params.h header file."""
    output = []

    output.append(COPYRIGHT_HEADER)
    output.append("")
    output.append("#ifndef GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H")
    output.append("#define GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H")
    output.append("")
    output.append("#include <memory>")
    output.append("#include <vector>")
    output.append("#include <array>")
    output.append("#include <utility>")
    output.append("#include <optional>")
    output.append("#include <variant>")
    output.append("#include <type_traits>")
    output.append("")
    output.append("// Common types")
    output.append('#include "common/rs_vector2.h"')
    output.append('#include "common/rs_vector3.h"')
    output.append('#include "common/rs_vector4.h"')
    output.append("// Drawing types")
    output.append('#include "utils/matrix.h"')
    output.append("// GE types")
    output.append('#include "ge_filter_type.h"')
    output.append('#include "ge_shader_mask.h"')
    output.append('#include "ge_sdf_shader_shape.h"')
    output.append('#include "ge_params_associate_types.h"')
    output.append("")
    output.append("namespace OHOS {")
    output.append("namespace Rosen {")
    output.append("namespace Drawing {")
    output.append("namespace GEV2 {")
    output.append("")
    output.append("// Parameter structure definitions")

    script_dir = Path(__file__).parent
    root_dir = script_dir.parent.parent
    params_dirs = [
        root_dir / "include" / "effect" / "filter",
        root_dir / "include" / "effect" / "mask",
        root_dir / "include" / "effect" / "shader",
        root_dir / "include" / "effect" / "shape",
    ]

    params_files = get_params_files(params_dirs)
    for params_file in params_files:
        rel_path = params_file.relative_to(root_dir / "include")
        output.append(f'#include "{rel_path}"')

    output.append("")
    output.append("} // namespace GEV2")
    output.append("} // namespace Drawing")
    output.append("} // namespace Rosen")
    output.append("} // namespace OHOS")
    output.append("")
    output.append("#endif // GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H")

    return "\n".join(output)


def main():
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent.parent
    output_file = root_dir / "include" / "effect" / "ge_effects_params.h"

    params_dirs = [
        root_dir / "include" / "effect" / "filter",
        root_dir / "include" / "effect" / "mask",
        root_dir / "include" / "effect" / "shader",
        root_dir / "include" / "effect" / "shape",
    ]

    existing_dirs = [d for d in params_dirs if d.exists()]
    if not existing_dirs:
        print(f"Error: No params directories found in {params_dirs}")
        return 1

    params_files = get_params_files(params_dirs)
    if not params_files:
        print(f"No .params files found in {params_dirs}")
        return 1

    print(f"Found {len(params_files)} .params files")

    print(f"Generating {output_file.name}...")
    header_content = generate_header()

    output_file.parent.mkdir(parents=True, exist_ok=True)

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(header_content)

    print(f"Generated {output_file}")
    print(f"  - {len(params_files)} parameter definition files included")
    return 0


if __name__ == "__main__":
    exit(main())
