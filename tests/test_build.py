"""The runnable build must not import comparison-only placeholders."""
import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import build
from sr.cdata import Types, number


class BuildTests(unittest.TestCase):
    def test_staging_and_native_make_order(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            with patch.object(build, 'OUT', root), patch.object(build.headers, 'generate'):
                jobs = build.prepare()
            self.assertEqual(len(jobs), 159)
            sources = {j['stem']: (root / j['source']).read_text() for j in jobs}
            self.assertIn('int main(int argc', sources['M_00659'])
            self.assertNotIn('static int near', sources['M_00659'])
            self.assertIn('#include "M_0DCBE.H"', sources['M_07800'])
            self.assertNotIn('F_0E218(int x) {', sources['M_07800'])
            self.assertNotIn('.STB', (root / 'LINK.RSP').read_text())
            self.assertIn('.CODE G2_TEXT', sources['M_2487A'])
            make = (root / 'MAKEFILE').read_text()
            # MAKE 4.07 evaluates rules in order, not recursively.
            self.assertGreater(make.index('SR.EXE:'), make.index('M_2B6DC.OBJ:'))
            self.assertEqual(make.count('\tLINK '), 1)
            self.assertIn('INCLUDE\\STRUCTS.H', make)
            self.assertIn('if exist SR.EXE del SR.EXE', (root / 'BUILD.BAT').read_text())

    def test_link_report_includes_fixup_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'LINK.LOG').write_text('DGROUP.OBJ : error L2002: fixup overflow\n')
            with contextlib.redirect_stdout(io.StringIO()):
                report = build.link_report(root)
            self.assertEqual(report['missing_symbols'], [])
            self.assertEqual(len(report['diagnostics']), 1)

    def test_msc_data_shapes(self):
        types = Types()
        self.assertEqual(types.declaration('int far *@')['size'], 4)
        self.assertEqual(types.declaration('int *@')['size'], 2)
        self.assertEqual(types.declaration('int (*@)(void)')['size'], 4)
        self.assertEqual(types.named('struct Bitmap')['size'], 48)
        self.assertEqual(types.named('struct Run')['size'], 6)
        self.assertEqual(types.named('struct List')['size'], 4)
        self.assertEqual(types.named('struct CgaFont')['size'], 1642)
        self.assertEqual(number('2 * (3 + 4)'), 14)
        with self.assertRaises(ValueError):
            number('__import__("os")')


if __name__ == '__main__':
    unittest.main()
