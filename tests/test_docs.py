"""Exercise the real Doxygen gate with isolated MSC-style header fixtures."""
import contextlib
import io
import shutil
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools.sr import docs

ROOT = Path(__file__).resolve().parents[1]


@unittest.skipUnless(shutil.which('doxygen') and shutil.which('dot'), 'requires shell.nix docs tools')
class DocsGate(unittest.TestCase):
    def run_docs(self, comment):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for folder in ('SYS', 'UI', 'GAME', 'VIDEO', 'CGA', 'EGA', 'TANDY',
                           'HERC', 'INPUT', 'SOUND', 'DATA'):
                (root / 'src' / folder).mkdir(parents=True)
            (root / 'toolchain/msc51/INCLUDE').mkdir(parents=True)
            (root / 'docs').mkdir()
            (root / 'docs/RECONSTRUCTION.md').write_text('# Reconstruction\n')
            (root / 'README.md').write_text('# Fixture\n')
            (root / 'Doxyfile').write_text((ROOT / 'Doxyfile').read_text() + '\nHAVE_DOT=NO\n')
            (root / 'src' / 'SYS' / 'TYPES.H').write_text(
                '/** @file */\n/** A car. */\ntypedef struct Car {\n'
                'int price; /**< Price in dollars. */\n} Car;\n')
            (root / 'src' / 'SYS' / 'TEST.H').write_text(
                '/** @file */\n#include "TYPES.H"\n' + comment +
                '\nCar far * near ex_find(int slot, int count);\n')
            with contextlib.redirect_stdout(io.StringIO()):
                status = docs.build(root)
            return status, (root / 'work/doxygen/doxygen.warn').read_text()

    def test_msc_header_and_reference(self):
        status, warnings = self.run_docs(
            '/** Find a @ref Car.\n * @param slot First slot.\n'
            ' * @param count Number of slots.\n * @return Borrowed car.\n */')
        self.assertEqual((status, warnings), (0, ''))

    def test_unresolved_reference_fails(self):
        status, warnings = self.run_docs(
            '/** Find @ref missing_car.\n * @param slot First slot.\n'
            ' * @param count Number of slots.\n */')
        self.assertEqual(status, 1)
        self.assertIn('missing_car', warnings)

    def test_wrong_parameter_fails(self):
        status, warnings = self.run_docs(
            '/** Find a car.\n * @param wrong First slot.\n'
            ' * @param count Number of slots.\n */')
        self.assertEqual(status, 1)
        self.assertIn('wrong', warnings)

    def test_missing_parameter_fails(self):
        status, warnings = self.run_docs('/** Find a car.\n * @param slot First slot.\n */')
        self.assertEqual(status, 1)
        self.assertIn('count', warnings)

    def test_no_parameter_docs_fails(self):
        status, warnings = self.run_docs('/** Find a car. */')
        self.assertEqual(status, 1)
        self.assertTrue(warnings.strip())


class Coverage(unittest.TestCase):
    def test_coverage_uses_source_denominator_and_discards_duplicate_members(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'src').mkdir()
            (root / 'src/TEST.C').write_text('')
            output = root / 'work/doxygen'
            (output / 'xml').mkdir(parents=True)
            xml = ('<doxygen><compounddef kind="file"><location file="src/TEST.C"/>'
                   '<briefdescription><para>File.</para></briefdescription>'
                   '<sectiondef><memberdef kind="function"><name>ex_find</name>'
                   '<briefdescription><para>Find.</para></briefdescription>'
                   '</memberdef></sectiondef></compounddef></doxygen>')
            (output / 'xml/a.xml').write_text(xml)
            (output / 'xml/b.xml').write_text(xml)
            with patch('tools.sr.source.game_functions', return_value=[
                    {'name': 'ex_find', 'kind': 'c'},
                    {'name': 'ex_other', 'kind': 'c'},
                    {'name': 'asm_func', 'kind': 'asm'}]):
                with contextlib.redirect_stdout(io.StringIO()):
                    result = docs.coverage(root, output)
            self.assertEqual(result['c_functions'], {'documented': 1, 'total': 2})
            self.assertEqual(result['c_files_and_headers'], {'documented': 1, 'total': 1})
