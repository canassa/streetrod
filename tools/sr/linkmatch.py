"""Fail-closed comparison of the actual linked MZ file, without relocation masks."""
import json
from pathlib import Path
import re
import struct

from . import config, db
from .exe import Image


def map_publics(path):
    """Public names -> (frame paragraph, offset), retaining the linker's frame."""
    result = {}
    for line in path.read_text(errors='replace').splitlines():
        m = re.match(r'^\s*([0-9A-Fa-f]{4}):([0-9A-Fa-f]{4})\s+(\S+)', line)
        if m:
            result[m[3]] = (int(m[1], 16), int(m[2], 16))
    return result


def byte_diff(want, have):
    common = min(len(want), len(have))
    positions = [i for i in range(common) if want[i] != have[i]]
    return dict(expected_size=len(want), actual_size=len(have),
                different_bytes=len(positions), missing_bytes=max(0, len(want)-len(have)),
                extra_bytes=max(0, len(have)-len(want)), first_offsets=positions[:16],
                identical=want == have)


def compare(executable, mapfile=None):
    executable = Path(executable)
    target, built = Image(config.EXE), Image(executable)
    original_file = config.EXE.read_bytes()
    built_file = executable.read_bytes()
    report = dict(target=str(config.EXE.relative_to(config.ROOT)), executable=str(executable),
                  file=byte_diff(original_file, built_file),
                  header=byte_diff(original_file[:target.hdr], built_file[:built.hdr]),
                  load_image=byte_diff(target.img, built.img))
    expected_relocs = target.relocs
    actual_relocs = built.relocs
    report['relocations'] = dict(expected_count=len(expected_relocs), actual_count=len(actual_relocs),
        same_order=expected_relocs == actual_relocs,
        missing_positions=sorted(set(expected_relocs)-set(actual_relocs)),
        extra_positions=sorted(set(actual_relocs)-set(expected_relocs)))
    # Mapped comparisons diagnose moved code; they NEVER change the raw verdict.
    if mapfile is not None:
        publics = map_publics(Path(mapfile))
        con = db.connect()
        rows = list(con.execute("SELECT * FROM functions WHERE status='matched' ORDER BY addr"))
        con.close()
        functions = []
        calls = []
        # A static function is not in the MAP: place it by a public of its own link unit
        # (one object, so the distance between them is fixed).
        units = json.loads((config.SRC/'link/LAYOUT.JSON').read_text())['units']
        unit_of = {s: u['start'] for u in units for s in u['sources']}
        mapped = {}
        for row in rows:
            frame = publics.get('_' + (row['name'] or f"F_{row['addr']:05X}"))
            if frame is not None:
                mapped.setdefault(unit_of.get(row['src']), []).append((row['addr'], frame[0]*16+frame[1]))
        for row in rows:
            name = row['name'] or f"F_{row['addr']:05X}"
            symbol = '_' + name
            frame = publics.get(symbol)
            item = dict(addr=row['addr'], name=name, symbol=symbol)
            if frame is not None:
                pos = frame[0]*16+frame[1]
            elif mapped.get(unit_of.get(row['src'])):
                anchor, at = min(mapped[unit_of[row['src']]], key=lambda a: abs(a[0]-row['addr']))
                pos = at + row['addr'] - anchor
                item['static'] = True
            else:
                item['missing'] = True
                functions.append(item)
                continue
            size = row['size']
            expected = target.img[row['addr']:row['addr']+size]
            actual = built.img[pos:pos+size]
            item.update(linked_addr=pos, shift=pos-row['addr'], **byte_diff(expected, actual))
            functions.append(item)
            from .disasm import linear
            for ins in linear(target.img, row['addr'], row['addr']+size):
                if ins.size != 3 or ins.bytes[0] != 0xe8:
                    continue
                offset = ins.addr-row['addr']
                linked = pos+offset
                raw = built.img[linked:linked+3]
                dest = ins.addr+3+struct.unpack('<h', ins.bytes[1:])[0]
                calls.append(dict(addr=ins.addr, caller=row['addr'], target=dest,
                                  linked_addr=linked, expected=ins.bytes.hex(), actual=raw.hex(),
                                  identical=raw == ins.bytes))
        report['functions'] = functions
        report['near_calls'] = calls
        report['mapped_summary'] = dict(functions=len(functions),
            missing=sum(bool(f.get('missing')) for f in functions),
            position_exact=sum(f.get('shift') == 0 for f in functions),
            bytes_exact=sum(bool(f.get('identical')) for f in functions),
            near_calls=len(calls), near_calls_exact=sum(c['identical'] for c in calls))
    return report


def check(executable=None, mapfile=None, output=None):
    executable = Path(executable) if executable else config.WORK/'game/SR.EXE'
    mapfile = Path(mapfile) if mapfile else executable.with_suffix('.MAP')
    output = Path(output) if output else executable.parent/'BYTE-MATCH.JSON'
    report = compare(executable, mapfile if mapfile.exists() else None)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2)+'\n')
    for key in ('file', 'header', 'load_image'):
        v = report[key]
        print(f"{key}: {'IDENTICAL' if v['identical'] else 'DIFF'}; "
              f"{v['different_bytes']} differing, {v['missing_bytes']} missing, {v['extra_bytes']} extra bytes")
    v = report['relocations']
    print(f"MZ relocations: {v['actual_count']} vs {v['expected_count']}; "
          f"order {'identical' if v['same_order'] else 'different'}")
    if 'mapped_summary' in report:
        v = report['mapped_summary']
        print(f"Mapped game functions: {v['bytes_exact']}/{v['functions']} raw-byte exact, "
              f"{v['position_exact']}/{v['functions']} at original positions, {v['missing']} missing")
        print(f"Near-call encodings: {v['near_calls_exact']}/{v['near_calls']} exact")
    print(f'Unmasked comparison report: {output}')
    return 0 if report['file']['identical'] else 1
