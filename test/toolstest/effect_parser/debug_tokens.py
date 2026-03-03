#!/usr/bin/env python3
"""
Debug script to see how the tokenizer/parser handles specific cases.
"""

import sys
from pathlib import Path

# Add parent directory to path
script_dir = Path(__file__).parent
root_dir = script_dir.parent
sys.path.insert(0, str(root_dir))

from tools.generate_reflection_metadata_v2 import (
    CppTokenizer,
    CppParser,
    AttributeParser,
    Token
)

def debug_file(file_path: Path):
    """Debug a specific file."""
    print(f"\n{'='*80}")
    print(f"DEBUGGING: {file_path.name}")
    print(f"{'='*80}")

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    print(f"\nFILE CONTENT:")
    print(content)

    print(f"\n{'-'*80}")
    print("TOKENS:")
    print(f"{'-'*80}")

    # Tokenize
    tokenizer = CppTokenizer(content)
    tokens = tokenizer.tokenize()

    for i, token in enumerate(tokens[:50]):  # Limit to first 50 tokens
        print(f"{i:3d}: [{token.type:12s}] '{token.value}' at line {token.line}")

    if len(tokens) > 50:
        print(f"... ({len(tokens) - 50} more tokens)")

    print(f"\n{'-'*80}")
    print("PARSE RESULT:")
    print(f"{'-'*80}")

    # Parse
    parser = CppParser(tokens, str(file_path))
    structs = parser.parse()

    # Show errors
    if parser.errors:
        print(f"\nErrors/Warnings ({len(parser.errors)}):")
        for error in parser.errors:
            print(f"  {error}")

    if structs:
        for struct in structs:
            print(f"\nStruct: {struct.name}")
            print(f"  Enum Type: {struct.enum_type}")
            print(f"  Filter Name: {struct.filter_name}")
            print(f"  Fields ({len(struct.fields)}):")
            for field in struct.fields:
                default = f" = {field.default_value}" if field.default_value else ""
                print(f"    - {field.type} {field.name}{default}")
    else:
        print("\nNo structs parsed")


def main():
    """Main entry point."""
    test_dir = Path(__file__).parent
    import sys

    # Get file from command line or use defaults
    if len(sys.argv) > 1:
        debug_files = sys.argv[1:]
    else:
        # Debug complex types case to see the parsing issue
        debug_files = [
            "valid/06_complex_types.def",
            "valid/11_cpp11_brace_init.def",
            "invalid/01_missing_semicolon.def",
        ]

    for file_name in debug_files:
        file_path = test_dir / file_name
        if not file_path.exists():
            print(f"File not found: {file_path}")
            continue
        debug_file(file_path)

    return 0


if __name__ == '__main__':
    sys.exit(main())
