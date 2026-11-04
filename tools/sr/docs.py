"""Build API documentation, fail on diagnostics, and report measured coverage."""

import json
import shutil
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path


def documented(element):
    return any(node is not None and ''.join(node.itertext()).strip()
               for node in (element.find('briefdescription'),
                            element.find('detaileddescription')))


def coverage(root, output):
    """Count documented C definitions and files, not generated HTML pages.

    Compound/member XML can repeat declarations; sets prevent double counting.
    The denominator comes from the source inventory, so EXTRACT_ALL=NO cannot
    make an empty documentation tree look like 100% coverage.
    """
    from . import source

    functions = {r['name'] for r in source.game_functions() if r['kind'] == 'c'}
    found = set()
    files = set()
    for path in sorted((output / 'xml').glob('*.xml')):
        tree = ET.parse(path)
        for member in tree.findall('.//memberdef[@kind="function"]'):
            if documented(member):
                found.add(member.findtext('name'))
        for compound in tree.findall('.//compounddef[@kind="file"]'):
            location = compound.find('location')
            if documented(compound) and location is not None:
                files.add(Path(location.get('file')).name)
    inputs = {p.name for p in (root / 'src').rglob('*')
              if p.suffix.upper() in ('.C', '.H')}
    result = {
        'c_functions': {'documented': len(functions & found), 'total': len(functions)},
        'c_files_and_headers': {'documented': len(inputs & files), 'total': len(inputs)},
    }
    (output / 'coverage.json').write_text(json.dumps(result, indent=2) + '\n')
    for label, counts in result.items():
        n, total = counts['documented'], counts['total']
        print(f"{label}: {n}/{total} documented ({100 * n / total if total else 0:.1f}%)")
    return result


def parameter_lint(output):
    """Doxygen 1.17 misses the case where every @param is absent.

    Check XML for documented functions with named arguments but no parameter
    documentation at all. Doxygen handles mismatches and partial omissions.
    """
    errors = set()
    for path in sorted((output / 'xml').glob('*.xml')):
        for member in ET.parse(path).findall('.//memberdef[@kind="function"]'):
            if not documented(member) or member.find('.//parameterlist[@kind="param"]') is not None:
                continue
            for param in member.findall('param'):
                name = param.findtext('declname') or param.findtext('defname')
                if name:
                    location = member.find('location')
                    file = location.get('file', '?') if location is not None else '?'
                    line = location.get('line', '?') if location is not None else '?'
                    errors.add(f"{file}:{line}: error: doc-lint: {member.findtext('name')} "
                               f"missing @param for {name}")
    return sorted(errors)


def build(root):
    root = Path(root)
    for program in ('doxygen', 'dot'):
        if shutil.which(program) is None:
            raise SystemExit(f'{program} missing: run nix-shell shell.nix --run "./sr docs"')
    output = root / 'work' / 'doxygen'
    output.mkdir(parents=True, exist_ok=True)
    warning_log = output / 'doxygen.warn'
    warning_log.unlink(missing_ok=True)
    # Clear XML only: otherwise deleted/renamed entities can inflate coverage.
    if (output / 'xml').exists():
        shutil.rmtree(output / 'xml')
    run = subprocess.run(['doxygen', 'Doxyfile'], cwd=root, text=True,
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (output / 'build.log').write_text(run.stdout)
    extra = parameter_lint(output)
    if extra:
        with warning_log.open('a') as stream:
            stream.write('\n'.join(extra) + '\n')
    warnings = warning_log.read_text() if warning_log.exists() else ''
    count = sum('warning:' in line or 'error:' in line for line in warnings.splitlines())
    print(f'Doxygen diagnostics: {count} (work/doxygen/doxygen.warn)')
    if run.stdout:
        print(run.stdout, end='')
    if warnings:
        print(warnings, end='')
    # Doxygen's exit status is authoritative; also reject any nonempty warning
    # log, including diagnostics whose wording does not contain "warning:".
    return int(bool(run.returncode or warnings.strip()))


def main():
    from . import config

    status = build(config.ROOT)
    coverage(config.ROOT, config.ROOT / 'work' / 'doxygen')
    print('HTML: work/doxygen/html/index.html')
    return status
