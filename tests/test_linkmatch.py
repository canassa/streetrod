"""The linked-file gate must reject even a one-byte relocated-address change."""
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import linkmatch


def mz(image, relocs=()):
    header_size = 64
    total = header_size + len(image)
    words = [0x5a4d, total % 512, (total+511)//512, len(relocs), 4,
             0, 0xffff, 0, 0, 0, 0, 0, 0x1c, 0]
    header = bytearray(header_size)
    struct.pack_into('<14H', header, 0, *words)
    for i, pos in enumerate(relocs):
        struct.pack_into('<HH', header, 0x1c+4*i, pos%16, pos//16)
    return bytes(header)+image


class LinkedMatchTests(unittest.TestCase):
    def test_real_relocated_word_is_not_masked(self):
        with tempfile.TemporaryDirectory() as tmp:
            original, built = Path(tmp)/'ORIG.EXE', Path(tmp)/'SR.EXE'
            original.write_bytes(mz(b'\xb8\x34\x12\xcb', [1]))
            built.write_bytes(mz(b'\xb8\x35\x12\xcb', [1]))
            with patch.object(linkmatch.config, 'EXE', original), patch.object(linkmatch.config, 'ROOT', Path(tmp)):
                report = linkmatch.compare(built)
            self.assertFalse(report['file']['identical'])
            self.assertEqual(report['load_image']['different_bytes'], 1)
            self.assertTrue(report['relocations']['same_order'])

    def test_relocation_row_order_is_part_of_file_match(self):
        with tempfile.TemporaryDirectory() as tmp:
            original, built = Path(tmp)/'ORIG.EXE', Path(tmp)/'SR.EXE'
            original.write_bytes(mz(bytes(8), [0, 4]))
            built.write_bytes(mz(bytes(8), [4, 0]))
            with patch.object(linkmatch.config, 'EXE', original), patch.object(linkmatch.config, 'ROOT', Path(tmp)):
                report = linkmatch.compare(built)
            self.assertFalse(report['file']['identical'])
            self.assertTrue(report['load_image']['identical'])
            self.assertFalse(report['relocations']['same_order'])
            self.assertEqual(report['relocations']['missing_positions'], [])
            self.assertEqual(report['relocations']['extra_positions'], [])

    def test_trailing_bytes_fail(self):
        self.assertFalse(linkmatch.byte_diff(b'abc', b'abc\0')['identical'])
        self.assertEqual(linkmatch.byte_diff(b'abc', b'abc\0')['extra_bytes'], 1)


if __name__ == '__main__':
    unittest.main()
