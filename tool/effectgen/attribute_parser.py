#!/usr/bin/env python3
"""
Attribute Parser for C++ attributes.

This module provides parsing of C++ attributes like [[ge::params(type=AIBAR, name="AIBAR")]].
Supports C#/Rust style key=value attributes.
"""

import re
from typing import Dict, List, Tuple, Any, Optional

# Import Token from cpp_tokenizer module
from tool.effectgen.cpp_tokenizer import Token


class AttributeParser:
    """Parse C++ attributes like [[ge::params(type=AIBAR, name="AIBAR")]]."""

    @staticmethod
    def parse_attribute(attr_token: Token) -> Dict[str, Any]:
        """Parse an attribute token and return a dict of key-value pairs."""
        content = attr_token.value[2:-2]  # Remove [[ and ]]
        content = content.strip()

        # Find the function call pattern: ge::params(type=AIBAR, name="AIBAR")
        match = re.match(r'(\w+)\s*::\s*(\w+)\s*\((.*)\)\s*', content, re.DOTALL)
        if not match:
            return {}

        namespace = match.group(1)
        function = match.group(2)
        params_str = match.group(3).strip()

        # Parse C#/Rust style: type=AIBAR, name="AIBAR"
        params, errors = AttributeParser._parse_params(params_str, attr_token.line, attr_token.column)

        return {
            'namespace': namespace,
            'function': function,
            'params': params,
            'errors': errors
        }

    @staticmethod
    def _parse_params(s: str, line: int, column: int) -> Tuple[Dict[str, Any], List[str]]:
        """Parse C#/Rust style key=value attributes."""
        result = {}
        errors = []
        s = s.strip()

        # Split by commas (not inside quotes, parentheses, brackets, or braces)
        parts = []
        depth = 0  # Tracks combined depth of all brackets
        in_string = False
        in_raw_string = False
        reported_raw_string_error = False  # Track if we already reported a specific raw string error
        escape = False
        current = []

        # Track individual bracket types for detailed validation
        bracket_counts = {'(': 0, '[': 0, '{': 0}

        i = 0
        while i < len(s):
            ch = s[i]

            # Handle raw string literals: R"delimiter(...)delimiter"
            if not in_string and not in_raw_string and ch == 'R' and i + 1 < len(s) and s[i + 1] == '"':
                # Start of raw string literal
                # Add the R" to current
                current.append(ch)
                current.append(s[i + 1])
                i += 2  # Skip R"

                # Extract the delimiter (everything between R" and ()
                delimiter = []
                while i < len(s) and s[i] != '(':
                    delimiter.append(s[i])
                    current.append(s[i])
                    i += 1

                # Check for opening parenthesis
                if i >= len(s) or s[i] != '(':
                    errors.append(f"line {line}, column {column}: Invalid raw string literal (missing opening '(' after R\"delimiter)")
                    # Try to recover
                    in_raw_string = True
                else:
                    delimiter_str = ''.join(delimiter)
                    current.append(s[i])  # Add '('
                    i += 1

                    # Find the closing sequence: )delimiter"
                    closing_seq = ')' + delimiter_str + '"'
                    closing_pos = s.find(closing_seq, i)

                    if closing_pos == -1:
                        # Unterminated raw string
                        errors.append(f"line {line}, column {column}: Unterminated raw string literal (missing closing ')delimiter\"\')")
                        reported_raw_string_error = True
                        # Add rest of string to current and exit
                        current.extend(s[i:])
                        in_raw_string = True
                        break
                    else:
                        # Add everything up to (and including) the closing sequence
                        current.extend(s[i:closing_pos + len(closing_seq)])
                        i = closing_pos + len(closing_seq)
                continue

            if escape:
                current.append(ch)
                escape = False
                i += 1
            elif ch == '\\':
                escape = True
                current.append(ch)
                i += 1
            elif ch == '"' and not escape:
                if in_raw_string:
                    # This should be the closing " of a raw string, but we already handled it above
                    # If we get here, something went wrong
                    in_raw_string = False
                in_string = not in_string
                current.append(ch)
                i += 1
            elif not in_string and not in_raw_string:
                if ch == '(':
                    bracket_counts['('] += 1
                    depth += 1
                elif ch == ')':
                    if bracket_counts['('] > 0:
                        bracket_counts['('] -= 1
                        depth -= 1
                elif ch == '[':
                    bracket_counts['['] += 1
                    depth += 1
                elif ch == ']':
                    if bracket_counts['['] > 0:
                        bracket_counts['['] -= 1
                        depth -= 1
                elif ch == '{':
                    bracket_counts['{'] += 1
                    depth += 1
                elif ch == '}':
                    if bracket_counts['{'] > 0:
                        bracket_counts['{'] -= 1
                        depth -= 1
                elif ch == ',' and depth == 0:
                    # Only split on commas at depth 0 (outside all brackets)
                    parts.append(''.join(current))
                    current = []
                    i += 1
                    continue
                current.append(ch)
                i += 1
            else:
                # Inside a string or raw string - just append
                current.append(ch)
                i += 1

        if current:
            parts.append(''.join(current))

        # Validate: all strings must be closed
        if (in_string or in_raw_string) and not reported_raw_string_error:
            string_type = "raw string" if in_raw_string else "string"
            errors.append(f"line {line}, column {column}: Unterminated {string_type} literal in attribute")

        # Validate: all brackets must be balanced
        for bracket, count in bracket_counts.items():
            if count > 0:
                errors.append(f"line {line}, column {column}: Unmatched '{bracket}' in attribute (missing {count} closing '{bracket}')")

        # Parse each part: key=value or key="string"
        for i, part in enumerate(parts):
            part = part.strip()
            if not part:
                continue

            if '=' in part:
                # Split only on the first '=' to handle values that contain '='
                eq_idx = part.index('=')
                key = part[:eq_idx].strip()
                value = part[eq_idx + 1:].strip()
                # Remove quotes from string values (but not from raw strings)
                if value.startswith('"') and value.endswith('"'):
                    # Check if it's a raw string (starts with R")
                    if not value.startswith('R"'):
                        value = value[1:-1]
                result[key] = value

                # Check for missing comma: look for unquoted identifier followed by '=' after the value
                # For example: type=TEST_NO_COMMA name="TestNoComma" - the "name=" part indicates missing comma
                value_with_rest = part[eq_idx + 1:].strip()
                if value_with_rest.startswith('"'):
                    # Quoted string - find closing quote and check if there's content after it
                    # For raw strings, we need to find the closing )delimiter"
                    if value_with_rest.startswith('R"'):
                        # Raw string - find the closing sequence
                        paren_pos = value_with_rest.find('(')
                        if paren_pos > 0:
                            delimiter = value_with_rest[3:paren_pos]  # Between R" and (
                            closing_seq = ')' + delimiter + '"'
                            end_raw = value_with_rest.find(closing_seq, paren_pos)
                            if end_raw > 0:
                                after_value = value_with_rest[end_raw + len(closing_seq):].strip()
                            else:
                                after_value = ""
                        else:
                            after_value = ""
                    else:
                        # Regular string - find closing quote
                        end_quote = value_with_rest.find('"', 1)
                        if end_quote > 0:
                            after_value = value_with_rest[end_quote + 1:].strip()
                        else:
                            after_value = ""
                    if after_value and '=' in after_value:
                        errors.append(f"line {line}, column {column}: Missing comma in attribute parameters (after '{key}')")
                else:
                    # Unquoted value - check if there's space followed by identifier=
                    # Look for pattern like: value name="..." or value name=...
                    remaining = value_with_rest
                    import re as re_internal
                    # Match: whitespace followed by identifier followed by =
                    if re_internal.search(r'\s+[a-zA-Z_]\w*=', remaining):
                        errors.append(f"line {line}, column {column}: Missing comma in attribute parameters (after '{key}')")
            else:
                # Single value without a key (e.g., ge::prop("value"))
                # If we have exactly one part and it's a valid value, store it as _value
                # If we have multiple parts without equals, this might be a missing comma error
                if len(parts) == 1:
                    # This is a single value like "Custom_Property_Name"
                    value = part.strip()
                    # Remove quotes from string values
                    if value.startswith('"') and value.endswith('"') and not value.startswith('R"'):
                        value = value[1:-1]
                    result['_value'] = value
                else:
                    # Multiple parts without equals - likely a missing comma error
                    # e.g., "type=TEST name=VALUE" instead of "type=TEST, name=VALUE"
                    errors.append(f"line {line}, column {column}: Missing comma or equals in attribute parameters (near '{part}')")

        # Backward compatibility: if there's a single _value and no 'name' key, treat it as 'name'
        if '_value' in result and 'name' not in result and 'NAME' not in result:
            result['name'] = result['_value']

        return result, errors
