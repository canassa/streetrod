"""The checked-in build: src/MAKEFILE (MSC MAKE 4.07) and src/SR.LNK are what ./sr build runs."""
import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import build, config, db, headers, linklayout, makefile, module
from sr.cdata import Types, number


class MakefileTests(unittest.TestCase):
    def test_one_build_definition(self):
        rules = makefile.compile_rules()
        lib, link = makefile.rule('LIB'), makefile.rule('LINK')
        objects, exe, mapfile, libs = makefile.response()
        # every object is compiled once and linked once; MAKE 4.07 runs the blocks in
        # file order, so the library comes first and the link last
        self.assertEqual(sorted(r.target for r in rules), sorted(objects))
        self.assertEqual(len(set(objects)), len(objects))
        self.assertLess(lib.first, min(r.first for r in rules))
        self.assertGreater(link.first, max(r.first for r in rules))
        self.assertTrue(set(objects) | {'MLIBCE.LIB', 'SR.LNK'} <= set(link.deps))
        self.assertEqual(link.command, 'LINK /MAP /STACK:2560 /EXEPACK @SR.LNK')
        self.assertEqual(lib.command, 'LIB MLIBCE.LIB +D:\\LIB\\MLIBCR.LIB+D:\\LIB\\LIBH.LIB'
                                      '+D:\\LIB\\EM.LIB+D:\\LIB\\MLIBFP.LIB;')
        self.assertEqual((exe, mapfile, libs), ('SR.EXE', 'SR.MAP', ''))
        # the fonts (class CODE) are linked just before the first file of code segment S0F38
        k = objects.index('M_0F38A.OBJ')
        self.assertEqual(objects[k - 2:k], ['FONTDATA.OBJ', 'CGAFONT.OBJ'])
        self.assertEqual(objects[-1], 'SLOTS.OBJ')

    def test_object_blocks(self):
        for r in makefile.compile_rules():
            self.assertTrue(r.path.exists(), r.source)
            self.assertEqual(r.stem, r.path.stem.upper())
            if r.compiler == 'CL':
                self.assertTrue(r.command.startswith('CL /c /W3 /Fc /IINCLUDE /AM '), r.target)
                # the dependencies are the source and the headers it includes
                self.assertEqual(r.deps, [r.source, *makefile.header_deps(r.path)], r.target)
            else:
                self.assertEqual(r.args[-1], f'{r.source},{r.stem}.OBJ,{r.stem}.LST;')
                self.assertEqual(r.deps, [r.source])
        fs = {f.path.name: f for f in module.files()}
        self.assertTrue(all(f.segment for f in fs.values()))
        self.assertEqual(fs['M_0F38A.C'].rule.command,
                         'CL /c /W3 /Fc /IINCLUDE /AM /Oalsi /Gs /FPi /NTS0F38_TEXT MODS\\M_0F38A.C')

    def test_parser_and_edits(self):
        text = ('# comment\r\nX=D:\\LIB\r\nCC=CL /c\r\n\r\nA.OBJ: MODS\\A.C \\\r\n\tINCLUDE\\A.H\r\n'
                '\t$(CC) /AM /Oas /NTS0000_TEXT MODS\\A.C\r\n\r\nL.LIB: $(X)\\M.LIB\r\n\tLIB L.LIB +$(X)\\M.LIB;\r\n')
        macros, rules = makefile.parse(text)
        self.assertEqual(macros['X'], 'D:\\LIB')
        a, lib = rules
        self.assertEqual(a.deps, ['MODS\\A.C', 'INCLUDE\\A.H'])
        self.assertEqual((a.flags, a.segment, a.source), ('/AM /Oas', 'S0000_TEXT', 'MODS\\A.C'))
        self.assertEqual(a.command, 'CL /c /AM /Oas /NTS0000_TEXT MODS\\A.C')
        self.assertEqual(lib.deps, ['D:\\LIB\\M.LIB'])
        self.assertEqual(lib.command, 'LIB L.LIB +D:\\LIB\\M.LIB;')
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'MAKEFILE'
            p.write_bytes(text.encode())
            makefile.set_flags('A', '/AM /Oals', p)
            new = p.read_bytes().decode()
            self.assertIn('\t$(CC) /AM /Oals /NTS0000_TEXT MODS\\A.C\r\n', new)
            self.assertTrue(new.startswith('# comment\r\nX=D:\\LIB\r\n'))
            self.assertEqual(makefile.rules(p)[0].deps, a.deps)
            self.assertEqual(makefile.rules(p)[1].command, lib.command)

    def test_prepare_copies_src(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            with patch.object(linklayout, 'OUT', root):
                rules = linklayout.prepare()
            for name in ('MAKEFILE', 'SR.LNK'):
                self.assertEqual((root / name).read_bytes(), (config.SRC / name).read_bytes())
            for r in rules:
                self.assertEqual((root / r.path.relative_to(config.SRC)).read_bytes(), r.path.read_bytes())

    def test_headers_are_current(self):
        # the headers, include blocks and MAKEFILE dependencies in git are what ./sr headers writes
        con = db.connect()
        with contextlib.redirect_stdout(io.StringIO()):
            drift = headers.check(con)
        con.close()
        self.assertEqual(sorted(str(p) for p in drift), [])

    def test_link_report_includes_fixup_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'LINK.LOG').write_text('MAIN.OBJ : error L2002: fixup overflow\n')
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
        self.assertEqual(types.named('struct CgaGlyph')['size'], 18)
        self.assertEqual(number('2 * (3 + 4)'), 14)
        with self.assertRaises(ValueError):
            number('__import__("os")')


if __name__ == '__main__':
    unittest.main()
