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
Attribute Parser for C++ attributes.

This module provides parsing of C++ attributes like [[ge::params(type=AIBAR, name="AIBAR")]].
Supports C#/Rust style key=value attributes.
"""

import re
from typing import Dict, List, Tuple, Any, Optional
from dataclasses import dataclass

# Import Token from cpp_tokenizer module
from tool.effectgen.cpp_tokenizer import Token


@dataclass
class SplitResult:
    """Result of splitting parameters by commas."""

    parts: List[str]
    errors: List[str]
    in_string: bool
    in_raw_string: bool
    bracket_counts: Dict[str, int]
    reported_raw_string_error: bool


class AttributeParser:
    """Parse C++ attributes like [[ge::params(type=AIBAR, name="AIBAR")]]."""

    @staticmethod
    def parse_attribute(attr_token: Token) -> Dict[str, Any]:
        """Parse an attribute token and return a dict of key-value pairs."""
        content = attr_token.value[2:-2]  # Remove [[ and ]]
        content = content.strip()

        # Find the function call pattern: ge::params(type=AIBAR, name="AIBAR")
        match = re.match(r"(\w+)\s*::\s*(\w+)\s*\((.*)\)\s*", content, re.DOTALL)
        if not match:
            if content.startswith("ge::"):
                return {"errors": [f"Malformed attribute: could not parse '{content}'"]}
            return {}

        namespace = match.group(1)
        function = match.group(2)
        params_str = match.group(3).strip()

        # Parse C#/Rust style: type=AIBAR, name="AIBAR"
        params, errors = AttributeParser._parse_params(params_str)

        return {"namespace": namespace, "function": function, "params": params, "errors": errors}

    @staticmethod
    def _split_params_by_commas(s: str) -> SplitResult:
        """Split parameters by commas, respecting quotes, brackets, and raw strings."""
        parts = []
        errors = []
        depth = 0
        in_string = False
        in_raw_string = False
        reported_raw_string_error = False
        escape = False
        current = []
        bracket_counts = {"(": 0, "[": 0, "{": 0}

        i = 0
        while i < len(s):
            ch = s[i]

            # Handle raw string literals: R"delimiter(...)delimiter"
            if not in_string and not in_raw_string and ch == "R" and i + 1 < len(s) and s[i + 1] == '"':
                current.append(ch)
                current.append(s[i + 1])
                i += 2

                delimiter = []
                while i < len(s) and s[i] != "(":
                    delimiter.append(s[i])
                    current.append(s[i])
                    i += 1

                if i >= len(s) or s[i] != "(":
                    errors.append(f"Invalid raw string literal (missing opening '(' after R\"delimiter)")
                    in_raw_string = True
                else:
                    delimiter_str = "".join(delimiter)
                    current.append(s[i])
                    i += 1

                    closing_seq = ")" + delimiter_str + '"'
                    closing_pos = s.find(closing_seq, i)

                    if closing_pos == -1:
                        errors.append(f"Unterminated raw string literal (missing closing ')delimiter\"')")
                        reported_raw_string_error = True
                        current.extend(s[i:])
                        in_raw_string = True
                        break
                    else:
                        current.extend(s[i : closing_pos + len(closing_seq)])
                        i = closing_pos + len(closing_seq)
                continue

            if escape:
                current.append(ch)
                escape = False
                i += 1
            elif ch == "\\":
                escape = True
                current.append(ch)
                i += 1
            elif ch == '"' and not escape:
                if in_raw_string:
                    in_raw_string = False
                in_string = not in_string
                current.append(ch)
                i += 1
            elif not in_string and not in_raw_string:
                if ch == "(":
                    bracket_counts["("] += 1
                    depth += 1
                elif ch == ")":
                    if bracket_counts["("] > 0:
                        bracket_counts["("] -= 1
                        depth -= 1
                elif ch == "[":
                    bracket_counts["["] += 1
                    depth += 1
                elif ch == "]":
                    if bracket_counts["["] > 0:
                        bracket_counts["["] -= 1
                        depth -= 1
                elif ch == "{":
                    bracket_counts["{"] += 1
                    depth += 1
                elif ch == "}":
                    if bracket_counts["{"] > 0:
                        bracket_counts["{"] -= 1
                        depth -= 1
                elif ch == "," and depth == 0:
                    parts.append("".join(current))
                    current = []
                    i += 1
                    continue
                current.append(ch)
                i += 1
            else:
                current.append(ch)
                i += 1

        if current:
            parts.append("".join(current))

        return SplitResult(
            parts=parts,
            errors=errors,
            in_string=in_string,
            in_raw_string=in_raw_string,
            bracket_counts=bracket_counts,
            reported_raw_string_error=reported_raw_string_error,
        )

    @staticmethod
    def _validate_split_result(split_result: SplitResult) -> List[str]:
        """Validate the split result for unterminated strings and unbalanced brackets."""
        errors = []

        if (split_result.in_string or split_result.in_raw_string) and not split_result.reported_raw_string_error:
            string_type = "raw string" if split_result.in_raw_string else "string"
            errors.append(f"Unterminated {string_type} literal in attribute")

        for bracket, count in split_result.bracket_counts.items():
            if count > 0:
                errors.append(f"Unmatched '{bracket}' in attribute (missing {count} closing '{bracket}')")

        return errors

    @staticmethod
    def _check_missing_comma(part: str, key: str) -> Optional[str]:
        """Check if there's a missing comma after a parameter value."""
        eq_idx = part.index("=")
        value_with_rest = part[eq_idx + 1 :].strip()

        if value_with_rest.startswith('"'):
            if value_with_rest.startswith('R"'):
                paren_pos = value_with_rest.find("(")
                if paren_pos > 0:
                    delimiter = value_with_rest[3:paren_pos]
                    closing_seq = ")" + delimiter + '"'
                    end_raw = value_with_rest.find(closing_seq, paren_pos)
                    if end_raw > 0:
                        after_value = value_with_rest[end_raw + len(closing_seq) :].strip()
                    else:
                        after_value = ""
                else:
                    after_value = ""
            else:
                end_quote = value_with_rest.find('"', 1)
                if end_quote > 0:
                    after_value = value_with_rest[end_quote + 1 :].strip()
                else:
                    after_value = ""
            if after_value and "=" in after_value:
                return f"Missing comma in attribute parameters (after '{key}')"
        else:
            remaining = value_with_rest
            if re.search(r"\s+[a-zA-Z_]\w*=", remaining):
                return f"Missing comma in attribute parameters (after '{key}')"

        return None

    @staticmethod
    def _parse_params(s: str) -> Tuple[Dict[str, Any], List[str]]:
        """Parse C#/Rust style key=value attributes."""
        result = {}
        errors = []
        s = s.strip()

        split_result = AttributeParser._split_params_by_commas(s)
        parts = split_result.parts
        errors.extend(split_result.errors)

        validation_errors = AttributeParser._validate_split_result(split_result)
        errors.extend(validation_errors)

        for part in parts:
            part = part.strip()
            if not part:
                continue

            if "=" in part:
                eq_idx = part.index("=")
                key = part[:eq_idx].strip()
                value = part[eq_idx + 1 :].strip()

                if value.startswith('"') and value.endswith('"'):
                    if not value.startswith('R"'):
                        value = value[1:-1]
                result[key] = value

                missing_comma_error = AttributeParser._check_missing_comma(part, key)
                if missing_comma_error:
                    errors.append(missing_comma_error)
            else:
                if len(parts) == 1:
                    value = part.strip()
                    if value.startswith('"') and value.endswith('"') and not value.startswith('R"'):
                        value = value[1:-1]
                    result["_value"] = value
                else:
                    errors.append(f"Missing comma or equals in attribute parameters (near '{part}')")

        if "_value" in result and "name" not in result and "NAME" not in result:
            result["name"] = result["_value"]

        return result, errors
