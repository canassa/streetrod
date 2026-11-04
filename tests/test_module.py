import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace as NS
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import module
from sr.cli import _stb_lines

class ModuleTests(unittest.TestCase):
    def test_multiline_placeholders_are_indivisible(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / 'F.STB'
            p.write_text('@far_fn\nstatic void near local(void)\n{\n}\n')
            self.assertEqual(_stb_lines(p), ['@far_fn', 'static void near local(void)\n{\n}'])

    def test_asm_merge_and_split_preserve_interfaces(self):
        texts = [".MODEL MEDIUM\n.CODE\nEXTRN F_00010:NEAR\nPUBLIC _F_00000\n"
                 "_F_00000 PROC FAR\nagain: call F_00010\nret\n_F_00000 ENDP\nEND\n",
                 ".MODEL MEDIUM\n.CODE\nPUBLIC F_00010\nF_00010 PROC NEAR\n"
                 "again: ret\nF_00010 ENDP\nEND\n"]
        rows = [dict(addr=0, name=None, far=1), dict(addr=16, name=None, far=0)]
        merged = module.merge_asm(texts, rows)
        self.assertNotIn('EXTRN F_00010', merged)
        self.assertIn('L00000_again:', merged)
        self.assertIn('L00010_again:', merged)
        parts = module.split_asm(merged, rows)
        self.assertIn('EXTRN F_00010:NEAR', parts[0])
        again = module.merge_asm([parts[0], parts[16]], rows)
        self.assertNotIn('L00000_L00000', again)
        self.assertNotIn('EXTRN F_00010', again)

    def test_asm_merge_keeps_the_source_model(self):
        # .MODEL SMALL declares _TEXT first (M_21962.ASM): the merge must not replace it
        rows = [dict(addr=0, name=None, far=1)]
        merged = module.merge_asm([".MODEL SMALL\nX_TEXT SEGMENT\nX_TEXT ENDS\nEND\n"], rows)
        self.assertTrue(merged.lstrip().startswith('.MODEL SMALL'))
        self.assertNotIn('MEDIUM', merged)
        merged = module.merge_asm(["X_TEXT SEGMENT\nX_TEXT ENDS\nEND\n"], rows)
        self.assertTrue(merged.lstrip().startswith('.MODEL MEDIUM'))

    def test_placeholder_public_is_not_an_in_module_definition(self):
        # A public stub precedes the member. Its EXTDEF fixup must remain external,
        # and must be counted even though raw 0000 points inside the member.
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
