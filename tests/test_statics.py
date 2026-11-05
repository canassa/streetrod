"""File-local statics: the listing gives their _BSS offsets, a source file defines each once."""
from pathlib import Path
import sys
import re
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import cod, config, fileheaders, source

LISTING = """_BSS      SEGMENT\r
COMM NEAR\t_bn7648:\tBYTE:\t 2\r
EXTRN\t_g7d84o:WORD\r
\tORG\t$+26\r
$S1619_g_6ca2\tDW 01H DUP (?)\r
\tORG\t$-28\r
$S1617_g_6c86\tDW 01H DUP (?)\r
$S1618_g_6c8aaav\tDW 0cH DUP (?)\r
\tORG\t$+2\r
_g_rngA\tDB 030H DUP (?)\r
_BSS      ENDS\r
"""


class StaticsTests(unittest.TestCase):
    def test_listing_offsets(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'T.COD'
            p.write_text(LISTING)
            self.assertEqual(cod.statics(p), {26: ('g_6ca2', 2), 0: ('g_6c86', 2),
                                              2: ('g_6c8aaav', 24), 28: ('g_rngA', 48)})

    def test_initialized_data_labels(self):
        listing = """_DATA SEGMENT WORD PUBLIC 'DATA'
_DATA ENDS
_DATA SEGMENT
$SG12 DB 'a,b', 00H
    ORG $+2
$S20_table DW 02H
    DW 3 DUP(0)
    ORG $-14
_public DD 00H
_DATA ENDS
_DATA SEGMENT
$S21_tail DB 2 DUP(3 DUP(0))
_DATA ENDS
"""
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'DATA.COD'
            path.write_text(listing)
            self.assertEqual(cod.data_labels(path), {'table': 6, 'public': 0, 'tail': 4})

    def test_initialized_static_declarators(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'DATA.C'
            path.write_text('static int table[2];\n'
                            'static int table[2] = {1, 2};\n'
                            'static char *message = "hello";\n'
                            'static void near helper(char *arg);\n'
                            'static void (far *hook)(void) = 0;\n')
            self.assertEqual(dict(source.static_declarations(path)),
                             {'table': 'int table[2]', 'message': 'char *message',
                              'hook': 'void (far *hook)(void)'})

    def test_each_static_is_defined_once(self):
        # one file per object: a static is not repeated, and no two files share one
        owner = {}
        for path in source.source_files():
            if path.suffix.upper() != '.C':
                continue
            text = path.read_text(encoding='latin-1')
            rel = path.relative_to(config.ROOT).as_posix()
            names = fileheaders.file_statics(path)
            for n in names:
                self.assertEqual(len(re.findall(rf'^static [^\n;=]*\b{n}\b[^\n;=]*(?:=|;)', text, re.M)),
                                 1, (rel, n))
                self.assertNotIn(n, owner, (rel, n))
                owner[n] = rel
        self.assertTrue(owner)
        self.assertEqual(owner, {g['name']: g['static'] for g in source.statics()})


if __name__ == '__main__':
    unittest.main()
