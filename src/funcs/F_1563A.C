/* F_1563A: bring picture number n into memory. With load set the picture goes through a work
   area in the fast pool (F_03967) and is copied there after it has been read; otherwise it is read
   into dst, or into a new block when dst is 0. The file position and the disk are looked after by
   F_15409 and the read is repeated after asking for the disk again when the data does not start
   and end with 0xbc. Returns 0 when there was no memory, 1 when the picture was loaded already,
   2 and 3 when it has just been read (3: copied to the fast pool) */

#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

static void near F_15409(int n);
static int near F_153D7(int n);

static int near F_1563A(int n, int load, char far *dst)
{
    int num;
    int total;
    char far *fp;
    long offset;
    struct Slot far *ent;
    int got;
    char pool;
    int retries;
    struct Slot far *sl;

    n = F_153D7(num = n);
    ent = sl = &g_slots[n];
    if (sl->ptr.p == 0) {
        if (load) {
            fp = F_03967(ent->f1c, &pool);
            sl->ptr.p = fp == 0 ? (char far *)0 : F_192AC(ent->f1c + 1, 2);
            sl->f1e = pool;
        } else {
            sl->ptr.p = dst ? dst : F_03709(ent->f1c + 1, 0);
            sl->f1e = 0xff;
        }
        if (sl->ptr.p == 0)
            return 0;
        F_15409(num);
        offset = (long)(*g_6c6a * 0x1e + 2) + ent->f18;
        if (F_15396(*g_6c6c, offset))
            goto bad;
        if (sl->f1e != 0xff) {
            outp(0x3c4, 2);
            outp(0x3c5, 1 << sl->f1e);
        }
        retries = 0;
        total = ent->f1c + 1;
        for (;;) {
            if (_dos_read(*g_6c6c, sl->ptr.p, total, &got) || got != total)
                goto bad;
            if (*sl->ptr.p == (char)0xbc && sl->ptr.p[total - 1] == (char)0xbc) {
                if (sl->f1e == 0xff)
                    goto done;
                movedata(sl->ptr.w.seg, sl->ptr.w.off, FP_SEG(fp), FP_OFF(fp), ent->f1c);
                sl->ptr.p = fp;
                F_19438(1);
                outp(0x3c4, 2);
                outp(0x3c5, 0xf);
                return 3;
            }
            if (g_6c68 > 2)
                goto bad;
            do {
                if (++retries > 3)
                    F_00FEA();
                g_4ee4[12] = *g_6c6c == g_4ed8 ? 1 : (g_vid.mode != -2 && g_vid.mode != -6) ? '3' : '2';
                g_4ee4[25] = *g_6c6e;
                F_1174E(g_4ee4);
                _dos_close(*g_6c6c);
            } while (_dos_open(g_6c6e, 0, g_6c6c));
            if (!F_15396(*g_6c6c, offset))
                continue;
            goto bad;
        }
    done:
        return 2;
    } else
        return 1;
bad:
    F_1174E(g_4ee8);
    F_00FEA();
}
