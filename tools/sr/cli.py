"""sr — the Street Rod decompilation driver. Run `./sr -h`."""

import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from sr import cod, config, dosbox, layout, source  # noqa: E402
from sr import module as mod  # noqa: E402
from sr.disasm import branch_target, linear  # noqa: E402
from sr.exe import image  # noqa: E402
from sr.match import compare, diff_rows  # noqa: E402
from sr.omf import Obj  # noqa: E402
from sr.lint import lint  # noqa: E402
from sr.refs import verify  # noqa: E402
from sr.scan import far_call_operand, scan  # noqa: E402

SWEEP_OPTS = ["/Od", "/Os", "/Ot", "/Oa", "/Oas", "/Oat", "/Ol", "/Ols", "/Olt", "/Oal", "/Oals",
              "/Oalt", "/Oi", "/Ois", "/Oit", "/Oais", "/Oait", "/Oils", "/Oilt", "/Oails", "/Oailt",
              "/Ox", "/O"]


def hexa(a):
    return f"0x{a:05x}"


fn_name = source.fname


# ---------------------------------------------------------------- scan / list / show


def cmd_scan(args):
    """Rediscover the functions in the EXE and compare them with what the sources define."""
    rows = {r["addr"]: r for r in scan()}
    game = {r["addr"]: r for r in source.game_functions()}
    runtime = [r for r in rows.values() if r["kind"] == "crt"]
    print(f"{len(rows)} functions in the EXE: {len(runtime)} runtime, {len(rows) - len(runtime)} game; "
          f"the sources define {len(game)}")
    extra = sorted(a for a, r in rows.items() if r["kind"] != "crt" and a not in game)
    missing = sorted(a for a in game if a not in rows)
    sized = sorted(a for a in game if a in rows and rows[a]["size"] != game[a]["size"])
    if extra:
        print(f"no source defines {len(extra)}: " + ", ".join(hexa(a) for a in extra[:20]))
    if missing:
        print(f"the scanner does not find {len(missing)}: " + ", ".join(fn_name(game[a]) for a in missing[:20]))
    for a in sized:
        print(f"{fn_name(game[a])}: {game[a]['size']} bytes in symbols.json, the scanner finds {rows[a]['size']}")
    if extra or missing or sized:
        sys.exit(1)


def cmd_list(args):
    rows = source.functions()
    if args.kind:
        rows = [r for r in rows if r["kind"] == args.kind]
    if args.seg:
        rows = [r for r in rows if r["seg"] == int(args.seg, 16)]
    if args.name:
        rows = [r for r in rows if args.name.lower() in fn_name(r).lower()]
    if args.max:
        rows = [r for r in rows if r["size"] <= args.max]
    if args.range:
        lo, hi = (int(x, 16) for x in args.range.split("-"))
        rows = [r for r in rows if lo <= r["addr"] <= hi]
    if args.by_size:
        rows = sorted(rows, key=lambda r: r["size"])
    print(f"{'addr':8} {'seg:off':10} {'size':>5} kind far name / src")
    for r in rows[: args.limit]:
        extra = fn_name(r) + (f"  {r['src']}" if r["src"] else "")
        print(f"{hexa(r['addr'])}  {r['seg']:04x}:{r['off']:04x} {r['size']:5} {r['kind']:4} {'F' if r['far'] else 'N':3} {extra}")
    if len(rows) > args.limit:
        print(f"... {len(rows) - args.limit} more (use --limit)")


def annotate(names, im, row, ins):
    notes = []
    t = far_call_operand(im, ins)
    if t is not None:
        notes.append(source.label(t))
    elif ins.bytes[0] == 0xE8 and ins.size == 3:
        notes.append(source.label(branch_target(ins)))
    if ins.mnemonic in ("call", "lcall", "jmp", "ljmp") or ins.mnemonic.startswith("j"):
        return "  ; " + ", ".join(notes) if notes else ""
    for m in re.finditer(r"0x([0-9a-f]{3,4})\b", ins.op_str):
        if re.search(r"\[[^\]]*\b[bs]p\b[^\]]*$", ins.op_str[: m.start()]):
            continue  # a frame offset, [bp - 0x110], not a DGROUP address
        v = int(m.group(1), 16)
        g = names.get(v)
        if g:
            # An immediate equal to a global's offset is only a guess: it may be a plain number.
            notes.append(g["name"] + ("" if "[" in ins.op_str or ins.mnemonic == "lea" else "?"))
        dpos = config.DGROUP_SEG * 16 + v
        if "[" in ins.op_str and dpos in im.reloc_pos:
            seg = im.word(dpos)
            far = [f for f in source.fardata() if f["seg"] == seg]
            notes.append(f"segment constant {seg:04x}: far data" + (f" ({', '.join(f['name'] for f in far)})" if far else ""))
            continue
        s = im.dgroup_string(v)
        if s is not None and ("[" not in ins.op_str or ins.mnemonic == "lea"):
            mid = im.img[config.DGROUP_SEG * 16 + v - 1] != 0
            notes.append(repr(s) + (" (inside a string: maybe just a number)" if mid else ""))
    return "  ; " + ", ".join(notes) if notes else ""


def cmd_show(args):
    im = image()
    row = source.resolve(args.fn)
    names = source.globals_()
    print(f"{fn_name(row)} @ {row['seg']:04x}:{row['off']:04x} (linear {hexa(row['addr'])}), {row['size']} bytes, "
          f"{'far' if row['far'] else 'near'}, kind {row['kind']}")
    if row["src"]:
        f = mod.file_of(row)
        print(f"  src: {row['src']} ({'CL' if f.kind == 'c' else 'MASM'} {f.command})")
    elif row.get("module"):
        print(f"  MSC 5.1 library module {row['module']}")
    calls = source.callees()
    callers = sorted(a for a, cs in calls.items() if row["addr"] in cs)
    if callers:
        print("  called by: " + ", ".join(source.label(c) for c in callers))
    if calls.get(row["addr"]):
        print("  calls:     " + ", ".join(source.label(c) for c in calls[row["addr"]]))
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
        print(f"  {ins.addr:05x}  {raw:<14}{text:<40}{annotate(names, im, row, ins)}")


# ---------------------------------------------------------------- authoring / checking


def _file_jobs(rows, flags_override=None):
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


def _check_rows(rows, flags_override=None, quiet=False):
    """Compile the files that define `rows` (once each), compare every function asked for,
    and run each file's gate over all its functions."""
    im = image()
    jobs = _file_jobs(rows, flags_override)
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
        res.refs = verify(obj, proc, im, r["addr"], r, procs)
        if not job.asm:
            res.lint = lint(job.src.read_text(errors="replace"), job.log)
        results[r["addr"]] = (res, proc, job)
    _file_gates(im, jobs, results)
    if not quiet:
        for job in jobs.values():
            shown = [ln for ln in job.log.splitlines() if re.search(r"warning C(?!4071|4016|4100)\d+|error", ln)]
            if shown:
                print(f"{job.src.name}:\n  " + "\n  ".join(shown))
        for r in rows:
            if results.get(r["addr"]):
                print(f"{hexa(r['addr'])} {fn_name(r)}: {results[r['addr']][0].line()}")
    return results


def _rows_for(what):
    """A function by address or name, or every function of a source file (BOBS.C)."""
    f = mod.find_file(what)
    if f is not None:
        return mod.members(f.rel)
    row = source.resolve(what)
    if row["src"] is None:
        raise SystemExit(f"{fn_name(row)} is a runtime function: no source defines it")
    return [row]


def cmd_check(args):
    if args.all:
        rows = source.game_functions()
    elif args.fn:
        rows = _rows_for(args.fn)
    else:
        raise SystemExit("give a function (address or name), a source file, or --all")
    results = _check_rows(rows, args.flags)
    bad = 0
    for r in rows:
        res = results.get(r["addr"])
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
    if mod.find_file(args.fn) is not None:
        raise SystemExit("give a function (address or name)")
    row = _rows_for(args.fn)[0]
    results = _check_rows([row], args.flags)
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
    row = _rows_for(args.fn)[0]
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
    """The runtime functions' names: the MSC 5.1 library modules located in the EXE."""
    rt = source.runtime()
    rows = rt["functions"]
    unnamed = [r for r in rows if r["name"] is None]
    print(f"{len(rows)} runtime functions, {len(rows) - len(unnamed)} named by their library module, "
          f"{sum(map(len, rt['aliases'].values()))} aliases, {len(rt['data'])} library data publics placed in DGROUP")
    if args.verbose:
        for r in rows:
            also = rt["aliases"].get(r["addr"])
            print(f"  {hexa(r['addr'])} {fn_name(r):<14} {r['module'] or '(no library match)'}"
                  + (f"  also {', '.join(also)}" if also else ""))


def cmd_rename(args):
    from sr import rename
    if args.file:
        pairs = rename.parse_pairs(Path(args.file).read_text().splitlines())
    elif args.old and args.new:
        pairs = [(args.old, args.new)]
    else:
        raise SystemExit("give OLD NEW, or --file PAIRS")
    for line in rename.rename(pairs, dry=args.dry_run):
        print(line)


def cmd_fit(args):
    from sr import rename
    for line in rename.fit(args.old, args.candidates):
        print(line)


def cmd_flags(args):
    """A source file's command line (one per file: its block in src/MAKEFILE)."""
    f = mod.find_file(args.fn, mod.sources()) or mod.file_of(_rows_for(args.fn)[0])
    if not args.flags:
        print(f"{f.path.name}: flags = {f.flags}" + (f" (compiled with /NT{f.segment})" if f.kind == "c" and f.segment else ""))
        return
    flags = " ".join(args.flags)
    matched = mod.members(f.rel)
    if matched and flags != f.flags and not args.force:
        raise SystemExit(f"{f.path.name} has {len(matched)} matched functions built with {f.flags}; "
                         "changing them may break them (--force, then ./sr check " + f.path.name + ")")
    from sr import makefile
    makefile.set_flags(f.rule.stem, flags)
    print(f"{f.path.name}: flags = {flags} (src/MAKEFILE)")


def cmd_data(args):
    im = image()
    names = {off: g["name"] for off, g in source.globals_().items()}
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


def cmd_format(args):
    from sr import format as formatter
    import subprocess
    targets = args.files or formatter.default_targets()
    try:
        drift = formatter.format_files(targets, check=args.check)
    except (ValueError, OSError, subprocess.CalledProcessError) as exc:
        sys.exit(str(exc))
    for path in drift:
        print(f"would format {path}")
    if drift:
        sys.exit(f"{len(drift)} file(s) need formatting")
    print(f"{'checked' if args.check else 'formatted'} {len(targets)} file(s)")


def cmd_headers(args):
    from sr import headers
    if args.check:
        drift, problems = headers.check()
        for p in sorted(drift):
            print("would rewrite " + str(p.relative_to(config.ROOT)))
        for p in problems:
            print(p)
        if drift or problems:
            sys.exit(f"{len(drift)} files differ from what ./sr headers writes, {len(problems)} header problems")
        print("the include blocks and the MAKEFILE dependencies are up to date; every header declares what its file defines")
        return
    problems = headers.generate()
    for p in problems:
        print(p)
    print("wrote the include blocks of the C sources and the header dependencies in src/MAKEFILE")
    if problems:
        sys.exit(1)


# ---------------------------------------------------------------- source files


def _file_gates(im, jobs, results):
    """Run the whole-span gate once per compiled file, over all its functions."""
    for job in jobs.values():
        if job.obj is None or (job.cod is None and not job.asm):
            continue
        f = job.file
        obj = Obj(job.obj)
        ms = mod.members(f.rel)
        if not ms:
            continue
        lengths = cod.asm_lengths(job.lst) if job.asm else None
        procs = ([cod.from_publics(obj, fn_name(m), lengths) for m in ms] if job.asm else cod.parse(job.cod))
        if not job.asm:
            obj.statics = cod.statics(job.cod)
        g = mod.gate(obj, [p for p in procs if p is not None], im.img, ms, data=True)
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
    im = image()
    srcs = {f.rel: f for f in mod.files()}
    every = source.functions()
    for lo, hi in mod.units(im):
        fs = [f for f in every if lo <= f["addr"] < hi]
        files = sorted({f["src"] for f in fs if f["src"]})
        flags = sorted({srcs[s].flags for s in files if s in srcs})
        print(f"{hexa(lo)}-{hexa(hi)}  {len(fs):3} functions"
              + (f", files {', '.join(Path(s).name for s in files)}" if files else "") + f"  flags: {' | '.join(flags)}")
        if args.verbose:
            for f in fs:
                print(f"    {hexa(f['addr'])} {fn_name(f):<10} {f['src'] or ''}")


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


def cmd_pic(args):
    from sr.pics import run
    run(args)


def main():
    p = argparse.ArgumentParser(prog="sr", description="Street Rod decompilation driver")
    sub = p.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("pic", help="decode resource pictures as PNG or inline kitty graphics")
    s.add_argument("ids", nargs="*", type=lambda n: int(n, 0), help="picture ids (decimal or 0x hex)")
    s.add_argument("--list", action="store_true", help="list ids, dimensions, packed sizes and libraries")
    s.add_argument("--png", type=Path, help="write a PNG; multiple pictures are stacked vertically")
    s.add_argument("--scale", type=int, default=1, help="positive integer nearest-neighbour scale")
    s.add_argument("--cells", type=int, nargs=2, metavar=("COLS", "N"),
                   help="cut N cells from COLS columns and two rows, then lay them in one row")
    s.add_argument("--mode", choices=("ega", "cga"), default="ega", help="resource variant (default ega)")
    s.add_argument("--palette", choices=("game", "intro", "standard", "cga-green"), default="game",
                   help="EGA game/intro/standard palette; CGA defaults to bright cyan/magenta/white")
    s.set_defaults(func=cmd_pic)

    s = sub.add_parser("dgroup", help="recover DGROUP ownership from the data layout; check the sources against it")
    s.add_argument("what", choices=["report", "problems", "order", "check"],
                   help="check: every initialized global is defined in the file that owned it (exit 1 if not)")
    s.add_argument("names", nargs="*", help="lo hi for report")
    s.set_defaults(func=cmd_dgroup)

    s = sub.add_parser("linkcheck", help="compare the linked MZ binary without masking any bytes")
    s.add_argument("executable", nargs="?", help="EXE to compare (default work/layout/SR.EXE)")
    s.add_argument("--map", dest="mapfile", help="linker MAP for position/call diagnostics")
    s.add_argument("--output", help="JSON report path")
    s.set_defaults(func=cmd_linkcheck)

    s = sub.add_parser("build", help="run src/MAKEFILE in work/layout (MSC 5.1, MASM 5.1, LIB 3.11, LINK 5.01), then verify against orig/")
    s.add_argument("--prepare-only", action="store_true", help="only copy src/ (MAKEFILE, SR.LNK, sources, headers) to work/layout")
    s.add_argument("--objects-only", action="store_true", help="compile (and gate) every object without linking")
    s.add_argument("--no-verify", action="store_true", help="only build: no object gates, no comparison (needs no orig/)")
    s.set_defaults(func=cmd_build)

    s = sub.add_parser("scan", help="rediscover the functions in the EXE, compare them with the sources' (exit 1 if they differ)")
    s.set_defaults(func=cmd_scan)

    s = sub.add_parser("crtnames", help="the runtime functions' names, from the MSC 5.1 library modules located in the EXE")
    s.add_argument("-v", "--verbose", action="store_true", help="list them")
    s.set_defaults(func=cmd_crtnames)

    s = sub.add_parser("list", help="list functions")
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

    s = sub.add_parser("check", help="compile the function's whole source file, byte-compare, gate the file")
    s.add_argument("fn", nargs="?", help="address, name, or a source file (BOBS.C: all its functions)")
    s.add_argument("--all", action="store_true", help="compile every file once, check every matched function (regression gate)")
    s.add_argument("--flags", help="try these CL/MASM flags for the file")
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

    s = sub.add_parser("rename", help="rename a function, global or source file everywhere (sources, headers, "
                       "MAKEFILE, SR.LNK, docs, tools), then ./sr headers")
    s.add_argument("old", nargs="?", help="current name (maybe_carhop, g_hercules) or source file (M_xxxxx.C)")
    s.add_argument("new", nargs="?")
    s.add_argument("--file", help="a file of OLD NEW lines (# comments) applied at once")
    s.add_argument("-n", "--dry-run", action="store_true", help="report what would change, write nothing")
    s.set_defaults(func=cmd_rename)

    s = sub.add_parser("fit", help="would these names keep a communal/static global at its address "
                       "(LINK bucket / MSC static key window)?")
    s.add_argument("old", help="the global's current name")
    s.add_argument("candidates", nargs="*")
    s.set_defaults(func=cmd_fit)

    s = sub.add_parser("flags", help="a source file's CL/MASM flags (no flags: print them)")
    s.add_argument("fn", help="a source file (BOBS.C) or a function in it")
    s.add_argument("flags", nargs="*")
    s.add_argument("--force", action="store_true", help="allow changing the flags of a file with matched functions")
    s.set_defaults(func=cmd_flags)

    s = sub.add_parser("module", help="candidate same-file spans from near-call evidence")
    s.add_argument("op", choices=["units"])
    s.add_argument("-v", "--verbose", action="store_true")
    s.set_defaults(func=cmd_module)

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

    s = sub.add_parser("format", help="format C sources and headers with the pinned clang-format style")
    s.add_argument("files", nargs="*", type=Path, help="default: every .C/.H under src/; never assembly")
    s.add_argument("--check", action="store_true", help="write nothing; exit 1 if formatting differs")
    s.set_defaults(func=cmd_format)

    s = sub.add_parser("headers", help="rewrite the sources' include blocks and the MAKEFILE dependencies from the headers")
    s.add_argument("--check", action="store_true", help="write nothing; exit 1 if a run would change a file or a header is wrong")
    s.set_defaults(func=cmd_headers)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
