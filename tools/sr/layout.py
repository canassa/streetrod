"""MSC 5.1 auto-local frame layout: predict [bp-N] slots from names, and suggest names.

The rule (measured; the recipe book has the details):
- A block's locals sit in a 16-bucket symbol table, bucket = (byte sum of the name, first
  31 chars) & 15, each new symbol inserted at the head of its chain. Slots are handed out
  walking buckets 0..15, chain order within a bucket: lower bucket = higher in the frame,
  and within a bucket the later-declared local sits higher.
- A slot is the size rounded up to even. Register locals get a slot too (their home).
- When a nested block closes its slots are freed. A later block takes the deepest free slot
  that is big enough (no splitting, no merging) before growing the frame.
- With /Ol two more rules apply. A block only reuses slots last held at its own nesting
  depth or deeper. The first block after the start of its level or after a label (any
  `if`/loop/`else`/`?:`...; written `|` here) keeps its slots until the next block at that
  level closes or a label comes, so the block right after it never reuses them.
  /Ol also takes hidden temps for strength-reduced loops (`p + i * 40` in a loop): a slot
  below everything allocated so far, written `~` (or `~4` for 4 bytes) where the loop is.
"""

import itertools
import re

TYPES = {"int": 2, "unsigned": 2, "short": 2, "char": 1, "ptr": 2, "near": 2, "long": 4,
         "far": 4, "float": 4, "double": 8, "reg": 2}
KEYWORDS = set("auto break case char const continue default do double else enum extern float "
               "for goto if int long register return short signed sizeof static struct switch "
               "typedef union unsigned void volatile while far near huge pascal cdecl fortran "
               "interrupt".split())
LABEL = "|"
TEMP = re.compile(r"~(\d*)$")  # a compiler temp of N bytes (default 2) taken at that point


# Plausible 1989 local names, for picking a readable one in a wanted bucket (`sr layout --bucket`).
COMMON = """a b c d e f g h i j k l m n o p q r s t u v w x y z
ax ay bx by dx dy cx cy x0 y0 x1 y1 x2 y2 i1 i2 j1 j2 n1 n2 p1 p2 q1 q2 c1 c2 s1 s2 w1 w2 h1 h2
buf str msg name text line key ch chr cnt count num len size max min sum total tmp temp val value
old new cur prev next last first top left right bot bottom width height wid ht pitch row col rows cols
src dst dest from to pos off ofs seg base ptr pp np lp sp bp tp ip fp mp list tbl table item node
idx index ix iy kx ky dir step inc dec delta diff err rc ret res result flag flags mode state type kind
mask bits bit byte word lo hi hw lw color colr pen fill shade pal plane bank page frame pic pict img
bm bmp map car rack part slot spot hot obj win box rect clip blt pkt rx ry rw rh sx sy sw sh
speed rpm gear time tick ticks delay wait loop pass done found ok hit ix1 ix2 iy1 iy2 xs ys xe ye
xl xr yt yb xmin xmax ymin ymax lim limit end start stop half dist cost price money cash score
nx ny nw nh ox oy ow oh px py pw ph tx ty tw th wx wy dw dh mx my bw bh cw ch2 fw fh lw lh
col1 col2 row1 row2 len1 len2 cnt1 cnt2 tmp1 tmp2 sum1 sum2 val1 val2 off1 off2 pos1 pos2
key1 key2 ret1 hi1 lo1 save saved keep hold orig copy back front fore dest2 from1 to1 cur1
add sub mul div rem mod quot sign neg abs amt unit units lap laps lane gas brake gear1 shift
cyl hp torque grip drag lift tire tires wheel body door hood roof seat engine motor axle cam
font glyph cell cells span spans run runs edge edges side sides peak flat ramp hill road track
nkey nsel sel sel1 pick choice opt opts menu entry entries rec recs file fd handle name1 path
buf1 buf2 str1 str2 msg1 txt line1 lines word1 chars nch nlen slen dlen width1 high wide deep
ok1 bad err1 fail done1 more any all none skip kept used free full empty open close shut
""".split()


def bucket(name):
    return sum(name[:31].encode()) & 15


def walk(names):
    """Allocation order of one block's locals, given in declaration order."""
    chains = {}
    for n in names:
        chains.setdefault(bucket(n), []).insert(0, n)
    return [n for b in sorted(chains) for n in chains[b]]


class Var:
    def __init__(self, name, size=2, reg=False):
        self.name, self.size, self.reg = name, (size + 1) & ~1, reg
        self.off = None
        self.where = None  # "si"/"di" when a register local gets one


def parse(tokens):
    """`a b:long c:10 { d e:reg } | { f }` -> nested lists of Var, LABEL and sub-blocks.
    A type is a byte count or a TYPES key."""
    root, stack = [], []
    cur = root
    for tok in " ".join(tokens).replace("{", " { ").replace("}", " } ").replace("|", " | ").split():
        if tok == "{":
            stack.append(cur)
            cur.append([])
            cur = cur[-1]
        elif tok == "}":
            if not stack:
                raise SystemExit("unbalanced }")
            cur = stack.pop()
        elif tok == LABEL:
            cur.append(LABEL)
        elif TEMP.match(tok):
            cur.append(tok)
        else:
            name, _, kind = tok.partition(":")
            if not re.fullmatch(r"[A-Za-z_]\w*", name):
                raise SystemExit(f"bad name: {name}")
            size = int(kind, 0) if kind[:1].isdigit() else TYPES.get(kind or "int")
            if size is None:
                raise SystemExit(f"unknown type {kind!r} (use a byte count or one of {', '.join(TYPES)})")
            cur.append(Var(name, size, kind == "reg"))
    return root


def place(block, ol=False):
    """Assign .off (and .where for register locals) to every Var; return the frame size."""
    free, pending, frame, pool, depth_of = [], [], [0], ["si", "di"], {}

    def alloc(vs, depth):
        taken = []
        by = {v.name: v for v in vs}
        for v in (by[n] for n in walk([v.name for v in vs])):
            fit = [s for s in free if s[1] >= v.size and (not ol or depth_of[s] >= depth)]
            if fit:
                slot = min(fit)  # the deepest one
                free.remove(slot)
            else:
                frame[0] += v.size
                slot = (-frame[0], v.size)
            depth_of[slot] = depth
            v.off = slot[0]
            taken.append(slot)
        return taken

    def run(blk, depth):
        vs = [v for v in blk if isinstance(v, Var)]
        mine = []
        if not ol:  # registers go by declaration order, while the block holds them
            for v in vs:
                if v.reg and pool:
                    v.where = pool.pop(0)
                    mine.append(v.where)
        taken = alloc(vs, depth)
        first = True
        for sub in blk:
            if sub == LABEL:
                free.extend(pending)
                pending.clear()
                first = True
            elif isinstance(sub, str):  # a hidden temp: grows the frame, never freed
                frame[0] += (int(TEMP.match(sub).group(1) or 2) + 1) & ~1
            elif isinstance(sub, list):
                slots = run(sub, depth + 1)
                free.extend(pending)
                pending.clear()
                (pending if ol and first else free).extend(slots)
                first = False
        pool.extend(mine)
        pool.sort()
        return taken

    run(block, 0)
    return frame[0]


def _blocks(blk, depth=0):
    yield blk, depth
    for sub in blk:
        if isinstance(sub, list):
            yield from _blocks(sub, depth + 1)


def show(block, ol=False):
    frame = place(block, ol)
    for blk, depth in _blocks(block):
        vs = sorted((v for v in blk if isinstance(v, Var)), key=lambda v: -v.off)
        pad = "  " * depth
        if depth:
            print(f"  {pad[2:]}{{")
        for v in vs:
            note = f"  in {v.where} (the slot is its home)" if v.where else ""
            if v.reg and ol:
                note = "  register: /Ol picks its own; the slot is reserved either way"
            print(f"  {pad}{v.off:5d}  {v.name:<16} bucket {bucket(v.name):2d}  size {v.size}{note}")
        if depth:
            print(f"  {pad[2:]}}}")
    print(f"  frame {frame} bytes")


# ---------------------------------------------------------------- suggest names

def _lnds(bs):
    """Indices of a longest non-decreasing subsequence of bs."""
    best = []
    for i, b in enumerate(bs):
        cand = [s for s in best if bs[s[-1]] <= b]
        best.append(max(cand, key=len, default=[]) + [i])
    return set(max(best, key=len, default=[]))


def _variants(name):
    """Small edits of a name, cheapest first."""
    digits, letters = "123456789", "sxtnpcrlbkdmaeiouvwyzfghjq"
    yield from (name + c for c in digits)
    yield from (name + c for c in letters)
    if len(name) > 1:
        yield from (name[:-1] + c for c in letters + digits)
    yield from (name + "_" + c for c in letters)
    yield from (name + a + b for a, b in itertools.product(letters, digits))


def suggest(names, avoid=()):
    """names in the wanted top-to-bottom order of one block. Keep the longest run whose
    buckets already ascend, rename the rest with small edits. Returns (names, decl order)."""
    bs = [bucket(n) for n in names]
    keep = _lnds(bs)
    out = list(names)
    used = set(names) | set(avoid)
    for i in range(len(out)):
        if i in keep:
            continue
        lo = bucket(out[i - 1]) if i else 0
        hi = min([bs[j] for j in keep if j > i], default=15)
        for cand in _variants(names[i]):
            if cand not in used and cand not in KEYWORDS and lo <= bucket(cand) <= hi:
                used.add(cand)
                out[i] = cand
                break
        else:
            raise SystemExit(f"no rename found for {names[i]}")
    # Within a bucket the later-declared local sits higher: declare each run bottom-up.
    decl = []
    for _, run in itertools.groupby(out, key=bucket):
        decl.extend(list(run)[::-1])
    assert walk(decl) == out
    return out, decl


def by_bucket(n, avoid=()):
    return [w for w in COMMON if bucket(w) == n and w not in avoid]
