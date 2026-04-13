#!/usr/bin/env python3
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
Parser robustness test runner.

This script runs the parser on various valid and invalid test cases
and checks if it handles them correctly with appropriate error messages.
"""

import sys
from pathlib import Path
from typing import Dict, List, Any, Optional
import traceback
import json
from dataclasses import dataclass, asdict, field, fields
from enum import Enum

script_dir = Path(__file__).parent
root_dir = script_dir.parent.parent.parent
sys.path.insert(0, str(root_dir))

from tool.effectgen.cpp_tokenizer import CppTokenizer
from tool.effectgen.cpp_parser import CppParser, FieldInfo, StructInfo


class TestStatus(Enum):
    """Test result status."""
    PARSED_ONLY = "PARSED_ONLY"
    TEST_PASSED = "TEST_PASSED"
    TEST_FAILED = "TEST_FAILED"


@dataclass
class PropAttributeExpected:
    """Expected result for a single prop attribute."""
    name: str
    array_accessor_length: Optional[int] = None
    array_accessor_type: Optional[str] = None
    alias: Optional[str] = None
    cast_from: Optional[str] = None
    custom: Optional[str] = None
    min_value: Optional[str] = None
    max_value: Optional[str] = None


@dataclass
class FieldExpectedResult:
    """Expected result for a parsed field."""
    type: str
    name: str
    default_value: str = ""
    prop_name: str = ""
    array_accessor_length: Optional[int] = None
    prop_attributes: List[PropAttributeExpected] = field(default_factory=list)


@dataclass
class StructExpectedResult:
    """Expected result for a parsed struct."""
    name: str
    enum_type: str
    filter_name: str
    fields: List[FieldExpectedResult] = field(default_factory=list)
    params: Dict[str, Any] = field(default_factory=dict)


@dataclass
class TestCaseExpectedResult:
    """Expected result for a test case."""
    name: str
    should_parse: bool
    expected_error_count: int = 0
    expected_errors: List[str] = field(default_factory=list)
    structs: List[StructExpectedResult] = field(default_factory=list)
    notes: str = ""


@dataclass
class TestCase:
    """Represents a test case with expected outcome."""
    name: str
    file_path: Path
    should_parse: bool
    result: Any = None
    results: List[Any] = field(default_factory=list)
    parser_errors: List[Any] = field(default_factory=list)
    error: Optional[str] = None
    passed: bool = False
    validation_passed: Optional[bool] = None
    validation_differences: List[str] = field(default_factory=list)
    status: TestStatus = TestStatus.PARSED_ONLY


class ValidationContext:
    """Context for validation operations."""

    def __init__(self, struct_idx: int = -1, field_idx: int = -1, prop_idx: int = -1):
        self.struct_idx = struct_idx
        self.field_idx = field_idx
        self.prop_idx = prop_idx

    def __str__(self):
        parts = []
        if self.struct_idx >= 0:
            parts.append(f"Struct {self.struct_idx + 1}")
        if self.field_idx >= 0:
            parts.append(f"field {self.field_idx + 1}")
        if self.prop_idx >= 0:
            parts.append(f"prop_attribute {self.prop_idx + 1}")
        return ", ".join(parts) if parts else ""


class Validator:
    """Handles validation of test results against expected results."""

    def __init__(self, test_dir: Path):
        self.test_dir = test_dir
        self._prop_attribute_validators = self._get_dataclass_fields(PropAttributeExpected)
        self._field_validators = self._get_dataclass_fields(FieldExpectedResult)
        self._struct_validators = self._get_dataclass_fields(StructExpectedResult)

    @staticmethod
    def _get_dataclass_fields(cls) -> List[str]:
        exclude_fields = {
            PropAttributeExpected: [],
            FieldExpectedResult: ['prop_attributes', 'array_accessor_length'],
            FieldInfo: ['prop_attributes', 'attributes'],
            StructExpectedResult: ['fields', 'params'],
            StructInfo: ['fields', 'errors', 'params'],
        }
        excluded = exclude_fields.get(cls, [])
        return [f.name for f in fields(cls) if f.name not in excluded]

    def _format_error_path(self, error_str: str) -> str:
        line_idx = error_str.find(':line')
        if line_idx > 0:
            abs_path = error_str[:line_idx]
            rest = error_str[line_idx:]
            try:
                rel_path = str(Path(abs_path).relative_to(self.test_dir))
                rel_path = rel_path.replace('\\', '/')
                return rel_path + rest
            except (ValueError, TypeError):
                pass
        return error_str

    def _add_difference(self, validation: Dict[str, Any], message: str):
        validation['passed'] = False
        validation['differences'].append(message)

    def _validate_with_validators(self, validation: Dict[str, Any], context: ValidationContext,
                                  validators: List[str], expected: Any, actual: Any):
        for attr_name in validators:
            exp_value = getattr(expected, attr_name)
            act_value = getattr(actual, attr_name)

            if attr_name in ('default_value', 'prop_name'):
                exp_value = exp_value or ''
                act_value = act_value or ''

            if exp_value != act_value:
                self._add_difference(
                    validation,
                    f"{context} {attr_name} mismatch: expected '{exp_value}', got '{act_value}'"
                )

    def _validate_prop_attribute(self, validation: Dict[str, Any], context: ValidationContext,
                                 exp_prop: PropAttributeExpected, act_prop: Any):
        """Validate a single prop attribute."""
        self._validate_with_validators(validation, context, self._prop_attribute_validators, exp_prop, act_prop)

    def _validate_field(self, validation: Dict[str, Any], context: ValidationContext,
                       exp_field: FieldExpectedResult, act_field: Any):
        """Validate a single field."""
        self._validate_with_validators(validation, context, self._field_validators, exp_field, act_field)

        exp_props = exp_field.prop_attributes
        act_props = act_field.prop_attributes
        if len(exp_props) != len(act_props):
            self._add_difference(
                validation,
                f"{context} prop_attributes count mismatch: expected {len(exp_props)}, got {len(act_props)}"
            )

        for k, (exp_prop, act_prop) in enumerate(zip(exp_props, act_props)):
            prop_context = ValidationContext(context.struct_idx, context.field_idx, k)
            self._validate_prop_attribute(validation, prop_context, exp_prop, act_prop)

    def _validate_params(self, validation: Dict[str, Any], context: ValidationContext,
                        exp_params: Dict, act_params: Dict):
        """Validate params dictionary."""
        if exp_params != act_params:
            self._add_difference(validation, f"{context} params mismatch")
            missing_keys = set(exp_params.keys()) - set(act_params.keys())
            extra_keys = set(act_params.keys()) - set(exp_params.keys())
            diff_keys = set(k for k in exp_params.keys() & act_params.keys() if exp_params[k] != act_params[k])

            if missing_keys:
                self._add_difference(validation, f"{context} params missing keys: {sorted(missing_keys)}")
            if extra_keys:
                self._add_difference(validation, f"{context} params extra keys: {sorted(extra_keys)}")
            for key in sorted(diff_keys):
                self._add_difference(
                    validation,
                    f"{context} params['{key}'] mismatch: expected '{exp_params[key]}', got '{act_params[key]}'"
                )

    def _validate_struct(self, validation: Dict[str, Any], context: ValidationContext,
                        exp_struct: StructExpectedResult, act_struct: Any):
        """Validate a single struct."""
        self._validate_with_validators(validation, context, self._struct_validators, exp_struct, act_struct)

        exp_params = exp_struct.params or {}
        act_params = act_struct.params or {}
        self._validate_params(validation, context, exp_params, act_params)

        exp_fields = exp_struct.fields
        act_fields = act_struct.fields
        if len(exp_fields) != len(act_fields):
            self._add_difference(
                validation,
                f"{context} field count mismatch: expected {len(exp_fields)}, got {len(act_fields)}"
            )

        for j, (exp_field, act_field) in enumerate(zip(exp_fields, act_fields)):
            field_context = ValidationContext(context.struct_idx, j)
            self._validate_field(validation, field_context, exp_field, act_field)

    def validate_test_case(self, test_case: TestCase, expected: TestCaseExpectedResult) -> Dict[str, Any]:
        """Validate a single test case against expected results."""
        validation = {
            'test_name': test_case.file_path.stem,
            'file_name': test_case.file_path.name,
            'passed': True,
            'differences': []
        }

        actual_error_count = len(test_case.parser_errors)
        if actual_error_count != expected.expected_error_count:
            self._add_difference(
                validation,
                f"Error count mismatch: expected {expected.expected_error_count}, got {actual_error_count}"
            )

        actual_errors = [self._format_error_path(str(e)) for e in test_case.parser_errors]
        expected_errors = expected.expected_errors or []

        if actual_errors != expected_errors:
            missing_errors = set(expected_errors) - set(actual_errors)
            extra_errors = set(actual_errors) - set(expected_errors)

            if missing_errors:
                self._add_difference(
                    validation,
                    f"Missing expected errors: {sorted(missing_errors)}"
                )
            if extra_errors:
                self._add_difference(
                    validation,
                    f"Unexpected errors: {sorted(extra_errors)}"
                )

        if test_case.should_parse and test_case.results:
            expected_structs = expected.structs
            actual_structs = test_case.results

            if len(expected_structs) != len(actual_structs):
                self._add_difference(
                    validation,
                    f"Struct count mismatch: expected {len(expected_structs)}, got {len(actual_structs)}"
                )

            for i, (exp_struct, act_struct) in enumerate(zip(expected_structs, actual_structs)):
                context = ValidationContext(i)
                self._validate_struct(validation, context, exp_struct, act_struct)

        return validation

    def validate_all(self, test_cases: List[TestCase],
                     expected_results: Dict[str, TestCaseExpectedResult]) -> List[Dict[str, Any]]:
        """Validate all test cases against expected results."""
        validation_results = []

        for test_case in test_cases:
            test_name = test_case.file_path.stem

            if test_name not in expected_results:
                validation = {
                    'test_name': test_name,
                    'file_name': test_case.file_path.name,
                    'passed': False,
                    'differences': ["No expected result found for this test"]
                }
                validation_results.append(validation)
                continue

            expected = expected_results[test_name]
            validation = self.validate_test_case(test_case, expected)
            validation_results.append(validation)

            test_case.validation_passed = validation['passed']
            test_case.validation_differences = validation['differences']

        return validation_results


class ResultFormatter:
    """Handles formatting and display of test results."""

    def __init__(self, show_all_details: bool = False):
        self.show_all_details = show_all_details

    def _format_test_summary_line(self, test: TestCase, status: TestStatus) -> str:
        """Format a single test summary line."""
        if test.result:
            struct_count = len(test.results)
            fields = len(test.result.fields)
            if struct_count > 1:
                return f"[{status.value}] {test.name} ({test.file_path.name}) - {struct_count} structs, first has {fields} fields"
            else:
                return f"[{status.value}] {test.name} ({test.file_path.name}) - {fields} fields"
        else:
            return f"[{status.value}] {test.name} ({test.file_path.name}) - handled gracefully"

    def _format_field_details(self, field: FieldInfo) -> List[str]:
        """Format details of a single field."""
        lines = []
        default = f" = {field.default_value}" if field.default_value else ""
        # prop = f" [prop: {field.prop_name}]" if field.prop_name else ""
        props_info = ""
        if field.prop_attributes:
            props_list = []
            for p in field.prop_attributes:
                prop_str = p.name
                if prop_str is None:
                    prop_str = "<unspecified name>"
                if p.array_accessor_length is not None:
                    prop_str += f"[{p.array_accessor_length}]"
                if p.alias:
                    prop_str += f" (alias: {p.alias})"
                props_list.append(prop_str)
            if len(props_list) > 0:
                props_info = f" [props: {', '.join(props_list)}]"
        lines.append(f"    - type: '{field.type}'")
        lines.append(f"      name: '{field.name}'{props_info}")
        if default:
            lines.append(f"      default:{default}")
        if not field.name or field.name == field.type:
            lines.append(f"      ⚠ WARNING: Field name appears to be missing or incorrect!")
        if not field.type:
            lines.append(f"      ⚠ WARNING: Field type is empty!")
        return lines

    def _format_struct_details(self, test: TestCase, struct, struct_idx: int, total_structs: int) -> List[str]:
        """Format details of a single struct."""
        lines = []
        if total_structs > 1:
            lines.append(f"\n{test.name} ({test.file_path.name}) - Struct {struct_idx+1}/{total_structs}:")
        else:
            lines.append(f"\n{test.name} ({test.file_path.name}):")
        lines.append(f"  Struct: {struct.name}")
        lines.append(f"  Enum Type: {struct.enum_type}")
        lines.append(f"  Filter Name: {struct.filter_name}")
        if struct.params:
            lines.append(f"  Params ({len(struct.params)}):")
            for key, value in sorted(struct.params.items()):
                lines.append(f"    - {key}: {value}")
        lines.append(f"  Fields ({len(struct.fields)}):")
        for field in struct.fields:
            lines.extend(self._format_field_details(field))
        return lines

    def print_summary(self, test_cases: List[TestCase]):
        """Print test results summary."""
        total = len(test_cases)
        parsed_only = sum(1 for t in test_cases if t.status == TestStatus.PARSED_ONLY)
        test_passed = sum(1 for t in test_cases if t.status == TestStatus.TEST_PASSED)
        test_failed = sum(1 for t in test_cases if t.status == TestStatus.TEST_FAILED)

        print("\n" + "=" * 80)
        print("TEST RESULTS")
        print("=" * 80)

        print(f"\nTotal: {total} | TEST PASSED: {test_passed} | PARSED ONLY: {parsed_only} | TEST FAILED: {test_failed}")

        if parsed_only > 0:
            print(f"\n⚠ Warning: {parsed_only} test(s) have no expected results (PARSED ONLY)")
            print("  Run with --save-expected to generate expected results files")

    def print_test_details(self, test_cases: List[TestCase]):
        """Print detailed test results."""
        parsed_only_tests = [t for t in test_cases if t.status == TestStatus.PARSED_ONLY]
        test_passed_tests = [t for t in test_cases if t.status == TestStatus.TEST_PASSED]
        test_failed_tests = [t for t in test_cases if t.status == TestStatus.TEST_FAILED]

        if test_failed_tests:
            print("\n" + "-" "-" * 79)
            print("TEST FAILED:")
            print("-" * 80)
            for test in test_failed_tests:
                print(f"\n[x] {test.name}:")
                print(f"    File: {test.file_path.name}")
                if test.error:
                    print(f"    Error: {test.error}")
                for diff in test.validation_differences:
                    print(f"    - {diff}")

        if parsed_only_tests:
            print("\n" + "-" * 80)
            print("PARSED ONLY (no expected results):")
            print("-" * 80)
            for test in parsed_only_tests:
                print(self._format_test_summary_line(test, TestStatus.PARSED_ONLY))

        if test_passed_tests and self.show_all_details:
            print("\n" + "-" * 80)
            print("TEST PASSED:")
            print("-" * 80)
            for test in test_passed_tests:
                print(self._format_test_summary_line(test, TestStatus.TEST_PASSED))

        if self.show_all_details:
            self._print_detailed_results(test_cases)

    def _print_detailed_results(self, test_cases: List[TestCase]):
        """Print detailed results for all tests."""
        print("\n" + "=" * 80)
        print("DETAILED RESULTS:")
        print("=" * 80)

        for test in test_cases:
            if test.should_parse and test.passed and test.results:
                for i, struct in enumerate(test.results):
                    lines = self._format_struct_details(test, struct, i, len(test.results))
                    for line in lines:
                        print(line)

            elif not test.should_parse:
                print(f"\n{test.name} ({test.file_path.name}):")

                if test.results:
                    print(f"  Parsed {len(test.results)} struct(s) despite being invalid test:")
                    for struct in test.results:
                        print(f"    - {struct.name} ({len(struct.fields)} fields)")

                if test.parser_errors:
                    print(f"  Parser errors/warnings ({len(test.parser_errors)}):")
                    for error in test.parser_errors:
                        print(f"    - {error}")

                if test.error:
                    print(f"  Test error: {test.error}")

                if not test.results and not test.parser_errors and not test.error:
                    print(f"  No structs parsed, no errors reported")


class TestRunner:
    """Runs parser robustness tests."""

    def __init__(self, test_dir: Path):
        self.test_dir = test_dir
        self.valid_dir = test_dir / "valid"
        self.invalid_dir = test_dir / "invalid"
        self.test_cases: List[TestCase] = []
        self.expected_results: Dict[str, TestCaseExpectedResult] = {}
        self.validator = Validator(test_dir)

    @staticmethod
    def _parse_prop_attribute(prop_data: Dict[str, Any]) -> PropAttributeExpected:
        """Parse a prop attribute from JSON data."""
        return PropAttributeExpected(
            name=prop_data.get('name', ''),
            array_accessor_length=prop_data.get('array_accessor_length'),
            array_accessor_type=prop_data.get('array_accessor_type'),
            alias=prop_data.get('alias'),
            cast_from=prop_data.get('cast_from'),
            custom=prop_data.get('custom'),
            min_value=prop_data.get('min_value'),
            max_value=prop_data.get('max_value')
        )

    def _parse_field_from_json(self, field_data: Dict[str, Any]) -> FieldExpectedResult:
        """Parse a field from JSON data."""
        return FieldExpectedResult(
            type=field_data['type'],
            name=field_data['name'],
            default_value=field_data.get('default_value', ''),
            prop_name=field_data.get('prop_name', ''),
            array_accessor_length=field_data.get('array_accessor_length'),
            prop_attributes=[self._parse_prop_attribute(pa) for pa in field_data.get('prop_attributes', [])]
        )

    def _parse_struct_from_json(self, struct_data: Dict[str, Any]) -> StructExpectedResult:
        """Parse a struct from JSON data."""
        return StructExpectedResult(
            name=struct_data['name'],
            enum_type=struct_data['enum_type'],
            filter_name=struct_data['filter_name'],
            fields=[self._parse_field_from_json(f) for f in struct_data.get('fields', [])],
            params=struct_data.get('params', {})
        )

    def load_expected_results(self) -> bool:
        """Load expected results from individual JSON files alongside test files."""
        loaded_count = 0
        for test_case in self.test_cases:
            expected_file = test_case.file_path.with_suffix('.params.json')

            if not expected_file.exists():
                continue

            try:
                with open(expected_file, 'r', encoding='utf-8') as f:
                    result_data = json.load(f)

                structs = [self._parse_struct_from_json(s) for s in result_data.get('structs', [])]

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

        return loaded_count > 0

    def save_expected_results(self):
        """Save current test results as expected results to individual JSON files."""
        saved_count = 0
        for test_case in self.test_cases:
            test_name = test_case.file_path.stem

            structs_data = []
            for struct in test_case.results:
                struct_data = StructExpectedResult(
                    name=struct.name,
                    enum_type=struct.enum_type,
                    filter_name=struct.filter_name,
                    fields=[FieldExpectedResult(
                        type=field.type,
                        name=field.name,
                        default_value=field.default_value or "",
                        prop_name=field.prop_name or "",
                        prop_attributes=[
                            PropAttributeExpected(
                                name=pa.name,
                                array_accessor_length=pa.array_accessor_length,
                                array_accessor_type=pa.array_accessor_type,
                                alias=pa.alias,
                                cast_from=pa.cast_from,
                                custom=pa.custom,
                                min_value=pa.min_value,
                                max_value=pa.max_value
                            ) for pa in field.prop_attributes
                        ]
                    ) for field in struct.fields],
                    params=struct.params
                )
                structs_data.append(asdict(struct_data))

            error_strings = [self.validator._format_error_path(str(error)) for error in test_case.parser_errors]

            expected_result = TestCaseExpectedResult(
                name=test_name,
                should_parse=test_case.should_parse,
                expected_error_count=len(test_case.parser_errors),
                expected_errors=error_strings,
                structs=structs_data,
                notes=""
            )

            # Save to a .params.json file next to the test file
            output_file = test_case.file_path.with_suffix('.params.json')
            with open(output_file, 'w', encoding='utf-8') as f:
                json.dump(asdict(expected_result), f, indent=2, ensure_ascii=False)

            saved_count += 1

        print(f"\nSaved {saved_count} expected results files alongside test files")
        print(f"Pattern: <test_file>.params.json")

    def discover_tests(self):
        """Discover all test cases."""
        valid_files = sorted(self.valid_dir.glob("*.params"))
        for file_path in valid_files:
            test_case = TestCase(
                name=f"[valid] {file_path.stem}",
                file_path=file_path,
                should_parse=True
            )
            self.test_cases.append(test_case)

        invalid_files = sorted(self.invalid_dir.glob("*.params"))
        for file_path in invalid_files:
            test_case = TestCase(
                name=f"[invalid] {file_path.stem}",
                file_path=file_path,
                should_parse=False
            )
            self.test_cases.append(test_case)

    def run_test(self, test_case: TestCase) -> bool:
        """Run a single test case."""
        try:
            with open(test_case.file_path, 'r', encoding='utf-8') as f:
                content = f.read()

            tokenizer = CppTokenizer(content)
            tokens = tokenizer.tokenize()

            parser = CppParser(tokens, str(test_case.file_path))
            structs = parser.parse()

            test_case.parser_errors = parser.errors

            if structs:
                test_case.result = structs[0]
                test_case.results = structs
                parsed_successfully = True
            else:
                test_case.results = []
                parsed_successfully = False

            if test_case.should_parse:
                if parsed_successfully:
                    test_case.passed = True
                else:
                    test_case.passed = False
                    test_case.error = "Expected to parse successfully but got no structs"
            else:
                test_case.passed = True
                test_case.results = structs

            return test_case.passed

        except Exception as e:
            test_case.error = str(e)
            test_case.error_traceback = traceback.format_exc()

            if not test_case.should_parse:
                test_case.passed = True
            else:
                test_case.passed = False

            return test_case.passed

    def run_all_tests(self):
        """Run all test cases."""
        print(f"Running {len(self.test_cases)} test cases...")

        for test_case in self.test_cases:
            self.run_test(test_case)

    def determine_test_status(self, test_case: TestCase) -> TestStatus:
        """Determine the test status based on validation results."""
        if test_case.validation_passed is True:
            return TestStatus.TEST_PASSED
        elif test_case.validation_passed is False:
            return TestStatus.TEST_FAILED
        else:
            return TestStatus.PARSED_ONLY

    def validate_and_set_status(self):
        """Validate all test cases and set their status."""
        validation_results = self.validator.validate_all(self.test_cases, self.expected_results)

        for test_case in self.test_cases:
            test_case.status = self.determine_test_status(test_case)

        return validation_results


def main():
    """Main entry point."""
    import argparse

    parser = argparse.ArgumentParser(description='Parser robustness test runner - rewritten version')
    parser.add_argument('--save-expected', action='store_true',
                        help='Save current results as expected results (saves .params.json files alongside test files)')
    parser.add_argument('--show-all-details', action='store_true',
                        help='Show detailed results for all tests (including TEST PASSED)')

    args = parser.parse_args()

    test_dir = Path(__file__).parent

    runner = TestRunner(test_dir)
    runner.discover_tests()

    loaded = runner.load_expected_results()
    if not loaded:
        print("Warning: No .params.json files found. Tests will be PARSED ONLY.")
        print("Run with --save-expected to generate expected results files.")
    else:
        print(f"Loaded {len(runner.expected_results)} expected results from .params.json files")

    runner.run_all_tests()

    validation_results = runner.validate_and_set_status()

    formatter = ResultFormatter(show_all_details=args.show_all_details)

    formatter.print_summary(runner.test_cases)
    formatter.print_test_details(runner.test_cases)

    test_failed = sum(1 for t in runner.test_cases if t.status == TestStatus.TEST_FAILED)
    success = test_failed == 0

    if args.save_expected:
        runner.save_expected_results()

    return 0 if success else 1


if __name__ == '__main__':
    sys.exit(main())
