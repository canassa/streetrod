import base64
from dataclasses import dataclass
from pathlib import Path
import re
import struct
import sys
import zlib

from . import config


RGB16 = (
    (0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170),
    (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
    (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255),
    (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255),
)


def source_array(filename, name):
    text = config.source_path(filename).read_text(encoding='latin-1')
    match = re.search(r'\b' + re.escape(name) + r'\s*\[\d+\]\s*=\s*\{([^}]+)\}', text)
    if not match:
        raise ValueError(f'{filename}: initializer for {name} not found')
    return tuple(int(n.strip(), 0) for n in match[1].split(','))


def picture_index(picture_id, mode='ega'):
    if not (1 <= picture_id <= 9 or 1000 <= picture_id <= 1267):
        raise ValueError(f'unknown picture id {picture_id}; use --list')
    index = picture_id - (991 if picture_id >= 1000 else 1)
    if mode == 'cga':
        index = source_array('MENU.C', 'g_cgaPicMap')[index]
    return index


def picture_ids():
    return [*range(1, 10), *range(1000, 1268)]


@dataclass(frozen=True)
class Entry:
    library: Path
    index: int
    width: int
    height: int
    flags: int
    dictionary: bytes
    offset: int
    size: int

    def payload(self):
        with self.library.open('rb') as stream:
            stream.seek(self.offset)
            data = stream.read(self.size + 1)
        if len(data) != self.size + 1 or data[:1] != b'\xbc' or data[-1:] != b'\xbc':
            raise ValueError(f'{self.library.name} entry {self.index}: invalid BC framing')
        return data[1:-1]


def read_library(path):
    path = Path(path)
    with path.open('rb') as stream:
        count_bytes = stream.read(2)
        if len(count_bytes) != 2:
            raise ValueError(f'{path}: missing picture count')
        count = struct.unpack('<H', count_bytes)[0]
        directory = stream.read(count * 30)
    if len(directory) != count * 30:
        raise ValueError(f'{path}: truncated picture directory')
    entries = []
    for index in range(count):
        header = directory[index * 30:(index + 1) * 30]
        width, height = struct.unpack_from('<HH', header)
        offset, size = struct.unpack_from('<IH', header, 24)
        if not width or not height or header[6] > 3 or not size:
            raise ValueError(f'{path} entry {index}: invalid picture header')
        entries.append(Entry(path, index, width, height, header[6], header[7:23],
                             2 + count * 30 + offset, size))
    return entries


def libraries(mode='ega'):
    names = ('LIB1', 'LIB2') if mode == 'ega' else ('LIB1C', 'LIB2C')
    entries = []
    for name in names:
        paths = [config.ROOT / 'orig' / folder / name for folder in ('freeware', 'extra')]
        path = next((p for p in paths if p.is_file()), None)
        if path is None:
            raise ValueError(f'missing resource {name} under orig/freeware or orig/extra')
        entries.extend(read_library(path))
    if len(entries) != 277:
        raise ValueError(f'{mode}: expected 277 picture entries, found {len(entries)}')
    return entries


def decode_rle(data, dictionary, expected, counts=None, values=None):
    if len(dictionary) != 16:
        raise ValueError('RLE dictionary must contain 16 bytes')
    if counts is None:
        counts = source_array('DISK.C', 'g_rleCounts')
    if values is None:
        values = source_array('DISK.C', 'g_rleValues')
    codes = {token: (counts[i], values[i]) for i, token in enumerate(dictionary)}
    out = bytearray()
    pos = 0
    while pos < len(data):
        token = data[pos]
        pos += 1
        if token in (0, 255):
            if pos == len(data):
                raise ValueError('truncated RLE run')
            count, value = data[pos] + 1, token
            pos += 1
        else:
            count, value = codes.get(token, (1, token))
        if len(out) + count > expected:
            raise ValueError(f'RLE output exceeds {expected} bytes')
        out.extend(bytes([value]) * count)
    if len(out) != expected:
        raise ValueError(f'RLE output has {len(out)} bytes, expected {expected}')
    return bytes(out)


@dataclass(frozen=True)
class Picture:
    width: int
    height: int
    pixels: bytes

    def __post_init__(self):
        if self.width <= 0 or self.height <= 0 or len(self.pixels) != self.width * self.height * 4:
            raise ValueError('invalid RGBA picture dimensions')


def palette_colors(name='game', mode='ega'):
    if mode == 'cga':
        return tuple(RGB16[n] for n in ((0, 10, 12, 14) if name == 'cga-green' else (0, 11, 13, 15)))
    if name == 'standard':
        return RGB16
    filename, symbol = ('DASH.C', 'g_introPalette') if name == 'intro' else ('VIDEO.C', 'g_palette')
    return tuple(RGB16[n] for n in source_array(filename, symbol)[:16])


def decode(entry, mode='ega', palette='game'):
    pitch = (entry.width + (7 if mode == 'ega' else 3)) // (8 if mode == 'ega' else 4)
    plane_size = pitch * entry.height
    mask_only = bool(entry.flags & 2)
    expected = plane_size * (1 if mask_only else 4 if mode == 'ega' else 2 if entry.flags else 1)
    raw = decode_rle(entry.payload(), entry.dictionary, expected)
    colors = palette_colors(palette, mode)
    pixels = bytearray()
    for y in range(entry.height):
        for x in range(entry.width):
            offset = y * pitch + x // (8 if mode == 'ega' else 4)
            if mode == 'ega':
                shift = 7 - x % 8
                value = sum(((raw[offset + plane_size * plane] >> shift) & 1) << plane
                            for plane in range(1 if mask_only else 4))
                alpha = 0 if entry.flags == 1 and value == 0 else 255
                rgb = (value * 255,) * 3 if mask_only else colors[value]
            else:
                shift = 6 - (x % 4) * 2
                value = (raw[offset] >> shift) & 3
                alpha = 0 if entry.flags == 1 and ((raw[offset + plane_size] >> shift) & 3) == 3 else 255
                rgb = (value * 85,) * 3 if mask_only else colors[value]
            pixels.extend((*rgb, alpha))
    return Picture(entry.width, entry.height, bytes(pixels))


def cut_cells(picture, columns, count):
    if columns <= 0 or count <= 0 or count > columns * 2:
        raise ValueError('--cells needs positive COLS and N with N <= 2 * COLS')
    width, height = picture.width // columns, picture.height // 2
    if not width or not height:
        raise ValueError('sprite cells would have zero width or height')
    rows = []
    for y in range(height):
        row = bytearray()
        for cell in range(count):
            start = ((cell // columns * height + y) * picture.width + cell % columns * width) * 4
            row.extend(picture.pixels[start:start + width * 4])
        rows.append(row)
    return Picture(width * count, height, b''.join(rows))


def scale(picture, factor):
    if factor <= 0:
        raise ValueError('--scale must be positive')
    if factor == 1:
        return picture
    rows = []
    stride = picture.width * 4
    for start in range(0, len(picture.pixels), stride):
        row = b''.join(picture.pixels[i:i + 4] * factor for i in range(start, start + stride, 4))
        rows.extend([row] * factor)
    return Picture(picture.width * factor, picture.height * factor, b''.join(rows))


def stack(pictures):
    width = max(p.width for p in pictures)
    rows = []
    for picture in pictures:
        stride = picture.width * 4
        padding = bytes((width - picture.width) * 4)
        rows.extend(picture.pixels[i:i + stride] + padding for i in range(0, len(picture.pixels), stride))
    return Picture(width, sum(p.height for p in pictures), b''.join(rows))


def png(picture):
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))
    stride = picture.width * 4
    data = b''.join(b'\0' + picture.pixels[i:i + stride] for i in range(0, len(picture.pixels), stride))
    header = struct.pack('>IIBBBBB', picture.width, picture.height, 8, 6, 0, 0, 0)
    return b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', header) + chunk(b'IDAT', zlib.compress(data)) + chunk(b'IEND', b'')


def kitty(picture):
    data = base64.b64encode(png(picture))
    messages = []
    for start in range(0, len(data), 4096):
        part = data[start:start + 4096]
        control = b'a=T,f=100,q=2,' if start == 0 else b''
        more = int(start + 4096 < len(data))
        messages.append(b'\x1b_G' + control + f'm={more};'.encode() + part + b'\x1b\\')
    return b''.join(messages)


def run(args):
    try:
        if args.scale <= 0:
            raise ValueError('--scale must be positive')
        if args.cells and (min(args.cells) <= 0 or args.cells[1] > args.cells[0] * 2):
            raise ValueError('--cells needs positive COLS and N with N <= 2 * COLS')
        if args.list and (args.ids or args.png or args.cells):
            raise ValueError('--list cannot be combined with ids, --png or --cells')
        if not args.list and not args.ids:
            raise ValueError('give one or more picture ids, or --list')
        if args.mode == 'ega' and args.palette == 'cga-green':
            raise ValueError('--palette cga-green requires --mode cga')
        entries = libraries(args.mode)
        if args.list:
            print('ID     SIZE       PACKED  FLAGS  LIBRARY')
            for picture_id in picture_ids():
                entry = entries[picture_index(picture_id, args.mode)]
                print(f'{picture_id:<6} {entry.width:>3}x{entry.height:<3} {entry.size:>8} '
                      f'{entry.flags:>5}  {entry.library.relative_to(config.ROOT)}')
            return
        pictures = []
        for picture_id in args.ids:
            entry = entries[picture_index(picture_id, args.mode)]
            picture = decode(entry, args.mode, args.palette)
            if args.cells:
                picture = cut_cells(picture, *args.cells)
            picture = scale(picture, args.scale)
            pictures.append(picture)
        if args.png:
            args.png.write_bytes(png(stack(pictures)))
            print(f'Wrote {args.png}: {max(p.width for p in pictures)}x{sum(p.height for p in pictures)}')
        else:
            for picture_id, picture in zip(args.ids, pictures):
                print(f'Picture {picture_id}: {picture.width}x{picture.height}', flush=True)
                sys.stdout.buffer.write(kitty(picture) + b'\n')
                sys.stdout.buffer.flush()
    except (OSError, ValueError) as error:
        raise SystemExit(f'sr pic: {error}') from error
