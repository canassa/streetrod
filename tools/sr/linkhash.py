"""MSC LINK 5.01.21 symbol hashing, recovered from its NE segment 2:4060.

The near-COMDEF allocator walks 256 buckets in order. Historical variable
names are unknown, like auto-local names; layout depends on their hash buckets.
"""

def value(symbol):
    raw = symbol.encode('ascii')
    if len(raw) > 255:
        raise ValueError('OMF symbol exceeds one-byte name length')
    return (len(raw)+sum((ch | 0x20) << (index & 3)
                        for index, ch in enumerate(raw, 1))) & 0xffff


def bucket(symbol):
    return value(symbol) & 255
