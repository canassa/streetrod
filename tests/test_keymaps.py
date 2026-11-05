"""Keep the C menu ids/results and the MASM bindings in agreement."""
import ast
import re
import unittest
from pathlib import Path

SRC = Path(__file__).resolve().parents[1] / 'src'


def value(expr, names):
    expr = re.sub(r'\b([0-9][0-9a-f]*)h\b', r'0x\1', expr, flags=re.I)

    def evaluate(node):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.Name):
            return names[node.id]
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
            return -evaluate(node.operand)
        if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub)):
            a, b = evaluate(node.left), evaluate(node.right)
            return a + b if isinstance(node.op, ast.Add) else a - b
        raise ValueError(expr)

    return evaluate(ast.parse(expr.strip(), mode='eval').body)


def c_enums(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    names = {name: value(expr, {}) for name, expr in
             re.findall(r'^#define (KEY_VALUE_BASE) (0x[0-9a-f]+)$', text, re.M)}
    for body in re.findall(r'\benum\s+\w+\s*\{([^}]+)\}', text, re.S):
        next_value = 0
        for member in body.split(','):
            name, sep, expr = member.strip().partition('=')
            if not name:
                continue
            names[name.strip()] = value(expr, names) if sep else next_value
            next_value = names[name.strip()] + 1
    return names


def asm_equ(text):
    names = {}
    for name, expr in re.findall(r'^(\w+)\s+EQU\s+([^;\n]+)', text, re.M):
        names[name] = value(expr, names)
    return names


class KeyMapAgreementTests(unittest.TestCase):
    def setUp(self):
        self.header = (SRC / 'INPUT/KEYMAPS.H').read_text()
        self.asm = (SRC / 'INPUT/KEYMAPS.ASM').read_text()
        self.c = c_enums(self.header)
        self.equ = asm_equ(self.asm)

    def test_screen_ids_match_pointer_table_slots(self):
        table = self.asm.split('_g_keyMaps DW', 1)[1]
        count = int(table.split()[0])
        labels = re.findall(r'OFFSET DGROUP:(\w+)', table)
        self.assertEqual(len(labels), count)
        self.assertEqual(self.c['KM_NONE'], 0)
        screens = {n: v for n, v in self.c.items() if n.startswith('KM_') and v}
        self.assertEqual(len(screens), count)
        self.assertEqual(set(screens.values()), set(range(1, count + 1)))
        for name, index in screens.items():
            self.assertEqual(labels[index - 1], name.replace('KM_', 'K_', 1), name)
            self.assertRegex(self.asm, r'(?m)^' + labels[index - 1] + r' LABEL BYTE$')

    def test_decoded_results_and_raw_patch_bytes_agree(self):
        for name, result in self.c.items():
            if name in {'KR_TIMEOUT', 'KR_EMPTY_LIST', 'KR_ANY_INPUT'}:
                self.assertNotIn(name, self.equ)
                continue
            if name.startswith('KR_'):
                self.assertIn(name, self.equ)
                self.assertEqual(result, self.equ[name], name)
            elif name.startswith('KV_'):
                decoded = name.replace('KV_', 'KR_', 1)
                self.assertEqual(result, self.equ['KEY_VALUE_BASE'] + self.equ[decoded], name)
        self.assertEqual(self.c['KEY_VALUE_BASE'], self.equ['KEY_VALUE_BASE'])
        self.assertEqual({n: self.c[n] for n in
                          ('KR_TIMEOUT', 'KR_EMPTY_LIST', 'KR_ANY_INPUT')},
                         {'KR_TIMEOUT': 1000, 'KR_EMPTY_LIST': 1002, 'KR_ANY_INPUT': 1003})
        self.assertEqual({n for n in self.c if n.startswith('KV_')},
                         {'KV_REPAIR_FIX', 'KV_REPAIR_JUNK'})
        for result in re.findall(r'\bKEY SC_\w+,\s+(KR_\w+)', self.asm):
            self.assertIn(result, self.equ)
        self.assertEqual(self.equ['KR_TIMING_ADVANCE'], self.equ['KR_SCROLL_DOWN'])
        self.assertEqual(self.equ['KR_TIMING_RETARD'], self.equ['KR_SCROLL_UP'])

    def test_all_negative_decoded_results_are_available_in_c(self):
        c = {n for n, v in self.c.items() if n.startswith('KR_') and v < 0}
        asm = {n for n, v in self.equ.items() if n.startswith('KR_') and v < 0}
        self.assertTrue(asm)
        self.assertEqual(c, asm)

    def test_hardware_scan_codes_agree_in_c_and_masm(self):
        header = (SRC / 'SYS/PC.H').read_text()
        c = {name: value(expr, {}) for name, expr in
             re.findall(r'^#define (SC_\w+)\s+(\S+)', header, re.M)}
        inc = (SRC / 'SYS/PC.INC').read_text()
        asm = {name: value(expr, {}) for name, expr in
               re.findall(r'^(SC_\w+)\s+EQU\s+(\S+)', inc, re.M)}
        self.assertTrue(c)
        self.assertEqual(c, asm)


class HotspotAgreementTests(unittest.TestCase):
    def setUp(self):
        header = (SRC / 'UI/HOTSPOT.H').read_text()
        self.c = c_enums(header)
        self.equ = asm_equ((SRC / 'UI/HOTSPOT.INC').read_text())
        source = (SRC / 'UI/HOTSPOT.C').read_text()
        self.records = [[field.strip() for field in row.split(',')] for row in
                        re.findall(r'\{(HOT_\w+,[^{}]+)\}', source)]

    def test_c_and_masm_hotspot_indexes_agree(self):
        self.assertTrue(self.equ)
        self.assertEqual({n for n in self.c if n.startswith('HOTS_')}, self.equ.keys())
        for name, index in self.equ.items():
            self.assertEqual(self.c[name], index, name)
        self.assertEqual(len(self.records), self.c['HOTS_COUNT'])

    def test_screen_slots_select_their_own_headers(self):
        maps = c_enums((SRC / 'INPUT/KEYMAPS.H').read_text())
        table = (SRC / 'UI/MESSAGES.ASM').read_text().split('_g_groupHot DW', 1)[1]
        table = table.split('PUBLIC', 1)[0]
        slots = re.findall(r'\bHOTS_\w+', table)
        self.assertEqual(len(slots), maps['KM_END_MENU'] + 1)
        for name, slot in maps.items():
            if not name.startswith('KM_') or name == 'KM_MUSIC':
                continue
            expected = name.replace('KM_', 'HOTS_', 1)
            self.assertEqual(slots[slot], expected, name)
            if name != 'KM_NONE':
                self.assertIn(self.records[self.c[expected]][0],
                              {'HOT_MENU', 'HOT_LIST_MENU', 'HOT_LIST_NO_FRAME'}, name)

    def test_links_stay_within_their_screen(self):
        starts = sorted(index for name, index in self.equ.items()
                        if name not in {'HOTS_NONE', 'HOTS_ENGINE_TEMPLATE', 'HOTS_COUNT'})
        self.assertEqual(starts, [i for i, row in enumerate(self.records)
                                  if row[0] in {'HOT_MENU', 'HOT_LIST_MENU', 'HOT_LIST_NO_FRAME'}])
        for start, end in zip(starts, starts[1:] + [len(self.records)]):
            for row in self.records[start:end]:
                links = [row[8]]
                if row[0] == 'HOT_LINK_BOX':
                    links.append(row[3])
                for expr in links:
                    target = value(expr, self.c)
                    self.assertTrue(target == 0 or start <= target < end,
                                    (start, end, expr))
