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
C++ Tokenizer for the reflection metadata generator.

This module provides tokenization of C++ code, handling:
- Whitespace (spaces, tabs, newlines)
- Single-line comments (// ...)
- Multi-line comments (/* ... */)
- String literals ("..." with escape sequences)
- Character literals ('...' with escape sequences)
- Numbers (integer and floating point)
- Identifiers and keywords
- Operators and punctuation
- Attributes ([[ ... ]])
- Raw string literals (C++11)
"""

from typing import List, Optional, NamedTuple


class Token(NamedTuple):
    """A C++ token."""

    type: str  # 'identifier', 'keyword', 'string', 'char', 'number', 'operator', 'punctuation', 'attribute'
    value: str
    line: int
    column: int


class CppTokenizer:
    """
    A C++ tokenizer that handles:
    - Whitespace (spaces, tabs, newlines)
    - Single-line comments (// ...)
    - Multi-line comments (/* ... */)
    - String literals ("..." with escape sequences)
    - Character literals ('...' with escape sequences)
    - Numbers (integer and floating point)
    - Identifiers and keywords
    - Operators and punctuation
    - Attributes ([[ ... ]])
    """

    KEYWORDS = {
        "struct",
        "class",
        "public",
        "private",
        "protected",
        "static",
        "const",
        "constexpr",
        "inline",
        "template",
        "typename",
        "using",
        "namespace",
        "enum",
        "union",
        "void",
        "bool",
        "char",
        "wchar_t",
        "char8_t",
        "char16_t",
        "char32_t",
        "int",
        "short",
        "long",
        "float",
        "double",
        "signed",
        "unsigned",
        "auto",
        "nullptr",
        "true",
        "false",
    }

    OPERATORS = {
        "++",
        "--",
        "->",
        "->*",
        "<<",
        ">>",
        "<=",
        ">=",
        "==",
        "!=",
        "&&",
        "||",
        "+=",
        "-=",
        "*=",
        "/=",
        "%=",
        "&=",
        "|=",
        "^=",
        "<<=",
        ">>=",
        ".*",
        "?:",
        "::",
        "...",
        "=",
        "+",
        "-",
        "*",
        "/",
        "%",
        "^",
        "&",
        "|",
        "~",
        "!",
        "<",
        ">",
        "(",
        ")",
        "[",
        "]",
        "{",
        "}",
        ".",
        ",",
        ";",
        ":",
        "?",
    }

    def __init__(self, content: str):
        self.content = content
        self.length = len(content)
        self.pos = 0
        self.line = 1
        self.column = 1

    def tokenize(self) -> List[Token]:
        """Tokenize the entire content."""
        tokens = []
        while self.pos < self.length:
            token = self._next_token()
            if token:
                tokens.append(token)
        return tokens

    def _peek(self, n: int = 1) -> str:
        """Peek at the next n characters."""
        end = min(self.pos + n, self.length)
        return self.content[self.pos : end]

    def _advance(self, n: int = 1) -> str:
        """Advance by n characters and return the consumed string."""
        result = self._peek(n)
        for ch in result:
            if ch == "\n":
                self.line += 1
                self.column = 1
            else:
                self.column += 1
        self.pos += n
        return result

    def _skip_whitespace_and_comments(self):
        """Skip whitespace and comments."""
        while self.pos < self.length:
            # Skip whitespace
            if self._peek().isspace():
                self._advance()
                continue

            # Skip single-line comments
            if self._peek(2) == "//":
                self._advance(2)
                while self.pos < self.length and self._peek() != "\n":
                    self._advance()
                continue

            # Skip multi-line comments
            if self._peek(2) == "/*":
                self._advance(2)
                while self.pos < self.length and self._peek(2) != "*/":
                    self._advance()
                if self.pos < self.length:
                    self._advance(2)
                continue

            break

    def _next_token(self) -> Optional[Token]:
        """Get the next token."""
        self._skip_whitespace_and_comments()
        if self.pos >= self.length:
            return None

        ch = self._peek()

        # Attribute start [[
        if ch == "[" and self._peek(2) == "[[":
            return self._read_attribute()

        # Raw string literal (C++11): R"(...)", R"delimiter(...)delimiter"
        if ch == "R" and self._peek(2) == 'R"':
            return self._read_raw_string()

        # String literal
        if ch == '"':
            return self._read_string()

        # Character literal
        if ch == "'":
            return self._read_char()

        # Number
        if ch.isdigit() or (ch == "." and self._peek(2).isdigit()):
            return self._read_number()

        # Identifier or keyword
        if ch.isalpha() or ch == "_":
            return self._read_identifier()

        # Operator or punctuation
        return self._read_operator()

    def _read_string(self) -> Token:
        """Read a string literal."""
        start_line, start_col = self.line, self.column
        self._advance()  # Skip opening "

        value = []
        while self.pos < self.length:
            ch = self._peek()
            if ch == "\\":
                # Escape sequence
                value.append(self._advance())
                if self.pos < self.length:
                    value.append(self._advance())
            elif ch == '"':
                self._advance()  # Skip closing "
                break
            else:
                value.append(self._advance())

        # Note: We don't error on unterminated strings here - the parser will handle it
        # The tokenizer should be tolerant to allow for partial parsing
        return Token("string", "".join(value), start_line, start_col)

    def _read_raw_string(self) -> Token:
        """Read a C++11 raw string literal: R"(...)", R"delimiter(...)delimiter"."""
        start_line, start_col = self.line, self.column
        self._advance(2)  # Skip R"

        # Parse the delimiter (everything between R" and the opening parenthesis)
        delimiter = []
        while self.pos < self.length and self._peek() != "(":
            delimiter.append(self._advance())

        # Check for opening parenthesis
        if self._peek() != "(":
            # Invalid raw string literal - should have opening parenthesis
            return Token("string", "", start_line, start_col)
        self._advance()  # Skip opening (

        delimiter_str = "".join(delimiter)

        # Read the content until we find the closing sequence: )delimiter"
        value = []
        closing_seq = ")" + delimiter_str + '"'

        while self.pos < self.length:
            # Look ahead to see if the closing sequence is at the current position
            if self._peek(len(closing_seq)) == closing_seq:
                # Found the complete closing sequence
                self._advance(len(closing_seq))
                break
            else:
                # Not the closing sequence, consume this character and add to value
                value.append(self._advance())

        return Token("string", "".join(value), start_line, start_col)

    def _read_char(self) -> Token:
        """Read a character literal."""
        start_line, start_col = self.line, self.column
        self._advance()  # Skip opening '

        value = []
        while self.pos < self.length:
            ch = self._peek()
            if ch == "\\":
                # Escape sequence
                value.append(self._advance())
                if self.pos < self.length:
                    value.append(self._advance())
            elif ch == "'":
                self._advance()  # Skip closing '
                break
            else:
                value.append(self._advance())

        return Token("char", "".join(value), start_line, start_col)

    def _read_number(self) -> Token:
        """Read a number (integer or floating point)."""
        start_line, start_col = self.line, self.column
        value = []

        # Hex prefix
        if self._peek(2) == "0x" or self._peek(2) == "0X":
            value.append(self._advance(2))
            while self.pos < self.length and (self._peek().isalnum() or self._peek() == "'"):
                value.append(self._advance())
            return Token("number", "".join(value), start_line, start_col)

        # Binary prefix
        if self._peek(2) == "0b" or self._peek(2) == "0B":
            value.append(self._advance(2))
            while self.pos < self.length and (self._peek() in "01" or self._peek() == "'"):
                value.append(self._advance())
            return Token("number", "".join(value), start_line, start_col)

        # Decimal number
        while self.pos < self.length and (self._peek().isdigit() or self._peek() == "'"):
            value.append(self._advance())

        # Fractional part
        if self.pos < self.length and self._peek() == ".":
            value.append(self._advance())
            while self.pos < self.length and (self._peek().isdigit() or self._peek() == "'"):
                value.append(self._advance())

        # Exponent part
        if self.pos < self.length and self._peek() in "eE":
            value.append(self._advance())
            if self.pos < self.length and self._peek() in "+-":
                value.append(self._advance())
            while self.pos < self.length and (self._peek().isdigit() or self._peek() == "'"):
                value.append(self._advance())

        # Suffix
        if self.pos < self.length and self._peek().isalpha():
            value.append(self._advance())

        return Token("number", "".join(value), start_line, start_col)

    def _read_identifier(self) -> Token:
        """Read an identifier or keyword."""
        start_line, start_col = self.line, self.column
        value = []

        while self.pos < self.length and (self._peek().isalnum() or self._peek() == "_"):
            value.append(self._advance())

        identifier = "".join(value)
        token_type = "keyword" if identifier in self.KEYWORDS else "identifier"

        return Token(token_type, identifier, start_line, start_col)

    def _read_operator(self) -> Token:
        """Read an operator or punctuation."""
        start_line, start_col = self.line, self.column

        # Try to match the longest operator first
        for op in sorted(self.OPERATORS, key=len, reverse=True):
            if self._peek(len(op)) == op:
                self._advance(len(op))
                return Token("operator", op, start_line, start_col)

        # Unknown character (skip it)
        ch = self._advance()
        return Token("punctuation", ch, start_line, start_col)

    def _read_attribute(self) -> Token:
        """Read an attribute [[...]]."""
        start_line, start_col = self.line, self.column
        self._advance(2)  # Skip [[

        depth = 1
        value = ["[", "["]
        while self.pos < self.length and depth > 0:
            ch = self._peek()
            if ch == "[" and self._peek(2) == "[[":
                depth += 1
                value.append(self._advance(2))
            elif ch == "]" and self._peek(2) == "]]":
                depth -= 1
                value.append(self._advance(2))
            else:
                value.append(self._advance())

        return Token("attribute", "".join(value), start_line, start_col)
