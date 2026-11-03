"""A relocation-masked arithmetic wrapper is not proof of a library alias."""
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import config, lib
from sr.omf import Obj, Fixup


def record(kind, data):
    # The parser deliberately leaves OMF checksum validation to the DOS tools.
    return bytes([kind])+(len(data)+1).to_bytes(2, 'little')+data+b'\0'


class LibraryTests(unittest.TestCase):
    def test_original_division_wrapper_is_not_remainder(self):
        libs = [config.TOOLCHAIN/'LIB'/name for name in ('MLIBCR.LIB', 'MLIBFP.LIB', 'LIBH.LIB', 'EM.LIB')]
        found = lib.locate(libs)
        self.assertEqual({name for name, _ in found[0x206ca]}, {'aFNaldiv'})
        self.assertEqual({name for name, _ in found[0x21460]}, {'sin'})
        self.assertNotIn('sinh', {name for names in found.values() for name, _ in names})

    def test_empty_default_code_segment_keeps_real_publics(self):
        names = b''.join(bytes([len(s)])+s for s in (b'DEFAULT_TEXT', b'REAL_TEXT', b'CODE'))
        data = record(0x96, names)
        data += record(0x98, b'\x48\0\0\x01\x03\0')
        data += record(0x98, b'\x48\x01\0\x02\x03\0')
        data += record(0x90, b'\0\x02\x05_func\0\0\0')
        data += record(0xa0, b'\x02\0\0\xcb')
        data += record(0x8a, b'\0')
        obj = Obj(data)
        self.assertEqual(obj.code_seg, 2)
        self.assertEqual(obj.code, b'\xcb')
        self.assertEqual(obj.publics, {'_func': 0})

    def test_communal_consumes_an_external_index(self):
        data = record(0xb0, b'\x07_common\0\x62\x81\0\x02')
        data += record(0x8c, b'\x07_target\0')
        data += record(0x8a, b'\0')
        obj = Obj(data)
        self.assertEqual(obj.communals, {'_common': 512})
        self.assertEqual(obj.target_name(Fixup(1, 0, 1, True, 'ext', 2, 0)), '_target')


if __name__ == '__main__':
    unittest.main()
