#!/usr/bin/env python3
"""
Parser robustness test runner.

This script runs the parser on various valid and invalid test cases
and checks if it handles them correctly with appropriate error messages.
"""

import sys
import os
from pathlib import Path
from typing import Tuple, Dict, List, Any, Optional
import traceback
import json
from dataclasses import dataclass, asdict

# Add parent directory to path to import the parser module
script_dir = Path(__file__).parent
root_dir = script_dir.parent.parent.parent
sys.path.insert(0, str(root_dir))

from tool.effectgen.cpp_tokenizer import CppTokenizer, Token
from tool.effectgen.cpp_parser import CppParser
from tool.effectgen.attribute_parser import AttributeParser


@dataclass
class PropAttributeExpected:
    """Expected result for a single prop attribute."""
    name: str
    array_accessor_length: Optional[int] = None
    alias: Optional[str] = None


@dataclass
class RangeAttributeExpected:
    """Expected result for a range attribute."""
    min_value: Optional[str] = None
    min_components: Optional[List[str]] = None
    max_value: Optional[str] = None
    max_components: Optional[List[str]] = None


@dataclass
class FieldExpectedResult:
    """Expected result for a parsed field."""
    type: str
    name: str
    default_value: str = ""
    prop_name: str = ""
    array_accessor_length: Optional[int] = None
    prop_attributes: List[PropAttributeExpected] = None
    range_attr: Optional[RangeAttributeExpected] = None

    def __post_init__(self):
        if self.prop_attributes is None:
            self.prop_attributes = []


@dataclass
class StructExpectedResult:
    """Expected result for a parsed struct."""
    name: str
    enum_type: str
    filter_name: str
    fields: List[FieldExpectedResult]


@dataclass
class TestCaseExpectedResult:
    """Expected result for a test case."""
    name: str
    should_parse: bool
    expected_error_count: int = 0
    expected_errors: List[str] = None
    structs: List[StructExpectedResult] = None
    notes: str = ""

    def __post_init__(self):
        if self.expected_errors is None:
            self.expected_errors = []
        if self.structs is None:
            self.structs = []


class TestCase:
    """Represents a test case with expected outcome."""

    def __init__(self, name: str, file_path: Path, should_parse: bool,
                 expected_error: str = None, expected_fields: int = None):
        self.name = name
        self.file_path = file_path
        self.should_parse = should_parse
        self.expected_error = expected_error
        self.expected_fields = expected_fields
        self.result = None  # First struct (for backward compatibility)
        self.results = []  # All structs parsed
        self.parser_errors = []  # Parser errors/warnings
        self.error = None  # Test-specific error
        self.passed = False
        self.validation_passed = None  # Validation result (None=not validated, True=passed, False=failed)
        self.validation_differences = []  # List of validation differences


class TestRunner:
    """Runs parser robustness tests."""

    def __init__(self, test_dir: Path):
        self.test_dir = test_dir
        self.valid_dir = test_dir / "valid"
        self.invalid_dir = test_dir / "invalid"
        self.test_cases: List[TestCase] = []
        self.results: Dict[str, List[TestCase]] = {
            "passed": [],
            "failed": [],
            "errors": []
        }
        self.expected_results: Dict[str, TestCaseExpectedResult] = {}
        self.validation_results: List[Dict[str, Any]] = []

    def load_expected_results(self) -> bool:
        """Load expected results from individual JSON files alongside test files."""
        loaded_count = 0
        for test_case in self.test_cases:
            # Expected result file is named: test_name.def.json (alongside the test file)
            expected_file = test_case.file_path.with_suffix('.def.json')

            if not expected_file.exists():
                continue

            try:
                with open(expected_file, 'r', encoding='utf-8') as f:
                    result_data = json.load(f)

                structs = [StructExpectedResult(
                    name=s['name'],
                    enum_type=s['enum_type'],
                    filter_name=s['filter_name'],
                    fields=[FieldExpectedResult(
                        type=f['type'],
                        name=f['name'],
                        default_value=f.get('default_value', ''),
                        prop_name=f.get('prop_name', ''),
                        array_accessor_length=f.get('array_accessor_length'),
                        prop_attributes=[PropAttributeExpected(
                            name=pa.get('name', ''),
                            array_accessor_length=pa.get('array_accessor_length'),
                            alias=pa.get('alias')
                        ) for pa in f.get('prop_attributes', [])],
                        range_attr=RangeAttributeExpected(
                            min_value=f.get('range_attr', {}).get('min_value'),
                            min_components=f.get('range_attr', {}).get('min_components'),
                            max_value=f.get('range_attr', {}).get('max_value'),
                            max_components=f.get('range_attr', {}).get('max_components')
                        ) if 'range_attr' in f else None
                    ) for f in s.get('fields', [])]
                ) for s in result_data.get('structs', [])]

                self.expected_results[test_case.file_path.stem] = TestCaseExpectedResult(
                    name=result_data['name'],
                    should_parse=result_data['should_parse'],
                    expected_error_count=result_data.get('expected_error_count', 0),
                    expected_errors=result_data.get('expected_errors', []),
                    structs=structs,
                    notes=result_data.get('notes', '')
                )
                loaded_count += 1
            except Exception as e:
                print(f"Error loading expected result from {expected_file}: {e}")

        if loaded_count > 0:
            print(f"Loaded {loaded_count} expected results from .def.json files alongside test files")
        else:
            print(f"Note: No .def.json files found alongside test files")

        return loaded_count > 0

    def save_expected_results(self):
        """Save current test results as expected results to individual JSON files."""
        saved_count = 0
        for test_case in self.test_cases:
            test_name = test_case.file_path.stem

            # Extract struct results
            structs_data = []
            for struct in test_case.results:
                fields_data = []
                for field in struct.fields:
                    field_dict = {
                        "type": field.type,
                        "name": field.name,
                        "default_value": field.default_value or "",
                        "prop_name": field.prop_name or ""
                    }
                    # Include prop_attributes if there are any
                    if field.prop_attributes:
                        field_dict["prop_attributes"] = [
                            {
                                "name": pa.name,
                                "array_accessor_length": pa.array_accessor_length,
                                "alias": pa.alias
                            } for pa in field.prop_attributes
                        ]
                    if field.range_attr:
                        field_dict["range_attr"] = {
                            "min_value": field.range_attr.min_value,
                            "min_components": field.range_attr.min_components,
                            "max_value": field.range_attr.max_value,
                            "max_components": field.range_attr.max_components
                        }
                    fields_data.append(field_dict)

                struct_data = StructExpectedResult(
                    name=struct.name,
                    enum_type=struct.enum_type,
                    filter_name=struct.filter_name,
                    fields=fields_data
                )
                structs_data.append(asdict(struct_data))

            # Build expected result
            # Convert parser error objects to strings for JSON serialization
            # Use relative paths (valid/... or invalid/...) instead of absolute paths
            error_strings = []
            for error in test_case.parser_errors:
                error_str = str(error)
                # Convert absolute path to relative path (e.g., M:\...\valid\test.def -> valid/test.def)
                # Error format: "path:line X, column Y: message"
                # Need to handle Windows paths like "M:\dir\file.def:line X"
                # Split on the first ":line" to find the path
                line_idx = error_str.find(':line')
                if line_idx > 0:
                    abs_path = error_str[:line_idx]
                    rest = error_str[line_idx:]
                    # Convert to relative path from test directory
                    try:
                        rel_path = str(Path(abs_path).relative_to(self.test_dir))
                        # Normalize path separators to forward slashes for cross-platform compatibility
                        rel_path = rel_path.replace('\\', '/')
                        error_str = rel_path + rest
                    except (ValueError, TypeError):
                        # If conversion fails, keep original
                        pass
                error_strings.append(error_str)

            expected_result = TestCaseExpectedResult(
                name=test_name,
                should_parse=test_case.should_parse,
                expected_error_count=len(test_case.parser_errors),
                expected_errors=error_strings,
                structs=structs_data,
                notes=""
            )

            # Save to a .def.json file next to the test file
            output_file = test_case.file_path.with_suffix('.def.json')
            with open(output_file, 'w', encoding='utf-8') as f:
                json.dump(asdict(expected_result), f, indent=2, ensure_ascii=False)

            saved_count += 1

        print(f"\nSaved {saved_count} expected results files alongside test files")
        print(f"Pattern: <test_file>.def.json")

    def validate_results(self) -> List[Dict[str, Any]]:
        """Validate actual results against expected results."""
        validation_results = []

        for test_case in self.test_cases:
            test_name = test_case.file_path.stem
            validation = {
                'test_name': test_name,
                'file_name': test_case.file_path.name,
                'passed': True,
                'differences': []
            }

            if test_name not in self.expected_results:
                validation['passed'] = False
                validation['differences'].append("No expected result found for this test")
                validation_results.append(validation)
                continue

            expected = self.expected_results[test_name]

            # Validate error count
            actual_error_count = len(test_case.parser_errors)
            if actual_error_count != expected.expected_error_count:
                validation['passed'] = False
                validation['differences'].append(
                    f"Error count mismatch: expected {expected.expected_error_count}, got {actual_error_count}"
                )

            # Validate structs for valid tests
            if test_case.should_parse and test_case.results:
                expected_structs = expected.structs
                actual_structs = test_case.results

                if len(expected_structs) != len(actual_structs):
                    validation['passed'] = False
                    validation['differences'].append(
                        f"Struct count mismatch: expected {len(expected_structs)}, got {len(actual_structs)}"
                    )

                # Compare each struct
                for i, (exp_struct, act_struct) in enumerate(zip(expected_structs, actual_structs)):
                    # Check struct name
                    if exp_struct.name != act_struct.name:
                        validation['passed'] = False
                        validation['differences'].append(
                            f"Struct {i+1} name mismatch: expected '{exp_struct.name}', got '{act_struct.name}'"
                        )

                    # Check enum type
                    if exp_struct.enum_type != act_struct.enum_type:
                        validation['passed'] = False
                        validation['differences'].append(
                            f"Struct {i+1} enum_type mismatch: expected '{exp_struct.enum_type}', got '{act_struct.enum_type}'"
                        )

                    # Check filter name
                    if exp_struct.filter_name != act_struct.filter_name:
                        validation['passed'] = False
                        validation['differences'].append(
                            f"Struct {i+1} filter_name mismatch: expected '{exp_struct.filter_name}', got '{act_struct.filter_name}'"
                        )

                    # Check fields
                    exp_fields = exp_struct.fields
                    act_fields = act_struct.fields

                    if len(exp_fields) != len(act_fields):
                        validation['passed'] = False
                        validation['differences'].append(
                            f"Struct {i+1} field count mismatch: expected {len(exp_fields)}, got {len(act_fields)}"
                        )

                    # Compare each field
                    for j, (exp_field, act_field) in enumerate(zip(exp_fields, act_fields)):
                        if exp_field.type != act_field.type:
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} type mismatch: expected '{exp_field.type}', got '{act_field.type}'"
                            )

                        if exp_field.name != act_field.name:
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} name mismatch: expected '{exp_field.name}', got '{act_field.name}'"
                            )

                        exp_default = exp_field.default_value or ''
                        act_default = act_field.default_value or ''
                        if exp_default != act_default:
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} default_value mismatch: expected '{exp_default}', got '{act_default}'"
                            )

                        exp_prop = exp_field.prop_name or ''
                        act_prop = act_field.prop_name or ''
                        if exp_prop != act_prop:
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} prop_name mismatch: expected '{exp_prop}', got '{act_prop}'"
                            )

                        # Note: array_accessor_length check removed - using prop_attributes only
                        # exp_array_len and act_array_len no longer used

                        # Check prop_attributes count
                        exp_props = exp_field.prop_attributes
                        act_props = act_field.prop_attributes
                        if len(exp_props) != len(act_props):
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} prop_attributes count mismatch: expected {len(exp_props)}, got {len(act_props)}"
                            )

                        # Check each prop_attribute
                        for k, (exp_prop, act_prop) in enumerate(zip(exp_props, act_props)):
                            if exp_prop.name != act_prop.name:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1}, prop_attribute {k+1} name mismatch: expected '{exp_prop.name}', got '{act_prop.name}'"
                                )
                            if exp_prop.array_accessor_length != act_prop.array_accessor_length:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1}, prop_attribute {k+1} array_accessor_length mismatch: expected {exp_prop.array_accessor_length}, got {act_prop.array_accessor_length}"
                                )
                            if exp_prop.alias != act_prop.alias:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1}, prop_attribute {k+1} alias mismatch: expected '{exp_prop.alias}', got '{act_prop.alias}'"
                                )
                            if exp_prop.array_accessor_length != act_prop.array_accessor_length:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1} prop_attribute {k+1} array_accessor_length mismatch: expected {exp_prop.array_accessor_length}, got {act_prop.array_accessor_length}"
                                )

                        exp_range = exp_field.range_attr
                        act_range = act_field.range_attr
                        if (exp_range is None) != (act_range is None):
                            validation['passed'] = False
                            validation['differences'].append(
                                f"Struct {i+1}, field {j+1} range_attr presence mismatch: expected {exp_range is not None}, got {act_range is not None}"
                            )
                        elif exp_range is not None and act_range is not None:
                            if exp_range.min_value != act_range.min_value:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1} range_attr min_value mismatch: expected '{exp_range.min_value}', got '{act_range.min_value}'"
                                )
                            if exp_range.min_components != act_range.min_components:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1} range_attr min_components mismatch: expected {exp_range.min_components}, got {act_range.min_components}"
                                )
                            if exp_range.max_value != act_range.max_value:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1} range_attr max_value mismatch: expected '{exp_range.max_value}', got '{act_range.max_value}'"
                                )
                            if exp_range.max_components != act_range.max_components:
                                validation['passed'] = False
                                validation['differences'].append(
                                    f"Struct {i+1}, field {j+1} range_attr max_components mismatch: expected {exp_range.max_components}, got {act_range.max_components}"
                                )

            validation_results.append(validation)

            # Store validation result in the test case for inline display
            test_case.validation_passed = validation['passed']
            test_case.validation_differences = validation['differences']

        self.validation_results = validation_results
        return validation_results

    def discover_tests(self):
        """Discover all test cases."""
        # Valid test cases - should parse successfully
        valid_files = sorted(self.valid_dir.glob("*.def"))
        for i, file_path in enumerate(valid_files, 1):
            test_case = TestCase(
                name=f"valid_{i:02d}_{file_path.stem}",
                file_path=file_path,
                should_parse=True
            )
            self.test_cases.append(test_case)

        # Invalid test cases - should fail gracefully
        invalid_files = sorted(self.invalid_dir.glob("*.def"))
        for i, file_path in enumerate(invalid_files, 1):
            test_case = TestCase(
                name=f"invalid_{i:02d}_{file_path.stem}",
                file_path=file_path,
                should_parse=False
            )
            self.test_cases.append(test_case)

    def run_test(self, test_case: TestCase) -> bool:
        """Run a single test case."""
        try:
            with open(test_case.file_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # Tokenize
            tokenizer = CppTokenizer(content)
            tokens = tokenizer.tokenize()

            # Parse
            parser = CppParser(tokens, str(test_case.file_path))
            structs = parser.parse()

            # Store parser errors
            test_case.parser_errors = parser.errors

            if structs:
                test_case.result = structs[0]
                test_case.results = structs
                parsed_successfully = True
            else:
                test_case.results = []
                parsed_successfully = False

            # Check if result matches expectation
            if test_case.should_parse:
                # Should have parsed successfully
                if parsed_successfully:
                    test_case.passed = True
                    if test_case.expected_fields is not None:
                        # Check first struct's field count
                        if len(structs[0].fields) != test_case.expected_fields:
                            test_case.passed = False
                            test_case.error = (
                                f"Expected {test_case.expected_fields} fields, "
                                f"got {len(structs[0].fields)}"
                            )
                else:
                    test_case.passed = False
                    test_case.error = "Expected to parse successfully but got no structs"
            else:
                # Should have failed gracefully (either no structs or handled error)
                # For invalid cases, we're just checking it doesn't crash
                test_case.passed = True  # Not crashing is considered a pass
                test_case.results = structs  # Still store what was parsed

            return test_case.passed

        except Exception as e:
            test_case.error = str(e)
            test_case.error_traceback = traceback.format_exc()

            # For invalid test cases, exceptions might be acceptable
            if not test_case.should_parse:
                test_case.passed = True  # Handled gracefully with exception
            else:
                test_case.passed = False

            return test_case.passed

    def run_all_tests(self):
        """Run all test cases."""
        print(f"Running {len(self.test_cases)} test cases...")
        print("=" * 80)

        for test_case in self.test_cases:
            self.run_test(test_case)

            # Categorize results
            if test_case.passed:
                self.results["passed"].append(test_case)
            else:
                if test_case.should_parse:
                    self.results["failed"].append(test_case)
                else:
                    self.results["errors"].append(test_case)

    def print_results(self):
        """Print test results."""
        ok = len(self.results["passed"])
        failed = len(self.results["failed"])
        errors = len(self.results["errors"])
        total = len(self.test_cases)

        print("\n" + "=" * 80)
        print("TEST RESULTS")
        print("=" * 80)

        # Print summary
        print(f"\nTotal: {total} | OK: {ok} | FAILED: {failed} | ERROR: {errors}")

        # Check if validation was run
        has_validation = any(t.validation_passed is not None for t in self.test_cases)

        # Print validation summary if validation was run
        if has_validation:
            validated = sum(1 for t in self.test_cases if t.validation_passed is not None)
            validation_passed = sum(1 for t in self.test_cases if t.validation_passed is True)
            validation_failed = sum(1 for t in self.test_cases if t.validation_passed is False)

            print(f"\nValidation: {validation_passed}/{validated} tests match expected results")
            if validation_failed > 0:
                print(f"  [!] {validation_failed} differ(s) - see details below")
            else:
                print(f"  [OK] All match expected results")
        else:
            print("\nValidation: not enabled (use --validate)")

        # Print failed tests
        if self.results["failed"]:
            print("\n" + "-" * 80)
            print("FAILED TESTS:")
            print("-" * 80)
            for test in self.results["failed"]:
                print(f"\n[x] {test.name}:")
                print(f"    File: {test.file_path.name}")
                if test.error:
                    print(f"    Error: {test.error}")

        # Organize passed tests by validation status
        if has_validation:
            validated_tests = [t for t in self.results["passed"] if t.validation_passed is True]
            not_validated_tests = [t for t in self.results["passed"] if t.validation_passed is None]
            diff_tests = [t for t in self.results["passed"] if t.validation_passed is False]

            # Print validated tests
            if validated_tests:
                print("\n" + "-" * 80)
                print("OK TESTS - validated (match expected):")
                print("-" * 80)
                for test in validated_tests:
                    if test.result:
                        struct_count = len(test.results)
                        fields = len(test.result.fields)
                        if struct_count > 1:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {struct_count} structs, first has {fields} fields [v]")
                        else:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {fields} fields [v]")
                    else:
                        print(f"[OK] {test.name} ({test.file_path.name}) - handled gracefully [v]")

            # Print tests that differ from expected
            if diff_tests:
                print("\n" + "-" * 80)
                print("OK TESTS - differ from expected:")
                print("-" * 80)
                for test in diff_tests:
                    if test.result:
                        struct_count = len(test.results)
                        fields = len(test.result.fields)
                        if struct_count > 1:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {struct_count} structs, first has {fields} fields [x]")
                        else:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {fields} fields [x]")

                        # Print validation differences inline
                        for diff in test.validation_differences:
                            print(f"       [x] {diff}")
                    else:
                        print(f"[OK] {test.name} ({test.file_path.name}) - handled gracefully [x]")

            # Print tests without validation data
            if not_validated_tests:
                print("\n" + "-" * 80)
                print("OK TESTS - not validated:")
                print("-" * 80)
                for test in not_validated_tests:
                    if test.result:
                        struct_count = len(test.results)
                        fields = len(test.result.fields)
                        if struct_count > 1:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {struct_count} structs, first has {fields} fields")
                        else:
                            print(f"[OK] {test.name} ({test.file_path.name}) - {fields} fields")
                    else:
                        print(f"[OK] {test.name} ({test.file_path.name}) - handled gracefully")
        else:
            # No validation - just print all tests together
            print("\n" + "-" * 80)
            print("OK TESTS:")
            print("-" * 80)
            for test in self.results["passed"]:
                if test.result:
                    struct_count = len(test.results)
                    fields = len(test.result.fields)
                    if struct_count > 1:
                        print(f"[OK] {test.name} ({test.file_path.name}) - {struct_count} structs, first has {fields} fields")
                    else:
                        print(f"[OK] {test.name} ({test.file_path.name}) - {fields} fields")
                else:
                    print(f"[OK] {test.name} ({test.file_path.name}) - handled gracefully")

        # Print detailed results for valid tests
        print("\n" + "=" * 80)
        print("DETAILED RESULTS FOR VALID TESTS:")
        print("=" * 80)
        for test in self.test_cases:
            if test.should_parse and test.passed and test.results:
                for i, struct in enumerate(test.results):
                    if len(test.results) > 1:
                        print(f"\n{test.name} ({test.file_path.name}) - Struct {i+1}/{len(test.results)}:")
                    else:
                        print(f"\n{test.name} ({test.file_path.name}):")
                    print(f"  Struct: {struct.name}")
                    print(f"  Enum Type: {struct.enum_type}")
                    print(f"  Filter Name: {struct.filter_name}")
                    print(f"  Fields ({len(struct.fields)}):")
                    for field in struct.fields:
                        default = f" = {field.default_value}" if field.default_value else ""
                        prop = f" [prop: {field.prop_name}]" if field.prop_name else ""
                        props_info = ""
                        if field.prop_attributes:
                            props_list = []
                            for p in field.prop_attributes:
                                prop_str = p.name
                                if p.array_accessor_length is not None:
                                    prop_str += f"[{p.array_accessor_length}]"
                                if p.alias:
                                    prop_str += f" (alias: {p.alias})"
                                props_list.append(prop_str)
                            props_info = f" [props: {', '.join(props_list)}]"
                        print(f"    - type: '{field.type}'")
                        print(f"      name: '{field.name}'{prop}{props_info}")
                        if default:
                            print(f"      default:{default}")
                        # Validate the field parsing
                        if not field.name or field.name == field.type:
                            print(f"      ⚠ WARNING: Field name appears to be missing or incorrect!")
                        if not field.type:
                            print(f"      ⚠ WARNING: Field type is empty!")

        # Print detailed results for invalid tests
        print("\n" + "=" * 80)
        print("DETAILED RESULTS FOR INVALID TESTS:")
        print("=" * 80)
        for test in self.test_cases:
            if not test.should_parse:
                print(f"\n{test.name} ({test.file_path.name}):")

                # Show what was parsed
                if test.results:
                    print(f"  Parsed {len(test.results)} struct(s) despite being invalid test:")
                    for struct in test.results:
                        print(f"    - {struct.name} ({len(struct.fields)} fields)")

                # Show parser errors/warnings
                if test.parser_errors:
                    print(f"  Parser errors/warnings ({len(test.parser_errors)}):")
                    for error in test.parser_errors:
                        print(f"    - {error}")

                # Show test-specific errors (exceptions)
                if test.error:
                    print(f"  Test error: {test.error}")

                # If nothing was parsed and no errors, note that
                if not test.results and not test.parser_errors and not test.error:
                    print(f"  No structs parsed, no errors reported")

        return failed == 0  # Return True if all tests passed


def main():
    """Main entry point."""
    import argparse

    parser = argparse.ArgumentParser(description='Parser robustness test runner')
    parser.add_argument('--save-expected', action='store_true',
                        help='Save current results as expected results (saves .def.json files alongside test files)')
    parser.add_argument('--validate', action='store_true',
                        help='Validate results against expected results')

    args = parser.parse_args()

    test_dir = Path(__file__).parent

    # Create test runner
    runner = TestRunner(test_dir)
    runner.discover_tests()

    # Load expected results if validating
    if args.validate:
        runner.load_expected_results()

    runner.run_all_tests()

    # Validate against expected results if requested (BEFORE printing for inline status)
    if args.validate and runner.expected_results:
        runner.validate_results()

    success = runner.print_results()

    # Print validation summary if requested
    if args.validate and runner.expected_results:
        print("\n" + "=" * 80)
        print("VALIDATION SUMMARY")
        print("=" * 80)

        # Print validation summary
        passed_validation = sum(1 for v in runner.validation_results if v['passed'])
        total_validation = len(runner.validation_results)

        # Print prominent validation status (using ASCII for Windows compatibility)
        if passed_validation == total_validation:
            print(f"\n[OK] All {total_validation} tests match expected results")
        else:
            failed_validation = total_validation - passed_validation
            print(f"\n[x] {passed_validation}/{total_validation} tests match expected results ({failed_validation} differ)")

        # Print validation failures
        failed_validations = [v for v in runner.validation_results if not v['passed']]
        if failed_validations:
            print("\n" + "-" * 80)
            print("VALIDATION DIFFERENCES:")
            print("-" * 80)
            for v in failed_validations:
                print(f"\n[x] {v['test_name']} ({v['file_name']}):")
                for diff in v['differences']:
                    print(f"    - {diff}")

        success = success and (passed_validation == total_validation)

    # Save expected results if requested
    if args.save_expected:
        runner.save_expected_results()

    return 0 if success else 1


if __name__ == '__main__':
    sys.exit(main())
