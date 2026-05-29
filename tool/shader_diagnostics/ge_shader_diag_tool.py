#!/usr/bin/env python3
"""Standalone entry point for GE Shader Diagnostics Post-Processing Tool.

Can be invoked directly:
  python3 ge_shader_diag_tool.py collect <dir> --label <name>
  python3 ge_shader_diag_tool.py report --label <name> --format html
  python3 ge_shader_diag_tool.py diff --left <L> --right <R> --format html

Or as a module from the parent directory:
  cd tool && python3 -m shader_diagnostics collect <dir> --label <name>
"""

import os
import sys

# Add parent directory to path so the shader_diagnostics package is importable
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from shader_diagnostics.__main__ import main

if __name__ == "__main__":
    main()