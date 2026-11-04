"""Exercise the pinned formatter, CLI and source-rewriting commands with real C text."""
import contextlib
import io
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import cli, config, fileheaders, format as formatter, headers, rename, source


class FormatTests(unittest.TestCase):
    def test_default_targets_and_reject_assembly_before_writing(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ('UPPER.C', 'LOWER.h', 'CODE.ASM', 'PC.INC', 'README.md'):
                (root / name).write_text('int x=1;\n')
            (root / 'DIR.C').mkdir()
            with patch.object(config, 'SRC', root):
                self.assertEqual([p.name for p in formatter.default_targets()],
                                 ['LOWER.h', 'UPPER.C'])
            before = (root / 'UPPER.C').read_bytes()
            for invalid in ('CODE.ASM', 'PC.INC', 'MISSING.C'):
                with self.assertRaises(ValueError):
                    formatter.format_files([root / 'UPPER.C', root / invalid])
                self.assertEqual((root / 'UPPER.C').read_bytes(), before)

    def test_check_write_and_idempotence_with_dos_c(self):
        raw = (b'#include <dos.h>\n#include "Z.H"\n#include "A.H"\n'
               b'int far *p;\nint _near f(register int x)\n{\n'
               b'union REGS r; if(x){int86(0x10,&r,&r);} return x;\n}\n'
               b'int old(x)\nint x;\n{ return x; }\n')
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'WITH SPACE.C'
            path.write_bytes(raw)
            self.assertEqual(formatter.format_files([path], check=True), [path])
            self.assertEqual(path.read_bytes(), raw)
            formatter.format_files([path])
            clean = path.read_bytes()
            self.assertIn(b'int86(0x10, &r, &r);', clean)
            self.assertLess(clean.index(b'"Z.H"'), clean.index(b'"A.H"'))
            self.assertEqual(formatter.format_files([path], check=True), [])
            formatter.format_files([path])
            self.assertEqual(path.read_bytes(), clean)

    def test_empty_targets_do_not_start_formatter(self):
        with patch.object(subprocess, 'run') as run:
            self.assertEqual(formatter.format_files([]), [])
            self.assertEqual(formatter.format_files([], check=True), [])
            run.assert_not_called()

    def test_cli_check_status_and_no_write(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'FILE.C'
            path.write_text('int x=1;\n')
            with contextlib.redirect_stdout(io.StringIO()):
                with patch.object(sys, 'argv', ['sr', 'format', '--check', str(path)]):
                    with self.assertRaises(SystemExit) as exc:
                        cli.main()
                    self.assertIn('need formatting', str(exc.exception))
                self.assertEqual(path.read_text(), 'int x=1;\n')
                with patch.object(sys, 'argv', ['sr', 'format', str(path)]):
                    cli.main()
                with patch.object(sys, 'argv', ['sr', 'format', '--check', str(path)]):
                    cli.main()

    def test_headers_dry_run_formats_generated_text_without_writing(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'DRAW.C'
            path.write_text('#include "STRUCTS.H"\nint x=1;\n')
            raw = path.read_bytes()
            def generate():
                fileheaders._rewrite_includes(path, 'DRAW', {}, {'DRAW'})
                return []
            with patch.object(headers, 'generate', side_effect=generate):
                drift, problems = headers.check()
            self.assertEqual(problems, [])
            self.assertEqual(path.read_bytes(), raw)
            self.assertEqual(drift[path], formatter.format_bytes(
                b'#include "STRUCTS.H"\n#include "DRAW.H"\nint x=1;\n', path))
            generate()
            self.assertEqual(formatter.format_files([path], check=True), [])
            with patch.object(headers, 'generate', side_effect=generate):
                self.assertEqual(headers.check(), ({}, []))

    def test_rename_reflows_changed_c_and_headers(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            paths = [root / 'ONE.C', root / 'ONE.H']
            for path in paths:
                path.write_text('int old=1;\n')
            unchanged = root / 'TWO.C'
            unchanged.write_text('int untouched=1;\n')
            make = root / 'MAKEFILE'
            link = root / 'SR.LNK'
            make.write_text('unchanged\n')
            link.write_text('unchanged\n')
            with patch.object(config, 'ROOT', root), \
                    patch.object(rename, 'text_files', return_value=paths + [unchanged]), \
                    patch.object(rename, '_taken', return_value={'OLD'}), \
                    patch.object(rename, '_symbol', return_value=('global', 1)), \
                    patch.object(rename.makefile, 'MAKEFILE', make), \
                    patch.object(rename.makefile, 'RESPONSE', link), \
                    patch.object(source, 'reload'), \
                    patch.object(headers, 'generate', return_value=[]):
                rename.rename([('old', 'longer_name')], dry=True)
                self.assertEqual(paths[0].read_text(), 'int old=1;\n')
                rename.rename([('old', 'longer_name')])
            self.assertEqual(formatter.format_files(paths, check=True), [])
            self.assertIn('longer_name', paths[0].read_text())
            self.assertEqual(unchanged.read_text(), 'int untouched=1;\n')
