"""Recover DGROUP ownership from positions, and write the C (or MASM) data definitions.

MSC 5.1 emits a module's _DATA as: string literals immediately in parse order (an initializer's
strings at the declaration, a function's literals while it compiles), and named initialized
data buffered until the next function is emitted, then flushed after that function's literals,
each item word-aligned when its type is. LINK concatenates the modules' _DATA in object order,
so the original offsets of a unit's literal pools pin the unit in the link order, and a named
item's offset tells which function it was declared before.

Reads the original image, the DB and the layout build's objects (`./sr build --layout`, for
the literal pools). `apply` writes definitions into src/mods/; `dgroup_asm` writes the data
that no source owns yet to src/data/DGROUP.ASM.
"""
import bisect
import json
import re
import struct
from pathlib import Path

from . import config, db
from .cdata import Types
from .exe import image
from .omf import Obj

DATA_END = 0x629C          # first runtime _DATA byte (crt0's _asizds)
BSS_START, BSS_PRIVATE_END, COMMON_START, END = 0x6C22, 0x6E2C, 0x6E60, 0x8F02
# Recovered array bounds and typed table pieces (from the earlier data audit).
BOUNDS = {'g_videoMenu': 6, 'g_msgs': 7, 'g_palette': 16, 'g_0692': 16, 'g_5082': 4, 'g_511e': 4,
          'g_579f': 43, 'g_6078': 21, 'g_4766': 0x66, 'g_4806': 44, 'g_4998': 5, 'ae7312y': 5, 'bp7316': 4}
REGIONS = [(0x6076, 0x6078, 'int @'), (0x2ee, 0x31e, 'signed char @[24][2]'), (0x358, 0x36e, 'signed char @[11][2]'),
           (0x462, 0x472, 'unsigned char @[16]'), (0x5d4, 0x5d6, 'int @'),
           (0x11b2, 0x121c, 'unsigned @[53]'), (0x1ac4, 0x1d8a, 'unsigned @[355]'), (0x1e2c, 0x1e38, 'unsigned @[6]'),
           (0x1e3a, 0x20aa, 'unsigned @[312]'), (0x4734, 0x4736, 'int @'), (0x474a, 0x474c, 'int @'),
           (0x47d1, 0x47d4, 'char @[3]'), (0x4804, 0x4806, 'int @'), (0x496c, 0x4970, 'int @[2]')]
# Data no C compiler could place (a pointer table at an odd address): assembler-defined,
# owned by the assembly unit that sits there in the link order.
FORCE_OWNER = [(0x5580, 0x57F6, 0x0E56A),
               # keyboard/mouse state and tables shared with the assembler keyboard handlers
               (0x4718, 0x4AFE, 0x02B9A),
               # driver hook tables: C data of the driver file that follows each assembler
               # stub (descending relocations in the original: compiler-emitted, not MASM)
               (0x5B04, 0x5B40, 0x1C2CA), (0x5B8A, 0x5BC6, 0x22364), (0x5BD0, 0x5C0C, 0x22B3A)]
# Data-only translation units: tables a code unit cannot have held (MSC's frame layout of a
# unit's functions changes with the number of file-scope definitions; these functions match
# only without them). Each gets a synthetic unit key above any code address.
# File names describe content; the historical names are unknown.
DATA_UNITS = [(0x09F4, 0x239E, 'src/mods/HOTSPOT.C'),
              # odd-address ints and arrays, count-prefixed message records: not C-placeable
              (0x239E, 0x4718, 'src/mods/MESSAGES.ASM'),
              # MSC emits a global whose address an earlier initializer takes before that
              # initializer: the table at DS:6078 precedes the second set, so two files
              (0x5EC6, 0x60A2, 'src/mods/CARMASKS.C'),
              (0x60A2, 0x629C, 'src/mods/CARMASK2.C')]
DATA_KEY = 0x100000
PRINTABLE = set(range(32, 127)) | {9, 10, 13}


# start of the hand-kept uninitialized declarations in a data-only unit source
COMMUNAL_MARK = '/* Uninitialized storage: communals'


class Item:
    __slots__ = ('off', 'size', 'kind', 'name', 'decl', 'node', 'unit', 'fn', 'owner', 'slot', 'note', 'synth')

    def __init__(self, off, size, kind, name='', decl='', node=None, unit=None, fn=None, synth=False):
        self.off, self.size, self.kind, self.name, self.decl, self.node = off, size, kind, name, decl, node
        self.unit, self.fn, self.owner, self.slot, self.note, self.synth = unit, fn, None, None, '', synth

    @property
    def end(self):
        return self.off + self.size

    def __repr__(self):
        return f'{self.off:04x}+{self.size} {self.kind} {self.name}'


SOURCE_DECLS = {}


def defined_in_sources():
    """global name -> source file, for globals a C or MASM module already defines.
    SOURCE_DECLS keeps each C definition's declaration (with @ for the name)."""
    out = {}
    for path in sorted((config.SRC / 'mods').glob('*.C')):
        text = re.sub(r'/\*.*?\*/', '', path.read_text(errors='replace'), flags=re.S)
        for m in re.finditer(r'(?m)^(?!extern\b|static\b|#)([A-Za-z_][\w *(]*?)\b(g_\w+)\s*((?:\[[^\]]*\]\s*)*)(?:\)\s*\([^)]*\))?\s*=', text):
            out[m.group(2)] = str(path.relative_to(config.ROOT))
            SOURCE_DECLS[m.group(2)] = (m.group(1) + '@' + m.group(3)).replace(' @', ' @').strip()
    for path in sorted((config.SRC / 'mods').glob('*.ASM')):
        text = path.read_text(errors='replace')
        for m in re.finditer(r'(?m)^\s*PUBLIC\s+_(g_\w+)', text):
            if re.search(rf'(?m)^_{m.group(1)}\s+(LABEL|D[BWDQ])\b', text):
                out[m.group(1)] = str(path.relative_to(config.ROOT))
    return out


def _marked(lines, i, pat):
    """Does the top-level statement starting at line i end with the generator's marker?"""
    depth, inq = 0, False
    for j in range(i, min(i + 400, len(lines))):
        s = lines[j]
        k = 0
        while k < len(s):
            c = s[k]
            if inq:
                if c == '\\':
                    k += 1
                elif c == '"':
                    inq = False
            elif c == '"':
                inq = True
            elif c == '{':
                depth += 1
            elif c == '}':
                depth -= 1
            elif c == ';' and depth == 0:
                return bool(pat.search(s[k:]))
            k += 1
    return False


class Analysis:
    def __init__(self):
        self.con = db.connect()
        self.im = image()
        self.types = Types()
        self.ds = config.DGROUP_SEG * 16
        self.dg = bytes(self.im.img[self.ds:self.ds + END])
        self.plan = json.loads((config.SRC / 'link/LAYOUT.JSON').read_text())
        self.globals = {r['off']: dict(r) for r in self.con.execute('SELECT * FROM globals ORDER BY off')}
        self.goffs = sorted(self.globals)
        self.fardata = [(r['seg'], r['off'], r['name'], r['decl']) for r in
                        self.con.execute('SELECT seg, off, name, decl FROM fardata ORDER BY seg, off')]
        self.funcs = {r['addr']: dict(r) for r in self.con.execute('SELECT * FROM functions ORDER BY addr')}
        self.faddrs = sorted(self.funcs)
        self.units = [dict(u) for u in self.plan['units']]
        self.code_starts = sorted(self.plan['code_starts'])
        for u in self.units:
            u['region'] = max(c for c in self.code_starts if c <= u['start'])
            u['functions'] = [self.funcs[a] for a in self.faddrs if u['start'] <= a < u['end']
                              and self.funcs[a]['status'] == 'matched']
        for lo, hi, src in DATA_UNITS:
            self.units.append(dict(start=DATA_KEY + lo, end=DATA_KEY + hi, kind='data', sources=[src],
                                   region=None, functions=[], lo=lo, hi=hi))
        self.unit_by = {u['start']: u for u in self.units}
        self.defined = defined_in_sources()
        self.lit_sizes = {}
        self.pools = self._pools()
        self.items = self._items()
        self.by_off = {it.off: it for it in self.items}
        self.gitems = {it.off: it for it in self.items if it.kind == 'global'}
        self.goffs2 = sorted(self.gitems)
        self._assign_owners()
        self._assign_slots()

    # ------------------------------------------------------------ literal pools
    def fn_at(self, addr):
        i = bisect.bisect_right(self.faddrs, addr) - 1
        return self.funcs[self.faddrs[i]]

    def _decl_size(self, row):
        """A global's extent from its source definition, its declaration, its recovered bound,
        or the next global."""
        if row['name'] in SOURCE_DECLS:
            try:
                node = self.types.declaration(SOURCE_DECLS[row['name']])
                if node['size']:
                    return node['size']
            except ValueError:
                pass
        if not row['decl']:
            return 1
        node = self.types.declaration(row['decl'])
        if node['size']:
            return node['size']
        esize = node['elem']['size'] if node['kind'] == 'array' else 1
        if row['name'] in BOUNDS:
            return BOUNDS[row['name']] * esize
        i = bisect.bisect_right(self.goffs, row['off'])
        nxt = self.goffs[i] if i < len(self.goffs) else DATA_END
        return max(1, nxt - row['off'])

    def _public_extents(self, obj, dseg):
        """[(lo, hi)] of named data the unit defines itself."""
        out = []
        byname = {'_' + g['name']: g for g in self.globals.values()}
        for name, (seg, off) in obj.all_publics.items():
            if seg != dseg:
                continue
            row = byname.get(name)
            size = self._decl_size(row) if row else 1
            out.append((off, off + size))
        return out

    @staticmethod
    def _unit_obj(u):
        stem = f'U_{u["start"]:05X}' if u['kind'] != 'data' else f'D_{u["lo"]:05X}'
        fresh = config.WORK / 'layout' / 'BUILD' / stem / (stem + '.OBJ')
        gated = config.WORK / 'layout' / (stem + '.OBJ')
        if fresh.exists() and (not gated.exists() or fresh.stat().st_mtime >= gated.stat().st_mtime):
            return fresh
        if gated.exists():
            return gated
        raise SystemExit(f'{gated} missing: run ./sr build --layout first')

    @staticmethod
    def _cod_literal_sizes(stem):
        """Byte sizes of the $SG string literals in a unit's /Fc listing, in emission order."""
        path = config.WORK / 'layout' / 'BUILD' / stem / (stem.lower() + '.cod')
        if not path.exists():
            return []
        sizes, cur, inside = [], None, False
        for line in path.read_text(errors='replace').splitlines():
            if re.match(r'^_DATA\s+SEGMENT', line):
                inside = True
                continue
            if re.match(r'^_DATA\s+ENDS', line):
                inside = False
                cur = None
                continue
            if not inside:
                continue
            m = re.match(r'^(\$SG\d+)\s+DB\s+(.*)$', line)
            if m:
                cur = len(sizes)
                sizes.append(0)
                body = m.group(2)
            elif cur is not None and re.match(r'^\s+DB\s+', line):
                body = line.split('DB', 1)[1]
            else:
                cur = None
                continue
            n = 0
            for part in re.findall(r"'(?:[^']|'')*'|[0-9A-Fa-f]+H|\d+", body):
                n += len(part[1:-1].replace("''", "'")) if part.startswith("'") else 1
            sizes[cur] += n
        return sizes

    def _pools(self):
        """unit start -> function addr -> sorted [(object offset, original offset)] of its literals."""
        pools = {}
        for u in self.units:
            if u['kind'] != 'c':
                continue
            obj = Obj(self._unit_obj(u))
            if '_DATA' not in obj.segments:
                continue
            dseg = obj.segments.index('_DATA') + 1
            named = self._public_extents(obj, dseg)
            codebase = u['start'] - min(obj.publics.values())
            per = {}
            for f in obj.fixups:
                if f.seg != obj.code_seg or f.loc not in (1, 5) or f.self_rel or f.kind != 'seg' or f.index != dseg:
                    continue
                objoff = (int.from_bytes(obj.code[f.pos:f.pos + 2], 'little') + f.disp) & 0xFFFF
                if any(lo <= objoff < hi for lo, hi in named):
                    continue
                if objoff >= len(obj.data.get(dseg, b'')):
                    continue        # below the unit's data (named[i - 1]): not a literal
                orig = self.im.word(codebase + f.pos)
                per.setdefault(self.fn_at(codebase + f.pos)['addr'], set()).add((objoff, orig))
            if per:
                pools[u['start']] = {a: sorted(v) for a, v in sorted(per.items())}
                # a literal's size from the compiler listing ($SG records, emission order)
                sizes = self._cod_literal_sizes(f'U_{u["start"]:05X}')
                litstarts = sorted({o for v in per.values() for o, _ in v})
                if len(sizes) == len(litstarts):
                    for o, size in zip(litstarts, sizes):
                        self.lit_sizes[(u['start'], o)] = size
        return pools

    # ------------------------------------------------------------ items
    def cstring(self, off):
        end = self.dg.index(b'\0', off)
        return self.dg[off:end]

    def _items(self):
        items = []
        lit_at = {}
        self.named_lits = []        # (unit, function, offset): a literal in our C, a named array in the original
        for ustart, per in self.pools.items():
            allrefs = sorted((o, f) for f, refs in per.items() for _, o in refs)
            clusters = [[allrefs[0]]]
            for ref in allrefs[1:]:
                if ref[0] - clusters[-1][-1][0] <= 0x400:
                    clusters[-1].append(ref)
                else:
                    clusters.append([ref])
            main = max(clusters, key=lambda c: (len({f for _, f in c}), len(c)))
            lo, hi = main[0][0], main[-1][0]
            shared = {}
            for faddr, refs in per.items():
                for objoff, orig in refs:
                    shared.setdefault(orig, set()).add(objoff)
            for faddr, refs in per.items():
                for objoff, orig in refs:
                    if not lo <= orig <= hi or len(shared[orig]) > 1:
                        self.named_lits.append((ustart, faddr, orig))
                        continue
                    if orig in lit_at:
                        continue
                    s = self.cstring(orig)
                    it = Item(orig, self.lit_sizes.get((ustart, objoff), len(s) + 1), 'lit', unit=ustart, fn=faddr)
                    lit_at[orig] = it
                    items.append(it)
        for ustart, faddr, orig in self.named_lits:
            if orig in lit_at or any(it.off == orig for it in items):
                continue
            s = self.cstring(orig)
            node = dict(kind='array', elem=dict(kind='scalar', name='char', size=1, align=1, ctype='char'),
                        count=len(s) + 1, size=len(s) + 1, align=1, dalign=2)
            it = Item(orig, len(s) + 1, 'global', self.globals[orig]['name'] if orig in self.globals else f'g_{orig:04x}',
                      'char @[]', node, synth=True)
            it.note = f'named string: F_{faddr:05X} uses it as a literal'
            items.append(it)
            lit_at[orig] = it
        items += self._masks()
        for lo, hi, decl in REGIONS:
            node = self.types.declaration(decl)
            name = f'g_{lo:04x}'
            if lo in self.globals:
                continue
            items.append(Item(lo, hi - lo, 'global', name, decl, node, synth=True))
        for off in self.goffs:
            if off >= DATA_END:
                break
            g = self.globals[off]
            if not g['decl']:
                items.append(Item(off, 0, 'global?', g['name']))
                continue
            node = self.types.declaration(g['decl'])
            items.append(Item(off, node['size'] or 0, 'global', g['name'], g['decl'], node))
        items.sort(key=lambda it: (it.off, 0 if it.kind == 'lit' else 1))
        # Unbounded arrays run to the next item, a pointer target inside them, or their recovered
        # bound. A global whose extent covers a later global's start makes that one an alias.
        self.aliases = {}
        unbounded = {it.off: it.node for it in items if it.kind == 'global' and it.node['kind'] == 'array' and it.node['count'] is None}
        targets = set()
        for passno in range(2):
            for i, it in enumerate(items):
                if it.kind != 'global' or it.off not in unbounded:
                    continue
                nxt = next((x.off for x in items[i + 1:] if x.off > it.off), DATA_END)
                node = unbounded[it.off]
                esize = node['elem']['size']
                cut = next((t for t in sorted(targets) if it.off < t < nxt), nxt)
                if cut < nxt and (cut - it.off) % esize == 0 and it.name not in BOUNDS:
                    nxt = cut
                count = BOUNDS.get(it.name, (nxt - it.off) // esize)
                it.node = {**node, 'count': count, 'size': count * esize}
                it.size = it.node['size']
            targets = self.pointer_targets(items)
        self.targets = targets
        base = None
        for it in items:
            if it.kind not in ('global',):
                continue
            if base is not None and it.off < base.end:
                self.aliases[it.name] = (base, it.off - base.off)
                it.kind = 'alias'
                continue
            if base is None or it.end > base.end:
                base = it
        # Anonymous strings (initializers of pointers) in what is left, empty ones where a
        # pointer names a NUL, then zero gaps.
        covered = bytearray(DATA_END)
        for it in items:
            for b in range(it.off, min(it.end, DATA_END)):
                covered[b] = 1
        for m in re.finditer(rb'[\x09\x0a\x0d\x20-\x7e]+\x00', self.dg[:DATA_END]):
            if not any(covered[b] for b in range(m.start(), m.end())):
                items.append(Item(m.start(), m.end() - m.start(), 'str'))
                for b in range(m.start(), m.end()):
                    covered[b] = 1
        for tgt in sorted(targets):
            if 0x42 <= tgt < DATA_END and not covered[tgt] and self.dg[tgt] == 0:
                items.append(Item(tgt, 1, 'str'))
                covered[tgt] = 1
        items.sort(key=lambda it: (it.off, 0 if it.kind == 'lit' else 1))
        out, a = [], 0x42
        for it in items:
            if it.off > a:
                g = Item(a, it.off - a, 'gap')
                if g.size == 1 and a % 2 == 1 and it.node and max(it.node.get('align', 1), it.node.get('dalign', 1)) == 2:
                    g.note = 'align'
                else:
                    # zero storage no code refers to: variables the original kept all the same.
                    # An array cannot start at an odd address, so an odd lead byte is a char.
                    for off, size in self._gap_pieces(a, g.size):
                        if size == 1:
                            decl = 'char @'
                        elif size == 2:
                            decl = 'int @'
                        elif size % 2 == 0:
                            decl = f'int @[{size // 2}]'
                        else:
                            decl = f'char @[{size}]'
                        piece = Item(off, size, 'global', self.globals[off]['name'] if off in self.globals else f'g_{off:04x}',
                                     decl, self.types.declaration(decl), synth=True)
                        piece.note = 'unreferenced zero storage'
                        out.append(piece)
                    a = max(a, it.end)
                    out.append(it)
                    continue
                out.append(g)
            out.append(it)
            a = max(a, it.end)
        if a < DATA_END:
            out.append(Item(a, DATA_END - a, 'gap'))
        return out

    @staticmethod
    def _gap_pieces(off, size):
        pieces = []
        if off % 2 and size > 1:
            pieces.append((off, 1)); off += 1; size -= 1
        if size % 2 and size > 1:
            pieces.append((off, size - 1)); pieces.append((off + size - 1, 1))
        else:
            pieces.append((off, size))
        return pieces

    def pointer_targets(self, items):
        out = set()
        for it in items:
            if it.kind == 'global' and it.node and it.node.get('count', 1) is not None:
                try:
                    for off, node, label in self.types.leaves(it.node, it.off, it.name):
                        if node['kind'] == 'pointer' and node['size'] == 2:
                            v = self.im.word(self.ds + off)
                            if 0x42 <= v < END:
                                out.add(v)
                except ValueError:
                    pass
        return out

    def _masks(self):
        """Run arrays {n, far Part*} and List descriptors {last, near Run*} of the car masks."""
        lo, hi = 0x5EC8, DATA_END
        parts = {s for s, o, n, d in self.fardata if n.startswith('g_parts')}
        runs = set()
        for p in self.im.relocs:
            a = p - self.ds
            if lo + 4 <= a < hi and self.im.word(p) in parts:
                runs.add(a - 4)
        lists = {}
        for a in range(lo, hi - 3, 2):
            n, target = struct.unpack_from('<HH', self.dg, a)
            if 0 < n < 100 and target + n * 6 == a and all(target + k * 6 in runs for k in range(n)):
                lists[a] = (n, target)
        items = []
        run_node = self.types.named('struct Run')
        list_node = self.types.named('struct List')
        for a, (n, target) in sorted(lists.items()):
            node = dict(kind='array', elem=run_node, count=n, size=n * 6, align=2)
            if target not in self.globals:
                items.append(Item(target, n * 6, 'global', f'g_{target:04x}', f'struct Run @[{n}]', node, synth=True))
            if a not in self.globals:
                items.append(Item(a, 4, 'global', f'g_{a:04x}', 'struct List @', dict(list_node), synth=True))
        return items

    # ------------------------------------------------------------ ownership
    def refs_to(self):
        """global name -> {unit start: fixup count} from the layout objects."""
        if hasattr(self, '_refs'):
            return self._refs
        name2off = {'_' + g['name']: off for off, g in self.globals.items()}
        refs = {}
        for u in self.units:
            if u['kind'] == 'data':
                continue
            obj = Obj(self._unit_obj(u))
            for f in obj.fixups:
                if f.kind == 'ext':
                    n = obj.externs[f.index - 1]
                    if n in name2off:
                        refs.setdefault(n[1:], {}).setdefault(u['start'], 0)
                        refs[n[1:]][u['start']] += 1
            dseg = obj.segments.index('_DATA') + 1 if '_DATA' in obj.segments else None
            if dseg:
                pubs = sorted((off, name) for name, (seg, off) in obj.all_publics.items() if seg == dseg)
                for f in obj.fixups:
                    if f.seg == obj.code_seg and f.kind == 'seg' and f.index == dseg and f.loc in (1, 5) and not f.self_rel:
                        objoff = (int.from_bytes(obj.code[f.pos:f.pos + 2], 'little') + f.disp) & 0xFFFF
                        i = bisect.bisect_right([o for o, _ in pubs], objoff) - 1
                        if i >= 0 and pubs[i][1] in name2off:
                            refs.setdefault(pubs[i][1][1:], {}).setdefault(u['start'], 0)
                            refs[pubs[i][1][1:]][u['start']] += 1
        self._refs = refs
        return refs

    def _assign_owners(self):
        """Walk the items in address order. A literal pool pins its unit; a run of named items
        between pools belongs to the unit before (its last flush), to the unit after (flushed by
        literal-less functions before its first pooled one), or to a unit without literals that
        can sit between them in the link order and is the only one referring to the item.
        Code segments are placed in first-encounter order, so a unit's placement also places
        the first unit of every earlier code region."""
        refs = self.refs_to()
        regions = {}
        for u in self.units:
            if u['region'] is not None:
                regions.setdefault(u['region'], []).append(u)
        pos = {}
        for it in self.items:
            if it.kind == 'lit':
                pos.setdefault(it.unit, it.off)
        placed, done = [], set()

        def can_place(u):
            if u['region'] is None:
                return True
            lst = regions[u['region']]
            return all(x['start'] in done or x['start'] not in pos for x in lst[:lst.index(u)])

        def place(u):
            if u['region'] is None:
                if u['start'] not in done:
                    done.add(u['start']); placed.append(u)
                return
            for r in self.code_starts:
                if r >= u['region']:
                    break
                first = regions[r][0]
                if first['start'] not in done:
                    place(first)
            lst = regions[u['region']]
            for x in lst[:lst.index(u)]:
                if x['start'] not in done:
                    done.add(x['start']); placed.append(x)
            if u['start'] not in done:
                done.add(u['start']); placed.append(u)

        pooled = sorted(pos, key=pos.get)
        src_unit = {}
        for u in self.units:
            for f in u['functions']:
                src_unit[f['src']] = u['start']
        lit_first_fn = {}
        for it in self.items:
            if it.kind == 'lit':
                lit_first_fn.setdefault(it.unit, it.fn)

        def leading_flush(u):
            return u['functions'] and u['functions'][0]['addr'] != lit_first_fn.get(u['start'])

        for it in self.items:
            if it.kind == 'lit':
                it.owner = it.unit
        # runs of named items between literal pools
        runs, cur, L = [], [], None
        for it in self.items:
            if it.kind == 'lit':
                runs.append((L, cur, it.unit))
                cur, L = [], it.unit
            elif it.kind in ('global', 'global?'):
                cur.append(it)
        runs.append((L, cur, None))
        for L, run, R in runs:
            if not run:
                continue
            Lu = self.unit_by[L] if L is not None else None
            Ru = self.unit_by[R] if R is not None else None
            if Lu is not None and L not in done:
                place(Lu)
            if L is not None and L == R:
                for it in run:
                    it.owner = L
                continue
            pref = []
            for it in run:
                who = refs.get(it.name, {})
                asm_only = bool(who) and all(self.unit_by[s]['kind'] == 'asm' for s in who)
                ok = [s for s in who if s == L or (s == R and leading_flush(Ru))
                      or (s not in done and s not in pos and can_place(self.unit_by[s])
                          and (self.unit_by[s]['kind'] == 'c' or asm_only))]
                pref.append(ok[0] if len(who) == 1 and ok else None)
            # fill: a neighbour that refers to the item, else the left one, else the run's left unit
            own = list(pref)
            for i in range(len(own)):
                if own[i] is not None:
                    continue
                who = refs.get(run[i].name, {})
                left = own[i - 1] if i > 0 else None
                right = next((p for p in pref[i + 1:] if p is not None), None)
                left_asm = left is not None and self.unit_by[left]['kind'] == 'asm' and not (who and all(self.unit_by[s]['kind'] == 'asm' for s in who))
                if right is not None and right in who and (left is None or left not in who):
                    own[i] = right
                elif left is not None and not left_asm:
                    own[i] = left
                elif left_asm and right is not None:
                    own[i] = right
                elif left is not None:
                    own[i] = left
                elif L is not None:
                    own[i] = L
                else:
                    own[i] = right if right is not None else (R if R is not None else self.units[0]['start'])
            # one stretch per unit: a unit split by another keeps its longest stretch
            def stretches(own):
                out, s = [], 0
                for i in range(1, len(own) + 1):
                    if i == len(own) or own[i] != own[s]:
                        out.append((own[s], s, i)); s = i
                return out
            changed = True
            while changed:
                changed = False
                st = stretches(own)
                seen = {}
                for k, (u, s, e) in enumerate(st):
                    seen.setdefault(u, []).append(k)
                for u, ks in seen.items():
                    if len(ks) > 1:
                        keep = max(ks, key=lambda k: st[k][2] - st[k][1])
                        for k in ks:
                            if k != keep:
                                u2, s, e = st[k]
                                repl = st[k - 1][0] if k > 0 else (st[k + 1][0] if k + 1 < len(st) else u2)
                                for i in range(s, e):
                                    own[i] = repl
                                changed = True
                        break
            # L's items come first, R's last; a unit needs a stretch of at least two items;
            # a unit that cannot be placed here joins the previous one
            st = stretches(own)
            seenR = False
            for k, (u, s, e) in enumerate(st):
                if seenR or (u == L and k > 0) or (e - s < 2 and u not in (L, R) and len(st) > 1 and self.unit_by[u]['kind'] == 'asm'):
                    repl = st[k - 1][0] if k > 0 else (st[k + 1][0] if k + 1 < len(st) else u)
                    if seenR:
                        repl = R
                    for i in range(s, e):
                        own[i] = repl
                if u == R:
                    seenR = True
            st = stretches(own)
            prev = L
            for u, s, e in st:
                uu = self.unit_by[u]
                if u not in done:
                    if not can_place(uu) or (R is not None and u != R and pos.get(u) is not None):
                        repl = prev if prev is not None else u
                        for i in range(s, e):
                            own[i] = repl
                        continue
                    place(uu)
                elif prev is not None and placed.index(uu) < placed.index(self.unit_by[prev]):
                    # already linked before the previous owner: its data cannot come after
                    for i in range(s, e):
                        own[i] = prev
                    continue
                prev = u
            for it, o in zip(run, own):
                it.owner = o
            if R is not None and R not in done:
                place(Ru)
        for u in self.units:
            if u['start'] not in done:
                place(u)
        for lo, hi, unit in FORCE_OWNER + [(lo, hi, DATA_KEY + lo) for lo, hi, _ in DATA_UNITS]:
            prev_owner = None
            for it in self.items:
                if it.off < lo and it.owner is not None and it.kind in ('global', 'lit'):
                    prev_owner = it.owner
                if lo <= it.off < hi and it.kind != 'lit':
                    it.owner = unit
            u = self.unit_by[unit]
            if u in placed:
                placed.remove(u)
            at = placed.index(self.unit_by[prev_owner]) + 1 if prev_owner is not None else len(placed)
            group = [u]
            if u['region'] is not None:
                lst = regions[u['region']]
                group = [x for x in lst[:lst.index(u)] if x['start'] not in done or placed.index(x) > at] + [u]
                for x in group[:-1]:
                    if x in placed:
                        placed.remove(x)
                at = placed.index(self.unit_by[prev_owner]) + 1 if prev_owner is not None else len(placed)
            for k, x in enumerate(group):
                placed.insert(at + k, x)
                done.add(x['start'])
            # what followed the forced block can no longer belong to the unit before it:
            # the next C unit in the link order that refers to any of it takes it
            after = [it for it in self.items if it.off >= hi and it.owner == prev_owner and it.kind != 'lit']
            if after:
                nxt_lit = next((it.off for it in self.items if it.off >= hi and it.kind == 'lit'), END)
                after = [it for it in after if it.off < nxt_lit]
                cands = [x['start'] for x in placed[at + 1:] if x['kind'] == 'c']
                taker = next((c for c in cands if any(c in refs.get(it.name, {}) for it in after)), cands[0] if cands else unit)
                for it in after:
                    it.owner = taker
        # code order within a segment is fixed: keep each region's units ascending in the
        # positions they occupy
        for r, lst in regions.items():
            slots = [k for k, u in enumerate(placed) if u['region'] == r]
            for k, u in zip(slots, sorted((placed[k] for k in slots), key=lambda u: u['start'])):
                placed[k] = u
        self.order = [u['start'] for u in placed]
        targets = {}
        for it in self.items:
            if it.kind == 'global' and it.node:
                for off, node, label in self.types.leaves(it.node, it.off, it.name):
                    if node['kind'] == 'pointer' and node['size'] == 2:
                        targets.setdefault(self.im.word(self.ds + off), []).append(it)
        last = None
        # a unit's named items span one block; an initializer string lies inside or just before it
        span = {}
        for it in self.items:
            if it.owner is not None and it.kind in ('global', 'lit'):
                lo, hi = span.get(it.owner, (it.off, it.end))
                span[it.owner] = (min(lo, it.off), max(hi, it.end))
        for it in self.items:
            if it.kind == 'str':
                owners = targets.get(it.off)
                near = [o for o in owners or [] if o.owner in span
                        and (span[o.owner][0] <= it.off < span[o.owner][1] or it.off < span[o.owner][0] and self._adjacent(it, span[o.owner][0]))]
                if near:
                    it.owner = near[0].owner
                    it.note = ''
                else:
                    it.owner = last.owner if last else None
                    it.note = 'no pointer names this string' if not owners else f'named string: {owners[0].name} refers to it'
                    if owners:
                        # a named array the pointer initializer refers to by name
                        it.kind = 'global'
                        it.name = self.globals[it.off]['name'] if it.off in self.globals else f'g_{it.off:04x}'
                        it.decl = 'char @[]'
                        it.node = dict(kind='array', elem=dict(kind='scalar', name='char', size=1, align=1, ctype='char'),
                                       count=it.size, size=it.size, align=1)
                        it.synth = True
                        self.gitems[it.off] = it
            elif it.kind == 'alias':
                it.owner = last.owner if last else None
            elif it.kind in ('global', 'lit'):
                last = it
            if it.kind == 'str' and it.note == 'no pointer names this string' and it.owner is not None \
                    and self.unit_by[it.owner]['kind'] == 'c' and it.off % 2 == 0:
                # unreferenced text in a C unit, not a literal of any function: a named array
                it.kind, it.synth = 'global', True
                it.name = self.globals[it.off]['name'] if it.off in self.globals else f'g_{it.off:04x}'
                it.decl = 'char @[]'
                it.node = dict(kind='array', elem=dict(kind='scalar', name='char', size=1, align=1, ctype='char'),
                               count=it.size, size=it.size, align=1, dalign=2)
                it.note = 'unreferenced named string'
                self.gitems[it.off] = it
        self.goffs2 = sorted(self.gitems)

    def _adjacent(self, it, block_lo):
        """Only strings (and alignment) between `it` and the block start."""
        for x in self.items:
            if it.end <= x.off < block_lo and x.kind not in ('str', 'gap'):
                return False
        return True

    def _assign_slots(self):
        """Named items: the function whose literal pool precedes them in their unit (they are
        declared just before it); before the first function when no pool precedes."""
        pool_pos = {}
        for it in self.items:
            if it.kind == 'lit':
                pool_pos.setdefault(it.unit, {}).setdefault(it.fn, it.off)
        for u in self.units:
            first = u['functions'][0]['addr'] if u['functions'] else None
            pp = sorted((p, a) for a, p in pool_pos.get(u['start'], {}).items())
            for it in self.items:
                if it.owner != u['start'] or it.kind == 'lit':
                    continue
                prev = [a for p, a in pp if p < it.off]
                it.slot = prev[-1] if prev else first

    def blocks(self):
        """unit start -> (lo, hi) of its owned items; raises on interleaving."""
        out = {}
        last = None
        for it in self.items:
            if it.owner is None:
                continue
            if it.owner in out and last != it.owner:
                raise ValueError(f'unit {it.owner:05X} owns {it.off:04x} after {last:05X} took over at {out[last][0]:04x}')
            lo, hi = out.get(it.owner, (it.off, it.off))
            out[it.owner] = (lo, it.end)
            last = it.owner
        return out

    # ------------------------------------------------------------ C emission
    def symbol(self, off):
        """A C expression for the DGROUP address `off` (near pointer target)."""
        if off == 0:
            return '0'
        it = self.by_off.get(off)
        if it is not None and it.kind in ('str', 'lit'):
            return self.clit(self.cstring(off))
        if off >= DATA_END:
            i = bisect.bisect_right(self.goffs, off) - 1
            g = self.globals[self.goffs[i]]
            base, name = self.goffs[i], g['name']
            node = self.types.declaration(g['decl']) if g['decl'] else None
            if node and node['kind'] == 'array' and node['count'] is None:
                node = None
        else:
            i = bisect.bisect_right(self.goffs2, off) - 1
            if i < 0:
                return f'(void *){off}'
            g = self.gitems[self.goffs2[i]]
            base, name, node = g.off, g.name, g.node
        if off == base:
            if node and node['kind'] == 'array':
                return name
            return '&' + name
        d = off - base
        path, leaf = self.path(node, d) if node else (None, None)
        if path is not None:
            if leaf['kind'] == 'array':
                return name + path
            return '&' + name + path
        if node and node['kind'] == 'array':
            return f"(char *){name} + {d}"
        return f"(char *)&{name} + {d}"

    def path(self, node, d):
        """('.field[i]...', leaf node) for byte offset d inside node, or (None, None)."""
        if node['kind'] == 'array':
            esize = node['elem']['size']
            if not esize:
                return None, None
            i, r = divmod(d, esize)
            if node['count'] is not None and i >= node['count']:
                return None, None
            if r == 0:
                return f'[{i}]', node['elem']
            sub, leaf = self.path(node['elem'], r)
            if sub is None:
                return None, None
            return f'[{i}]' + sub, leaf
        if node['kind'] in ('struct', 'union'):
            for name, foff, child in node['fields']:
                if foff <= d < foff + child['size']:
                    sub, leaf = self.path(child, d - foff)
                    if sub is None:
                        return None, None
                    return '.' + name + sub, leaf
            return None, None
        return ('', node) if d == 0 else (None, None)

    def far_symbol(self, seg, off, node):
        a = seg * 16 + off
        ctype = node.get('ctype') or node['name'].replace('@', '').strip()
        if seg == config.DGROUP_SEG:
            return f"({ctype}){self.symbol(off)}"
        f = self.funcs.get(a)
        if f is not None:
            name = f['name'] or f'F_{a:05X}'
            return name if '(' in node['name'] else f"({ctype}){name}"
        best = None
        for fseg, foff, name, decl in self.fardata:
            if fseg == seg and foff <= off and (best is None or foff > best[1]):
                best = (fseg, foff, name, decl)
        if best is None:
            raise ValueError(f'far pointer {seg:04x}:{off:04x} names no known far data')
        _, foff, name, decl = best
        fnode = self.types.declaration(decl)
        d = off - foff
        if d == 0:
            return name if fnode['kind'] == 'array' else '&' + name
        if fnode['kind'] == 'array' and d % fnode['elem']['size'] == 0:
            return f'{name} + {d // fnode["elem"]["size"]}'
        return f'(char far *){name} + {d}'

    @staticmethod
    def clit(s):
        out = ''
        for c in s:
            ch = chr(c)
            if ch == '"': out += '\\"'
            elif ch == '\\': out += '\\\\'
            elif ch == '\n': out += '\\n'
            elif ch == '\t': out += '\\t'
            elif ch == '\r': out += '\\r'
            elif 32 <= c < 127: out += ch
            else: out += f'\\{c:03o}'
        return '"' + out + '"'

    def value(self, node, off):
        """C initializer text for `node` at DGROUP offset `off`."""
        d = self.dg
        k = node['kind']
        if k == 'scalar':
            n = node['name']; size = node['size']
            if n in ('float', 'double'):
                return repr(struct.unpack('<f' if size == 4 else '<d', d[off:off + size])[0])
            v = int.from_bytes(d[off:off + size], 'little', signed='unsigned' not in n)
            return str(v) + ('L' if size == 4 else '')
        if k == 'pointer':
            if node['size'] == 2:
                return self.symbol(self.im.word(self.ds + off))
            o, s = self.im.word(self.ds + off), self.im.word(self.ds + off + 2)
            if self.ds + off + 2 in self.im.reloc_pos:
                return self.far_symbol(s, o, node)
            if o == 0 and s == 0:
                return '0'
            return f"({node.get('ctype') or node['name'].replace('@', '').strip()})0x{s:04X}{o:04X}L"
        if k == 'array':
            e = node['elem']
            if e['kind'] == 'scalar' and e['name'] == 'char' and node['count']:
                raw = d[off:off + node['size']]
                s = raw.split(b'\0', 1)[0]
                if all(c in PRINTABLE for c in s) and len(s) < node['count'] and not any(raw[len(s):]):
                    return self.clit(s)
            vals = [self.value(e, off + i * e['size']) for i in range(node['count'])]
            return '{' + ', '.join(vals) + '}'
        if k in ('struct', 'union'):
            fields = node['fields'][:1] if k == 'union' else node['fields']
            seen = set(); vals = []
            for name, foff, child in fields:
                if foff in seen:
                    raise ValueError('bitfield initializer not supported yet')
                seen.add(foff)
                vals.append(self.value(child, off + foff))
            return '{' + ', '.join(vals) + '}'
        raise ValueError(k)

    def definition(self, it, comment=True):
        tail = f'   /* DS:{it.off:04X} */' if comment else ''
        if it.kind == 'global':
            decl = it.decl.replace('@', it.name)
            node = it.node
            v = self.value(node, it.off)
            if node['kind'] == 'array' and '[]' in it.decl:
                if not (v.startswith('"') and node['count'] == len(self.cstring(it.off)) + 1):
                    decl = decl.replace('[]', f'[{node["count"]}]', 1)
            return f'{decl} = {v};{tail}'
        if it.kind == 'gap':
            if it.note == 'align':
                return ''
            if it.off % 2 == 0 and it.size % 2 == 0:
                return f'int g_{it.off:04x}[{it.size // 2}] = {{0}};{tail}   /* unreferenced zero storage */'
            return f'char g_{it.off:04x}[{it.size}] = {{0}};{tail}   /* unreferenced zero storage */'
        if it.kind == 'str':
            return f'/* {it.off:04x}: {self.clit(self.cstring(it.off))} ({it.note or "initializer string"}) */'
        if it.kind == 'alias':
            base, d = self.aliases[it.name]
            return f'/* alias: {it.name} = {self.symbol(it.off)} ({base.name}+{d}) */'
        return f'/* {it!r} */'

    # ------------------------------------------------------------ reports
    def summary(self):
        import collections
        kinds = collections.Counter(it.kind for it in self.items)
        orphan = [it for it in self.items if it.kind == 'str' and it.note]
        gaps = [it for it in self.items if it.kind == 'gap' and it.note != 'align']
        return (f'items {dict(kinds)}; aliases {len(self.aliases)}; orphan strings {len(orphan)}; '
                f'real gaps {len(gaps)} ({sum(g.size for g in gaps)} bytes); named literals {len(self.named_lits)}')

    def _points_at(self, it, target):
        try:
            leaves = list(self.types.leaves(it.node, it.off, it.name))
        except ValueError:
            return False
        for off, node, label in leaves:
            if node['kind'] == 'pointer' and node['size'] == 2 and self.im.word(self.ds + off) == target.off:
                return True
        return False

    def forward_refs(self):
        """(item, target) where an initializer in a C unit takes the address of a global the
        same unit defines later: MSC would emit the target first, so the order cannot hold."""
        out = []
        owner_of = {it.off: it for it in self.items if it.kind == 'global'}
        for it in self.items:
            if it.kind != 'global' or it.owner is None or self.unit_by[it.owner]['kind'] == 'asm':
                continue
            if self.unit_by[it.owner]['kind'] == 'data' and self.unit_by[it.owner]['sources'][0].upper().endswith('.ASM'):
                continue
            try:
                leaves = list(self.types.leaves(it.node, it.off, it.name))
            except ValueError:
                continue
            for off, node, label in leaves:
                if node['kind'] != 'pointer':
                    continue
                if node['size'] == 2:
                    tgt = self.im.word(self.ds + off)
                elif self.ds + off + 2 in self.im.reloc_pos and self.im.word(self.ds + off + 2) == config.DGROUP_SEG:
                    tgt = self.im.word(self.ds + off)
                else:
                    continue
                i = bisect.bisect_right(self.goffs2, tgt) - 1
                if i < 0:
                    continue
                g = self.gitems[self.goffs2[i]]
                if g.owner == it.owner and g.off > it.off:
                    out.append((it, g))
        return out

    def problems(self):
        lines = [f'{o:04x} named string used as a literal by F_{f:05X} (unit {u:05X})' for u, f, o in self.named_lits]
        lines += [f'{it.off:04x} {it.name} takes the address of {g.name} (DS:{g.off:04x}), defined later in the same unit '
                  f'{it.owner:05X}: MSC emits it first; split the unit' for it, g in self.forward_refs()]
        for it in self.items:
            if it.kind == 'str' and it.note:
                lines.append(f'{it.off:04x} orphan string {self.clit(self.cstring(it.off))[:60]}')
            elif it.kind == 'alias':
                base, d = self.aliases[it.name]
                lines.append(f'{it.off:04x} alias {it.name} = {base.name}+{d} ({base.decl})')
            elif it.kind == 'global?':
                lines.append(f'{it.off:04x} no decl: {it.name}')
            elif it.kind == 'global':
                try:
                    d = self.definition(it)
                    if '(void *)' in d or re.search(r'\(char \*\)g_|\(char far \*\)g_|\)0x(?!A000|B800|B000)', d):
                        lines.append(f'{it.off:04x} fallback expression in {it.name}: {d[:120]}')
                except Exception as e:  # noqa
                    lines.append(f'{it.off:04x} !! {it.name} {it.decl}: {e}')
        return lines

    def report(self, lo=0x42, hi=DATA_END):
        lines = ['link order: ' + ' '.join(f'{s:05X}' for s in self.order), '']
        for it in self.items:
            if not lo <= it.off < hi:
                continue
            owner = f'{it.owner:05X}' if it.owner is not None else '-----'
            slot = f'{it.slot:05X}' if it.slot is not None else '     '
            if it.kind == 'lit':
                desc = f'lit {it.fn:05X} {self.clit(self.cstring(it.off))[:50]}'
            else:
                try:
                    desc = self.definition(it, comment=False)
                except Exception as e:  # noqa
                    desc = f'{it.kind} {it.name} {it.decl}: !! {e}'
            lines.append(f'{it.off:04x} {it.size:5d} {owner} {slot} {desc[:150]}' + (f'   ; {it.note}' if it.note else ''))
        return '\n'.join(lines)

    # ------------------------------------------------------------ apply
    def path_leaf(self, node, d):
        """Like path(), but descends into the scalar/pointer leaf at offset d."""
        if node['kind'] == 'array':
            esize = node['elem']['size']
            if not esize:
                return None, None
            i, r = divmod(d, esize)
            sub, leaf = self.path_leaf(node['elem'], r)
            return (None, None) if sub is None else (f'[{i}]' + sub, leaf)
        if node['kind'] in ('struct', 'union'):
            for name, foff, child in node['fields']:
                if foff <= d < foff + child['size']:
                    sub, leaf = self.path_leaf(child, d - foff)
                    return (None, None) if sub is None else ('.' + name + sub, leaf)
            return None, None
        return ('', node) if d == 0 else (None, None)

    def alias_rewrites(self):
        """alias name -> C expression (and MASM operand) of the base it is a view into."""
        out = {}
        for name, (base, d) in self.aliases.items():
            it = next(x for x in self.items if x.name == name)
            node = self.types.declaration(it.decl) if it.decl else None
            ctype = (it.decl or '').replace('@', '').strip()
            if node and node['kind'] == 'array':
                # a view used as an array: pointer arithmetic keeps `alias[i]` working
                bnode = base.node
                ectype = node['elem'].get('ctype', 'char')
                if bnode['kind'] == 'array' and d % bnode['elem']['size'] == 0 and bnode['elem'].get('ctype') == ectype:
                    expr = f"({base.name} + {d // bnode['elem']['size']})"
                else:
                    expr = f"(({ectype} *)((char *){base.name} + {d}))"
            else:
                sub, leaf = self.path_leaf(base.node, d)
                if sub is None:
                    expr = f"(*({ctype} *)((char *){base.name} + {d}))"
                elif leaf.get('ctype') == ctype or (leaf['kind'] == 'pointer' and leaf.get('ctype', '').replace(' ', '') == ctype.replace(' ', '')):
                    expr = base.name + sub
                else:
                    expr = f"(*({ctype} *)&{base.name}{sub})"
            out[name] = (expr, f'_{base.name}+{d}' if d else f'_{base.name}')
        return out

    def c_definitions(self, ustart):
        """{slot function addr: [definition lines]} for a C unit, in address order."""
        out = {}
        for it in self.items:
            if it.owner != ustart or it.kind in ('lit', 'str', 'alias'):
                continue
            text = self.definition(it)
            if text:
                out.setdefault(it.slot, []).append(text)
        return out

    def masm_lines(self, items, label_public=True):
        """MASM data records for items (remaining data or an assembly unit's own data)."""
        lines, externs = [], {}
        for it in items:
            if it.kind == 'lit':
                continue
            if it.kind == 'gap':
                if it.note == 'align':
                    lines.append(f'        DB 0 ; +{it.off:04X} alignment')
                else:
                    lines.append(f'        DB {it.size} DUP (0) ; +{it.off:04X}')
                continue
            if it.kind == 'str':
                lines.append(f'S_{it.off:04x} LABEL BYTE')
                lines.append('        ' + self.masm_string(self.cstring(it.off)) + f' ; +{it.off:04X}')
                continue
            if it.kind == 'alias':
                continue
            if it.node is None:
                raise ValueError(f'{it!r} has no declaration')
            if label_public:
                lines.append(f'        PUBLIC _{it.name}')
            lines.append(f'_{it.name} LABEL BYTE ; +{it.off:04X} {it.decl}')
            lines += self.masm_value(it.node, it.off, externs)
        return lines, externs

    @staticmethod
    def masm_string(s):
        vals, part = [], ''
        for b in s:
            if 32 <= b < 127 and b != 39:
                part += chr(b)
            else:
                if part:
                    vals.append("'" + part + "'"); part = ''
                vals.append(str(b))
        if part:
            vals.append("'" + part + "'")
        vals.append('0')
        return 'DB ' + ', '.join(vals)

    def masm_symbol(self, off, externs):
        """MASM operand for a near pointer to DGROUP `off`."""
        if off < 0x42:
            return str(off)
        it = self.by_off.get(off)
        if it is not None and it.kind == 'str':
            return f'OFFSET DGROUP:S_{off:04x}'
        if off >= DATA_END:
            i = bisect.bisect_right(self.goffs, off) - 1
            base, name = self.goffs[i], self.globals[self.goffs[i]]['name']
        else:
            i = bisect.bisect_right(self.goffs2, off) - 1
            base, name = self.goffs2[i], self.gitems[self.goffs2[i]].name
        externs.setdefault('_' + name, 'BYTE')
        return f'OFFSET DGROUP:_{name}' + (f'+{off - base}' if off != base else '')

    def masm_value(self, node, off, externs):
        lines = []
        for loff, leaf, label in self.types.leaves(node, off, ''):
            if leaf['kind'] == 'scalar':
                v = int.from_bytes(self.dg[loff:loff + leaf['size']], 'little')
                op = {1: 'DB', 2: 'DW', 4: 'DD', 8: 'DQ'}[leaf['size']]
                lines.append(f'        {op} 0{v:X}h')
            elif leaf['size'] == 2:
                lines.append(f'        DW {self.masm_symbol(self.im.word(self.ds + loff), externs)}')
            else:
                o, s = self.im.word(self.ds + loff), self.im.word(self.ds + loff + 2)
                if self.ds + loff + 2 not in self.im.reloc_pos:
                    lines.append(f'        DD 0{s:04X}{o:04X}h')
                elif s == config.DGROUP_SEG:
                    lines.append(f'        DW {self.masm_symbol(o, externs)}, SEG DGROUP')
                else:
                    a = s * 16 + o
                    f = self.funcs.get(a)
                    if f is not None:
                        sym = '_' + (f['name'] or f'F_{a:05X}')
                        externs[sym] = 'FAR' if f['far'] else 'NEAR'
                        lines.append(f'        DD {sym}')
                    else:
                        best = max(((fs, fo, n) for fs, fo, n, d in self.fardata if fs == s and fo <= o), key=lambda t: t[1])
                        externs['_' + best[2]] = 'BYTE'
                        lines.append(f'        DW OFFSET _{best[2]}+{o - best[1]}, SEG _{best[2]}')
        return lines

    def write_dgroup_asm(self, path=config.SRC / 'data/DGROUP.ASM'):
        """The initialized data no C or MASM module owns yet, in original order."""
        converted = {u for u in self.order if self.unit_converted(u)}
        remaining = [it for it in self.items if it.owner not in converted and it.kind != 'lit'
                     and not (it.kind == 'gap' and it.note == 'align')]   # the owner's compiler aligns
        body, externs = self.masm_lines(remaining)
        own = {ln.split()[0] for ln in body if ' LABEL BYTE' in ln}
        externs = {s: k for s, k in externs.items() if s not in own}
        head = ['; Reconstructed game globals not yet owned by a source module: original order,',
                '; symbolic pointers. Regenerated by `./sr dgroup asm`; shrinks as modules take',
                '; their data. CRT storage comes from the real libraries.']
        ext = [f'EXTRN {s}:{k}' for s, k in sorted(externs.items())]
        text = '\n'.join(head + ext + ["_DATA SEGMENT WORD PUBLIC 'DATA'", 'DGROUP GROUP _DATA', 'ASSUME DS:DGROUP']
                         + body + ['_DATA ENDS', 'END', ''])
        path.write_text(text)
        return len(remaining)

    def unit_converted(self, ustart):
        """A unit owns its data in source when every named item it owns is defined there."""
        names = [it.name for it in self.items if it.owner == ustart and it.kind == 'global' and not it.synth]
        return bool(names) and all(n in self.defined for n in names)


    # ------------------------------------------------------------ writing sources
    def register(self, write=True):
        """Give the DB the synthesized globals (zero storage, typed table pieces, mask records,
        named strings) so the headers and the gate know them. Returns the new rows."""
        new = []
        for it in self.items:
            if it.kind == 'global' and it.synth and it.off not in self.globals:
                new.append((it.off, it.name, it.decl))
        if write:
            for off, name, decl in new:
                self.con.execute('INSERT INTO globals (off, name, decl, notes) VALUES (?, ?, ?, ?)',
                                 (off, name, decl, 'recovered from the data layout (./sr dgroup)'))
            self.con.commit()
        return new

    def write_order(self):
        order = list(self.order)
        # relocation-order evidence for objects without data: [units], after
        for hint in self.plan.get('order_hints', []):
            units = [int(s, 16) for s in hint['units']]
            after = int(hint['after'], 16)
            order = [s for s in order if s not in units]
            k = order.index(after) + 1
            order[k:k] = units
        # hints must not reorder code inside a segment
        regions = {}
        for k, s in enumerate(order):
            u = self.unit_by[s]
            if u['region'] is not None:
                regions.setdefault(u['region'], []).append(k)
        for r, slots in regions.items():
            for k, s in zip(slots, sorted(order[k] for k in slots)):
                order[k] = s
        self.plan['link_order'] = order
        self.plan['data_units'] = [dict(start=DATA_KEY + lo, lo=lo, hi=hi, sources=[src]) for lo, hi, src in DATA_UNITS]
        (config.SRC / 'link/LAYOUT.JSON').write_text(json.dumps(self.plan, indent=2) + '\n')

    def apply_unit(self, ustart, write=False):
        """Insert the unit's data definitions into its module sources. Returns {path: text}."""
        u = self.unit_by[ustart]
        changed = {}
        if u['kind'] == 'data' and u['sources'][0].upper().endswith('.ASM'):
            items = [it for it in self.items if it.owner == ustart and it.kind != 'lit']
            body, externs = self.masm_lines(items, label_public=True)
            own = {ln.split()[0] for ln in body if ' LABEL BYTE' in ln}
            ext = [f'        EXTRN   {s}:{k}' for s, k in sorted(externs.items()) if s not in own]
            head = ['        .MODEL MEDIUM', '',
                    f'; Initialized data DS:{u["lo"]:04X}..{u["hi"]:04X}, recovered from the original layout',
                    '; (./sr dgroup). Odd-address words and arrays: assembler data, not C definitions.', '',
                    '        .DATA'] + ext
            path = config.ROOT / u['sources'][0]
            changed[path] = '\n'.join(head + body + ['', '        END', ''])
            if write:
                path.write_text(changed[path])
            return changed
        if u['kind'] == 'data':
            lines = ['#include "GLOBALS.H"', '#include "PROTOS.H"', '']
            own = [it for it in self.items if it.owner == ustart and it.kind not in ('lit', 'str', 'alias')]
            # a definition that points at the item just before it was written first in the
            # original: MSC then emits the earlier item at the reference, in a record of its
            # own (the original relocation table shows one run per such item)
            k = 0
            while k + 1 < len(own):
                if own[k].kind == 'global' and own[k + 1].kind == 'global' and self._points_at(own[k + 1], own[k]):
                    own[k], own[k + 1] = own[k + 1], own[k]
                    k += 2
                else:
                    k += 1
            for it in own:
                text = self.definition(it)
                if text:
                    lines.append(text)
            path = config.ROOT / u['sources'][0]
            changed[path] = '\n'.join(lines) + '\n'
            # hand-kept communal declarations (uninitialized storage) follow the data
            old = path.read_text() if path.exists() else ''
            if COMMUNAL_MARK in old:
                changed[path] += '\n' + old[old.index(COMMUNAL_MARK):]
            if write:
                path.write_text(changed[path])
            return changed
        if u['kind'] == 'asm':
            items = [it for it in self.items if it.owner == ustart and it.kind != 'lit']
            if not items:
                return changed
            lines, externs = self.masm_lines(items, label_public=True)
            own = {ln.split()[0] for ln in lines if ' LABEL BYTE' in ln}
            path = config.ROOT / u['functions'][0]['src']
            text = path.read_text(errors='replace')
            text = self._strip_asm_definitions(text, [it.name for it in items if it.kind == 'global'])
            for sym in own:
                text = re.sub(rf'(?m)^[ \t]*EXTRN[ \t]+{re.escape(sym)}:\w+[ \t]*\r?\n', '', text)
            block = '\n'.join(['        ; data (recovered DGROUP layout)'] + lines) + '\n'
            m = re.search(r'(?m)^\s*\.DATA\s*$', text)
            if m is None:
                c = re.search(r'(?m)^\s*\.CODE\b', text)
                if c is None:
                    raise SystemExit(f'{path}: no .CODE section to put the data before')
                text = text[:c.start()] + '        .DATA\n\n' + text[c.start():]
                m = re.search(r'(?m)^\s*\.DATA\s*$', text)
            # after the EXTRNs that follow .DATA
            pos = m.end()
            for em in re.finditer(r'(?m)^\s*EXTRN\s+[^\n]*$', text[pos:]):
                if text[pos:pos + em.start()].strip():
                    break
                pos += em.end()
            ext = ''.join(f'        EXTRN   {s}:{k}\n' for s, k in sorted(externs.items())
                          if s not in own and not re.search(rf'EXTRN\s+{re.escape(s)}:', text))
            text = text[:pos] + '\n' + ext + block + text[pos:]
            changed[path] = text
        else:
            defs = self.c_definitions(ustart)
            names = [it.name for it in self.items if it.owner == ustart and it.kind == 'global']
            files = {}
            for slot, lines in defs.items():
                fn = self.funcs[slot] if slot is not None else u['functions'][0]
                path = config.ROOT / fn['src']
                files.setdefault(path, []).append((fn, lines))
            for path, inserts in files.items():
                text = path.read_text(errors='replace')
                text = self._strip_c_definitions(text, names)
                for fn, lines in inserts:
                    text = self._insert_before(text, fn['name'] or f"F_{fn['addr']:05X}", '\n'.join(lines) + '\n')
                changed[path] = text
        if write:
            for path, text in changed.items():
                path.write_text(text)
        return changed

    @staticmethod
    def _insert_before(text, fname, block):
        lines = text.split('\n')
        d = next((i for i in range(len(lines)) if not lines[i][:1].isspace()
                  and re.search(rf'\b{fname}\s*\(', lines[i]) and not lines[i].rstrip().endswith(';')), None)
        if d is None:
            raise SystemExit(f'{fname}: no definition found to insert data before')
        i = d
        while i > 0 and (not lines[i - 1].strip() or lines[i - 1].lstrip().startswith('/*') and lines[i - 1].rstrip().endswith('*/')):
            i -= 1
        # keep one blank line between the previous code and the data block
        before = lines[:i]
        while before and not before[-1].strip():
            before.pop()
        after = lines[i:]
        while after and not after[0].strip():
            after.pop(0)
        return '\n'.join(before + ([''] if before else []) + block.rstrip('\n').split('\n') + [''] + after)

    @staticmethod
    def _strip_c_definitions(text, names):
        """Remove the top-level definitions a previous run of this tool wrote (marked /* DS:xxxx */)."""
        pat = re.compile(r'/\* DS:[0-9A-F]{4} \*/')
        out, i, lines = [], 0, text.split('\n')
        while i < len(lines):
            ln = lines[i]
            if ln and (ln[0].isalpha() or ln[0] == '_') and not ln.lstrip().startswith(('static', 'extern')) \
                    and '=' in ln and _marked(lines, i, pat):
                # statement runs to the first ';' outside quotes/braces
                depth, inq, j = 0, False, i
                done = False
                while j < len(lines) and not done:
                    s = lines[j]
                    k = 0
                    while k < len(s):
                        c = s[k]
                        if inq:
                            if c == '\\':
                                k += 1
                            elif c == '"':
                                inq = False
                        elif c == '"':
                            inq = True
                        elif c == '{':
                            depth += 1
                        elif c == '}':
                            depth -= 1
                        elif c == ';' and depth == 0:
                            done = True
                            break
                        k += 1
                    j += 1
                i = j
                while i < len(lines) and not lines[i].strip() and out and not out[-1].strip():
                    i += 1
                continue
            out.append(ln)
            i += 1
        return '\n'.join(out)

    @staticmethod
    def _strip_asm_definitions(text, names):
        if not names:
            return text
        m = re.search(r'(?m)^        ; data \(recovered DGROUP layout\)\n', text)
        if m is None:
            return text
        end = re.search(r'(?m)^\s*\.CODE\b', text[m.start():])
        return text[:m.start()] + text[m.start() + end.start():]

    def apply_aliases(self, write=False):
        """Rewrite uses of alias globals as views of their base and drop them from the DB."""
        rew = self.alias_rewrites()
        changed = {}
        for path in sorted((config.SRC / 'mods').glob('*.C')):
            text = path.read_text(errors='replace')
            new = text
            for name, (expr, _) in rew.items():
                m = re.fullmatch(r'\((g_\w+) \+ (\d+)\)', expr)
                if m:
                    new = re.sub(rf'\b{re.escape(name)}\s*\[([^\]]+)\]', lambda mm: f'{m.group(1)}[{m.group(2)} + {mm.group(1)}]', new)
                new = re.sub(rf'\b{re.escape(name)}\b', expr, new)
            if new != text:
                changed[path] = new
        for path in sorted((config.SRC / 'mods').glob('*.ASM')):
            text = path.read_text(errors='replace')
            new = text
            needed = set()
            for name, (_, asm) in rew.items():
                if not re.search(rf'\b_{re.escape(name)}\b', new):
                    continue
                new = re.sub(rf'(?m)^[ \t]*EXTRN[ \t]+_{re.escape(name)}:\w+[ \t]*\r?\n', '', new)
                new = re.sub(rf'\b_{re.escape(name)}\b', asm, new)
                needed.add(asm.split('+')[0])
            for base in sorted(needed):
                ext = re.search(rf'EXTRN[ \t]+{re.escape(base)}:', new)
                use = re.search(rf'(?m)^(?![ \t]*EXTRN).*\b{re.escape(base)}\b', new)
                if ext and (use is None or ext.start() < use.start()):
                    continue
                m = re.search(r'(?m)^[ \t]*\.DATA[ \t]*\r?$', new)
                if m is None:
                    raise SystemExit(f'{path}: no .DATA section for EXTRN {base}')
                nl = '\r\n' if '\r\n' in new else '\n'
                new = new[:m.end()] + nl + f'        EXTRN   {base}:BYTE' + new[m.end():]
            if new != text:
                changed[path] = new
        if write:
            for path, text in changed.items():
                path.write_text(text)
            for name in rew:
                self.con.execute('DELETE FROM globals WHERE name = ?', (name,))
            self.con.commit()
        return rew, changed
