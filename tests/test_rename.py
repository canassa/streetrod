"""`./sr rename`: whole-word replacement (C and MASM spellings) and the name checks."""
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import rename, source


class RenameTests(unittest.TestCase):
    def test_whole_words_in_c_and_masm(self):
        pat = rename._pattern({'F_00100': 'menu_draw', 'g_hercules': 'g_count'})
        text = ('EXTRN _F_00100:FAR\n call _F_00100\n F_00100(g_hercules, g_00420, xF_00100, $S1_g_0042);\n'
                '/* F_00100 @ 0000:0100 */')
        self.assertEqual(rename._sub(pat, {'F_00100': 'menu_draw', 'g_hercules': 'g_count'}, text),
                         'EXTRN _menu_draw:FAR\n call _menu_draw\n menu_draw(g_count, g_00420, xF_00100, $S1_g_0042);\n'
                         '/* menu_draw @ 0000:0100 */')

    def test_file_stems(self):
        pat = rename._pattern({'FAME': 'PROTECT'}, prefix=False)
        self.assertEqual(rename._sub(pat, {'FAME': 'PROTECT'}, 'MODS\\FAME.C FAME.OBJ _M_02FC8'),
                         'MODS\\PROTECT.C PROTECT.OBJ _M_02FC8')

    def test_pairs(self):
        self.assertEqual(rename.parse_pairs(['# c', 'dos_print a  # why', '', 'FAME.C B.C']),
                         [('dos_print', 'a'), ('FAME.C', 'B.C')])

    def test_refuses_taken_and_bad_names(self):
        used = next(r['name'] for r in source.functions() if r['name'] == 'main')
        for pairs in ([('dos_print', used)], [('dos_print', 'MAIN')], [('dos_print', '1x')],
                      [('FAME.C', 'protect.c')], [('FAME.C', 'PROTECT.ASM')], [('no_such_name', 'x_y')]):
            with self.assertRaises(SystemExit, msg=pairs):
                rename.rename(pairs, dry=True)

    def test_fit_windows(self):
        self.assertEqual(rename.static_key('g_score'), 34)
        common = [g['name'] for off, g in source.globals_().items() if 0x6E60 <= off < 0x8F02]
        lines = rename.fit(common[1], [common[1], 'zz_unlikely_name'])
        self.assertIn('window', lines[0])
        self.assertTrue(lines[1].endswith('fits') or lines[1].endswith('(build decides)'), lines)


if __name__ == '__main__':
    unittest.main()
