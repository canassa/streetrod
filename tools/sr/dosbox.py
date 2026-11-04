"""Compile C (CL) and MASM jobs with MSC 5.1 / MASM 5.1 under a DOS emulator.

`compile_all` lays every job out in work/<name>-<pid>/Jnnnn/ (safe in parallel) and runs
them with the backend SR_EMU selects:

- `emu2` (default): one emu2 process per job, run in parallel (tools/sr/emu2.py). No boot.
- `dosbox`: one headless DOSBox-X boot runs RUN.BAT over all jobs (the original backend).

Both produce byte-identical OBJ, COD, LST and CC.LOG (tools/scripts/emucmp.py).
"""

import os
import shutil
import subprocess

from . import config, emu2

_SET = ("quit warning=false", "core=dynamic_x86", "cycles=max", "pcspeaker=false",
        "mpu401=none", "serial1=disabled", "serial2=disabled", "parallel1=disabled")
BACKENDS = ("emu2", "dosbox")


class Job:
    def __init__(self, src, flags, tag):
        self.src = src
        self.flags = flags
        self.tag = tag
        self.stem = src.stem.upper()
        self.asm = src.suffix.upper() == ".ASM"
        self.dir = None

    @property
    def obj(self):
        return self._find(".OBJ")

    @property
    def cod(self):
        return self._find(".COD")

    @property
    def lst(self):
        return self._find(".LST")

    @property
    def log(self):
        p = self._find(".LOG", stem="CC")
        return p.read_text(errors="replace").replace("\r", "") if p else ""

    def _find(self, ext, stem=None):
        want = f"{stem or self.stem}{ext}".lower()
        for p in self.dir.iterdir():
            if p.name.lower() == want:
                return p
        return None


def backend():
    emu = os.environ.get("SR_EMU", "emu2").lower()
    if emu not in BACKENDS:
        raise SystemExit(f"SR_EMU={emu!r}: one of {', '.join(BACKENDS)}")
    return emu


def compile_all(jobs, name="build", emu=None):
    """Compile every job. Output goes to work/<name>-<pid>/J<n>/, so parallel `sr`
    processes never share a build tree."""
    emu = emu or backend()
    root = config.WORK / f"{name}-{os.getpid()}"
    if root.exists():
        shutil.rmtree(root)
    (root / ".home").mkdir(parents=True)
    (root / "TMP").mkdir()
    config.stage_headers(root / "HEADERS", flat=True)
    lines = ["@echo off"]
    for n, job in enumerate(jobs):
        job.dir = root / f"J{n:04d}"
        job.dir.mkdir()
        lines.append(f"cd \\J{n:04d}")
        if job.asm:
            shutil.copy2(job.src, job.dir / f"{job.stem}.ASM")
            lines.append(f"masm {job.flags} {job.stem}.ASM,{job.stem}.OBJ,{job.stem}.LST; > CC.LOG")
        else:
            # the source exactly as it is in git
            shutil.copy2(job.src, job.dir / f"{job.stem}.C")
            # /W3 only adds diagnostics (C4071: call without a prototype); codegen is unaffected.
            lines.append(f"cl /c /W3 {job.flags} /Fc{job.stem}.COD {job.stem}.C > CC.LOG")
    if emu == "emu2":
        emu2.run(root, jobs)
        return jobs
    (root / "RUN.BAT").write_text("\r\n".join(lines) + "\r\n")

    tc = config.TOOLCHAIN
    argv = ["dosbox-x", "-silent", "-fastlaunch", "-nogui", "-exit"]
    for s in _SET:
        argv += ["-set", s]
    argv += [
        "-c", f'mount c "{root}"',
        "-c", f'mount d "{tc}"',
        "-c", f'mount e "{root / "HEADERS"}"',
        "-c", f'mount f "{config.MASM}"',
        "-c", "set PATH=D:\\BIN;F:\\BIN",
        "-c", "set INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE",
        "-c", "set LIB=D:\\LIB",
        "-c", "set TMP=C:\\TMP",
        "-c", "c:",
        "-c", "RUN.BAT",
        "-c", "exit",
    ]  # fmt: skip
    env = {**os.environ, "SDL_VIDEODRIVER": "dummy", "HOME": str(root / ".home"),
           "XDG_CONFIG_HOME": str(root / ".home" / ".config")}
    env.pop("DISPLAY", None)
    with open(root / "dosbox.log", "wb") as log:
        subprocess.run(["timeout", str(30 + 5 * len(jobs)), *argv], stdout=log,
                       stderr=subprocess.STDOUT, env=env, check=False)
    return jobs
