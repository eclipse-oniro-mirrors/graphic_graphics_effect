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
