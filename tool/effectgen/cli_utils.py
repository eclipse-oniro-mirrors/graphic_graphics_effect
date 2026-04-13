# -*- coding: utf-8 -*-

# Copyright (c) 2024 Huawei Device Co., Ltd.
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
CLI utilities for modern terminal output.

This module provides color-coded output and error/warning tracking
for command-line tools.
"""

import sys


# ANSI Color codes for modern CLI output
class Colors:
    """ANSI color codes for terminal output."""

    RESET = "\033[0m"
    BOLD = "\033[1m"
    DIM = "\033[2m"

    # Foreground colors
    BLACK = "\033[30m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"
    WHITE = "\033[37m"

    # Bright foreground colors
    BRIGHT_RED = "\033[91m"
    BRIGHT_GREEN = "\033[92m"
    BRIGHT_YELLOW = "\033[93m"
    BRIGHT_BLUE = "\033[94m"
    BRIGHT_MAGENTA = "\033[95m"
    BRIGHT_CYAN = "\033[96m"
    BRIGHT_WHITE = "\033[97m"

    # Background colors
    BG_RED = "\033[41m"
    BG_GREEN = "\033[42m"
    BG_YELLOW = "\033[43m"
    BG_BLUE = "\033[44m"


class Console:
    """Manages CLI output with colors and error/warning tracking."""

    def __init__(self, use_colors: bool = True):
        self.use_colors = use_colors and sys.stdout.isatty()
        self.warning_count = 0
        self.error_count = 0


    def info(self, message: str) -> None:
        """Print informational message."""
        print(message)

    def success(self, message: str) -> None:
        """Print success message in green."""
        print(self._colorize(message, Colors.BRIGHT_GREEN))

    def warning(self, message: str) -> None:
        """Print warning message in yellow."""
        self.warning_count += 1
        print(self._colorize(f"⚠ {message}", Colors.BRIGHT_YELLOW), file=sys.stderr)

    def error(self, message: str) -> None:
        """Print error message in red."""
        self.error_count += 1
        print(self._colorize(f"✗ {message}", Colors.BRIGHT_RED), file=sys.stderr)

    def header(self, message: str) -> None:
        """Print section header in cyan with bold."""
        print(f"\n{self._colorize('▶', Colors.BRIGHT_CYAN)} {self._colorize(message, Colors.BOLD + Colors.BRIGHT_CYAN)}")

    def step(self, message: str) -> None:
        """Print step message in blue."""
        print(f"  {self._colorize('•', Colors.BRIGHT_BLUE)} {message}")

    def file(self, message: str) -> None:
        """Print file-related message in dim."""
        print(f"  {self._colorize('📄', Colors.DIM)} {message}")

    def summary(self) -> None:
        """Print execution summary."""
        print()
        if self.error_count == 0 and self.warning_count == 0:
            self.success("✓ Generation completed successfully!")
        elif self.error_count == 0:
            self.warning(f"Generation completed with {self.warning_count} warning(s)")
        else:
            self.error(f"Generation failed with {self.error_count} error(s) and {self.warning_count} warning(s)")

    def _colorize(self, text: str, color: str) -> str:
        """Apply color to text if colors are enabled."""
        if self.use_colors:
            return f"{color}{text}{Colors.RESET}"
        return text