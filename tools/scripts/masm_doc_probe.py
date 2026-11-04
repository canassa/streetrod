"""Reproduce documentation-aid measurements with the project's MASM 5.1.

Run: nix-shell shell.nix --run 'python3 tools/scripts/masm_doc_probe.py'
Inputs and DOS outputs live under work/; no game source is modified.
"""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from sr import config, dosbox
from sr.omf import Obj

PREFIX = '.MODEL MEDIUM\n.CODE S1234_TEXT\n'


def main():
    root = config.WORK / 'masm-doc-inputs'
    cases = {
        'base': ('', 'mov ax,2\nret'),
        'comments': ('; banner\n', 'mov ax,2 ; value\n\nret ; return'),
        'equ': ('VALUE EQU 2\n', 'mov ax,VALUE\nret'),
        'stack_raw': ('', 'mov ax,ss:[bp+6]\nret'),
        'stack_equ': ('arg EQU WORD PTR ss:[bp+6]\n', 'mov ax,arg\nret'),
        'field_raw': ('', 'mov ax,es:[di+2]\nret'),
        'field_dot': ('PAIR STRUC\nfirst DW ?\nsecond DW ?\nPAIR ENDS\n', 'mov ax,es:[di].second\nret'),
        'field_qualified': ('PAIR STRUC\nfirst DW ?\nsecond DW ?\nPAIR ENDS\n', 'mov ax,es:[di+PAIR.second]\nret'),
        'record_raw': ('', 'test al,80h\nmov cl,7\nret'),
        'record': ('BITS RECORD highbit:1,lowbits:7\n', 'test al,MASK highbit\nmov cl,highbit\nret'),
        'repeat_raw': ('', 'inc ax\ninc ax\nret'),
        'rept': ('', 'REPT 2\ninc ax\nENDM\nret'),
        'macro': ('STEP MACRO\ninc ax\nENDM\n', 'STEP\nSTEP\nret'),
        'macro_local': ('STEP MACRO\nLOCAL done\njmp short done\ndone:\nENDM\n', 'STEP\nSTEP\nret'),
        'macro_raw': ('', 'jmp short a\na:\njmp short b\nb:\nret'),
        'near': ('', 'mov ax,2\nret'),
        'label': ('', 'alias LABEL WORD\nmov ax,2\nret'),
        'listing': ('TITLE Documentation probe\nSUBTTL Scratch\nPAGE 60,80\n', 'mov ax,2\nret'),
        'locals': ('LOCALS\n', 'mov ax,2\nret'),
        'at_named': ('', 'jmp short @@done\n@@done:\nret'),
        'at_anonymous': ('', 'jmp short @F\n@@:\nret'),
        'proc_arg': ('', 'mov ax,arg\nret'),
        'proc_cfar': ('', 'mov ax,arg\nret'),
        'proc_model': ('', 'mov ax,arg\nret'),
        'proc_cuses': ('', 'mov ax,2\nret'),
        'proc_local': ('', 'LOCAL temp:WORD\nmov ax,2\nret'),
        'local_model': ('', 'LOCAL temp:WORD\nmov ax,temp\nret'),
        'uses_model': ('', 'mov ax,2\nret'),
        'proc_uses': ('', 'mov ax,2\nret'),
        'forward_equ': ('', 'mov ax,VALUE\nVALUE EQU 2\nret'),
    }
    jobs = []
    for name, (defs, body) in cases.items():
        folder = root / name
        folder.mkdir(parents=True, exist_ok=True)
        proc = 'NEAR' if name == 'near' else 'FAR'
        if name == 'proc_arg':
            proc += ' C arg:WORD'
        if name == 'proc_cfar':
            proc = 'C FAR arg:WORD'
        if name == 'proc_model':
            proc = 'FAR arg:WORD'
        if name == 'proc_cuses':
            proc = 'C USES si di'
        if name == 'uses_model':
            proc = 'FAR USES si di'
        if name == 'proc_uses':
            proc += ' USES si di'
        path = folder / 'PROBE.ASM'
        prefix = PREFIX.replace('.MODEL MEDIUM', '.MODEL MEDIUM,C') if name in ('proc_model', 'local_model', 'uses_model') else PREFIX
        path.write_text(prefix + defs + '_probe PROC ' + proc + '\n' + body + '\n_probe ENDP\nEND\n')
        jobs.append(dosbox.Job(path, '/Ml', name))
    dosbox.compile_all(jobs, 'masm-doc')
    results = {}
    for job in jobs:
        ok = '0 Severe' in job.log and job.obj is not None
        listing = job.lst.read_text(errors='replace') if job.lst else ''
        diagnostics = [line.strip() for line in listing.splitlines() if ': warning ' in line or ': error ' in line]
        results[job.tag] = {'ok': ok, 'diagnostics': diagnostics, 'log': job.log, 'obj_dir': str(job.dir)}
        if ok:
            results[job.tag]['code'] = bytes(Obj(job.obj).code).hex()
    pairs = [('base', n) for n in ('comments', 'equ', 'label', 'listing', 'near', 'forward_equ')]
    pairs += [('stack_raw', 'stack_equ'), ('field_raw', 'field_dot'), ('field_raw', 'field_qualified'),
              ('record_raw', 'record'), ('repeat_raw', 'rept'), ('repeat_raw', 'macro'), ('macro_raw', 'macro_local')]
    for left, right in pairs:
        a, b = jobs[list(cases).index(left)], jobs[list(cases).index(right)]
        if results[left]['ok'] and results[right]['ok']:
            results[right]['obj_identical_to_' + left] = a.obj.read_bytes() == b.obj.read_bytes()
    report = config.WORK / 'masm-doc-results.json'
    report.write_text(json.dumps(results, indent=2) + '\n')
    for name, result in results.items():
        print(name, {k: v for k, v in result.items() if k not in ('log', 'obj_dir')})
        if not result['ok']:
            print(result['log'])
    print(report)


if __name__ == '__main__':
    main()
