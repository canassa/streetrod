"""Explicit C formatting; the style is never discovered automatically by editors."""

from pathlib import Path
import subprocess

from . import config

STYLE = Path(__file__).with_name('clang-format.yaml')


def default_targets():
    return sorted(p for p in config.SRC.rglob('*')
                  if p.is_file() and p.suffix.upper() in ('.C', '.H'))


def format_bytes(data, path):
    """Format generated text without writing, including during headers --check."""
    return subprocess.run(
        ['clang-format', f'--style=file:{STYLE}', f'--assume-filename={path}'],
        input=data, stdout=subprocess.PIPE, check=True,
    ).stdout


def format_files(paths, check=False):
    """Validate all targets first. Return files with drift in non-writing check mode."""
    targets = list(dict.fromkeys(Path(p) for p in paths))
    for path in targets:
        if path.suffix.upper() not in ('.C', '.H'):
            raise ValueError(f'{path}: expected a C source or header (.C/.H)')
        if not path.is_file():
            raise ValueError(f'{path}: no such file')
    if check:
        return [p for p in targets if format_bytes(p.read_bytes(), p) != p.read_bytes()]
    if targets:
        subprocess.run(['clang-format', '-i', f'--style=file:{STYLE}',
                        '--', *(str(p) for p in targets)], check=True)
    return []
