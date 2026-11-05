#include <dos.h>
#include <fcntl.h>
#include <conio.h>
#include <memory.h>
#include <bios.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include "DISK.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CURSOR.H"
#include "FAME.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MASKBLT.H"
#include "MEM.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "PC.H"
#include "RLE.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "SYS.H"
#include "TANDYPIC.H"

#define PIC_MARK 0xbc

enum PictureLibrary {
    LIBRARY_FIRST = 1,
    LIBRARY_SECOND = 2
};

enum LoadResult {
    LOAD_NO_MEMORY,
    LOAD_CACHED,
    LOAD_READ,
    LOAD_READ_VRAM
};

static int g_initialDrive;
static struct PicLibSelection g_picLib;
static char g_dataPath[16];

struct Model g_models[26];
struct CarSpec g_carData[26];
struct CarHotspots g_bodyRects[26];
struct CarPicLayout g_layoutAutoSprArray[26];
struct StickerPos g_imageDecalLayout[8];
int g_usedCarAdFirst;
unsigned g_carRatingsTable[3][3];
struct ServicePoints g_pointsModelService[26];
unsigned g_rivalStakeToleranceTable[3][3];
int g_lib1Count;
int g_countLibBmB;
char *g_library1Path;
char *g_picLibBPath;
int g_acceptProbability[4][3];

static int near pic_index(int n);
static void near open_lib(int n);
static enum LoadResult near load_pic(int n, int load, char far *dst);
static void near unscramble_hall(void);
static void near scramble_hall(void);

static char *g_dataFileName = "hot_data";
static char *g_dataBlocks[13] = {(char *)g_racerRecordsArray,
                                 (char *)g_carData,
                                 (char *)g_models,
                                 (char *)g_layoutAutoSprArray,
                                 (char *)g_bodyRects,
                                 (char *)g_pointsModelService,
                                 g_desc,
                                 (char *)&g_usedCarAdFirst,
                                 g_txt,
                                 (char *)g_carRatingsTable,
                                 (char *)g_rivalStakeToleranceTable,
                                 (char *)g_acceptProbability,
                                 (char *)g_imageDecalLayout};
static unsigned g_dataSizes[13] = {396, 260, 260, 312, 520, 156, 1850, 2, 200, 18, 18, 24, 48};
static int g_lib1Handle = 0;
static int g_lib2Handle = 0;
static char *g_lib1Name = "?:lib1";
static char *g_lib1cName = "?:lib1c";
static char *g_lib2Name = "?:lib2";
static char *g_lib2cName = "?:lib2c";
static char *g_msgInsertDisk = "insert disk ? into drive ?";
static char *g_msgCopyFiles = "copy all files to current directory";
static char *g_msgBadFormat = "bad file format";
static char *g_msgWriteError = "write error";
static char *g_msgNotFound = "file not found: ????";
unsigned char g_rleCounts[16] = {1, 1, 2, 3, 4, 2, 3, 5, 6, 6, 4, 7, 7, 5, 8, 8};
unsigned char g_rleValues[16] = {0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 0};
int g_picNewHeader = 1;

/* Seek the DOS file handle to the absolute position pos (INT 21h function 42h, method 0).
   Returns 0 on success, else the error in _doserrno, also copied to errno */

int di_seek(int handle, long pos)
{
    union REGS in, out;

    in.x.ax = (DOS_SEEK << 8) | DOS_SEEK_START;
    in.x.bx = handle;
    in.x.cx = (int)(pos >> 16);
    in.x.dx = (int)pos;
    intdos(&in, &out);
    if (out.x.cflag == 0)
        return 0;
    return errno = _doserrno;
}

/* Map a picture number to its table index: numbers from 1000 on are shifted down, the rest
   are 1 based. In the video modes -2 and -6 that is the result, else g_cgaPicMap translates it */

static int near pic_index(int n)
{
    if (n >= PICTURE_SECOND_BASE)
        n -= PICTURE_SECOND_INDEX_BIAS;
    else
        n--;
    if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY)
        return g_cgaPicMap[n];
    else
        return n;
}

/* Make sure the picture file holding picture number n is open (the pictures come in two
   files, split at g_lib1Count). Closes the other one, asks for the right disk until the open works
   and checks the file header word */

static void near open_lib(int n)
{
    enum PictureLibrary kind;
    int tries;
    unsigned got;

    if (pic_index(n) >= g_lib1Count) {
        kind = LIBRARY_SECOND;
        g_picLib.path = g_picLibBPath;
        g_picLib.handle = &g_lib2Handle;
        g_picLib.otherHandle = &g_lib1Handle;
        g_picLib.entryCount = &g_countLibBmB;
    } else {
        kind = LIBRARY_FIRST;
        g_picLib.path = g_library1Path;
        g_picLib.handle = &g_lib1Handle;
        g_picLib.otherHandle = &g_lib2Handle;
        g_picLib.entryCount = &g_lib1Count;
    }
    if (*g_picLib.handle == 0) {
        _dos_close(*g_picLib.otherHandle);
        *g_picLib.otherHandle = 0;
        g_msgInsertDisk[12] = kind == LIBRARY_FIRST                                ? '1'
                              : (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? '3'
                                                                                   : '2';
        g_msgInsertDisk[25] = *g_picLib.path;
        tries = 0;
        while (_dos_open(g_picLib.path, O_RDONLY, g_picLib.handle)) {
            if (tries++ == 3)
                sy_exit();
            sc_wait_msg(g_msgInsertDisk);
        }
        if (_dos_read(*g_picLib.handle, (void far *)g_picLib.entryCount, 2, &got) || got != 2 ||
            g_lib1Count + g_countLibBmB != PICTURE_COUNT) {
            sc_wait_msg(g_msgBadFormat);
            sy_exit();
        }
    }
}

/* Read the picture headers of one picture file into g_slot: which 1 is the first file,
   anything else the second (its entries follow the first file's, g_lib1Count of them). Clears the
   run time fields of each entry. Returns the largest data size found */

int di_read_index(int which)
{
    int num;
    unsigned height;
    int i;
    int count;
    int got;
    int base;
    struct PictureSlot far *sl;

    base = which == 1 ? 0 : g_lib1Count;
    num = which == 1 ? 1 : PICTURE_SECOND_BASE;
    height = 0;
    open_lib(num);
    count = which == 1 ? g_lib1Count : g_countLibBmB;
    for (i = 0; i < count; i++) {
        sl = &g_slot[base + i];
        if (di_seek(*g_picLib.handle, (long)(i * 0x1e + 2)) ||
            _dos_read(*g_picLib.handle, (void far *)sl, 0x1e, &got) || got != 0x1e) {
            sc_wait_msg(g_msgBadFormat);
            sy_exit();
        }
        sl->vramPlane = NOT_IN_VRAM;
        sl->packed.p = 0;
        sl->unused4 = 0;
        sl->unused5 = 0;
        sl->unused6 = -1;
        sl->unused7 = 1;
        if (sl->packedSpan > height)
            height = sl->packedSpan;
    }
    return height;
}

/* Bring picture number n into memory. With load set the picture goes through a work
   area in the fast pool (me_vram_alloc) and is copied there after it has been read; otherwise it is read
   into dst, or into a new block when dst is 0. The file position and the disk are looked after by
   open_lib and the read is repeated after asking for the disk again when the data does not start
   and end with 0xbc. Returns 0 when there was no memory, 1 when the picture was loaded already,
   2 and 3 when it has just been read (3: copied to the fast pool) */

static enum LoadResult near load_pic(int n, int load, char far *dst)
{
    int num;
    int total;
    char far *fp;
    long offset;
    struct PictureSlot far *ent;
    int got;
    char pool;
    int retries;
    struct PictureSlot far *sl;

    n = pic_index(num = n);
    ent = sl = &g_slot[n];
    if (sl->packed.p == 0) {
        if (load) {
            fp = me_vram_alloc(ent->packedSpan, &pool);
            sl->packed.p = fp == 0 ? (char far *)0 : ar_alloc(ent->packedSpan + 1, ARENA_HIGH_STACK);
            sl->vramPlane = pool;
        } else {
            sl->packed.p = dst ? dst : me_alloc(ent->packedSpan + 1, 0);
            sl->vramPlane = NOT_IN_VRAM;
        }
        if (sl->packed.p == 0)
            return LOAD_NO_MEMORY;
        open_lib(num);
        offset = (long)(*g_picLib.entryCount * 0x1e + 2) + ent->dataOffset;
        if (di_seek(*g_picLib.handle, offset))
            goto bad;
        if (sl->vramPlane != NOT_IN_VRAM) {
            outp(SEQ_INDEX, SEQ_MAP_MASK);
            outp(SEQ_DATA, 1 << sl->vramPlane);
        }
        retries = 0;
        total = ent->packedSpan + 1;
        for (;;) {
            if (_dos_read(*g_picLib.handle, sl->packed.p, total, &got) || got != total)
                goto bad;
            if (*sl->packed.p == (char)PIC_MARK && sl->packed.p[total - 1] == (char)PIC_MARK) {
                if (sl->vramPlane == NOT_IN_VRAM)
                    goto done;
                movedata(sl->packed.w.seg, sl->packed.w.off, FP_SEG(fp), FP_OFF(fp), ent->packedSpan);
                sl->packed.p = fp;
                ma_pop_high(1);
                outp(SEQ_INDEX, SEQ_MAP_MASK);
                outp(SEQ_DATA, SEQ_ALL_PLANES);
                return LOAD_READ_VRAM;
            }
            if (g_initialDrive > 2)
                goto bad;
            do {
                if (++retries > 3)
                    sy_exit();
                g_msgInsertDisk[12] = *g_picLib.handle == g_lib1Handle                     ? 1
                                      : (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? '3'
                                                                                           : '2';
                g_msgInsertDisk[25] = *g_picLib.path;
                sc_wait_msg(g_msgInsertDisk);
                _dos_close(*g_picLib.handle);
            } while (_dos_open(g_picLib.path, O_RDONLY, g_picLib.handle));
            if (!di_seek(*g_picLib.handle, offset))
                continue;
            goto bad;
        }
    done:
        return LOAD_READ;
    } else
        return LOAD_CACHED;
bad:
    sc_wait_msg(g_msgBadFormat);
    sy_exit();
}

/* Frees and clears the far pointer in slot n of the far table g_slot. */

void di_free_pic(enum PictureId n)
{
    char far *far *pp;

    n = pic_index(n);
    if (*(pp = &g_slot[n].packed.p)) {
        me_free(*pp);
        *pp = 0;
    }
}

/* Unpack picture number id into the bitmap bm (a new one when bm is 0). The picture is
   loaded to the work area g_imageBufferPointer first; its data is RLE coded (rl_decode) and expanded for the video
   mode in use into the planes bits and alt of the bitmap. Returns the bitmap */

struct Bitmap far *di_unpack_pic(enum PictureId id, struct Bitmap far *bm)
{
    int num;
    char far *src;
    unsigned char colors16[16];
    struct PictureSlot far *cur;
    int j;
    enum LoadResult res;
    struct Bitmap far *pic;
    struct PictureSlot far *sl;

    cu_ctl(CURSOR_HIDE_NOW);
    res = load_pic(id, 0, g_imageBufferPointer);
    cur = sl = &g_slot[pic_index(id)];
    if (g_picNewHeader)
        pic = me_new_bitmap(cur->width, cur->height, cur->bitmapFlags, BITMAP_PICTURE, bm);
    else
        pic = bm;
    for (j = 0; j < 16; j++)
        colors16[j] = cur->rleTokens[j];
    if (sl->vramPlane != NOT_IN_VRAM) {
        outp(GC_INDEX, GC_READ_MAP);
        outp(GC_DATA, sl->vramPlane);
    }
    src = sl->packed.p + 1;
    num = cur->packedSpan - 1;
    if (r_vid.mode == VID_EGA) {
        if (pic->bits.p == 0) {
            rl_decode(src, pic->mask.p, num, colors16, 16);
        } else {
            rl_decode(src, pic->bits.p, num, colors16, 16);
            if (pic->mask.p)
                mk_make_mask(pic->bits.p, pic->mask.p, pic->pixelBytes);
        }
    } else if (r_vid.mode == VID_TANDY) {
        if (pic->bits.p == 0) {
            tp_unpack_mask(src, pic->mask.p, pic->pixelBytes, colors16);
        } else {
            tp_unpack(src, pic->bits.p, pic->pixelBytes, colors16);
            if (pic->mask.p)
                tp_make_mask(pic->bits.p, pic->mask.p, pic->pixelBytes);
        }
    } else {
        rl_decode(src, pic->bits.p == 0 ? pic->mask.p : pic->bits.p, num, colors16, 16);
    }
    if (sl->vramPlane != NOT_IN_VRAM) {
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, SEQ_ALL_PLANES);
    }
    if (res == LOAD_READ)
        sl->packed.p = 0;
    cu_ctl(CURSOR_SHOW_NOW);
    return pic;
}

/* Fields of slot picture number n: *w, *h and the byte *x */

void di_pic_size(enum PictureId n, int *w, int *h, int *x)
{
    n = pic_index(n);
    *w = g_slot[n].width;
    *h = g_slot[n].height;
    *x = g_slot[n].bitmapFlags;
}

/* Allocate a bitmap of the given mode for slot picture id, draw the picture into it
   (di_unpack_pic) and return it */

struct Bitmap far *di_bm(enum PictureId id, int mode)
{
    int h;
    int w1;
    int x;
    struct Bitmap far *bm;

    di_pic_size(id, &w1, &h, &x);
    bm = ar_bm(w1, h, x, mode);
    di_unpack_pic(id, bm);
    return bm;
}

/* Load every picture of the zero terminated list of picture numbers (see load_pic). Does
   nothing when g_noUeMem is set. Returns 1 */

int di_cache_list(int *list)
{
    int n;

    if (g_noUeMem)
        return 1;
    while (n = *list++)
        load_pic(n, 1, 0L);
    return 1;
}

/* Try the pictures of the zero terminated list in turn with load_pic (load flag 0) and
   count the ones that succeed. A failure ends the walk unless all is set. Returns the count */

int di_load_list(int *list, int all)
{
    int n;
    int count;

    count = 0;
    while (n = *list++) {
        if (load_pic(n, 0, 0L) == LOAD_NO_MEMORY) {
            if (!all)
                return count;
        } else
            count++;
    }
    return count;
}

/* Free the pictures listed in the zero terminated array of picture numbers */

void di_free_list(int *list)
{
    int n;

    while (n = *list++)
        di_free_pic(n);
}

/* Open the two picture files (names g_library1Path and g_picLibBPath, drive letter patched into their
   first character) and read the entry counts g_lib1Count and g_countLibBmB from their headers. disks is the
   number of disk drives wanted: 2 only when the equipment word says there are two floppies. mode 2
   and 5 pick the other file names. Any failure shows an error text */

void di_open_libs(int disks, enum VideoChoice mode)
{
    int equip;
    int tries;
    int err;

    _dos_getdrive((unsigned *)&g_initialDrive);
    if (g_initialDrive < 3 && disks > 1 && (equip = _bios_equiplist(), equip & 1) && (equip & 0xc0))
        disks = 2;
    else
        disks = 1;
    g_library1Path = (mode == VIDEO_CHOICE_EGA || mode == VIDEO_CHOICE_TANDY) ? g_lib1Name : g_lib1cName;
    *g_library1Path = g_initialDrive + 0x40;
    err = _dos_open(g_library1Path, O_RDONLY, &g_lib1Handle);
    if (err || g_lib1Handle == 0) {
        if (g_initialDrive > 2) {
            sc_wait_msg(g_msgCopyFiles);
        } else {
            strcpy(g_msgNotFound + 0x10, g_library1Path + 2);
            sc_wait_msg(g_msgNotFound);
        }
        sy_exit();
    }
    if (_dos_read(g_lib1Handle, (void far *)&g_lib1Count, 2, &err) || err != 2) {
        sc_wait_msg(g_msgBadFormat);
        sy_exit();
    }
    g_picLibBPath = (mode == VIDEO_CHOICE_EGA || mode == VIDEO_CHOICE_TANDY) ? g_lib2Name : g_lib2cName;
    *g_picLibBPath = *g_library1Path;
    if (_dos_open(g_picLibBPath, O_RDONLY, &g_lib2Handle) || g_lib2Handle == 0) {
        if (g_initialDrive > 2) {
            sc_wait_msg(g_msgCopyFiles);
            sy_exit();
        } else if (disks == 1) {
            g_lib2Handle = 0;
        } else {
            *g_picLibBPath = g_initialDrive == 1 ? 'B' : 'A';
            err = _dos_open(g_picLibBPath, O_RDONLY, &g_lib2Handle);
            for (tries = 0; err || g_lib2Handle == 0; tries++) {
                if (tries == 3)
                    sy_exit();
                g_msgInsertDisk[12] = (mode != VIDEO_CHOICE_EGA && mode != VIDEO_CHOICE_TANDY) ? '3' : '2';
                g_msgInsertDisk[0x19] = *g_picLibBPath;
                sc_wait_msg(g_msgInsertDisk);
                err = _dos_open(g_picLibBPath, O_RDONLY, &g_lib2Handle);
            }
        }
    }
    if (g_lib2Handle) {
        if (_dos_read(g_lib2Handle, (void far *)&g_countLibBmB, 2, &err) || err != 2 ||
            g_lib1Count + g_countLibBmB != PICTURE_COUNT) {
            sc_wait_msg(g_msgBadFormat);
            sy_exit();
        }
    }
}

static char g_hallFileName[12] = "?:hall_dat";

/* Load the game data. Builds the file name g_dataPath from the first two characters of
   g_picLibBPath and the text g_dataFileName points to (g_dataFileName ends up behind its terminator), opens it and
   reads the 13 blocks of the data table (buffer g_dataBlocks[i], size g_dataSizes[i]); any failure shows an
   error text. Ends by loading the hall of fame (di_load_hall) */

void di_load_data(void)
{
    int i;
    int got;
    int fd;
    char *drive;
    int j;

    drive = g_dataPath;
    *drive++ = g_picLibBPath[0];
    *drive++ = g_picLibBPath[1];
    while (*drive++ = *g_dataFileName++)
        ;
    if (_dos_open(g_dataPath, O_RDONLY, &fd)) {
        sc_wait_msg("Can't open data file");
        sy_exit();
    }
    for (i = j = 0; i < 13; i++) {
        if (!_dos_read(fd, (void far *)g_dataBlocks[i], g_dataSizes[i], &got) && got == g_dataSizes[i])
            continue;
        sc_wait_msg("Data disc read fail");
        sy_exit();
    }
    _dos_close(fd);
    di_load_hall();
}

/* Make sure the game disk is in the drive: tries to open the file g_dataPath (asking for
   disk 2 again and again) and closes it. From the third drive on (g_initialDrive > 2, a hard disk) it
   returns the drive letter code g_initialDrive + 0x40 at once. Otherwise returns the first character of
   g_dataPath */

int di_data_drive(void)
{
    int file;
    int tries;

    tries = 0;
    if (g_initialDrive > 2)
        return (char)(g_initialDrive + 0x40);
    for (;;) {
        if (_dos_open(g_dataPath, O_RDONLY, &file)) {
            if (++tries > 3)
                sy_exit();
            g_msgInsertDisk[12] = (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? '3' : '2';
            g_msgInsertDisk[25] = *g_picLibBPath;
            sc_wait_msg(g_msgInsertDisk);
        } else {
            _dos_close(file);
            open_lib(PICTURE_SECOND_BASE);
            return g_dataPath[0];
        }
    }
}

/* Set the high bit of all 230 bytes of g_hallFameTbl (10 hall of fame entries of 23 bytes) to scramble
   it before it is written to the file (see di_save_hall) */

static void near scramble_hall(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)g_hallFameTbl)[i] |= 0x80;
}

/* Clear the high bit of all 230 bytes of g_hallFameTbl again, unscrambling the hall of fame */

static void near unscramble_hall(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)g_hallFameTbl)[i] &= 0x7f;
}

/* Read the hall of fame file (name g_hallFileName, first character the drive of the game disk):
   a count word, then g_nScore entries. Any read failure shows the error text. A missing file
   leaves the table empty */

void di_load_hall(void)
{
    struct HallEntry *p;
    int i;
    int got;
    int fd;

    g_hallFileName[0] = *g_picLibBPath;
    di_data_drive();
    if (_dos_open(g_hallFileName, O_RDONLY, &fd) || fd == 0)
        g_nScore = 0;
    else {
        if (_dos_read(fd, (void far *)&g_nScore, 2, &got) || got != 2) {
            sc_wait_msg(g_msgBadFormat);
            sy_exit();
        }
        for (i = 0, p = g_hallFameTbl; i < g_nScore; p++, i++) {
            if (_dos_read(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p) {
                sc_wait_msg(g_msgBadFormat);
                sy_exit();
            }
        }
        _dos_close(fd);
        unscramble_hall();
    }
}

/* Write the hall of fame file (g_hallFileName): the table is scrambled (scramble_hall), the file made
   (asking for the disk until it works on a floppy), the count word and the g_nScore entries
   written. Any failure shows the error text and deletes the file. The table is unscrambled at
   the end (unscramble_hall) */

void di_save_hall(void)
{
    struct HallEntry *p;
    int tries;
    int i;
    unsigned got;
    int fd;

    scramble_hall();
    di_data_drive();
    got = _dos_creat(g_hallFileName, _A_NORMAL, &fd);
    if (g_initialDrive > 2) {
        if (fd == 0 || got) {
            sc_wait_msg(g_msgWriteError);
            sy_exit();
        }
    } else {
        g_msgInsertDisk[12] = '1';
        g_msgInsertDisk[25] = g_hallFileName[0];
        for (tries = 0; fd == 0 || got; tries++) {
            if (tries == 3)
                sy_exit();
            sc_wait_msg(g_msgBadFormat);
            got = _dos_creat(g_hallFileName, _A_NORMAL, &fd);
        }
    }
    if (_dos_write(fd, (void far *)&g_nScore, 2, &got) || got != 2) {
    bad:
        sc_wait_msg(g_msgWriteError);
        unlink(g_hallFileName);
        sc_wait_msg(g_msgWriteError);
    } else {
        for (i = 0, p = g_hallFameTbl; i < g_nScore; p++, i++)
            if (_dos_write(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p)
                goto bad;
        if (_dos_close(fd))
            goto bad;
    }
    unscramble_hall();
}
