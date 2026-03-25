#!/usr/bin/env python3
"""
Validation script for ge_params_reflection_v2.h consistency.

This script validates that:
1. All Params type names (struct names) in ge_params_reflection_v2.h exist in ge_shader_filter_params.h
2. All registered property strings in ge_params_reflection_v2.h exist in ge_shader_filter_params.h

The script parses ge_params_reflection_v2.h (which is relatively regular) and
performs string searches in ge_shader_filter_params.h (which has weak regularity).
"""

import sys
import re
import argparse
from pathlib import Path
from typing import Set, Tuple, List, Dict


def parse_ge_params_reflection_v2(header_path: Path) -> Tuple[Set[str], Set[str], Dict[str, List[str]]]:
    """
    Parse ge_params_reflection_v2.h to extract:
    - Params type names (struct names used in GE_PARAMS_TYPE_INFO)
    - Member tag names (enum values in GEParamsMemberTag)
    - Property names (strings in GEParamsFieldAccessor::name)

    Returns:
        Tuple of (params_types, member_tags, property_names_by_tag)
    """
    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()

    params_types = set()
    member_tags = set()
    property_names_by_tag = {}

    # 1. Extract params type names from GE_PARAMS_TYPE_INFO macro invocations
    # Pattern: GE_PARAMS_TYPE_INFO(StructName, EnumType, FilterName)
    # We need to match the actual invocations, not the macro definition
    # The invocations typically start with 'GE' prefix (e.g., GEAIBarShaderFilterParams)
    params_type_pattern = r'GE_PARAMS_TYPE_INFO\((GE\w+),\s*\w+,\s*\w+\)'
    for match in re.finditer(params_type_pattern, content):
        params_types.add(match.group(1))

    # 2. Extract member tag names from GEParamsMemberTag enum
    # Pattern: enum values inside the enum class GEParamsMemberTag
    enum_start = content.find('enum class GEParamsMemberTag')
    if enum_start == -1:
        print("Error: Could not find GEParamsMemberTag enum", file=sys.stderr)
        return params_types, member_tags, property_names_by_tag

    # Find the closing brace of the enum
    enum_end = content.find('};', enum_start)
    if enum_end == -1:
        print("Error: Could not find end of GEParamsMemberTag enum", file=sys.stderr)
        return params_types, member_tags, property_names_by_tag

    enum_content = content[enum_start:enum_end]

    # Extract all enum values (excluding INVALID and comments)
    enum_value_pattern = r'\b([A-Z][A-Z0-9_]*)\s*,'
    for match in re.finditer(enum_value_pattern, enum_content):
        tag_name = match.group(1)
        if tag_name != 'INVALID':
            member_tags.add(tag_name)

    # 3. Extract property names from GEParamsFieldAccessor specializations
    # Pattern: GE_PARAMS_FIELD_ACCESSOR(Struct, Field, Tag, PropName)
    # Pattern: GE_PARAMS_ARRAY_ELEMENT_ACCESSOR(Struct, Field, Index, Tag, PropName)
    # Note: We require the first parameter to start with 'GE' to skip macro definition
    field_accessor_pattern = r'GE_PARAMS_FIELD_ACCESSOR\((GE\w+),\s*[^,]+,\s*(\w+),\s*(\w+)\)'
    array_accessor_pattern = r'GE_PARAMS_ARRAY_ELEMENT_ACCESSOR\((GE\w+),\s*[^,]+,\s*[^,]+,\s*(\w+),\s*(\w+)\)'

    for match in re.finditer(field_accessor_pattern, content):
        tag_name = match.group(2)
        prop_name = match.group(3)
        if tag_name not in property_names_by_tag:
            property_names_by_tag[tag_name] = []
        property_names_by_tag[tag_name].append(prop_name)

    for match in re.finditer(array_accessor_pattern, content):
        tag_name = match.group(2)
        prop_name = match.group(3)
        if tag_name not in property_names_by_tag:
            property_names_by_tag[tag_name] = []
        property_names_by_tag[tag_name].append(prop_name)

    return params_types, member_tags, property_names_by_tag


def search_in_shader_filter_params(shader_params_path: Path, search_strings: Set[str]) -> Dict[str, bool]:
    """
    Search for strings in ge_shader_filter_params.h.

    Args:
        shader_params_path: Path to ge_shader_filter_params.h
        search_strings: Set of strings to search for

    Returns:
        Dictionary mapping each string to whether it was found
    """
    with open(shader_params_path, 'r', encoding='utf-8') as f:
        content = f.read()

    results = {}
    for search_str in search_strings:
        # Search for string as a literal or identifier
        # We search for patterns like:
        # - "StringName" (as a string literal - this is the primary search)
        # - GE_FILTER_STRING_NAME (as a constant name)
        # - struct StringName (as a struct name)

        found = False

        # Check for string literal (primary search - this is what's used at runtime)
        if f'"{search_str}"' in content:
            found = True

        # Also check for string literal with backslash escape (e.g., "AIBAR_LOW")
        if f'\\"{search_str}\\"' in content:
            found = True

        # Check for constant name (common pattern: GE_FILTER_XXX or GE_MASK_XXX or GE_SHADER_XXX or GEX_SHADER_XXX)
        # This is a fallback for cases where the constant name matches the search string
        constant_patterns = [
            f'GE_FILTER_{search_str}',
            f'GE_MASK_{search_str}',
            f'GE_SHADER_{search_str}',
            f'GEX_SHADER_{search_str}',
            f'GE_{search_str}',
        ]
        for pattern in constant_patterns:
            if pattern in content:
                found = True
                break

        # Check for struct definition
        if f'struct {search_str}' in content:
            found = True

        results[search_str] = found

    return results


def validate_params_types(params_types: Set[str], shader_params_path: Path) -> Tuple[List[str], List[str]]:
    """
    Validate that all params types exist in ge_shader_filter_params.h.

    Returns:
        Tuple of (found_types, missing_types)
    """
    search_results = search_in_shader_filter_params(shader_params_path, params_types)

    found = [name for name, found in search_results.items() if found]
    missing = [name for name, found in search_results.items() if not found]

    return found, missing


def validate_property_names(property_names_by_tag: Dict[str, List[str]], shader_params_path: Path) -> Tuple[Dict[str, List[str]], Dict[str, List[str]]]:
    """
    Validate that all property names exist in ge_shader_filter_params.h.

    Returns:
        Tuple of (found_names_by_tag, missing_names_by_tag)
    """
    # Collect all unique property names
    all_property_names = set()
    for names in property_names_by_tag.values():
        all_property_names.update(names)

    search_results = search_in_shader_filter_params(shader_params_path, all_property_names)

    found_by_tag = {}
    missing_by_tag = {}

    for tag_name, names in property_names_by_tag.items():
        found_by_tag[tag_name] = [name for name in names if search_results.get(name, False)]
        missing_by_tag[tag_name] = [name for name in names if not search_results.get(name, False)]

    return found_by_tag, missing_by_tag


def extract_shader_params_property_names(shader_params_path: Path) -> Set[str]:
    """
    Extract all property string literals from ge_shader_filter_params.h.

    This extracts strings like "AIBAR_LOW", "BorderLight_Color", etc.
    Only matches strings assigned to constexpr char variables with specific prefixes.
    """
    with open(shader_params_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Extract string literals that are assigned to constexpr char variables
    # Pattern matches: constexpr char GE_FILTER_XXX[] = "PropertyName";
    #              constexpr char GE_MASK_XXX[] = "PropertyName";
    #              constexpr char GE_SHADER_XXX[] = "PropertyName";
    #              constexpr char GEX_SHADER_XXX[] = "PropertyName";
    # This avoids matching random strings in comments or other contexts
    # Require at least one underscore in the property name to exclude filter type names
    property_pattern = r'constexpr\s+char\s+(?:GE_FILTER_|GE_MASK_|GE_SHADER_|GEX_SHADER_)\w+\[\]\s*=\s*"([^"]+)"'
    property_names = set(match.group(1) for match in re.finditer(property_pattern, content) if '_' in match.group(1))

    return property_names


def main():
    parser = argparse.ArgumentParser(
        description='Validate ge_params_reflection_v2.h consistency with ge_shader_filter_params.h',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python validate_reflection_metadata.py
  python validate_reflection_metadata.py --reflection-header custom/reflection.h --shader-params custom/params.h
        """
    )

    parser.add_argument(
        '--reflection-header',
        type=str,
        default=None,
        help='Path to ge_params_reflection_v2.h (default: include/effect/ge_params_reflection_v2.h)'
    )

    parser.add_argument(
        '--shader-params',
        type=str,
        default=None,
        help='Path to ge_shader_filter_params.h (default: include/effect/ge_shader_filter_params.h)'
    )

    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Show detailed information about all found items'
    )

    parser.add_argument(
        '--check-regression',
        action='store_true',
        help='Check for properties in shader_params.h that are missing from reflection header'
    )

    args = parser.parse_args()

    # Determine file paths
    script_dir = (Path(__file__).parent.parent)
    root_dir = script_dir

    reflection_header = Path(args.reflection_header) if args.reflection_header else root_dir / "include" / "effect" / "ge_params_reflection_v2.h"
    shader_params = Path(args.shader_params) if args.shader_params else root_dir / "include" / "effect" / "ge_shader_filter_params.h"

    # Check if files exist
    if not reflection_header.exists():
        print(f"Error: Reflection header not found: {reflection_header}", file=sys.stderr)
        return 1

    if not shader_params.exists():
        print(f"Error: Shader params header not found: {shader_params}", file=sys.stderr)
        return 1

    print(f"Parsing reflection header: {reflection_header}")
    print(f"Validating against: {shader_params}")
    print()

    # Parse ge_params_reflection_v2.h
    params_types, member_tags, property_names_by_tag = parse_ge_params_reflection_v2(reflection_header)

    print(f"Found {len(params_types)} params types")
    print(f"Found {len(member_tags)} member tags")
    print(f"Found {sum(len(names) for names in property_names_by_tag.values())} property names")
    print()

    # Validate params types
    print("=" * 80)
    print("VALIDATING PARAMS TYPES")
    print("=" * 80)
    found_types, missing_types = validate_params_types(params_types, shader_params)

    if args.verbose:
        print(f"\nFound params types ({len(found_types)}):")
        for name in sorted(found_types):
            print(f"  ✓ {name}")

    if missing_types:
        print(f"\n❌ MISSING params types ({len(missing_types)}):")
        for name in sorted(missing_types):
            print(f"  ✗ {name}")
    else:
        print(f"\n✓ All {len(found_types)} params types found!")

    # Validate property names
    print()
    print("=" * 80)
    print("VALIDATING PROPERTY NAMES")
    print("=" * 80)

    found_by_tag, missing_by_tag = validate_property_names(property_names_by_tag, shader_params)

    total_found = sum(len(names) for names in found_by_tag.values())
    total_missing = sum(len(names) for names in missing_by_tag.values())

    if args.verbose:
        print(f"\nFound property names by tag ({total_found}):")
        for tag_name in sorted(found_by_tag.keys()):
            names = found_by_tag[tag_name]
            if names:
                print(f"  {tag_name}:")
                for name in names:
                    print(f"    ✓ {name}")

    if missing_by_tag:
        print(f"\n❌ MISSING property names ({total_missing}):")
        for tag_name in sorted(missing_by_tag.keys()):
            names = missing_by_tag[tag_name]
            if names:
                print(f"  {tag_name}:")
                for name in names:
                    print(f"    ✗ {name}")
    else:
        print(f"\n✓ All {total_found} property names found!")

    # Summary
    print()
    print("=" * 80)
    print("SUMMARY")
    print("=" * 80)

    total_issues = len(missing_types) + total_missing

    if total_issues == 0:
        print("✓ VALIDATION PASSED: All params types and property names are consistent!")
    else:
        print(f"❌ VALIDATION FAILED: {total_issues} issues found")
        print(f"  - Missing params types: {len(missing_types)}")
        print(f"  - Missing property names: {total_missing}")

    # Regression check: find properties in shader_params.h that are missing from reflection header
    # This runs independently of whether validation passes or fails
    if args.check_regression:
        print()
        print("=" * 80)
        print("REGRESSION CHECK: Properties in shader_params.h missing from reflection header")
        print("=" * 80)

        shader_params_properties = extract_shader_params_property_names(shader_params)
        reflection_properties = set()
        for names in property_names_by_tag.values():
            reflection_properties.update(names)

        missing_from_reflection = shader_params_properties - reflection_properties

        if missing_from_reflection:
            print(f"\n⚠️  WARNING: {len(missing_from_reflection)} properties in shader_params.h are NOT in reflection header")
            print("   These properties may have been removed or renamed in the new system:")
            for name in sorted(missing_from_reflection):
                    # Skip if it looks like a filter type name (no underscore)
                    if '_' not in name:
                        continue
                    print(f"     - {name}")
        else:
            print("\n✓ No regression detected: All properties in shader_params.h are in reflection header")

    return 0 if total_issues == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
