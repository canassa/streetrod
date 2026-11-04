#include <dos.h>
#include <conio.h>
#include <memory.h>
#include <bios.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "DISK.H"
#include "ARENA.H"
#include "CURSOR.H"
#include "FAME.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MASKBLT.H"
#include "MEM.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "RLE.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "SYS.H"
#include "TANDYPIC.H"

#pragma intrinsic(outp)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_6c68;                         /* DS:6C68 */
static int *g_6c6a;                        /* DS:6C6A */
static int *g_6c6c;                        /* DS:6C6C */
static char *g_6c6e;                       /* DS:6C6E */
static int *g_6c70a;                       /* DS:6C70 */
static char g_6c72a[16];                   /* DS:6C72 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Model w_8df0[26];
struct Racer av7ff8[22];
struct Spec g7d86n[26];
struct Panel ai70e4t[26];
struct Spr12 g7682n[26];
struct Pos6 a784als[8];
int a_7d16q;
unsigned g7d68m[3][3];
struct Spr6 ap818c[26];
unsigned ag8baey[3][3];
int h_8bc8;
int s_8dd2;
char *aw8dd4x;
char *au8dd6y;
int h_8dd8[4][3];

static int near pic_index(int n);
static void near open_lib(int n);
static int near load_pic(int n, int load, char far *dst);
static void near unscramble_hall(void);
static void near scramble_hall(void);

char *g_4ea2 = "hot_data";   /* DS:4EA2 */
char *g_4ea4[13] = {(char *)av7ff8, (char *)g7d86n, (char *)w_8df0, (char *)g7682n, (char *)ai70e4t, (char *)ap818c, g_2c02, (char *)&a_7d16q, g_239e, (char *)g7d68m, (char *)ag8baey, (char *)h_8dd8, (char *)a784als};   /* DS:4EA4 */
unsigned g_4ebe[13] = {396, 260, 260, 312, 520, 156, 1850, 2, 200, 18, 18, 24, 48};   /* DS:4EBE */
int g_4ed8 = 0;   /* DS:4ED8 */
int g_4eda = 0;   /* DS:4EDA */
char *g_4edc = "?:lib1";   /* DS:4EDC */
char *g_4ede = "?:lib1c";   /* DS:4EDE */
char *g_4ee0 = "?:lib2";   /* DS:4EE0 */
char *g_4ee2 = "?:lib2c";   /* DS:4EE2 */
char *g_4ee4 = "insert disk ? into drive ?";   /* DS:4EE4 */
char *g_4ee6 = "copy all files to current directory";   /* DS:4EE6 */
char *g_4ee8 = "bad file format";   /* DS:4EE8 */
char *g_4eea = "write error";   /* DS:4EEA */
char *g_4eec = "file not found: ????";   /* DS:4EEC */
unsigned char g_4eee[16] = {1, 1, 2, 3, 4, 2, 3, 5, 6, 6, 4, 7, 7, 5, 8, 8};   /* DS:4EEE */
unsigned char g_4efe[16] = {0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 0};   /* DS:4EFE */
int g_4f0e = 1;   /* DS:4F0E */

/* disk_seek @ 0F38:6016 (linear 0x15396, 65 bytes)

   Seek the DOS file handle to the absolute position pos (INT 21h function 42h, method 0).
   Returns 0 on success, else the error in _doserrno, also copied to errno */

int disk_seek(int handle, long pos)
{
    union REGS in, out;

    in.x.ax = 0x4200;
    in.x.bx = handle;
    in.x.cx = (int)(pos >> 16);
    in.x.dx = (int)pos;
    intdos(&in, &out);
    if (out.x.cflag == 0)
        return 0;
    return errno = _doserrno;
}

/* pic_index @ 0F38:6057 (linear 0x153D7, 50 bytes)

   Map a picture number to its table index: numbers from 1000 on are shifted down, the rest
   are 1 based. In the video modes -2 and -6 that is the result, else g_07c6 translates it */

static int near pic_index(int n)
{
    if (n >= 1000)
        n -= 0x3df;
    else
        n--;
    if (r_vid.mode != -2 && r_vid.mode != -6)
        return g_07c6[n];
    else
        return n;
}

/* open_lib @ 0F38:6089 (linear 0x15409, 290 bytes)

   Make sure the picture file holding picture number n is open (the pictures come in two
   files, split at h_8bc8). Closes the other one, asks for the right disk until the open works
   and checks the file header word */

static void near open_lib(int n)
{
    int kind;
    int tries;
    unsigned got;

    if (pic_index(n) >= h_8bc8) {
        kind = 2;
        g_6c6e = au8dd6y;
        g_6c6c = &g_4eda;
        g_6c70a = &g_4ed8;
        g_6c6a = &s_8dd2;
    } else {
        kind = 1;
        g_6c6e = aw8dd4x;
        g_6c6c = &g_4ed8;
        g_6c70a = &g_4eda;
        g_6c6a = &h_8bc8;
    }
    if (*g_6c6c == 0) {
        _dos_close(*g_6c70a);
        *g_6c70a = 0;
        g_4ee4[12] = kind == 1 ? '1' : (r_vid.mode != -2 && r_vid.mode != -6) ? '3' : '2';
        g_4ee4[25] = *g_6c6e;
        tries = 0;
        while (_dos_open(g_6c6e, 0, g_6c6c)) {
            if (tries++ == 3)
                sys_exit();
            screen_wait_msg(g_4ee4);
        }
        if (_dos_read(*g_6c6c, (void far *)g_6c6a, 2, &got) || got != 2 || h_8bc8 + s_8dd2 != 0x115) {
            screen_wait_msg(g_4ee8);
            sys_exit();
        }
    }
}

/* disk_read_index @ 0F38:61AB (linear 0x1552B, 271 bytes)

   Read the picture headers of one picture file into g_slot: which 1 is the first file,
   anything else the second (its entries follow the first file's, h_8bc8 of them). Clears the
   run time fields of each entry. Returns the largest data size found */

int disk_read_index(int which)
{
    int num;
    unsigned height;
    int i;
    int count;
    int got;
    int base;
    struct Slot far *sl;

    base = which == 1 ? 0 : h_8bc8;
    num = which == 1 ? 1 : 1000;
    height = 0;
    open_lib(num);
    count = which == 1 ? h_8bc8 : s_8dd2;
    for (i = 0; i < count; i++) {
        sl = &g_slot[base + i];
        if (disk_seek(*g_6c6c, (long)(i * 0x1e + 2)) ||
            _dos_read(*g_6c6c, (void far *)sl, 0x1e, &got) || got != 0x1e) {
            screen_wait_msg(g_4ee8);
            sys_exit();
        }
        sl->f1e = 0xff;
        sl->ptr.p = 0;
        sl->f24 = 0;
        sl->f28 = 0;
        sl->f2c = -1;
        sl->f2e = 1;
        if (sl->f1c > height)
            height = sl->f1c;
    }
    return height;
}

/* load_pic @ 0F38:62BA (linear 0x1563A, 676 bytes)

   Bring picture number n into memory. With load set the picture goes through a work
   area in the fast pool (mem_vram_alloc) and is copied there after it has been read; otherwise it is read
   into dst, or into a new block when dst is 0. The file position and the disk are looked after by
   open_lib and the read is repeated after asking for the disk again when the data does not start
   and end with 0xbc. Returns 0 when there was no memory, 1 when the picture was loaded already,
   2 and 3 when it has just been read (3: copied to the fast pool) */

static int near load_pic(int n, int load, char far *dst)
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

    n = pic_index(num = n);
    ent = sl = &g_slot[n];
    if (sl->ptr.p == 0) {
        if (load) {
            fp = mem_vram_alloc(ent->f1c, &pool);
            sl->ptr.p = fp == 0 ? (char far *)0 : arena_alloc(ent->f1c + 1, 2);
            sl->f1e = pool;
        } else {
            sl->ptr.p = dst ? dst : mem_alloc(ent->f1c + 1, 0);
            sl->f1e = 0xff;
        }
        if (sl->ptr.p == 0)
            return 0;
        open_lib(num);
        offset = (long)(*g_6c6a * 0x1e + 2) + ent->f18;
        if (disk_seek(*g_6c6c, offset))
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
                    sys_exit();
                g_4ee4[12] = *g_6c6c == g_4ed8 ? 1 : (r_vid.mode != -2 && r_vid.mode != -6) ? '3' : '2';
                g_4ee4[25] = *g_6c6e;
                screen_wait_msg(g_4ee4);
                _dos_close(*g_6c6c);
            } while (_dos_open(g_6c6e, 0, g_6c6c));
            if (!disk_seek(*g_6c6c, offset))
                continue;
            goto bad;
        }
    done:
        return 2;
    } else
        return 1;
bad:
    screen_wait_msg(g_4ee8);
    sys_exit();
}

/* disk_free_pic @ 0F38:655E (linear 0x158DE, 80 bytes)

   Frees and clears the far pointer in slot n of the far table g_slot. */

void disk_free_pic(int n)
{
    char far * far *pp;

    n = pic_index(n);
    if (*(pp = &g_slot[n].ptr.p)) {
        mem_free(*pp);
        *pp = 0;
    }
}

/* disk_unpack_pic @ 0F38:65AE (linear 0x1592E, 593 bytes)

   Unpack picture number id into the bitmap bm (a new one when bm is 0). The picture is
   loaded to the work area ab7564z first; its data is RLE coded (rle_decode) and expanded for the video
   mode in use into the planes bits and alt of the bitmap. Returns the bitmap */

struct Bitmap far *disk_unpack_pic(int id, struct Bitmap far *bm)
{
    int num;
    char far *src;
    unsigned char colors16[16];
    struct Slot far *cur;
    int j;
    int res;
    struct Bitmap far *pic;
    struct Slot far *sl;

    F_120FE(-4);
    res = load_pic(id, 0, ab7564z);
    cur = sl = &g_slot[pic_index(id)];
    if (g_4f0e)
        pic = mem_new_bitmap(cur->f0, cur->f2, cur->f6, 1, bm);
    else
        pic = bm;
    for (j = 0; j < 16; j++)
        colors16[j] = cur->pal[j];
    if (sl->f1e != 0xff) {
        outp(0x3ce, 4);
        outp(0x3cf, sl->f1e);
    }
    src = sl->ptr.p + 1;
    num = cur->f1c - 1;
    if (r_vid.mode == -2) {
        if (pic->bits.p == 0) {
            rle_decode(src, pic->alt.p, num, colors16, 16);
        } else {
            rle_decode(src, pic->bits.p, num, colors16, 16);
            if (pic->alt.p)
                maskblt_make_mask(pic->bits.p, pic->alt.p, pic->planesize);
        }
    } else if (r_vid.mode == -6) {
        if (pic->bits.p == 0) {
            tandypic_unpack_mask(src, pic->alt.p, pic->planesize, colors16);
        } else {
            tandypic_unpack(src, pic->bits.p, pic->planesize, colors16);
            if (pic->alt.p)
                tandypic_make_mask(pic->bits.p, pic->alt.p, pic->planesize);
        }
    } else {
        rle_decode(src, pic->bits.p == 0 ? pic->alt.p : pic->bits.p, num, colors16, 16);
    }
    if (sl->f1e != 0xff) {
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
    }
    if (res == 2)
        sl->ptr.p = 0;
    F_120FE(-2);
    return pic;
}

/* disk_pic_size @ 0F38:67FF (linear 0x15B7F, 63 bytes)

   Fields of slot picture number n: *w, *h and the byte *x */

void disk_pic_size(int n, int *w, int *h, int *x)
{
    n = pic_index(n);
    *w = g_slot[n].f0;
    *h = g_slot[n].f2;
    *x = g_slot[n].f6;
}

/* F_15BBE @ 0F38:683E (linear 0x15BBE, 73 bytes)

   Allocate a bitmap of the given mode for slot picture id, draw the picture into it
   (disk_unpack_pic) and return it */

struct Bitmap far *F_15BBE(int id, int mode)
{
    int h;
    int w1;
    int x;
    struct Bitmap far *bm;

    disk_pic_size(id, &w1, &h, &x);
    bm = F_1934E(w1, h, x, mode);
    disk_unpack_pic(id, bm);
    return bm;
}

/* disk_cache_list @ 0F38:6887 (linear 0x15C07, 67 bytes)

   Load every picture of the zero terminated list of picture numbers (see load_pic). Does
   nothing when g_0280 is set. Returns 1 */

int disk_cache_list(int *list)
{
    int n;

    if (g_0280)
        return 1;
    while (n = *list++)
        load_pic(n, 1, 0L);
    return 1;
}

/* disk_load_list @ 0F38:68CA (linear 0x15C4A, 75 bytes)

   Try the pictures of the zero terminated list in turn with load_pic (load flag 0) and
   count the ones that succeed. A failure ends the walk unless all is set. Returns the count */

int disk_load_list(int *list, int all)
{
    int n;
    int count;

    count = 0;
    while (n = *list++) {
        if (!load_pic(n, 0, 0L)) {
            if (!all)
                return count;
        } else
            count++;
    }
    return count;
}

/* disk_free_list @ 0F38:6915 (linear 0x15C95, 46 bytes)

   Free the pictures listed in the zero terminated array of picture numbers */

void disk_free_list(int *list)
{
    int n;

    while (n = *list++)
        disk_free_pic(n);
}

/* disk_open_libs @ 0F38:6943 (linear 0x15CC3, 566 bytes)

   Open the two picture files (names aw8dd4x and au8dd6y, drive letter patched into their
   first character) and read the entry counts h_8bc8 and s_8dd2 from their headers. disks is the
   number of disk drives wanted: 2 only when the equipment word says there are two floppies. mode 2
   and 5 pick the other file names. Any failure shows an error text */

void disk_open_libs(int disks, int mode)
{
    int equip;
    int tries;
    int err;

    _dos_getdrive((unsigned *)&g_6c68);
    if (g_6c68 < 3 && disks > 1 && (equip = _bios_equiplist(), equip & 1) && (equip & 0xc0))
        disks = 2;
    else
        disks = 1;
    aw8dd4x = (mode == 2 || mode == 5) ? g_4edc : g_4ede;
    *aw8dd4x = g_6c68 + 0x40;
    err = _dos_open(aw8dd4x, 0, &g_4ed8);
    if (err || g_4ed8 == 0) {
        if (g_6c68 > 2) {
            screen_wait_msg(g_4ee6);
        } else {
            strcpy(g_4eec + 0x10, aw8dd4x + 2);
            screen_wait_msg(g_4eec);
        }
        sys_exit();
    }
    if (_dos_read(g_4ed8, (void far *)&h_8bc8, 2, &err) || err != 2) {
        screen_wait_msg(g_4ee8);
        sys_exit();
    }
    au8dd6y = (mode == 2 || mode == 5) ? g_4ee0 : g_4ee2;
    *au8dd6y = *aw8dd4x;
    if (_dos_open(au8dd6y, 0, &g_4eda) || g_4eda == 0) {
        if (g_6c68 > 2) {
            screen_wait_msg(g_4ee6);
            sys_exit();
        } else if (disks == 1) {
            g_4eda = 0;
        } else {
            *au8dd6y = g_6c68 == 1 ? 'B' : 'A';
            err = _dos_open(au8dd6y, 0, &g_4eda);
            for (tries = 0; err || g_4eda == 0; tries++) {
                if (tries == 3)
                    sys_exit();
                g_4ee4[12] = (mode != 2 && mode != 5) ? '3' : '2';
                g_4ee4[0x19] = *au8dd6y;
                screen_wait_msg(g_4ee4);
                err = _dos_open(au8dd6y, 0, &g_4eda);
            }
        }
    }
    if (g_4eda) {
        if (_dos_read(g_4eda, (void far *)&s_8dd2, 2, &err) || err != 2 || h_8bc8 + s_8dd2 != 0x115) {
            screen_wait_msg(g_4ee8);
            sys_exit();
        }
    }
}

char g_4f3a[12] = "?:hall_dat";   /* DS:4F3A */

/* disk_load_data @ 0F38:6B79 (linear 0x15EF9, 196 bytes)

   Load the game data. Builds the file name g_6c72a from the first two characters of
   au8dd6y and the text g_4ea2 points to (g_4ea2 ends up behind its terminator), opens it and
   reads the 13 blocks of the data table (buffer g_4ea4[i], size g_4ebe[i]); any failure shows an
   error text. Ends by loading the hall of fame (disk_load_hall) */

void disk_load_data(void)
{
    int i;
    int got;
    int fd;
    char *drive;
    int j;

    drive = g_6c72a;
    *drive++ = au8dd6y[0];
    *drive++ = au8dd6y[1];
    while (*drive++ = *g_4ea2++)
        ;
    if (_dos_open(g_6c72a, 0, &fd)) {
        screen_wait_msg("Can't open data file");
        sys_exit();
    }
    for (i = j = 0; i < 13; i++) {
        if (!_dos_read(fd, (void far *)g_4ea4[i], g_4ebe[i], &got) && got == g_4ebe[i])
            continue;
        screen_wait_msg("Data disc read fail");
        sys_exit();
    }
    _dos_close(fd);
    disk_load_hall();
}

/* disk_data_drive @ 0F38:6C3D (linear 0x15FBD, 146 bytes)

   Make sure the game disk is in the drive: tries to open the file g_6c72a (asking for
   disk 2 again and again) and closes it. From the third drive on (g_6c68 > 2, a hard disk) it
   returns the drive letter code g_6c68 + 0x40 at once. Otherwise returns the first character of
   g_6c72a */

int disk_data_drive(void)
{
    int file;
    int tries;

    tries = 0;
    if (g_6c68 > 2)
        return (char)(g_6c68 + 0x40);
    for (;;) {
        if (_dos_open(g_6c72a, 0, &file)) {
            if (++tries > 3)
                sys_exit();
            g_4ee4[12] = (r_vid.mode != -2 && r_vid.mode != -6) ? '3' : '2';
            g_4ee4[25] = *au8dd6y;
            screen_wait_msg(g_4ee4);
        } else {
            _dos_close(file);
            open_lib(1000);
            return g_6c72a[0];
        }
    }
}

/* scramble_hall @ 0F38:6CCF (linear 0x1604F, 29 bytes)

   Set the high bit of all 230 bytes of aytablex (10 hall of fame entries of 23 bytes) to scramble
   it before it is written to the file (see disk_save_hall) */

static void near scramble_hall(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)aytablex)[i] |= 0x80;
}

/* unscramble_hall @ 0F38:6CEC (linear 0x1606C, 29 bytes)

   Clear the high bit of all 230 bytes of aytablex again, unscrambling the hall of fame */

static void near unscramble_hall(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)aytablex)[i] &= 0x7f;
}

/* disk_load_hall @ 0F38:6D09 (linear 0x16089, 201 bytes)

   Read the hall of fame file (name g_4f3a, first character the drive of the game disk):
   a count word, then bucount entries. Any read failure shows the error text. A missing file
   leaves the table empty */

void disk_load_hall(void)
{
    struct Entry *p;
    int i;
    int got;
    int fd;

    g_4f3a[0] = *au8dd6y;
    disk_data_drive();
    if (_dos_open(g_4f3a, 0, &fd) || fd == 0)
        bucount = 0;
    else {
        if (_dos_read(fd, (void far *)&bucount, 2, &got) || got != 2) {
            screen_wait_msg(g_4ee8);
            sys_exit();
        }
        for (i = 0, p = aytablex; i < bucount; p++, i++) {
            if (_dos_read(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p) {
                screen_wait_msg(g_4ee8);
                sys_exit();
            }
        }
        _dos_close(fd);
        unscramble_hall();
    }
}

/* disk_save_hall @ 0F38:6DD2 (linear 0x16152, 312 bytes)

   Write the hall of fame file (g_4f3a): the table is scrambled (scramble_hall), the file made
   (asking for the disk until it works on a floppy), the count word and the bucount entries
   written. Any failure shows the error text and deletes the file. The table is unscrambled at
   the end (unscramble_hall) */

void disk_save_hall(void)
{
    struct Entry *p;
    int tries;
    int i;
    unsigned got;
    int fd;

    scramble_hall();
    disk_data_drive();
    got = _dos_creat(g_4f3a, 0, &fd);
    if (g_6c68 > 2) {
        if (fd == 0 || got) {
            screen_wait_msg(g_4eea);
            sys_exit();
        }
    } else {
        g_4ee4[12] = '1';
        g_4ee4[25] = g_4f3a[0];
        for (tries = 0; fd == 0 || got; tries++) {
            if (tries == 3)
                sys_exit();
            screen_wait_msg(g_4ee8);
            got = _dos_creat(g_4f3a, 0, &fd);
        }
    }
    if (_dos_write(fd, (void far *)&bucount, 2, &got) || got != 2) {
bad:
        screen_wait_msg(g_4eea);
        unlink(g_4f3a);
        screen_wait_msg(g_4eea);
    } else {
        for (i = 0, p = aytablex; i < bucount; p++, i++)
            if (_dos_write(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p)
                goto bad;
        if (_dos_close(fd))
            goto bad;
    }
    unscramble_hall();
}
