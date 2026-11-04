"""The build's generated DOS files: what MAKE 4.07 and BUILD.BAT run, in order."""
import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import build, config, linklayout
from sr.cdata import Types, number


class BuildTests(unittest.TestCase):
    def test_native_make_order(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            with patch.object(linklayout, 'OUT', root), patch.object(linklayout.headers, 'generate'):
                jobs = linklayout.prepare()
            stems = {j['stem'] for j in jobs}
            response = (root / 'LINK.RSP').read_text()
            # every compiled object is linked, and only those
            linked = {n[:-4] for n in response.split('\nSR.EXE')[0].replace('+', ' ').split()}
            self.assertEqual(linked, stems)
            self.assertNotIn('.STB', response)
            # each source is compiled under its own name, exactly as it is in git
            for j in jobs:
                if j['stem'].startswith('M_'):
                    src = config.SRC / 'mods' / j['source']
                    self.assertEqual((root / j['source']).read_bytes(), src.read_bytes(), j['source'])
            self.assertIn('M_0F38A', stems)
            self.assertFalse(any(s.startswith(('U_', 'D_')) for s in stems))
            self.assertTrue(response.rstrip().endswith('SR.EXE\nSR.MAP\n\n;'))
            make = (root / 'MAKEFILE').read_text()
            # MAKE 4.07 runs rules in file order: the library first, the link last
            self.assertLess(make.index('MLIBCE.LIB:'), min(make.index(s + '.OBJ:') for s in stems))
            self.assertGreater(make.index('SR.EXE:'), max(make.index(s + '.OBJ:') for s in stems))
            self.assertEqual(make.count('\tLINK '), 1)
            self.assertIn('\tLINK /MAP /STACK:2560 /EXEPACK @LINK.RSP', make)
            self.assertIn('INCLUDE\\STRUCTS.H', make)
            batch = (root / 'BUILD.BAT').read_text()
            self.assertIn('LIB MLIBCE.LIB +D:\\LIB\\MLIBCR.LIB+D:\\LIB\\LIBH.LIB+D:\\LIB\\EM.LIB+D:\\LIB\\MLIBFP.LIB;', batch)
            self.assertIn('if exist SR.EXE del SR.EXE', batch)

    def test_link_report_includes_fixup_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'LINK.LOG').write_text('U_00000.OBJ : error L2002: fixup overflow\n')
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
