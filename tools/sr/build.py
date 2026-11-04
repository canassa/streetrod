"""Run the DOS build tools: CL, MASM and LIB under emu2, LINK under DOSBox-X."""
import json
import os
import re
import resource
import subprocess

from . import config


def _limit():
    resource.setrlimit(resource.RLIMIT_FSIZE, (64 << 20, 64 << 20))

# The DOS side of the build: C: the build directory (a copy of src/), D: MSC 5.1, F: MASM 5.1,
# and the environment MSC SETUP sets up for them. The MAKEFILE needs nothing else.
DOS_ENV = ('PATH=D:\\BIN;F:\\BIN', 'INCLUDE=D:\\INCLUDE;F:\\INCLUDE', 'LIB=D:\\LIB', 'TMP=C:\\TMP')


def run_dos(root, program, args, log):
    """Run one MAKEFILE command (CL, MASM or LIB) under emu2 in `root` (C:\\), bounded."""
    drive = 'F' if program == 'MASM' else 'D'
    exe = (config.MASM if drive == 'F' else config.TOOLCHAIN) / 'BIN' / (program + '.EXE')
    env = {**os.environ, 'EMU2_DRIVE_C': str(root), 'EMU2_DRIVE_D': str(config.TOOLCHAIN),
           'EMU2_DRIVE_F': str(config.MASM), 'EMU2_DEFAULT_DRIVE': 'C', 'EMU2_CWD': '\\',
           'EMU2_PROGNAME': f'{drive}:\\BIN\\{program}.EXE'}
    with log.open('wb') as f:
        p = subprocess.run(['timeout', '90', 'emu2', str(exe), *args, '--', *DOS_ENV],
                           cwd=root, env=env, stdin=subprocess.DEVNULL, stdout=f,
                           stderr=subprocess.STDOUT, preexec_fn=_limit)
    # emu2 creates DOS outputs with lowercase host filenames. Normalize them for
    # the exported DOS build tree and make host-side existence checks unambiguous.
    for path in root.iterdir():
        if path.is_file() and path.suffix.lower() in ('.obj', '.lst', '.exe', '.map', '.lib'):
            target = path.with_name(path.name.upper())
            if path != target:
                path.replace(target)
    return p.returncode

def link(root, command):
    """Run the MAKEFILE's LINK command in `root` under DOSBox-X (LINK 5.01 crashes under
    emu2 while resolving these libraries)."""
    for name in ('LINK.STA', 'LINK.LOG', 'SR.EXE', 'SR.MAP'):
        (root / name).unlink(missing_ok=True)
    batch = ('@echo off\n' + ''.join(f'set {v}\n' for v in DOS_ENV)
             + f'{command} > LINK.LOG\n'
             'if errorlevel 1 goto bad\necho OK> LINK.STA\ngoto done\n'
             ':bad\necho FAIL> LINK.STA\n:done\nexit\n')
    (root / 'DOSLINK.BAT').write_bytes(batch.replace('\n', '\r\n').encode())
    argv = ['timeout', '90', 'dosbox-x', '-silent', '-fastlaunch', '-nogui', '-exit',
            '-set', 'quit warning=false', '-set', 'core=dynamic_x86', '-set', 'cycles=max',
            '-c', f'mount c "{root}"', '-c', f'mount d "{config.TOOLCHAIN}"',
            '-c', 'c:', '-c', 'DOSLINK.BAT']
    with (root / 'DOSBOX.LOG').open('wb') as out:
        result = subprocess.run(argv, stdout=out, stderr=subprocess.STDOUT,
                                env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, preexec_fn=_limit)
    for path in root.iterdir():
        if path.is_file() and path.suffix.lower() in ('.log', '.sta', '.exe', '.map'):
            target = path.with_name(path.name.upper())
            if path != target:
                path.replace(target)
    status = root / 'LINK.STA'
    return result.returncode == 0 and status.exists() and status.read_text().strip() == 'OK'


def link_report(root):
    """LINK's unresolved symbols and diagnostics from LINK.LOG (also written to LINK.JSON)."""
    text = (root / 'LINK.LOG').read_text(errors='replace') if (root / 'LINK.LOG').exists() else ''
    missing = sorted(set(re.findall(r'^([^ \r\n]+) in file\(s\):', text, re.M)))
    errors = re.findall(r'^.*(?:fatal error|error|warning) L[0-9]+.*$', text, re.M)
    report = dict(diagnostics=errors, missing_symbols=missing)
    (root / 'LINK.JSON').write_text(json.dumps(report, indent=2))
    print(f'Link: {len(missing)} unresolved symbols')
    if missing:
        print('Missing symbols: ' + ', '.join(missing))
    if errors:
        print(f'Link diagnostics: {len(errors)} errors/warnings')
        for error in list(dict.fromkeys(errors))[:12]:
            print(error.strip())
    print(f'Full diagnostics: {root}/LINK.LOG')
    return report

