"""Source rules `./sr check` enforces on C files (CLAUDE.md rule 4)."""

import re

_PROTO_WARN = re.compile(r"warning C(4071|4016)\b.*")

# Block-scope `extern` declarations are flagged like file-scope ones. The only exceptions are the two
# PROVISIONAL far symbols of DASH.C (da_speedo_pts, da_tach_pts): unreachable accesses that reproduce two
# SEG constants in CONST that no instruction reads; the original statements are unknown.
_PROVISIONAL_EXTERNS = {"extern char far x_trk0[][2]", "extern char far x_trk1[][2]"}


def lint(text, log=""):
    problems = []
    body = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    body = re.sub(r'"(\\.|[^"\\])*"', '""', body)
    body = re.sub(r"'(\\.|[^'\\])+'", "' '", body)
    depth, stmt = 0, ""
    for ch in body:
        if ch == "{":
            if depth == 0 and re.search(r"\bstruct\s+\w+\s*$", stmt.strip()):
                problems.append(f"struct definition `{stmt.strip()}` belongs in its owning module's header")
            depth += 1
            stmt = ""
            continue
        if ch == "}":
            depth -= 1
            stmt = ""
            continue
        if ch == ";":
            s = " ".join(stmt.split())
            if re.search(r"\bextern\b", s):
                if depth and s in _PROVISIONAL_EXTERNS:
                    stmt = ""
                    continue
                problems.append(f"`{s}`: declarations come from the defining file's header (`./sr headers`), never `extern` in a source")
            elif depth == 0 and "(" in s and not s.startswith("#") and not re.search(r"\bstatic\b.*\bnear\b", s) \
                    and not re.match(r"[^(]*=", s) \
                    and not re.match(r"[^(]*\((interrupt\s+)?(far\s+|near\s+)?\*\s*\w+\)\s*\(", s):  # a function-pointer variable
                problems.append(f"`{s}`: function prototypes come from the defining file's header "
                                "(only `static ... near` module-mates may be forward-declared)")
            stmt = ""
            continue
        if ch == "\n" and stmt.lstrip().startswith("#"):
            stmt = ""
            continue
        stmt += ch
    for m in _PROTO_WARN.finditer(log):
        problems.append(m.group(0).strip() + " (prototype it in the defining file's header)")
    return problems
