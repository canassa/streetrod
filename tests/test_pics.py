import base64
from contextlib import redirect_stdout
import hashlib
import io
from pathlib import Path
import struct
import sys
import tempfile
from types import SimpleNamespace
import unittest
import zlib

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import pics


class PictureTests(unittest.TestCase):
    def test_rle_literal_dictionary_and_long_runs(self):
        dictionary = bytes(range(16, 32))
        counts = (1, 1, 2, 3, 4, 2, 3, 5, 6, 6, 4, 7, 7, 5, 8, 8)
        values = (0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 0)
        data = bytes([0, 0, 255, 255, 18, 22, 42])
        expected = b'\0' + b'\xff' * 256 + b'\0' * 2 + b'\xff' * 3 + b'*'
        self.assertEqual(pics.decode_rle(data, dictionary, len(expected), counts, values), expected)
        self.assertEqual(pics.decode_rle(b'\0\0\xff\0', bytes(16), 2, counts, values), b'\0\xff')
        for data, size in ((b'\0', 1), (b'\xff\x02', 2), (b'\x2a', 2)):
            with self.subTest(data=data), self.assertRaises(ValueError):
                pics.decode_rle(data, dictionary, size, counts, values)

    def test_directory_offsets_shared_framing_and_unused_word(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'LIB1'
            a = bytearray(30)
            struct.pack_into('<HHH', a, 0, 8, 1, 999)
            struct.pack_into('<IH', a, 24, 0, 3)
            b = bytearray(a)
            struct.pack_into('<IH', b, 24, 3, 3)
            path.write_bytes(struct.pack('<H', 2) + a + b + b'\xbc\0\1\xbc\xff\1\xbc')
            first, second = pics.read_library(path)
            self.assertEqual((first.offset, second.offset), (62, 65))
            self.assertEqual(first.payload(), b'\0\1')
            self.assertEqual(second.payload(), b'\xff\1')
            self.assertEqual(pics.decode(first, 'cga').pixels, bytes([0, 0, 0, 255]) * 8)
            path.write_bytes(path.read_bytes()[:-1])
            with self.assertRaisesRegex(ValueError, 'framing'):
                second.payload()
            path.write_bytes(b'\x02\0')
            with self.assertRaisesRegex(ValueError, 'directory'):
                pics.read_library(path)

    def test_ids_and_cga_mapping(self):
        self.assertEqual(len(pics.picture_ids()), 277)
        self.assertEqual([pics.picture_index(n) for n in (1, 9, 1000, 1267)], [0, 8, 9, 276])
        self.assertEqual([pics.picture_index(n, 'cga') for n in (5, 6, 7, 8, 9)], [5, 6, 7, 4, 8])
        for n in (0, -1, 10, 999, 1268):
            with self.subTest(n=n), self.assertRaises(ValueError):
                pics.picture_index(n)

    def test_known_pictures(self):
        expected = {
            ('ega', 1): (320, 200, '5cdb9aed92df6f5dd88bf9652cf455f8f5f269ca303a48a2bfc99233de75e420'),
            ('ega', 1141): (240, 82, '1034c14ce0229505a06220d48569f1569207b8497f4c8f1498593e427c6c6d45'),
            ('ega', 1142): (240, 82, 'f770b578b7d0da33b0cef9948480a0cdd8683602c1c6884b5f07cdbf006d3fc4'),
            ('cga', 1): (320, 200, 'ae904b9fac1747d72042f9886fec43e563dbb52691fe773b45567ca888d6f834'),
        }
        for (mode, picture_id), (width, height, checksum) in expected.items():
            with self.subTest(mode=mode, id=picture_id):
                entries = pics.libraries(mode)
                picture = pics.decode(entries[pics.picture_index(picture_id, mode)], mode)
                self.assertEqual((picture.width, picture.height), (width, height))
                self.assertEqual(hashlib.sha256(picture.pixels).hexdigest(), checksum)
                if picture_id in (1141, 1142):
                    self.assertEqual(set(picture.pixels[3::4]), {0, 255})

    def test_all_resource_entries(self):
        for mode in ('ega', 'cga'):
            entries = pics.libraries(mode)
            self.assertEqual(len(entries), 277)
            for picture_id in pics.picture_ids():
                with self.subTest(mode=mode, id=picture_id):
                    entry = entries[pics.picture_index(picture_id, mode)]
                    picture = pics.decode(entry, mode)
                    self.assertEqual((picture.width, picture.height), (entry.width, entry.height))
                    self.assertEqual(len(picture.pixels), entry.width * entry.height * 4)
            if mode == 'cga':
                self.assertEqual(entries[-1].library.parts[-2:], ('extra', 'LIB2C'))

    def test_cell_order_scaling_and_stacking(self):
        pixels = b''.join(bytes([n, n, n, 255]) for n in range(16))
        picture = pics.Picture(4, 4, pixels)
        cells = pics.cut_cells(picture, 2, 3)
        self.assertEqual((cells.width, cells.height), (6, 2))
        self.assertEqual(list(cells.pixels[::4]), [0, 1, 2, 3, 8, 9, 4, 5, 6, 7, 12, 13])
        scaled = pics.scale(cells, 3)
        self.assertEqual((scaled.width, scaled.height), (18, 6))
        self.assertEqual(list(scaled.pixels[:72:4]), [n for n in (0, 1, 2, 3, 8, 9) for _ in range(3)])
        joined = pics.stack([cells, picture])
        self.assertEqual((joined.width, joined.height), (6, 6))
        self.assertEqual(joined.pixels[(2 * 6 + 4) * 4:(2 * 6 + 6) * 4], bytes(8))
        for cols, count in ((0, 1), (2, 0), (2, 5), (8, 1)):
            with self.subTest(cols=cols, count=count), self.assertRaises(ValueError):
                pics.cut_cells(picture, cols, count)
        with self.assertRaises(ValueError):
            pics.scale(picture, 0)

    def test_png_and_kitty_chunk_round_trip(self):
        picture = pics.Picture(80, 60, hashlib.shake_256(b'pixels').digest(80 * 60 * 4))
        encoded = pics.png(picture)
        self.assertEqual(encoded[:8], b'\x89PNG\r\n\x1a\n')
        pos, kinds, compressed = 8, [], bytearray()
        while pos < len(encoded):
            length = struct.unpack_from('>I', encoded, pos)[0]
            kind, data = encoded[pos + 4:pos + 8], encoded[pos + 8:pos + 8 + length]
            crc = struct.unpack_from('>I', encoded, pos + 8 + length)[0]
            self.assertEqual(crc, zlib.crc32(kind + data))
            kinds.append(kind)
            if kind == b'IHDR':
                self.assertEqual(struct.unpack('>IIBBBBB', data), (80, 60, 8, 6, 0, 0, 0))
            if kind == b'IDAT':
                compressed.extend(data)
            pos += 12 + length
        self.assertEqual(kinds, [b'IHDR', b'IDAT', b'IEND'])
        filtered = zlib.decompress(compressed)
        self.assertEqual(b''.join(filtered[y * 321 + 1:(y + 1) * 321] for y in range(60)), picture.pixels)
        chunks = pics.kitty(picture).split(b'\x1b\\')[:-1]
        self.assertGreater(len(chunks), 1)
        payloads = []
        for i, chunk in enumerate(chunks):
            control, payload = chunk.split(b';', 1)
            self.assertTrue(control.startswith(b'\x1b_G'))
            self.assertLessEqual(len(payload), 4096)
            self.assertTrue(control.endswith(b'm=0' if i == len(chunks) - 1 else b'm=1'))
            payloads.append(payload)
        self.assertIn(b'a=T,f=100', chunks[0])
        self.assertEqual(base64.b64decode(b''.join(payloads)), encoded)

    def test_output_and_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            args = SimpleNamespace(ids=[1142, 1141], scale=3, cells=[6, 11], list=False,
                                   mode='ega', palette='game', png=Path(tmp) / 'carhops.png')
            with redirect_stdout(io.StringIO()):
                pics.run(args)
            self.assertEqual(struct.unpack_from('>II', args.png.read_bytes(), 16), (1320, 246))
            for changes in ({'ids': []}, {'ids': [999]}, {'scale': 0}, {'cells': [2, 5]}):
                bad = SimpleNamespace(**(vars(args) | changes))
                with self.subTest(changes=changes), self.assertRaises(SystemExit):
                    pics.run(bad)


if __name__ == '__main__':
    unittest.main()
