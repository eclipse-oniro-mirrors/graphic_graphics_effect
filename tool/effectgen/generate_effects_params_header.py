#!/usr/bin/env python3
"""
Generate ge_effects_params.h header file.

This script generates a header file that includes all .def files from include/params/.
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


def get_def_files(params_dir: Path) -> List[Path]:
    """Get all .def files sorted alphabetically."""
    def_files = sorted(params_dir.glob("*.def"))
    return def_files


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
    output.append("#include \"common/rs_vector2.h\"")
    output.append("#include \"common/rs_vector3.h\"")
    output.append("#include \"common/rs_vector4.h\"")
    output.append("// Drawing types")
    output.append("#include \"utils/matrix.h\"")
    output.append("// GE types")
    output.append("#include \"ge_filter_type.h\"")
    output.append("#include \"ge_shader_mask.h\"")
    output.append("#include \"sdf/ge_sdf_shader_shape.h\"")
    output.append("")
    output.append("namespace OHOS {")
    output.append("namespace Rosen {")
    output.append("namespace Drawing {")
    output.append("")
    output.append("// Parameter structure definitions")

    # Include all .def files
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    params_dir = root_dir / "include" / "params"

    def_files = get_def_files(params_dir)
    for def_file in def_files:
        # Use params/ prefix (not include/params/)
        output.append(f"#include \"params/{def_file.name}\"")

    output.append("")
    output.append("} // namespace Drawing")
    output.append("} // namespace Rosen")
    output.append("} // namespace OHOS")
    output.append("")
    output.append("#endif // GRAPHICS_EFFECT_GE_EFFECTS_PARAMS_H")

    return '\n'.join(output)


def main():
    """Main generation function."""
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    output_file = root_dir / "include" / "ge_effects_params.h"

    # Check if params directory exists
    params_dir = root_dir / "include" / "params"
    if not params_dir.exists():
        print(f"Error: {params_dir} does not exist")
        return 1

    # Count .def files
    def_files = get_def_files(params_dir)
    if not def_files:
        print(f"No .def files found in {params_dir}")
        return 1

    print(f"Found {len(def_files)} .def files")

    # Generate header file
    print(f"Generating {output_file.name}...")
    header_content = generate_header()

    # Ensure output directory exists
    output_file.parent.mkdir(parents=True, exist_ok=True)

    # Write to file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)

    print(f"Generated {output_file}")
    print(f"  - {len(def_files)} parameter definition files included")
    return 0


if __name__ == '__main__':
    exit(main())
