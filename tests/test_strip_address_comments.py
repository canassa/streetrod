from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from strip_address_comments import strip_text


class StripAddressTests(unittest.TestCase):
    def test_c_annotations_and_prose(self):
        text = ('/* f @ 0000:1234 (linear 0x01234, 4 bytes) */\n'
                '/* f @ 0000:1234 (linear 0x01234, 4 bytes)\n\n'
                '   Find the hot spot.\n   Keep this explanation. */\n'
                'int x;                 /* DS:554C */\n'
                'int f(void);           /* 0x01234 */\n'
                'extern int far a[];    /* 1234:5678 */\n'
                'static int b; /* DS:1234 PROVISIONAL filler */\n')
        result, count = strip_text(text)
        self.assertEqual(count, 6)
        self.assertIn('/* Find the hot spot.\n   Keep this explanation. */', result)
        self.assertIn('/* PROVISIONAL filler */', result)
        self.assertNotIn('DS:', result)
        self.assertNotIn('0x', result)
        self.assertTrue(all(line == line.rstrip() for line in result.splitlines()))
        self.assertEqual(strip_text(result), (result, 0))

    def test_unrelated_comments_and_literals(self):
        text = ('/* File-local storage (_BSS): names chosen to fit MSC hash order */\n'
                '/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02) */\n'
                '/* budget/hash note; PROVISIONAL */\n'
                'char *s = "/* DS:554C */";\n'
                '// /* DS:554C */\n')
        self.assertEqual(strip_text(text), (text, 0))

    def test_token_boundaries_and_crlf(self):
        result, count = strip_text('int/* DS:1234 */x;   /* DS:1234 */\r\n')
        self.assertEqual(result, 'int x;\r\n')
        self.assertEqual(count, 2)

    def test_masm_and_inc(self):
        text = ('; kg_key @ 0000:2DB6 (linear 0x02DB6, 13 bytes)\n'
                '_x DB 0          ; DS:47D0 char\n'
                '; DS:47D1 char[2]\n'
                '_msg DB "; DS:1234", 0 ; explanation\n'
                '; PROVISIONAL hash budget\n'
                '_y DB 0 ; DS:47D2 PROVISIONAL filler\n')
        result, count = strip_text(text, asm=True)
        self.assertEqual(count, 4)
        self.assertIn('_x DB 0\n', result)
        self.assertIn('_msg DB "; DS:1234", 0 ; explanation', result)
        self.assertIn('; PROVISIONAL hash budget', result)
        self.assertIn('_y DB 0 ; PROVISIONAL filler', result)
        self.assertEqual(strip_text(result, asm=True), (result, 0))


if __name__ == '__main__':
    unittest.main()
