"""GLSL/SkSL source formatting and syntax highlighting for HTML output.

Formatting: inserts newlines around { } and indents block contents,
making compact shader source readable in reports.

Highlighting: produces self-contained HTML with <span> tags and CSS classes.
No external JS dependencies required.

Uses a regex-based tokenizer: each source character is classified into
a token type (comment, keyword, type, etc.) and wrapped in a <span>
with the corresponding CSS class.  Token patterns are tried in order;
first match wins, so comments are matched before keywords (preventing
inner highlighting of keywords inside comments).
"""

import re
import html

# ── Source formatting ──

def format_glsl_source(source):
    """Format GLSL/SkSL source code for display.

    Inserts newlines around { } braces and indents block contents.
    Braces inside comments (// and /* */) are left untouched.
    Preserves existing newlines — only adds where missing.

    Args:
        source: Raw shader source text string.

    Returns:
        Formatted source text string.
    """
    if not source:
        return source

    # Step 0: Protect comments — replace with placeholders so braces
    # inside comments are not reformatted.
    _COMMENT_RE = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/')
    comments = []
    def _save_comment(m):
        comments.append(m.group())
        return f'\x00C{len(comments) - 1}\x00'
    protected = _COMMENT_RE.sub(_save_comment, source)

    # Step 1: Insert newlines around braces where missing
    # Newline before { (unless already preceded by newline/whitespace-only line)
    result = re.sub(r'([^\n\s])\s*\{', r'\1\n{', protected)
    # Newline after { (unless already followed by newline)
    result = re.sub(r'\{\s*([^\n])', r'{\n\1', result)
    # Newline before } (unless already preceded by newline)
    result = re.sub(r'([^\n])\s*\}', r'\1\n}', result)
    # Newline after } (unless already followed by newline or end of string)
    result = re.sub(r'\}\s*([^\n])', r'}\n\1', result)

    # Step 2: Indent lines inside { } blocks
    lines = result.split('\n')
    indent_level = 0
    formatted_lines = []
    for line in lines:
        stripped = line.strip()
        if not stripped:
            formatted_lines.append('')
            continue
        # Decrease indent before } lines
        if stripped.startswith('}'):
            indent_level = max(0, indent_level - 1)
        formatted_lines.append('    ' * indent_level + stripped)
        # Increase indent after { lines (but not for } { on same line)
        if stripped.endswith('{') and not stripped.startswith('}'):
            indent_level += 1

    result = '\n'.join(formatted_lines)

    # Step 3: Restore comments from placeholders
    def _restore_comment(m):
        idx = int(m.group(1))
        return comments[idx]
    result = re.sub(r'\x00C(\d+)\x00', _restore_comment, result)

    return result

# ── Token types and regex patterns (order matters: first match wins) ──

TOKENS = [
    # Comments — must come first to prevent inner highlighting
    ("comment_line",  r"//[^\n]*"),
    ("comment_block", r"/\*[\s\S]*?\*/"),
    # Preprocessor directives
    ("preprocessor",  r"#\w+"),
    # String literals (rare in shaders but possible)
    ("string",        r'"[^"]*"'),
    # Types — SkSL uses half/float prefix, GLSL uses vec/mat prefix
    ("type",          r"\b(?:half[234]?|float[234]?|double|int|uint|bool|void"
                      r"|mat[234]|vec[234]|ivec[234]|bvec[234]"
                      r"|sampler2D|samplerCube|shader|Shader)\b"),
    # Keywords
    ("keyword",       r"\b(?:uniform|varying|attribute|in|out|inout|const"
                      r"|if|else|for|while|do|switch|case|default"
                      r"|break|continue|return|discard"
                      r"|layout|precision|lowp|mediump|highp"
                      r"|flat|smooth|struct|inline"
                      r"|coherent|volatile|restrict|readonly|writeonly)\b"),
    # Built-in functions
    ("builtin",       r"\b(?:main|sin|cos|tan|asin|acos|atan|atan2"
                      r"|pow|exp|exp2|log|log2|sqrt|inversesqrt"
                      r"|abs|sign|floor|ceil|fract|mod|min|max|clamp|mix"
                      r"|step|smoothstep|length|distance|dot|cross|normalize"
                      r"|reflect|refract|texture|texture2D|sample"
                      r"|dFdx|dFdy|fwidth|radians|degrees|lerp|saturate)\b"),
    # Numeric literals — float (with optional exponent/suffix), hex, int
    ("number",        r"\b\d+\.?\d*(?:[eE][+-]?\d+)?[fF]?\b"
                      r"|\.\d+(?:[eE][+-]?\d+)?[fF]?\b"
                      r"|\b0[xX][0-9a-fA-F]+\b"),
    # Operators and punctuation
    ("operator",      r"[+\-*/%=<>!&|^~?:;,.{}()\[\]]"),
    # Remaining identifiers (variable names, etc.)
    ("identifier",    r"\b\w+\b"),
    # Whitespace
    ("whitespace",    r"\s+"),
    # Any remaining single character
    ("other",         r"."),
]

# ── CSS class mapping (token type → CSS class name) ──

CSS_MAP = {
    "comment_line":  "hl-cmt",
    "comment_block": "hl-cmt",
    "preprocessor":  "hl-pp",
    "string":        "hl-str",
    "type":          "hl-type",
    "keyword":       "hl-kw",
    "builtin":       "hl-fn",
    "number":        "hl-num",
    "operator":      "hl-op",
}

# ── Pre-compiled regex ──

# Each token pattern becomes a numbered capturing group in the combined regex.
# We check which group matched to determine the token type.
_COMBINED = "|".join(f"({pattern})" for _, pattern in TOKENS)
_REGEX = re.compile(_COMBINED, re.MULTILINE | re.DOTALL)


def highlight_glsl(source):
    """Apply GLSL/SkSL syntax highlighting to shader source code.

    Args:
        source: Raw shader source text string.

    Returns:
        HTML string with <span> tags for syntax-colored tokens.
        Non-highlighted tokens (identifiers, whitespace, other)
        are HTML-escaped plain text without spans.
    """
    if not source:
        return ""

    parts = []
    for match in _REGEX.finditer(source):
        for i, (token_type, _) in enumerate(TOKENS):
            group = match.group(i + 1)
            if group is not None:
                css_class = CSS_MAP.get(token_type)
                escaped = html.escape(group)
                if css_class:
                    parts.append(f'<span class="{css_class}">{escaped}</span>')
                else:
                    parts.append(escaped)
                break

    return "".join(parts)