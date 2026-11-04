import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace as NS
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import cod, config, db, module

class SourceFileTests(unittest.TestCase):
    def test_one_source_per_object(self):
        # every code unit is one file in git, with one recorded command line
        plan = module.plan()
        for u in plan['units']:
            self.assertEqual(len(u['sources']), 1, u['start'])
            self.assertTrue((config.ROOT / u['sources'][0]).exists(), u['sources'][0])
            self.assertTrue(u['flags'].strip(), u['sources'][0])
        self.assertEqual(list(module.MODS.glob('*.STB')), [])

    def test_functions_point_at_their_file(self):
        con = db.connect()
        for f in module.files():
            rows = con.execute("SELECT src FROM functions WHERE status = 'matched' AND addr >= ? AND addr < ?",
                               (f.start, f.end)).fetchall()
            self.assertTrue(rows, f.rel)
            self.assertEqual({r['src'] for r in rows}, {f.rel})
        con.close()

    def test_command_line(self):
        fs = {f.path.name: f for f in module.files()}
        self.assertTrue(fs['M_0F38A.C'].command.endswith(' /NTS0F38_TEXT'))
        self.assertEqual(fs['M_24602.ASM'].command, '/Ml')   # MASM: the segment is in .CODE
        self.assertIn('.CODE S2460_TEXT', fs['M_24602.ASM'].path.read_text())

    def test_asm_extent_from_the_listing(self):
        # data between two PROCs (a flag byte in the code segment) belongs to neither
        with tempfile.TemporaryDirectory() as d:
            lst = Path(d) / 'T.LST'
            lst.write_text('_F_1DFA9 . . . . . . . . . . . .  \tN PROC\t0287\tS1C11_TEXT\tGlobal\tLength = 0028\n')
            lengths = cod.asm_lengths(lst)
        self.assertEqual(lengths, {'_F_1DFA9': 0x28})
        obj = NS(publics={'_F_1DFA9': 0x287, '_F_1DFD2': 0x2b0}, code=bytes(0x300))
        self.assertEqual(cod.from_publics(obj, 'F_1DFA9').end, 0x2b0)
        self.assertEqual(cod.from_publics(obj, 'F_1DFA9', lengths).end, 0x2af)


class GateTests(unittest.TestCase):
    def test_external_near_call_is_not_an_in_file_definition(self):
        # A public of another object precedes the member. Its EXTDEF fixup must remain
        # external, and must be counted even though raw 0000 points inside the member.
        fix = NS(seg=1, pos=5, kind='ext', loc=1, disp=0, self_rel=True, size=2)
        obj = NS(code=bytes.fromhex('cb909090e80000cb'), code_seg=1,
                 fixups=[fix], publics={'_stub': 0}, target_name=lambda f: '_stub')
        proc = NS(c_name='F_00100', name='_F_00100', start=4, end=8)
        row = dict(addr=0x100, off=0x100, size=4, name=None)
        img = bytearray(0x104); img[0x100:] = bytes.fromhex('e8fd00cb')
        gate = module.gate(obj, [proc], img, [row])
        self.assertTrue(gate.ok, gate.line('test'))
        self.assertEqual(gate.nearcalls, 1)
        self.assertEqual(gate.selfrefs, 0)

    def test_internal_relocation_must_match(self):
        fix = NS(seg=1, pos=1, kind='ext', loc=1, disp=0, self_rel=True, size=2)
        obj = NS(code=bytes.fromhex('e80000cbcb'), code_seg=1,
                 fixups=[fix], publics={'_F_00104': 4}, target_name=lambda f: '_F_00104')
        procs = [NS(c_name=f'F_{a:05X}', name=f'_F_{a:05X}', start=s, end=e)
                 for a,s,e in [(0x100,0,4),(0x104,4,5)]]
        rows = [dict(addr=a, off=a, size=n, name=None) for a,n in [(0x100,4),(0x104,1)]]
        img = bytearray(0x105); img[0x100:] = bytes.fromhex('e80100cbcb')
        self.assertTrue(module.gate(obj,procs,img,rows).ok)
        img[0x101] = 2
        self.assertFalse(module.gate(obj,procs,img,rows).ok)

if __name__ == '__main__':
    unittest.main()
