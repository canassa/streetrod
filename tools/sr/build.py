"""Run the DOS build tools: CL, MASM and LIB under emu2, LINK under DOSBox-X."""
import json
import os
import re
import resource
import subprocess

from . import config, db


def _limit():
    resource.setrlimit(resource.RLIMIT_FSIZE, (64 << 20, 64 << 20))

def run_dos(root, program, args, log):
    """A bounded invocation with the same DOS drives as the generated BUILD.BAT."""
    drive = 'F' if program == 'MASM' else 'D'
    exe = (config.MASM if drive == 'F' else config.TOOLCHAIN) / 'BIN' / (program + '.EXE')
    env = {**os.environ, 'EMU2_DRIVE_C': str(root), 'EMU2_DRIVE_D': str(config.TOOLCHAIN),
           'EMU2_DRIVE_E': str(root / 'INCLUDE'), 'EMU2_DRIVE_F': str(config.MASM),
           'EMU2_DEFAULT_DRIVE': 'C', 'EMU2_CWD': '\\',
           'EMU2_PROGNAME': f'{drive}:\\BIN\\{program}.EXE'}
    dosenv = ['PATH=D:\\BIN;F:\\BIN', 'INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE',
              'LIB=D:\\LIB', 'TMP=C:\\TMP']
    with log.open('wb') as f:
        p = subprocess.run(['timeout', '90', 'emu2', str(exe), *args, '--', *dosenv],
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

def link(root, options):
    """LINK 5.01 needs DOSBox-X: emu2 crashes while resolving these libraries."""
    for name in ('LINK.STA', 'LINK.LOG', 'SR.EXE', 'SR.MAP'):
        (root / name).unlink(missing_ok=True)
    batch = ('@echo off\nset LIB=D:\\LIB\n'
             f'D:\\BIN\\LINK {" ".join(options)} @LINK.RSP > LINK.LOG\n'
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
    text = (root / 'LINK.LOG').read_text(errors='replace') if (root / 'LINK.LOG').exists() else ''
    missing = sorted(set(re.findall(r'^([^ \r\n]+) in file\(s\):', text, re.M)))
    con = db.connect()
    declarations = {'_' + r['name']: dict(r) for r in con.execute('SELECT * FROM globals')}
    declarations.update({'_' + r['name']: dict(r) for r in con.execute('SELECT * FROM fardata')})
    con.close()
    errors = re.findall(r'^.*(?:fatal error|error|warning) L[0-9]+.*$', text, re.M)
    report = dict(diagnostics=errors, missing_symbols=missing, data_definitions=[declarations[n] for n in missing if n in declarations],
                  other_symbols=[n for n in missing if n not in declarations])
    (root / 'LINK.JSON').write_text(json.dumps(report, indent=2))
    print(f"Link: {len(missing)} unresolved symbols ({len(report['data_definitions'])} game data definitions)")
    if errors:
        print(f'Link diagnostics: {len(errors)} errors/warnings')
        for error in list(dict.fromkeys(errors))[:12]:
            print(error.strip())
    if report['other_symbols']:
        print('Other missing symbols: ' + ', '.join(report['other_symbols']))
    print(f'Full diagnostics: {root}/LINK.LOG; data inventory: {root}/LINK.JSON')
    return report


def header_deps(text):
    """MAKEFILE dependencies: the quoted headers a C source includes."""
    return ''.join(' INCLUDE\\' + h for h in dict.fromkeys(re.findall(r'^#include\s+"(\w+\.H)"', text, re.M)))
