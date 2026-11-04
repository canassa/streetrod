"""sr — the Street Rod decompilation driver. Run `./sr -h`."""

import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from sr import cod, config, db, dosbox, layout  # noqa: E402
from sr import module as mod  # noqa: E402
from sr.disasm import branch_target, linear  # noqa: E402
from sr.exe import image  # noqa: E402
from sr.match import compare, diff_rows  # noqa: E402
from sr.omf import Obj  # noqa: E402
from sr.headers import generate as gen_headers  # noqa: E402
from sr.lint import lint  # noqa: E402
from sr.refs import verify  # noqa: E402
from sr.scan import far_call_operand, scan  # noqa: E402

SWEEP_OPTS = ["/Od", "/Os", "/Ot", "/Oa", "/Oas", "/Oat", "/Ol", "/Ols", "/Olt", "/Oal", "/Oals",
              "/Oalt", "/Oi", "/Ois", "/Oit", "/Oais", "/Oait", "/Oils", "/Oilt", "/Oails", "/Oailt",
              "/Ox", "/O"]


def hexa(a):
    return f"0x{a:05x}"


def fn_name(row):
    return row["name"] or f"F_{row['addr']:05X}"


# ---------------------------------------------------------------- scan / list / show


def cmd_scan(args):
    con = db.connect()
    rows = scan()
    known = {r["addr"] for r in con.execute("SELECT addr FROM functions")}
    added = 0
    for r in rows:
        if r["addr"] in known:
            con.execute("UPDATE functions SET seg=?, off=?, size=?, far=? WHERE addr=?",
                        (r["seg"], r["off"], r["size"], r["far"], r["addr"]))
        else:
            status = "skip" if r["kind"] == "crt" else "todo"
            con.execute("INSERT INTO functions (addr, seg, off, size, kind, far, status, updated) "
                        "VALUES (?,?,?,?,?,?,?,?)",
                        (r["addr"], r["seg"], r["off"], r["size"], r["kind"], r["far"], status, db.now()))
            added += 1
        con.execute("DELETE FROM callees WHERE caller = ?", (r["addr"],))
        con.executemany("INSERT OR IGNORE INTO callees VALUES (?, ?)", [(r["addr"], c) for c in r["callees"]])
    con.commit()
    gone = known - {r["addr"] for r in rows}
    print(f"{len(rows)} functions ({added} new, {len(rows) - added} updated)")
    if gone:
        print(f"warning: {len(gone)} DB rows no longer found by the scanner: "
              + ", ".join(hexa(a) for a in sorted(gone)[:20]))



_IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]{0,30}")
_DECL = re.compile(r"[A-Za-z0-9_ *\[\]()@,.]+")


def check_ident(name):
    # Every name and declaration lands in a shared header: one bad one breaks every build.
    if not _IDENT.fullmatch(name):
        raise SystemExit(f"{name!r} is not a C identifier (letters, digits, _, at most 31 chars)")


def check_decl(decl):
    if decl is not None and (decl.count("@") != 1 or not _DECL.fullmatch(decl)
                             or "." in decl.replace("...", "")):  # a shell glob expanded to a file name
        raise SystemExit(f"{decl!r}: a declaration needs exactly one @ and only C declarator characters")

def cmd_list(args):
    con = db.connect()
    q, p = "SELECT * FROM functions WHERE 1=1", []
    if args.status:
        q += " AND status = ?"
        p.append(args.status)
    if args.kind:
        q += " AND kind = ?"
        p.append(args.kind)
    if args.seg:
        q += " AND seg = ?"
        p.append(int(args.seg, 16))
    if args.name:
        q += " AND name LIKE ?"
        p.append(f"%{args.name}%")
    if args.max:
        q += " AND size <= ?"
        p.append(args.max)
    if args.range:
        lo, hi = (int(x, 16) for x in args.range.split("-"))
        q += " AND addr BETWEEN ? AND ?"
        p += [lo, hi]
    q += " ORDER BY " + ("size" if args.by_size else "addr")
    rows = con.execute(q, p).fetchall()
    print(f"{'addr':8} {'seg:off':10} {'size':>5} kind far {'status':8} {'mism':>4} name / src")
    for r in rows[: args.limit]:
        mism = "" if r["mismatches"] is None else str(r["mismatches"])
        extra = fn_name(r) + (f"  {r['src']}" if r["src"] else "")
        print(f"{hexa(r['addr'])}  {r['seg']:04x}:{r['off']:04x} {r['size']:5} {r['kind']:4} {'F' if r['far'] else 'N':3} "
              f"{r['status']:8} {mism:>4} {extra}")
    if len(rows) > args.limit:
        print(f"... {len(rows) - args.limit} more (use --limit)")


def annotate(con, im, row, ins):
    notes = []
    t = far_call_operand(im, ins)
    if t is not None:
        notes.append(db.label(con, t))
    elif ins.bytes[0] == 0xE8 and ins.size == 3:
        notes.append(db.label(con, branch_target(ins)))
    if ins.mnemonic in ("call", "lcall", "jmp", "ljmp") or ins.mnemonic.startswith("j"):
        return "  ; " + ", ".join(notes) if notes else ""
    for m in re.finditer(r"0x([0-9a-f]{3,4})\b", ins.op_str):
        if re.search(r"\[[^\]]*\b[bs]p\b[^\]]*$", ins.op_str[: m.start()]):
            continue  # a frame offset, [bp - 0x110], not a DGROUP address
        v = int(m.group(1), 16)
        g = con.execute("SELECT name FROM globals WHERE off = ?", (v,)).fetchone()
        if g:
            # An immediate equal to a global's offset is only a guess: it may be a plain number.
            notes.append(g["name"] + ("" if "[" in ins.op_str or ins.mnemonic == "lea" else "?"))
        dpos = config.DGROUP_SEG * 16 + v
        if "[" in ins.op_str and dpos in im.reloc_pos:
            seg = im.word(dpos)
            far = con.execute("SELECT name FROM fardata WHERE seg = ?", (seg,)).fetchall()
            notes.append(f"segment constant {seg:04x}: far data" + (f" ({', '.join(f['name'] for f in far)})" if far else ""))
            continue
        s = im.dgroup_string(v)
        if s is not None and ("[" not in ins.op_str or ins.mnemonic == "lea"):
            mid = im.img[config.DGROUP_SEG * 16 + v - 1] != 0
            notes.append(repr(s) + (" (inside a string: maybe just a number)" if mid else ""))
    return "  ; " + ", ".join(notes) if notes else ""


def cmd_show(args):
    con = db.connect()
    im = image()
    row = db.resolve(con, args.fn)
    print(f"{fn_name(row)} @ {row['seg']:04x}:{row['off']:04x} (linear {hexa(row['addr'])}), {row['size']} bytes, "
          f"{'far' if row['far'] else 'near'}, kind {row['kind']}, status {row['status']}")
    if row["src"]:
        f = mod.file_of(row)
        print(f"  src: {row['src']} ({'CL' if f.kind == 'c' else 'MASM'} {f.command})")
    if row["notes"]:
        print(f"  notes: {row['notes']}")
    callers = con.execute("SELECT caller FROM callees WHERE callee = ?", (row["addr"],)).fetchall()
    callees = con.execute("SELECT callee FROM callees WHERE caller = ?", (row["addr"],)).fetchall()
    if callers:
        print("  called by: " + ", ".join(db.label(con, c["caller"]) for c in callers))
    if callees:
        print("  calls:     " + ", ".join(db.label(con, c["callee"]) for c in callees))
    print()
    # Decoded at segment offsets (relative targets wrap at 64K like the CPU), printed as linear.
    base = row["seg"] * 16
    for ins in linear(im.img, row["addr"], row["addr"] + row["size"], ip0=row["off"]):
        t = branch_target(ins)
        text = ins.text()
        if t is not None:
            text = text.replace(f"0x{t:x}", f"0x{base + (t & 0xFFFF):x}")
            ins.op_str = ins.op_str.replace(f"0x{t:x}", f"0x{base + (t & 0xFFFF):x}")
        raw = ins.bytes.hex() if args.bytes else ""
        print(f"  {ins.addr:05x}  {raw:<14}{text:<40}{annotate(con, im, row, ins)}")


def cmd_next(args):
    con = db.connect()
    q = "SELECT * FROM functions WHERE status = 'todo' AND kind = ?"
    rows = con.execute(q, (args.kind,)).fetchall()
    if args.near:
        a = db.resolve(con, args.near)["addr"]
        rows.sort(key=lambda r: abs(r["addr"] - a))
    else:
        rows.sort(key=lambda r: r["size"])
    for r in rows[: args.n]:
        print(f"{hexa(r['addr'])}  {r['seg']:04x}:{r['off']:04x} {r['size']:5} bytes  {'far' if r['far'] else 'near'}")


# ---------------------------------------------------------------- authoring / checking


def _file_jobs(con, rows, flags_override=None):
    """One compile job per source file of `rows`: the whole file, with its command line."""
    fs = mod.files()
    jobs = {}
    for r in rows:
        f = mod.file_of(r, fs)
        if f.rel not in jobs:
            flags = f.with_flags(flags_override) if flags_override else f.command
            jobs[f.rel] = dosbox.Job(f.path, flags, f.rel)
            jobs[f.rel].file = f
    return jobs


def _check_rows(con, rows, flags_override=None, quiet=False):
    """Compile the files that define `rows` (once each), compare every function asked for,
    and run each file's gate over all its functions."""
    im = image()
    jobs = _file_jobs(con, rows, flags_override)
    dosbox.compile_all(list(jobs.values()), name="check")
    results = {}
    for r in rows:
        job = jobs[r["src"]]
        if job.obj is None or (job.cod is None and not job.asm):
            print(f"{hexa(r['addr'])} {fn_name(r)}: build failed\n{job.log}")
            results[r["addr"]] = None
            continue
        obj = Obj(job.obj)
        procs = []
        if job.asm:
            proc = cod.from_publics(obj, fn_name(r), cod.asm_lengths(job.lst))
            found = ", ".join(obj.publics)
        else:
            procs = cod.parse(job.cod)
            proc = cod.find(procs, fn_name(r))
            obj.statics = cod.statics(job.cod)
            found = ", ".join(p.c_name for p in procs)
        if proc is None:
            print(f"{hexa(r['addr'])}: no procedure {fn_name(r)} in {job.src.name} (found: {found})")
            results[r["addr"]] = None
            continue
        res = compare(obj, proc, im.img, r["addr"], r["size"])
        res.refs = verify(con, obj, proc, im, r["addr"], r, procs)
        if not job.asm:
            res.lint = lint(job.src.read_text(errors="replace"), job.log)
        results[r["addr"]] = (res, proc, job)
    _file_gates(con, im, jobs, results)
    if not quiet:
        for job in jobs.values():
            shown = [ln for ln in job.log.splitlines() if re.search(r"warning C(?!4071|4016|4100)\d+|error", ln)]
            if shown:
                print(f"{job.src.name}:\n  " + "\n  ".join(shown))
        for r in rows:
            if results.get(r["addr"]):
                print(f"{hexa(r['addr'])} {fn_name(r)}: {results[r['addr']][0].line()}")
    return results


def _record(con, row, res):
    was = row["status"]
    if res is None:
        return
    if res.verdict.startswith("MATCH"):
        db.update(con, row["addr"], status="matched", mismatches=0, nearcalls=res.nearcalls)
    else:
        # A matched function stays matched, so `check --all` keeps failing until someone fixes it
        # (a shared type or prototype change usually) or deliberately reopens it with `sr mark`.
        status = "wip" if was in ("todo", "wip") else was
        db.update(con, row["addr"], status=status, mismatches=len(res.mismatches) + abs(res.size_exe - res.size_obj),
                  nearcalls=res.nearcalls)
        if was == "matched":
            print(f"  !! REGRESSION: {fn_name(row)} was matched and no longer is. Fix it (often a shared "
                  "global/prototype change), or reopen it with `./sr mark ADDR wip -m why`")


def _rows_for(con, what):
    """A function by address or name, or every function of a source file (M_0914C.C)."""
    f = mod.find_file(what)
    if f is not None:
        return mod.members(con, f.rel)
    return [db.resolve(con, what)]


def cmd_check(args):
    con = db.connect()
    if args.all:
        rows = con.execute("SELECT * FROM functions WHERE status = 'matched' ORDER BY addr").fetchall()
    elif args.fn:
        rows = _rows_for(con, args.fn)
    else:
        raise SystemExit("give a function (address or name), a source file, or --all")
    results = _check_rows(con, rows, args.flags)
    bad = 0
    for r in rows:
        res = results.get(r["addr"])
        if args.flags is None:
            _record(con, r, res[0] if res else None)
        if res is None or res[0].verdict == "DIFF":
            bad += 1
        if res and len(rows) == 1 and res[1].locals:
            print("  locals: " + ", ".join(f"{k}={v}" for k, v in sorted(res[1].locals.items(), key=lambda kv: -kv[1])))
    if args.all:
        done = [results[r["addr"]][0] for r in rows if results.get(r["addr"])]
        exact = sum(res.verdict == "MATCH" for res in done)
        star = sum(res.verdict == "MATCH*" for res in done)
        print(f"{len(rows) - bad}/{len(rows)} still match ({exact} MATCH, {star} MATCH* with near "
              "calls to other objects)")
    sys.exit(1 if bad else 0)


def cmd_diff(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    results = _check_rows(con, [row], args.flags)
    res = results[row["addr"]]
    if res is None:
        sys.exit(1)
    result, proc, job = res
    rows = diff_rows(Obj(job.obj), proc, image().img, row["addr"], row["size"], row["off"])
    width = 52
    print(f"  {'ours (' + job.src.name + ')':<{width}}  original")
    shown = set()
    interesting = [i for i, r in enumerate(rows) if r[0] != " "]
    for i, (mark, a, b) in enumerate(rows):
        if args.full or any(abs(i - j) <= 2 for j in interesting):
            if not args.full and shown and i - 1 not in shown:
                print("  ...")
            shown.add(i)
            print(f"{mark} {a:<{width}}  {b}")
    if proc.locals:
        print("locals: " + ", ".join(f"{k}={v}" for k, v in sorted(proc.locals.items(), key=lambda kv: -kv[1])))


def cmd_sweep(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    f = mod.file_of(row)
    if f.kind != "c":
        raise SystemExit("sweep is for C sources; MASM has no codegen flags to search")
    base = args.base or "/AM /FPi"
    combos = [f"{base} {o}{g}" for o in SWEEP_OPTS for g in ("", " /Gs")]
    im = image()
    # every combination compiles the whole file (its other functions change with the flags too)
    jobs = [dosbox.Job(f.path, f.with_flags(c), c) for c in combos]
    dosbox.compile_all(jobs, name="sweep")
    out = []
    for job in jobs:
        if job.obj is None or job.cod is None:
            continue
        proc = cod.find(cod.parse(job.cod), fn_name(row))
        if proc is None:
            continue
        res = compare(Obj(job.obj), proc, im.img, row["addr"], row["size"])
        out.append((len(res.mismatches) + abs(res.size_exe - res.size_obj), job.tag, res))
    out.sort(key=lambda x: x[0])
    for score, flags, res in out[: args.top]:
        print(f"{score:5}  {flags:<28} {res.verdict}  size {res.size_obj}/{res.size_exe}")
    if out and out[0][0] == 0:
        print(f"\nthe file's flags are {f.flags}; set them with: ./sr flags {f.path.name} {out[0][1]} --force "
              f"(then ./sr check {f.path.name})")


def cmd_crtnames(args):
    from sr.lib import locate

    con = db.connect()
    libdir = config.TOOLCHAIN / "LIB"
    found = locate([libdir / n for n in ("MLIBCR.LIB", "MLIBFP.LIB", "LIBH.LIB", "EM.LIB")])
    named = 0
    aliases = 0
    for addr, pubs in sorted(found.items()):
        row = con.execute("SELECT * FROM functions WHERE addr = ?", (addr,)).fetchone()
        if row is None:
            continue
        names = [pub for pub, _ in pubs]
        managed = row["kind"] == "crt" and (row["notes"] or "").startswith("MSC 5.1 library module ")
        if row["name"] is None or row["name"] in names or managed:
            pub, module = pubs[names.index(row["name"])] if row["name"] in names else pubs[0]
            db.update(con, addr, name=pub, kind="crt", status="skip", notes=f"MSC 5.1 library module {module}")
            named += 1
        # Re-identification must invalidate false historical wrapper aliases.
        con.execute("DELETE FROM aliases WHERE addr = ?", (addr,))
        current = con.execute("SELECT name FROM functions WHERE addr = ?", (addr,)).fetchone()[0]
        for pub in names:
            if pub != current:
                aliases += con.execute("INSERT OR IGNORE INTO aliases (addr, name) VALUES (?, ?)", (addr, pub)).rowcount
    con.commit()
    crt = con.execute("SELECT COUNT(*) FROM functions WHERE kind = 'crt'").fetchone()[0]
    unnamed = con.execute("SELECT COUNT(*) FROM functions WHERE kind = 'crt' AND name IS NULL").fetchone()[0]
    print(f"{len(found)} library addresses located, {named} functions named, {aliases} new aliases; "
          f"{unnamed}/{crt} CRT rows still unnamed")


# ---------------------------------------------------------------- bookkeeping


def cmd_mark(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    fields = {"status": args.status}
    if args.message:
        fields["notes"] = args.message
    db.update(con, row["addr"], **fields)
    print(f"{fn_name(row)}: {row['status']} -> {args.status}")


def _sources():
    return sorted(p for d in ("mods", "data") for p in (config.SRC / d).glob("*")
                  if p.suffix.upper() in (".C", ".ASM"))


def cmd_name(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    if args.name is None:
        print(fn_name(row))
        return
    check_ident(args.name)
    old = fn_name(row)
    users = [p.name for p in _sources() if re.search(rf"\b_?{old}\b", p.read_text(errors="replace"))]
    if users and not args.force:
        raise SystemExit(f"{old} is used by {', '.join(users)}; rename it there too, then rerun with --force")
    db.update(con, row["addr"], name=args.name)
    print(f"{hexa(row['addr'])}: {fn_name(row)} -> {args.name}")


def cmd_kind(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    fields = {"kind": args.kind}
    if args.message:
        fields["notes"] = args.message
    db.update(con, row["addr"], **fields)
    print(f"{fn_name(row)}: kind {row['kind']} -> {args.kind}")


def cmd_note(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    text = " ".join(args.text)
    if row["notes"] and not args.replace:
        text = row["notes"] + " | " + text
    db.update(con, row["addr"], notes=text)


def cmd_flags(args):
    """A source file's command line (one per file: its block in src/MAKEFILE)."""
    con = db.connect()
    f = mod.find_file(args.fn, mod.sources()) or mod.file_of(db.resolve(con, args.fn))
    if not args.flags:
        print(f"{f.path.name}: flags = {f.flags}" + (f" (compiled with /NT{f.segment})" if f.kind == "c" and f.segment else ""))
        return
    flags = " ".join(args.flags)
    matched = [r for r in mod.members(con, f.rel) if r["status"] == "matched"]
    if matched and flags != f.flags and not args.force:
        raise SystemExit(f"{f.path.name} has {len(matched)} matched functions built with {f.flags}; "
                         "changing them may break them (--force, then ./sr check " + f.path.name + ")")
    from sr import makefile
    makefile.set_flags(f.rule.stem, flags)
    print(f"{f.path.name}: flags = {flags} (src/MAKEFILE)")


def cmd_global(args):
    con = db.connect()
    if args.off is None:
        for g in con.execute("SELECT * FROM globals ORDER BY off"):
            print(f"DS:{g['off']:04x}  {g['name']:<24} {g['decl'] or '(no decl)'}")
        for g in con.execute("SELECT * FROM fardata ORDER BY seg, off"):
            print(f"{g['seg']:04x}:{g['off']:04x}  {g['name']:<22} {g['decl'] or '(no decl)'}  (far data)")
        return
    if ":" in args.off:
        return _far_global(con, args)
    off = int(args.off, 16)
    if args.delete:
        con.execute("DELETE FROM globals WHERE off = ?", (off,))
        con.commit()
        gen_headers(con)
        print(f"DS:{off:04x} removed")
        return
    if args.name is None:
        g = con.execute("SELECT * FROM globals WHERE off = ?", (off,)).fetchone()
        print(f"DS:{off:04x}  {g['name']}  {g['decl'] or '(no decl)'}" if g else f"DS:{off:04x}: not declared")
        return
    check_ident(args.name)
    check_decl(args.decl)
    if args.decl and "@" not in args.decl:
        raise SystemExit("--decl needs @ where the name goes, e.g. \"int @\" or \"struct Entry @[]\"")
    clash = con.execute("SELECT off FROM globals WHERE name = ? AND off != ?", (args.name, off)).fetchone()
    if clash:
        raise SystemExit(f"{args.name} is already DS:{clash['off']:04x}")
    old = con.execute("SELECT name FROM globals WHERE off = ?", (off,)).fetchone()
    if old and old["name"] != args.name:
        users = [p.name for p in _sources() if re.search(rf"\b_?{old['name']}\b", p.read_text(errors="replace"))]
        if users:
            raise SystemExit(f"DS:{off:04x} is {old['name']}, used by {', '.join(users)}; renaming would "
                             f"break them. Use the existing name, or rename it in those files too")
    con.execute("INSERT INTO globals (off, name, decl) VALUES (?, ?, ?) "
                "ON CONFLICT(off) DO UPDATE SET name = excluded.name, decl = COALESCE(excluded.decl, decl)",
                (off, args.name, args.decl))
    con.commit()
    gen_headers(con)
    row = con.execute("SELECT * FROM globals WHERE off = ?", (off,)).fetchone()
    print(f"DS:{off:04x} = {row['name']}  {row['decl'] or '(no decl yet)'}")


def _far_global(con, args):
    seg, off = (int(x, 16) for x in args.off.split(":"))
    if args.delete:
        con.execute("DELETE FROM fardata WHERE seg = ? AND off = ?", (seg, off))
    elif args.name is None:
        g = con.execute("SELECT * FROM fardata WHERE seg = ? AND off = ?", (seg, off)).fetchone()
        print(f"{seg:04x}:{off:04x}  {g['name']}  {g['decl'] or '(no decl)'}" if g else f"{seg:04x}:{off:04x}: not declared")
        return
    else:
        check_ident(args.name)
        check_decl(args.decl)
        if args.decl and ("@" not in args.decl or "far" not in args.decl):
            raise SystemExit('far data needs a far declaration with @ for the name, e.g. "struct Slot far @[]"')
        clash = con.execute("SELECT seg, off FROM fardata WHERE name = ? AND NOT (seg = ? AND off = ?)",
                            (args.name, seg, off)).fetchone()
        if clash or con.execute("SELECT 1 FROM globals WHERE name = ?", (args.name,)).fetchone():
            raise SystemExit(f"{args.name} is already declared elsewhere")
        con.execute("INSERT INTO fardata (seg, off, name, decl) VALUES (?, ?, ?, ?) ON CONFLICT(seg, off) DO UPDATE "
                    "SET name = excluded.name, decl = COALESCE(excluded.decl, decl)", (seg, off, args.name, args.decl))
    con.commit()
    gen_headers(con)
    print(f"{seg:04x}:{off:04x} " + ("removed" if args.delete else f"= {args.name}"))


def cmd_proto(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    if args.proto is None:
        print(row["proto"] or "(no prototype)")
        return
    if args.proto == "":
        db.update(con, row["addr"], proto=None)
        gen_headers(con)
        print(f"{fn_name(row)}: prototype cleared")
        return
    check_decl(args.proto)
    if not row["far"] and "interrupt" not in args.proto and "near @" not in args.proto \
            and not ("far @" in args.proto and args.force):
        raise SystemExit(f"{fn_name(row)} is near (RET). A static module-mate is forward-declared in the "
                         "callers' .C as `static T near F(...);`, not a header prototype. A near function "
                         "other files call (same code segment) is public: `T near @(...)` goes into its file's "
                         "header. (If C code really far-calls this near routine, an original bug, use "
                         "`T far @(...)` with --force.)")
    same = row["proto"] and re.sub(r"\w+(?=\s*[,)])", "", row["proto"]) == re.sub(r"\w+(?=\s*[,)])", "", args.proto)
    if row["proto"] and row["proto"] != args.proto and not same and not args.force:
        raise SystemExit(f"{fn_name(row)} already has {row['proto']!r}; other files rely on it "
                         "(--force to replace, then ./sr check --all)")
    if "@" not in args.proto:
        raise SystemExit("the prototype needs @ where the name goes, e.g. \"int far @(int n)\"")
    db.update(con, row["addr"], proto=args.proto)
    gen_headers(con)
    print(f"{fn_name(row)}: {args.proto.replace('@', fn_name(row))}")


def cmd_data(args):
    con = db.connect()
    im = image()
    names = {g["off"]: g["name"] for g in con.execute("SELECT off, name FROM globals")}
    off = int(args.off, 16)
    for a in range(off, off + args.n, 16):
        chunk = im.img[config.DGROUP_SEG * 16 + a : config.DGROUP_SEG * 16 + min(a + 16, off + args.n)]
        here = [f"{k:04x}={names[k]}" for k in range(a, a + len(chunk)) if k in names]
        text = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
        print(f"DS:{a:04x}  {chunk.hex(' '):<47}  {text:<16} {' '.join(here)}")


def cmd_layout(args):
    if args.bucket is not None:
        print(" ".join(layout.by_bucket(args.bucket, args.avoid or ())))
        return
    if not args.names:
        raise SystemExit("give local names (or --bucket N)")
    block = layout.parse(args.names)
    if args.want:
        # Hidden /Ol temps (`~`) keep their position; only the named locals are renamed.
        temps = {k: t for k, t in enumerate(block) if isinstance(t, str) and layout.TEMP.match(t)}
        named = [v for k, v in enumerate(block) if k not in temps]
        if not named or any(not isinstance(v, layout.Var) for v in named):
            raise SystemExit("--want takes the locals of one block (and ~ temps), no { } or |")
        names, decl = layout.suggest([v.name for v in named], args.avoid or ())
        for v, n in zip(named, names):
            if n != v.name:
                print(f"  rename {v.name} -> {n}")
            v.name = n
        by = {v.name: v for v in named}
        block = [by[n] for n in decl]
        for k in sorted(temps):
            block.insert(min(k, len(block)), temps[k])
        print("  declare in this order: " + ", ".join(decl))
    layout.show(block, args.ol)
    if args.ol and any(v.reg for b, d in layout._blocks(block) if d for v in b if isinstance(v, layout.Var)):
        print("  note: /Ol with a register local in a nested block is not modelled; ./sr check it")


def cmd_headers(args):
    con = db.connect()
    if args.check:
        from sr.headers import check
        drift = check(con)
        for p in sorted(drift):
            print(("would delete " if drift[p] is None else "would rewrite ") + str(p.relative_to(config.ROOT)))
        if drift:
            sys.exit(f"{len(drift)} files differ from what ./sr headers writes")
        print("the headers, the include blocks and the MAKEFILE dependencies are up to date")
        return
    gen_headers(con, rewrite_sources=True)
    print("wrote the per-file headers in src/include, the include blocks of the C sources "
          "and the header dependencies in src/MAKEFILE")


def cmd_status(args):
    con = db.connect()
    rows = con.execute("SELECT * FROM functions ORDER BY addr").fetchall()
    kinds = {}
    for r in rows:
        k = kinds.setdefault(r["kind"], {})
        n, b = k.get(r["status"], (0, 0))
        k[r["status"]] = (n + 1, b + r["size"])
    lines = ["# Progress", "", f"Generated by `./sr status` on {db.now()}. Do not edit by hand.", "",
             "| kind | status | functions | bytes |", "|---|---|---|---|"]
    for kind in sorted(kinds):
        for st in config.STATUSES:
            if st in kinds[kind]:
                n, b = kinds[kind][st]
                lines.append(f"| {kind} | {st} | {n} | {b} |")
    parts = []
    for kind, label in (("c", "C"), ("asm", "asm")):
        sel = [r for r in rows if r["kind"] == kind]
        done = [r for r in sel if r["status"] == "matched"]
        pct_n = 100 * len(done) / max(len(sel), 1)
        pct_b = 100 * sum(r["size"] for r in done) / max(sum(r["size"] for r in sel), 1)
        parts.append(f"{label} {len(done)}/{len(sel)} ({pct_n:.1f}% by count, {pct_b:.1f}% by bytes)")
    summary = "Game functions matched: " + "; ".join(parts)
    lines += ["", summary, ""]
    flags = {f.rel: f.flags for f in mod.files()}
    for title, st in (("Matched", "matched"), ("In progress", "wip"), ("Partial", "partial"), ("Blocked", "blocked")):
        sel = [r for r in rows if r["status"] == st]
        if not sel:
            continue
        lines += [f"## {title}", "", "| addr | name | size | src | flags | notes |", "|---|---|---|---|---|---|"]
        for r in sel:
            near = f" ({r['nearcalls']} near calls to other objects)" if r["nearcalls"] else ""
            lines.append(f"| `{hexa(r['addr'])}` | `{fn_name(r)}` | {r['size']} | `{r['src'] or ''}` | {flags.get(r['src'], '')} | "
                         f"{(r['notes'] or '') + near} |")
        lines.append("")
    config.PROGRESS.parent.mkdir(exist_ok=True)
    config.PROGRESS.write_text("\n".join(lines))
    print(summary)
    print(f"wrote {config.PROGRESS.relative_to(config.ROOT)}")


# ---------------------------------------------------------------- source files


def _file_gates(con, im, jobs, results):
    """Run the whole-span gate once per compiled file, over all its functions."""
    for job in jobs.values():
        if job.obj is None or (job.cod is None and not job.asm):
            continue
        f = job.file
        obj = Obj(job.obj)
        ms = [m for m in mod.members(con, f.rel) if m["status"] == "matched"
              or results.get(m["addr"])]
        if not ms:
            continue
        lengths = cod.asm_lengths(job.lst) if job.asm else None
        procs = ([cod.from_publics(obj, fn_name(m), lengths) for m in ms] if job.asm else cod.parse(job.cod))
        if not job.asm:
            obj.statics = cod.statics(job.cod)
        g = mod.gate(obj, [p for p in procs if p is not None], im.img, ms, con=con)
        print(g.line(f.path.name))
        lo, hi = ms[0]["addr"], ms[-1]["addr"] + ms[-1]["size"]
        for m in ms:
            res = results.get(m["addr"])
            if res:
                res[0].module = g
                if g.ok:
                    # the gate resolved every near call inside the file; what is left
                    # goes to another object (the layout build checks those encodings)
                    res[0].nearcalls = sum(not lo <= t < hi for t in res[0].near_targets)


def cmd_module(args):
    """Candidate same-file spans from near-call evidence (how the files' extents were found)."""
    con = db.connect()
    im = image()
    srcs = {f.rel: f for f in mod.files()}
    for lo, hi in mod.units(con, im):
        fs = con.execute("SELECT * FROM functions WHERE addr >= ? AND addr < ? ORDER BY addr", (lo, hi)).fetchall()
        done = sum(f["status"] == "matched" for f in fs)
        files = sorted({f["src"] for f in fs if f["src"]})
        flags = sorted({srcs[s].flags for s in files if s in srcs})
        print(f"{hexa(lo)}-{hexa(hi)}  {len(fs):3} functions, {done} matched"
              + (f", files {', '.join(Path(s).name for s in files)}" if files else "") + f"  flags: {' | '.join(flags)}")
        if args.verbose:
            for f in fs:
                print(f"    {hexa(f['addr'])} {fn_name(f):<10} {f['status']:<8} {f['src'] or ''}")


def cmd_dgroup(args):
    from sr.dgroup import Analysis
    a = Analysis()
    if args.what == 'report':
        lo = int(args.names[0], 16) if args.names else 0x42
        hi = int(args.names[1], 16) if len(args.names) > 1 else 0x629C
        print(a.summary())
        try:
            a.blocks()
        except ValueError as e:
            print('!! ' + str(e))
        print(a.report(lo, hi))
    elif args.what == 'problems':
        print(a.summary())
        print('\n'.join(a.problems()))
    elif args.what == 'order':
        from sr import makefile
        print(' '.join(f'{s:05X}' for s in a.link_order()))
        want, have = a.response_order(), makefile.link_order()
        if want != have:
            print('src/SR.LNK differs from the recovered order; it would be:\n' + '+\n'.join(want))
            sys.exit(1)
        print('src/SR.LNK lists the objects in this order')
    elif args.what == 'register':
        new = a.register(write=args.write)
        for off, name, decl in new:
            print(f'DS:{off:04x} {name} {decl}')
        print(f'{len(new)} synthesized globals' + (' registered' if args.write else ' (use --write)'))
        if args.write:
            gen_headers(a.con)
    elif args.what == 'aliases':
        rew, changed = a.apply_aliases(write=args.write)
        for name, (expr, asm) in rew.items():
            print(f'{name} -> {expr}   (asm: {asm})')
        print(f'{len(changed)} source files' + (' rewritten' if args.write else ' would change (use --write)'))
        if args.write:
            gen_headers(a.con)
    elif args.what == 'check':
        # the sources are the truth for data definitions; this only verifies them
        problems = a.check_sources()
        print('\n'.join(problems) if problems else
              'Every initialized DGROUP global is defined in the source of the unit that owned it')
        if problems:
            raise SystemExit(1)
    else:
        raise SystemExit(args.what)


def cmd_linkcheck(args):
    from sr.linkmatch import check
    raise SystemExit(check(args.executable, args.mapfile, args.output))


def cmd_build(args):
    from sr.linklayout import run
    sys.exit(run(args.prepare_only, args.objects_only, verify=not args.no_verify))


def main():
    p = argparse.ArgumentParser(prog="sr", description="Street Rod decompilation driver")
    sub = p.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("dgroup", help="recover DGROUP ownership from the data layout; check the sources against it")
    s.add_argument("what", choices=["report", "problems", "order", "register", "aliases", "check"],
                   help="check: every initialized global is defined in the file that owned it (exit 1 if not)")
    s.add_argument("names", nargs="*", help="lo hi for report")
    s.add_argument("--write", action="store_true")
    s.set_defaults(func=cmd_dgroup)

    s = sub.add_parser("linkcheck", help="compare the linked MZ binary without masking any bytes")
    s.add_argument("executable", nargs="?", help="EXE to compare (default work/layout/SR.EXE)")
    s.add_argument("--map", dest="mapfile", help="linker MAP for position/call diagnostics")
    s.add_argument("--output", help="JSON report path")
    s.set_defaults(func=cmd_linkcheck)

    s = sub.add_parser("build", help="run src/MAKEFILE in work/layout (MSC 5.1, MASM 5.1, LIB 3.11, LINK 5.01), then verify against orig/")
    s.add_argument("--prepare-only", action="store_true", help="only copy src/ (MAKEFILE, SR.LNK, sources, headers) to work/layout")
    s.add_argument("--objects-only", action="store_true", help="compile (and gate) every object without linking")
    s.add_argument("--no-verify", action="store_true", help="only build: no object gates, no comparison (needs neither orig/ nor the DB)")
    s.set_defaults(func=cmd_build)

    s = sub.add_parser("scan", help="(re)discover functions in the EXE and update the DB")
    s.set_defaults(func=cmd_scan)

    s = sub.add_parser("crtnames", help="name CRT functions by locating MSC 5.1 library modules")
    s.set_defaults(func=cmd_crtnames)

    s = sub.add_parser("list", help="list functions")
    s.add_argument("-s", "--status", choices=config.STATUSES)
    s.add_argument("-k", "--kind", choices=config.KINDS)
    s.add_argument("--seg", help="segment value, hex")
    s.add_argument("-n", "--name", help="name substring")
    s.add_argument("--max", type=int, help="max size in bytes")
    s.add_argument("--by-size", action="store_true")
    s.add_argument("-r", "--range", help="address range, hex: 10d6c-168f2")
    s.add_argument("--limit", type=int, default=60)
    s.set_defaults(func=cmd_list)

    s = sub.add_parser("show", help="annotated disassembly of the original function")
    s.add_argument("fn")
    s.add_argument("-b", "--bytes", action="store_true", help="also show raw bytes")
    s.set_defaults(func=cmd_show)

    s = sub.add_parser("next", help="suggest todo functions to work on")
    s.add_argument("-n", type=int, default=15)
    s.add_argument("-k", "--kind", default="c", choices=config.KINDS)
    s.add_argument("--near", help="sort by distance to this function (same module, likely same flags)")
    s.set_defaults(func=cmd_next)

    s = sub.add_parser("check", help="compile the function's whole source file, byte-compare; records the result")
    s.add_argument("fn", nargs="?", help="address, name, or a source file (M_0914C.C: all its functions)")
    s.add_argument("--all", action="store_true", help="compile every file once, check every matched function (regression gate)")
    s.add_argument("--flags", help="try these CL/MASM flags for the file without recording the result")
    s.set_defaults(func=cmd_check)

    s = sub.add_parser("diff", help="two-column instruction diff, ours vs original")
    s.add_argument("fn")
    s.add_argument("--full", action="store_true")
    s.add_argument("--flags")
    s.set_defaults(func=cmd_diff)

    s = sub.add_parser("sweep", help="compile the function's file with 46 /O flag combinations, score the function")
    s.add_argument("fn")
    s.add_argument("--base", help="flags kept in every combo (default: /AM /FPi)")
    s.add_argument("--top", type=int, default=10)
    s.set_defaults(func=cmd_sweep)

    s = sub.add_parser("mark", help="set a function's status")
    s.add_argument("fn")
    s.add_argument("status", choices=config.STATUSES)
    s.add_argument("-m", "--message", help="note to store with it")
    s.set_defaults(func=cmd_mark)

    s = sub.add_parser("name", help="name a function (no name: print the current one)")
    s.add_argument("fn")
    s.add_argument("name", nargs="?")
    s.add_argument("--force", action="store_true", help="rename even though sources use the old name")
    s.set_defaults(func=cmd_name)

    s = sub.add_parser("kind", help="correct the scanner's c/asm/crt guess")
    s.add_argument("fn")
    s.add_argument("kind", choices=config.KINDS)
    s.add_argument("-m", "--message", help="why (stored as the note)")
    s.set_defaults(func=cmd_kind)

    s = sub.add_parser("note", help="append to a function's notes (--replace to overwrite)")
    s.add_argument("fn")
    s.add_argument("text", nargs="+")
    s.add_argument("--replace", action="store_true")
    s.set_defaults(func=cmd_note)

    s = sub.add_parser("flags", help="a source file's CL/MASM flags (no flags: print them)")
    s.add_argument("fn", help="a source file (M_0914C.C) or a function in it")
    s.add_argument("flags", nargs="*")
    s.add_argument("--force", action="store_true", help="allow changing the flags of a file with matched functions")
    s.set_defaults(func=cmd_flags)

    s = sub.add_parser("module", help="candidate same-file spans from near-call evidence")
    s.add_argument("op", choices=["units"])
    s.add_argument("-v", "--verbose", action="store_true")
    s.set_defaults(func=cmd_module)

    s = sub.add_parser("global", help="name/declare a DGROUP global (no args: list them)")
    s.add_argument("off", nargs="?", help="hex DGROUP offset, or SEG:OFF for far data")
    s.add_argument("name", nargs="?")
    s.add_argument("--decl", help='C declaration with @ for the name, e.g. "int @" or "struct Slot far @[]"')
    s.add_argument("--delete", action="store_true", help="remove the entry")
    s.set_defaults(func=cmd_global)

    s = sub.add_parser("proto", help="set a function's prototype for its file's header (no proto: print it)")
    s.add_argument("fn")
    s.add_argument("proto", nargs="?", help='with @ for the name, e.g. "int far @(int n)"')
    s.add_argument("--force", action="store_true", help="replace an existing prototype")
    s.set_defaults(func=cmd_proto)

    s = sub.add_parser("data", help="hex dump of DGROUP bytes (initial values), with known globals")
    s.add_argument("off", help="DGROUP offset, hex")
    s.add_argument("n", type=lambda x: int(x, 0), nargs="?", default=64, help="byte count (0x40 or 64)")
    s.set_defaults(func=cmd_data)

    s = sub.add_parser("layout", help="predict the [bp-N] layout of locals from their names; --want suggests names")
    s.add_argument("names", nargs="*", help="locals in declaration order: NAME[:TYPE|:BYTES], { } for blocks, "
                   "| for a label, ~ for an /Ol loop temp")
    s.add_argument("-w", "--want", action="store_true",
                   help="the names are the wanted order of one block, top of the frame first ([bp-2], highest address): suggest renames and a declaration order")
    s.add_argument("--avoid", nargs="+", help="names a rename must not use (params, globals)")
    s.add_argument("--ol", action="store_true", help="the function is compiled with /Ol (block slot reuse differs)")
    s.add_argument("--bucket", type=int, help="list common readable names in bucket N (0-15) instead")
    s.set_defaults(func=cmd_layout)

    s = sub.add_parser("headers", help="regenerate the per-file headers, the sources' include blocks and the MAKEFILE dependencies")
    s.add_argument("--check", action="store_true", help="write nothing; exit 1 if a regeneration would change a file")
    s.set_defaults(func=cmd_headers)

    s = sub.add_parser("status", help="summary + regenerate docs/PROGRESS.md")
    s.set_defaults(func=cmd_status)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
