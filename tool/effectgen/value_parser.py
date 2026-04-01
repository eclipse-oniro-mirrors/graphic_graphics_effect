#!/usr/bin/env python3
"""
Value Parser for C++ default values.

This module provides parsing utilities for C++ default values,
including numeric type inference and component value extraction.
"""

from typing import List


def infer_numeric_type(value_str: str) -> str:
    """Infer numeric type from a string value.

    Returns 'float' if value contains a decimal point or 'f' suffix, otherwise 'int'.

    Args:
        value_str: The string value to analyze

    Returns:
        'float' or 'int'
    """
    value = value_str.strip()
    if "." in value or value.endswith("f") or value.endswith("F"):
        return "float"
    return "int"


def parse_component_values(value_str: str) -> List[str]:
    """Parse component values from a brace-enclosed string.

    Example: "{1.0f, 2.0f, 3.0f}" -> ["1.0f", "2.0f", "3.0f"]

    Args:
        value_str: The brace-enclosed string to parse

    Returns:
        List of component values (stripped of whitespace)
    """
    value_str = value_str.strip()
    if not value_str.startswith("{") or not value_str.endswith("}"):
        return []

    inner = value_str[1:-1].strip()
    if not inner:
        return []

    components = [c.strip() for c in inner.split(",")]
    return [c for c in components if c]
