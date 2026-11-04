import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import config, fileheaders, source


class MacroHeaderTests(unittest.TestCase):
    def test_standalone_macros_follow_source_usage(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'PC.H').write_text('#ifndef PC_H\n#define PC_H\n'
                                     '#define BIOS_TICKS ((long far *)0x46cL)\n#endif\n')
            used = root / 'USED.C'
            unused = root / 'UNUSED.C'
            used.write_text('#include "STRUCTS.H"\nlong tick = *BIOS_TICKS;\n')
            unused.write_text('#include "STRUCTS.H"\n#include "PC.H"\n'
                              'char *s = "BIOS_TICKS";\n')
            with patch.object(config, 'SRC', root), \
                    patch.object(fileheaders, 'owners_layout', return_value=[('USED', [used]), ('UNUSED', [unused])]), \
                    patch.object(source, 'headers', return_value=dict(globals=[], fardata=[], functions=[])):
                self.assertEqual(fileheaders.generate(), [])
                self.assertIn('#include "PC.H"', used.read_text())
                self.assertNotIn('#include "PC.H"', unused.read_text())
                before = used.read_bytes(), unused.read_bytes()
                self.assertEqual(fileheaders.generate(), [])
                self.assertEqual((used.read_bytes(), unused.read_bytes()), before)
                used.write_text(used.read_text().replace('*BIOS_TICKS', '0L'))
                self.assertEqual(fileheaders.generate(), [])
                self.assertNotIn('#include "PC.H"', used.read_text())

    def test_macro_header_without_owner_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'UNKNOWN.H').write_text('#define FIXED_ADDR ((char far *)0x80000000L)\n')
            with patch.object(config, 'SRC', root), \
                    patch.object(fileheaders, 'owners_layout', return_value=[]), \
                    patch.object(source, 'headers', return_value=dict(globals=[], fardata=[], functions=[])):
                self.assertEqual(fileheaders.generate(),
                                 ['UNKNOWN.H: no source file UNKNOWN.C or UNKNOWN.ASM'])

    def test_module_header_constants_follow_callers_without_source_definitions(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'OWNED.H').write_text('#ifndef OWNED_H\n#define OWNED_H\n'
                                         '#define SHOW (-1)\n#define HIDE (-2)\n#endif\n')
            owned = root / 'OWNED.C'
            caller = root / 'CALLER.C'
            owned.write_text('#include "STRUCTS.H"\nint value;\n')
            caller.write_text('#include "STRUCTS.H"\nint mode = SHOW;\n')
            with patch.object(config, 'SRC', root), \
                    patch.object(fileheaders, 'owners_layout', return_value=[('OWNED', [owned]), ('CALLER', [caller])]), \
                    patch.object(source, 'headers', return_value=dict(globals=[], fardata=[], functions=[])):
                self.assertEqual(fileheaders.generate(), [])
                self.assertIn('#include "OWNED.H"', caller.read_text())
                self.assertIn('#include "OWNED.H"', owned.read_text())
                before = owned.read_bytes(), caller.read_bytes()
                self.assertEqual(fileheaders.generate(), [])
                self.assertEqual((owned.read_bytes(), caller.read_bytes()), before)
                caller.write_text(caller.read_text().replace('SHOW', '0'))
                self.assertEqual(fileheaders.generate(), [])
                self.assertNotIn('#include "OWNED.H"', caller.read_text())

    def test_duplicate_module_macro_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for stem in ('A', 'B'):
                (root / (stem + '.H')).write_text('#define MODE 1\n')
                (root / (stem + '.C')).write_text('#include "STRUCTS.H"\n')
            with patch.object(config, 'SRC', root), \
                    patch.object(fileheaders, 'owners_layout', return_value=[(stem, [root / (stem + '.C')]) for stem in ('A', 'B')]), \
                    patch.object(source, 'headers', return_value=dict(globals=[], fardata=[], functions=[])):
                self.assertEqual(fileheaders.generate(), ['MODE is declared in A.H and B.H'])

    def test_module_declaration_still_requires_source_definition(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'OWNED.H').write_text('#define MODE 1\nextern int missing;\n')
            owned = root / 'OWNED.C'
            owned.write_text('#include "STRUCTS.H"\n')
            with patch.object(config, 'SRC', root), \
                    patch.object(fileheaders, 'owners_layout', return_value=[('OWNED', [owned])]), \
                    patch.object(source, 'headers', return_value=dict(globals=[dict(name='missing', header='OWNED')], fardata=[], functions=[])):
                self.assertEqual(fileheaders.generate(), ['missing: declared in OWNED.H, but OWNED.C does not define it'])
