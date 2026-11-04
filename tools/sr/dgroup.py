"""Recover DGROUP ownership from positions, and check the sources' data definitions against it.

MSC 5.1 emits a module's _DATA as: string literals immediately in parse order (an initializer's
strings at the declaration, a function's literals while it compiles), and named initialized
data buffered until the next function is emitted, then flushed after that function's literals,
each item word-aligned when its type is. LINK concatenates the modules' _DATA in object order,
so the original offsets of a unit's literal pools pin the unit in the link order, and a named
item's offset tells which function it was declared before.

Reads the original image, the DB and the layout build's objects (`./sr build`, for
the literal pools). The sources are the truth for the definitions: `check` verifies that every
named item is defined in the file that owned it; the build checks the bytes.
"""
import bisect
import re
import struct
from pathlib import Path

from . import config, db, makefile, module
from .cdata import Types
from .exe import image
from .omf import Obj

DATA_END = 0x629C          # first runtime _DATA byte (crt0's _asizds)
BSS_START, BSS_PRIVATE_END, COMMON_START, END = 0x6C22, 0x6E2C, 0x6E60, 0x8F02
# Recovered array bounds and typed table pieces (from the earlier data audit).
BOUNDS = {'g_videoMenu': 6, 'g_gearNames': 7, 'g_palette': 16, 'g_protectPal': 16, 'g_5082': 4, 'g_511e': 4,
          'g_keyMaps': 43, 'g_scenery': 21, 'g_scanMap': 0x66, 'g_4806': 44, 'g_4998': 5, 'ae7312y': 5, 'bp7316': 4}
REGIONS = [(0x6076, 0x6078, 'int @'), (0x2ee, 0x31e, 'signed char @[24][2]'), (0x358, 0x36e, 'signed char @[11][2]'),
           (0x462, 0x472, 'unsigned char @[16]'), (0x5d4, 0x5d6, 'int @'),
           (0x4734, 0x4736, 'int @'), (0x474a, 0x474c, 'int @'),
           (0x47d1, 0x47d4, 'char @[3]'), (0x496c, 0x4970, 'int @[2]')]
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
              (0x5EC6, 0x60A2, 'src/mods/SCENERY.C'),
              (0x60A2, 0x629C, 'src/mods/SCENERY2.C')]
DATA_KEY = 0x100000
# Relocation-order evidence for objects whose position no literal pool pins: (units, after,
# why). `link_order()` moves the units after `after`; each code segment's units then keep
# their code order.
ORDER_HINTS = [
    (('24602', '2462E', '2487A', '24BE2'), '22364',
     'the original relocation table lists tandytxt_render SEG fixup between 22364 and 226F4'),
    (('26458', '265BA', '2869E', '2B6DC', '105EC6', '1060A2'), '242EC',
     'the original relocation table lists 23F68 and 242EC before the C units of segment 2645; '
     'the car-mask data follows 286D8 in DGROUP'),
]
PRINTABLE = set(range(32, 127)) | {9, 10, 13}




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


def defined_in_sources(prefix='g_'):
    """global name -> source file, for the initialized globals the C and MASM sources define
    (C file-scope definitions with an initializer, MASM PUBLIC data labels); names starting
    with `prefix`. SOURCE_DECLS keeps each C definition's declaration (with @ for the name)."""
    out = {}
    name = re.escape(prefix) + r'\w*' if prefix else r'[A-Za-z_]\w*'
    for path in sorted((config.SRC / 'mods').glob('*.C')):
        text = re.sub(r'/\*.*?\*/', '', path.read_text(errors='replace'), flags=re.S)
        for m in re.finditer(rf'(?m)^(?!extern\b|static\b|#)([A-Za-z_][\w *(]*?)\b({name})\s*((?:\[[^\]]*\]\s*)*)(?:\)\s*\([^)]*\))?\s*=', text):
            out[m.group(2)] = str(path.relative_to(config.ROOT))
            if m.group(2).startswith('g_'):
                SOURCE_DECLS[m.group(2)] = (m.group(1) + '@' + m.group(3)).replace(' @', ' @').strip()
    for path in sorted((config.SRC / 'mods').glob('*.ASM')):
        text = path.read_text(errors='replace')
        for m in re.finditer(rf'(?m)^\s*PUBLIC\s+_({name})', text):
            if re.search(rf'(?m)^_{m.group(1)}\s+(LABEL|D[BWDQ])\b', text):
                out[m.group(1)] = str(path.relative_to(config.ROOT))
    return out




class Analysis:
    def __init__(self):
        self.con = db.connect()
        self.im = image()
        self.types = Types()
        self.ds = config.DGROUP_SEG * 16
        self.dg = bytes(self.im.img[self.ds:self.ds + END])
        self.globals = {r['off']: dict(r) for r in self.con.execute('SELECT * FROM globals ORDER BY off')}
        self.goffs = sorted(self.globals)
        self.fardata = [(r['seg'], r['off'], r['name'], r['decl']) for r in
                        self.con.execute('SELECT seg, off, name, decl FROM fardata ORDER BY seg, off')]
        self.funcs = {r['addr']: dict(r) for r in self.con.execute('SELECT * FROM functions ORDER BY addr')}
        self.faddrs = sorted(self.funcs)
        files = module.files()      # the code files of src/MAKEFILE, extents from the DB
        self.units = [dict(start=f.start, end=f.end, sources=[f.rel], kind=f.kind, flags=f.flags) for f in files]
        self.code_starts = module.code_starts(files)
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
        stem = Path(u['sources'][0]).stem          # the layout build compiles each file under its own name
        fresh = config.WORK / 'layout' / 'BUILD' / stem / (stem + '.OBJ')
        gated = config.WORK / 'layout' / (stem + '.OBJ')
        if fresh.exists() and (not gated.exists() or fresh.stat().st_mtime >= gated.stat().st_mtime):
            return fresh
        if gated.exists():
            return gated
        raise SystemExit(f'{gated} missing: run ./sr build first')

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
                sizes = self._cod_literal_sizes(Path(u['sources'][0]).stem)
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
        """global name -> {unit start: fixup count} from the layout objects.

        Ownership is decided from these sets, so they must not depend on where a global is
        defined now (the previous `apply`): a reference counts the same as an EXTDEF fixup
        when another file defines the global and as a fixup to the object's own _DATA, inside
        the global's extent, when this file does. In a C object both code and data
        initializers count (`char *g_noNameMsgPtr = g_noNameMsg;` is SAVEGAME's reference to DS:4FA6, which
        no code uses; counting it only as an external made DS:4FA6 flip between GAME and
        SAVEGAME on every apply). In an assembler object only code counts: its data tables
        point into the blocks FORCE_OWNER and DATA_UNITS assign anyway."""
        if hasattr(self, '_refs'):
            return self._refs
        name2off = {'_' + g['name']: off for off, g in self.globals.items()}
        byname = {'_' + g['name']: g for g in self.globals.values()}
        refs = {}

        def count(name, u):
            refs.setdefault(name[1:], {}).setdefault(u['start'], 0)
            refs[name[1:]][u['start']] += 1

        for u in self.units:
            if u['kind'] == 'data':
                continue
            obj = Obj(self._unit_obj(u))
            dseg = obj.segments.index('_DATA') + 1 if '_DATA' in obj.segments else None
            segs = {obj.code_seg} | ({dseg} if u['kind'] == 'c' and dseg else set())
            for f in obj.fixups:
                if f.kind == 'ext' and f.seg in segs and obj.externs[f.index - 1] in name2off:
                    count(obj.externs[f.index - 1], u)
            if not dseg:
                continue
            pubs = sorted((off, off + self._decl_size(byname[name]), name)
                          for name, (seg, off) in obj.all_publics.items() if seg == dseg and name in byname)
            starts = [lo for lo, _, _ in pubs]
            for f in obj.fixups:
                if f.seg not in segs or f.kind != 'seg' or f.index != dseg or f.loc not in (1, 5) or f.self_rel:
                    continue
                data = obj.data.get(f.seg, b'')
                objoff = (int.from_bytes(data[f.pos:f.pos + 2], 'little') + f.disp) & 0xFFFF
                i = bisect.bisect_right(starts, objoff) - 1
                if i < 0 or objoff >= pubs[i][1]:
                    continue        # a literal, not named data
                if f.seg == dseg and pubs[i][0] <= f.pos < pubs[i][1]:
                    continue        # a table pointing into itself says nothing about its owner
                count(pubs[i][2], u)
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

    def forward_refs(self):
        """(item, target) where an initializer in a C unit takes the address of a global the
        same unit defines later: MSC would emit the target first, so the order cannot hold."""
        out = []
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

    # ------------------------------------------------------------ checking sources
    def check_sources(self):
        """Where the sources' initialized DGROUP data disagrees with the recovered layout:
        every named item must be defined in the source file of the unit that owned it in the
        original, and nothing the layout does not hold as a global may be defined. The bytes
        themselves are checked by the build (datacheck per object, the linked EXE)."""
        defined = defined_in_sources(prefix='')
        problems, seen = [], set()
        for it in self.items:
            if it.kind != 'global':
                continue
            seen.add(it.name)
            if it.owner is None:
                problems.append(f'DS:{it.off:04X} {it.name}: no unit owns it')
                continue
            want = self.unit_by[it.owner]['sources'][0]
            have = defined.get(it.name)
            if have is None:
                problems.append(f'DS:{it.off:04X} {it.name}: not defined; {want} owned it')
            elif have != want:
                problems.append(f'DS:{it.off:04X} {it.name}: defined in {have}, but {want} owned it')
        for g in self.con.execute('SELECT off, name FROM globals WHERE off < ?', (DATA_END,)):
            if g['name'] in defined and g['name'] not in seen:
                problems.append(f'DS:{g["off"]:04X} {g["name"]}: defined in {defined[g["name"]]}, '
                                'but the layout holds no such global (literal, alias or gap)')
        for name, (base, d) in self.aliases.items():
            problems.append(f'{name}: a DB global inside {base.name} (+{d}); write it as a view of {base.name}')
        for it in self.items:
            if it.kind == 'global' and it.synth and it.off not in self.globals:
                problems.append(f'DS:{it.off:04X} {it.name}: the layout holds a global the DB lacks ({it.decl})')
        return problems

    # ------------------------------------------------------------ the DB
    def link_order(self):
        """The recovered order of the units: the literal-pool order with ORDER_HINTS applied."""
        order = list(self.order)
        # relocation-order evidence for objects without data: [units], after
        for hint_units, hint_after, _why in ORDER_HINTS:
            units = [int(s, 16) for s in hint_units]
            after = int(hint_after, 16)
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
        return order

    def response_order(self):
        """SR.LNK's object list with the units in the recovered order (the other objects,
        far data and storage, keep their lines)."""
        name = {u['start']: Path(u['sources'][0]).stem.upper() + '.OBJ' for u in self.units}
        objects = makefile.link_order()
        mine = [name[s] for s in self.link_order()]
        missing = set(mine) - set(objects)
        if missing:
            raise SystemExit('SR.LNK lacks ' + ', '.join(sorted(missing)))
        it = iter(mine)
        return [next(it) if o in set(mine) else o for o in objects]

