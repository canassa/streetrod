"""Read an MSC 5.1 / MASM 5.1 .OBJ: segments, data, fixups (with targets), publics, externs."""

import struct


class Fixup:
    __slots__ = ("seg", "pos", "loc", "self_rel", "kind", "index", "disp")

    def __init__(self, seg, pos, loc, self_rel, kind, index, disp):
        self.seg = seg          # SEGDEF index (1-based) of the LEDATA it patches
        self.pos = pos          # offset within that segment
        self.loc = loc          # 0 lobyte, 1 offset16, 2 base16, 3 ptr32, 5 loader offset16
        self.self_rel = self_rel
        self.kind = kind        # "seg" | "grp" | "ext" | "frame"
        self.index = index      # 1-based index into segments / groups / externs
        self.disp = disp

    @property
    def size(self):
        return {0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2}.get(self.loc, 2)


class Obj:
    def __init__(self, path):
        self.segments = []      # names, 1-based by index - 1
        self.groups = []
        self.communals = {}     # name -> byte size (COMDEF also consumes an external index)
        self.externs = []
        self.publics = {}       # code-segment publics: name -> offset
        self.all_publics = {}   # name -> (segment index, offset), including data
        self.data = {}          # SEGDEF index -> bytearray
        self.fixups = []
        self.code_seg = None
        self.raw = path if isinstance(path, bytes) else path.read_bytes()
        self._parse(self.raw)
        # MASM .MODEL may declare an empty default code segment before a
        # named .CODE contribution. Select the actual code, retaining its pubs.
        if not self.data.get(self.code_seg):
            self.code_seg = next((i for i, name in enumerate(self.segments, 1)
                                  if name.endswith('_TEXT') and self.data.get(i)), self.code_seg)
        self.publics = {name: off for name, (seg, off) in self.all_publics.items()
                        if seg == self.code_seg}
        self.code = self.data.get(self.code_seg, bytearray())
        self.mask = bytearray(len(self.code))
        for f in self.fixups:
            if f.seg == self.code_seg:
                for z in range(f.size):
                    if f.pos + z < len(self.mask):
                        self.mask[f.pos + z] = 1

    @staticmethod
    def _index(rec, k):
        if rec[k] & 0x80:
            return ((rec[k] & 0x7F) << 8) | rec[k + 1], k + 2
        return rec[k], k + 1

    def _parse(self, d):
        lnames, last, prev = [], None, None
        threads = {"frame": {}, "target": {}}
        i = 0
        while i + 3 <= len(d):
            t = d[i]
            n = struct.unpack("<H", d[i + 1 : i + 3])[0]
            rec = d[i + 3 : i + 2 + n]
            if t == 0x96:
                k = 0
                while k < len(rec):
                    lnames.append(rec[k + 1 : k + 1 + rec[k]].decode("latin-1"))
                    k += 1 + rec[k]
            elif t == 0x98:
                k = 1 if rec[0] >> 5 else 4
                k += 2
                name, k = self._index(rec, k)
                self.segments.append(lnames[name - 1])
                if self.code_seg is None and self.segments[-1].endswith("_TEXT"):
                    self.code_seg = len(self.segments)
            elif t == 0x9A:
                name, _ = self._index(rec, 0)
                self.groups.append(lnames[name - 1])
            elif t in (0x8C, 0xB4):  # EXTDEF, LEXTDEF (static): one index space
                k = 0
                while k < len(rec):
                    self.externs.append(rec[k + 1 : k + 1 + rec[k]].decode("latin-1"))
                    k += 1 + rec[k]
                    _, k = self._index(rec, k)
            elif t in (0xB0, 0xB8):  # COMDEF / LCOMDEF
                k = 0
                while k < len(rec):
                    length = rec[k]
                    name = rec[k+1:k+1+length].decode('latin-1')
                    k += 1+length
                    _, k = self._index(rec, k)
                    kind = rec[k]
                    k += 1
                    values = []
                    for _ in range(2 if kind == 0x61 else 1):
                        tag = rec[k]
                        k += 1
                        if tag <= 0x80:
                            value = tag
                        else:
                            width = {0x81: 2, 0x84: 3, 0x88: 4}[tag]
                            value = int.from_bytes(rec[k:k+width], 'little')
                            k += width
                        values.append(value)
                    self.communals[name] = values[0]*(values[1] if len(values) == 2 else 1)
                    self.externs.append(name)
            elif t in (0x90, 0xB6):  # PUBDEF, LPUBDEF (static)
                _, k = self._index(rec, 0)
                seg, k = self._index(rec, k)
                if seg == 0:
                    k += 2
                while k < len(rec):
                    ln = rec[k]
                    name = rec[k + 1 : k + 1 + ln].decode("latin-1")
                    off = struct.unpack("<H", rec[k + 1 + ln : k + 3 + ln])[0]
                    k += 3 + ln
                    _, k = self._index(rec, k)
                    self.all_publics[name] = (seg, off)
                    if seg == self.code_seg:
                        self.publics[name] = off
            elif t == 0xA0:
                seg, k = self._index(rec, 0)
                off = struct.unpack("<H", rec[k : k + 2])[0]
                body = rec[k + 2 :]
                buf = self.data.setdefault(seg, bytearray())
                if len(buf) < off + len(body):
                    buf.extend(b"\0" * (off + len(body) - len(buf)))
                buf[off : off + len(body)] = body
                last = (seg, off)
            elif t == 0xA2:  # LIDATA (MASM DUP): expanded into the segment's bytes
                seg, k = self._index(rec, 0)
                off = struct.unpack("<H", rec[k : k + 2])[0]
                body, k = bytearray(), k + 2
                while k < len(rec):
                    part, k = self._iterated(rec, k)
                    body += part
                buf = self.data.setdefault(seg, bytearray())
                if len(buf) < off + len(body):
                    buf.extend(b"\0" * (off + len(body) - len(buf)))
                buf[off : off + len(body)] = body
            elif t == 0x9C:
                if last is None and prev == 0xA2:
                    raise ValueError("FIXUPP after LIDATA is not supported")
                self._fixupp(rec, last, threads)
            if t not in (0x9C, 0xA0):
                last = None
            if t == 0x8A:
                break
            prev = t
            i += 3 + n

    @staticmethod
    def _iterated(rec, k):
        """One LIDATA data block (16-bit form): repeat count, block count, then the content
        (a counted byte string, or that many nested blocks)."""
        repeat, blocks = struct.unpack("<HH", rec[k : k + 4])
        k += 4
        if blocks == 0:
            content = bytes(rec[k + 1 : k + 1 + rec[k]])
            k += 1 + rec[k]
        else:
            content = bytearray()
            for _ in range(blocks):
                part, k = Obj._iterated(rec, k)
                content += part
        return bytes(content) * repeat, k

    def _fixupp(self, rec, last, threads):
        k = 0
        while k < len(rec):
            b = rec[k]
            if not b & 0x80:
                is_frame = bool(b & 0x40)
                method = (b >> 2) & 7
                num = b & 3
                k += 1
                idx = None
                if method < 3:
                    idx, k = self._index(rec, k)
                threads["frame" if is_frame else "target"][num] = (method, idx)
                continue
            self_rel = not (b & 0x40)
            loc = (b >> 2) & 0xF
            pos = ((b & 3) << 8) | rec[k + 1]
            k += 2
            fd = rec[k]
            k += 1
            if not fd & 0x80:
                if (fd >> 4) & 7 < 3:
                    _, k = self._index(rec, k)
            if fd & 0x08:
                method, idx = threads["target"].get(fd & 3, (0, None))
                method &= 3
            else:
                method = fd & 3
                idx, k = self._index(rec, k)
            disp = 0
            if not fd & 0x04:
                disp = struct.unpack("<H", rec[k : k + 2])[0]
                k += 2
            if last is None:
                continue
            kind = {0: "seg", 1: "grp", 2: "ext", 3: "frame"}[method]
            self.fixups.append(Fixup(last[0], last[1] + pos, loc, self_rel, kind, idx, disp))

    def target_name(self, f):
        if f.kind == "ext":
            return self.externs[f.index - 1]
        if f.kind == "seg":
            return self.segments[f.index - 1]
        if f.kind == "grp":
            return self.groups[f.index - 1]
        return None
