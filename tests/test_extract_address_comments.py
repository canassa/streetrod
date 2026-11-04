import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from strip_address_comments import main


class ExtractAddressTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / 'src').mkdir()
        self.metadata = self.root / 'tools/sr/symbols.json'

    def write(self, filename, text):
        path = self.root / 'src' / filename
        path.write_text(text)
        return path

    def store(self, functions=None, data=None):
        self.metadata.parent.mkdir(parents=True, exist_ok=True)
        self.metadata.write_text(json.dumps(dict(functions=functions or {}, data=data or {})))

    def run_cli(self, *args):
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return main(['--root', str(self.root), *args])

    def test_extract_rebuilds_renamed_and_preserves_stripped_symbols(self):
        self.store(functions={'old': dict(seg=0, off=16, size=4),
                              'kept': dict(seg=0, off=32, size=5)},
                   data={'old_data': {'off': 48}, 'kept_data': {'off': 64}})
        source = self.write('A.C', '/* new @ 0000:0010 (linear 0x00010, 4 bytes)\n'
                            '   Keep prose. */\nint new(void) { return 1; }\n'
                            'int kept(void) { return 2; }\n'
                            'int renamed_data = 3; /* DS:0030 */\nint kept_data;\n')
        header = self.write('A.H', 'int new(void); /* 0x00010 */\n'
                            'extern int renamed_data; /* DS:0030 */\n')
        before = {p: p.read_bytes() for p in (source, header)}
        self.assertEqual(self.run_cli('--extract'), 0)
        rows = json.loads(self.metadata.read_text())
        self.assertEqual(rows, dict(functions={'new': dict(seg=0, off=16, size=4),
                                              'kept': dict(seg=0, off=32, size=5)},
                                    data={'renamed_data': {'off': 48}, 'kept_data': {'off': 64}}))
        self.assertEqual(before, {p: p.read_bytes() for p in before})
        self.assertEqual(self.run_cli('--check'), 1)
        self.assertEqual(self.run_cli(), 0)
        self.assertIn('/* Keep prose. */', source.read_text())
        self.assertEqual(self.run_cli('--check'), 0)
        metadata_bytes = self.metadata.read_bytes()
        source_bytes = source.read_bytes()
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertEqual(self.run_cli(), 0)
        self.assertEqual(self.metadata.read_bytes(), metadata_bytes)
        self.assertEqual(source.read_bytes(), source_bytes)

    def test_rebuild_without_existing_metadata(self):
        self.write('F.C', '/* f @ 1234:0010 (linear 0x12350, 7 bytes) */\n'
                   'int f(void) { return 1; }\n'
                   'int a[2] = {1, 2}; /* DS:0050 */\n'
                   'static int b; /* DS:0054 PROVISIONAL filler */\n'
                   'int (far *callback)(void); /* DS:0056 */\n')
        self.write('F.H', 'int f(void); /* 0x12350 */\n'
                   'extern int far far_data[]; /* 4321:0010 */\n')
        self.write('D.INC', '_x DB 0 ; DS:0060 char\n'
                   '; DS:0061 char[2]\n PUBLIC _y\n_y DB 0, 0\n'
                   'S_0063 DB "text", 0 ; DS:0063 string\n')
        self.assertEqual(self.run_cli('--extract'), 0)
        rows = json.loads(self.metadata.read_text())
        self.assertEqual(rows['functions'], {'f': dict(seg=0x1234, off=16, size=7)})
        self.assertEqual(rows['data'], dict(a={'off': 80}, b={'off': 84}, callback={'off': 86},
                         far_data={'seg': 0x4321, 'off': 16}, x={'off': 96},
                         y={'off': 97}, S_0063={'off': 99}))
        self.assertEqual(self.run_cli(), 0)

    def test_missing_symbol_refuses_before_any_write(self):
        self.store(data={'known': {'off': 16}})
        first = self.write('A.H', 'extern int known; /* DS:0010 */\n')
        last = self.write('Z.H', 'extern int missing; /* DS:0020 */\n')
        before = {p: p.read_bytes() for p in (first, last, self.metadata)}
        self.assertEqual(self.run_cli(), 2)
        self.assertEqual(before, {p: p.read_bytes() for p in before})
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertEqual(self.run_cli(), 0)

    def test_conflicts_fail_without_writing(self):
        self.store(data={'x': {'off': 16}})
        source = self.write('A.C', 'int x; /* DS:0020 */\n')
        header = self.write('A.H', 'extern int x; /* DS:0010 */\n')
        before = {p: p.read_bytes() for p in (source, header, self.metadata)}
        self.assertEqual(self.run_cli(), 2)
        self.assertEqual(self.run_cli('--extract'), 2)
        self.assertEqual(before, {p: p.read_bytes() for p in before})

    def test_annotation_replaces_old_location(self):
        self.store(data={'x': {'off': 16}})
        self.write('A.C', 'int x; /* DS:0020 */\n')
        self.assertEqual(self.run_cli(), 2)
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertEqual(json.loads(self.metadata.read_text())['data']['x'], {'off': 32})
        self.assertEqual(self.run_cli(), 0)

    def test_bad_linear_address_or_prototype_fails(self):
        path = self.write('F.C', '/* f @ 0000:0010 (linear 0x00011, 4 bytes) */\n'
                          'int f(void) { return 1; }\n')
        self.assertEqual(self.run_cli('--extract'), 2)
        self.assertFalse(self.metadata.exists())
        path.write_text('/* f @ 0000:0010 (linear 0x00010, 4 bytes) */\n'
                        'int f(void) { return 1; }\n')
        self.write('F.H', 'int f(void); /* 0x00011 */\n')
        self.assertEqual(self.run_cli('--extract'), 2)
        self.assertFalse(self.metadata.exists())

    def test_asm_function_and_prototype(self):
        self.write('F.ASM', '; f @ 0000:0010 (linear 0x00010, 4 bytes)\n'
                   '_f PROC FAR\n ret\n_f ENDP\n')
        self.write('F.H', 'int f(void); /* 0x00010 */\n')
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertEqual(json.loads(self.metadata.read_text())['functions'],
                         {'f': dict(seg=0, off=16, size=4)})
        self.assertEqual(self.run_cli(), 0)
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertIn('f', json.loads(self.metadata.read_text())['functions'])

    def test_prototype_alone_cannot_invent_a_function_extent(self):
        self.write('F.H', 'int f(void); /* 0x00010 */\n')
        self.assertEqual(self.run_cli('--extract'), 2)
        self.assertFalse(self.metadata.exists())

    def test_no_sources_cannot_erase_metadata(self):
        self.store(data={'x': {'off': 16}})
        before = self.metadata.read_bytes()
        self.assertEqual(self.run_cli('--extract'), 2)
        self.assertEqual(self.metadata.read_bytes(), before)

    def test_strings_and_prose_do_not_create_symbols(self):
        self.write('F.C', 'char *s = "/* DS:1234 */";\n'
                   '/* Uninitialized storage (DS:1234..5678): PROVISIONAL */\n')
        self.assertEqual(self.run_cli('--extract'), 0)
        self.assertEqual(json.loads(self.metadata.read_text()), dict(functions={}, data={}))


if __name__ == '__main__':
    unittest.main()
