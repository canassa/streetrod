import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr.lint import lint


class LintTests(unittest.TestCase):
    def test_extern_at_file_scope(self):
        self.assertTrue(lint("extern int g;\nvoid f(void)\n{\n}\n"))

    def test_extern_at_block_scope(self):
        self.assertTrue(lint("void f(void)\n{\n    { extern int g; g = 1; }\n}\n"))
        self.assertTrue(lint("void f(void)\n{\n    extern int g;\n    g = '}';\n}\n"))

    def test_provisional_exception_only_in_a_block(self):
        self.assertEqual(lint("void f(int i)\n{\n    { extern char far x_trk0[][2]; i = x_trk0[i][0]; }\n}\n"), [])
        self.assertTrue(lint("extern char far x_trk0[][2];\n"))

    def test_calls_in_bodies_are_not_prototypes(self):
        self.assertEqual(lint("static void near g(int);\nvoid f(void)\n{\n    g(1);\n}\n"), [])


if __name__ == '__main__':
    unittest.main()
