"""Build the recovered linked layout from source and the untouched MSC libraries.

LAYOUT.JSON records recovered compilation groups and link inputs, not binary bytes.
Normal preparation/compilation never reads the original executable. The comparison
and unit gates read it only to verify compiler/linker output.
"""
import concurrent.futures
import json
from pathlib import Path
import re
import shutil

from . import build, cod, config, db, headers, module
from .exe import image
from .omf import Obj

OUT = config.WORK/'layout'
PLAN = config.SRC/'link/LAYOUT.JSON'


# MSC 5.1 SETUP's combined medium-model emulator library. The objects' default-library
# record names it, so LINK pulls the runtime from it without /NOD or a library field.
# LIB keeps all 402 component modules in this order, and LINK extracts modules in
# library order: other component orders move runtime code (5381..12426 bytes differ).
LIBRARY = 'MLIBCE.LIB'
COMPONENTS = ('MLIBCR.LIB', 'LIBH.LIB', 'EM.LIB', 'MLIBFP.LIB')
LIB_ARGS = [LIBRARY, ''.join('+D:\\LIB\\'+name for name in COMPONENTS)+';']
LINK_OPTIONS = ('/MAP', '/STACK:2560', '/EXEPACK')


def build_library():
    """Build MLIBCE.LIB with LIB 3.11 from the untouched component libraries."""
    root = OUT/'BUILD'/'MLIBCE'
    root.mkdir(parents=True, exist_ok=True)
    for path in root.iterdir():
        if path.is_file():
            path.unlink()
    rc = build.run_dos(root, 'LIB', LIB_ARGS, root/'LIB.LOG')
    if rc or not (root/LIBRARY).exists():
        print(f'{LIBRARY}: LIB failed\n'+(root/'LIB.LOG').read_text(errors='replace')[-2000:])
        return False
    shutil.copyfile(root/LIBRARY, OUT/LIBRARY)
    return True


def prepare():
    plan = json.loads(PLAN.read_text())
    con = db.connect()
    headers.generate(con)
    rows = [dict(r) for r in con.execute("SELECT * FROM functions WHERE status='matched' ORDER BY addr")]
    con.close()
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT/'INCLUDE').mkdir(exist_ok=True)
    for directory in (config.INCLUDE, config.SRC/'data'):
        for path in directory.glob('*.H'):
            shutil.copyfile(path, OUT/'INCLUDE'/path.name)
    covered = []
    jobs = []
    sequence = []
    def assembly(path, filename=None):
        filename = filename or path.name
        shutil.copyfile(path, OUT/filename)
        stem = Path(filename).stem
        jobs.append(dict(stem=stem, source=filename, program='MASM',
                         args=['/Ml', f'{filename},{stem}.OBJ,{stem}.LST;']))
        return stem+'.OBJ'
    order = plan.get('link_order')
    units = plan['units'] + [dict(d, kind='data') for d in plan.get('data_units', [])]
    if order:
        by = {u['start']: u for u in units}
        units = [by[s] for s in order if s in by] + [u for u in units if u['start'] not in set(order)]
    regions = sorted(plan['code_starts'])
    first_seen = set()
    for unit in units:
        if unit['kind'] == 'data' and unit['sources'][0].upper().endswith('.ASM'):
            sequence.append(assembly(config.ROOT / unit['sources'][0], f"D_{unit['lo']:05X}.ASM"))
            continue
        if unit['kind'] == 'data':
            # a data-only translation unit: compiled, linked at its place, no code gate
            src = config.ROOT / unit['sources'][0]
            stem = f"D_{unit['lo']:05X}"
            filename = stem + '.C'
            (OUT/filename).write_text('#include "CRT.H"\n' + src.read_text())
            jobs.append(dict(stem=stem, source=filename, program='CL',
                             args=['/c', '/W3', *config.DEFAULT_FLAGS.split(), '/Fc', filename]))
            sequence.append(stem + '.OBJ')
            continue
        start, end = unit['start'], unit['end']
        members = [r for r in rows if start <= r['addr'] < end]
        if not members or list(dict.fromkeys(r['src'] for r in members)) != unit['sources']:
            raise ValueError(f'Layout source membership changed: {start:05X}; review LAYOUT.JSON')
        if start != members[0]['addr'] or end != members[-1]['addr']+members[-1]['size']:
            raise ValueError(f'Layout extent changed: {start:05X}')
        covered.extend(r['addr'] for r in members)
        seg = 'S'+f"{max(s for s in plan['code_starts'] if s <= start)//16:04X}"+'_TEXT'
        stem = f'U_{start:05X}'
        flags = module.unit_flags(members)
        if unit['kind'] == 'c':
            parts = [(config.ROOT/source).read_text() for source in unit['sources']]
            text = ('#include "CRT.H"\n'
                    + ''.join(build.near_declarations(config.ROOT/s) for s in unit['sources'])
                    + module.merge(parts))
            text = re.sub(r'\bstatic\s+(?=[^;{}\n]*\bnear\b)', '', text)
            if start <= 0x66f < end:
                text = '#define F_0066F main\n'+text
            filename = stem+'.C'
            program = 'CL'
            args = ['/c', '/W3', *flags.split(), '/NT'+seg, '/Fc', filename]
        else:
            texts = [(config.ROOT/s).read_text() for s in unit['sources']]
            firsts = [next(r for r in members if r['src'] == s) for s in unit['sources']]
            text = module.merge_asm(texts, firsts)
            text = re.sub(r'(?im)^\s*\.CODE\s*$', '        .CODE '+seg, text)
            filename = stem+'.ASM'
            program = 'MASM'
            args = [*flags.split(), f'{filename},{stem}.OBJ,{stem}.LST;']
        (OUT/filename).write_text(text)
        jobs.append(dict(stem=stem, source=filename, program=program, args=args, members=members))
        region = max(s for s in regions if s <= start)
        if region not in first_seen:
            first_seen.add(region)
            if region == 0xf38a:
                sequence += [assembly(config.SRC/'data'/f) for f in ('FONTDATA.ASM', 'CGAFONT.ASM')]
            if region == 0x21962:
                sequence.append(assembly(config.SRC/'link/CRTORDER.ASM'))
        sequence.append(stem+'.OBJ')
    if sorted(covered) != sorted(r['addr'] for r in rows):
        raise ValueError('Layout must compile every matched game function exactly once')
    for name in ('SCREENS', 'TRACKPTS', 'SCRATCH', 'SONG', 'EFFECTS', 'SCREEN2', 'PARTS1', 'PARTS2'):
        sequence.append(assembly(config.SRC/'data'/(name+'.ASM')))
    # SLOTS is far communal storage (src/data/SLOTS.C): LINK places FAR_BSS after the
    # library objects, so the emulator follows its first reference inside the runtime
    slots = 'SLOTS.OBJ'
    (OUT/'SLOTS.C').write_text((config.SRC/'data/SLOTS.C').read_text())
    jobs.append(dict(stem='SLOTS', source='SLOTS.C', program='CL',
                     args=['/c', '/W3', *config.DEFAULT_FLAGS.split(), 'SLOTS.C']))
    sequence.append(slots)
    sequence.append(assembly(config.SRC/'data/DGROUP.ASM'))
    next(job for job in jobs if job['stem'] == 'DGROUP')['args'].insert(0, '/DORIGINAL_LAYOUT')
    # DS:6E60..8F02 is communal storage (C tentative definitions in the game files): LINK
    # creates c_common after crt0's XOB/XO/XOE and allocates it by symbol-hash bucket
    response = '+\n'.join(sequence)+'\nSR.EXE\nSR.MAP\n\n;\n'
    (OUT/'LINK.RSP').write_bytes(response.replace('\n', '\r\n').encode())
    env = ['@echo off', 'set PATH=D:\\BIN;F:\\BIN', 'set INCLUDE=E:\\;D:\\INCLUDE;F:\\INCLUDE',
           'set LIB=D:\\LIB', 'set TMP=C:\\TMP']
    (OUT/'ENV.BAT').write_bytes(('\r\n'.join(env)+'\r\n').encode())
    (OUT/'TMP').mkdir(exist_ok=True)
    make = ['# MSC MAKE 4.07: ENV.BAT, then MAKE MAKEFILE.', '']
    batch = ['@echo off', 'if exist SR.EXE del SR.EXE']
    # MAKE 4.07 runs rules in file order: the library comes before the link
    command = 'LIB '+' '.join(LIB_ARGS)
    make += [f'{LIBRARY}: '+' '.join('D:\\LIB\\'+name for name in COMPONENTS), '\t'+command, '']
    batch += [f'if exist {LIBRARY} del {LIBRARY}', command, 'if errorlevel 1 goto failed']
    for job in jobs:
        command = job['program']+' '+' '.join(job['args'])
        deps = build.header_deps((OUT/job['source']).read_text(errors='replace')) if job['program'] == 'CL' else ''
        make += [f"{job['stem']}.OBJ: {job['source']}{deps}", '\t'+command, '']
        batch += [command, 'if errorlevel 1 goto failed']
    command = 'LINK '+' '.join(LINK_OPTIONS)+' @LINK.RSP'
    make += ['SR.EXE: '+' \\\n    '.join(sequence)+f' {LIBRARY} LINK.RSP', '\t'+command, '']
    batch += [command, 'if errorlevel 1 goto failed', 'goto done', ':failed',
              'if exist SR.EXE del SR.EXE', 'echo BUILD FAILED', ':done']
    (OUT/'MAKEFILE').write_bytes(('\n'.join(make)).replace('\n','\r\n').encode())
    (OUT/'BUILD.BAT').write_bytes(('\r\n'.join(batch)+'\r\n').encode())
    (OUT/'BUILD.JSON').write_text(json.dumps(jobs, indent=2)+'\n')
    return jobs


def run(prepare_only=False, objects_only=False):
    jobs = prepare()
    print(f'Prepared {len(jobs)} source jobs and the {LIBRARY} rule in work/layout', flush=True)
    if prepare_only:
        return 0
    for name in ('SR.EXE', 'SR.MAP'):
        (OUT/name).unlink(missing_ok=True)
    original = image()
    def compile_one(job):
        root = OUT/'BUILD'/job['stem']
        root.mkdir(parents=True, exist_ok=True)
        (root/'TMP').mkdir(exist_ok=True)
        shutil.copytree(OUT/'INCLUDE', root/'INCLUDE', dirs_exist_ok=True)
        shutil.copyfile(OUT/job['source'], root/job['source'])
        objpath = root/(job['stem']+'.OBJ')
        objpath.unlink(missing_ok=True)
        rc = build.run_dos(root, job['program'], job['args'], root/'CC.LOG')
        if rc or not objpath.exists():
            return job['stem'], (root/'CC.LOG').read_text(errors='replace')[-2000:]
        if 'members' in job:
            obj = Obj(objpath)
            members = job['members']
            if job['program'] == 'CL':
                procs = cod.parse(root/(job['stem'].lower()+'.cod'))
                obj.statics = cod.statics(root/(job['stem'].lower()+'.cod'))
                members = [{**r, 'name': 'main'} if r['addr'] == 0x66f else r for r in members]
            else:
                procs = [cod.from_publics(obj, module.fname(r)) for r in members]
            gate = module.gate(obj, procs, original.img, members, con=db.connect())
            if not gate.ok:
                return job['stem'], gate.line(job['stem'])
        shutil.copyfile(objpath, OUT/objpath.name)
        return job['stem'], None
    with concurrent.futures.ThreadPoolExecutor(max_workers=12) as pool:
        library = pool.submit(build_library)
        results = list(pool.map(compile_one, jobs))
    if not library.result():
        return 1
    failures = [(name, error) for name, error in results if error]
    print(f'{len(results)-len(failures)}/{len(results)} compiled and gated source jobs', flush=True)
    if failures:
        for name, error in failures:
            print(name+': '+error)
        return 1
    if objects_only:
        return 0
    # the shipped file is EXEPACK-packed by LINK: build it first, then the unpacked file
    # the diagnostics read
    if build.link(OUT, options=LINK_OPTIONS):
        (OUT/'SR.EXE').replace(OUT/'SRPACKED.EXE')
    ok = build.link(OUT, options=tuple(o for o in LINK_OPTIONS if o != '/EXEPACK'))
    build.link_report(OUT)
    if not ok:
        (OUT/'SR.EXE').unlink(missing_ok=True)
        return 1
    from .linkmatch import check
    # A successful build remains usable while the unmasked comparison is red.
    # linkcheck itself fails until the entire file is identical.
    check(OUT/'SR.EXE')
    packed, shipped = OUT/'SRPACKED.EXE', config.EXE_PACKED
    if packed.exists():
        a, b = packed.read_bytes(), shipped.read_bytes()
        diff = sum(x != y for x, y in zip(a, b)) + abs(len(a) - len(b))
        print(f'Packed EXE vs {shipped.relative_to(config.ROOT)}: '
              + ('IDENTICAL' if diff == 0 else f'{diff} differing bytes (size {len(a)} vs {len(b)})'))
    return 0
