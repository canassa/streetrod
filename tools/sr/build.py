"""Prepare and run a real MSC 5.1/MASM 5.1 compile/link, without check-only stubs."""
import json
import os
from pathlib import Path
import re
import resource
import shutil
import subprocess

from . import config, db, headers

OUT = config.WORK / 'game'

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

def link(root, options=('/NOD', '/MAP')):
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


def code_segment(addr):
    # The original's four game code regions; near interfaces must share CS.
    return ('G0_TEXT' if addr < 0xF38A else 'G1_TEXT' if addr < 0x21960
            else 'G2_TEXT' if addr < 0x26458 else 'G3_TEXT')


def header_deps(text):
    """MAKEFILE dependencies: the quoted headers a C source includes."""
    return ''.join(' INCLUDE\\' + h for h in dict.fromkeys(re.findall(r'^#include\s+"(\w+\.H)"', text, re.M)))


def near_declarations(path):
    """Retain typed near interfaces from check scaffolding, without its bodies.

    A split source can call a real near function in another object. Dropping its
    declaration silently changes that call to far (and breaks its RET contract).
    """
    stub = path.with_suffix('.STB')
    if not stub.exists():
        return ''
    declarations = re.findall(
        r'^(?:static\s+)([^{};]+\bnear\b[^{};]*\([^{};]*\))\s*\{[^{}]*\}',
        stub.read_text(), re.M)
    if not declarations:
        return ''
    return '#include "STRUCTS.H"\n' + '\n'.join(d.strip()+';' for d in declarations) + '\n'


def prepare():
    con = db.connect()
    headers.generate(con)
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'TMP').mkdir(exist_ok=True)
    (OUT / 'INCLUDE').mkdir(exist_ok=True)
    for p in config.INCLUDE.glob('*.H'):
        shutil.copyfile(p, OUT / 'INCLUDE' / p.name)
    for p in (config.SRC / 'data').glob('*.H'):
        shutil.copyfile(p, OUT / 'INCLUDE' / p.name)
    rows = con.execute("SELECT * FROM functions WHERE status='matched' ORDER BY addr").fetchall()
    con.close()
    units = {}
    for row in rows:
        units.setdefault(row['src'], []).append(row)
    jobs = []
    for source, members in units.items():
        path = config.ROOT / source
        text = path.read_text()
        seg = code_segment(members[0]['addr'])
        if any(code_segment(r['addr']) != seg for r in members):
            raise SystemExit(f'{source}: crosses a code segment boundary')
        if path.suffix == '.C':
            text = near_declarations(path) + text
            # Split compilation groups share real near interfaces. No .STB code
            # enters the executable; expose near definitions across object files.
            text = re.sub(r'\bstatic\s+(?=[^;{}\n]*\bnear\b)', '', text)
            flags = members[0]['flags'] or config.DEFAULT_FLAGS
            program = 'CL'
            args = ['/c', '/W3', *flags.split(), '/NT' + seg, path.name]
        else:
            text = re.sub(r'(?im)^\s*\.CODE\s*$', '        .CODE ' + seg, text)
            flags = members[0]['flags'] or config.DEFAULT_ASM_FLAGS
            program = 'MASM'
            args = [*flags.split(), f'{path.name},{path.stem}.OBJ,{path.stem}.LST;']
        (OUT / path.name).write_text(text)
        jobs.append(dict(stem=path.stem, source=path.name, program=program, args=args))
    # Optional reconstructed storage/data objects belong to the link, never to
    # check-only placeholder sidecars. Missing definitions remain linker errors.
    data_dir = config.SRC / 'data'
    if data_dir.exists():
        for path in sorted(data_dir.glob('*.ASM')):
            shutil.copyfile(path, OUT / path.name)
            jobs.insert(0, dict(stem=path.stem, source=path.name, program='MASM',
                               args=['/Ml', f'{path.name},{path.stem}.OBJ,{path.stem}.LST;']))
    response = '+\n'.join(j['stem'] + '.OBJ' for j in jobs)
    response += '\nSR.EXE\nSR.MAP\nD:\\LIB\\MLIBCR.LIB+D:\\LIB\\MLIBFP.LIB+D:\\LIB\\LIBH.LIB+D:\\LIB\\EM.LIB\n;\n'
    (OUT / 'LINK.RSP').write_bytes(response.replace('\n', '\r\n').encode())
    bat = ['@echo off', 'if exist SR.EXE del SR.EXE', 'set PATH=D:\\BIN;F:\\BIN',
           'set INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE', 'set LIB=D:\\LIB', 'set TMP=C:\\TMP']
    # MAKE 4.07 is a sequential dependency processor, unlike NMAKE/GNU make.
    # Compile rules must precede the executable rule; its input is positional.
    make = ['# MSC 5.1 MAKE: set the environment with ENV.BAT, then MAKE MAKEFILE.', '']
    for j in jobs:
        cmd = j['program'] + ' ' + ' '.join(j['args'])
        bat += [cmd, 'if errorlevel 1 goto failed']
        deps = header_deps((OUT / j["source"]).read_text(errors="replace")) if j["program"] == "CL" else ""
        make += [f"{j['stem']}.OBJ: {j['source']}{deps}", '\t' + cmd, '']
    make += ['SR.EXE: ' + ' \\\n    '.join(j['stem'] + '.OBJ' for j in jobs) + ' LINK.RSP',
             '\tLINK /NOD /MAP @LINK.RSP', '']
    env = ['@echo off', 'set PATH=D:\\BIN;F:\\BIN',
           'set INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE', 'set LIB=D:\\LIB', 'set TMP=C:\\TMP']
    (OUT / 'ENV.BAT').write_bytes(('\r\n'.join(env) + '\r\n').encode())
    bat += ['LINK /NOD /MAP @LINK.RSP', 'if errorlevel 1 goto failed', 'goto end', ':failed', 'if exist SR.EXE del SR.EXE',
            'echo BUILD FAILED - inspect compiler/linker diagnostics', ':end']
    (OUT / 'BUILD.BAT').write_bytes(('\r\n'.join(bat) + '\r\n').encode())
    (OUT / 'MAKEFILE').write_bytes(('\n'.join(make)).replace('\n', '\r\n').encode())
    (OUT / 'BUILD.JSON').write_text(json.dumps(jobs, indent=2))
    return jobs

def build(prepare_only=False, objects_only=False):
    jobs = prepare()
    print(f'Prepared {len(jobs)} real source objects in {OUT.relative_to(config.ROOT)}', flush=True)
    if prepare_only:
        return 0
    # A failed rebuild must not leave an older executable looking successful.
    for name in ('SR.EXE', 'SR.MAP'):
        (OUT / name).unlink(missing_ok=True)
    def compile_job(j):
        obj = OUT / (j['stem'] + '.OBJ')
        obj.unlink(missing_ok=True)
        rc = run_dos(OUT, j['program'], j['args'], OUT / (j['stem'] + '.LOG'))
        return j, rc == 0 and obj.exists()
    # CL uses fixed intermediate filenames: this shared DOS directory is serial,
    # just like BUILD.BAT and the native MAKEFILE.
    results = [compile_job(j) for j in jobs]
    failures = [j for j, ok in results if not ok]
    print(f'{len(jobs)-len(failures)}/{len(jobs)} objects compiled', flush=True)
    if failures:
        for j in failures:
            print((OUT / (j['stem'] + '.LOG')).read_text(errors='replace')[-1800:])
        return 1
    if objects_only:
        return 0
    ok = link(OUT)
    link_report(OUT)
    if not ok or not (OUT / 'SR.EXE').exists():
        (OUT / 'SR.EXE').unlink(missing_ok=True)
        return 1
    print('Built work/game/SR.EXE (linked build; not a byte-match claim)')
    return 0
