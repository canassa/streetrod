"""Audit original ownership of compiler-emitted DATA/CONST in the layout build.

Forensics only: derives pool positions from original instructions and object
fixups. It never changes source, DB, objects or the linked executable.
"""
import collections
import json
from pathlib import Path
import re
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from sr import config
from sr.disasm import linear
from sr.exe import image
from sr.omf import Obj

im = image()
plan = json.loads((config.SRC/'link/LAYOUT.JSON').read_text())
report = []
float_checks = []
for unit in plan['units']:
    if unit['kind'] != 'c':
        continue
    obj = Obj(config.WORK/'layout'/f'U_{unit["start"]:05X}.OBJ')
    codebase = unit['start']-min(obj.publics.values())
    refs = collections.defaultdict(list)
    instructions = linear(im.img, unit['start'], unit['end'])
    for fix in obj.fixups:
        if (fix.seg != obj.code_seg or fix.loc not in (1, 5) or fix.self_rel
                or fix.kind != 'seg' or fix.index == obj.code_seg):
            continue
        original = im.word(codebase+fix.pos)
        offset = (int.from_bytes(obj.code[fix.pos:fix.pos+2], 'little')+fix.disp)&0xffff
        item = dict(addr=codebase+fix.pos, original_offset=original,
                    object_offset=offset, inferred_pool_base=(original-offset)&0xffff)
        refs[fix.index].append(item)
        if obj.segments[fix.index-1] == 'CONST':
            ins = next((ins for ins in instructions if ins.addr <= item['addr'] < ins.addr+ins.size), None)
            if ins and ins.mnemonic.startswith('emu_'):
                match = re.search(r'\b(qword|dword|word) ptr', ins.op_str)
                if match:
                    size = {'qword': 8, 'dword': 4, 'word': 2}[match[1]]
                    expected = im.img[config.DGROUP_SEG*16+original:config.DGROUP_SEG*16+original+size]
                    actual = bytes(obj.data[fix.index][offset:offset+size])
                    check = dict(**item, size=size, expected=expected.hex(), actual=actual.hex(),
                                 identical=expected == actual, instruction=ins.text())
                    float_checks.append(check)
    unit_report = dict(start=unit['start'], sources=unit['sources'], segments=[])
    for index, data in obj.data.items():
        if index == obj.code_seg or not data:
            continue
        bases = collections.Counter(r['inferred_pool_base'] for r in refs[index])
        unit_report['segments'].append(dict(index=index, name=obj.segments[index-1],
            size=len(data), bases=dict(bases), refs=refs[index]))
        print(f"{unit['start']:05X} {obj.segments[index-1]} {len(data):04X}: "
              + ', '.join(f'{base:04X} ({count} refs)' for base, count in bases.items()))
    report.append(unit_report)
output = config.WORK/'layout/DATA-OWNERSHIP.JSON'
output.write_text(json.dumps(dict(units=report, numeric_constants=float_checks), indent=2)+'\n')
wrong = [r for r in float_checks if not r['identical']]
print(f'Numeric constant operands: {len(float_checks)-len(wrong)}/{len(float_checks)} exact')
for check in wrong:
    print(f"WRONG {check['addr']:05X}: {check['instruction']}: {check['actual']} vs {check['expected']}")
print(output)
raise SystemExit(bool(wrong))
