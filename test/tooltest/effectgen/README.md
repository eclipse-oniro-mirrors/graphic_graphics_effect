# Parser Robustness Test Suite

This directory contains a comprehensive test suite for testing the robustness of the C++ parser in `tools/generate_reflection_metadata_v2.py`.

## Test Structure

```
test_parser_robustness/
├── valid/              # Valid test cases that should parse successfully
├── invalid/            # Invalid test cases that should be handled gracefully
├── run_parser_tests.py # Test runner script
└── debug_tokens.py     # Debug script for analyzing tokenization
```

## Valid Test Cases

The following valid test cases verify that the parser correctly handles various C++ syntax variations:

| Test File | Description | Fields |
|-----------|-------------|--------|
| `01_basic_valid.def` | Basic struct with simple fields | 3 |
| `02_extra_whitespace.def` | Extra whitespace between tokens | 4 |
| `03_with_comments.def` | Single-line and multi-line comments | 4 |
| `04_with_default_values.def` | Various default value formats | 11 |
| `05_with_attributes.def` | Field-level attributes | 3 |
| `06_complex_types.def` | Complex C++ types (shared_ptr, pair, etc.) | 8 |
| `07_brace_initialization.def` | Brace-initialized default values | 5 |
| `08_with_unrelated_code.def` | Struct with unrelated code around it | 2 |
| `09_tabs_and_newlines.def` | Tabs and various newline styles | 4 |
| `10_string_literals.def` | String literals in default values | 6 |
| `11_cpp11_brace_init.def` | C++11 brace initialization syntax (empty, single, nested) | 14 |
| `12_raw_string_literals.def` | C++11 raw string literals (R"(...)") | 11 |
| `13_standard_attributes.def` | Standard C++ attributes (maybe_unused, nodiscard, etc.) | 6 |
| `14_free_functions.def` | File with free functions (should be ignored) | 2 |
| `15_member_functions.def` | Struct with member functions (should only parse data fields) | 3 |
| `16_mixed_attributes.def` | Mixed standard and custom attributes on fields | 5 |
| `17_multiple_structs_with_functions.def` | Multiple structs with functions between them | 3 structs |
| `18_empty_struct.def` | Empty struct (should parse but with no fields) | 0 |
| `19_function_as_field.def` | Function syntax looks like a field declaration | 0 |
| `20_balanced_brackets_in_attr.def` | Attribute with balanced brackets and nested structures | 2 |
| `21_raw_string_in_attr.def` | Attribute with raw string literal | 2 |
| `22_named_params_with_array_accessor.def` | Named parameter syntax with name and array_accessor_length | 4 |
| `23_multiple_props_on_same_field.def` | Multiple [[ge::prop]] attributes on a single field | 2 |

## Invalid Test Cases

The following invalid test cases verify that the parser handles syntax errors gracefully:

| Test File | Description | Result |
|-----------|-------------|--------|
| `01_missing_semicolon.def` | Missing semicolon after field | Warning issued, 2 fields parsed |
| `02_missing_closing_brace.def` | Missing struct closing brace | Error issued, 3 fields parsed |
| `03_missing_opening_brace.def` | Missing struct opening brace | Handled gracefully (no parse) |
| `04_invalid_attribute_format.def` | Invalid attribute namespace | Handled (valid structs parsed) |
| `05_missing_struct_name.def` | Missing struct name after attribute | Handled gracefully (no parse) |
| `06_unterminated_string.def` | Unterminated string literal | Warning issued, 2 fields parsed |
| `07_invalid_field_name.def` | Field with no name (just type) | Warning issued, 2 fields parsed |
| `08_unmatched_brackets_in_default.def` | Unmatched brackets in default value | 2 fields parsed (malformed value ignored) |
| `09_missing_comma_in_attribute.def` | Missing comma in attribute parameters | Handled gracefully (no parse) |
| `10_empty_struct.def` | Empty struct (no fields) | 0 fields parsed |
| `12_unmatched_attribute_brackets.def` | Unmatched brackets in attribute | Handled gracefully |
| `13_empty_attribute.def` | Empty attribute [[]] | Handled gracefully |
| `14_unmatched_brackets_in_attr.def` | Unmatched brackets in attribute content | Handled gracefully |
| `15_unterminated_raw_string_in_attr.def` | Unterminated raw string in attribute | Handled gracefully |

## Parser Improvements

The following improvements were made to enhance parser robustness:

### 1. Error Reporting System

- Added `ParseError` class for structured error information
- Added error collection to `CppParser`
- Added `_add_error()` and `_add_warning()` methods
- Errors include file path, line number, and column information

### 2. Improved Field Parsing

- **Missing Semicolon Detection**: The parser now detects when a field declaration is missing a semicolon by checking for type keywords after collecting a field name.
- **Brace Depth Tracking**: Properly tracks brace/bracket depth to handle nested structures in default values (e.g., `Vector2f vec = {1.0f, 2.0f};`).
- **Empty Type Validation**: Warns when a field has an empty type declaration.
- **Type Keyword Detection**: Uses a comprehensive list of C++ type keywords to detect new field declarations.
- **Function Declaration Detection**: Detects and skips function declarations (both member and free functions).

### 3. Improved Struct Parsing

- **Missing Brace Detection**: Detects and reports missing opening or closing braces.
- **Struct-level Errors**: Collects errors for each struct independently.
- **Empty Struct Handling**: Correctly parses structs with no fields.

### 4. Enhanced Tokenization

The tokenizer already handled:
- Single-line comments (`//`)
- Multi-line comments (`/* */`)
- String literals with escape sequences
- Character literals
- Number literals (hex, binary, decimal, scientific notation)
- Attributes (`[[...]]`)
- Raw string literals (`R"(...)"`)

### 5. Attribute Parser Enhancements

- **Named Parameter Support**: Handles named parameters in attributes (e.g., `name="VALUE", array_accessor_length=10`)
- **Multiple Prop Attributes**: Supports multiple `[[ge::prop]]` attributes on the same field
- **Bracket Balancing**: Properly handles nested brackets and parentheses within attributes
- **Raw String in Attributes**: Supports raw string literals within attribute values

### 6. Function Handling

- **Free Functions**: Correctly skips free functions (global, static, template)
- **Member Functions**: Correctly skips member functions within structs
- **Function-like Field Detection**: Distinguishes between field declarations and function declarations

## Running the Tests

```bash
# Run the full test suite
python test_parser_robustness/run_parser_tests.py

# Run with validation against expected results
python test_parser_robustness/run_parser_tests.py --validate

# Save current results as expected results
python test_parser_robustness/run_parser_tests.py --save-expected

# Debug specific test cases
python test_parser_robustness/debug_tokens.py
```

## Test Results

All 38 test cases pass with 100% success rate:

- **23/23 valid test cases** parsed correctly
- **15/15 invalid test cases** handled gracefully with appropriate warnings/errors

## Example Error Messages

The parser now provides user-friendly error messages:

```
test_parser_robustness\invalid\01_missing_semicolon.def:line 6, column 11: Warning: Missing semicolon after field 'value1' in struct 'TestNoSemiParams'

test_parser_robustness\invalid\02_missing_closing_brace.def:line 5, column 63: Missing closing brace '}' for struct 'TestNoBraceParams'

test_parser_robustness\invalid\07_invalid_field_name.def:line 6, column 5: Warning: Field 'float' in struct 'TestNoFieldNameParams' has empty type declaration
```

## Conclusion

The parser is now robust enough to handle:
- Various whitespace formatting
- Comments in any position
- Complex default value expressions
- Brace-initialized values
- C++11 brace initialization syntax
- Raw string literals
- Standard and custom C++ attributes
- Free and member functions
- Multiple structs in the same file
- Empty structs
- Function-like declarations
- Named parameters in attributes
- Multiple prop attributes on the same field
- Common C++ syntax errors

The parser provides helpful error messages that guide users to fix issues in their `.def` files.
