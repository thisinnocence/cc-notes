#!/usr/bin/env python3
"""Tiny ELF64: print first PT_LOAD bytes as hex (single main)."""
from struct import unpack, calcsize
import sys
from typing import Optional, Tuple

def main(path: str) -> None:
    EH = '<16sHHIQQQIHHHHHH'
    PH = '<IIQQQQQQ'
    PT_LOAD = 1  # 
    with open(path, 'rb') as f:
        eh_sz = calcsize(EH) # eh_sz = ELF header size
        ph_sz = calcsize(PH) # ph_sz = Program header size
        hdr = f.read(eh_sz)  # ELF program-header type load
        if len(hdr) != eh_sz or hdr[:4] != b'\x7fELF':
            print('Not ELF64'); return

        e = unpack(EH, hdr)
        # e[5] → e_phoff：program header table 在文件中的偏移（文件内字节偏移，整数）。
        # e[9] → e_phentsize：program header table 中每个条目的大小（字节数，整数）。
        # e[10] → e_phnum：program header table 中条目的数量（整数）。
        e_phoff, e_phentsize, e_phnum = e[5], e[9], e[10]

        f.seek(e_phoff)
        target: Optional[Tuple[int, int, int]] = None
        for _ in range(e_phnum):
            ph = f.read(e_phentsize or ph_sz)
            if len(ph) < ph_sz: break
            p = unpack(PH, ph[:ph_sz])
            if p[0] == PT_LOAD:
                target = (p[2], p[5], p[3]); break

        if not target:
            print('No PT_LOAD'); return

        off, size, vaddr = target
        f.seek(off)
        d = f.read(size)
        print(f'PT_LOAD @0x{off:x} v=0x{vaddr:x} size={size}')
        for i in range(0, len(d), 16):
            print(' '.join(f'{x:02x}' for x in d[i:i+16]))

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: parse_elf.py <elf>'); sys.exit(1)
    main(sys.argv[1])
