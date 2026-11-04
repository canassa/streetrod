"""Source rules `./sr check` enforces on C files (CLAUDE.md rule 4)."""

import re

_PROTO_WARN = re.compile(r"warning C(4071|4016)\b.*")


def lint(text, log=""):
    problems = []
    body = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    body = re.sub(r'"(\\.|[^"\\])*"', '""', body)
    depth, stmt = 0, ""
    for ch in body:
        if ch == "{":
            if depth == 0 and re.search(r"\bstruct\s+\w+\s*$", stmt.strip()):
                problems.append(f"struct definition `{stmt.strip()}` belongs in STRUCTS.H")
            depth += 1
            stmt = ""
            continue
        if ch == "}":
            depth -= 1
            stmt = ""
            continue
        if depth:
            continue
        if ch == ";":
            s = " ".join(stmt.split())
            if re.search(r"\bextern\b", s):
                problems.append(f"`{s}`: declarations come from GLOBALS.H / PROTOS.H, never `extern` in a source")
            elif "(" in s and not s.startswith("#") and not re.search(r"\bstatic\b.*\bnear\b", s) \
                    and not re.match(r"[^(]*=", s) \
                    and not re.match(r"[^(]*\((interrupt\s+)?(far\s+|near\s+)?\*\s*\w+\)\s*\(", s):  # a function-pointer variable
                problems.append(f"`{s}`: function prototypes come from PROTOS.H "
                                "(only `static ... near` module-mates may be forward-declared)")
            stmt = ""
            continue
        if ch == "\n" and stmt.lstrip().startswith("#"):
            stmt = ""
            continue
        stmt += ch
    for m in _PROTO_WARN.finditer(log):
        problems.append(m.group(0).strip() + " (add it with ./sr proto)")
    return problems
