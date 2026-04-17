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
CLI utilities for modern terminal output.

This module provides color-coded output and error/warning tracking
for command-line tools.
"""

import os
import sys
import shutil
import subprocess
import locale
from pathlib import Path
from typing import Optional


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

    # Unicode symbols for modern terminals
    UNICODE_SYMBOLS = {
        'warning': '⚠',
        'error': '✗',
        'success': '✓',
        'header': '▶',
        'step': '•',
        'file': '📄',
    }

    # ASCII fallback for terminals that don't support Unicode
    ASCII_SYMBOLS = {
        'warning': '[!]',
        'error': '[x]',
        'success': '[ok]',
        'header': '>',
        'step': '*',
        'file': '[f]',
    }

    def __init__(self, use_colors: Optional[bool] = None):
        # Determine color usage:
        # 1. If explicitly set (True/False), use that value
        # 2. If None, check NO_COLOR environment variable (https://no-color.org/)
        # 3. If NO_COLOR is not set, default to TTY detection
        if use_colors is None:
            # NO_COLOR standard: if the environment variable exists (even if empty),
            # colors should be disabled
            self.use_colors = 'NO_COLOR' not in os.environ and sys.stdout.isatty()
        else:
            self.use_colors = use_colors and sys.stdout.isatty()

        # Detect Unicode support in terminal
        self.use_unicode = self._detect_unicode_support()

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
        symbol = self._get_symbol('warning')
        print(self._colorize(f"{symbol} {message}", Colors.BRIGHT_YELLOW), file=sys.stderr)

    def error(self, message: str) -> None:
        """Print error message in red."""
        self.error_count += 1
        symbol = self._get_symbol('error')
        print(self._colorize(f"{symbol} {message}", Colors.BRIGHT_RED), file=sys.stderr)

    def header(self, message: str) -> None:
        """Print section header in cyan with bold."""
        symbol = self._get_symbol('header')
        print(f"\n{self._colorize(symbol, Colors.BRIGHT_CYAN)} {self._colorize(message, Colors.BOLD + Colors.BRIGHT_CYAN)}")

    def step(self, message: str) -> None:
        """Print step message in blue."""
        symbol = self._get_symbol('step')
        print(f"  {self._colorize(symbol, Colors.BRIGHT_BLUE)} {message}")

    def file(self, message: str) -> None:
        """Print file-related message in dim."""
        symbol = self._get_symbol('file')
        print(f"  {self._colorize(symbol, Colors.DIM)} {message}")

    def summary(self) -> None:
        """Print execution summary."""
        print()
        success_symbol = self._get_symbol('success')
        if self.error_count == 0 and self.warning_count == 0:
            self.success(f"{success_symbol} Generation completed successfully!")
        elif self.error_count == 0:
            self.warning(f"Generation completed with {self.warning_count} warning(s)")
        else:
            self.error(f"Generation failed with {self.error_count} error(s) and {self.warning_count} warning(s)")
    
    def _detect_unicode_support(self) -> bool:
        """Detect if the terminal supports Unicode output.

        Returns:
            True if Unicode is supported, False otherwise (use ASCII fallback)
        """
        # Check if stdout is a TTY (non-TTY may pipe to files/tools that expect ASCII)
        if not sys.stdout.isatty():
            return False

        # Check environment variables that indicate ASCII-only terminals
        # TERM=dumb indicates a terminal with no special capabilities
        term = os.environ.get('TERM', '')
        if term == 'dumb' or term == '':
            return False

        # Check if the terminal encoding supports Unicode
        try:
            # Get the encoding of stdout
            encoding = sys.stdout.encoding
            if encoding is None:
                return False

            # UTF-8 and UTF-16 variants support Unicode
            if encoding.lower().startswith('utf'):
                return True

            # Check locale encoding
            try:
                loc = locale.getpreferredencoding(False)
                if loc and loc.lower().startswith('utf'):
                    return True
            except Exception:
                pass

            # Check LANG environment variable for UTF-8 hint
            lang = os.environ.get('LANG', '')
            if 'utf' in lang.lower() or 'UTF' in lang:
                return True

            # Default to ASCII for other encodings (e.g., ASCII, ISO-8859-*, etc.)
            return False
        except Exception:
            # If any encoding check fails, default to ASCII for safety
            return False

    def _get_symbol(self, symbol_name: str) -> str:
        """Get appropriate symbol based on Unicode support.

        Args:
            symbol_name: Key from UNICODE_SYMBOLS/ASCII_SYMBOLS dict

        Returns:
            Unicode or ASCII symbol depending on terminal support
        """
        if self.use_unicode:
            return self.UNICODE_SYMBOLS.get(symbol_name, '')
        return self.ASCII_SYMBOLS.get(symbol_name, '')
    
    def _colorize(self, text: str, color: str) -> str:
        """Apply color to text if colors are enabled."""
        if self.use_colors:
            return f"{color}{text}{Colors.RESET}"
        return text

def find_clang_format() -> Optional[str]:
    """Find clang-format executable in the system.

    Returns:
        Path to clang-format if found, None otherwise
    """
    # Try common names in order of preference
    for name in ["clang-format", "clang-format-18", "clang-format-17", "clang-format-16", "clang-format-15"]:
        path = shutil.which(name)
        if path:
            return path
    return None


def format_file_with_clang_format(file_path: Path, clang_format_path: str, console: Console) -> bool:
    """Format a file using clang-format.

    Args:
        file_path: Path to the file to format
        clang_format_path: Path to clang-format executable
        console: Console instance for output

    Returns:
        True if formatting succeeded, False otherwise
    """
    try:
        result = subprocess.run(
            [clang_format_path, "-i", str(file_path)],
            capture_output=True,
            text=True,
        )
        if result.returncode == 0:
            console.success(f"Formatted {file_path.name} with clang-format")
            return True
        else:
            console.warning(f"clang-format failed for {file_path}: {result.stderr}")
            return False
    except Exception as e:
        console.warning(f"Failed to run clang-format: {e}")
        return False