import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import config
from sr.cdata import Types


class DataTypesTests(unittest.TestCase):
    def test_module_header_records_enums_and_typedefs(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'SYS').mkdir()
            (root / 'GAME').mkdir()
            (root / 'SYS' / 'STRUCTS.H').write_text('struct Shared { int value; };\n')
            (root / 'GAME' / 'PRIVATE.H').write_text('''
#include "STRUCTS.H"
enum Mode { IDLE, ACTIVE };
typedef unsigned char ModeCode;
typedef struct Private { struct Shared shared; enum Mode mode; ModeCode code; } Private;
typedef union { int word; char bytes[2]; } Value;
''')
            with patch.object(config, 'SRC', root):
                types = Types()
            self.assertEqual([p.name for p in types.headers], ['PRIVATE.H', 'STRUCTS.H'])
            self.assertEqual(types.named('struct Private')['size'], 6)
            self.assertEqual([(n, o) for n, o, _ in types.named('Private')['fields']],
                             [('shared', 0), ('mode', 2), ('code', 4)])
            self.assertEqual(types.named('enum Mode')['size'], 2)
            self.assertEqual(types.named('ModeCode')['size'], 1)
            self.assertEqual(types.named('Value')['size'], 2)
