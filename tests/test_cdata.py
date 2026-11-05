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

    def test_named_array_bounds_across_headers(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'PAINT.H').write_text('''
enum Paint { UNASSIGNED = -1, RED, CYAN, GREEN, MAGENTA, BLUE, WHITE, PAINT_COUNT };
enum Shade { BASE, HIGHLIGHT, SHADE_COUNT = HIGHLIGHT + 1 };
struct PaintTable { char pairs[PAINT_COUNT][SHADE_COUNT]; int next; };
''')
            (root / 'ADVERT.H').write_text('''
#include "PAINT.H"
enum Advert { ADVERT_COUNT = WHITE };
struct Preview { char choices[ADVERT_COUNT]; int next; };
''')
            with patch.object(config, 'SRC', root):
                types = Types()
            table = types.declaration('char @[PAINT_COUNT][SHADE_COUNT]')
            self.assertEqual((table['count'], table['elem']['count'], table['size']), (6, 2, 12))
            self.assertEqual(types.named('struct PaintTable')['size'], 14)
            self.assertEqual(types.named('struct Preview')['fields'][1][1], 6)
            self.assertEqual(types.declaration('char @[+UNASSIGNED + 2]')['size'], 1)

    def test_invalid_named_array_bounds_fail_closed(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'BOUNDS.H').write_text('''
enum Bad { FIRST = SECOND, SECOND = FIRST, UNSUPPORTED = (1 < 2) };
''')
            with patch.object(config, 'SRC', root):
                types = Types()
            for name in ('UNKNOWN', 'FIRST', 'UNSUPPORTED'):
                with self.subTest(name=name), self.assertRaises(ValueError):
                    types.declaration('char @[' + name + ']')
