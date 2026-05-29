"""JSON manifest storage for GE shader diagnostics.

Each collection is stored as a separate JSON manifest file:
  ge_shader_diag.{label}.json

Manifest contains metadata and shader call sites.
Source text is NOT duplicated — it stays in the original .sksl files
on disk and is loaded on demand for report generation.

This avoids the redundancy and overhead of SQLite for datasets
of ~50-100 shaders (~10-20KB metadata), where indexing and
transaction guarantees provide no benefit.
"""

import json
from datetime import datetime
from pathlib import Path

from .parser import PREFIX, SKSL_EXT, parse_csv_line, scan_directory

MANIFEST_PREFIX = "ge_shader_diag"
MANIFEST_EXT = ".json"


def manifest_path(label, manifest_dir):
    """Get the file path for a collection's manifest.

    Args:
        label: Collection label.
        manifest_dir: Directory containing manifest files.

    Returns:
        Path object for the manifest file.
    """
    return Path(manifest_dir) / f"{MANIFEST_PREFIX}.{label}{MANIFEST_EXT}"


def collect_directory(directory, label, manifest_dir=".", force=False):
    """Scan a directory for diagnostic files and write a JSON manifest.

    Args:
        directory: Path to directory containing diagnostic files.
        label: Human-readable label for this collection.
        manifest_dir: Directory to store the manifest file.
        force: If True, overwrite existing manifest with same label.

    Returns:
        Number of shaders collected, or -1 on error.
    """
    mpath = manifest_path(label, manifest_dir)
    if mpath.exists():
        if force:
            mpath.unlink()
        else:
            print(f"Error: Collection '{label}' already exists. Use --force to overwrite.")
            return -1

    dir_path = Path(directory)
    if not dir_path.is_dir():
        print(f"Error: Directory '{directory}' does not exist.")
        return -1

    csv_files, sksl_files = scan_directory(directory)
    if not csv_files and not sksl_files:
        print(f"No diagnostic files found in '{directory}'.")
        return -1

    shaders = []

    # Process hashes that have CSV files (primary source of metadata)
    for hash_str in sorted(csv_files.keys()):
        csv_path = csv_files[hash_str]

        call_sites = []
        source_len = 0
        with open(csv_path, "r") as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                parsed = parse_csv_line(line)
                if parsed is None:
                    print(f"Warning: Malformed CSV line in {csv_path.name}: {line}")
                    continue
                file_val, func_val, line_val, src_len_val = parsed
                source_len = src_len_val
                call_sites.append({"file": file_val, "function": func_val, "line": line_val})

        shaders.append({
            "hash": hash_str,
            "source_len": source_len,
            "call_sites": call_sites,
        })

    # Handle hashes that only have SKSL files (no CSV — rare edge case)
    for hash_str in sorted(sksl_files.keys()):
        if hash_str in csv_files:
            continue
        sksl_path = sksl_files[hash_str]
        source_text = ""
        with open(sksl_path, "r") as f:
            source_text = f.read()
        shaders.append({
            "hash": hash_str,
            "source_len": len(source_text),
            "call_sites": [],
        })

    manifest = {
        "label": label,
        "timestamp": datetime.now().isoformat(),
        "directory": str(dir_path.resolve()),
        "shader_count": len(shaders),
        "shaders": shaders,
    }

    # Ensure manifest_dir exists
    Path(manifest_dir).mkdir(parents=True, exist_ok=True)

    with open(mpath, "w") as f:
        json.dump(manifest, f, indent=2, ensure_ascii=False)

    print(f"Collected {len(shaders)} shaders from '{directory}' as '{label}'.")
    return len(shaders)


def load_manifest(label, manifest_dir="."):
    """Load a JSON manifest for a collection.

    Args:
        label: Collection label.
        manifest_dir: Directory containing manifest files.

    Returns:
        Dict with manifest data, or None if not found.
    """
    mpath = manifest_path(label, manifest_dir)
    if not mpath.exists():
        return None
    with open(mpath, "r") as f:
        return json.load(f)


def load_source_text(hash_str, directory):
    """Load shader source text from a .sksl file on disk.

    Args:
        hash_str: SHA256 hash of the shader.
        directory: Directory containing the .sksl files.

    Returns:
        Source text string, or None if file not found.
    """
    sksl_path = Path(directory) / f"{PREFIX}.{hash_str}{SKSL_EXT}"
    if not sksl_path.exists():
        return None
    with open(sksl_path, "r") as f:
        return f.read()


def build_report_data(label, manifest_dir=".", source_dir=None):
    """Build structured report data for a collection.

    Loads metadata from the manifest and source text from .sksl files
    on demand (no duplication).

    Args:
        label: Collection label.
        manifest_dir: Directory containing manifest files.
        source_dir: Override directory for .sksl files (default: use
            manifest's 'directory' field). Useful when diagnostic files
            have been moved or pulled from a device to a different path.

    Returns:
        Dict with collection metadata and shader details, or None if
        manifest not found.
    """
    manifest = load_manifest(label, manifest_dir)
    if manifest is None:
        return None

    # Use source_dir override if provided, otherwise use manifest's directory
    sksl_dir = source_dir if source_dir is not None else manifest["directory"]

    report_data = {
        "collection": {
            "label": manifest["label"],
            "timestamp": manifest["timestamp"],
            "directory": manifest["directory"],
            "shader_count": manifest["shader_count"],
        },
        "shaders": [],
    }

    for shader in manifest["shaders"]:
        source = load_source_text(shader["hash"], sksl_dir)
        shader_info = {
            "hash": shader["hash"],
            "source_length": shader["source_len"],
            "source_text": source if source is not None else "",
            "call_sites": shader["call_sites"],
        }
        report_data["shaders"].append(shader_info)

    return report_data


def build_diff_data(left_label, right_label, manifest_dir=".", source_dir=None):
    """Build structured diff data between two collections.

    Source text is loaded on demand from .sksl files for added/removed/changed
    shaders, enabling HTML diff reports to show expandable source code.
    Source integrity check is unnecessary because hash = SHA256(source):
    if hashes match, sources must match by definition.

    Args:
        left_label: Left (baseline) collection label.
        right_label: Right (new) collection label.
        manifest_dir: Directory containing manifest files.
        source_dir: Override directory for .sksl files. If provided, both
            left and right source text are loaded from this directory.
            If None, left source is loaded from left manifest's directory
            and right source from right manifest's directory.

    Returns:
        Dict with comparison results, or error string if manifest not found.
    """
    left = load_manifest(left_label, manifest_dir)
    right = load_manifest(right_label, manifest_dir)

    if left is None:
        return f"Error: Collection '{left_label}' not found."
    if right is None:
        return f"Error: Collection '{right_label}' not found."

    # Determine source directories
    left_sksl_dir = source_dir if source_dir is not None else left["directory"]
    right_sksl_dir = source_dir if source_dir is not None else right["directory"]

    left_shaders = {s["hash"]: s for s in left["shaders"]}
    right_shaders = {s["hash"]: s for s in right["shaders"]}

    left_hashes = set(left_shaders.keys())
    right_hashes = set(right_shaders.keys())

    added_hashes = sorted(right_hashes - left_hashes)
    removed_hashes = sorted(left_hashes - right_hashes)
    common_hashes = sorted(left_hashes & right_hashes)

    # Check for changed call sites among common shaders
    changed_sites_hashes = []
    for hash_str in common_hashes:
        left_sites = left_shaders[hash_str]["call_sites"]
        right_sites = right_shaders[hash_str]["call_sites"]
        if left_sites != right_sites:
            changed_sites_hashes.append(hash_str)

    diff_data = {
        "left": {
            "label": left["label"],
            "timestamp": left["timestamp"],
            "shader_count": left["shader_count"],
        },
        "right": {
            "label": right["label"],
            "timestamp": right["timestamp"],
            "shader_count": right["shader_count"],
        },
        "summary": {
            "added": len(added_hashes),
            "removed": len(removed_hashes),
            "common": len(common_hashes),
            "changed_call_sites": len(changed_sites_hashes),
        },
        "added": [],
        "removed": [],
        "changed_call_sites": [],
    }

    for hash_str in added_hashes:
        shader = right_shaders[hash_str]
        source = load_source_text(hash_str, right_sksl_dir)
        diff_data["added"].append({
            "hash": hash_str,
            "source_length": shader["source_len"],
            "source_text": source if source is not None else "",
            "call_sites": shader["call_sites"],
        })

    for hash_str in removed_hashes:
        shader = left_shaders[hash_str]
        source = load_source_text(hash_str, left_sksl_dir)
        diff_data["removed"].append({
            "hash": hash_str,
            "source_length": shader["source_len"],
            "source_text": source if source is not None else "",
            "call_sites": shader["call_sites"],
        })

    for hash_str in changed_sites_hashes:
        # Same hash = same source; load from left directory
        source = load_source_text(hash_str, left_sksl_dir)
        diff_data["changed_call_sites"].append({
            "hash": hash_str,
            "source_text": source if source is not None else "",
            "left_sites": left_shaders[hash_str]["call_sites"],
            "right_sites": right_shaders[hash_str]["call_sites"],
        })

    return diff_data


def list_manifests(manifest_dir="."):
    """List all manifests in the manifest directory.

    Args:
        manifest_dir: Directory containing manifest files.

    Returns:
        List of dicts with label, timestamp, directory, shader_count.
    """
    dir_path = Path(manifest_dir)
    if not dir_path.is_dir():
        return []

    manifests = []
    for f in sorted(dir_path.iterdir()):
        if not f.is_file():
            continue
        if not f.name.startswith(MANIFEST_PREFIX + ".") or not f.name.endswith(MANIFEST_EXT):
            continue
        try:
            with open(f, "r") as fh:
                data = json.load(fh)
            manifests.append({
                "label": data["label"],
                "timestamp": data["timestamp"],
                "directory": data["directory"],
                "shader_count": data["shader_count"],
            })
        except (json.JSONDecodeError, KeyError):
            continue

    return manifests