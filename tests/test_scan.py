"""Regression fixtures for scanner gaps, using the shipped image as ground truth."""
import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr.scan import gap_entries, scan

class GapTests(unittest.TestCase):
    def test_frameless_leaf(self):
        self.assertEqual(gap_entries(bytes.fromhex('c606d04700cb'), 0, 6, 0), [0])

    def test_reject_data_and_unfinished_code(self):
        for raw in ('0000cb', 'b80000', 'eb03cb', 'ffe0', '7401b8cb00'):
            b = bytes.fromhex(raw)
            self.assertEqual(gap_entries(b, 0, len(b), 0), [])

    def test_multiple_routines_and_alignment(self):
        b = bytes.fromhex('b80100cbb80200cb9000')
        self.assertEqual(gap_entries(b, 0, len(b), 0), [0, 4])

    def test_real_image(self):
        rows = {r['addr']: r for r in scan()}
        expected = {0x2db0: 6, 0xfead: 18, 0xfebf: 18, 0x1636c: 21,
                    0x16ada: 48, 0x16b0a: 48, 0x1d080: 36, 0x1d0a4: 36,
                    0x23f4d: 26}
        for addr, size in expected.items():
            self.assertEqual(rows[addr]['size'], size, hex(addr))
        self.assertNotIn(0xf8c2, rows)
        self.assertEqual(rows[0xf7a5]['size'], 0xf972 - 0xf7a5)
        self.assertFalse(any(0xe9ef <= a < 0xf38a for a in rows))

if __name__ == '__main__':
    unittest.main()
