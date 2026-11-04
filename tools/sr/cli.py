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


def src_path(row):
    return config.ROOT / (row["src"] or f"src/funcs/F_{row['addr']:05X}.C")


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
        extra = fn_name(r) + (f"  {r['src']}" if r["src"] else "") + (f"  [{r['flags']}]" if r["flags"] else "")
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
    for k in ("src", "flags", "notes"):
        if row[k]:
            print(f"  {k}: {row[k]}")
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


def cmd_new(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    name = row["name"] or f"F_{row['addr']:05X}"
    path = config.FUNCS / f"F_{row['addr']:05X}.{'ASM' if args.asm else 'C'}"
    if path.exists():
        # Adopt an existing file (e.g. written before `kind` was corrected); never overwrite it.
        db.update(con, row["addr"], status="wip", src=str(path.relative_to(config.ROOT)), name=name)
        print(f"{path.relative_to(config.ROOT)} already exists, kept it; src -> it, status -> wip")
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    where = f"{name} @ {row['seg']:04X}:{row['off']:04X} (linear 0x{row['addr']:05X}, {row['size']} bytes)"
    if args.asm:
        sym = "_" + name
        path.write_text(
            f"; {where}\n\n        .MODEL  MEDIUM\n        .CODE\n\n        PUBLIC  {sym}\n"
            f"{sym}  PROC    {'FAR' if row['far'] else 'NEAR'}\n{sym}  ENDP\n\n        END\n"
        )
    else:
        if row["proto"] and (row["far"] or "interrupt" in row["proto"] or "near @" in row["proto"]):
            head = row["proto"].replace("@", name)
        elif row["proto"]:
            head = "static " + row["proto"].replace("@", "near " + name)
        else:
            head = f"{'int' if row['far'] else 'static int near'} {name}(void)"
        path.write_text(f"/* {where} */\n\n#include \"STRUCTS.H\"\n\n{head}\n{{\n}}\n")
    db.update(con, row["addr"], status="wip", src=str(path.relative_to(config.ROOT)), name=name)
    print(f"created {path.relative_to(config.ROOT)}; status -> wip")


def _job_key(row, flags_override=None):
    src = src_path(row)
    default = config.DEFAULT_ASM_FLAGS if src.suffix.upper() == ".ASM" else config.DEFAULT_FLAGS
    return (str(src), flags_override or row["flags"] or default)


def _check_rows(con, rows, flags_override=None, quiet=False):
    im = image()
    gen_headers(con)
    jobs = {}
    for r in rows:
        key = _job_key(r, flags_override)
        jobs.setdefault(key, dosbox.Job(Path(key[0]), key[1], key))
    dosbox.compile_all(list(jobs.values()), name="check")
    results = {}
    for r in rows:
        job = jobs[_job_key(r, flags_override)]
        if job.obj is None or (job.cod is None and not job.asm):
            print(f"{hexa(r['addr'])} {fn_name(r)}: build failed\n{job.log}")
            results[r["addr"]] = None
            continue
        obj = Obj(job.obj)
        procs = []
        if job.asm:
            proc = cod.from_publics(obj, fn_name(r))
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
        if not quiet:
            shown = [ln for ln in job.log.splitlines()
                     if re.search(r"warning C(?!4071|4016|4100)\d+|error", ln)]
            warn = "  " + "\n  ".join(shown) if shown else ""
            print(f"{hexa(r['addr'])} {fn_name(r)}: {res.line()}" + (f"\n{warn}" if warn else ""))
            if not res.verdict.startswith("MATCH") and not job.asm:
                miss = _missing_stb(con, im, r, job.src, procs)
                if miss:
                    print(f"  hint: the original calls {', '.join(miss)} with `push cs; call near` "
                          "(same module): list them in the .STB")
                extra = _extra_stb(con, im, r, job.src)
                if extra:
                    print(f"  hint: the original far-calls {', '.join(extra)} (another module): "
                          "remove them from the .STB")
    _module_gates(con, im, jobs, rows, results, flags_override)
    return results


def _extra_stb(con, im, row, src):
    """`@NAME` lines in the .STB for functions the original reaches with a far `lcall`."""
    stub = src.with_suffix(".STB")
    if not stub.exists():
        return []
    names = re.findall(r"(?m)^@(\w+)\s*$", stub.read_text(errors="replace"))
    out = []
    for ins in linear(im.img, row["addr"], row["addr"] + row["size"]):
        t = far_call_operand(im, ins)
        if t is None:
            continue
        f = con.execute("SELECT * FROM functions WHERE addr = ?", (t,)).fetchone()
        if f and fn_name(f) in names and fn_name(f) not in out:
            out.append(fn_name(f))
    return out


def _missing_stb(con, im, row, src, procs):
    """Far functions the original reaches by `push cs; call near` that our build doesn't define."""
    ours = {p.c_name for p in procs}
    out = []
    for ins in linear(im.img, row["addr"], row["addr"] + row["size"], ip0=row["off"]):
        if ins.bytes[0] != 0xE8 or ins.size != 3 or im.img[ins.addr - 1] != 0x0E:
            continue
        t = row["seg"] * 16 + (branch_target(ins) & 0xFFFF)
        f = con.execute("SELECT * FROM functions WHERE addr = ?", (t,)).fetchone()
        if f and f["far"] and fn_name(f) not in ours and fn_name(f) not in out:
            out.append(fn_name(f))
    return out


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


def cmd_check(args):
    con = db.connect()
    if args.all:
        rows = con.execute("SELECT * FROM functions WHERE status = 'matched' ORDER BY addr").fetchall()
    else:
        rows = [db.resolve(con, args.fn)]
    results = _check_rows(con, rows, args.flags)
    bad = 0
    for r in rows:
        res = results.get(r["addr"])
        if args.flags is None:
            _record(con, r, res[0] if res else None)
        if res is None or res[0].verdict == "DIFF":
            bad += 1
        if res and not args.all and res[1].locals:
            print("  locals: " + ", ".join(f"{k}={v}" for k, v in sorted(res[1].locals.items(), key=lambda kv: -kv[1])))
    if args.all:
        print(f"{len(rows) - bad}/{len(rows)} still match")
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
    if src_path(row).suffix.upper() == ".ASM":
        raise SystemExit("sweep is for C sources; MASM has no codegen flags to search")
    base = args.base or "/AM /FPi"
    combos = [f"{base} {o}{g}" for o in SWEEP_OPTS for g in ("", " /Gs")]
    im = image()
    jobs = [dosbox.Job(src_path(row), c, c) for c in combos]
    dosbox.compile_all(jobs, name="sweep")
    out = []
    for job in jobs:
        if job.obj is None or job.cod is None:
            continue
        proc = cod.find(cod.parse(job.cod), fn_name(row))
        if proc is None:
            continue
        res = compare(Obj(job.obj), proc, im.img, row["addr"], row["size"])
        out.append((len(res.mismatches) + abs(res.size_exe - res.size_obj), job.flags, res))
    out.sort(key=lambda x: x[0])
    for score, flags, res in out[: args.top]:
        print(f"{score:5}  {flags:<28} {res.verdict}  size {res.size_obj}/{res.size_exe}")
    if out and out[0][0] == 0:
        print(f"\nset it with: ./sr flags {fn_name(row)} {out[0][1]}")


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


def cmd_name(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    if args.name is None:
        print(fn_name(row))
        return
    check_ident(args.name)
    old = fn_name(row)
    users = [p.name for p in sorted(config.ROOT.glob("src/funcs/*.[CcAaSs]*"))
             if re.search(rf"\b_?{old}\b", p.read_text(errors="replace"))]
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
    con = db.connect()
    row = db.resolve(con, args.fn)
    if not args.flags and not args.reset:
        print(f"{fn_name(row)}: flags = {row['flags'] or config.DEFAULT_FLAGS + ' (default)'}")
        return
    flags = None if args.reset else " ".join(args.flags)
    if row["status"] == "matched" and flags != row["flags"] and not args.force:
        raise SystemExit(f"{fn_name(row)} is matched with {row['flags'] or 'the default flags'}; "
                         "changing them would break it (--force to override, then recheck)")
    db.update(con, row["addr"], flags=flags)
    print(f"{fn_name(row)}: flags = {flags or config.DEFAULT_FLAGS + ' (default)'}")


def cmd_src(args):
    con = db.connect()
    row = db.resolve(con, args.fn)
    module = Path(args.path).name if Path(args.path).parent.name == 'mods' else None
    db.update(con, row["addr"], src=args.path, module=module)
    print(f"{fn_name(row)}: src = {args.path}" + (f", module {module}" if module else ""))


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
        users = [p.name for p in sorted(config.ROOT.glob("src/funcs/*.[CcAa]*"))
                 if re.search(rf"\b_?{old['name']}\b", p.read_text(errors="replace"))]
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
    from sr import fileheaders
    con = db.connect()
    gen_headers(con)
    fileheaders.generate(con, rewrite_sources=True)
    print("wrote the per-file headers in src/include and the include blocks of the C sources")


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
    for title, st in (("Matched", "matched"), ("In progress", "wip"), ("Partial", "partial"), ("Blocked", "blocked")):
        sel = [r for r in rows if r["status"] == st]
        if not sel:
            continue
        lines += [f"## {title}", "", "| addr | name | size | src | flags | notes |", "|---|---|---|---|---|---|"]
        for r in sel:
            flags = r["flags"] or ""
            near = f" ({r['nearcalls']} near calls unverified)" if r["nearcalls"] else ""
            lines.append(f"| `{hexa(r['addr'])}` | `{fn_name(r)}` | {r['size']} | `{r['src'] or ''}` | {flags} | "
                         f"{(r['notes'] or '') + near} |")
        lines.append("")
    config.PROGRESS.parent.mkdir(exist_ok=True)
    config.PROGRESS.write_text("\n".join(lines))
    print(summary)
    print(f"wrote {config.PROGRESS.relative_to(config.ROOT)}")


# ---------------------------------------------------------------- modules


def _module_gates(con, im, jobs, rows, results, flags_override=None):
    """Run the whole-span gate once per module object built in this check."""
    gates = {}
    for r in rows:
        name = r["module"]
        if not name or name in gates:
            continue
        job = jobs.get(_job_key(r, flags_override))
        if job is None or job.obj is None or (job.cod is None and not job.asm):
            continue
        obj = Obj(job.obj)
        ms = mod.members(con, name)
        procs = ([cod.from_publics(obj, fn_name(m)) for m in ms] if job.asm else cod.parse(job.cod))
        if not job.asm:
            obj.statics = cod.statics(job.cod)
        g = mod.gate(obj, [p for p in procs if p is not None], im.img, ms, con=con)
        gates[name] = g
        print(g.line(name))
        for m in mod.members(con, name):
            res = results.get(m["addr"])
            if res:
                res[0].module = g
                if g.ok and not g.nearcalls:
                    res[0].nearcalls = 0  # every near call in the span was resolved and verified
    return gates


def _stb_lines(p):
    if not p.exists():
        return []
    # Keep a multiline placeholder definition together when removing member stubs.
    text = p.read_text(errors="replace")
    return [m.group(0).strip() for m in re.finditer(
        r"static\b[^{}]*\{[^{}]*\}|[^\n]+", text) if m.group(0).strip()]


def _placeholder_for(text, row):
    """A near member's definition head from the module text, as an .STB placeholder."""
    m = re.search(rf"^(static [^;{{]*\bnear\s+{fn_name(row)}\s*\([^)]*\))\s*$", text, re.M)
    if not m:
        return None
    head = m.group(1)
    return head + (" { }" if re.match(r"static\s+void\b", head) else " { return 0; }")


def _module_write(con, name, rows, texts, stubs):
    names = {fn_name(r) for r in rows}
    stub = []
    for ln in stubs:
        ref = re.match(r"^@(\w+)\s*$", ln) or re.search(r"\b(\w+)\s*\(", ln)
        if ref and ref.group(1) in names or ln in stub:
            continue
        stub.append(ln)
    mod.MODS.mkdir(exist_ok=True)
    mod.path(name).write_text(mod.merge_asm(texts, rows) if name.endswith(".ASM") else mod.merge(texts))
    st = mod.path(name).with_suffix(".STB")
    if stub:
        st.write_text("\n".join(stub) + "\n")
    elif st.exists():
        st.unlink()


def _snapshot(paths):
    return {p: (p.read_bytes() if p.exists() else None) for p in paths}


def _restore(snap):
    for p, data in snap.items():
        if data is None:
            p.unlink(missing_ok=True)
        else:
            p.write_bytes(data)


def _module_verify(con, name):
    """Rebuild every member from the module file; True when all match and the span does."""
    rows = mod.members(con, name)
    results = _check_rows(con, rows)
    ok = all(results.get(r["addr"]) and results[r["addr"]][0].verdict.startswith("MATCH") for r in rows)
    if ok:
        for r in rows:
            _record(con, r, results[r["addr"]][0])
    return ok


def cmd_module(args):
    con = db.connect()
    im = image()
    if args.op == "list":
        for (name,) in con.execute("SELECT DISTINCT module FROM functions WHERE module IS NOT NULL ORDER BY module"):
            ms = mod.members(con, name)
            print(f"{name:<12} {len(ms):3} functions  {hexa(ms[0]['addr'])}-{hexa(ms[-1]['addr'] + ms[-1]['size'])}"
                  f"  {sum(m['size'] for m in ms)} bytes  [{ms[0]['flags'] or config.DEFAULT_FLAGS}]")
        return
    if args.op == "units":
        for lo, hi in mod.units(con, im):
            fs = con.execute("SELECT * FROM functions WHERE addr >= ? AND addr < ? ORDER BY addr", (lo, hi)).fetchall()
            flags = sorted({f["flags"] or config.DEFAULT_FLAGS for f in fs if f["kind"] == "c"})
            done = sum(f["status"] == "matched" for f in fs)
            mods = sorted({f["module"] for f in fs if f["module"]})
            print(f"{hexa(lo)}-{hexa(hi)}  {len(fs):3} functions, {done} matched"
                  + (f", modules {', '.join(mods)}" if mods else "") + f"  flags: {' | '.join(flags)}")
            if args.verbose:
                for f in fs:
                    print(f"    {hexa(f['addr'])} {fn_name(f):<10} {f['status']:<8} {f['module'] or ''}")
        return
    if args.op == "show":
        ms = mod.members(con, args.name)
        if not ms:
            raise SystemExit(f"no module {args.name}")
        for m in ms:
            print(f"{hexa(m['addr'])} {fn_name(m):<10} {m['size']:5} {m['status']}")
        _module_verify(con, args.name)
        return
    name = args.name.upper()
    if args.op in ("create", "add"):
        old = mod.members(con, name)
        if args.op == "create" and old:
            raise SystemExit(f"{name} exists; use `module add`")
        if args.op == "add" and not old:
            raise SystemExit(f"no module {name}; use `module create`")
        new = [db.resolve(con, f) for f in args.fns]
        rows = mod.validate(con, list(old) + new, name)
        texts = ((mod.split_asm if name.endswith(".ASM") else mod.split)(mod.path(name).read_text(), old) if old else {})
        stubs = _stb_lines(mod.path(name).with_suffix(".STB"))
        singles = []
        for r in new:
            p = src_path(r)
            if p.parent != config.FUNCS:
                raise SystemExit(f"{fn_name(r)}'s source is {p}, not a singleton in src/funcs")
            texts[r["addr"]] = p.read_text()
            stubs += _stb_lines(p.with_suffix(".STB"))
            singles += [p, p.with_suffix(".STB")]
        snap = _snapshot([mod.path(name), mod.path(name).with_suffix(".STB"), *singles])
        before = {r["addr"]: (r["src"], r["module"], r["flags"]) for r in rows}
        flags = args.flags or mod.unit_flags(rows)
        _module_write(con, name, rows, [texts[r["addr"]] for r in rows], stubs)
        mod.set_members(con, name, rows)
        for r in rows:
            db.update(con, r["addr"], flags=None if flags == config.DEFAULT_FLAGS else flags)
        for p in singles:
            p.unlink(missing_ok=True)
        if not _module_verify(con, name):
            _restore(snap)
            for a, (s, m, f) in before.items():
                db.update(con, a, src=s, module=m, flags=f)
            raise SystemExit(f"{name}: did not verify as a module; restored the previous sources")
        print(f"{name}: {len(rows)} functions, verified with {flags}")
        return
    if args.op == "dissolve":
        rows = mod.members(con, name)
        if not rows:
            raise SystemExit(f"no module {name}")
        text = mod.path(name).read_text()
        parts = (mod.split_asm if name.endswith(".ASM") else mod.split)(text, rows)
        stubs = _stb_lines(mod.path(name).with_suffix(".STB"))
        for r in rows:
            body = parts[r["addr"]]
            st = list(stubs)
            for o in ([] if name.endswith(".ASM") else rows):
                if o["addr"] != r["addr"] and re.search(rf"\b{fn_name(o)}\s*\(", body):
                    st.append(f"@{fn_name(o)}" if o["far"] else _placeholder_for(text, o))
            p = config.FUNCS / f"F_{r['addr']:05X}{mod.path(name).suffix}"
            p.write_text(body)
            if any(st):
                p.with_suffix(".STB").write_text("\n".join(s for s in st if s) + "\n")
            db.update(con, r["addr"], src=str(p.relative_to(config.ROOT)), module=None)
        mod.path(name).unlink()
        mod.path(name).with_suffix(".STB").unlink(missing_ok=True)
        print(f"{name}: dissolved into {len(rows)} singletons; run ./sr check on them")


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
        print(' '.join(f'{s:05X}' for s in a.order))
        if args.write:
            a.write_order()
            print('written to src/link/LAYOUT.JSON (link_order)')
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
    elif args.what == 'apply':
        if args.names == ['all']:
            units = [s for s in a.order if any(it.owner == s and it.kind not in ('lit',) for it in a.items)]
            if args.write:
                # ownership may have moved between files: start from sources without generated data
                for path in sorted((config.SRC / 'mods').glob('*.C')):
                    text = path.read_text(errors='replace')
                    clean = a._strip_c_definitions(text, [])
                    if clean != text:
                        path.write_text(clean)
        else:
            units = [int(n, 16) for n in args.names] if args.names else []
        for u in units:
            changed = a.apply_unit(u, write=args.write)
            for path, text in changed.items():
                print(f'{u:05X}: {path.relative_to(config.ROOT)}' + ('' if args.write else ' (dry run)'))
                if not args.write:
                    print(text)
    elif args.what == 'asm':
        n = a.write_dgroup_asm()
        print(f'src/data/DGROUP.ASM: {n} records not yet owned by a module')
    else:
        raise SystemExit(args.what)


def cmd_linkcheck(args):
    from sr.linkmatch import check
    raise SystemExit(check(args.executable, args.mapfile, args.output))


def cmd_build(args):
    if args.layout:
        from sr.linklayout import run as build
    else:
        from sr.build import build
    sys.exit(build(args.prepare_only, args.objects_only))


def main():
    p = argparse.ArgumentParser(prog="sr", description="Street Rod decompilation driver")
    sub = p.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("dgroup", help="recover DGROUP ownership from the data layout; write definitions")
    s.add_argument("what", choices=["report", "problems", "order", "register", "aliases", "apply", "asm"])
    s.add_argument("names", nargs="*", help="unit starts (hex) for apply; lo hi for report")
    s.add_argument("--write", action="store_true")
    s.set_defaults(func=cmd_dgroup)

    s = sub.add_parser("linkcheck", help="compare the linked MZ binary without masking any bytes")
    s.add_argument("executable", nargs="?", help="EXE to compare (default work/game/SR.EXE)")
    s.add_argument("--map", dest="mapfile", help="linker MAP for position/call diagnostics")
    s.add_argument("--output", help="JSON report path")
    s.set_defaults(func=cmd_linkcheck)

    s = sub.add_parser("build", help="compile and link the game with MSC 5.1 and MASM 5.1")
    s.add_argument("--prepare-only", action="store_true", help="write DOS sources, MAKEFILE, BUILD.BAT and LINK.RSP")
    s.add_argument("--objects-only", action="store_true", help="compile all real objects without linking")
    s.add_argument("--layout", action="store_true", help="build recovered original code/link layout into work/layout")
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

    s = sub.add_parser("new", help="create src/funcs/F_xxxxx.C (or .ASM) for a function and mark it wip")
    s.add_argument("fn")
    s.add_argument("--asm", action="store_true", help="hand-written function: create a MASM 5.1 .ASM")
    s.set_defaults(func=cmd_new)

    s = sub.add_parser("check", help="compile and byte-compare; records the result in the DB")
    s.add_argument("fn", nargs="?")
    s.add_argument("--all", action="store_true", help="re-check every matched function (regression gate)")
    s.add_argument("--flags", help="try these CL flags without recording the result")
    s.set_defaults(func=cmd_check)

    s = sub.add_parser("diff", help="two-column instruction diff, ours vs original")
    s.add_argument("fn")
    s.add_argument("--full", action="store_true")
    s.add_argument("--flags")
    s.set_defaults(func=cmd_diff)

    s = sub.add_parser("sweep", help="try many /O flag combinations in one DOSBox boot")
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

    s = sub.add_parser("flags", help="set per-function CL flags (no flags + --reset = default)")
    s.add_argument("fn")
    s.add_argument("flags", nargs="*")
    s.add_argument("--reset", action="store_true")
    s.add_argument("--force", action="store_true", help="allow changing a matched function's flags")
    s.set_defaults(func=cmd_flags)

    s = sub.add_parser("module", help="group matched functions into one source file, gated as a whole")
    s.add_argument("op", choices=["list", "units", "show", "create", "add", "dissolve"])
    s.add_argument("name", nargs="?", help="DOS 8.3 file name, e.g. GARAGE.C")
    s.add_argument("fns", nargs="*", help="functions to put in it (create/add)")
    s.add_argument("-v", "--verbose", action="store_true")
    s.add_argument("--flags", help="CL flags for the whole file (default: the union of the members' /O letters)")
    s.set_defaults(func=cmd_module)

    s = sub.add_parser("src", help="point a function at a different source file (e.g. a module)")
    s.add_argument("fn")
    s.add_argument("path", help="relative to the project root")
    s.set_defaults(func=cmd_src)

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

    s = sub.add_parser("headers", help="regenerate the per-file headers and the sources' include blocks")
    s.set_defaults(func=cmd_headers)

    s = sub.add_parser("status", help="summary + regenerate docs/PROGRESS.md")
    s.set_defaults(func=cmd_status)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
