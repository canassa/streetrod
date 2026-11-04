"""source.py: the symbol tables read from the sources, the headers and the EXE."""
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import config, module, source


class SourceTests(unittest.TestCase):
    def test_game_functions(self):
        fs = source.game_functions()
        self.assertEqual(len(fs), 572)
        self.assertEqual(sum(r['kind'] == 'c' for r in fs), 509)
        self.assertEqual(sum(r['kind'] == 'asm' for r in fs), 63)
        by = {r['name']: r for r in fs}
        self.assertEqual(by['garage_day'], dict(addr=0x06476, seg=0, off=0x6476, size=45, kind='c', far=1,
                                                name='garage_day', src='src/GAME/GARAGE.C', proto='int @(void)'))
        self.assertEqual(by['F_06B06']['far'], 0)
        self.assertEqual(by['F_06B06']['proto'], 'int near @(int mode)')
        self.assertEqual(by['cgaline_pixel_addr']['far'], 0)
        self.assertEqual(by['cgaline_draw']['far'], 1)
        # the functions of a file are contiguous: no other file's function inside its extent
        for f in module.files():
            self.assertEqual({r['src'] for r in fs if f.start <= r['addr'] < f.end}, {f.rel})

    def test_runtime(self):
        rt = [r for r in source.functions() if r['kind'] == 'crt']
        self.assertEqual(len(rt), 147)
        self.assertTrue(all(config.CRT[0] <= r['addr'] < config.CRT[1] for r in rt))
        self.assertEqual(source.resolve('1f686')['name'], 'free')
        self.assertIn('nfree', source.aliases()[0x1F686])
        self.assertEqual(source.global_offsets()['errno'], 0x630D)
        self.assertEqual(source.global_offsets()['_doserrno'], 0x6318)
        self.assertEqual(source.global_offsets()['_amblksiz'], 0x64A6)

    def test_globals(self):
        gs = source.globals_()
        self.assertEqual(gs[0x5030]['name'], 'g_whlPic')
        self.assertEqual(gs[0x5030]['decl'], 'int @[][4]')
        self.assertEqual(gs[0x5030]['header'], 'GARAGE')
        self.assertEqual(gs[0x6C36], dict(off=0x6C36, name='g_knobPos', decl='int @', header=None,
                                          static='src/GAME/DASH.C'))
        self.assertEqual(gs[0x6C42]['name'], 'g_6c42')
        far = {g['name']: g for g in source.fardata()}
        self.assertEqual((far['g_screen1']['seg'], far['g_screen1']['off'], far['g_screen1']['decl']),
                         (0x2E3E, 0x30, 'struct Bitmap far @'))

    def test_declared_name(self):
        self.assertEqual(source.declared_name('int (far *g_stepSpriteFrm)(struct Sprite *)'), 'g_stepSpriteFrm')
        self.assertEqual(source.declared_name('void (interrupt far *g_clockPrev)(void)'), 'g_clockPrev')
        self.assertEqual(source.declared_name('char *g_mon[]'), 'g_mon')
        self.assertEqual(source.declared_name('struct Bitmap far *g_adPicture'), 'g_adPicture')


if __name__ == '__main__':
    unittest.main()
