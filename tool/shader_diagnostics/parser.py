"""File parsing utilities for GE shader diagnostic files.

Diagnostic file naming convention:
  ge_shader_diagnostics.{sha256_hash}.csv  — file,function,line,srcLen
  ge_shader_diagnostics.{sha256_hash}.sksl — shader source code

CSV format follows RFC 4180 (quoted fields for commas, newlines, double-quotes),
matching the C++ FormatCsvField implementation.
"""

import csv
import io
from pathlib import Path

PREFIX = "ge_shader_diagnostics"
CSV_EXT = ".csv"
SKSL_EXT = ".sksl"
HASH_LEN = 64  # SHA256 hex digest length


def parse_hash_from_filename(filename):
    """Extract SHA256 hash from a diagnostic filename.

    Expected format: ge_shader_diagnostics.{64-char-hex}.{csv|sksl}

    Args:
        filename: The filename string (not full path).

    Returns:
        The 64-character hex hash string if valid, None otherwise.
    """
    if not filename.startswith(PREFIX + "."):
        return None
    rest = filename[len(PREFIX) + 1:]
    dot_pos = rest.rfind(".")
    if dot_pos < 0:
        return None
    hash_str = rest[:dot_pos]
    ext = rest[dot_pos:]
    if ext not in (CSV_EXT, SKSL_EXT):
        return None
    if len(hash_str) != HASH_LEN:
        return None
    try:
        int(hash_str, 16)
    except ValueError:
        return None
    return hash_str


def parse_csv_line(line):
    """Parse a CSV line in format: file,function,line,srcLen.

    Handles RFC 4180 quoted fields (double-quote escaping for commas,
    newlines, and embedded double-quotes), matching the C++ FormatCsvField
    implementation on the producer side.

    Args:
        line: A single CSV line string.

    Returns:
        Tuple of (file, function, line_int, src_len_int) if valid,
        None on parse failure.
    """
    reader = csv.reader(io.StringIO(line))
    try:
        row = next(reader)
    except csv.Error:
        return None
    if len(row) < 4:
        return None
    try:
        line_int = int(row[2])
        src_len_int = int(row[3])
    except ValueError:
        return None
    return (row[0], row[1], line_int, src_len_int)


def scan_directory(directory):
    """Scan a directory for diagnostic files and group by hash.

    Args:
        directory: Path to the directory containing diagnostic files.

    Returns:
        Tuple of (csv_files, sksl_files) where each is a dict mapping
        hash string to Path object. Returns (empty dicts, empty dicts)
        if directory is invalid or contains no diagnostic files.
    """
    dir_path = Path(directory)
    if not dir_path.is_dir():
        return {}, {}

    csv_files = {}
    sksl_files = {}

    for f in sorted(dir_path.iterdir()):
        if not f.is_file():
            continue
        hash_str = parse_hash_from_filename(f.name)
        if hash_str is None:
            continue
        if f.name.endswith(CSV_EXT):
            csv_files[hash_str] = f
        elif f.name.endswith(SKSL_EXT):
            sksl_files[hash_str] = f

    return csv_files, sksl_files