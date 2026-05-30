"""Diff generation for GE shader diagnostics.

Compares two collections and classifies shaders as:
  added       — New in right, not in left
  removed     — In left, not in right
  changed     — Same hash in both, but different call sites

No source integrity check needed: hash = SHA256(source), so
matching hashes guarantee matching source text by definition.

Supports three output formats:
  text — Human-readable plain text
  json — Structured JSON for tooling integration
  html — Self-contained HTML page with embedded CSS and color-coded sections
"""

import json

from .store import build_diff_data
from .html_template import (
    render_page,
    render_diff_summary,
    render_added_section,
    render_removed_section,
    render_changed_section,
    render_empty_diff,
)


def generate_diff(left_label, right_label, manifest_dir=".", fmt="text", source_dir=None):
    """Compare two collections and show differences in the specified format.

    Args:
        left_label: Left (baseline) collection label.
        right_label: Right (new) collection label.
        manifest_dir: Directory containing manifest files.
        fmt: Output format — 'text', 'json', or 'html'.
        source_dir: Override directory for .sksl files.

    Returns:
        Formatted diff string, or error message if collection not found.
    """
    data = build_diff_data(left_label, right_label, manifest_dir, source_dir=source_dir)
    if isinstance(data, str):
        return data

    if fmt == "json":
        return format_diff_json(data)
    elif fmt == "html":
        return format_diff_html(data)
    else:
        return format_diff_text(data)


def format_diff_text(data):
    """Format diff data as human-readable plain text."""
    left = data["left"]
    right = data["right"]
    summary = data["summary"]

    lines = [
        "GE Shader Diagnostics Diff",
        "=" * 60,
        f"Left:  {left['label']} ({left['timestamp']}, {left['shader_count']} shaders)",
        f"Right: {right['label']} ({right['timestamp']}, {right['shader_count']} shaders)",
        "=" * 60,
        f"Added:   {summary['added']} shaders",
        f"Removed: {summary['removed']} shaders",
        f"Common:  {summary['common']} shaders ({summary['changed_call_sites']} with changed call sites)",
    ]
    lines.append("")

    if data["added"]:
        lines.append(f"Added shaders ({summary['added']}):")
        lines.append("-" * 40)
        for item in data["added"]:
            lines.append(f"  [{item['hash']}] srcLen={item['source_length']}")
            for site in item["call_sites"]:
                lines.append(f"    {site['file']}:{site['function']}:{site['line']}")
        lines.append("")

    if data["removed"]:
        lines.append(f"Removed shaders ({summary['removed']}):")
        lines.append("-" * 40)
        for item in data["removed"]:
            lines.append(f"  [{item['hash']}] srcLen={item['source_length']}")
            for site in item["call_sites"]:
                lines.append(f"    {site['file']}:{site['function']}:{site['line']}")
        lines.append("")

    if data["changed_call_sites"]:
        lines.append(f"Changed call sites ({summary['changed_call_sites']}):")
        lines.append("-" * 40)
        for item in data["changed_call_sites"]:
            lines.append(f"  [{item['hash']}]")
            lines.append("    Left:")
            for site in item["left_sites"]:
                lines.append(f"      {site['file']}:{site['function']}:{site['line']}")
            lines.append("    Right:")
            for site in item["right_sites"]:
                lines.append(f"      {site['file']}:{site['function']}:{site['line']}")
        lines.append("")

    if not data["added"] and not data["removed"] and not data["changed_call_sites"]:
        lines.append("No differences found. Both collections are identical.")

    return "\n".join(lines)


def format_diff_json(data):
    """Format diff data as JSON."""
    return json.dumps(data, indent=2, ensure_ascii=False)


def format_diff_html(data):
    """Format diff data as a self-contained HTML page with color-coded sections."""
    left = data["left"]
    right = data["right"]

    body_parts = [
        f'<h1>GE Shader Diagnostics Diff</h1>',
        f'<div class="meta">'
        f'Left: <strong>{left["label"]}</strong> ({left["timestamp"]}, {left["shader_count"]} shaders) &middot; '
        f'Right: <strong>{right["label"]}</strong> ({right["timestamp"]}, {right["shader_count"]} shaders)'
        f'</div>',
        render_diff_summary(data["summary"]),
    ]

    has_changes = data["added"] or data["removed"] or data["changed_call_sites"]

    if has_changes:
        body_parts.append(render_added_section(data["added"]))
        body_parts.append(render_removed_section(data["removed"]))
        body_parts.append(render_changed_section(data["changed_call_sites"]))
    else:
        body_parts.append(render_empty_diff())

    return render_page("GE Shader Diagnostics Diff", "\n".join(body_parts))