"""Run the jobs prepared by dosbox.compile_all under emu2 instead of DOSBox-X.

emu2 (github.com/dmsc/emu2) is a user-space 8086 + DOS-call emulator: no BIOS, no boot,
host directories are the DOS drives. The MSC 5.1 passes are plain real-mode DOS programs
that only use int 21h, so they run under it unchanged; CL's spawn of C1/C2/C3 becomes a
child emu2 process. One CL run costs ~25 ms instead of a ~1.5 s DOSBox-X boot, and the
jobs run in parallel (one emu2 per job, each with its own TMP).

The drives are the same as in DOSBox-X (C: work root, D: MSC 5.1, E: src/include,
F: MASM 5.1), the current directory is C:\\Jnnnn, and only DOS stdout goes to CC.LOG,
exactly like `cl ... > CC.LOG` in RUN.BAT, so OBJ, COD, LST and CC.LOG are the same bytes
(tools/scripts/emucmp.py checks that). Every run is bounded by `timeout` and RLIMIT_FSIZE.
"""

import os
import resource
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor

from . import config

_FSIZE = 64 << 20  # no file a compile writes comes near this; it stops a runaway log
_TIMEOUT = "60"


def _limit():
    resource.setrlimit(resource.RLIMIT_FSIZE, (_FSIZE, _FSIZE))


def workers(n):
    want = int(os.environ.get("SR_JOBS", "0")) or min(16, os.cpu_count() or 1)
    return max(1, min(want, n))


def _run_one(root, n, job, exe):
    tmp = root / "TMP" / f"J{n:04d}"
    tmp.mkdir(parents=True, exist_ok=True)
    if job.asm:
        prog, dos = config.MASM / "BIN" / "MASM.EXE", "F:\\BIN\\MASM.EXE"
        args = [*job.flags.split(), f"{job.stem}.ASM,{job.stem}.OBJ,{job.stem}.LST;"]
    else:
        prog, dos = config.TOOLCHAIN / "BIN" / "CL.EXE", "D:\\BIN\\CL.EXE"
        args = ["/c", "/W3", *job.flags.split(), f"/Fc{job.stem}.COD", f"{job.stem}.C"]
    env = {
        **{k: os.environ[k] for k in ("PATH", "TZ", "TZDIR") if k in os.environ},  # TZ: LST dates
        "EMU2_DRIVE_C": str(root), "EMU2_DRIVE_D": str(config.TOOLCHAIN),
        "EMU2_DRIVE_E": str(config.INCLUDE), "EMU2_DRIVE_F": str(config.MASM),
        "EMU2_DEFAULT_DRIVE": "C", "EMU2_CWD": f"\\J{n:04d}", "EMU2_PROGNAME": dos,
    }  # fmt: skip
    dosenv = ["PATH=D:\\BIN;F:\\BIN", "INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE", "LIB=D:\\LIB",
              f"TMP=C:\\TMP\\J{n:04d}"]
    with open(job.dir / "CC.LOG", "wb") as out, open(root / "TMP" / f"J{n:04d}.err", "wb") as err:
        subprocess.run(["timeout", _TIMEOUT, exe, str(prog), *args, "--", *dosenv],
                       cwd=job.dir, env=env, stdin=subprocess.DEVNULL, stdout=out, stderr=err,
                       preexec_fn=_limit, check=False)


def run(root, jobs):
    exe = shutil.which("emu2")
    if exe is None:
        raise SystemExit("emu2 not found: run ./sr (it enters shell.nix), or set SR_EMU=dosbox")
    with ThreadPoolExecutor(workers(len(jobs))) as pool:
        for f in [pool.submit(_run_one, root, n, job, exe) for n, job in enumerate(jobs)]:
            f.result()
