"""Report generation for GE shader diagnostics.

Supports three output formats:
  text — Human-readable plain text
  json — Structured JSON for tooling integration
  html — Self-contained HTML page with embedded CSS

Source text is loaded on demand from .sksl files (no duplication).
If .sksl files are unavailable, source_text is empty in the output.
"""

import json

from .store import build_report_data
from .html_template import render_page, render_summary_cards, render_shader_table


def generate_report(label, manifest_dir=".", fmt="text", source_dir=None):
    """Generate a report for a collection in the specified format.

    Args:
        label: Collection label to report on.
        manifest_dir: Directory containing manifest files.
        fmt: Output format — 'text', 'json', or 'html'.
        source_dir: Override directory for .sksl files.

    Returns:
        Formatted report string, or error message if collection not found.
    """
    data = build_report_data(label, manifest_dir, source_dir=source_dir)
    if data is None:
        return f"Error: Collection '{label}' not found."

    if fmt == "json":
        return format_report_json(data)
    elif fmt == "html":
        return format_report_html(data)
    else:
        return format_report_text(data)


def format_report_text(data):
    """Format report data as human-readable plain text."""
    coll = data["collection"]
    lines = [
        "GE Shader Diagnostics Report",
        "=" * 60,
        f"Collection:  {coll['label']}",
        f"Timestamp:   {coll['timestamp']}",
        f"Directory:   {coll['directory']}",
        f"Shaders:     {coll['shader_count']}",
        "=" * 60,
    ]

    total_sites = sum(len(s["call_sites"]) for s in data["shaders"])
    src_lens = [s["source_length"] for s in data["shaders"] if s["source_length"] > 0]
    lines.append(f"Total call sites: {total_sites}")
    if src_lens:
        lines.append(
            f"Source length: min={min(src_lens)}, max={max(src_lens)}, "
            f"avg={sum(src_lens) // len(src_lens)}"
        )
    lines.append("")

    for shader in data["shaders"]:
        lines.append(f"  [{shader['hash']}] srcLen={shader['source_length']}")
        for site in shader["call_sites"]:
            lines.append(f"    {site['file']}:{site['function']}:{site['line']}")
        if shader["source_text"]:
            src_lines = shader["source_text"].split("\n")
            for sl in src_lines[:3]:
                lines.append(f"    | {sl}")
            if len(src_lines) > 3:
                lines.append(f"    | ... ({len(src_lines)} lines total)")
        lines.append("")

    return "\n".join(lines)


def format_report_json(data):
    """Format report data as JSON."""
    return json.dumps(data, indent=2, ensure_ascii=False)


def format_report_html(data):
    """Format report data as a self-contained HTML page."""
    coll = data["collection"]

    total_sites = sum(len(s["call_sites"]) for s in data["shaders"])
    src_lens = [s["source_length"] for s in data["shaders"] if s["source_length"] > 0]
    cards = [
        {"label": "Shaders", "value": coll["shader_count"]},
        {"label": "Call Sites", "value": total_sites},
    ]
    if src_lens:
        cards.append({"label": "Min Source Length", "value": min(src_lens)})
        cards.append({"label": "Max Source Length", "value": max(src_lens)})
        cards.append({"label": "Avg Source Length", "value": sum(src_lens) // len(src_lens)})

    body_parts = [
        f'<h1>GE Shader Diagnostics Report</h1>',
        f'<div class="meta">'
        f'Collection: <strong>{coll["label"]}</strong> &middot; '
        f'Timestamp: {coll["timestamp"]} &middot; '
        f'Directory: {coll["directory"]}'
        f'</div>',
        render_summary_cards(cards),
        f'<h2>Shader Details</h2>',
        render_shader_table(data["shaders"]),
    ]

    return render_page("GE Shader Diagnostics Report", "\n".join(body_parts))