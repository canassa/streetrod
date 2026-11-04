"""emucmp.py [--quick] [--keep] : prove the emu2 backend byte-identical to DOSBox-X.

Compiles the same jobs under both backends and compares every file each job leaves behind
(OBJ, COD, LST, CC.LOG) byte for byte. Job sets:
  - every src/ source, with its command line (src/MAKEFILE) and a spread of other
    flag sets (/Ol, /Oi, /Ox, /Od, other models)
  - the 46 `./sr sweep` combos on a few sources
  - hand-written probes: floating point constants, errors and warnings, MASM
Sources and headers are snapshot first (other agents edit them meanwhile).
Run it through nix-shell: nix-shell shell.nix --run 'python3 tools/scripts/emucmp.py'.
"""

import argparse
import re
import shutil
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from sr import cli, config, dosbox, module  # noqa: E402

FLAGSETS = ["/AM /Oas /Gs /FPi", "/AM /Ol /Gs /FPi", "/AM /Oails /Gs /FPi", "/AM /Oi /Gs /FPi",
            "/AM /Ox /Gs /FPi", "/AM /Od /FPi", "/AL /Oalt /Gs /FPi", "/AS /Ot /FPi87", "/AM /Oas /Zi /Gs /FPi"]

PROBES = {
    "TFLT.C": """double g = 1.0 / 3.0; float h = 2.5e-3f * 4;
double qqfn(double x, float y) { long n = (long)(x * 1.7e10); return x * 3.14159 + y / 7.0 + n + 1e300 * 1e-299; }
""",
    "TERR.C": """int qqfn(int a) { int b; char *p = a; undeclared(b); return a +; }\n""",
    "TWARN.C": """int qqfn(int a, long l) { int b; char c = l; if (a = 3) return c; }\n""",
    "TBIG.C": "".join(f"int f{i}(int *p, int n) {{ int s = 0, j; for (j = 0; j < n; j++) "
                      f"{{ s += p[j] * {i} + (s >> 3); if (s & {i + 1}) s ^= p[n - j - 1]; }} return s; }}\n"
                      for i in range(150)),
    "TSW.C": """int qqfn(int k, char *s) { switch (k) { case 1: return s[0]; case 2: case 3: return 7;
case 10: return s[k]; case 100: return -1; case 1000: return 0x55; default: return k * 3; } }
""",
    "TASM.ASM": """        .MODEL  MEDIUM
        .DATA
tab     DW      1, 2, 3, 4
msg     DB      'hello', 0
        .CODE
        EXTRN   _far_callee:FAR
        PUBLIC  _qqasm
_qqasm  PROC    FAR
        push    bp
        mov     bp,sp
        mov     bx,[bp+6]
        shl     bx,1
        mov     ax,tab[bx]
        call    _far_callee
        mov     sp,bp
        pop     bp
        ret
_qqasm  ENDP
        END
""",
    "TASMERR.ASM": "        .MODEL SMALL\n        .CODE\nfoo PROC\n  mov ax,bogus\n  movx bx,1\nfoo ENDP\n",
}


def snapshot(dest):
    """Snapshot the source tree so both backends see the same files."""
    shutil.copytree(config.SRC, dest)
    return sorted(p for p in dest.rglob('*') if p.suffix in ('.C', '.ASM')), dest


# The two things that legitimately differ, both MASM-only and not code: the LST page header
# carries the wall-clock time of the run, and MASM's "Bytes symbol space free" statistic
# reports free memory (DOSBox-X keeps COMMAND.COM, RUN.BAT and EMS around; emu2 gives MASM
# the whole 640K). They are reported as `benign`, never hidden.
_BENIGN = [(re.compile(rb"(Microsoft \(R\) Macro Assembler Version 5\.10 +)\d+/\d+/\d+ \d+:\d+:\d+"), rb"\1DATE"),
           (re.compile(rb"\d+ \+ \d+ Bytes symbol space free"), rb"N + N Bytes symbol space free")]


def benign(data):
    for rx, rep in _BENIGN:
        data = rx.sub(rep, data)
    return data


def files(job):
    if job.dir is None or not job.dir.exists():
        return {}
    return {p.name.upper(): p.read_bytes() for p in job.dir.iterdir() if p.is_file()}


def run_backend(emu, specs, label, chunk):
    """Build fresh Job objects for (src, flags) specs and compile them; DOSBox in chunks run
    concurrently (separate work roots), emu2 in one call (it parallelises itself)."""
    jobs = [dosbox.Job(s, f, (s, f)) for s, f in specs]
    t = time.monotonic()
    if emu == "emu2":
        dosbox.compile_all(jobs, name=f"emucmp-{label}-emu2", emu="emu2")
    else:
        parts = [jobs[i:i + chunk] for i in range(0, len(jobs), chunk)]
        with ThreadPoolExecutor(len(parts)) as pool:
            list(pool.map(lambda kv: dosbox.compile_all(kv[1], name=f"emucmp-{label}-dbx{kv[0]}", emu="dosbox"),
                          enumerate(parts)))
    return jobs, time.monotonic() - t


def compare(label, specs, include, chunk=40):
    config.SRC = include
    a, ta = run_backend("dosbox", specs, label, chunk)
    b, tb = run_backend("emu2", specs, label, chunk)
    bad = soft = 0
    kinds = {}
    for ja, jb in zip(a, b):
        fa, fb = files(ja), files(jb)
        for k in fa:
            kinds[k.rsplit(".", 1)[-1]] = kinds.get(k.rsplit(".", 1)[-1], 0) + 1
        if fa == fb:
            continue
        diff = sorted(k for k in set(fa) | set(fb) if fa.get(k) != fb.get(k))
        real = [k for k in diff if k not in fa or k not in fb or benign(fa[k]) != benign(fb[k])
                or not (k.endswith(".LST") or k == "CC.LOG") or not ja.asm]
        if real:
            bad += 1
            print(f"  DIFF {ja.src.name} {ja.flags}: {real}")
        else:
            soft += 1
    nobj = sum(1 for j in a if j.obj is not None)
    print(f"{label}: {len(specs)} jobs ({nobj} with an OBJ; files {kinds}), {bad} differ, "
          f"{soft} MASM jobs differ only in the LST time / free-memory line; "
          f"dosbox {ta:.1f}s (chunks of {chunk}), emu2 {tb:.1f}s")
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--quick", action="store_true", help="each file's own flags only, no flag spread")
    ap.add_argument("--keep", action="store_true", help="keep work/emucmp-* trees")
    args = ap.parse_args()
    base = config.WORK / "emucmp-snap"
    shutil.rmtree(base, ignore_errors=True)
    # each file's command line (its src/MAKEFILE block)
    command = {f.path.name.upper(): f.command for f in module.sources()}
    srcs, inc = snapshot(base / "mc")
    bad = 0

    def own(p):
        return command.get(p.name.upper())

    specs = []
    for p in srcs:
        if own(p) is None:
            raise SystemExit(f"{p.name}: not compiled by src/MAKEFILE")
        fl = {own(p)}
        if not args.quick and p.suffix == ".C":
            fl |= set(FLAGSETS)
        specs += [(p, f) for f in sorted(fl)]
    bad += compare("mc-sources", specs, inc)

    some = [p for p in srcs if p.suffix == ".C"][:3]
    sweep = [(p, f"/AM /FPi {o}{g}") for p in some for o in cli.SWEEP_OPTS for g in ("", " /Gs")]
    bad += compare("sweep-combos", sweep, inc)

    pd = base / "probes"
    pd.mkdir()
    probes = []
    for name, text in PROBES.items():
        (pd / name).write_text(text.replace("\n", "\r\n"))
        fl = ["/Ml", "/Mx /Zi"] if name.endswith(".ASM") else FLAGSETS
        probes += [(pd / name, f) for f in fl]
    bad += compare("probes", probes, inc)

    if not args.keep:
        for p in config.WORK.glob("emucmp-*"):
            shutil.rmtree(p, ignore_errors=True)
    print("IDENTICAL" if bad == 0 else f"{bad} job(s) differ")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
