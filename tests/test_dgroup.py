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
        self.assertIn((0x5580, 0x57F6, 'src/INPUT/KEYMAPS.ASM'), dgroup.DATA_UNITS)

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


class AggregateRegionTests(unittest.TestCase):
    def test_fallback_does_not_invent_an_alias_inside_a_source_record(self):
        for declared_alias in (False, True):
            with self.subTest(declared_alias=declared_alias):
                analysis = object.__new__(dgroup.Analysis)
                analysis.types = dgroup.Types()
                analysis.types.defs['struct DgroupProbe'] = (
                    'struct', 'int first; int unused; int last;')
                analysis.globals = {
                    0x100: {'name': 'g_probe', 'decl': 'struct DgroupProbe @'}}
                if declared_alias:
                    analysis.globals[0x102] = {'name': 'g_overlap', 'decl': 'int @'}
                analysis.goffs = sorted(analysis.globals)
                analysis.pools = {}
                analysis.dg = bytes(dgroup.DATA_END)
                analysis._masks = lambda: []
                analysis.pointer_targets = lambda items: set()
                regions = [(0x102, 0x104, 'int @'), (0x110, 0x112, 'int @')]
                with patch.object(dgroup, 'REGIONS', regions):
                    items = analysis._items()
                self.assertNotIn('g_0102', [item.name for item in items])
                outside = next(item for item in items if item.off == 0x110)
                self.assertEqual((outside.name, outside.size, outside.synth),
                                 ('g_0110', 2, True))
                self.assertEqual('g_overlap' in analysis.aliases, declared_alias)
                if declared_alias:
                    base, offset = analysis.aliases['g_overlap']
                    self.assertEqual((base.name, offset), ('g_probe', 2))
