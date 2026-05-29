"""CLI entry point for GE Shader Diagnostics Post-Processing Tool.

Usage:
  python3 -m shader_diagnostics collect <dir> --label <name>
  python3 -m shader_diagnostics report --label <name> [--format text|json|html]
  python3 -m shader_diagnostics diff --left <L> --right <R> [--format text|json|html]
  python3 -m shader_diagnostics list

Diagnostic files are at /data/service/el0/render_service/ on the device.
Pull them off with: hdc file recv /data/service/el0/render_service/ ./local_dir/

Storage: per-collection JSON manifests (ge_shader_diag.{label}.json).
No SQLite, no source text duplication — .sksl files are referenced on demand.
"""

import argparse
import sys

from .store import collect_directory, list_manifests
from .report import generate_report
from .diff import generate_diff


def main():
    parser = argparse.ArgumentParser(
        description="GE Shader Diagnostics Post-Processing Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  collect   Scan diagnostic files directory, write JSON manifest
  report    Generate human-readable or JSON/HTML report from manifest
  diff      Compare two collections, show added/removed/changed shaders
  list      List all collections (manifests) in manifest directory

Diagnostic files are at /data/service/el0/render_service/ on the device.
Pull them off with: hdc file recv /data/service/el0/render_service/ ./local_dir/

Storage: per-collection JSON manifests (ge_shader_diag.{label}.json).
No source text duplication — .sksl files are referenced on demand.

Examples:
  # Collect from pulled device files
  %(prog)s collect ./diag_files --label baseline

  # Collect from a second run
  %(prog)s collect ./diag_files_v2 --label after-refactor

  # Generate text report
  %(prog)s report --label baseline

  # Generate HTML report (source from a different directory)
  %(prog)s report --label baseline --format html --source-dir ./diag_files --output report.html

  # Compare two collections
  %(prog)s diff --left baseline --right after-refactor

  # HTML diff with color-coded sections
  %(prog)s diff --left baseline --right after-refactor --format html --output diff.html

  # JSON output for tooling integration
  %(prog)s diff --left baseline --right after-refactor --format json
""",
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    # collect command
    collect_p = subparsers.add_parser("collect", help="Scan directory and write JSON manifest")
    collect_p.add_argument("directory", help="Directory containing diagnostic files")
    collect_p.add_argument("--manifest-dir", default=".", help="Directory to store manifest files (default: current directory)")
    collect_p.add_argument("--label", required=True, help="Label for this collection (e.g., 'baseline', 'after-refactor')")
    collect_p.add_argument("--force", action="store_true", help="Overwrite existing manifest with same label")

    # report command
    report_p = subparsers.add_parser("report", help="Generate report from manifest")
    report_p.add_argument("--manifest-dir", default=".", help="Directory containing manifest files")
    report_p.add_argument("--label", required=True, help="Collection label to report on")
    report_p.add_argument("--source-dir", default=None, help="Override directory for .sksl files (default: use manifest's directory)")
    report_p.add_argument("--format", choices=["text", "json", "html"], default="text", help="Output format (default: text)")
    report_p.add_argument("--output", default=None, help="Output file path (default: stdout)")

    # diff command
    diff_p = subparsers.add_parser("diff", help="Compare two collections")
    diff_p.add_argument("--manifest-dir", default=".", help="Directory containing manifest files")
    diff_p.add_argument("--left", required=True, help="Left (baseline) collection label")
    diff_p.add_argument("--right", required=True, help="Right (new) collection label")
    diff_p.add_argument("--source-dir", default=None, help="Override directory for .sksl files (default: use each manifest's directory)")
    diff_p.add_argument("--format", choices=["text", "json", "html"], default="text", help="Output format (default: text)")
    diff_p.add_argument("--output", default=None, help="Output file path (default: stdout)")

    # list command
    list_p = subparsers.add_parser("list", help="List all collections in manifest directory")
    list_p.add_argument("--manifest-dir", default=".", help="Directory containing manifest files")

    args = parser.parse_args()

    if args.command == "collect":
        result = collect_directory(args.directory, args.label, args.manifest_dir, args.force)
        if result < 0:
            sys.exit(1)

    elif args.command == "report":
        output = generate_report(args.label, args.manifest_dir, args.format, args.source_dir)
        write_output(output, args.output)

    elif args.command == "diff":
        output = generate_diff(args.left, args.right, args.manifest_dir, args.format, args.source_dir)
        write_output(output, args.output)

    elif args.command == "list":
        manifests = list_manifests(args.manifest_dir)
        if not manifests:
            print("No collections found in manifest directory.")
        else:
            print("Collections in manifest directory:")
            print("=" * 60)
            for m in manifests:
                print(f"  {m['label']}")
                print(f"    Timestamp: {m['timestamp']}")
                print(f"    Directory: {m['directory']}")
                print(f"    Shaders:   {m['shader_count']}")


def write_output(content, output_path):
    """Write content to file or stdout.

    Args:
        content: String content to write.
        output_path: File path, or None for stdout.
    """
    if output_path:
        with open(output_path, "w") as f:
            f.write(content)
        print(f"Output written to {output_path}")
    else:
        print(content)


if __name__ == "__main__":
    main()