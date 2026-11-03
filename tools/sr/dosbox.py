"""Compile C files with MSC 5.1 inside one headless DOSBox-X boot."""

import os
import shutil
import subprocess

from . import config, stb

_SET = ("quit warning=false", "core=dynamic_x86", "cycles=max", "pcspeaker=false",
        "mpu401=none", "serial1=disabled", "serial2=disabled", "parallel1=disabled")


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
    def log(self):
        p = self._find(".LOG", stem="CC")
        return p.read_text(errors="replace").replace("\r", "") if p else ""

    def _find(self, ext, stem=None):
        want = f"{stem or self.stem}{ext}".lower()
        for p in self.dir.iterdir():
            if p.name.lower() == want:
                return p
        return None


def compile_all(jobs, name="build"):
    """Compile every job in one DOSBox-X session. Output goes to work/<name>-<pid>/J<n>/,
    so parallel `sr` processes never share a build tree."""
    root = config.WORK / f"{name}-{os.getpid()}"
    if root.exists():
        shutil.rmtree(root)
    (root / ".home").mkdir(parents=True)
    (root / "TMP").mkdir()
    lines = ["@echo off"]
    for n, job in enumerate(jobs):
        job.dir = root / f"J{n:04d}"
        job.dir.mkdir()
        lines.append(f"cd \\J{n:04d}")
        if job.asm:
            shutil.copy2(job.src, job.dir / f"{job.stem}.ASM")
            lines.append(f"masm {job.flags} {job.stem}.ASM,{job.stem}.OBJ,{job.stem}.LST; > CC.LOG")
        else:
            # A sidecar F_xxxxx.STB holds placeholder definitions of same-module callees, so
            # a one-function build emits `push cs; call near` for them. It is prepended at
            # build time and never part of the reconstructed source.
            stub = job.src.with_suffix(".STB")
            text = (stb.expand(stub.read_text()) + "\n" if stub.exists() else "") + job.src.read_text()
            (job.dir / f"{job.stem}.C").write_text(text)
            # /W3 only adds diagnostics (C4071: call without a prototype); codegen is unaffected.
            lines.append(f"cl /c /W3 {job.flags} /Fc{job.stem}.COD {job.stem}.C > CC.LOG")
    (root / "RUN.BAT").write_text("\r\n".join(lines) + "\r\n")

    tc = config.TOOLCHAIN
    argv = ["dosbox-x", "-silent", "-fastlaunch", "-nogui", "-exit"]
    for s in _SET:
        argv += ["-set", s]
    argv += [
        "-c", f'mount c "{root}"',
        "-c", f'mount d "{tc}"',
        "-c", f'mount e "{config.INCLUDE}"',
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
