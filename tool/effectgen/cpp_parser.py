"""
C++ Parser for struct definitions with attributes.

This module provides parsing of C++ struct definitions with custom attributes.
Focuses on parsing struct definitions with [[ge::params(...)]] attributes.
"""

from typing import List, Optional, Dict, Any
from dataclasses import dataclass, field

# Import Token from cpp_tokenizer module
from tool.effectgen.cpp_tokenizer import Token

# Import AttributeParser from attribute_parser module
from tool.effectgen.attribute_parser import AttributeParser


@dataclass
class ParseError:
    """Represents a parsing error."""

    message: str
    line: int
    column: int
    file_path: Optional[str] = None
    context: str = ""

    def __str__(self) -> str:
        loc = f"line {self.line}"
        if self.column > 0:
            loc += f", column {self.column}"
        if self.file_path:
            loc = f"{self.file_path}:{loc}"
        return f"{loc}: {self.message}"


@dataclass
class PropAttribute:
    """Information about a single [[ge::prop]] attribute."""

    name: str
    array_accessor_length: Optional[int] = None
    array_accessor_type: Optional[str] = None
    alias: Optional[str] = None
    cast_from: Optional[str] = None
    custom: Optional[str] = None
    min_value: Optional[str] = None
    max_value: Optional[str] = None


@dataclass
class FieldInfo:
    """Information about a struct field."""

    name: str
    type: str
    prop_name: Optional[str] = None  # For backward compatibility - first prop name
    prop_attributes: List[PropAttribute] = field(default_factory=list)  # All prop attributes
    default_value: Optional[str] = None
    attributes: List[Token] = field(default_factory=list)


@dataclass
class StructInfo:
    """Information about a params struct."""

    name: str
    enum_type: str
    filter_name: str
    fields: List[FieldInfo] = field(default_factory=list)
    errors: List[ParseError] = field(default_factory=list)
    params: Dict[str, Any] = field(default_factory=dict)  # All parsed params (as-is)


class CppParser:
    """
    Parse C++ struct definitions from tokens.

    Focuses on parsing struct definitions with attributes.
    """

    def __init__(self, tokens: List[Token], file_path: str = None):
        self.tokens = tokens
        self.pos = 0
        self.file_path = file_path
        self.errors: List[ParseError] = []

    def parse(self) -> List[StructInfo]:
        """Parse all struct definitions."""
        structs = []
        while self.pos < len(self.tokens):
            # Look for struct keyword followed by [[ge::params(...)]] attribute
            if self._match_keyword("struct"):
                # Save position
                saved_pos = self.pos
                self._consume()  # Consume 'struct'

                # Check if next token is an attribute [[ge::params(...)]]
                if self._peek() and self._peek().type == "attribute":
                    attr_token = self._consume()
                    parsed = AttributeParser.parse_attribute(attr_token)

                    # Add any attribute parsing errors
                    for error in parsed.get("errors", []):
                        self._add_error(error, attr_token.line, attr_token.column)

                    if parsed.get("namespace") == "ge" and parsed.get("function") == "params":
                        # This is a params struct with attribute after struct keyword
                        struct = self._parse_struct([attr_token])
                        if struct:
                            structs.append(struct)
                        else:
                            # _parse_struct returned None - check if it's because struct name is missing
                            # Peek at the next token after consuming 'struct' and the attribute
                            if self._peek() and self._peek().type not in ("identifier", "keyword"):
                                self._add_warning(
                                    "Struct has [[ge::params(...)]] attribute but is missing a struct name",
                                    attr_token.line,
                                    attr_token.column,
                                )
                        continue
                    else:
                        # Not a params attribute - add warning if it has a valid namespace
                        if parsed.get("namespace") and parsed.get("function"):
                            self._add_warning(
                                f"Skipping struct with attribute '{parsed.get('namespace')}::{parsed.get('function')}' (only 'ge::params' is parsed)",
                                attr_token.line,
                                attr_token.column,
                            )
                        # Restore position
                        self.pos = saved_pos
                else:
                    # Not an attribute after struct, restore position
                    self.pos = saved_pos

            self.pos += 1
        return structs

    def _peek(self, offset: int = 0) -> Optional[Token]:
        """Peek at the token at current position + offset."""
        idx = self.pos + offset
        if 0 <= idx < len(self.tokens):
            return self.tokens[idx]
        return None

    def _match_keyword(self, keyword: str) -> bool:
        """Check if current token is a specific keyword."""
        token = self._peek()
        return token is not None and token.type == "keyword" and token.value == keyword

    def _match_identifier(self, identifier: str = None) -> bool:
        """Check if current token is an identifier (optionally with a specific value)."""
        token = self._peek()
        if token is None or token.type != "identifier":
            return False
        if identifier is not None:
            return token.value == identifier
        return True

    def _match_operator(self, op: str) -> bool:
        """Check if current token is a specific operator."""
        token = self._peek()
        return token is not None and token.type == "operator" and token.value == op

    def _consume(self) -> Token:
        """Consume and return current token."""
        token = self._peek()
        self.pos += 1
        return token

    def _parse_struct(self, attributes: List[Token]) -> Optional[StructInfo]:
        """Parse a struct definition.

        Expected format:
            struct [[ge::params(...)]] StructName { ... }

        The 'struct' keyword should already be consumed before calling this method.
        """
        # Struct name (comes after the attribute)
        if not self._match_identifier():
            return None
        struct_name_token = self._consume()
        struct_name = struct_name_token.value

        # Create a local errors list for this struct
        struct_errors: List[ParseError] = []

        # Parse attributes to extract params info
        enum_type = None
        filter_name = None
        all_params = {}  # Store all params as-is
        for attr in attributes:
            parsed = AttributeParser.parse_attribute(attr)
            if parsed.get("namespace") == "ge" and parsed.get("function") == "params":
                params = parsed.get("params", {})
                all_params = params.copy()  # Store all params
                enum_type = params.get("type")
                filter_name = params.get("name")

        if not enum_type or not filter_name:
            # Not a params struct, skip it
            return None

        # Skip until opening brace
        while self._peek() and not self._match_operator("{"):
            self._consume()

        if not self._match_operator("{"):
            self._add_error(
                f"Missing opening brace '{{' after struct name '{struct_name}'",
                struct_name_token.line,
                struct_name_token.column,
            )
            return None
        self._consume()  # Consume {

        # Parse fields
        fields = []
        while self._peek() and not self._match_operator("}"):
            field = self._parse_field(struct_name)
            if field:
                fields.append(field)

        # Consume closing brace
        if not self._match_operator("}"):
            self._add_error(
                f"Missing closing brace '}}' for struct '{struct_name}'",
                struct_name_token.line,
                struct_name_token.column,
            )
            # Try to recover by consuming until we find a closing brace or end of tokens
            depth = 1
            while self._peek() and depth > 0:
                if self._match_operator("{"):
                    depth += 1
                elif self._match_operator("}"):
                    depth -= 1
                self._consume()
        else:
            self._consume()  # Consume }

        return StructInfo(name=struct_name, enum_type=enum_type, filter_name=filter_name, fields=fields, errors=struct_errors, params=all_params)

    def _add_error(self, message: str, line: int, column: int):
        """Add a parsing error."""
        error = ParseError(message=message, line=line, column=column, file_path=self.file_path)
        self.errors.append(error)

    def _add_warning(self, message: str, line: int, column: int):
        """Add a parsing warning (treated as non-fatal error)."""
        error = ParseError(message=f"Warning: {message}", line=line, column=column, file_path=self.file_path)
        self.errors.append(error)

    def _parse_field(self, struct_name: str) -> Optional[FieldInfo]:
        """Parse a single field declaration."""
        start_token = self._peek()
        if not start_token:
            return None

        # Collect attributes
        attributes = []
        while self._peek() and self._peek().type == "attribute":
            attributes.append(self._consume())

        # Collect all tokens until semicolon or closing brace at depth 0
        # We use a formal parse method: collect tokens and validate afterwards,
        # rather than trying to detect patterns early with hard-coded type lists.
        tokens_until_semicolon = []
        depth = 0  # Track nesting depth for templates/brackets/braces

        # Check if this looks like a function declaration (for function body detection)
        # A function has: return_type func_name ( ...
        is_function_like = False
        # Skip leading attributes when checking for function pattern
        attr_idx = 0
        while attr_idx < len(attributes):
            attr_idx += 1
        # We need to peek ahead to see if there's a function pattern
        # But we haven't collected tokens yet, so we'll check after collection

        while self._peek():
            next_token = self._peek()

            # Stop at semicolon
            if self._match_operator(";"):
                break

            # Track nesting depth for templates, brackets, braces, parentheses
            # IMPORTANT: Update depth BEFORE checking for closing brace
            # This ensures we check the depth AFTER the current token changes it
            if next_token.value in "<{([":
                new_depth = depth + 1
            elif next_token.value in ">})]":
                new_depth = max(0, depth - 1)
            else:
                new_depth = depth

            # Stop at closing brace only when depth will be 0 AFTER this token
            # AND we're parsing a function (detected by () pattern)
            # This prevents consuming tokens from the next field when a function body ends
            if next_token.value == "}" and new_depth == 0:
                # Check if this looks like a function body ending
                # A function has the pattern: type name ( ... ) qualifiers { ... }
                # We check if we've seen () before this }
                if is_function_like:
                    break
                # Also check if there's no = sign before this } (default values have =)
                has_equals = False
                for t in tokens_until_semicolon:
                    if t.value == "=":
                        has_equals = True
                        break
                if not has_equals:
                    # No = sign, this might be a function body or a C++11 brace init without =
                    # Check for function pattern: type name ( ...
                    has_function_pattern = False
                    for j in range(len(tokens_until_semicolon) - 1):
                        if (
                            tokens_until_semicolon[j].type in ("keyword", "identifier")
                            and j + 2 < len(tokens_until_semicolon)
                            and tokens_until_semicolon[j + 1].type in ("keyword", "identifier")
                            and tokens_until_semicolon[j + 2].value == "("
                        ):
                            has_function_pattern = True
                            break
                    if has_function_pattern:
                        # This is a function body ending
                        break

            # Update depth and collect the token
            depth = new_depth
            token = self._consume()
            tokens_until_semicolon.append(token)

        # Check if we found a semicolon
        found_semicolon = self._match_operator(";")
        if found_semicolon:
            self._consume()

        if not tokens_until_semicolon:
            # Empty field declaration (just semicolon or closing brace)
            if not found_semicolon and start_token and start_token.type == "operator" and start_token.value == "}":
                # This is just the closing brace, not a field
                return None
            return None

        # Detect and skip function declarations (member functions, static methods, etc.)
        # A function declaration has: [attributes] return_type function_name ( ... ) [qualifiers] [ { ... } ]
        # We detect this by looking for: identifier/keyword followed by identifier followed by ( at depth 0
        # Example: [[nodiscard]] bool is_valid ( ) const { ... }
        depth = 0
        found_paren_pattern = False
        # Skip leading attributes when checking for function pattern
        start_idx = 0
        while start_idx < len(tokens_until_semicolon) and tokens_until_semicolon[start_idx].type == "attribute":
            start_idx += 1

        for i in range(start_idx, len(tokens_until_semicolon)):
            token = tokens_until_semicolon[i]
            if token.value in "<{([":
                depth += 1
            elif token.value in ">})]":
                depth = max(0, depth - 1)

            # Look for pattern: identifier/keyword followed by identifier followed by ( at depth 0
            # This indicates: return_type function_name ( ...
            if depth == 0 and token.type in ("keyword", "identifier"):
                # Check if this is followed by another identifier/keyword and then (
                if (
                    i + 2 < len(tokens_until_semicolon)
                    and tokens_until_semicolon[i + 1].type in ("keyword", "identifier")
                    and tokens_until_semicolon[i + 2].value == "("
                ):
                    found_paren_pattern = True
                    break

        if found_paren_pattern:
            # This is a function declaration, skip it
            # But we need to properly consume its body if it has one
            # Check if we collected a function body (has { in tokens) vs a simple declaration
            # When we collect tokens, we break at the function body's closing },
            # so tokens_until_semicolon will end with {, not }
            has_function_body = False
            if tokens_until_semicolon:
                # Look for the opening brace { in the tokens
                # If found after the parameter list, this is a function definition with body
                for token in tokens_until_semicolon:
                    if token.value == "{":
                        has_function_body = True
                        break

            # Only consume the next brace if we actually have a function body
            # For simple declarations like "void func();", the next } is the struct's closing brace
            # For function definitions like "void func() {}", the next } is the function body's closing brace
            if has_function_body and self._peek() and self._peek().value == "}":
                self._consume()  # Consume the function body's closing brace
            # Return None without adding a field
            return None

        # Detect missing semicolon: check for pattern like "type name1 type name2" at depth 0
        # A normal field declaration is: type name [ = default ]
        # If we have multiple consecutive type+name patterns at depth 0, semicolon is missing
        # We use a pattern-based check instead of a simple count to avoid false positives on templates
        depth = 0
        consecutive_ident_at_depth_0 = 0
        max_consecutive_ident_at_depth_0 = 0
        has_template = False
        for i, token in enumerate(tokens_until_semicolon):
            if token.value in "<{([":
                depth += 1
                if token.value == "<":
                    has_template = True
                consecutive_ident_at_depth_0 = 0
            elif token.value in ">})]":
                depth = max(0, depth - 1)
                consecutive_ident_at_depth_0 = 0
            elif depth == 0 and token.type in ("keyword", "identifier"):
                consecutive_ident_at_depth_0 += 1
                max_consecutive_ident_at_depth_0 = max(max_consecutive_ident_at_depth_0, consecutive_ident_at_depth_0)
            else:
                consecutive_ident_at_depth_0 = 0

        # Check if we have a likely missing semicolon
        # Pattern: we see type (ident/keyword) followed by another type at depth 0
        # This typically means: "type1 name1 type2 name2" (missing semicolon between declarations)
        # We look for 4+ consecutive idents at depth 0, which suggests two complete declarations
        # Or if we have 3+ idents but no template (suggests "type name type" pattern)
        if max_consecutive_ident_at_depth_0 >= 4 or (max_consecutive_ident_at_depth_0 >= 3 and not has_template):
            # Find where to truncate
            depth = 0
            ident_count_at_depth_0 = 0
            for i, token in enumerate(tokens_until_semicolon):
                if token.value in "<{([":
                    depth += 1
                    ident_count_at_depth_0 = 0
                elif token.value in ">})]":
                    depth = max(0, depth - 1)
                    ident_count_at_depth_0 = 0
                elif depth == 0 and token.type in ("keyword", "identifier"):
                    ident_count_at_depth_0 += 1
                    # After we've seen type+name (2 idents), the next ident starts a new declaration
                    if ident_count_at_depth_0 > 2:
                        self._add_error(
                            f"Missing semicolon in field declaration in struct '{struct_name}' (found '{token.value}' which may start a new field)",
                            token.line,
                            token.column,
                        )
                        # Truncate at the suspected new field to avoid parsing garbage
                        tokens_until_semicolon = tokens_until_semicolon[:i]
                        break

        # Validate that the first token is a type (keyword or identifier)
        first_token = tokens_until_semicolon[0]
        if first_token.type not in ("keyword", "identifier"):
            self._add_error(
                f"Invalid field declaration in struct '{struct_name}': expected type, got '{first_token.value}' ({first_token.type})",
                first_token.line,
                first_token.column,
            )
            return None

        # Validate bracket matching in all tokens (catch cases like {1.0f, 2.0f;)
        # IMPORTANT: Skip string and char tokens - their contents should not be parsed for brackets
        depth = 0
        for token in tokens_until_semicolon:
            # String and char literals are opaque - don't count brackets inside them
            if token.type in ("string", "char"):
                continue

            if token.value in "{[<(":
                depth += 1
            # Check if token contains any closing bracket
            elif any(char in "}])>" for char in token.value):
                # Handle special case: >> in C++11 templates is two closing brackets
                # We count the actual > characters in the token value
                for char in token.value:
                    if char in "}])>":
                        depth -= 1
        if depth != 0:
            # Unmatched brackets/braces
            self._add_error(
                f"Unmatched brackets in field declaration in struct '{struct_name}'",
                first_token.line,
                first_token.column,
            )
            return None

        # Find where default value starts (look for = or { from the end)
        # C++11 supports brace initialization: int a{42}; or int a{};
        # Traditional initialization uses =: int a = 42;
        default_start_idx = -1
        default_value_type = None  # '=' or '{'
        depth = 0
        for i in range(len(tokens_until_semicolon) - 1, -1, -1):
            token = tokens_until_semicolon[i]

            # Update depth first (must be done before checking = or {)
            # When iterating backwards, we increment for closing delimiters and decrement for opening
            if token.value in "}])>":
                depth += 1
            elif token.value in "{[<(":
                depth -= 1

            # Check for traditional initialization (= at depth 0)
            if token.value == "=" and depth == 0:
                default_start_idx = i
                default_value_type = "="
                break

            # Check for C++11 brace initialization ({ at depth 0 AFTER updating depth)
            # When iterating backwards, we decrement depth on { (to undo the forward increment)
            # After decrementing, if depth == 0, we've just returned to the outer level
            if token.value == "{" and depth == 0:
                # C++11 brace initialization: int a{42}; or int a{};
                # Check if this is really a brace init (not part of something else)
                # by looking at the token before it
                if i > 0 and tokens_until_semicolon[i - 1].type in ("identifier", "keyword"):
                    default_start_idx = i
                    default_value_type = "{"
                    break

        # Extract default value
        default_value = None
        if default_start_idx >= 0:
            if default_value_type == "=":
                # Traditional initialization: int a = 42;
                default_tokens = tokens_until_semicolon[default_start_idx + 1 :]
                # Validate brace matching in default value
                # IMPORTANT: Skip string and char tokens - their contents should not be parsed for brackets
                depth = 0
                for i, token in enumerate(default_tokens):
                    # String and char literals are opaque - don't count brackets inside them
                    if token.type in ("string", "char"):
                        continue

                    if token.value in "{[<(":
                        depth += 1
                    # Check if token contains any closing bracket
                    elif any(char in "}])>" for char in token.value):
                        # Handle special case: >> in C++11 templates is two closing brackets
                        # We count the actual > characters in the token value
                        for char in token.value:
                            if char in "}])>":
                                depth -= 1
                if depth != 0:
                    # Unmatched brackets/braces
                    self._add_error(
                        f"Unmatched brackets in default value for field in struct '{struct_name}'",
                        first_token.line,
                        first_token.column,
                    )
                    return None
                default_value = "".join([t.value for t in default_tokens]).strip()
                tokens_until_semicolon = tokens_until_semicolon[:default_start_idx]
            elif default_value_type == "{":
                # C++11 brace initialization: int a{42}; or int a{};
                # Find the matching closing brace
                depth = 0
                end_idx = -1
                for i in range(default_start_idx, len(tokens_until_semicolon)):
                    token = tokens_until_semicolon[i]
                    if token.value == "{":
                        depth += 1
                    elif token.value == "}":
                        depth -= 1
                        if depth == 0:
                            end_idx = i
                            break

                if end_idx >= 0:
                    # Extract content between { and }
                    default_tokens = tokens_until_semicolon[default_start_idx + 1 : end_idx]
                    default_value = "".join([t.value for t in default_tokens]).strip()
                    # Include braces in the default value to preserve the syntax
                    default_value = "{" + default_value + "}"
                    tokens_until_semicolon = tokens_until_semicolon[:default_start_idx]
                else:
                    # Unclosed brace - this is a compile error in C++
                    self._add_error(
                        f"Unclosed brace in default value for field in struct '{struct_name}'",
                        first_token.line,
                        first_token.column,
                    )
                    return None

        # Now find the field name: it's typically the last identifier/keyword in the remaining tokens
        # Build the type from everything before the field name
        if not tokens_until_semicolon:
            self._add_error(
                f"Invalid field declaration in struct '{struct_name}': no field name",
                first_token.line,
                first_token.column,
            )
            return None

        # The field name is the last identifier/keyword
        # Everything before it is the type
        field_name = None
        for i in range(len(tokens_until_semicolon) - 1, -1, -1):
            token = tokens_until_semicolon[i]
            if token.type in ("identifier", "keyword"):
                field_name = token.value
                type_tokens = tokens_until_semicolon[:i]
                break

        if not field_name:
            self._add_error(
                f"Invalid field declaration in struct '{struct_name}': "
                f"could not find field name in '{' '.join(t.value for t in tokens_until_semicolon)}'",
                first_token.line,
                first_token.column,
            )
            return None

        # Build type string with proper spacing based on C++ decl-specifier-seq rules
        # Rules:
        # 1. Spaces between decl-specifiers (cv-qualifiers, type specifiers, etc.)
        # 2. No spaces around operators (::, *, &, <, >)
        type_parts = []
        for i, token in enumerate(type_tokens):
            if token.type == "operator":
                # Operators (::, *, &, <, >) attach without spaces
                # They attach to the previous token on the left
                type_parts.append(token.value)
            else:
                # Keywords and identifiers (decl-specifiers) need space before them
                # (except for the first token)
                if i > 0 and type_tokens[i - 1].type != "operator":
                    type_parts.append(" ")
                type_parts.append(token.value)

        field_type = "".join(type_parts).strip()

        # Validate that type is not empty (compile error in C++)
        if not field_type:
            self._add_error(
                f"Field '{field_name}' in struct '{struct_name}' has empty type declaration",
                first_token.line,
                first_token.column,
            )
            return None

        # Check if we found a semicolon (compile error in C++)
        if not found_semicolon:
            self._add_error(
                f"Missing semicolon after field '{field_name}' in struct '{struct_name}'",
                tokens_until_semicolon[-1].line,
                tokens_until_semicolon[-1].column,
            )
            return None

        # Parse attributes to extract prop_name and array_accessor_length
        prop_name = None
        array_accessor_length = None
        prop_attributes = []  # List to collect all prop attributes
        prop_alias = None  # Alias for the first prop attribute

        for attr in attributes:
            parsed = AttributeParser.parse_attribute(attr)

            # Report any attribute parsing errors
            for error in parsed.get("errors", []):
                self._add_error(error, attr.line, attr.column)

            namespace = parsed.get("namespace")
            function = parsed.get("function")

            if namespace == "ge" and function == "params":
                # ge::params should only be used on struct names, not on member variables
                # Log a warning and ignore
                self._add_error(
                    "Warning: [[ge::params]] should be used on struct name, not on member variables. Use [[ge::prop]] instead.",
                    attr.line,
                    attr.column,
                )

            elif namespace == "ge" and function == "prop":
                params = parsed.get("params", {})
                current_prop_name = params.get("NAME") or params.get("name") or params.get("_value")

                current_array_length = None
                if "array_accessor_length" in params:
                    try:
                        current_array_length = int(params["array_accessor_length"])
                    except (ValueError, TypeError):
                        self.errors.append(
                            ParseError(
                                f"Invalid array_accessor_length value: '{params['array_accessor_length']}' (must be an integer)",
                                attr.line,
                                attr.column,
                            )
                        )

                current_array_type = params.get("array_accessor_type")
                current_cast_from = params.get("cast_from")
                current_custom = params.get("custom")
                current_min_value = params.get("min")
                current_max_value = params.get("max")
                current_alias = params.get("alias")

                has_any_param = (current_cast_from is not None or current_custom is not None or
                               current_min_value is not None or current_max_value is not None or
                               current_alias is not None or current_array_length is not None or
                               current_array_type is not None)

                if current_prop_name or has_any_param:
                    prop_attributes.append(
                        PropAttribute(
                            name=current_prop_name or "",
                            array_accessor_length=current_array_length,
                            array_accessor_type=current_array_type,
                            cast_from=current_cast_from,
                            custom=current_custom,
                            min_value=current_min_value,
                            max_value=current_max_value,
                            alias=current_alias,
                        )
                    )

                if not prop_name and current_prop_name:
                    prop_name = current_prop_name

                if array_accessor_length is None and current_array_length is not None:
                    array_accessor_length = current_array_length

        return FieldInfo(
            name=field_name,
            type=field_type,
            prop_name=prop_name,
            prop_attributes=prop_attributes,
            default_value=default_value,
            attributes=attributes,
        )
