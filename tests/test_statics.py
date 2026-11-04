"""File-local statics: the listing gives their _BSS offsets, a merged unit defines each once."""
from pathlib import Path
import sys
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import cod, module

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

    def test_merge_hoists_shared_statics_once(self):
        a = '#include "STRUCTS.H"\n\nstatic int g_x;   /* DS:6C4A */\n\nint f(void) { return g_x; }\n'
        b = '#include "STRUCTS.H"\n\nstatic int g_x;   /* DS:6C4A */\nstatic int g_y;\n\nint g(void) { return g_y; }\n'
        text = module.merge([a, b])
        self.assertEqual(text.count('static int g_x;'), 1)
        self.assertLess(text.index('static int g_y;'), text.index('int f(void)'))


if __name__ == '__main__':
    unittest.main()
