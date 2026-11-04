"""Remove decompilation address annotations from src/; preserve code and prose.

Run: nix-shell shell.nix --run 'python3 tools/strip_address_comments.py'
Use --check to report remaining annotations without writing. This deliberately does
not update symbols.json: register new symbols there before running the stripper.
"""
import argparse
from pathlib import Path
import re
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args()
    total = files = 0
    for path in sorted((args.root / 'src').rglob('*')):
        if path.suffix.upper() not in ('.C', '.H', '.ASM', '.INC'):
            continue
        raw = path.read_bytes()
        new, count = strip_text(raw.decode('latin-1'), path.suffix.upper() in ('.ASM', '.INC'))
        if count:
            total += count
            files += 1
            if not args.check:
                path.write_bytes(new.encode('latin-1'))
    print(f'{total} address annotations in {files} files' + (' remain' if args.check else ' removed'))
    return int(args.check and total > 0)


if __name__ == '__main__':
    raise SystemExit(main())
