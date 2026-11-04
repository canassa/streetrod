"""File-local statics: the listing gives their _BSS offsets, a source file defines each once."""
from pathlib import Path
import sys
import re
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import cod, fileheaders, module

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

    def test_each_static_is_defined_once(self):
        # one file per object: a static is not repeated, and no two files share one
        owner = {}
        for f in module.files():
            if f.kind != 'c':
                continue
            text = f.path.read_text(encoding='latin-1')
            names = fileheaders.file_statics(f.path)
            for n in names:
                self.assertEqual(len(re.findall(rf'^static [^;(]*\b{n}\b', text, re.M)), 1, (f.rel, n))
                self.assertNotIn(n, owner, (f.rel, n))
                owner[n] = f.rel
        self.assertGreater(len(owner), 100)


if __name__ == '__main__':
    unittest.main()
