"""The checked-in build: src/MAKEFILE (MSC MAKE 4.07) and src/SR.LNK are what ./sr build runs."""
import contextlib
import io
import re
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import build, config, headers, linklayout, makefile, module
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
        k = objects.index('DASH.OBJ')
        self.assertEqual(objects[k - 2:k], ['FONTDATA.OBJ', 'CGAFONT.OBJ'])
        self.assertEqual(objects[-1], 'SLOTS.OBJ')

    def test_object_blocks(self):
        for r in makefile.compile_rules():
            self.assertTrue(r.path.exists(), r.source)
            self.assertEqual(r.stem, r.path.stem.upper())
            if r.compiler == 'CL':
                self.assertTrue(r.command.startswith('CL /c /W3 /Fc /AM '), r.target)
                # the dependencies are the source and the headers it includes
                self.assertEqual(r.deps, [r.source, *makefile.header_deps(r.path)], r.target)
            else:
                self.assertEqual(r.args[-1], f'{r.source},{r.stem}.OBJ,{r.stem}.LST;')
                self.assertEqual(r.deps, [r.source, *makefile.header_deps(r.path)])
        fs = {f.path.name: f for f in module.files()}
        self.assertTrue(all(f.segment for f in fs.values()))
        self.assertEqual(fs['DASH.C'].rule.command,
                         'CL /c /W3 /Fc /AM /Oalsi /Gs /FPi /NTS0F38_TEXT GAME\\DASH.C')

    def test_parser_and_edits(self):
        text = ('# comment\r\nX=D:\\LIB\r\nCC=CL /c\r\n\r\nA.OBJ: SYS\\A.C \\\r\n\tSYS\\A.H\r\n'
                '\t$(CC) /AM /Oas /NTS0000_TEXT SYS\\A.C\r\n\r\nL.LIB: $(X)\\M.LIB\r\n\tLIB L.LIB +$(X)\\M.LIB;\r\n')
        macros, rules = makefile.parse(text)
        self.assertEqual(macros['X'], 'D:\\LIB')
        a, lib = rules
        self.assertEqual(a.deps, ['SYS\\A.C', 'SYS\\A.H'])
        self.assertEqual((a.flags, a.segment, a.source), ('/AM /Oas', 'S0000_TEXT', 'SYS\\A.C'))
        self.assertEqual(a.command, 'CL /c /AM /Oas /NTS0000_TEXT SYS\\A.C')
        self.assertEqual(lib.deps, ['D:\\LIB\\M.LIB'])
        self.assertEqual(lib.command, 'LIB L.LIB +D:\\LIB\\M.LIB;')
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'MAKEFILE'
            p.write_bytes(text.encode())
            makefile.set_flags('A', '/AM /Oals', p)
            new = p.read_bytes().decode()
            self.assertIn('\t$(CC) /AM /Oals /NTS0000_TEXT SYS\\A.C\r\n', new)
            self.assertTrue(new.startswith('# comment\r\nX=D:\\LIB\r\n'))
            self.assertEqual(makefile.rules(p)[0].deps, a.deps)
            self.assertEqual(makefile.rules(p)[1].command, lib.command)

    def test_shared_pc_constants(self):
        asm = dict(re.findall(r'^(\w+)\s+EQU\s+(\w+)\s*$',
                              config.source_path('PC.INC').read_text(), re.M))
        shared = re.findall(r'^#define[ \t]+(\w+)[ \t]+(0x[\da-f]+|\d+)\s*$',
                            config.source_path('PC.H').read_text(), re.M | re.I)
        c_only = {'EQUIP_VIDEO_COLOR80': 0x20}
        for name, value in shared:
            if name in c_only:
                self.assertNotIn(name, asm)
                self.assertEqual(int(value, 0), c_only[name])
                continue
            self.assertIn(name, asm)
            literal = asm[name]
            actual = int(literal[:-1], 16) if literal.lower().endswith('h') else int(literal)
            self.assertEqual(actual, int(value, 0), name)

    def test_asm_include_dependencies(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            include = root / 'SYS'
            include.mkdir()
            (include / 'PC.INC').write_text('INCLUDE BITS.INC\n')
            (include / 'BITS.INC').write_text('GC_MODE EQU 5\nINCLUDE PC.INC\n')
            asm = root / 'A.ASM'
            asm.write_text('        .MODEL MEDIUM\n        include pc.inc ; hardware\n'
                           '; INCLUDE UNUSED.INC\n')
            mk = root / 'MAKEFILE'
            mk.write_text('ASM=MASM\nA.OBJ: A.ASM\n\t$(ASM) /Ml A.ASM,A.OBJ,A.LST;\n')
            with patch.object(config, 'SRC', root):
                self.assertEqual(makefile.header_deps(asm), ['SYS\\BITS.INC', 'SYS\\PC.INC'])
                self.assertEqual(makefile.sync_deps(mk), ['A.OBJ'])
                self.assertEqual(makefile.compile_rules(mk)[0].deps,
                                 ['A.ASM', 'SYS\\BITS.INC', 'SYS\\PC.INC'])
                self.assertEqual(makefile.sync_deps(mk), [])
                asm.write_text('        .MODEL MEDIUM\n')
                self.assertEqual(makefile.sync_deps(mk), ['A.OBJ'])
                self.assertEqual(makefile.compile_rules(mk)[0].deps, ['A.ASM'])

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
        # the include blocks and MAKEFILE dependencies in git are what ./sr headers writes,
        # and every header declares only what its file defines
        with contextlib.redirect_stdout(io.StringIO()):
            drift, problems = headers.check()
        self.assertEqual(sorted(str(p) for p in drift), [])
        self.assertEqual(problems, [])

    def test_link_report_includes_fixup_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'LINK.LOG').write_text('MAIN.OBJ : error L2002: fixup overflow\n')
            with contextlib.redirect_stdout(io.StringIO()):
                report = build.link_report(root)
            self.assertEqual(report['missing_symbols'], [])
            self.assertEqual(len(report['diagnostics']), 1)

    def test_record_with_named_function_pointers(self):
        with tempfile.TemporaryDirectory() as tmp:
            include = Path(tmp)
            (include / 'STRUCTS.H').write_text(
                'struct Callbacks { char enabled; '
                'void (far *show)(void); '
                'int (near *read)(int); int count; };')
            with patch.object(config, 'SRC', include):
                record = Types().named('struct Callbacks')
        self.assertEqual(record['size'], 10)
        self.assertEqual([(name, off, node['size']) for name, off, node in record['fields']],
                         [('enabled', 0, 1), ('show', 2, 4), ('read', 6, 2), ('count', 8, 2)])

    def test_record_with_enum_member(self):
        with tempfile.TemporaryDirectory() as tmp:
            include = Path(tmp)
            (include / 'STRUCTS.H').write_text(
                'enum Choice { NONE, FIRST, SECOND }; '
                'struct Selection { char enabled; enum Choice choice; };')
            with patch.object(config, 'SRC', include):
                record = Types().named('struct Selection')
        self.assertEqual(record['size'], 4)
        self.assertEqual([(name, off, node['size']) for name, off, node in record['fields']],
                         [('enabled', 0, 1), ('choice', 2, 2)])

    def test_msc_data_shapes(self):
        types = Types()
        self.assertEqual(types.declaration('int far *@')['size'], 4)
        self.assertEqual(types.declaration('int *@')['size'], 2)
        self.assertEqual(types.declaration('int (*@)(void)')['size'], 4)
        self.assertEqual(types.named('struct Bitmap')['size'], 48)
        self.assertEqual(types.named('struct ScenerySprite')['size'], 6)
        self.assertEqual(types.named('struct SceneryLods')['size'], 4)
        self.assertEqual(types.named('struct CgaGlyph')['size'], 18)
        self.assertEqual(number('2 * (3 + 4)'), 14)
        with self.assertRaises(ValueError):
            number('__import__("os")')


if __name__ == '__main__':
    unittest.main()
