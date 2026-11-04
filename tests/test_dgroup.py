import sys
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import dgroup, fileheaders, source


class KeymapModuleTests(unittest.TestCase):
    def test_data_header_belongs_to_the_data_only_source(self):
        owners = dict(fileheaders.owners_layout())
        path, = owners['KEYMAPS']
        names = {'g_keyMapData', 'g_keyMaps'}
        self.assertEqual(fileheaders._defines(path, names), names)
        self.assertEqual(fileheaders._defines(owners['HERCDET'][0], names), set())
        self.assertEqual({r['header'] for r in source.headers()['globals'] if r['name'] in names},
                         {'KEYMAPS'})
        self.assertIn((0x5580, 0x57F6, 'src/mods/KEYMAPS.ASM'), dgroup.DATA_UNITS)

    def test_relocation_order_keeps_hercules_before_music_without_its_data(self):
        perf, maps, herc, music, hercgfx = 0xDCBE, 0x105580, 0xE56A, 0x1628A, 0xE5D2
        analysis = SimpleNamespace(
            order=[perf, maps, music, herc, hercgfx],
            unit_by={s: {'region': 0 if s in (perf, herc, hercgfx) else None}
                     for s in (perf, maps, herc, music, hercgfx)})
        hints = [h for h in dgroup.ORDER_HINTS if '0E56A' in h[0]]
        self.assertTrue(hints)
        with patch.object(dgroup, 'ORDER_HINTS', hints):
            self.assertEqual(dgroup.Analysis.link_order(analysis),
                             [perf, maps, herc, music, hercgfx])
