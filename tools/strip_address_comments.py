"""Remove decompilation address annotations from src/; preserve code and prose.

Run: nix-shell shell.nix --run 'python3 tools/strip_address_comments.py'
Use --extract to merge surviving annotations into tools/sr/symbols.json (without
stripping). Run extraction before stripping a tree merged from annotated branches.
Stripping validates all annotations against the metadata before writing any sources.
Use --check to report remaining annotations without writing.
"""
import argparse
import json
import os
from pathlib import Path
import re
import sys
import textwrap

_FUNCTION = r'\w+ @ [0-9a-f]{4}:[0-9a-f]{4} \(linear 0x[0-9a-f]+, \d+ bytes\)'
_ADDRESS = r'(?:DS|[0-9a-f]{4}):[0-9a-f]{4}'
_C_TOKEN = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/', re.S)
_ASM_TOKEN = re.compile(r'\'(?:\'\'|[^\'\n])*\'|"(?:""|[^"\n])*"|;[^\n]*')


def strip_text(text, asm=False):
    count = 0

    def replace(m):
        nonlocal count
        token = m[0]
        if asm:
            if not token.startswith(';'):
                return token
            body = token[1:].strip()
            match = re.match(r'(?:' + _FUNCTION + r'|' + _ADDRESS + r')(?=\s|$)', body, re.I)
            if not match:
                return token
            prose = body[match.end():].strip()
            # A MASM data trailer may include its generated type. Anything else is prose.
            if re.fullmatch(r'(?:(?:unsigned|signed)\s+)?(?:char|int|long|short|float|double|string|struct\s+\w+)\s*\**(?:\[[^]\n]*\])*', prose):
                prose = ''
            count += 1
            return '; ' + prose if prose else ''
        if not token.startswith('/*'):
            return token
        body = token[2:-2].strip()
        match = re.match(_FUNCTION + r'(?:\s*\n|$)', body, re.I)
        if match:
            prose = body[match.end():].strip('\n')
        elif re.fullmatch(_ADDRESS + r'|0x[0-9a-f]{5}', body, re.I):
            prose = ''
        else:
            # An address followed by prose keeps the prose (including PROVISIONAL notes).
            match = re.match(_ADDRESS + r'\s+(.+)', body, re.S | re.I)
            if not match:
                return token
            prose = match[1]
        count += 1
        prose = textwrap.dedent(prose).strip()
        return '/* ' + prose.replace('\n', '\n   ') + ' */' if prose else ' '

    changed = (_ASM_TOKEN if asm else _C_TOKEN).sub(replace, text)
    if count:
        # Multiline prose can change line counts; alignment padding is always trailing space.
        changed = re.sub(r'[ \t]+(?=\r?$)', '', changed, flags=re.M)
    return changed, count


_FUNC_ROW = re.compile(r'(\w+) @ ([0-9a-f]{4}):([0-9a-f]{4}) '
                       r'\(linear 0x([0-9a-f]+), (\d+) bytes\)(?=\s|$)', re.I)
_DATA_ROW = re.compile(r'(DS|[0-9a-f]{4}):([0-9a-f]{4})(?=\s|$)', re.I)
_PROTO_ROW = re.compile(r'0x([0-9a-f]{5})(?=\s|$)', re.I)


def _code(text, asm):
    """Hide comments and literals, retaining positions and newlines."""
    token = _ASM_TOKEN if asm else _C_TOKEN
    return token.sub(lambda m: re.sub(r'[^\r\n]', ' ', m[0]), text)


def _c_declarations(text):
    """Top-level semicolon statements (including brace-enclosed initializers)."""
    code = _code(text, False)
    code = re.sub(r'^[ \t]*#[^\n]*', lambda m: ' ' * len(m[0]), code, flags=re.M)
    start = depth = 0
    function_body = False
    for at, char in enumerate(code):
        if char == '{':
            if depth == 0:
                prefix = code[start:at]
                function_body = '=' not in prefix and ')' in prefix
            depth += 1
        elif char == '}':
            depth -= 1
            if depth == 0 and function_body:
                start = at + 1
                function_body = False
        elif char == ';' and depth == 0:
            decl = code[start:at].strip()
            yield at + 1, decl
            start = at + 1


def annotations(path, text):
    """Yield (kind, name, location) for each removable annotation.

    Function prototypes contribute linear-address assertions, not extents. MASM's
    private data labels are kept too; strip the ABI underscore only when present.
    Unrecognised attachment sites fail closed instead of discarding an address.
    """
    from sr.source import declared_name
    asm = path.suffix.upper() in ('.ASM', '.INC')
    declarations = list(_c_declarations(text)) if not asm else []
    code = _code(text, asm)
    for token in (_ASM_TOKEN if asm else _C_TOKEN).finditer(text):
        raw = token[0]
        if not raw.startswith(';' if asm else '/*'):
            continue
        body = raw[1:].strip() if asm else raw[2:-2].strip()
        # Use exactly the stripper's decision about which comments are removable.
        if not strip_text(raw, asm)[1]:
            continue
        fn = _FUNC_ROW.match(body)
        if fn:
            name, seg, off, linear, size = fn.groups()
            loc = dict(seg=int(seg, 16), off=int(off, 16), size=int(size))
            if loc['seg'] * 16 + loc['off'] != int(linear, 16):
                raise ValueError(f'{path}: inconsistent linear address for {name}')
            yield 'functions', name, loc
            continue
        address = _DATA_ROW.match(body)
        proto = _PROTO_ROW.fullmatch(body)
        if not address and not proto:
            raise ValueError(f'{path}: cannot extract {raw!r}')
        if asm:
            prefix = code[code.rfind('\n', 0, token.start()) + 1:token.start()]
            label = re.match(r'\s*([A-Za-z_]\w*)\s+(?:DB|DW|DD|DQ|DT|LABEL)\b', prefix, re.I)
            if not label and not prefix.strip():
                # Standalone trailers precede PUBLIC and then the definition.
                following = code[token.end():]
                label = re.match(r'\s*(?:PUBLIC\s+_?\w+\s*)?([A-Za-z_]\w*)\s+'
                                 r'(?:DB|DW|DD|DQ|DT|LABEL)\b', following, re.I)
            if not label:
                raise ValueError(f'{path}: address annotation has no data label: {raw}')
            name = label[1].removeprefix('_')
        else:
            candidates = [(end, decl) for end, decl in declarations if end <= token.start()]
            if not candidates:
                raise ValueError(f'{path}: address annotation has no preceding declaration: {raw}')
            end, decl = candidates[-1]
            if code[end:token.start()].strip():
                raise ValueError(f'{path}: address annotation is detached from its declaration: {raw}')
            if proto:
                match = re.search(r'\b(\w+)\s*\(', decl)
                if not match:
                    raise ValueError(f'{path}: annotated prototype has no name: {decl}')
                yield 'prototypes', match[1], int(proto[1], 16)
                continue
            name = declared_name(decl.split('=', 1)[0].removeprefix('extern ').strip())
        seg, off = address.groups()
        loc = dict(off=int(off, 16))
        if seg.upper() != 'DS':
            loc['seg'] = int(seg, 16)
        yield 'data', name, loc


def collect(paths):
    """Read everything first so errors cannot leave partially stripped sources."""
    contents, rows, live = {}, [], set()
    for path in paths:
        text = path.read_bytes().decode('latin-1')
        contents[path] = text
        asm = path.suffix.upper() in ('.ASM', '.INC')
        names = re.findall(r'\b[A-Za-z_]\w*\b', _code(text, asm))
        live.update(n.removeprefix('_') if asm else n for n in names)
        rows.extend(annotations(path, text))
    return contents, rows, live


def merge_annotations(existing, rows, live):
    """Annotations override stale locations; preserve still-live unannotated names."""
    merged = {kind: {name: row.copy() for name, row in existing.get(kind, {}).items() if name in live}
              for kind in ('functions', 'data')}
    found = {'functions': {}, 'data': {}}
    for kind, name, loc in rows:
        if kind == 'prototypes':
            continue
        if name in found[kind] and found[kind][name] != loc:
            raise ValueError(f'{name}: conflicting {kind} annotations')
        found[kind][name] = loc
    for kind in found:
        merged[kind].update(found[kind])
    validate_annotations(merged, rows)
    return merged


def validate_annotations(metadata, rows):
    """Require every annotation's name and original location before any removal."""
    for kind, name, loc in rows:
        group = 'functions' if kind == 'prototypes' else kind
        stored = metadata.get(group, {}).get(name)
        if stored is None:
            raise ValueError(f'{name}: missing {group} metadata; run --extract before stripping')
        if kind == 'prototypes':
            matches = stored['seg'] * 16 + stored['off'] == loc
        else:
            matches = stored == loc
        if not matches:
            raise ValueError(f'{name}: annotation disagrees with symbols.json; run --extract')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    modes = parser.add_mutually_exclusive_group()
    modes.add_argument('--check', action='store_true')
    modes.add_argument('--extract', action='store_true', help='merge annotations into symbols.json without stripping')
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args(argv)
    paths = sorted(p for p in (args.root / 'src').rglob('*')
                   if p.suffix.upper() in ('.C', '.H', '.ASM', '.INC'))
    metadata_path = args.root / 'tools' / 'sr' / 'symbols.json'
    try:
        if not paths:
            raise ValueError(f'no project source files under {args.root / "src"}')
        contents, rows, live = collect(paths)
        existing = json.loads(metadata_path.read_text()) if metadata_path.exists() else {}
        if args.extract:
            merged = merge_annotations(existing, rows, live)
            metadata_path.parent.mkdir(parents=True, exist_ok=True)
            temp = metadata_path.with_name(f'.{metadata_path.name}.{os.getpid()}')
            try:
                temp.write_text(json.dumps(merged, indent=2, sort_keys=True) + '\n')
                temp.replace(metadata_path)
            finally:
                temp.unlink(missing_ok=True)
            print(f"Extracted {len(rows)} annotations; symbols.json has "
                  f"{len(merged['functions'])} functions and {len(merged['data'])} data addresses")
            return 0
        validate_annotations(existing, rows)
    except (ValueError, KeyError, OSError) as error:
        print(f'Refusing to proceed: {error}', file=sys.stderr)
        return 2
    total = files = 0
    for path, text in contents.items():
        new, count = strip_text(text, path.suffix.upper() in ('.ASM', '.INC'))
        if count:
            total += count
            files += 1
            if not args.check:
                path.write_bytes(new.encode('latin-1'))
    print(f'{total} address annotations in {files} files' + (' remain' if args.check else ' removed'))
    return int(args.check and total > 0)


if __name__ == '__main__':
    raise SystemExit(main())
