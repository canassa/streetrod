"""`./sr rename`: give a function, a global or a source file its name everywhere at once.

A symbol (game function, DGROUP global or static, far data label) is renamed as a whole word
in the sources, the headers, the docs and the tools (`_name` in assembler too). A source file
(`M_xxxxx.C NAME.C`) and its header are moved with git, and the stem is replaced in
src/MAKEFILE, src/SR.LNK, the docs and the tools (and in the header's include guard).
`./sr headers` runs afterwards (include blocks, MAKEFILE dependencies). Names stay checked: a
new name must be a C identifier that no source, header or runtime library name uses yet,
ignoring case (LINK runs without /NOI). plans/diary.md is history and is never rewritten."""
import re
import subprocess

from . import config, makefile, source

_IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]{0,30}")
_FILE = re.compile(r"[A-Z0-9_]{1,8}\.(C|ASM)")


def text_files():
    """Every checked-in text file that may name a symbol or a source file."""
    root = config.ROOT
    out = config.source_files('.C', '.ASM', '.H', '.INC')
    out += [p for p in config.SRC.rglob('*') if p.suffix.upper() == '.MD']
    out += [root / "CLAUDE.md", root / "README.md"]
    out += list((root / "docs").glob("*.md"))
    out += list((root / ".claude" / "skills").rglob("*.md"))
    out += list((root / "tools" / "sr").glob("*.py")) + list((root / "tests").glob("*.py"))
    return sorted(p for p in out if p.is_file())


def _pattern(names, prefix=True):
    alt = "|".join(re.escape(n) for n in sorted(names, key=len, reverse=True))
    return re.compile(rf"(?<![A-Za-z0-9_$])({'_?' if prefix else ''})({alt})(?![A-Za-z0-9_])")


def _sub(pat, mapping, text):
    return pat.sub(lambda m: m.group(1) + mapping[m.group(2)], text)


def _symbol(name):
    """('function', addr) | ('global', off) | ('far', (seg, off)) for a current name."""
    hits = [("function", r["addr"]) for r in source.game_functions() if r["name"] == name]
    hits += [("global", off) for off, g in source.globals_(runtime_data=False).items() if g["name"] == name]
    hits += [("far", (g["seg"], g["off"])) for g in source.fardata() if g["name"] == name]
    if not hits and any(source.fname(r) == name for r in source.runtime()["functions"]):
        raise SystemExit(f"{name}: a runtime function keeps its library name")
    if len(hits) != 1:
        raise SystemExit(f"{name}: {'no' if not hits else len(hits)} functions/globals/far data of that name")
    return hits[0]


def _taken(files):
    """Every name in use, upper-cased: functions (and the runtime's aliases), globals (the
    runtime's data publics too), far data, and every identifier of the sources."""
    names = {source.fname(r).upper() for r in source.functions()}
    names |= {n.upper() for ns in source.aliases().values() for n in ns}
    names |= {n.upper() for n in source.global_offsets()}
    names |= {g["name"].upper() for g in source.fardata()}
    from .fileheaders import _strip
    for p in files:
        if p.suffix.upper() in (".C", ".ASM", ".H"):
            text = p.read_text(encoding="latin-1")
            # identifiers only: not the words of strings and comments
            text = re.sub(r"'[^'\n]*'|\"[^\"\n]*\"|;[^\n]*", "", text) if p.suffix.upper() == ".ASM" else _strip(text)
            for w in re.findall(r"[A-Za-z_]\w*", text):
                names.add(w.upper())
                if w.startswith("_"):
                    names.add(w[1:].upper())
    return names


def parse_pairs(lines):
    out = []
    for ln in lines:
        ln = ln.split("#")[0].strip()
        if ln:
            old, new = ln.split()
            out.append((old, new))
    return out


def rename(pairs, dry=False):
    """Apply (old, new) pairs; returns a report (list of lines)."""
    files = text_files()
    taken = _taken(files)
    syms, stems, moves, report = {}, {}, [], []
    news = [n for _, n in pairs]
    if len({n.upper() for n in news}) != len(news):
        raise SystemExit("a new name appears twice")
    olds = {o for o, _ in pairs}
    for old, new in pairs:
        if new in olds:
            raise SystemExit(f"{new} is both a new and an old name: rename in two steps")
        if _FILE.fullmatch(old.upper()) and "." in old:
            if not _FILE.fullmatch(new) or new.rsplit(".", 1)[1] != old.upper().rsplit(".", 1)[1]:
                raise SystemExit(f"{new}: a source file name is DOS 8.3, upper case, same extension as {old}")
            src = config.source_index().get(old.upper())
            if src is None:
                raise SystemExit(f"{old}: not a source file under src/")
            ostem, nstem = src.stem.upper(), new.rsplit(".", 1)[0]
            if nstem != ostem and any(p.stem.upper() == nstem for p in config.source_index().values()):
                raise SystemExit(f"{new}: a file of that name exists")
            stems[ostem] = nstem
            moves.append((src, src.with_name(new)))
            continue
        if not _IDENT.fullmatch(new):
            raise SystemExit(f"{new!r} is not a C identifier (letters, digits, _, at most 31 chars)")
        if new.upper() in taken and new.upper() != old.upper():
            raise SystemExit(f"{new} is already used (sources or runtime, ignoring case)")
        syms[old] = (new, _symbol(old))
    mapping = {o: n for o, (n, _) in syms.items()}
    spat = _pattern(mapping) if mapping else None
    fpat = _pattern(stems, prefix=False) if stems else None

    def fix(text, guard=None):
        if spat:
            text = _sub(spat, mapping, text)
        if fpat:
            text = _sub(fpat, stems, text)
        if guard:
            text = re.sub(rf"\b{guard[0]}_H\b", f"{guard[1]}_H", text)
        return text

    headers = {p.with_suffix('.H'): (p.stem, q.stem) for p, q in moves}
    for p in files + [makefile.MAKEFILE, makefile.RESPONSE]:
        raw = p.read_bytes().decode("latin-1")
        new = fix(raw, headers.get(p))
        if new != raw:
            n = (len(spat.findall(raw)) if spat else 0) + (len(fpat.findall(raw)) if fpat else 0)
            report.append(f"{p.relative_to(config.ROOT)}: {n} replacements")
            if not dry:
                p.write_bytes(new.encode("latin-1"))
    for old, (new, (kind, key)) in syms.items():
        report.append(f"{kind} {old} -> {new}")
    for src, dst in moves:
        orel, nrel = (p.relative_to(config.ROOT).as_posix() for p in (src, dst))
        report.append(f"file {orel} -> {nrel}")
        if dry:
            continue
        subprocess.run(["git", "mv", str(src), str(dst)], cwd=config.ROOT, check=True)
        header = src.with_suffix('.H')
        if header.exists():
            subprocess.run(["git", "mv", str(header), str(dst.with_suffix('.H'))],
                           cwd=config.ROOT, check=True)
    if dry:
        return report
    source.reload()
    if moves:
        makefile._cache.clear()
        _rewrap_link()
    from .headers import generate
    report += ["header problem: " + p for p in generate()]
    return report


def _rewrap_link():
    """The link block's dependency lines, wrapped again after object names changed length."""
    r = makefile.rule("LINK")
    makefile._replace(r, makefile._block(r.target, r.deps, r.raw[0]), makefile.MAKEFILE)


def static_key(name):
    """MSC 5.1's order key for a file's uninitialized statics (CLAUDE.md 4f)."""
    return sum(b & 0xDF for b in name.encode("ascii")) % 256


def fit(old, candidates):
    """Whether each candidate name keeps a placed global where it is: a communal must stay
    in its LINK hash-bucket window (CLAUDE.md 4e), a file static in its key window (4f).
    Ties with a neighbour depend on creation/declaration order: the build decides those.
    Returns lines of text."""
    from . import dgroup, fileheaders, linkhash, module
    placed = source.globals_(runtime_data=False)
    off = next((o for o, g in placed.items() if g["name"] == old), None)
    if off is None:
        return [f"{old}: not a DGROUP global; initialized data, functions and far data are placed "
                "by their definitions, not their names (only the compiler budget, 4d, counts the length)"]
    if dgroup.COMMON_START <= off < dgroup.END:
        rows = [g for o, g in placed.items() if dgroup.COMMON_START <= o < dgroup.END]
        key, what = (lambda n: linkhash.bucket("_" + n)), "bucket"
    elif dgroup.BSS_START <= off < dgroup.BSS_PRIVATE_END:
        mine = next(fileheaders.file_statics(f.path) for f in module.files() if old in fileheaders.file_statics(f.path))
        rows = [g for g in placed.values() if g["name"] in mine]
        key, what = static_key, "key"
    else:
        return [f"{old}: initialized data (DS:{off:04X}) is placed by its definition, not its name"]
    i = [r["name"] for r in rows].index(old)
    lo = key(rows[i - 1]["name"]) if i > 0 else -1
    hi = key(rows[i + 1]["name"]) if i + 1 < len(rows) else 256
    out = [f"{old} DS:{off:04X} {what} {key(old)}; window {lo}..{hi} "
           f"({rows[i - 1]['name'] if i else '-'} .. {rows[i + 1]['name'] if i + 1 < len(rows) else '-'})"]
    for c in candidates:
        k = key(c)
        verdict = "fits" if lo < k < hi else "tie (build decides)" if k in (lo, hi) else "no"
        out.append(f"  {c:<28} {what} {k:3}  {verdict}")
    return out
